#include "MatIO.hpp"
#include "Sparse.hpp"
#include "Exception.hpp"
#include "Print.hpp"
#include "Malloc.hpp"
#include "MemPtr.hpp"
#include "Array.hpp"
#include <zlib.h>

extern void SwapBuffer(char* cp, int count, int elsize);

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
// Sparse yet to do..

enum mxArrayTypes {
  mxCELL_CLASS = 1,
  mxSTRUCT_CLASS = 2,
  mxOBJECT_CLASS = 3,
  mxCHAR_CLASS = 4,
  mxSPARSE_CLASS = 5,
  mxDOUBLE_CLASS = 6,
  mxSINGLE_CLASS = 7,
  mxINT8_CLASS = 8,
  mxUINT8_CLASS = 9,
  mxINT16_CLASS = 10,
  mxUINT16_CLASS = 11,
  mxINT32_CLASS = 12,
  mxUINT32_CLASS = 13,
  mxINT64_CLASS = 14,
  mxUINT64_CLASS = 15,
  mxFUNCTION_CLASS = 16
};

bool isNormalClass(mxArrayTypes type) {
  return (((type >= mxDOUBLE_CLASS) && (type < mxFUNCTION_CLASS)) 
	  || (type == mxCHAR_CLASS));
}

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

Array FromDimensions(Dimensions dims) {
  Array x(Array::int32VectorConstructor(dims.getLength()));
  for (int i=0;i<dims.getLength();i++)
    ((int32*) x.getReadWriteDataPointer())[i] = dims[i];
  return x;
}

Dimensions ToDimensions(Array dims) {
  if (dims.getLength() > maxDims)
    throw Exception(string("MAT Variable has more dimensions than maxDims (currently set to ") + 
		    maxDims + ")."); // FIXME - more graceful ways to do this
  Dimensions dm;
  for (int i=0;i<dims.getLength();i++)
    dm[i] = ((const int32*) dims.getDataPointer())[i];
  return dm;
}

Array SynthesizeClass(MatIO* m, Array dims) {
  char buffer[100];
  bool ateof;
  Array className(m->getAtom(ateof));
  if (className.getDataClass() != FM_INT8)
    throw Exception("Corrupted MAT file - invalid class name");
  stringVector classname;
  classname.push_back(ArrayToString(className));
  Array fieldNameLength(m->getAtom(ateof));
  fieldNameLength.promoteType(FM_INT32);
  const int32 *qp = (const int32*) fieldNameLength.getDataPointer();
  int fieldNameLen = qp[0];
  Array fieldNames(m->getAtom(ateof));
  fieldNames.promoteType(FM_INT8);
  int fieldNamesLen = fieldNames.getLength();
  int fieldNameCount = fieldNamesLen / fieldNameLen;
  const int8 *dp = (const int8*) fieldNames.getDataPointer();
  stringVector names;
  for (int i=0;i<fieldNameCount;i++) {
    for (int j=0;j<fieldNameLen;j++)
      buffer[j] = dp[i*fieldNameLen+j];
    buffer[fieldNameLen] = 0;
    names.push_back(string(buffer));
  }
  Dimensions dm(ToDimensions(dims));
  int num = dm.getElementCount();
  Array *sp = new Array[num*fieldNameCount];
  for (int j=0;j<fieldNameCount;j++)
    for (int i=0;i<num;i++)
      sp[i*fieldNameCount+j] = m->getAtom(ateof);
  return Array(FM_STRUCT_ARRAY,dm,sp,false,names,classname);
}

Array SynthesizeStruct(MatIO* m, Array dims) {
  char buffer[100];
  bool ateof;
  Array fieldNameLength(m->getAtom(ateof));
  fieldNameLength.promoteType(FM_INT32);
  const int32 *qp = (const int32*) fieldNameLength.getDataPointer();
  int fieldNameLen = qp[0];
  Array fieldNames(m->getAtom(ateof));
  fieldNames.promoteType(FM_INT8);
  int fieldNamesLen = fieldNames.getLength();
  int fieldNameCount = fieldNamesLen / fieldNameLen;
  const int8 *dp = (const int8*) fieldNames.getDataPointer();
  stringVector names;
  for (int i=0;i<fieldNameCount;i++) {
    for (int j=0;j<fieldNameLen;j++)
      buffer[j] = dp[i*fieldNameLen+j];
    buffer[fieldNameLen] = 0;
    names.push_back(string(buffer));
  }
  Dimensions dm(ToDimensions(dims));
  int num = dm.getElementCount();
  Array *sp = new Array[num*fieldNameCount];
  for (int j=0;j<fieldNameCount;j++)
    for (int i=0;i<num;i++)
      sp[i*fieldNameCount+j] = m->getAtom(ateof);
  return Array(FM_STRUCT_ARRAY,dm,sp,false,names);
}

