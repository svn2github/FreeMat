// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Array.hpp"
#include "Math.hpp"
#include "Exception.hpp"
#include <math.h>
#include <stdio.h>
#include "Core.hpp"
#include "Malloc.hpp"


namespace FreeMat {
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
	dims[0] = t.getContentsAsIntegerScalar();
	dims[1] = dims[0];
      } else {
	// If all scalars and and multiple arguments, we count dimensions
	for (i=1;i<arg.size();i++) {
	  t = arg[i];
	  dims[i-1] = t.getContentsAsIntegerScalar();
	}

      }
    } else {
      if (arg.size() > 2)
	throw Exception("Arguments to reshape function must be either all scalars or a single vector");
      t = arg[1];
      t.promoteType(FM_UINT32);
      dp = (int*) t.getDataPointer();
      for (i=0;i<t.getLength();i++)
	dims[i] = dp[i];
    }
    bool allPositive;
    allPositive = true;
    for (i=0;i<dims.getLength();i++)
      allPositive &= (dims[i] >= 0);
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
  //functions (e.g., @|uint8|, @|int8|, etc.).
  //    
  //The second syntax specifies the array dimensions as a vector,
  //where each element in the vector specifies a dimension length:
  //@[
  //   y = zeros([d1,d2,...,dn]).
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
  //!
  ArrayVector ZerosFunction(int nargout, const ArrayVector& arg) {
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
	  dims[0] = t.getContentsAsIntegerScalar();
	  dims[1] = dims[0];
	} else {
	  // If all scalars and and multiple arguments, we count dimensions
	  for (i=0;i<arg.size();i++) {
	    t = arg[i];
	    dims[i] = t.getContentsAsIntegerScalar();
	  }
	  
	}
      } else {
	if (arg.size() > 1)
	  throw Exception("Arguments to zeros function must be either all scalars or a single vector");
	t = arg[0];
	t.promoteType(FM_UINT32);
	dp = (int*) t.getDataPointer();
	for (i=0;i<t.getLength();i++)
	  dims[i] = dp[i];
      }
      bool allPositive;
      allPositive = true;
      for (i=0;i<dims.getLength();i++)
	allPositive &= (dims[i] >= 0);
      if (!allPositive)
	throw Exception("Zeros function requires positive arguments");
    }
    s = Array(FM_FLOAT,dims,Calloc(sizeof(float)*dims.getElementCount()));
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
	  dims[0] = t.getContentsAsIntegerScalar();
	  dims[1] = dims[0];
	} else {
	  // If all scalars and and multiple arguments, we count dimensions
	  for (i=0;i<arg.size();i++) {
	    t = arg[i];
	    dims[i] = t.getContentsAsIntegerScalar();
	  }
	  
	}
      } else {
	if (arg.size() > 1)
	  throw Exception("Arguments to ones function must be either all scalars or a single vector");
	t = arg[0];
	t.promoteType(FM_UINT32);
	dp = (int*) t.getDataPointer();
	for (i=0;i<t.getLength();i++)
	  dims[i] = dp[i];
      }
      bool allPositive;
      allPositive = true;
      for (i=0;i<dims.getLength();i++)
	allPositive &= (dims[i] >= 0);
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
  //   y = struct(n1,v1,n2,v2,...)
  //@]
  //where @|ni| are the names of the fields in the structure array, and
  //@|vi| are the values.  The values @|v_i| must either all be
  //scalars, or be cell-arrays of all the same dimensions.  In the latter case, the
  //output structure array will have dimensions dictated by this common
  //size.  Scalar entries for the @|v_i| are replicated to fill out
  //their dimensions. An error is raised if the inputs are not properly matched (i.e., are
  //not pairs of field names and values), or if the size of any two non-scalar
  //values cell-arrays are different.
  //@@Example
  //This example creates a 3-element structure array with two fields, @|foo|
  //and @|bar|, where the contents of @|foo| are provided explicitly, and
  //the contents of @|bar| are replicated from a scalar.
  //@<
  //y = struct('foo',{1,3,4},'bar',{'cheese','cola','beer'},'key',508)
  //y(1)
  //y(2)
  //y(3)
  //@>
  //!
  ArrayVector StructFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
      throw Exception("struct function requires at least two arguments");
    if (arg.size() % 2)
      throw Exception("struct function requires pairs of field names and values");
    int pairCount = arg.size() / 2;
    stringVector names;
    ArrayVector values(pairCount);
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
}
