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

#include "Core.hpp"
#include "Exception.hpp"
#include "Malloc.hpp"
#include <math.h>
#include "ranlib.h"
#include <stdio.h>

namespace FreeMat { 
  static bool initialized = false;

  //!
  //@Module SEED Seed the Random Number Generator
  //@@Usage
  //Seeds the random number generator using the given integer seed.  
  //Changing the seed allows you to choose which pseudo-random
  //sequence is generated.  The seed takes a single @|uint32| value:
  //@[
  //  seed(s)
  //@]
  //where @|s| is the seed value.
  //@@Example
  //Here's an example of how the seed value can be used to reproduce
  //a specific random number sequence.
  //@<
  //seed(32);
  //rand(1,5)
  //seed(32);
  //rand(1,5)
  //@>
  //!
  ArrayVector SeedFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Seed function requires a single integer argument");
    Array tmp(arg[0]);
    uint32 seedval;
    seedval = tmp.getContentsAsIntegerScalar();
    init_genrand(seedval);
    initialized = true;
    return ArrayVector();
  }

  //!
  //@Module RANDBETA Beta Deviate Random Number Generator
  //@@Usage
  //Creates an array of beta random deviates based on the supplied
  //two parameters.
  //A beta distribution is characterized by 
  //a density function 
  //!
  ArrayVector RandBeta(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("randbeta requires two parameter arguments");
    Array arg1(arg[0]);
    Array arg2(arg[1]);
    // Check the logic to see if one or both are scalar values
    if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
      throw Exception("randbeta requires either one of the two arguments to be a scalar, or both arguments to be the same size");
    int arg1_advance;
    int arg2_advance;
    arg1_advance = (arg1.isScalar()) ? 0 : 1;
    arg2_advance = (arg2.isscalar()) ? 0 : 1;
    // Output dimension is the larger of the two
    Dimensions outDims;
    if (arg1.getLength() > arg2.getLength()) {
      outDims = arg1.getDimensions();
    } else {
      outDims = arg2.getDimensions();
    }
    arg1.promoteType(FM_FLOAT_ARRAY);
    arg2.promoteType(FM_FLOAT_ARRAY);
    float *dp;
    dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
    float *p1;
    p1 = arg1.getDataPointer();
    float *p2;
    p2 = arg2.getDataPointer();
    int i;
    for (i=0;i<outDims.getElementCount();i++) 
      dp[i] = genbet(p1[i*arg1_advance],p2[i*arg2_advance]);
    ArrayVector retval;
    retval.push_back(Array(FM_FLOAT_ARRAY,outDims,dp));
    return retval;
  }
  
  //!
  //@Module Generate Chi-Square Random Variable
  //!
  ArrayVector RandChi(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("randchi requires exactly one parameter (the vector of degrees of freedom)");
    Array arg1(arg[0]);
    arg1.promoteType(FM_FLOAT_ARRAY);
    // Output dimension is the larger of the two
    Dimensions outDims(arg1.getDimensions());
    float *dp;
    dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
    float *p1;
    p1 = arg1.getDataPointer();
    int i;
    for (i=0;i<outDims.getElementCount();i++) {
      if (p1[i] <= 0)
	throw Exception("argument to randchi must be positive");
      dp[i] = genchi(p1[i]);
    }
    ArrayVector retval;
    retval.push_back(Array(FM_FLOAT_ARRAY,outDims,dp));
    return retval;
  }

  //!
  //@Module Generate Exponential Random Variable
  //!
  ArrayVector RandExp(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("randexp requires exactly one parameter (the vector of means)");
    Array arg1(arg[0]);
    arg1.promoteType(FM_FLOAT_ARRAY);
    // Output dimension is the larger of the two
    Dimensions outDims(arg1.getDimensions());
    float *dp;
    dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
    float *p1;
    p1 = arg1.getDataPointer();
    int i;
    for (i=0;i<outDims.getElementCount();i++) 
      dp[i] = genexp(p1[i]);
    ArrayVector retval;
    retval.push_back(Array(FM_FLOAT_ARRAY,outDims,dp));
    return retval;
  }

  //!
  //@Module Generate F-Distributed Random Variable
  //!
  ArrayVector RandF(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("randf requires two parameter arguments");
    Array arg1(arg[0]);
    Array arg2(arg[1]);
    // Check the logic to see if one or both are scalar values
    if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
      throw Exception("randf requires either one of the two arguments to be a scalar, or both arguments to be the same size");
    int arg1_advance;
    int arg2_advance;
    arg1_advance = (arg1.isScalar()) ? 0 : 1;
    arg2_advance = (arg2.isscalar()) ? 0 : 1;
    // Output dimension is the larger of the two
    Dimensions outDims;
    if (arg1.getLength() > arg2.getLength()) {
      outDims = arg1.getDimensions();
    } else {
      outDims = arg2.getDimensions();
    }
    arg1.promoteType(FM_FLOAT_ARRAY);
    arg2.promoteType(FM_FLOAT_ARRAY);
    float *dp;
    dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
    float *p1;
    p1 = arg1.getDataPointer();
    float *p2;
    p2 = arg2.getDataPointer();
    int i;
    for (i=0;i<outDims.getElementCount();i++) {
      if ((p1[i*arg1_advance] <= 0) || (p2[i*arg2_advance]) <= 0)
	throw Exception("randf requires positive arguments");
      dp[i] = genf(p1[i*arg1_advance],p2[i*arg2_advance]);
    }
    ArrayVector retval;
    retval.push_back(Array(FM_FLOAT_ARRAY,outDims,dp));
    return retval;
  }

  //!
  //@Module Generate Gamma-Distributed Random Variable
  //!
  ArrayVector RandGamma(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("randgamma requires two parameter arguments");
    Array arg1(arg[0]);
    Array arg2(arg[1]);
    // Check the logic to see if one or both are scalar values
    if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
      throw Exception("randgamma requires either one of the two arguments to be a scalar, or both arguments to be the same size");
    int arg1_advance;
    int arg2_advance;
    arg1_advance = (arg1.isScalar()) ? 0 : 1;
    arg2_advance = (arg2.isscalar()) ? 0 : 1;
    // Output dimension is the larger of the two
    Dimensions outDims;
    if (arg1.getLength() > arg2.getLength()) {
      outDims = arg1.getDimensions();
    } else {
      outDims = arg2.getDimensions();
    }
    arg1.promoteType(FM_FLOAT_ARRAY);
    arg2.promoteType(FM_FLOAT_ARRAY);
    float *dp;
    dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
    float *p1;
    p1 = arg1.getDataPointer();
    float *p2;
    p2 = arg2.getDataPointer();
    int i;
    for (i=0;i<outDims.getElementCount();i++) {
      dp[i] = gengam(p1[i*arg1_advance],p2[i*arg2_advance]);
    }
    ArrayVector retval;
    retval.push_back(Array(FM_FLOAT_ARRAY,outDims,dp));
    return retval;
  }

  //!
  //!
  ArrayVector RandMulti(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("randmulti requires two parameter arguments");
    Array arg1(arg[0]);
    Array arg2(arg[1]);
    int N = arg1.getContentsAsIntegerScalar();
    if (N<0) 
      throw Exception("number of events to generate for randmulti must be a nonnegative integer");
    arg2.promoteType(FM_FLOAT_ARRAY);
    // Verify the correctness of the probability argument
    float *dp;
    dp = arg2.getDataPointer();
    float Psum = 0.0;
    int i;
    for (i=0;i<arg2.getLength();i++) {
      if ((dp[i] < 0) || (dp[i] > 1)) 
	throw Exception("probabiliy vector argument to randmulti must have all elements between 0 and 1");
      Psum += dp[i];
    }
    if (Psum > 0.99999F)
      throw Exception("sum of probabilities (effectively) greater than 1");
    Dimensions outDims;
    outDims = arg2.getDimensions();
    int32 *ip = (int32*) Malloc(sizeof(int32)*arg2.getLength());
    genmul(N,dp,ip);
    ArrayVector retval;
    retval.push_back(Array(FM_INT32_ARRAY,outDims,ip));
    return retval;
  }
  
  //!
  //@Module Generate Noncentral Chi-Square Random Variable
  //!
  ArrayVector RandNChi(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("randnchi requires two parameter arguments");
    Array arg1(arg[0]);
    Array arg2(arg[1]);
    // Check the logic to see if one or both are scalar values
    if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
      throw Exception("randnchi requires either one of the two arguments to be a scalar, or both arguments to be the same size");
    int arg1_advance;
    int arg2_advance;
    arg1_advance = (arg1.isScalar()) ? 0 : 1;
    arg2_advance = (arg2.isscalar()) ? 0 : 1;
    // Output dimension is the larger of the two
    Dimensions outDims;
    if (arg1.getLength() > arg2.getLength()) {
      outDims = arg1.getDimensions();
    } else {
      outDims = arg2.getDimensions();
    }
    arg1.promoteType(FM_FLOAT_ARRAY);
    arg2.promoteType(FM_FLOAT_ARRAY);
    float *dp;
    dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
    float *p1;
    p1 = arg1.getDataPointer();
    float *p2;
    p2 = arg2.getDataPointer();
    int i;
    for (i=0;i<outDims.getElementCount();i++) {
      if (p1[i*arg1_advance] <= 1.0)
	throw Exception("degrees of freedom argument must be > 1.0");
      if (p2[i*arg2_advance] < 0.0)
	throw Exception("noncentrality parameter must be positive");
      dp[i] = gennch(p1[i*arg1_advance],p2[i*arg2_advance]);
    }
    ArrayVector retval;
    retval.push_back(Array(FM_FLOAT_ARRAY,outDims,dp));
    return retval;
  }

  //!
  //@Module Generate Noncentral F-Distribution Random Variable
  //!
  ArrayVector RandNf(int nargout, const ArrayVector& arg) {
    if (arg.size() != 3)
      throw Exception("randnf requires three parameter arguments");
    Array arg1(arg[0]);
    Array arg2(arg[1]);
    Array arg3(arg[2]);
    // 
    // Check the logic to see if one or both are scalar values
    if (!(arg1.isScalar() || arg2.isScalar() || (arg1.getDimensions().equals(arg2.getDimensions()))))
      throw Exception("randnf requires either one of the two arguments to be a scalar, or both arguments to be the same size");
    int arg1_advance;
    int arg2_advance;
    arg1_advance = (arg1.isScalar()) ? 0 : 1;
    arg2_advance = (arg2.isscalar()) ? 0 : 1;
    // Output dimension is the larger of the two
    Dimensions outDims;
    if (arg1.getLength() > arg2.getLength()) {
      outDims = arg1.getDimensions();
    } else {
      outDims = arg2.getDimensions();
    }
    arg1.promoteType(FM_FLOAT_ARRAY);
    arg2.promoteType(FM_FLOAT_ARRAY);
    float *dp;
    dp = (float *) Malloc(sizeof(float)*outDims.getElementCount());
    float *p1;
    p1 = arg1.getDataPointer();
    float *p2;
    p2 = arg2.getDataPointer();
    int i;
    for (i=0;i<outDims.getElementCount();i++) {
      if (p1[i*arg1_advance] <= 1.0)
	throw Exception("degrees of freedom argument must be > 1.0");
      if (p2[i*arg2_advance] < 0.0)
	throw Exception("noncentrality parameter must be positive");
      dp[i] = gennch(p1[i*arg1_advance],p2[i*arg2_advance]);
    }
    ArrayVector retval;
    retval.push_back(Array(FM_FLOAT_ARRAY,outDims,dp));
    return retval;
  }

  //!
  //@Module RANDN Gaussian (Normal) Random Number Generator
  //@@Usage
  //Creates an array of pseudo-random numbers of the specified size.
  //The numbers are normally distributed with zero mean and a unit
  //standard deviation (i.e., @|mu = 0, sigma = 1|). 
  // Two seperate syntaxes are possible.  The first syntax specifies the array 
  //dimensions as a sequence of scalar dimensions:
  //@[
  //  y = randn(d1,d2,...,dn).
  //@]
  //The resulting array has the given dimensions, and is filled with
  //random numbers.  The type of @|y| is @|double|, a 64-bit floating
  //point array.  To get arrays of other types, use the typecast 
  //functions.
  //    
  //The second syntax specifies the array dimensions as a vector,
  //where each element in the vector specifies a dimension length:
  //@[
  //  y = randn([d1,d2,...,dn]).
  //@]
  //This syntax is more convenient for calling @|randn| using a 
  //variable for the argument.
  //@@Function Internals
  //Recall that the
  //probability density function (PDF) of a normal random variable is
  //\[
  //f(x) = \frac{1}{\sqrt{2\pi \sigma^2}} e^{\frac{-(x-\mu)^2}{2\sigma^2}}.
  //\]
  //The Gaussian random numbers are generated from pairs of uniform random numbers using a transformation technique. 
  //@@Example
  //The following example demonstrates an example of using the first form of the @|randn| function.
  //@<
  //randn(2,2,2)
  //@>
  //The second example demonstrates the second form of the @|randn| function.
  //@<
  //randn([2,2,2])
  //@>
  //In the next example, we create a large array of 10000  normally distributed pseudo-random numbers.  We then shift the mean to 10, and the variance to 5.  We then numerically calculate the mean and variance using @|mean| and @|var|, respectively.
  //@<
  //x = 10+sqrt(5)*randn(1,10000);
  //mean(x)
  //var(x)
  //@>
  //!
  ArrayVector RandnFunction(int nargout, const ArrayVector& arg) {
	  int i;
    unsigned long init[4]={0x923, 0x234, 0x405, 0x456}, length=4;
    if (!initialized) {
      init_by_array(init, length);
      initialized = true;
    }
    Array t, s;
    Dimensions dims;
    int32 *dp;
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
	  throw Exception("Arguments to randn function must be either all scalars or a single vector");
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
	throw Exception("Randn function requires positive arguments");
    }
    double *qp;
    qp = (double*) Malloc(sizeof(double)*dims.getElementCount());
    int len;
    int j;
    len = dims.getElementCount();
    for (j=0;j<len;j+=2) {
      double x1, x2, w, y1, y2;
      do {
	x1 = 2.0*genrand_res53()-1.0;
	x2 = 2.0*genrand_res53()-1.0;
	w = x1 * x1 + x2 * x2;
      } while ( w >= 1.0 );
      w = sqrt( (-2.0 * log( w ) ) / w );
      y1 = x1 * w;
      y2 = x2 * w;
      qp[j] = y1;
      if (j<len-1)
	qp[j+1] = y2;
    }
    s = Array(FM_DOUBLE,dims,qp);
    ArrayVector retval;
    retval.push_back(s);
    return retval;
  }

  //!
  //@Module RAND Uniform Random Number Generator
  //@@Usage
  //Creates an array of pseudo-random numbers of the specified size.
  //The numbers are uniformly distributed on @|[0,1)|.  
  //Two seperate syntaxes are possible.  The first syntax specifies the array 
  //dimensions as a sequence of scalar dimensions:
  //@[
  //  y = rand(d1,d2,...,dn).
  //@]
  //The resulting array has the given dimensions, and is filled with
  //random numbers.  The type of @|y| is @|double|, a 64-bit floating
  //point array.  To get arrays of other types, use the typecast 
  //functions.
  //    
  //The second syntax specifies the array dimensions as a vector,
  //where each element in the vector specifies a dimension length:
  //@[
  //  y = rand([d1,d2,...,dn]).
  //@]
  //This syntax is more convenient for calling @|rand| using a 
  //variable for the argument.
  //@@Example
  //The following example demonstrates an example of using the first form of the @|rand| function.
  //@<
  //rand(2,2,2)
  //@>
  //The second example demonstrates the second form of the @|rand| function.
  //@<
  //rand([2,2,2])
  //@>
  //The third example computes the mean and variance of a large number of uniform random numbers.  Recall that the mean should be @|1/2|, and the variance should be @|1/12 ~ 0.083|.
  //@<
  //x = rand(1,10000);
  //mean(x)
  //var(x)
  //@>
  //!
  ArrayVector RandFunction(int nargout, const ArrayVector& arg) {
	  int i;
    unsigned long init[4]={0x923, 0x234, 0x405, 0x456}, length=4;
    if (!initialized) {
      init_by_array(init, length);
      initialized = true;
    }
    Array t, s;
    Dimensions dims;
    int32 *dp;
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
	  throw Exception("Arguments to rand function must be either all scalars or a single vector");
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
	throw Exception("Rand function requires positive arguments");
    }
    double *qp;
    qp = (double*) Malloc(sizeof(double)*dims.getElementCount());
    int len;
    int j;
    len = dims.getElementCount();
    for (j=0;j<len;j++)
      qp[j] = genrand_res53();
    s = Array(FM_DOUBLE,dims,qp);
    ArrayVector retval;
    retval.push_back(s);
    return retval;
  }
}
