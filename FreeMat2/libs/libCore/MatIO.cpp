#include "MatIO.hpp"
#include "Sparse.hpp"
#include "Exception.hpp"
#include "Print.hpp"
#include "Malloc.hpp"
#include "MemPtr.hpp"
#include "Array.hpp"
#include "Interpreter.hpp"
#include <zlib.h>

// Things to still look at:
//  Logical/Global flags - done
//  Compressed mode write - done
//  Sparse matrices - done
//  Matrix size calculation  - done
//  Load/Save interfacing - done


const int CHUNK = 32768;

extern void SwapBuffer(char* cp, int count, int elsize);

// A class to read/write Matlab MAT files.  Implemented based on the Matlab 7.1
// version of the file matfile_format.pdf from the Mathworks web site.

void ComplexSplit(const Array &x, Array &real, Array &imag) {
  if (x.dataClass() == FM_COMPLEX) {
    real = Array(FM_FLOAT,x.dimensions());
    imag = Array(FM_FLOAT,x.dimensions());
    const float *dp = (const float *) x.getDataPointer();
    float *rp = (float *) real.getReadWriteDataPointer();
    float *ip = (float *) imag.getReadWriteDataPointer();
    for (unsigned i=0;i<x.getLength();i++) {
      rp[i] = dp[2*i];
      ip[i] = dp[2*i+1];
    }      
  } else {
    real = Array(FM_DOUBLE,x.dimensions());
    imag = Array(FM_DOUBLE,x.dimensions());
    const double *dp = (const double *) x.getDataPointer();
    double *rp = (double *) real.getReadWriteDataPointer();
    double *ip = (double *) imag.getReadWriteDataPointer();
    for (unsigned i=0;i<x.getLength();i++) {
      rp[i] = dp[2*i];
      ip[i] = dp[2*i+1];
    }      
  }
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
  case FM_STRING:
    return sizeof(char);
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
  case FM_STRING:
    return miINT8;
  }
  throw Exception("Should not be here...");
}

