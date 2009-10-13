/*
 * Copyright (c) 2009 Samit Basu
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

#include "Operators.hpp"
#include "Complex.hpp"
#include "Math.hpp"
#include <cmath>

struct OpAbs {
  static inline float func(float t) {return fabsf(t);}
  static inline double func(double t) {return fabs(t);}
  template <typename T>
  static inline void func(T x, T y, T &rx, T &ry) {
    rx = complex_abs(x,y); ry = 0;
  }
};

//!
//@Module ABS Absolute Value Function
//@@Section ELEMENTARY
//@@Usage
//Returns the absolute value of the input array for all elements.  The 
//general syntax for its use is
//@[
//   y = abs(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.  The output 
//is the same numerical type as the input, unless the input is @|complex|
//or @|dcomplex|.  For @|complex| inputs, the absolute value is a floating
//point array, so that the return type is @|float|.  For @|dcomplex|
//inputs, the absolute value is a double precision floating point array, so that
//the return type is @|double|.
//@@Example
//The following demonstrates the @|abs| applied to a complex scalar.
//@<
//abs(3+4*i)
//@>
//The @|abs| function applied to integer and real values:
//@<
//abs([-2,3,-4,5])
//@>
//For a double-precision complex array,
//@<
//abs([2.0+3.0*i,i])
//@>
//@@Tests
//@$exact#y1=abs(x1)
//@@Signature
//function abs AbsFunction
//inputs x
//outputs y
//!
ArrayVector AbsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("abs function requires 1 argument");
  return ArrayVector(Real(UnaryOp<OpAbs>(arg[0])));
}
