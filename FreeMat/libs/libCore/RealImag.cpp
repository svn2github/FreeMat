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
#include "Algorithms.hpp"

//!
//@Module REAL Real Function
//@@Section ELEMENTARY
//@@Usage
//Returns the real part of the input array for all elements.  The 
//general syntax for its use is
//@[
//   y = real(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.  The output 
//is the same numerical type as the input, unless the input is @|complex|
//or @|dcomplex|.  For @|complex| inputs, the real part is a floating
//point array, so that the return type is @|float|.  For @|dcomplex|
//inputs, the real part is a double precision floating point array, so that
//the return type is @|double|.  The @|real| function does
//nothing to real and integer types.
//@@Example
//The following demonstrates the @|real| applied to a complex scalar.
//@<
//real(3+4*i)
//@>
//The @|real| function has no effect on real arguments:
//@<
//real([2,3,4])
//@>
//For a double-precision complex array,
//@<
//real([2.0+3.0*i,i])
//@>
//@@Tests
//@$exact#y1=real(x1)
//@@Signature
//function real RealFunction
//inputs x
//outputs x
//!
ArrayVector RealFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("real function requires 1 argument");
  Array arg0(arg[0]);
  if (arg0.isString() || (arg0.dataClass() == Bool)) 
    arg0 = arg0.toClass(Double);
  return ArrayVector(Real(arg0));
}

//!
//@Module IMAG Imaginary Function
//@@Section ELEMENTARY
//@@Usage
//Returns the imaginary part of the input array for all elements.  The 
//general syntax for its use is
//@[
//   y = imag(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.  The output 
//is the same numerical type as the input, unless the input is @|complex|
//or @|dcomplex|.  For @|complex| inputs, the imaginary part is a floating
//point array, so that the return type is @|float|.  For @|dcomplex|
//inputs, the imaginary part is a double precision floating point array, so that
//the return type is @|double|.  The @|imag| function returns zeros for 
//real and integer types.
//@@Example
//The following demonstrates @|imag| applied to a complex scalar.
//@<
//imag(3+4*i)
//@>
//The imaginary part of real and integer arguments is a vector of zeros, the
//same type and size of the argument.
//@<
//imag([2,4,5,6])
//@>
//For a double-precision complex array,
//@<
//imag([2.0+3.0*i,i])
//@>
//@@Tests
//@$exact#y1=imag(x1)
//@@Signature
//function imag ImagFunction
//inputs x
//outputs y
//!
ArrayVector ImagFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("imag function requires 1 argument");
  Array arg0(arg[0]); 
  if (arg0.isString() || (arg0.dataClass() == Bool))
    arg0 = arg0.toClass(Double);
  arg0.forceComplex();
  return ArrayVector(Imag(arg0));
}
