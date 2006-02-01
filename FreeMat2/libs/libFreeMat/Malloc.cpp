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

#include "Malloc.hpp"
#include "Exception.hpp"
#include <stdlib.h>
#include <stdio.h>

namespace FreeMat {

  static int memoryBalance = 0;

  void* Malloc(int count) {
    void* ptr;
    try {
      ptr = malloc(count);
      if (!ptr) throw Exception("Memory allocation error... You may have run out of memory!");
      memset(ptr,0,count);
      return ptr;
    } catch (...) {
      throw Exception("Memory allocation error... You may have run out of memory!");
    }
  }

  void Free(void *dp) {
    if (dp == NULL) return;
    free(dp);
  }

  void* Calloc(int count) {
    return Malloc(count);
  }
}
