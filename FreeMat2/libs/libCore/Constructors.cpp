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
#include "Math.hpp"
#include "Exception.hpp"
#include <math.h>
#include <stdio.h>
#include "Core.hpp"
#include "Malloc.hpp"
#include <ctype.h>

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
//!
ArrayVector ReshapeFunction(int nargout, const ArrayVector& arg) {
  Array t, s;
  Dimensions dims;
  int32 *dp;
  int i;
  if (arg.size() == 0)
    throw Exception("reshape function requires at least one argument");
  Array x(arg[0]);
  if (arg.size() == 1) {
    ArrayVector retval;
    retval.push_back(x);
    return retval;
  }
  // Case 1 - all of the entries are scalar
  bool allScalars;
  allScalars = true;
  for (i=1;i<arg.size();i++)
    allScalars &= arg[i].isScalar();
  if (allScalars) {
    t = arg[1];
    if (arg.size() == 2) {
      // If all scalars and only one argument - we want a square output matrix
      dims.set(0,t.getContentsAsIntegerScalar());
      dims.set(1,dims.get(0));
    } else {
      // If all scalars and and multiple arguments, we count dimensions
      for (i=1;i<arg.size();i++) {
	t = arg[i];
	dims.set(i-1,t.getContentsAsIntegerScalar());
      }

    }
  } else {
    if (arg.size() > 2)
      throw Exception("Arguments to reshape function must be either all scalars or a single vector");
    t = arg[1];
    t.promoteType(FM_UINT32);
    dp = (int*) t.getDataPointer();
    for (i=0;i<t.getLength();i++)
      dims.set(i,dp[i]);
  }
  bool allPositive;
  allPositive = true;
  for (i=0;i<dims.getLength();i++)
    allPositive &= (dims.get(i) >= 0);
  if (!allPositive)
    throw Exception("reshape function requires positive arguments");
  x.reshape(dims);
  ArrayVector retval;
  retval.push_back(x);
  return retval;
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
//all zeros.  The type of @|y| is @|float|, a 32-bit floating
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
//!
ArrayVector ZerosFunction(int nargout, const ArrayVector& arg) {
  Array t, s;
  Dimensions dims;
  int32 *dp;
  Class cls = FM_FLOAT;
  int i;
  // Trim out the classname if it was specified
  ArrayVector trim_arg(arg);
  if (trim_arg.size() > 0) {
    // Check for the classname
    if (trim_arg.back().isString()) {
      // Get the classname as a string
      char *cp = strdup(ArrayToString(trim_arg.back()));
      // Convert to lowercase
      char *dp = cp;
      while (*dp) {
	*dp = tolower(*dp);
	dp++;
      }
      if (strcmp(cp,"double")==0)
	cls = FM_DOUBLE;
      else if (strcmp(cp,"single")==0)
	cls = FM_FLOAT;
      else if (strcmp(cp,"float")==0)
	cls = FM_FLOAT;
      else if (strcmp(cp,"int8")==0)
	cls = FM_INT8;
      else if (strcmp(cp,"uint8")==0)
	cls = FM_UINT8;
      else if (strcmp(cp,"int16")==0)
	cls = FM_INT16;
      else if (strcmp(cp,"uint16")==0)
	cls = FM_UINT16;
      else if (strcmp(cp,"int32")==0)
	cls = FM_INT32;
      else if (strcmp(cp,"uint32")==0)
	cls = FM_UINT32;
      else if (strcmp(cp,"int64")==0)
	cls = FM_INT64;
      else if (strcmp(cp,"uint64")==0)
	cls = FM_UINT64;
      else if (strcmp(cp,"logical")==0)
	cls = FM_LOGICAL;
      else
	throw Exception(std::string("Unsupported type ") + cp + std::string(" as classname argument to zeros function"));
      // Remove the classspec
      trim_arg.pop_back();
    }
  }
  if (trim_arg.size() == 0)
    dims.makeScalar();
  else {
    // Case 1 - all of the entries are scalar
    bool allScalars;
    allScalars = true;
    for (i=0;i<trim_arg.size();i++)
      allScalars &= trim_arg[i].isScalar();
    if (allScalars) {
      t = trim_arg[0];
      if (trim_arg.size() == 1) {
	// If all scalars and only one argument - we want a square zero matrix
	dims.set(0,t.getContentsAsIntegerScalar());
	dims.set(1,dims.get(0));
      } else {
	// If all scalars and and multiple arguments, we count dimensions
	for (i=0;i<trim_arg.size();i++) {
	  t = trim_arg[i];
	  dims.set(i,t.getContentsAsIntegerScalar());
	}
	  
      }
    } else {
      if (trim_arg.size() > 1)
	throw Exception("Arguments to zeros function must be either all scalars or a single vector");
      t = trim_arg[0];
      t.promoteType(FM_UINT32);
      dp = (int*) t.getDataPointer();
      for (i=0;i<t.getLength();i++)
	dims.set(i,dp[i]);
    }
    bool allPositive;
    allPositive = true;
    for (i=0;i<dims.getLength();i++)
      allPositive &= (dims.get(i) >= 0);
    if (!allPositive)
      throw Exception("Zeros function requires positive arguments");
  }
  s = Array(cls,dims,Calloc(TypeSize(cls)*dims.getElementCount()));
  ArrayVector retval;
  retval.push_back(s);
  return retval;
}

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
//!
ArrayVector CellFunction(int nargout, const ArrayVector& arg) {
  Array t, s;
  Dimensions dims;
  int32 *dp;
  int i;
  if (arg.size() == 0)
    dims.makeScalar();
  else {
    // Case 1 - all of the entries are scalar
    bool allScalars;
    allScalars = true;
    for (i=0;i<arg.size();i++)
      allScalars &= arg[i].isScalar();
    if (allScalars) {
      t = arg[0];
      if (arg.size() == 1) {
	// If all scalars and only one argument - we want a square zero matrix
	dims.set(0,t.getContentsAsIntegerScalar());
	dims.set(1,dims.get(0));
      } else {
	// If all scalars and and multiple arguments, we count dimensions
	for (i=0;i<arg.size();i++) {
	  t = arg[i];
	  dims.set(i,t.getContentsAsIntegerScalar());
	}
	  
      }
    } else {
      if (arg.size() > 1)
	throw Exception("Arguments to cell function must be either all scalars or a single vector");
      t = arg[0];
      t.promoteType(FM_UINT32);
      dp = (int*) t.getDataPointer();
      for (i=0;i<t.getLength();i++)
	dims.set(i,dp[i]);
    }
    bool allPositive;
    allPositive = true;
    for (i=0;i<dims.getLength();i++)
      allPositive &= (dims.get(i) >= 0);
    if (!allPositive)
      throw Exception("Zeros function requires positive arguments");
  }
  s = Array(FM_CELL_ARRAY,dims,Array::allocateArray(FM_CELL_ARRAY,dims.getElementCount()));
  ArrayVector retval;
  retval.push_back(s);
  return retval;
}

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
//!
ArrayVector OnesFunction(int nargout, const ArrayVector& arg) {
  Array t, s;
  Dimensions dims;
  int32 *dp;
  int i;
  if (arg.size() == 0)
    dims.makeScalar();
  else {
    // Case 1 - all of the entries are scalar
    bool allScalars;
    allScalars = true;
    for (i=0;i<arg.size();i++)
      allScalars &= arg[i].isScalar();
    if (allScalars) {
      t = arg[0];
      if (arg.size() == 1) {
	// If all scalars and only one argument - we want a square zero matrix
	dims.set(0,t.getContentsAsIntegerScalar());
	dims.set(1,dims.get(0));
      } else {
	// If all scalars and and multiple arguments, we count dimensions
	for (i=0;i<arg.size();i++) {
	  t = arg[i];
	  dims.set(i,t.getContentsAsIntegerScalar());
	}
	  
      }
    } else {
      if (arg.size() > 1)
	throw Exception("Arguments to ones function must be either all scalars or a single vector");
      t = arg[0];
      t.promoteType(FM_UINT32);
      dp = (int*) t.getDataPointer();
      for (i=0;i<t.getLength();i++)
	dims.set(i,dp[i]);
    }
    bool allPositive;
    allPositive = true;
    for (i=0;i<dims.getLength();i++)
      allPositive &= (dims.get(i) >= 0);
    if (!allPositive)
      throw Exception("Ones function requires positive arguments");
  }
  int len;
  len = dims.getElementCount();
  float *qp;
  qp = (float*) Malloc(sizeof(float)*len);
  for (i=0;i<len;i++)
    qp[i] = 1.0f;
  s = Array(FM_FLOAT,dims,qp);
  ArrayVector retval;
  retval.push_back(s);
  return retval;
}

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
//!
ArrayVector StructFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) {
    Array t(arg[0]);
    if (!t.isUserClass() && t.dataClass() == FM_STRUCT_ARRAY)
      return singleArrayVector(t);
    if (!t.isUserClass())
      throw Exception("can only convert objects (user-defined types) into structs");
    t.setClassName(rvstring());
    return singleArrayVector(t);
  }
  if (arg.size() % 2)
    throw Exception("struct function requires pairs of field names and values");
  int pairCount = arg.size() / 2;
  rvstring names;
  ArrayVector values;
  for (int i=0;i<pairCount;i++) values.push_back(Array());
  for (int i=0;i<pairCount*2;i+=2) {
    if (!(arg[i].isString()))
      throw Exception("struct function requires pairs of field names and values");
    char *c = (arg[i].getContentsAsCString());
    names.push_back(c);
    Free(c);
    values[i/2] = arg[i+1];
  }
  ArrayVector retval;
  retval.push_back(Array::structConstructor(names,values));
  return retval;
}