void WriteStruct(MatIO* m, Array x) {
  // Calculate the maximum field name length
  stringVector fnames(x.getFieldNames()); // FIXME - should we truncate to 32 byte fieldnames?
  int fieldNameCount = fnames.size();
  int maxlen = 0;
  for (int i=0;i<fieldNameCount;i++)
    maxlen = max(maxlen,fnames[i].size());
  // Write it as an int32 
  Array fieldNameLength(Array::int32Constructor(maxlen));
  m->putNumeric(fieldNameLength);
  // Build a character array 
  Array fieldNameText(FM_INT8,Dimensions(1,fieldNameCount*maxlen));
  int8* dp = (int8*) fieldNameText.getReadWriteDataPointer();
  for (int i=0;i<fieldNameCount;i++)
    for (int j=0;j<fnames[i].size();j++)
      dp[i*maxlen+j] = fnames[i][j];
  m->putNumeric(fieldNameText);
  int num = x.getLength();
  const Array *sp = (const Array *) x.getDataPointer();
  for (int j=0;j<fieldNameCount;j++)
    for (int i=0;i<num;i++)
      m->putMatrix(sp[i*fieldNameCount+j]);
}

// Convert from CRS-->IJV
// Replace the col_ptr array by a new one
// [1,4,8,10,13,17,20] -->
//   [1 1 1, 2 2 2 2, 3 3, 4, 4, 4, 5...]

Array SynthesizeSparse(MatIO* m, Array dims, bool complexFlag, bool logicalFlag) {
  Dimensions dm(ToDimensions(dims));
  bool ateof;
  Array ir(m->getAtom(ateof));
  Array jc(m->getAtom(ateof));
  Array pr(m->getAtom(ateof));
  int nnz = pr.getLength();
  Array pi;
  ir.promoteType(FM_UINT32);
  uint32* ir_data = (uint32*) ir.getReadWriteDataPointer();
  for (int i=0;i<ir.getLength();i++)
    ir_data[i]++;
  jc.promoteType(FM_UINT32);
  if (complexFlag) pi = m->getAtom(ateof);
  if (pr.getDataClass() < FM_INT32) {
    pr.promoteType(FM_INT32);
    pi.promoteType(FM_INT32);
  }
  Class outType(pr.getDataClass());
  if (complexFlag)
    outType = (outType == FM_FLOAT) ? FM_COMPLEX : FM_DCOMPLEX;
  if (logicalFlag)
    pr.promoteType(FM_LOGICAL);
  // Convert jc into jr, the col
  const uint32 *jc_data = (const uint32*) jc.getDataPointer();
  MemBlock<uint32> jrBlock(nnz);
  uint32 *jr = &jrBlock;
  int outptr = 0;
  for (int i=0;i<dm[1];i++)
    for (int j=jc_data[i];j<jc_data[i+1];j++)
      jr[outptr++] = (i+1);
  if (!complexFlag)
    return Array(outType,dm,
		 makeSparseFromIJV(outType,
				   dm[0],dm[1],nnz,
				   ir_data, 1,
				   jr, 1,
				   pr.getDataPointer(),1), true);
  else {
    if (outType == FM_COMPLEX) {
      float *qp = (float*) Malloc(nnz*2*sizeof(float));
      const float *qp_real = (const float *) pr.getDataPointer();
      const float *qp_imag = (const float *) pi.getDataPointer();
      for (int i=0;i<nnz;i++) {
	qp[2*i] = qp_real[i];
	qp[2*i+1] = qp_imag[i];
      }
      return Array(outType,dm,
		   makeSparseFromIJV(outType,dm[0],dm[1],nnz,
				     ir_data, 1, jr, 1, qp,1),true);
    } else {
      double *qp = (double*) Malloc(nnz*2*sizeof(double));
      const double *qp_real = (const double *) pr.getDataPointer();
      const double *qp_imag = (const double *) pi.getDataPointer();
      for (int i=0;i<nnz;i++) {
	qp[2*i] = qp_real[i];
	qp[2*i+1] = qp_imag[i];
      }
      return Array(outType,dm,
		   makeSparseFromIJV(outType,dm[0],dm[1],nnz,
				     ir_data, 1, jr, 1, qp,1),true);
    }
  }
  throw Exception("Unhandled sparse case");
}

