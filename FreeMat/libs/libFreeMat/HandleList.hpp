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

#ifndef __HandleList_hpp__
#define __HandleList_hpp__
#include <map>

namespace FreeMat {
  template<class T>
  class HandleList {
    typedef T value_type;
    std::map<unsigned int, T, std::less<unsigned int> > handles;
    unsigned int max_handle;
  public:
    HandleList() : max_handle(0) {
    }
    ~HandleList() {
    }

    unsigned int maxHandle() {
      return max_handle;
    }

    unsigned int assignHandle(T val) {
      int nxt = 0;
      bool freeHandleFound = false;
      while ((nxt < max_handle) && !freeHandleFound) {
	freeHandleFound = (handles.count(nxt) == 0);
	if (!freeHandleFound) nxt++;
      }
      handles[nxt] = val;
      if (nxt >= max_handle) max_handle++;
      return nxt+1;
    }

    T lookupHandle(unsigned int handle) {
      if (handles.count(handle-1) == 0)
	throw Exception("Invalid handle!");
      return handles[handle-1];
    }

    void deleteHandle(unsigned int handle) {
      if ((handle-1) == max_handle) 
	max_handle--;
      handles.erase(handle-1);
    }
  };
}
#endif
