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
    void* dp;

    try {
      ptr = malloc(count+sizeof(int));
      if (!ptr) throw Exception("Memory allocation error... You may have run out of memory!");
      *((int *) ptr) = count;
      memoryBalance += count;
      dp = (void*) ((char*) ptr + sizeof(int));
      memset(dp,0,count);
      return dp;
    } catch (...) {
      throw Exception("Memory allocation error... You may have run out of memory!");
    }
  }

  void Free(void *dp) {
    if (dp == NULL) return;
    int* sp = (int*) dp;
    sp--;
    memoryBalance -= *sp;
    free(sp);
  }

  void* Calloc(int count) {
    void* dp;
    dp = Malloc(count);
    memset(dp,0,count);
    return dp;
  }
}