Array SynthesizeCell(MatIO* m, Array dims) {
  // Count how many elements we expect
  Dimensions dm(ToDimensions(dims));
  int num = dm.getElementCount();
  bool ateof;
  Array *dp = new Array[num];
  for (int i=0;i<num;i++)
    dp[i] = m->getAtom(ateof);
  return Array(FM_CELL_ARRAY,dm,dp);
}

void WriteCell(MatIO*m, Array x) {
  const Array *dp = (const Array *) x.getDataPointer();
  int num = x.getLength();
  for (int i=0;i<num;i++)
    m->putMatrix(dp[i],false);
}

void MatIO::FreeDecompressor() {
  inflateEnd(zstream);
  free(zstream);
  m_compressed_data = false;
  Free(m_compression_buffer);
}

void MatIO::DecompressBytes(uint32 bcount) {
  // Allocate an array to hold the compressed bytes
  m_compression_buffer = (uint8*) Malloc(bcount);
  fread(m_compression_buffer,sizeof(uint8),bcount,m_fp);
  // Set up the zstream...
  zstream = (z_streamp) calloc(1,sizeof(z_stream));
  zstream->zalloc = NULL;
  zstream->zfree = NULL;
  zstream->opaque = NULL;
  zstream->next_in = m_compression_buffer;
  zstream->next_out = NULL;
  zstream->avail_in = bcount;
  zstream->avail_out = 0;
  int retval;
  retval = inflateInit(zstream);
  if (retval) throw Exception("inflateInit didn't work");
  m_compressed_data = true;
  //   uint8 *sp = (uint8*) Malloc(100);
  //   uLongf len;
  //   len = 100;
  //   int retval = uncompress(sp,&len,dp,bcount);
  std::cout << "Retval : " << retval << "\r\n";
}

Array SynthesizeNumeric(mxArrayTypes arrayType, bool isComplex, 
			Array dims, Array pr, Array pi) {
  // Depending on the type of arrayType, we may need to adjust
  // the types of the data vectors;
  switch (arrayType) {
  case mxINT8_CLASS:
    pr.promoteType(FM_INT8);
    pi.promoteType(FM_INT8);
    break;
  case mxUINT8_CLASS:
    pr.promoteType(FM_UINT8);
    pi.promoteType(FM_UINT8);
    break;
  case mxINT16_CLASS:
    pr.promoteType(FM_INT16);
    pi.promoteType(FM_INT16);
    break;
  case mxUINT16_CLASS:
    pr.promoteType(FM_UINT16);
    pi.promoteType(FM_UINT16);
    break;
  case mxINT32_CLASS:
    pr.promoteType(FM_INT32);
    pi.promoteType(FM_INT32);
    break;
  case mxUINT32_CLASS:
    pr.promoteType(FM_UINT32);
    pi.promoteType(FM_UINT32);
    break;
  case mxINT64_CLASS:
    pr.promoteType(FM_INT64);
    pi.promoteType(FM_INT64);
    break;
  case mxUINT64_CLASS:
    pr.promoteType(FM_UINT64);
    pi.promoteType(FM_UINT64);
    break;
  case mxSINGLE_CLASS:
    pr.promoteType(FM_FLOAT);
    pi.promoteType(FM_FLOAT);
    break;
  case mxDOUBLE_CLASS:
    pr.promoteType(FM_DOUBLE);
    pi.promoteType(FM_DOUBLE);
    break;
  case mxCHAR_CLASS:
    pr.promoteType(FM_STRING);
    pi.promoteType(FM_STRING);
    break;
  }
  if (!isComplex) {
    Dimensions dm(ToDimensions(dims));
    pr.reshape(dm);
    return pr;
  }
  throw Exception("complex types not handled yet??");
  return pr;
}

uint16 MatIO::readUint16() {
  uint16 x;
  if (!m_compressed_data) {
    fread(&x,sizeof(uint16),1,m_fp);
  } else 
    ReadCompressedBytes(&x,sizeof(uint16));
  if (!m_endianSwap) return x;
  SwapBuffer((char*)&x,1,sizeof(uint16));
  return x;
}

