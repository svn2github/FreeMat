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

#include "Array.hpp"
#include <math.h>

namespace FreeMat {
  ArrayVector InfFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::floatConstructor(atof("inf")));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  ArrayVector NaNFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::floatConstructor(atof("nan")));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }
  
  ArrayVector IFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::complexConstructor(0.0,1.0));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }
  
  ArrayVector PiFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::doubleConstructor(4.0*atan(1.0)));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }  

  ArrayVector EFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::doubleConstructor(exp(1.0)));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }  

}
