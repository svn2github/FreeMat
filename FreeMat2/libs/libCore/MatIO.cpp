#include "MatIO.hpp"
#include "Sparse.hpp"
#include "Exception.hpp"
#include "Print.hpp"
#include "Malloc.hpp"
#include "MemPtr.hpp"
#include "Array.hpp"
#include "Interpreter.hpp"
#include <zlib.h>

extern void SwapBuffer(char* cp, int count, int elsize);

// A class to read/write Matlab MAT files.  Implemented based on the Matlab 7.1
// version of the file matfile_format.pdf from the Mathworks web site.

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


MatTypes ToMatType(Class x) {
  switch (x) {
  case FM_INT8:
    return miINT8;
  case FM_UINT8:
    return miUINT8;
  case FM_INT16:
    return miINT16;
  case FM_UINT16:
    return miUINT16;
  case FM_INT32:
    return miINT32;
  case FM_UINT32:
    return miUINT32;
  case FM_FLOAT:
    return miSINGLE;
  case FM_DOUBLE:
    return miDOUBLE;
  case FM_INT64:
    return miINT64;
  case FM_UINT64:
    return miUINT64;
  }
  throw Exception("Should not be here...");
}

mxArrayTypes GetArrayType(Class x) {
  switch(x) {
  case FM_FUNCPTR_ARRAY:
    return mxFUNCTION_CLASS;
  case FM_CELL_ARRAY:
    return mxCELL_CLASS;
  case FM_STRUCT_ARRAY:
    return mxSTRUCT_CLASS;
  case FM_LOGICAL:
    return mxINT32_CLASS;
  case FM_UINT8:
    return mxUINT8_CLASS;
  case FM_INT8:
    return mxINT8_CLASS5
  case FM_UINT16:
      return mxUINT16_CLASS;
  case FM_INT16:
    return mxINT16_CLASS;
  case FM_UINT32:
    return mxUINT32_CLASS;
  case FM_INT32:
    return mxINT32_CLASS;
  case FM_UINT64:
    return mxUINT64_CLASS;
  case FM_INT64:
    return mxINT64_CLASS;
  case FM_FLOAT:
    return mxSINGLE_CLASS;
  case FM_DOUBLE:
    return mxDOUBLE_CLASS;
  case FM_COMPLEX:
    return mxSINGLE_CLASS;
  case FM_DCOMPLEX:
    return mxDOUBLE_CLASS;
  case FM_STRING:
    return mxCHAR_CLASS;
  }  
}

Class ToFreeMatClass(MatTypes x) {
  switch (x) {
  case miINT8:
    return FM_INT8;
  case miUINT8:
    return FM_UINT8;
  case miINT16:
    return FM_INT16;
  case miUINT16:
    return FM_UINT16;
  case miINT32:
    return FM_INT32;
  case miUINT32:
    return FM_UINT32;
  case miSINGLE:
    return FM_FLOAT;
  case miDOUBLE:
    return FM_DOUBLE;
  case miINT64:
    return FM_INT64;
  case miUINT64:
    return FM_UINT64;
  case miUTF8:
    return FM_UINT8;
  case miUTF16:
    return FM_UINT16;
  case miUTF32:
    return FM_UINT32;
  }
  throw Exception("Should not be here...");
}

bool isNormalClass(MatIO::mxArrayTypes type) {
  return (((type >= MatIO::mxDOUBLE_CLASS) && (type < MatIO::mxFUNCTION_CLASS)) 
	  || (type == MatIO::mxCHAR_CLASS));
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
  Array x(FM_INT32,Dimensions(1,dims.getLength()));
  for (int i=0;i<dims.getLength();i++)
    ((int32*) x.getReadWriteDataPointer())[i] = dims[i];
  return x;
}

Dimensions ToDimensions(Array dims) {
  if (dims.getLength() > maxDims)
    throw Exception(string("MAT Variable has more dimensions ") + 
		    string("than maxDims (currently set to ") + 
		    maxDims + ")."); // FIXME - more graceful ways to do this
  Dimensions dm;
  for (int i=0;i<dims.getLength();i++)
    dm[i] = ((const int32*) dims.getDataPointer())[i];
  return dm;
}

