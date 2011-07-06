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
#include "IEEEFP.hpp"
#include <math.h>
#include <float.h>
#include "Operators.hpp"

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
//@@Tests
//@$exact#y1=i
//@$exact#y1=j
//@@Signature
//function i IFunction
//inputs none
//outputs y
//@@Signature
//function j IFunction
//inputs none
//outputs y
//!
ArrayVector IFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(double(0),double(1)));
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
//@@Tests
//@$exact#y1=pi
//@@Signature
//function pi PiFunction jitsafe
//inputs none
//outputs y
//!
ArrayVector PiFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(double(4.0*atan(1.0))));
}  

JitScalarFunc0(pi,double(4.0*atan(1.0)));

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
//@@Tests
//@$exact#y1=e
//@@Signature
//function e EFunction jitsafe
//inputs none
//outputs y
//!
ArrayVector EFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(exp(1.0)));
}  

JitScalarFunc0(e,double(exp(1.0)));

struct OpEps {
  static inline float func(float t) {
    if (IsInfinite(t)) return NaN();
    return fepsf(t);
  }
  static inline double func(double t) {
    if (IsInfinite(t)) return NaN();
    return feps(t);
  }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = qMax(fepsf(x),fepsf(y)); 
    ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = qMax(feps(x),feps(y)); 
    ry = 0;
  }
};

//!
//@Module EPS Double Precision Floating Point Relative Machine Precision Epsilon
//@@Section CONSTANTS
//@@Usage
//Returns @|eps|, which quantifies the relative machine precision
//of floating point numbers (a machine specific quantity).  The syntax
//for @|eps| is:
//@[
//   y = eps
//   y = eps('double')
//   y = eps(X)
//@]
//First form returns @|eps| for @|double| precision values. For most
//typical processors, this value is approximately @|2^-52|, or 2.2204e-16.
//Second form return @|eps| for class @|double| or @|single|.
//Third form returns distance to the next value greater than X.
//@@Example
//The following example demonstrates the use of the @|eps| function,
//and one of its numerical consequences.
//@<
//eps
//1.0+eps
//eps(1000.)
//@>
//@{ test_eps1.m
//function test_val = test_eps1
//a = eps('double');
//b = eps(1.);
//c = eps('single');
//d = eps(single(1.));
//test_val = test(a==b && c==d);
//@}
//@@Tests
//@$exact#y1=eps
//@$exact#y1=eps('double')
//@$exact#y1=eps('single')
//@$exact#y1=eps(x1)
//@@Signature
//function eps EpsFunction jitsafe
//inputs varargin
//outputs y
//!
ArrayVector EpsFunction(int nargout, const ArrayVector& arg) {
  ArrayVector retval;
  
  if( arg.size()> 1 )
    throw Exception("eps takes no more than 1 argument");
  if( arg.size()==1 ){
    Array a( arg[0] );
    if( a.isString() ){
      QString str = a.asString().toLower();
      if( str == QString( "double" ) ){
	retval << Array( feps( 1. ) ); 
      }
      else if( str == QString( "single" ) ){
	retval << Array( fepsf( 1. ) );
      }
      else{
	throw Exception("Class must be 'double' or 'single'");
      }
    }
    else { //numeric argument
      return ArrayVector(Real(UnaryOp<OpEps>(a)));
    }
  } else{
    retval << Array( feps( 1. ) );
  }
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
//@@Signature
//function feps FepsFunction
//inputs none
//outputs y
//!
ArrayVector FepsFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(float(nextafterf(1.0,2.0)-1.0f)));
}

