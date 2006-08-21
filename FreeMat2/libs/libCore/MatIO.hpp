#ifndef __MatIO_hpp__
#define __MatIO_hpp__

#include "Array.hpp"
#include <string>
#include <zlib.h>

using namespace std;

class MatIO {
public:
  enum openMode {
    readMode = 0,
    writeMode
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
public:
  uint32 BytesToAlign64Bit();
  void putAtom(Array a);
  uint16 readUint16();
  uint32 readUint32();
  void readBytesRaw(char *ptr, int len);
  void DecompressBytes(uint32 bcount);
  void FreeDecompressor();
  void ReadCompressedBytes(void *dest, int toread);
  bool inEndianSwap() {return m_endianSwap;}
  bool inCompressionMode() {return m_compressed_data;}
  MatIO(string filename, MatIO::openMode mode);
  ~MatIO();
  Array getAtom(bool &ateof);
  string getHeader();
  string getArray(Array &dat);
  void putHeader(string txt);
  void putArray(Array dat, string name);
};

#endif
