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
//@Module ONES Array of Ones
//@@Section ARRAY
//@@Usage
//Creates an array of ones of the specified size.  Two seperate 
//syntaxes are possible.  The first syntax specifies the array 
//dimensions as a sequence of scalar dimensions:
//@[
//   y = ones(d1,d2,...,dn).
//@]
//The resulting array has the given dimensions, and is filled with
//all ones.  The type of @|y| is @|float|, a 32-bit floating
//point array.  To get arrays of other types, use the typecast 
//functions (e.g., @|uint8|, @|int8|, etc.).
//    
//The second syntax specifies the array dimensions as a vector,
//where each element in the vector specifies a dimension length:
//@[
//   y = ones([d1,d2,...,dn]).
//@]
//This syntax is more convenient for calling @|ones| using a 
//variable for the argument.  In both cases, specifying only one
//dimension results in a square matrix output.
//@@Example
//The following examples demonstrate generation of some arrays of ones
//using the first form.
//@<
//ones(2,3,2)
//ones(1,3)
//@>
//The same expressions, using the second form.
//@<
//ones([2,6])
//ones([1,3])
//@>
//Finally, an example of using the type casting function @|uint16| to generate an array of 16-bit unsigned integers with a value of 1.
//@<
//uint16(ones(3))
//@>
//@@Tests
//@{ test_ones1.m
//function test_val = test_ones1
//  test_val = issame(ones(3),zeros(3)+1);
//@}
//@@Signature
//function ones OnesFunction
//inputs varargin
//outputs x
//!
ArrayVector OnesFunction(int nargout, const ArrayVector& arg) {
  NTuple dims(ArrayVectorAsDimensions(arg));
  Array r(Double,dims);
  BasicArray<double> &rp(r.real<double>());
  for (index_t i=1;i<=rp.length();i++) rp[i] = 1;
  return ArrayVector(r);
}
