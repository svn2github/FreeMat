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

#include "Array.hpp"
#include "Utils.hpp"


//!
//@Module INF Infinity Constant
//@@Section CONSTANTS
//@@Usage
//Returns a value that represents positive infinity 
//for both 32 and 64-bit floating point values. 
//There are several forms for the @|Inf| function.
//The first form returns a double precision @|Inf|.
//@[
//   y = inf
//@]
//The next form takes a class name that can be either @|'double'| 
//@[
//   y = inf('double')
//@]
//or @|'single'|:
//@[
//   y = inf('single')
//@]
//With a single parameter it generates a square matrix of @|inf|s.
//@[
//   y = inf(n)
//@]
//Alternatively, you can specify the dimensions of the array via
//@[
//   y = inf(m,n,p,...)
//@]
//or
//@[
//   y = inf([m,n,p,...])
//@]
//Finally, you can add a classname of either @|'single'| or @|'double'|.
//@@Function Internals
//The infinity constant has
//several interesting properties.  In particular:
//\[
//\begin{array}{ll}
//   \infty \times 0 & = \mathrm{NaN} \\                                             \infty \times a & = \infty \, \mathrm{for all} \, a > 0 \\   \infty \times a & = -\infty \, \mathrm{for all} \, a < 0 \\   \infty / \infty & = \mathrm{NaN} \\   \infty / 0 & = \infty 
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
//@@Tests
//@$exact#y1=inf
//@@Signature
//function inf InfFunction
//inputs varargin
//outputs y
//@@Signature
//function Inf InfFunction
//inputs varargin
//outputs y
//!
ArrayVector InfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector(Array(Inf()));
  // Trim out the classname if it was specified
  DataClass dataclass = Double;
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    if (trim_arg.back().isString()) {
      // Check for the classname
      dataclass = Invalid;
      // Get the classname as a string
      QString cp = trim_arg.back().asString().toLower();
      if (cp == "single") dataclass = Float;
      if (cp == "double") dataclass = Double;
      if (cp == "float") dataclass = Float;
      if (dataclass == Invalid)
	throw Exception("unrecognized type for argument to nan function");
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  NTuple dims = ArrayVectorAsDimensions(trim_arg);
  Array p(dataclass,dims);
  if (dataclass == Float)
    p.real<float>().fill(float(Inf()));
  else
    p.real<double>().fill(Inf());
  return ArrayVector(p);
}

//!
//@Module NAN Not-a-Number Constant
//@@Section ARRAY
//@@Usage
//Returns a value that represents ``not-a-number'' for both 32 and 64-bit 
//floating point values.  This constant is meant to represent the result of
//arithmetic operations whose output cannot be meaningfully defined (like 
//zero divided by zero).  There are several forms for the @|NaN| function.
//The first form returns a double precision @|NaN|.
//@[
//   y = nan
//@]
//The next form takes a class name that can be either @|'double'| 
//@[
//   y = nan('double')
//@]
//or @|'single'|:
//@[
//   y = nan('single')
//@]
//With a single parameter it generates a square matrix of @|nan|s.
//@[
//   y = nan(n)
//@]
//Alternatively, you can specify the dimensions of the array via
//@[
//   y = nan(m,n,p,...)
//@]
//or
//@[
//   y = nan([m,n,p,...])
//@]
//Finally, you can add a classname of either @|'single'| or @|'double'|.
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
//@@Tests
//@$exact#y1=nan
//@@Signature
//function nan NaNFunction
//inputs varargin
//outputs y
//@@Signature
//function NaN NaNFunction
//inputs varargin
//outputs y
//!
ArrayVector NaNFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector(Array(NaN()));
  // Trim out the classname if it was specified
  DataClass dataclass = Double;
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    if (trim_arg.back().isString()) {
      // Check for the classname
      dataclass = Invalid;
      // Get the classname as a string
      QString cp = trim_arg.back().asString().toLower();
      if (cp == "single") dataclass = Float;
      if (cp == "double") dataclass = Double;
      if (cp == "float") dataclass = Float;
      if (dataclass == Invalid)
	throw Exception("unrecognized type for argument to nan function");
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  NTuple dims = ArrayVectorAsDimensions(trim_arg);
  Array p(dataclass,dims);
  if (dataclass == Float)
    p.real<float>().fill(float(NaN()));
  else
    p.real<double>().fill(NaN());
  return ArrayVector(p);
}

