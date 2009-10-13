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

//!
//@Module SIZE Size of a Variable
//@@Section INSPECTION
//@@Usage
//Returns the size of a variable.  There are two syntaxes for its
//use.  The first syntax returns the size of the array as a vector
//of integers, one integer for each dimension
//@[
//  [d1,d2,...,dn] = size(x)
//@]
//The other format returns the size of @|x| along a particular
//dimension:
//@[
//  d = size(x,n)
//@]
//where @|n| is the dimension along which to return the size.
//@@Example
//@<
//a = randn(23,12,5);
//size(a)
//@>
//Here is an example of the second form of @|size|.
//@<
//size(a,2)
//@>
//@@Tests
//@{ test_size1.m
//% Check the size function with an n-dim argument & one output
//function test_val = test_size1
//a = [];
//a(3,7,2) = 1.0;
//c = size(a);
//test_val = test(c(1) == 3) & test(c(2) == 7) & test(c(3) == 2);
//@}
//@{ test_size2.m
//% Check the size function with an n-dim argument & multiple outputs
//function test_val = test_size2
//a = [1,2,3;4,5,6];
//[c1,c2,c3] = size(a);
//test_val = test(c1 == 2) & test(c2 == 3) & test(c3 == 1);
//@}
//@{ test_size3.m
//% Check the size function with two arguments and one output
//function test_val = test_size3
//a = [1,2,3;4,5,6];
//n = size(a,2);
//test_val = test(n == 3);
//@}
//@{ test_size4.m
//% Check the size function with two arguments and two outputs
//function test_val = test_size4
//a = [1,2,3;4,5,6];
//test_val = 0;
//[c,d] = size(a,2);
//test_val = (c == 3);
//@}
//@$exact#y1=size(x1)
//@@Signature
//function size SizeFunction
//inputs x dim
//outputs varargout
//!
ArrayVector SizeFunction(int nargout, const ArrayVector& arg) {
  ArrayVector retval;
  if (arg.size() < 1)
    throw Exception("size function requires either one or two arguments");
  NTuple sze(arg[0].dimensions());
  if (arg.size() == 1) {
    if (nargout > 1) {
      ArrayVector retval;
      for (int i=0;i<nargout;i++)
	retval.push_back(Array(double(sze.get(i))));
      return retval;
    } else {
      BasicArray<index_t> rp(NTuple(1,sze.lastNotOne()));
      for (int i=0;i<sze.lastNotOne();i++)
	rp.set(i+1,sze[i]);
      return ArrayVector(Array(rp));
    }
  }
  int dimval = arg[1].asInteger();
  if (dimval<1)
    throw Exception("illegal value for dimension argument in call to size function");
  return ArrayVector(Array(double(sze[dimval-1])));
}
