#ifndef __MatIO_hpp__
#define __MatIO_hpp__

#include "Array.hpp"
#include <string>

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

  void putAtom(Array a);
  uint16 readUint16();
  uint32 readUint32();
  void readBytesRaw(char *ptr, int len);


public:
  MatIO(string filename, MatIO::openMode mode);
  ~MatIO();
  Array getAtom();
  string getHeader();
  string getArray(Array &dat);
  void putHeader(string txt);
  void putArray(Array dat, string name);
};

#endif
