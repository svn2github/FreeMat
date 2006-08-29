#ifndef __MatIO_hpp__
#define __MatIO_hpp__

#include "Array.hpp"
#include <string>
#include <zlib.h>

using namespace std;

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


class MatIO {
public:
  enum openMode {
    readMode = 0,
    writeMode
  };

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

private:
  FILE *m_fp;
  string m_filename;
  bool m_endianSwap;
  openMode m_mode;
  string m_txt;
  bool m_compressed_data;
  uint8* m_compression_buffer;
  z_streamp zstream;
  uint32 m_writecount;
  bool m_phantomWriteMode;
private:
  // Reads various types of arrays
  Array getSparseArray(Dimensions dm, bool complexFlag);
  Array getNumericArray(mxArrayTypes arrayType, Dimensions dm, bool complexFlag);
  Array getClassArray(Dimensions dm);
  Array getStructArray(Dimensions dm);
  Array getCellArray(Dimensions dm);
  Array getDataElement();
  // Writes various types of arrays
  void putSparseArray(const Array &x);
  void putNumericArray(const Array &x);
  void putClassArray(const Array &x);
  void putStructArray(const Array &x);
  void putCellArray(const Array &x);
  void putDataElement(const Array &x);
  void putArraySpecific(const Array &x, Array aFlags, string name, mxArrayTypes arrayType);
  // Align us to the next 64 bit boundary.
  void Align64Bit();
  // Elementary read/write operations
  uint16 getUint16();
  uint32 getUint32();
  void putUint16(uint16 x);
  void putUint32(uint32 x);
  // Methods that control the decompression engine
  void InitializeDecompressor(uint32 bcount);
  void ReadCompressedBytes(void *dest, uint32 toread);
  void CloseDecompressor();
  // Methods that control the compression engine
  void InitializeCompressor();
  void WriteCompressedBytes(const void *dest, uint32 towrite);
  void CloseCompressor();
  // Read data directly from the file
  void ReadFileBytes(void *dest, uint32 toread);
  // Read data (selects decompression if necessary)
  void ReadData(void *dest, uint32 toread);
  // Write data directly to the file
  void WriteFileBytes(const void *dest, uint32 towrite);
  // Write data (selects compression if necessary)
  void WriteData(const void *dest, uint32 towrite);
public:
  // Constructor pairs
  MatIO(string filename, MatIO::openMode mode);
  ~MatIO();
  // Get/Put for arrays
  Array getArray(bool &atEof, string &name, bool &match, bool &isGlobal);
  void putArray(const Array &x, string name = string(), bool isGlobal = false);
  void putArrayCompressed(const Array &x, string name);
  // Header routines
  string getHeader();
  void putHeader(string header);
};

#endif
