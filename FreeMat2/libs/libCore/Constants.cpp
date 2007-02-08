/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Array.hpp"
#include "LAPACK.hpp"
#include <math.h>

//!
//@Module INF Infinity Constant
//@@Section CONSTANTS
//@@Usage
//Returns a value that represents positive infinity 
//for both 32 and 64-bit floating point values.
//@[
//   y = inf
//@]
//The returned type is a 64-bit float, but demotion to
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
  union {
    float f;
    unsigned int i;
  } u;
  u.i = 0x7f800000;
  Array A(Array::doubleConstructor(u.f));
  ArrayVector retval;
  retval.push_back(A);
  return retval;
}

//!
//@Module NAN Not-a-Number Constant
//@@Section CONSTANTS
//@@Usage
//Returns a value that represents ``not-a-number'' for both 32 and 64-bit 
//floating point values.  This constant is meant to represent the result of
//arithmetic operations whose output cannot be meaningfully defined (like 
//zero divided by zero).
//@[
//   y = nan
//@]
//The returned type is a 64-bit float, but demotion to 32 bits preserves the not-a-number.  The not-a-number constant has one simple property.  In particular, any arithmetic operation with a @|NaN| results in a @|NaN|. These calculations run significantly slower than calculations involving finite quantities!  Make sure that you use @|NaN|s in extreme circumstances only.  Note that @|NaN| is not preserved under type conversion to integer types (see the examples below).
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
  union {
    float f;
    unsigned int i;
  } u;
  u.i = 0x7fC00000;
  Array A(Array::doubleConstructor(u.f));
  ArrayVector retval;
  retval.push_back(A);
  return retval;
}
  
//!
//@Module I-J Square Root of Negative One
//@@Section CONSTANTS
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
//@@Section CONSTANTS
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
//@@Section CONSTANTS
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

//!
//@Module EPS Double Precision Floating Point Relative Machine Precision Epsilon
//@@Section CONSTANTS
//@@Usage
//Returns @|eps|, which quantifies the relative machine precision
//of floating point numbers (a machine specific quantity).  The syntax
//for @|eps| is:
//@[
//   y = eps
//@]
//which returns @|eps| for @|double| precision values. For most
//typical processors, this value is approximately @|2^-52|, or 2.2204e-16.
//@@Example
//The following example demonstrates the use of the @|eps| function,
//and one of its numerical consequences.
//@<
//eps
//1.0+eps
//@>
//!
ArrayVector EpsFunction(int nargout, const ArrayVector& arg) {
  char CMACH = 'E';
  Array A(Array::doubleConstructor(dlamch_(&CMACH)));
  ArrayVector retval;
  retval.push_back(A);
  return retval;
}

//!
//@Module FEPS Single Precision Floating Point Relative Machine Precision Epsilon
//@@Section CONSTANTS
//@@Usage
//Returns @|feps|, which quantifies the relative machine precision
//of floating point numbers (a machine specific quantity).  The syntax
//for @|feps| is:
//@[
//   y = feps
//@]
//which returns @|feps| for @|single| precision values. For most
//typical processors, this value is approximately @|2^-24|, or 5.9604e-8.
//@@Example
//The following example demonstrates the use of the @|feps| function,
//and one of its numerical consequences.
//@<
//feps
//1.0f+eps
//@>
//!
ArrayVector FepsFunction(int nargout, const ArrayVector& arg) {
  char CMACH = 'E';
  Array A(Array::floatConstructor(slamch_(&CMACH)));
  ArrayVector retval;
  retval.push_back(A);
  return retval;
}

//!
//@Module TRUE Logical TRUE
//@@Section CONSTANTS
//@@Usage
//Returns a logical 1.  The syntax for its use is
//@[
//   y = true
//@]
//@@Tests
//@$"y=logical(1)","true","exact"
//@$"y=logical(0)","~true","exact"
//!
ArrayVector TrueFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector() << Array::logicalConstructor(1);
}

//!
//@Module FALSE Logical False
//@@Section CONSTANTS
//@@Usage
//Returns a logical 0.  The syntax for its use is
//@[
//   y = false
//@]
//@@Tests
//@$"y=logical(0)","false","exact"
//@$"y=logical(1)","~false","exact"
//!
ArrayVector FalseFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector() << Array::logicalConstructor(0);
}