//!
//@Module ZEROS Array of Zeros
//@@Section ARRAY
//@@Usage
//Creates an array of zeros of the specified size.  Two seperate 
//syntaxes are possible.  The first syntax specifies the array 
//dimensions as a sequence of scalar dimensions:
//@[
//   y = zeros(d1,d2,...,dn).
//@]
//The resulting array has the given dimensions, and is filled with
//all zeros.  The type of @|y| is @|double|, a 64-bit floating
//point array.  To get arrays of other types, use the typecast 
//functions (e.g., @|uint8|, @|int8|, etc.).  An alternative syntax
//is to use the following notation:
//@[
//   y = zeros(d1,d2,...,dn,classname)
//@]
//where @|classname| is one of 'double', 'single', 'int8', 'uint8',
//'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64', 'float', 'logical'.  
//    
//The second syntax specifies the array dimensions as a vector,
//where each element in the vector specifies a dimension length:
//@[
//   y = zeros([d1,d2,...,dn]),
//@]
//or
//@[
//   y = zeros([d1,d2,...,dn],classname).
//@]
//This syntax is more convenient for calling @|zeros| using a 
//variable for the argument.  In both cases, specifying only one
//dimension results in a square matrix output.
//@@Example
//The following examples demonstrate generation of some zero arrays 
//using the first form.
//@<
//zeros(2,3,2)
//zeros(1,3)
//@>
//The same expressions, using the second form.
//@<
//zeros([2,6])
//zeros([1,3])
//@>
//Finally, an example of using the type casting function @|uint16| to generate an array of 16-bit unsigned integers with zero values.
//@<
//uint16(zeros(3))
//@>
//Here we use the second syntax where the class of the output is specified 
//explicitly
//@<
//zeros(3,'int16')
//@>
//@@Tests
//@{ test_sparse69.m
//% Test the zeros function
//function x = test_sparse69
//xi = int32(sparse(100,200));
//yi = int32(zeros(100,200));
//xf = float(sparse(100,200));
//yf = float(zeros(100,200));
//xd = double(sparse(100,200));
//yd = double(zeros(100,200));
//xc = complex(sparse(100,200));
//yc = complex(zeros(100,200));
//xz = dcomplex(sparse(100,200));
//yz = dcomplex(zeros(100,200));
//x = testeq(xi,yi) & testeq(xf,yf) & testeq(xd,yd) & testeq(xc,yc) & testeq(xz,yz);
//@}
//@{ test_zeros1.m
//function test_val = test_zeros1
//  a = zeros(2,3,'single');
//  test_val = (size(a) == [2,3]) && strcmp(typeof(a),'single');
//@}
//@@Signature
//function zeros ZerosFunction
//inputs varargin
//outputs y
//!

#define MacroZeros(ctype,cls) \
  if (cp == #ctype) dataclass = cls;

ArrayVector ZerosFunction(int nargout, const ArrayVector& arg) {
  // Trim out the classname if it was specified
  DataClass dataclass = Double;
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    if (trim_arg.back().isString()) {
      // Check for the classname
      dataclass = Invalid;
      // Get the classname as a string
      QString cp = trim_arg.back().asString().toLower();
      MacroExpandCasesSimple(MacroZeros);
      if (cp == "single") dataclass = Float;
      if (cp == "logical") dataclass = Bool;
      if (dataclass == Invalid)
	throw Exception("unrecognized type for argument to zeros function");
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  NTuple dims = ArrayVectorAsDimensions(trim_arg);
  return ArrayVector(Array(dataclass,dims));
}

#undef MacroZeros