uint32 MatIO::readUint32() {
  uint32 x;
  if (!m_compressed_data) {
    fread(&x,sizeof(uint32),1,m_fp);
  } else 
    ReadCompressedBytes(&x,sizeof(uint32));
  if (!m_endianSwap) return x;
  SwapBuffer((char*)&x,1,sizeof(uint32));
  return x;
}

uint32 ElementSize(Class cls) {
  switch (cls) {
  case FM_LOGICAL:
    return sizeof(logical);
  case FM_UINT8:
    return sizeof(uint8);
  case FM_INT8:
    return sizeof(int8);
  case FM_UINT16:
    return sizeof(uint16);
  case FM_INT16:
    return sizeof(int16);
  case FM_UINT32:
    return sizeof(uint32);
  case FM_INT32:
    return sizeof(int32);
  case FM_UINT64:
    return sizeof(uint64);
  case FM_INT64:
    return sizeof(int64);
  case FM_FLOAT:
    return sizeof(float);
  case FM_DOUBLE:
    return sizeof(double);
  }
}

template <class T>
Array readRawArray(MatIO *m, Class cls, uint32 len, FILE *m_fp) {
  T* dp = (T*) Array::allocateArray(cls,len);
  uint8 dummy[8];
  if (!m->inCompressionMode()) {
    fread(dp,sizeof(T),len,m_fp);
    fseek(m_fp,m->BytesToAlign64Bit(),SEEK_CUR);
  } else {
    m->ReadCompressedBytes(dp,sizeof(T)*len);
    memset(dummy,0,8);
    m->ReadCompressedBytes(dummy,m->BytesToAlign64Bit());
  }
  if (m->inEndianSwap())
    SwapBuffer((char*)dp,len,ElementSize(cls));
  return Array(cls,Dimensions(len,1),dp);
}

uint32 MatIO::BytesToAlign64Bit() {
  if (!inCompressionMode()) 
    return ((8-(ftell(m_fp)&0x7)) % 8);
  else
    return (8-(zstream->total_out)&0x7);
}

void MatIO::ReadCompressedBytes(void *dest, int toread) {
  zstream->next_out = (Bytef*) dest;
  zstream->avail_out = toread;
  while (zstream->avail_out) {
    int ret = inflate(zstream,Z_SYNC_FLUSH);
    if (ret < 0)
      throw Exception(string("inflate failed with code: ") + ret);
  }
}

// Write a data element to the stream
void MatIO::putNumeric(Array x) {
  // Write out the data tag
  writeUint32(GetDataType(x));
  writeUint32(GetByteCount(x));
  writeRawArray(this,x.getDataPointer(),ByteCount,m_fp);
  return;
}

// Write a matrix to the stream
void MatIO::putMatrix(Array x, bool isGlobal) {
  Array aFlags(uint32VectorConstructor(2));
  uint32 *dp = (uint32 *) aFlags.getReadWriteDataPointer();
  bool isComplex = x.isComplex();
  bool isLogical = (x.getDataClass() == FM_LOGICAL);
  mxArrayTypes arrayType = GetArrayType(x);
  dp[0] = arrayType;
  if (isGlobal)   dp[0] |= (complexFlag << 8);
  if (isLogical)  dp[0] |= (logicalFlag << 8);
  if (isComplex)  dp[0] |= (globalFlag << 8);
  putNumeric(aFlags);
  Array dims(FromDimensions(x.getDimensions()));
  putNumeric(dims);
  if (isNormalClass(arrayType)) {
    if (!isComplex)
      putNumeric(x);
    else {
      Array xreal, ximag;
      ComplexSplit(x,xreal,ximag);
      putNumeric(xreal);
      putNumeric(ximag);
    }
  } else if  (arrayType == mxCELL_CLASS) 
    return WriteCell(this,x);
  else if ((arrayType == mxSTRUCT_CLASS))
    return WriteStruct(this,x);
  else if (arrayType == mxOBJECT_CLASS)
    return WriteObject(this,x);
  else if (arrayType == mxSPARSE_CLASS)
    return WriteSparse(this,x);
  else throw Exception(string("Unable to do this one :") + arrayType);
}

