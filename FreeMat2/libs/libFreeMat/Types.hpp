// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#ifndef __Types_hpp__
#define __Types_hpp__

#include <string>
#include <vector>

namespace FreeMat {
  typedef unsigned char  logical;
  typedef char           int8;
  typedef unsigned char  uint8;
  typedef short          int16;
  typedef unsigned short uint16;
  typedef int            int32;
  typedef unsigned int   uint32;
  typedef unsigned int   indexType;
  typedef const indexType * constIndexPtr;

  typedef enum {
    FM_FUNCPTR_ARRAY,
    FM_CELL_ARRAY,
    FM_STRUCT_ARRAY,
    FM_LOGICAL,
    FM_UINT8,
    FM_INT8,
    FM_UINT16,
    FM_INT16,
    FM_UINT32,
    FM_INT32,
    FM_FLOAT,
    FM_DOUBLE,
    FM_COMPLEX,
    FM_DCOMPLEX,
    FM_STRING,
  } Class;
  typedef std::vector<std::string> stringVector;
}
#endif
