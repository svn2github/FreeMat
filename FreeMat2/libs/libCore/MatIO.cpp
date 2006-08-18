#include "MatIO.hpp"
#include "Exception.hpp"
#include "Print.hpp"

// A class to read/write Matlab MAT files.  Implemented based on the Matlab 7.1
// version of the file matfile_format.pdf from the Mathworks web site.

enum MatTypes {
  miINT8 = 1,
  miUINT8 = 2,
  miINT16 = 3,
  miUINT16 = 4,
  miINT32 = 5,
  miUINT32 = 6,
  miSINGLE = 7,
  miDOUBLE = 9,
  miINT64 = 12,
  miUINT64 = 13,
  miMATRIX = 14,
  miCOMPRESSED = 15,
  miUTF8 = 16,
  miUTF16 = 17,
  miUTF32 = 18
};

const uint8 complexFlag = 8;
const uint8 globalFlag = 4;
const uint8 logicalFlag = 2;

uint32 UpperWord(uint32 x) {
  return x >> 16;
}

uint32 LowerWord(uint32 x) {
  return x & 0xffff;
}

uint8 ByteOne(uint32 x) {
  return (x & 0xff);
}

uint8 ByteTwo(uint32 x) {
  return ((x & 0xff00) >> 8);
}

uint8 ByteThree(int32 x) {
  return ((x & 0xff0000) >> 16);
}

uint8 ByteFour(uint32 x) {
  return ((x & 0xff000000) >> 24);
}

Array SynthesizeNumeric(MatTypes arrayType, bool isComplex, 
			Array dims, Array pr, Array pi) {
  // Depending on the type of arrayType, we may need to adjust
  // the types of the data vectors;
  switch (arrayType) {
  case miINT8:
    pr.promoteType(FM_INT8);
    pi.promoteType(FM_INT8);
    break;
  case miUTF8:
  case miUINT8:
    pr.promoteType(FM_UINT8);
    pi.promoteType(FM_UINT8);
    break;
  case miINT16:
    pr.promoteType(FM_INT16);
    pi.promoteType(FM_INT16);
    break;
  case miUTF16:
  case miUINT16:
    pr.promoteType(FM_UINT16);
    pi.promoteType(FM_UINT16);
    break;
  case miINT32:
    pr.promoteType(FM_INT32);
    pi.promoteType(FM_INT32);
    break;
  case miUTF32:
  case miUINT32:
    pr.promoteType(FM_UINT32);
    pi.promoteType(FM_UINT32);
    break;
  case miSINGLE:
    pr.promoteType(FM_FLOAT);
    pi.promoteType(FM_FLOAT);
    break;
  case miDOUBLE:
    pr.promoteType(FM_DOUBLE);
    pi.promoteType(FM_DOUBLE);
    break;
  case miINT64:
  case miUINT64:
    pr.promoteType(FM_INT64);
    pi.promoteType(FM_UINT64);
    break;
  }
  if (!isComplex) {
    if (dims.getLength() > maxDims)
      throw Exception(string("MAT Variable has more dimensions than maxDims (currently set to ") + 
		      maxDims + ")."); // FIXME - more graceful ways to do this
    Dimensions dm;
    for (int i=0;i<dims.getLength();i++)
      dm[i] = ((const int32*) dims.getDataPointer())[i];
    pr.reshape(dm);
    return pr;
  }
  return pr;
}

uint16 MatIO::readUint16() {
  uint16 x;
  fread(&x,sizeof(uint16),1,m_fp);
  if (!m_endianSwap) return x;
  return (ByteOne(x) << 8) | (ByteTwo(x));
}

uint32 MatIO::readUint32() {
  uint32 x;
  fread(&x,sizeof(uint32),1,m_fp);
  if (!m_endianSwap) return x;
  return ((ByteOne(x) << 24) |
	  (ByteTwo(x) << 16) |
	  (ByteThree(x) << 8) | 
	  ByteFour(x));
}

template <class T>
Array readRawArray(Class cls, uint32 len, FILE *m_fp) {
  T* dp = (T*) Array::allocateArray(cls,len);
  fread(dp,sizeof(T),len,m_fp);
  fseek(m_fp,(8-(ftell(m_fp)&0x7)) % 8,SEEK_CUR);
  return Array(cls,Dimensions(len,1),dp);
}

