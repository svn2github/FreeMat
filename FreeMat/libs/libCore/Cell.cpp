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
//@Module CELL Cell Array of Empty Matrices
//@@Section ARRAY
//@@Usage
//Creates a cell array of empty matrix entres.  Two seperate 
//syntaxes are possible.  The first syntax specifies the array 
//dimensions as a sequence of scalar dimensions:
//@[
//   y = cell(d1,d2,...,dn).
//@]
//The resulting array has the given dimensions, and is filled with
//all zeros.  The type of @|y| is @|cell|, a cell array.  
//    
//The second syntax specifies the array dimensions as a vector,
//where each element in the vector specifies a dimension length:
//@[
//   y = cell([d1,d2,...,dn]).
//@]
//This syntax is more convenient for calling @|zeros| using a 
//variable for the argument.  In both cases, specifying only one
//dimension results in a square matrix output.
//@@Example
//The following examples demonstrate generation of some zero arrays 
//using the first form.
//@<
//cell(2,3,2)
//cell(1,3)
//@>
//The same expressions, using the second form.
//@<
//cell([2,6])
//cell([1,3])
//@>
//@@Tests
//@{ test_cell1.m
//function test_val = test_cell1
//  A = cell(1,2,4);
//  B = [1 2 4];
//  test_val = issame(size(A),B);
//@}
//@@Signature
//function cell CellFunction
//inputs varargin
//outputs y
//!
ArrayVector CellFunction(int nargout, const ArrayVector& arg) {
  Array p(CellArray,ArrayVectorAsDimensions(arg));
  BasicArray<Array> &dp(p.real<Array>());
  for (index_t i=1;i<=dp.length();i++) {
    dp[i] = Array(Double);
  }
  return ArrayVector(p);
}
