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
//@Module RESHAPE Reshape An Array
//@@Section ARRAY
//@@Usage
//Reshapes an array from one size to another. Two seperate 
//syntaxes are possible.  The first syntax specifies the array 
//dimensions as a sequence of scalar dimensions:
//@[
//   y = reshape(x,d1,d2,...,dn).
//@]
//The resulting array has the given dimensions, and is filled with
//the contents of @|x|.  The type of @|y| is the same as @|x|.
//
//As a special case, you can specify exactly one of the dimensions
//as an empty matrix @|[]|, in which case FreeMat will compute the
//size required in that dimension to make the reshape work.  The
//syntax for this version is
//@[
//   y = reshape(x,d1,...,da,[],db,...,dn)
//@]
//
//The second syntax specifies the array dimensions as a vector,
//where each element in the vector specifies a dimension length:
//@[
//   y = reshape(x,[d1,d2,...,dn]).
//@]
//This syntax is more convenient for calling @|reshape| using a 
//variable for the argument. The
//@|reshape| function requires that the length of @|x| equal the product
//of the @|di| values.
//Note that arrays are stored in column format, 
//which means that elements in @|x| are transferred to the new array
//@|y| starting with the first column first element, then proceeding to 
//the last element of the first column, then the first element of the
//second column, etc.
//@@Example
//Here are several examples of the use of @|reshape| applied to
//various arrays.  The first example reshapes a row vector into a 
//matrix.
//@<
//a = uint8(1:6)
//reshape(a,2,3)
//@>
//The second example reshapes a longer row vector into a volume with 
//two planes.
//@<
//a = uint8(1:12)
//reshape(a,[2,3,2])
//@>
//The third example reshapes a matrix into another matrix.
//@<
//a = [1,6,7;3,4,2]
//reshape(a,3,2)
//@>
//@@Tests
//@{ test_reshape1.m
//function test_val = test_reshape1
//  a = [1,6,7;3,4,2]; 
//  b = [1,4;3,7;6,2];
//  c = reshape(a,3,2);
//  test_val = issame(b,c);
//@} 
//@{ test_reshape2.m
//function test_val = test_reshape2
//  a = rand(3,4);
//  b = reshape(a,6,2);
//  c = reshape(a,6,[]);
//  d = reshape(a,[],2);
//  test_val = issame(b,c) && issame(b,d);
//@}
//@@Signature
//function reshape ReshapeFunction jitsafe
//inputs x varargin
//outputs y
//!
ArrayVector ReshapeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("reshape function requires at least one argument");
  if (arg.size() == 1) return arg;
  ArrayVector arg_copy(arg);
  arg_copy.pop_front();
  // Check for the special case of exactly one of the dimensions being
  // empty.
  int empty_count = 0;
  int empty_slot = 0;
  for (int i=0;i<arg_copy.size();i++)
    if (arg_copy[i].isEmpty()) 
      {
	empty_count++;
	empty_slot = i;
      }
  if (empty_count > 1)
    throw Exception("reshape can take exactly one empty dimension argument");
  if (empty_count == 1)
    arg_copy[empty_slot] = Array(int(1));
  NTuple dims(ArrayVectorAsDimensions(arg_copy));
  if (empty_count == 1)
    {
      int A_count = arg[0].length();
      int Desired_count = dims.count();
      // Check to see if A_count is a multiple of Desired_count
      if (A_count % Desired_count != 0)
	throw Exception("reshape cannot be done");
      dims[empty_slot] = int(A_count/Desired_count);
    }
  Array y(arg[0]);
  y.reshape(dims);
  return ArrayVector(y);
}
