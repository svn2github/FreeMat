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

#ifndef __Module_hpp__
#define __Module_hpp__

#include "Scope.hpp"

namespace FreeMat {
  /**
   * Load a function dynamically.  Throws an Exception if 
   * there are not at least two arguments to the function,
   * if the file could not be found, or if the requested
   * symbol is not present.
   */
  ArrayVector LoadFunction(int nargout,const ArrayVector& arg) throw(Exception);
  /**
   * Make a CCall - this is a special call into a library
   * that allows us to bypass the need to compile against 
   * the FreeMat libraries or source.  A good "raw" interface
   * for low-level codes.
   */
  void CCallFunction(const Array& libName,const Array& funcName,ArrayVector& funcValues) 
    throw(Exception);

  /**
   * import a C function 
   */
  ArrayVector ImportFunction(int nargout, const ArrayVector& arg, 
			     WalkTree* eval) throw(Exception);
}

#endif
