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
#include "Struct.hpp"
#include "Algorithms.hpp"

//!
//@Module STRUCT Structure Array Constructor
//@@Section VARIABLES
//@@Usage
//Creates an array of structures from a set of field, value pairs.
//The syntax is
//@[
//   y = struct(n_1,v_1,n_2,v_2,...)
//@]
//where @|n_i| are the names of the fields in the structure array, and
//@|v_i| are the values.  The values @|v_i| must either all be
//scalars, or be cell-arrays of all the same dimensions.  In the latter 
//case, the
//output structure array will have dimensions dictated by this common
//size.  Scalar entries for the @|v_i| are replicated to fill out
//their dimensions. An error is raised if the inputs are not properly matched (i.e., are
//not pairs of field names and values), or if the size of any two non-scalar
//values cell-arrays are different.
//
//Another use of the @|struct| function is to convert a class into a 
//structure.  This allows you to access the members of the class, directly 
//but removes the class information from the object.
//
//@@Example
//This example creates a 3-element structure array with three fields, @|foo|
//@|bar| and @|key|, where the contents of @|foo| and @|bar| are provided 
//explicitly as cell arrays of the same size, and the contents of @|bar| 
//are replicated from a scalar.
//@<
//y = struct('foo',{1,3,4},'bar',{'cheese','cola','beer'},'key',508)
//y(1)
//y(2)
//y(3)
//@>
//
//An alternate way to create a structure array is to initialize the last
//element of each field of the structure
//@<
//Test(2,3).Type = 'Beer';
//Test(2,3).Ounces = 12;
//Test(2,3).Container = 'Can';
//Test(2,3)
//Test(1,1)
//@>
//@@Tests
//@{ test_struct1.m
//% Test the ability to combine structures with different ordering in their elements.
//function test_val = test_struct1
//a.foo = 1;
//a.goo = 2;
//b.goo = 5;
//b.foo = 4;
//c = [a,b];
//test_val = (c(1).foo == 1) & (c(2).foo == 4) & (c(1).goo == 2) & (c(2).foo == 4);
//@}
//@{ test_struct2.m
//% Test the ability to combine structues with different fields - when valid
//function test_val = test_struct2
//a.foo = 1;
//a.goo = 2;
//b.goo = 5;
//c = [a,b];
//test_val = (c(1).foo == 1) & (c(1).goo == 2) & (c(2).goo == 5);
//@}
//@{ test_struct3.m
//% Test the ability to add a new field name to a structure
//function test_val = test_struct3
//a.foo = 1;
//a.goo = 2;
//c(1) = a;
//c(2) = a;
//c(1).hoo = 6;
//test_val = (c(1).foo == 1) & (c(2).foo == 1) & (c(1).goo == 2) & (c(2).goo == 2) & (c(1).hoo == 6);
//@}
//@{ test_struct4.m
//% Test the structure constructor
//function test_val = test_struct4
//a = struct('foo',4,'goo',{5},'hoo',{'time',8});
//test_val = test(a(1).foo == 4) & test(a(2).foo == 4) ...
//    & test(a(1).goo == 5) & test(a(2).goo == 5) ...
//    & test(strcmp(a(1).hoo,'time')) & test(a(2).hoo == 8);
//@}
//@{ test_struct5.m
//% Test the field dereference as an expression list
//function test_val = test_struct5
//  a = struct('foo',{5,8,10});
//  sm = test_struct5_assist(a.foo);
//  test_val = test(sm == 23);
//
//function a = test_struct5_assist(b,c,d)
//  a = b + c + d;
//@}
//@{ test_struct6.m
//% Test the field set function with a scalar argument
//function test_val = test_struct6
//  a = struct('foo',5,'color','blue');
//  a.color = 'red';
//  test_val = test(strcmp(a.color,'red'));
//@}
//@{ test_struct7.m
//% Test the field set function with a vector argument
//function test_val = test_struct7
//  a = struct('foo',{6,5,3},'color','blue');
//  test_val = 0;
//  try
//    a.foo = 7;
//  catch
//    test_val = 1;
//  end
//@}
//@{ test_struct8.m
//% Test the structure field-based multiple assign in a function call
//function test_val = test_struct8
//a = struct('foo',{0,0,0,0});
//[a.foo] = test_struct8_assist;
//test_val = test(a(1).foo == 1) & test(a(2).foo == 2) & ...
//    test(a(3).foo == 3) & test(a(4).foo == 4);
//
//function [a,b,c,d] = test_struct8_assist
//  a = 1;
//  b = 2;
//  c = 3;
//  d = 4;
//@}
//@{ test_struct9.m
//% Test the ability to assign a single element in a structure array
//function test_val = test_struct9
//d = struct('foo',3,'goo','hello','shoo',1:5);
//d(3).shoo = 3;
//test_val = test(d(3).shoo == 3);
//@}
//@@Signature
//function struct StructFunction
//inputs varargin
//outputs struct
//!
ArrayVector StructFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) {
    Array t(arg[0]);
    if (!t.isUserClass() && t.dataClass() == Struct)
      return ArrayVector(t);
    if (!t.isUserClass())
      throw Exception("can only convert objects (user-defined types) into structs");
    t.structPtr().setClassPath(StringVector());
    return ArrayVector(t);
  }
  if (arg.size() % 2)
    throw Exception("struct function requires pairs of field names and values");
  int pairCount = arg.size() / 2;
  StringVector names;
  ArrayVector values;
  for (int i=0;i<pairCount;i++) 
    values.push_back(Array());
  for (int i=0;i<pairCount*2;i+=2) {
    if (!(arg[i].isString()))
      throw Exception("struct function requires pairs of field names and values");
    names.push_back(arg[i].asString());
    values[i/2] = arg[i+1];
  }
  return ArrayVector(StructConstructor(names,values));
}