MatIO::mxArrayTypes GetArrayType(Class x) {
  switch(x) {
  case FM_FUNCPTR_ARRAY:
    return MatIO::mxFUNCTION_CLASS;
  case FM_CELL_ARRAY:
    return MatIO::mxCELL_CLASS;
  case FM_STRUCT_ARRAY:
    return MatIO::mxSTRUCT_CLASS;
  case FM_LOGICAL:
    return MatIO::mxINT32_CLASS;
  case FM_UINT8:
    return MatIO::mxUINT8_CLASS;
  case FM_INT8:
    return MatIO::mxINT8_CLASS;
  case FM_UINT16:
    return MatIO::mxUINT16_CLASS;
  case FM_INT16:
    return MatIO::mxINT16_CLASS;
  case FM_UINT32:
    return MatIO::mxUINT32_CLASS;
  case FM_INT32:
    return MatIO::mxINT32_CLASS;
  case FM_UINT64:
    return MatIO::mxUINT64_CLASS;
  case FM_INT64:
    return MatIO::mxINT64_CLASS;
  case FM_FLOAT:
    return MatIO::mxSINGLE_CLASS;
  case FM_DOUBLE:
    return MatIO::mxDOUBLE_CLASS;
  case FM_COMPLEX:
    return MatIO::mxSINGLE_CLASS;
  case FM_DCOMPLEX:
    return MatIO::mxDOUBLE_CLASS;
  case FM_STRING:
    return MatIO::mxCHAR_CLASS;
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

const uint8 bcomplexFlag = 8;
const uint8 bglobalFlag = 4;
const uint8 blogicalFlag = 2;

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
    ((int32*) x.getReadWriteDataPointer())[i] = dims.get(i);
  return x;
}

Dimensions ToDimensions(Array dims) {
  if (dims.getLength() > maxDims)
    throw Exception(string("MAT Variable has more dimensions ") + 
		    string("than maxDims (currently set to ") + 
		    maxDims + ")."); // FIXME - more graceful ways to do this
  Dimensions dm;
  for (int i=0;i<dims.getLength();i++)
    dm.set(i,((const int32*) dims.getDataPointer())[i]);
  return dm;
}

void MatIO::putSparseArray(const Array &x) {
  uint32 *I, *J;
  int nnz;
  void *dp = SparseToIJV2(x.dataClass(),x.rows(),x.columns(),
			  x.getSparseDataPointer(),I,J,nnz);
  // Wrap I and J into arrays
  putDataElement(Array(FM_UINT32,Dimensions(nnz,1),I));
  putDataElement(Array(FM_UINT32,Dimensions(x.columns()+1,1),J));
  if (!x.isComplex())
    putDataElement(Array(x.dataClass(),Dimensions(nnz,1),dp));
  else {
    Array vset(x.dataClass(),Dimensions(nnz,1),dp);
    Array vset_real, vset_imag;
    ComplexSplit(vset,vset_real,vset_imag);
    putDataElement(vset_real);
    putDataElement(vset_imag);
  }
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
  if (pr.dataClass() < FM_INT32) {
    pr.promoteType(FM_INT32);
    pi.promoteType(FM_INT32);
  }
  Class outType(pr.dataClass());
  if (complexFlag)
    outType = (outType == FM_FLOAT) ? FM_COMPLEX : FM_DCOMPLEX;
  // Convert jc into jr, the col
  const uint32 *jc_data = (const uint32*) jc.getDataPointer();
  MemBlock<uint32> jrBlock(nnz);
  uint32 *jr = &jrBlock;
  int outptr = 0;
  for (int i=0;i<dm.get(1);i++)
    for (int j=jc_data[i];j<jc_data[i+1];j++)
      jr[outptr++] = (i+1);
  if (!complexFlag)
    return Array(outType,dm,
		 makeSparseFromIJV(outType,
				   dm.get(0),dm.get(1),nnz,
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
		   makeSparseFromIJV(outType,dm.get(0),dm.get(1),nnz,
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
		   makeSparseFromIJV(outType,dm.get(0),dm.get(1),nnz,
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
  } else {
    if (arrayType == mxSINGLE_CLASS) {
      pr.promoteType(FM_COMPLEX);
      pi.promoteType(FM_FLOAT);
      float *dp = (float *) pr.getReadWriteDataPointer();
      const float *ip = (const float *) pi.getDataPointer();
      for (int i=0;i<pr.getLength();i++)
	dp[2*i+1] = ip[i];
    } else {
      pr.promoteType(FM_DCOMPLEX);
      pi.promoteType(FM_DOUBLE);
      double *dp = (double *) pr.getReadWriteDataPointer();
      const double *ip = (const double *) pi.getDataPointer();
      for (int i=0;i<pr.getLength();i++)
	dp[2*i+1] = ip[i];
    }
    pr.reshape(dm);
    return pr;
  }
  throw Exception("complex types not handled yet??");
}

Array MatIO::getClassArray(Dimensions dm) {
  char buffer[100];
  Array className(getDataElement());
  if (className.dataClass() != FM_INT8)
    throw Exception("Corrupted MAT file - invalid class name");
  rvstring classname;
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
  rvstring names;
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
      bool atEof; string name; bool match; bool global;
      sp[i*fieldNameCount+j] = getArray(atEof,name,match,global);
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
  rvstring names;
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
      bool atEof; string name; bool match; bool global;
      sp[i*fieldNameCount+j] = getArray(atEof,name,match,global);
    }
  return Array(FM_STRUCT_ARRAY,dm,sp,false,names);
}

Array MatIO::getCellArray(Dimensions dm) {
  // Count how many elements we expect
  int num = dm.getElementCount();
  Array *dp = new Array[num];
  for (int i=0;i<num;i++) {
    bool ateof; string name; bool match; bool global;
    dp[i] = getArray(ateof,name,match,global);
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
  Class fmClass(x.dataClass());
  MatTypes DataType(ToMatType(fmClass));
  uint32 ByteCount(x.getLength()*ElementSize(fmClass));
  putUint32((uint32)DataType);
  putUint32(ByteCount);
  WriteData(x.getDataPointer(),ByteCount);
  Align64Bit();
}

void MatIO::InitializeCompressor() {
  // Allocate an array to hold the compressed bytes
  m_compression_buffer = (uint8*) Malloc(CHUNK);
  // Set up the zstream...
  zstream = (z_streamp) calloc(1,sizeof(z_stream));
  zstream->zalloc = NULL;
  zstream->zfree = NULL;
  zstream->opaque = NULL;
  zstream->next_in = NULL;
  zstream->next_out = m_compression_buffer;
  zstream->avail_in = 0;
  zstream->avail_out = CHUNK;
  int retval;
  retval = deflateInit(zstream,9);
  if (retval) throw Exception("defaultinit didn't work");
  m_compressed_data = true;
  
}

void MatIO::WriteCompressedBytes(const void *dest, uint32 towrite) {
  zstream->next_in = (Bytef*) dest;
  zstream->avail_in = towrite;
  do {
    int ret = deflate(zstream,Z_NO_FLUSH);
    if (ret == Z_STREAM_ERROR)
      throw Exception("Compression engine failed on write!");
    if (zstream->avail_out == 0) {
      WriteFileBytes(m_compression_buffer,CHUNK);
      zstream->avail_out = CHUNK;
      zstream->next_out = m_compression_buffer;
    }
  } while (zstream->avail_in);
}

void MatIO::CloseCompressor() {
  int ret;
  do {
    ret = deflate(zstream,Z_FINISH);
    if (zstream->avail_out == 0) {
      WriteFileBytes(m_compression_buffer,CHUNK);
      zstream->avail_out = CHUNK;
      zstream->next_out = m_compression_buffer;
    }
  } while (ret != Z_STREAM_END);
  WriteFileBytes(m_compression_buffer,CHUNK-zstream->avail_out);
  deflateEnd(zstream);
  free(zstream);
  m_compressed_data = false;
  Free(m_compression_buffer);
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
  m_writecount += towrite;
  if (m_phantomWriteMode) return;
  if (!m_compressed_data)
    WriteFileBytes(dest,towrite);
  else
    WriteCompressedBytes(dest,towrite);
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

void MatIO::putUint16(uint16 x) {
  WriteData(&x,sizeof(uint16));
}

void MatIO::putUint32(uint32 x) {
  WriteData(&x,sizeof(uint32));
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
  } else {
    char buffer[8];
    uint32 adjustBytes;
    adjustBytes = ((8-(m_writecount&0x7)) % 8);
    WriteData(buffer,adjustBytes);
  }
}

void MatIO::putStructArray(const Array &x) {
  // Calculate the maximum field name length
  rvstring fnames(x.fieldNames()); // FIXME - should we truncate to 32 byte fieldnames?
  int fieldNameCount = fnames.size();
  size_t maxlen = 0;
  for (int i=0;i<fieldNameCount;i++)
    maxlen = max(maxlen,fnames.at(i).size());
  // Write it as an int32 
  Array fieldNameLength(Array::int32Constructor(maxlen));
  putDataElement(fieldNameLength);
  // Build a character array 
  Array fieldNameText(FM_INT8,Dimensions(1,fieldNameCount*maxlen));
  int8* dp = (int8*) fieldNameText.getReadWriteDataPointer();
  for (int i=0;i<fieldNameCount;i++)
    for (int j=0;j<fnames.at(i).size();j++)
      dp[i*maxlen+j] = fnames.at(i)[j];
  putDataElement(fieldNameText);
  int num = x.getLength();
  const Array *sp = (const Array *) x.getDataPointer();
  for (int j=0;j<fieldNameCount;j++)
    for (int i=0;i<num;i++)
      putArray(sp[i*fieldNameCount+j]);
}

void MatIO::putClassArray(const Array &x) {
  string className = x.className().back();
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

void MatIO::putArrayCompressed(const Array &x, string name) {
  size_t spos, fpos;
  // Set the write count to zero
  m_writecount = 0;
  m_phantomWriteMode = false;
  // Write out a compression flag
  putUint32(miCOMPRESSED);
  // Get the current file position
  spos = ftell(m_fp);
  // Put out a dummy 0 place holder
  putUint32(0);
  InitializeCompressor();
  putArray(x,name);
  CloseCompressor();
  // Get our current position
  fpos = ftell(m_fp);
  // Seek to the place holder and overwrite with a byte count
  fseek(m_fp,spos,SEEK_SET);
  putUint32(fpos-spos-sizeof(int32));
  // Return to the end of the stream
  fseek(m_fp,fpos,SEEK_SET);
}

//Write a matrix to the stream
void MatIO::putArray(const Array &x, string name, bool isGlobal) {
  Array aFlags(FM_UINT32,Dimensions(1,2));
  uint32 *dp = (uint32 *) aFlags.getReadWriteDataPointer();
  bool isComplex = x.isComplex();
  bool isLogical = (x.dataClass() == FM_LOGICAL);
  mxArrayTypes arrayType = GetArrayType(x.dataClass());
  if (x.sparse())  arrayType = mxSPARSE_CLASS;
  dp[0] = arrayType;
  if (isGlobal)   dp[0] = dp[0] | (bglobalFlag << 8);
  if (isLogical)  dp[0] = dp[0] | (blogicalFlag << 8);
  if (isComplex)  dp[0] = dp[0] | (bcomplexFlag << 8);
  putUint32(miMATRIX);
  uint32 bcount = m_writecount;
  if (!m_phantomWriteMode) {
    m_phantomWriteMode = true;
    putUint32(0);
    putArraySpecific(x,aFlags,name,arrayType);
    m_phantomWriteMode = false;
    putUint32(m_writecount-bcount-4);
    m_writecount = bcount+4;
    putArraySpecific(x,aFlags,name,arrayType);
  } else {
    putUint32(0);
    putArraySpecific(x,aFlags,name,arrayType);
  }
}


void MatIO::putArraySpecific(const Array &x, Array aFlags, 
			     string name, mxArrayTypes arrayType) {
  putDataElement(aFlags);
  putDataElement(FromDimensions(x.dimensions()));
  putDataElement(Array::stringConstructor(name));
  if (isNormalClass(arrayType))
    putNumericArray(x);
  else if  (arrayType == mxCELL_CLASS) 
    putCellArray(x);
  else if (arrayType == mxSTRUCT_CLASS)
    putStructArray(x);
  else if (arrayType == mxOBJECT_CLASS)
    putClassArray(x);
  else if (arrayType == mxSPARSE_CLASS)
    putSparseArray(x);
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

Array MatIO::getArray(bool &atEof, string &name, bool &match, bool &isGlobal) {
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
    Array ret(getArray(atEof,name,match,isGlobal));
    CloseDecompressor();
    return ret;
  } 
  if (DataType != miMATRIX) 
    throw Exception("Unexpected data tag when looking for an array");
  uint32 fp(ftell(m_fp));
  Array aFlags(getDataElement());
  if ((aFlags.dataClass() != FM_UINT32) || (aFlags.getLength() != 2))
    throw Exception("Corrupted MAT file - array flags");
  const uint32 *dp = (const uint32 *) aFlags.getDataPointer();
  mxArrayTypes arrayType = (mxArrayTypes) (ByteOne(dp[0]));
  uint8 arrayFlags = ByteTwo(dp[0]);
  bool isComplex = (arrayFlags & bcomplexFlag) != 0;
  isGlobal = (arrayFlags & bglobalFlag) != 0;
  bool isLogical = (arrayFlags & blogicalFlag) != 0;
  Array dims(getDataElement());
  if (dims.dataClass() != FM_INT32)
    throw Exception("Corrupted MAT file - dimensions array");
  Dimensions dm(ToDimensions(dims));
  Array namearray(getDataElement());
  if (namearray.dataClass() != FM_INT8)
    throw Exception("Corrupted MAT file - array name");
  namearray.promoteType(FM_STRING);
  string tname = ArrayToString(namearray);
//   if (tname != name) {
//     match = false;
//     if (m_compressed_data)
//       return Array();
//     else {
//       // Take the current position, subtract fp
//       fseek(m_fp,fp+ByteCount,SEEK_SET);
//       return Array();
//     }
//   }
  match = true;
  name = tname;
  Array toret;
  if (isNormalClass(arrayType)) 
    toret = getNumericArray(arrayType,dm,isComplex);
  else if (arrayType == mxCELL_CLASS) 
    toret = getCellArray(dm);
  else if (arrayType == mxSTRUCT_CLASS)
    toret = getStructArray(dm);
  else if (arrayType == mxOBJECT_CLASS)
    toret = getClassArray(dm);
  else if (arrayType == mxSPARSE_CLASS)
    toret = getSparseArray(dm,isComplex);
  else 
    throw Exception(string("Unable to do this one :") + arrayType);
  if (isLogical)
    toret.promoteType(FM_LOGICAL);
  return toret;
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
  m_phantomWriteMode = false;
  m_writecount = 0;
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
   memcpy(hdrtxt,hdr.c_str(),min((size_t)123,hdr.size()));
   fwrite(hdrtxt,1,124,m_fp);
   putUint16(0x100);
   putUint16('M' << 8 | 'I');
   m_writecount = 0;
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
    bool globalFlag = false;
    bool match = false;
    Array a(m.getArray(ateof,name,match,globalFlag));
    if (!ateof) {
      if (globalFlag)
	eval->getContext()->addGlobalVariable(name);
      eval->getContext()->insertVariable(name,a);
    }
  }
  return ArrayVector();
}

ArrayVector MatSaveFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) throw Exception("Need filename");
  MatIO m(ArrayToString(arg[0]),MatIO::writeMode);
  Context *cntxt = eval->getContext();
  stringVector names;
  if (arg.size() == 1)
    names = cntxt->getCurrentScope()->listAllVariables();
  else {
    for (int i=1;i<arg.size();i++) {
      Array varName(arg[i]);
      names.push_back(varName.getContentsAsCString());
    }
  }
  char header[116];
  time_t t = time(NULL);
  snprintf(header, 116, "MATLAB 5.0 MAT-file, Created on: %s by %s",
	   ctime(&t), Interpreter::getVersionString().c_str());
  m.putHeader(header);
  for (int i=0;i<names.size();i++) {
    Array *toWrite = cntxt->lookupVariable(names[i]);
    if (toWrite)
      m.putArray(*toWrite,names[i],cntxt->isVariableGlobal(names[i]));
    else
      eval->warningMessage(string("variable ") + names[i] + " does not exist to save");
  }
  return ArrayVector();
}

