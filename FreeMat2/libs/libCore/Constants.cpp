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
  //!
  //@Module INF Infinity Constant
  //@@Usage
  //Returns a value that represents positive infinity 
  //for both 32 and 64-bit floating point values.
  //@[
  //   y = inf
  //@]
  //The returned type is a 32-bit float, but promotion to
  //64 bits preserves the infinity.
  //@@Function Internals
  //The infinity constant has
  //several interesting properties.  In particular:
  //\[
  //\begin{array}{ll}
  //   \infty \times 0 & = \mathrm{NaN} \\
  //   \infty \times a & = \infty \, \mathrm{for all} \, a > 0 \\
  //   \infty \times a & = -\infty \, \mathrm{for all} \, a < 0 \\
  //   \infty / \infty & = \mathrm{NaN} \\
  //   \infty / 0 & = \infty 
  //\end{array}
  //\]
  //Note that infinities are not preserved under type conversion to integer types (see the examples below).
  //@@Example
  //The following examples demonstrate the various properties of the infinity constant.
  //@<
  //inf*0
  //inf*2
  //inf*-2
  //inf/inf
  //inf/0
  //inf/nan
  //@>
  //Note that infinities are preserved under type conversion to floating point types (i.e., @|float|, @|double|, @|complex| and @|dcomplex| types), but not integer  types.
  //@<
  //uint32(inf)
  //complex(inf)
  //@>
  //!
  ArrayVector InfFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::floatConstructor(atof("inf")));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module NAN Not-a-Number Constant
  //@@Usage
  //Returns a value that represents ``not-a-number'' for both 32 and 64-bit 
  //floating point values.  This constant is meant to represent the result of
  //arithmetic operations whose output cannot be meaningfully defined (like 
  //zero divided by zero).
  //@[
  //   y = nan
  //@]
  //The returned type is a 32-bit float, but promotion to 64 bits preserves the not-a-number.  The not-a-number constant has one simple property.  In particular, any arithmetic operation with a @|NaN| results in a @|NaN|. These calculations run significantly slower than calculations involving finite quantities!  Make sure that you use @|NaN|s in extreme circumstances only.  Note that @|NaN| is not preserved under type conversion to integer types (see the examples below).
  //@@Example
  //The following examples demonstrate a few calculations with the not-a-number constant.
  //@<
  //nan*0
  //nan-nan
  //@>
  //Note that @|NaN|s are preserved under type conversion to floating point types (i.e., @|float|, @|double|, @|complex| and @|dcomplex| types), but not integer  types.
  //@<
  //uint32(nan)
  //complex(nan)
  //@>
  //!
  ArrayVector NaNFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::floatConstructor(atof("nan")));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }
  
  //!
  //@Module I-J Square Root of Negative One
  //@@Usage
  //Returns a @|complex| value that represents the square root of -1.  There are two
  //functions that return the same value:
  //@[
  //   y = i
  //@]
  //and 
  //@[
  //   y = j.
  //@]
  //This allows either @|i| or @|j| to be used as loop indices.  The returned value is a 32-bit complex value.
  //@@Example
  //The following examples demonstrate a few calculations with @|i|.
  //@<
  //i
  //i^2
  //@>
  //The same calculations with @|j|:
  //@<
  //j
  //j^2
  //@>
  //Here is an example of how @|i| can be used as a loop index and then recovered as the square root of -1.
  //@<
  //accum = 0; for i=1:100; accum = accum + i; end; accum
  //i
  //clear i
  //i
  //@>
  //!
  ArrayVector IFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::complexConstructor(0.0,1.0));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

  //!
  //@Module PI Constant Pi
  //@@Usage
  //Returns a @|double| (64-bit floating point number) value that represents pi (ratio between the circumference and diameter of a circle...).  Typical usage 
  //@[
  //   y = pi
  //@]
  //This value is approximately 3.141592653589793.
  //@@Example
  //The following example demonstrates the use of the @|pi| function.
  //@<
  //pi
  //cos(pi)
  //@>
  //!
  ArrayVector PiFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::doubleConstructor(4.0*atan(1.0)));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }  

  //!
  //@Module E Euler Constant (Base of Natural Logarithm)
  //@@Usage
  //Returns a @|double| (64-bit floating point number) value that represents Euler's constant, the base of the natural logarithm.  Typical usage 
  //@[
  //   y = e
  //@]
  //This value is approximately 2.718281828459045.
  //@@Example
  //The following example demonstrates the use of the @|e| function.
  //@<
  //e
  //log(e)
  //@>
  //!
  ArrayVector EFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::doubleConstructor(exp(1.0)));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }  
}
