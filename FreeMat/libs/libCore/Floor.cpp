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
#include "Array.hpp"
#include <cmath>

struct OpFloor {
  static inline float func(float t) { return floorf(t); }
  static inline double func(double t) {return floor(t); }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = floorf(x); ry = floorf(y);
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = floor(x); ry = floor(y);
  }
};

//!
//@Module FLOOR Floor Function
//@@Section ELEMENTARY
//@@Usage
//Computes the floor of an n-dimensional array elementwise.  The
//floor of a number is defined as the smallest integer that is
//less than or equal to that number. The general syntax for its use
//is
//@[
//   y = floor(x)
//@]
//where @|x| is a multidimensional array of numerical type.  The @|floor| 
//function preserves the type of the argument.  So integer arguments 
//are not modified, and @|float| arrays return @|float| arrays as 
//outputs, and similarly for @|double| arrays.  The @|floor| function 
//is not defined for complex types.
//@@Example
//The following demonstrates the @|floor| function applied to various
//(numerical) arguments.  For integer arguments, the floor function has
//no effect:
//@<
//floor(3)
//floor(-3)
//@>
//Next, we take the @|floor| of a floating point value:
//@<
//floor(3.023)
//floor(-2.341)
//@>
//@@Tests
//@$exact#y1=floor(x1)
//@@Signature
//function floor FloorFunction jitsafe
//input x
//output y
//!
ArrayVector FloorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("floor requires one argument");
  return ArrayVector(UnaryOp<OpFloor>(arg[0]));
}

JitScalarFunc1(floor,OpFloor::func);

struct OpFix {
  static inline float func(float t) { 
    if (t>0)
      return floorf(t);
    else if (t<0)
      return ceilf(t);
    else
      return t;
  }
  static inline double func(double t) {
    if (t>0)
      return floor(t);
    else if (t<0)
      return ceil(t);
    else
      return t;
  }
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = OpFix::func(x);
    ry = OpFix::func(y);
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = OpFix::func(x);
    ry = OpFix::func(y);
  }
};

JitScalarFunc1(fix,OpFix::func);

//!
//@Module FIX Round Towards Zero
//@@Section MATHFUNCTIONS
//@@Usage
//Rounds the argument array towards zero.  The syntax for its use is
//@[
//   y = fix(x)
//@]
//where @|x| is a numeric array.  For positive elements of @|x|, the output
//is the largest integer smaller than @|x|.  For negative elements of @|x|
//the output is the smallest integer larger than @|x|.  For complex @|x|,
//the operation is applied seperately to the real and imaginary parts.
//@@Example
//Here is a simple example of the @|fix| operation on some values
//@<
//a = [-1.8,pi,8,-pi,-0.001,2.3+0.3i]
//fix(a)
//@>
//@@Tests
//@$exact#y1=fix(x1)
//@@Signature
//function fix FixFunction
//input x
//output y
//!
ArrayVector FixFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("fix requires one argument");
  return ArrayVector(UnaryOp<OpFix>(arg[0]));
}