Array MatIO::getAtom() {
  uint32 tag1 = readUint32();
  uint32 DataType, ByteCount;
  // Is the upper word of tag1 zero?
  if (UpperWord(tag1) == 0) {
    // Yes, then tag1 is the datatype
    // and tag2 is the byte count
    DataType = tag1;
    ByteCount = readUint32();
  } else {
    // No, then the upper word of tag1 is the byte count
    // and the lower word of tag1 is the data type
    DataType = UpperWord(tag1);
    ByteCount = LowerWord(tag1);
  }
  // Is it a compression flag?
  //   if (DataType == miCOMPRESSED) {
  //     DecompressBytes(ByteCount);
  //     return getAtom();
  //   } 
  if (DataType == miMATRIX) {
    Array aFlags(getAtom());
    if (aFlags.getDataClass() != FM_UINT32)
      throw Exception("Corrupted MAT file - array flags");
    if (aFlags.getLength() != 2)
      throw Exception("Corrupted MAT file - array flags");
    const uint32 *dp = (const uint32 *) aFlags.getDataPointer();
    uint8 arrayType = ByteOne(dp[0]);
    uint8 arrayFlags = ByteTwo(dp[0]);
    bool isComplex = (arrayType & complexFlag) != 0;
    bool isGlobal = (arrayType & globalFlag) != 0;
    bool isLogical = (arrayType & globalFlag) != 0;
    Array dims(getAtom());
    if (dims.getDataClass() != FM_INT32)
      throw Exception("Corrupted MAT file - dimensions array");
    Array name(getAtom());
    name.promoteType(FM_INT8); // Problems with UNICODE names for variables?
    if (name.getDataClass() != FM_INT8)
      throw Exception("Corrupted MAT file - array name");
    name.promoteType(FM_STRING);
    cout << " Found array: " << ArrayToString(name) << "\r\n";
    Array pr(getAtom());
    Array pi;
    if (isComplex)
      pi = getAtom();
    return SynthesizeNumeric((MatTypes) arrayType,isComplex,dims,pr,pi);
  }
  if (DataType == miINT8) 
    return readRawArray<int8>(FM_INT8,ByteCount,m_fp);
  if (DataType == miUINT8)
    return readRawArray<uint8>(FM_UINT8,ByteCount,m_fp);
  if (DataType == miINT16) 
    return readRawArray<int16>(FM_INT16,ByteCount>>1,m_fp);
  if (DataType == miUINT16)
    return readRawArray<uint16>(FM_UINT16,ByteCount>>1,m_fp);
  if (DataType == miINT32) 
    return readRawArray<int32>(FM_INT32,ByteCount>>2,m_fp);
  if (DataType == miUINT32)
    return readRawArray<uint32>(FM_UINT32,ByteCount>>2,m_fp);
  if (DataType == miSINGLE)
    return readRawArray<float>(FM_FLOAT,ByteCount>>2,m_fp);
  if (DataType == miDOUBLE)
    return readRawArray<double>(FM_FLOAT,ByteCount>>3,m_fp);
}

MatIO::MatIO(string filename, openMode mode) :
  m_filename(filename), m_mode(mode) {
  if (m_mode == writeMode) {
    m_fp = fopen(filename.c_str(),"wb");
    if (!m_fp)
      throw Exception("Unable to open file " + filename + " for writing");
  } else {
    m_fp = fopen(filename.c_str(),"rb");
    if (!m_fp)
      throw Exception("Unable to open file " + filename + " for reading");
  }
  m_endianSwap = false;
}

string MatIO::getHeader() {
  // Read the header...
  char hdrtxt[124];
  fread(hdrtxt,1,124,m_fp);
  if (readUint16() != 0x100) throw Exception("Not a valid MAT file");
  uint16 byteOrder = readUint16();
  if (byteOrder == ('M' << 8 | 'I'))
    m_endianSwap = false;
  else if (byteOrder == ('I' << 8 | 'M'))
    m_endianSwap = true;
  else 
    throw Exception("Unable to determine the byte order of the MAT file");
  return string(hdrtxt);
}

void MatIO::putHeader(string hdr) {
  char hdrtxt[124];
  memset(hdrtxt,0,124);
  memcpy(hdrtxt,hdr.c_str(),hdr.size());
  //   writeBytesRaw(hdrtxt,124);
  //   writeUint16(0x100);
  //   writeUint16('M' << 8 | 'I');
}

MatIO::~MatIO() {
  fclose(m_fp);
}

ArrayVector MatLoadFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) throw Exception("Need filename");
  MatIO m(ArrayToString(arg[0]),MatIO::readMode);
  cout << m.getHeader() << "\r\n";
  cout.flush();
  while(1) {
    Array a(m.getAtom());
    PrintArrayClassic(a,1000,eval,true);
  }
  return ArrayVector();
}
