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
  ArrayVector ReshapeFunction(int nargout, const ArrayVector& arg) {
    Array t, s;
    Dimensions dims;
    int32 *dp;
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
    for (int i=1;i<arg.size();i++)
      allScalars &= arg[i].isScalar();
    if (allScalars) {
      t = arg[1];
      if (arg.size() == 2) {
	// If all scalars and only one argument - we want a square output matrix
	dims[0] = t.getContentsAsIntegerScalar();
	dims[1] = dims[0];
      } else {
	// If all scalars and and multiple arguments, we count dimensions
	for (int i=1;i<arg.size();i++) {
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
      for (int i=0;i<t.getLength();i++)
	dims[i] = dp[i];
    }
    bool allPositive;
    allPositive = true;
    for (int i=0;i<dims.getLength();i++)
      allPositive &= (dims[i] >= 0);
    if (!allPositive)
      throw Exception("reshape function requires positive arguments");
    x.reshape(dims);
    ArrayVector retval;
    retval.push_back(x);
    return retval;
  }

  ArrayVector ZerosFunction(int nargout, const ArrayVector& arg) {
    Array t, s;
    Dimensions dims;
    int32 *dp;
    if (arg.size() == 0)
      dims.makeScalar();
    else {
      // Case 1 - all of the entries are scalar
      bool allScalars;
      allScalars = true;
      for (int i=0;i<arg.size();i++)
	allScalars &= arg[i].isScalar();
      if (allScalars) {
	t = arg[0];
	if (arg.size() == 1) {
	  // If all scalars and only one argument - we want a square zero matrix
	  dims[0] = t.getContentsAsIntegerScalar();
	  dims[1] = dims[0];
	} else {
	  // If all scalars and and multiple arguments, we count dimensions
	  for (int i=0;i<arg.size();i++) {
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
	for (int i=0;i<t.getLength();i++)
	  dims[i] = dp[i];
      }
      bool allPositive;
      allPositive = true;
      for (int i=0;i<dims.getLength();i++)
	allPositive &= (dims[i] >= 0);
      if (!allPositive)
	throw Exception("Zeros function requires positive arguments");
    }
    s = Array(FM_FLOAT,dims,Calloc(sizeof(float)*dims.getElementCount()));
    ArrayVector retval;
    retval.push_back(s);
    return retval;
  }

  ArrayVector OnesFunction(int nargout, const ArrayVector& arg) {
    Array t, s;
    Dimensions dims;
    int32 *dp;
    if (arg.size() == 0)
      dims.makeScalar();
    else {
      // Case 1 - all of the entries are scalar
      bool allScalars;
      allScalars = true;
      for (int i=0;i<arg.size();i++)
	allScalars &= arg[i].isScalar();
      if (allScalars) {
	t = arg[0];
	if (arg.size() == 1) {
	  // If all scalars and only one argument - we want a square zero matrix
	  dims[0] = t.getContentsAsIntegerScalar();
	  dims[1] = dims[0];
	} else {
	  // If all scalars and and multiple arguments, we count dimensions
	  for (int i=0;i<arg.size();i++) {
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
	for (int i=0;i<t.getLength();i++)
	  dims[i] = dp[i];
      }
      bool allPositive;
      allPositive = true;
      for (int i=0;i<dims.getLength();i++)
	allPositive &= (dims[i] >= 0);
      if (!allPositive)
	throw Exception("Ones function requires positive arguments");
    }
    int len;
    len = dims.getElementCount();
    float *qp;
    qp = (float*) Malloc(sizeof(float)*len);
    for (int i=0;i<len;i++)
      qp[i] = 1.0f;
    s = Array(FM_FLOAT,dims,qp);
    ArrayVector retval;
    retval.push_back(s);
    return retval;
  }

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