Array MatIO::getAtom(bool &ateof) {
  uint32 tag1 = readUint32();
  ateof = false;
  if (feof(m_fp)) {
    ateof = true;
    return Array();
  }
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
    DataType = LowerWord(tag1);
    ByteCount = UpperWord(tag1);
  }
  // Is it a compression flag?
  if (DataType == miCOMPRESSED) {
    DecompressBytes(ByteCount);
    Array ret(getAtom(ateof));
    FreeDecompressor();
    return ret;
  } 
  if (DataType == miMATRIX) {
    int fstart = ftell(m_fp);
    Array aFlags(getAtom(ateof));
    if (aFlags.getDataClass() != FM_UINT32)
      throw Exception("Corrupted MAT file - array flags");
    if (aFlags.getLength() != 2)
      throw Exception("Corrupted MAT file - array flags");
    const uint32 *dp = (const uint32 *) aFlags.getDataPointer();
    mxArrayTypes arrayType = (mxArrayTypes) (ByteOne(dp[0]));
    uint8 arrayFlags = ByteTwo(dp[0]);
    bool isComplex = (arrayFlags & complexFlag) != 0;
    bool isGlobal = (arrayFlags & globalFlag) != 0;
    bool isLogical = (arrayFlags & globalFlag) != 0;
    Array dims(getAtom(ateof));
    if (dims.getDataClass() != FM_INT32)
      throw Exception("Corrupted MAT file - dimensions array");
    Array name(getAtom(ateof));
    if (name.getDataClass() != FM_INT8)
      throw Exception("Corrupted MAT file - array name");
    name.promoteType(FM_STRING);
    cout << " Found array: " << ArrayToString(name) << " of type " << arrayType << "\r\n";
    if (isNormalClass(arrayType)) {
      Array pr(getAtom(ateof));
      Array pi;
      if (isComplex)
	pi = getAtom(ateof);
      return SynthesizeNumeric((mxArrayTypes)arrayType,isComplex,dims,pr,pi);
    } else if (arrayType == mxCELL_CLASS) 
      return SynthesizeCell(this,dims);
    else if ((arrayType == mxSTRUCT_CLASS))
      return SynthesizeStruct(this,dims);
    else if (arrayType == mxOBJECT_CLASS)
      return SynthesizeClass(this,dims);
    else if (arrayType == mxSPARSE_CLASS)
      return SynthesizeSparse(this,dims,isComplex,isLogical);
    else throw Exception(string("Unable to do this one :") + arrayType);
  }
  if (DataType == miINT8) 
    return readRawArray<int8>(this,FM_INT8,ByteCount,m_fp);
  if ((DataType == miUINT8) || (DataType == miUTF8))
    return readRawArray<uint8>(this,FM_UINT8,ByteCount,m_fp);
  if (DataType == miINT16) 
    return readRawArray<int16>(this,FM_INT16,ByteCount>>1,m_fp);
  if ((DataType == miUINT16) || (DataType == miUTF16))
    return readRawArray<uint16>(this,FM_UINT16,ByteCount>>1,m_fp);
  if (DataType == miINT32) 
    return readRawArray<int32>(this,FM_INT32,ByteCount>>2,m_fp);
  if ((DataType == miUINT32) || (DataType == miUTF32))
    return readRawArray<uint32>(this,FM_UINT32,ByteCount>>2,m_fp);
  if (DataType == miSINGLE)
    return readRawArray<float>(this,FM_FLOAT,ByteCount>>2,m_fp);
  if (DataType == miDOUBLE)
    return readRawArray<double>(this,FM_DOUBLE,ByteCount>>3,m_fp);
  if (DataType == miINT64)
    return readRawArray<int64>(this,FM_INT64,ByteCount>>3,m_fp);
  if (DataType == miUINT64)
    return readRawArray<uint64>(this,FM_UINT64,ByteCount>>3,m_fp);
  throw Exception(string("Unhandled data thingy ") + DataType);
}

MatIO::MatIO(string filename, openMode mode) :
  m_filename(filename), m_mode(mode), m_compressed_data(false) {
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
  uint16 val = readUint16();
  if ((val != 0x100) && (val != 0x1)) throw Exception("Not a valid MAT file");
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
  fwrite(hdrtxt,1,124,m_fp);
  writeUint16(0x100);
  writeUint16('M' << 8 | 'I');
}

MatIO::~MatIO() {
  fclose(m_fp);
}

ArrayVector MatLoadFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) throw Exception("Need filename");
  MatIO m(ArrayToString(arg[0]),MatIO::readMode);
  m.getHeader();
  bool ateof = false;
  while(!ateof) {
    Array a(m.getAtom(ateof));
    if (!ateof)
      PrintArrayClassic(a,1000,eval,true);
  }
  return ArrayVector();
}