Array MatIO::getSparseArray(Dimensions dm, bool complexFlag) {
  Array ir(getDataElement());
  Array jc(getDataElement());
  Array pr(getDataElement());
  int nnz = pr.getLength();
  Array pi;
  ir.promoteType(FM_UINT32);
  uint32* ir_data = (uint32*) ir.getReadWriteDataPointer();
  for (int i=0;i<ir.getLength();i++)
    ir_data[i]++;
  jc.promoteType(FM_UINT32);
  if (complexFlag) pi = getDataElement();
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

Array MatIO::getNumericArray(mxArrayTypes arrayType, Dimensions dm, bool complexFlag) {
  Array pr(getDataElement());
  Array pi;
  if (complexFlag)
    pi = getDataElement();
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
  if (!complexFlag) {
    pr.reshape(dm);
    return pr;
  } 
  throw Exception("complex types not handled yet??");
}

Array MatIO::getClassArray(Dimensions dm) {
  char buffer[100];
  Array className(getDataElement());
  if (className.getDataClass() != FM_INT8)
    throw Exception("Corrupted MAT file - invalid class name");
  stringVector classname;
  classname.push_back(ArrayToString(className));
  Array fieldNameLength(getDataElement());
  fieldNameLength.promoteType(FM_INT32);
  const int32 *qp = (const int32*) fieldNameLength.getDataPointer();
  int fieldNameLen = qp[0];
  Array fieldNames(getDataElement());
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
  int num = dm.getElementCount();
  Array *sp = new Array[num*fieldNameCount];
  for (int j=0;j<fieldNameCount;j++)
    for (int i=0;i<num;i++) {
      bool atEof; string name;
      sp[i*fieldNameCount+j] = getArray(atEof,name);
    }
  return Array(FM_STRUCT_ARRAY,dm,sp,false,names,classname);
}

Array MatIO::getStructArray(Dimensions dm) {
  char buffer[100];
  Array fieldNameLength(getDataElement());
  fieldNameLength.promoteType(FM_INT32);
  const int32 *qp = (const int32*) fieldNameLength.getDataPointer();
  int fieldNameLen = qp[0];
  Array fieldNames(getDataElement());
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
  int num = dm.getElementCount();
  Array *sp = new Array[num*fieldNameCount];
  for (int j=0;j<fieldNameCount;j++)
    for (int i=0;i<num;i++) {
      bool atEof; string name;
      sp[i*fieldNameCount+j] = getArray(atEof,name);
    }
  return Array(FM_STRUCT_ARRAY,dm,sp,false,names);
}

Array MatIO::getCellArray(Dimensions dm) {
  // Count how many elements we expect
  int num = dm.getElementCount();
  Array *dp = new Array[num];
  for (int i=0;i<num;i++) {
    bool ateof; string name;
    dp[i] = getArray(ateof,name);
  }
  return Array(FM_CELL_ARRAY,dm,dp);
}

Array MatIO::getDataElement() {
  uint32 tag1 = getUint32();
  uint32 DataType, ByteCount;
  // Is the upper word of tag1 zero?
  if (UpperWord(tag1) == 0) {
    // Yes, then tag1 is the datatype
    // and tag2 is the byte count
    DataType = tag1;
    ByteCount = getUint32();
  } else {
    // No, then the upper word of tag1 is the byte count
    // and the lower word of tag1 is the data type
    DataType = LowerWord(tag1);
    ByteCount = UpperWord(tag1);
  }
  void *dp = Malloc(ByteCount);
  ReadData(dp,ByteCount);
  Align64Bit();
  Class fmClass(ToFreeMatClass((MatTypes)DataType));
  uint32 byteSize(ElementSize(fmClass));
  uint32 len(ByteCount/byteSize);
  if (m_endianSwap)
    SwapBuffer((char*)dp,len,byteSize);
  return Array(fmClass,Dimensions(len,1),dp);
}

void MatIO::putDataElement(const Array &x) {
  Class fmClass(x.getDataClass());
  MatTypes DataType(ToMatType(fmClass));
  uint32 ByteCount(x.getLength()*ElementSize(fmClass));
  putUint32((uint32)DataType);
  putUint32(ByteCount);
  WriteData(x.getDataPointer(),ByteCount);
  Align64Bit();
}

void MatIO::InitializeDecompressor(uint32 bcount) {
  // Allocate an array to hold the compressed bytes
  m_compression_buffer = (uint8*) Malloc(bcount);
  ReadFileBytes(m_compression_buffer,bcount);
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
}

void MatIO::ReadCompressedBytes(void *dest, uint32 toread) {
  zstream->next_out = (Bytef*) dest;
  zstream->avail_out = toread;
  while (zstream->avail_out) {
    int ret = inflate(zstream,Z_SYNC_FLUSH);
    if (ret < 0)
      throw Exception(string("inflate failed with code: ") + ret);
  }
}

void MatIO::CloseDecompressor() {
  inflateEnd(zstream);
  free(zstream);
  m_compressed_data = false;
  Free(m_compression_buffer);
}

void MatIO::ReadFileBytes(void *dest, uint32 toread) {
  fread(dest,1,toread,m_fp);
}

void MatIO::WriteFileBytes(const void *dest, uint32 towrite) {
  fwrite(dest,1,towrite,m_fp);
}

void MatIO::WriteData(const void *dest, uint32 towrite) {
  if (!m_compressed_data)
    WriteFileBytes(dest,towrite);
  else
    throw Exception("Unhandled case of compression writing");
    //    WriteCompressedBytes(dest,towrite);
}

void MatIO::ReadData(void *dest, uint32 toread) {
  if (!m_compressed_data)
    ReadFileBytes(dest,toread);
  else
    ReadCompressedBytes(dest,toread);
}

uint16 MatIO::getUint16() {
  uint16 x;
  ReadData(&x,sizeof(uint16));
  if (m_endianSwap)
    SwapBuffer((char*)&x,1,sizeof(uint16));
  return x;
}

uint32 MatIO::getUint32() {
  uint32 x;
  ReadData(&x,sizeof(uint32));
  if (m_endianSwap)
    SwapBuffer((char*)&x,1,sizeof(uint32));
  return x;
}

void MatIO::Align64Bit() {
  if (m_mode == readMode) {
    uint32 adjustBytes;
    if (!m_compressed_data) {
      adjustBytes = ((8-(ftell(m_fp)&0x7)) % 8);
      fseek(m_fp,adjustBytes,SEEK_CUR);
    } else {
      uint8 dummy[8];
      adjustBytes = (8-(zstream->total_out) & 0x7);
      ReadCompressedBytes(dummy,adjustBytes);
    }
  }
}

void MatIO::putStructArray(const Array &x) {
  // Calculate the maximum field name length
  stringVector fnames(x.getFieldNames()); // FIXME - should we truncate to 32 byte fieldnames?
  int fieldNameCount = fnames.size();
  size_t maxlen = 0;
  for (int i=0;i<fieldNameCount;i++)
    maxlen = max(maxlen,fnames[i].size());
  // Write it as an int32 
  Array fieldNameLength(Array::int32Constructor(maxlen));
  putDataElement(fieldNameLength);
  // Build a character array 
  Array fieldNameText(FM_INT8,Dimensions(1,fieldNameCount*maxlen));
  int8* dp = (int8*) fieldNameText.getReadWriteDataPointer();
  for (int i=0;i<fieldNameCount;i++)
    for (int j=0;j<fnames[i].size();j++)
      dp[i*maxlen+j] = fnames[i][j];
  putDataElement(fieldNameText);
  int num = x.getLength();
  const Array *sp = (const Array *) x.getDataPointer();
  for (int j=0;j<fieldNameCount;j++)
    for (int i=0;i<num;i++)
      putArray(sp[i*fieldNameCount+j]);
}

void MatIO::putClassArray(const Array &x) {
  string className = x.getClassName().back();
  Array classNameArray(FM_INT8,Dimensions(1,className.size()));
  int8* dp = (int8*) classNameArray.getDataPointer();
  for (int i=0;i<className.size();i++)
    dp[i] = className[i];
  putDataElement(classNameArray);
  putStructArray(x);
}


// Convert from CRS-->IJV
// Replace the col_ptr array by a new one
// [1,4,8,10,13,17,20] -->
//   [1 1 1, 2 2 2 2, 3 3, 4, 4, 4, 5...]

void MatIO::putCellArray(const Array &x) {
    // Count how many elements we expect
  int num = x.getLength();
  const Array *dp = (const Array *) x.getDataPointer();
  for (int i=0;i<num;i++)
    putArray(dp[i]);
}

//Write a matrix to the stream
void MatIO::putArray(const Array &x, string name) {
  Array aFlags(FM_UINT32,Dimensions(1,2));
  uint32 *dp = (uint32 *) aFlags.getReadWriteDataPointer();
  bool isComplex = x.isComplex();
  bool isLogical = (x.getDataClass() == FM_LOGICAL);
  bool globalFlag = false;
  mxArrayTypes arrayType = GetArrayType(x.getDataClass());
  dp[0] = arrayType;
  if (isGlobal)   dp[0] |= (complexFlag << 8);
  if (isLogical)  dp[0] |= (logicalFlag << 8);
  if (isComplex)  dp[0] |= (globalFlag << 8);
  putDataElement(aFlags);
  putDataElement(FromDimensions(x.getDimensions()));
  if (isNormalClass(arrayType))
    putNumericArray(x);
  else if  (arrayType == mxCELL_CLASS) 
    putCellArray(x);
  else if (arrayType == mxSTRUCT_CLASS)
    putStructArray(x);
  else if (arrayType == mxOBJECT_CLASS)
    putClassArray(x);
//   else if (arrayType == mxSPARSE_CLASS)
//     putSparseArray(x);
  else throw Exception(string("Unable to do this one :") + arrayType);
}

void MatIO::putNumericArray(const Array &x) {
  if (!x.isComplex())
    putDataElement(x);
  else {
    Array xreal, ximag;
    ComplexSplit(x,xreal,ximag);
    putDataElement(xreal);
    putDataElement(ximag);
  }
}

Array MatIO::getArray(bool &atEof, string &name) {
  uint32 tag1 = getUint32();
  atEof = false;
  if (feof(m_fp)) {
    atEof = true;
    return Array();
  }
  uint32 DataType, ByteCount;
  // Is the upper word of tag1 zero?
  if (UpperWord(tag1) == 0) {
    // Yes, then tag1 is the datatype
    // and tag2 is the byte count
    DataType = tag1;
    ByteCount = getUint32();
  } else {
    // No, then the upper word of tag1 is the byte count
    // and the lower word of tag1 is the data type
    DataType = LowerWord(tag1);
    ByteCount = UpperWord(tag1);
  }
  // Is it a compression flag?
  if (DataType == miCOMPRESSED) {
    InitializeDecompressor(ByteCount);
    Array ret(getArray(atEof,name));
    CloseDecompressor();
    return ret;
  } 
  if (DataType != miMATRIX) 
    throw Exception("Unexpected data tag when looking for an array");
  Array aFlags(getDataElement());
  if ((aFlags.getDataClass() != FM_UINT32) || (aFlags.getLength() != 2))
    throw Exception("Corrupted MAT file - array flags");
  const uint32 *dp = (const uint32 *) aFlags.getDataPointer();
  mxArrayTypes arrayType = (mxArrayTypes) (ByteOne(dp[0]));
  uint8 arrayFlags = ByteTwo(dp[0]);
  bool isComplex = (arrayFlags & complexFlag) != 0;
  bool isGlobal = (arrayFlags & globalFlag) != 0;
  bool isLogical = (arrayFlags & globalFlag) != 0;
  Array dims(getDataElement());
  if (dims.getDataClass() != FM_INT32)
    throw Exception("Corrupted MAT file - dimensions array");
  Dimensions dm(ToDimensions(dims));
  Array namearray(getDataElement());
  if (namearray.getDataClass() != FM_INT8)
    throw Exception("Corrupted MAT file - array name");
  namearray.promoteType(FM_STRING);
  name = ArrayToString(namearray);
  if (isNormalClass(arrayType)) 
    return getNumericArray(arrayType,dm,isComplex);
  else if (arrayType == mxCELL_CLASS) 
    return getCellArray(dm);
  else if (arrayType == mxSTRUCT_CLASS)
    return getStructArray(dm);
  else if (arrayType == mxOBJECT_CLASS)
    return getClassArray(dm);
  else if (arrayType == mxSPARSE_CLASS)
    return getSparseArray(dm,isComplex);
  else 
    throw Exception(string("Unable to do this one :") + arrayType);
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
  uint16 val = getUint16();
  if ((val != 0x100) && (val != 0x1)) throw Exception("Not a valid MAT file");
  uint16 byteOrder = getUint16();
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
   memcpy(hdrtxt,hdr.c_str(),min(123,hdr.size()));
   fwrite(hdrtxt,1,124,m_fp);
   putUint16(0x100);
   putUint16('M' << 8 | 'I');
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
    string name;
    Array a(m.getArray(ateof,name));
    if (!ateof) {
      eval->outputMessage("Array: "  + name);
      PrintArrayClassic(a,1000,eval,true);
    }
  }
  return ArrayVector();
}
