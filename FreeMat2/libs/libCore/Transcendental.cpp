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
#include "Array.hpp"
#include "Malloc.hpp"
#include "Utils.hpp"
#include <math.h>
#include "IEEEFP.hpp"

namespace FreeMat {
  //!
  //@Module LOG Natural Logarithm Function
  //@@Usage
  //Computes the @|log| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = log(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|log| function.  Output @|y| is of the
  //same size as the input @|x|. For strictly positive, real inputs, 
  //the output type is the same as the input.
  //For negative and complex arguments, the output is complex.
  //@@Function Internals
  //Mathematically, the @|log| function is defined for all real
  //valued arguments @|x| by the integral
  //\[
  //  \log x \equiv \int_1^{x} \frac{d\,t}{t}.
  //\]
  //For complex-valued arguments, @|z|, the complex logarithm is
  //defined as
  //\[
  //  \log z \equiv \log |z| + i \arg z,
  //\]
  //where @|arg| is the complex argument of @|z|.
  //@@Example
  //The following piece of code plots the real-valued @|log|
  //function over the interval @|[1,100]|:
  //@<
  //x = linspace(1,100);
  //plot(x,log(x))
  //xlabel('x');
  //ylabel('log(x)');
  //mprintplot('logplot');
  //@>
  //@figure logplot
  //!
  ArrayVector LogFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Log function takes exactly one argument");
     Array input(arg[0]);
     Array output;
     Class argType(input.getDataClass());
     if (argType < FM_FLOAT) {
       input.promoteType(FM_DOUBLE);
       argType = FM_DOUBLE;
     }
     if (argType > FM_DCOMPLEX)
       throw Exception("argument to exp must be numeric");
     switch (argType) {
     case FM_FLOAT: {
       if (input.isPositive()) {
	 const float *dp=((const float *)input.getDataPointer());
	 int len(input.getLength());
	 float *op = (float *)Malloc(len*sizeof(float));
	 for (int i=0;i<len;i++)
	   op[i] = log(dp[i]);
	 output = Array(FM_FLOAT,input.getDimensions(),op);
       } else {
	 const float *dp=((const float *)input.getDataPointer());
	 int len(input.getLength());
	 float *op = (float *)Malloc(2*len*sizeof(float));
	 for (int i=0;i<len;i++)
	   if (dp[i] >= 0.0)
	     op[2*i] = log(dp[i]); 
	   else {
	     op[2*i] = log(fabs(dp[i]));
	     op[2*i+1] = M_PI;
	   }
	 output = Array(FM_COMPLEX,input.getDimensions(),op);
       }
       break;
     }
     case FM_DOUBLE: {
       if (input.isPositive()) {
	 const double *dp=((const double *)input.getDataPointer());
	 int len(input.getLength());
	 double *op = (double *)Malloc(len*sizeof(double));
	 for (int i=0;i<len;i++)
	   op[i] = log(dp[i]);
	 output = Array(FM_DOUBLE,input.getDimensions(),op);
       } else {
	 const double *dp=((const double *)input.getDataPointer());
	 int len(input.getLength());
	 double *op = (double *)Malloc(2*len*sizeof(double));
	 for (int i=0;i<len;i++)
	   if (dp[i] >= 0.0)
	     op[2*i] = log(dp[i]); 
	   else {
	     op[2*i] = log(fabs(dp[i]));
	     op[2*i+1] = M_PI;
	   }
	 output = Array(FM_DCOMPLEX,input.getDimensions(),op);
       }
       break;
     }
     case FM_COMPLEX: {
       const float *dp=((const float *)input.getDataPointer());
       int len(input.getLength());
       float *op = (float *)Malloc(len*sizeof(float)*2);
       for (int i=0;i<2*len;i+=2) {
	 op[i] = log(complex_abs(dp[i],dp[i+1]));
	 op[i+1] = atan2(dp[i+1],dp[i]);
       }
       output = Array(FM_COMPLEX,input.getDimensions(),op);
       break;      
     }
     case FM_DCOMPLEX: {
       const double *dp=((const double *)input.getDataPointer());
       int len(input.getLength());
       double *op = (double *)Malloc(len*sizeof(double)*2);
       for (int i=0;i<2*len;i+=2) {
	 op[i] = log(complex_abs(dp[i],dp[i+1]));
	 op[i+1] = atan2(dp[i+1],dp[i]);
       }
       output = Array(FM_DCOMPLEX,input.getDimensions(),op);
       break;      
     }
     }
     ArrayVector retval;
     retval.push_back(output);
     return retval;
   }

  //!
  //@Module EXP Exponential Function
  //@@Usage
  //Computes the @|exp| function for its argument.  The general
  //syntax for its use is
  //@[
  //   y = exp(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|exp| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).
  //@@Function Internals
  //Mathematically, the @|exp| function is defined for all real
  //valued arguments @|x| as
  //\[
  //  \exp x \equiv e^{x},
  //\]
  //where
  //\[
  //  e = \sum_{0}^{\infty} \frac{1}{k!}
  //\]
  //and is approximately @|2.718281828459045| (returned by the function 
  //@|e|).  For complex values
  //@|z|, the famous Euler formula is used to calculate the 
  //exponential
  //\[
  //  e^{z} = e^{|z|} \left[ \cos \Re z + i \sin \Re z \right]
  //\]
  //@@Example
  //The following piece of code plots the real-valued @|exp|
  //function over the interval @|[-1,1]|:
  //@<
  //x = linspace(-1,1);
  //plot(x,exp(x))
  //mprintplot('expplot1');
  //@>
  //@figure expplot1
  //In the second example, we plot the unit circle in the 
  //complex plane @|e^{i 2 pi x}| for @|x in [-1,1]|.
  //@<
  //x = linspace(-1,1);
  //plot(exp(-i*x*2*pi))
  //mprintplot('expplot2');
  //@>
  //@figure expplot2
  //!
   ArrayVector ExpFunction(int nargout, const ArrayVector& arg) {
     if (arg.size() != 1)
       throw Exception("Exp function takes exactly one argument");
     Array input(arg[0]);
     Array output;
     Class argType(input.getDataClass());
     if (argType < FM_FLOAT) {
       input.promoteType(FM_DOUBLE);
       argType = FM_DOUBLE;
     }
     if (argType > FM_DCOMPLEX)
       throw Exception("argument to exp must be numeric");
     switch (argType) {
     case FM_FLOAT: {
       const float *dp=((const float *)input.getDataPointer());
       int len(input.getLength());
       float *op = (float *)Malloc(len*sizeof(float));
       for (int i=0;i<len;i++)
	 op[i] = exp(dp[i]);
       output = Array(FM_FLOAT,input.getDimensions(),op);
       break;
     }
     case FM_DOUBLE: {
       const double *dp=((const double *)input.getDataPointer());
       int len(input.getLength());
       double *op = (double *)Malloc(len*sizeof(double));
       for (int i=0;i<len;i++)
	 op[i] = exp(dp[i]);
       output = Array(FM_DOUBLE,input.getDimensions(),op);
       break;
     }
     case FM_COMPLEX: {
       const float *dp=((const float *)input.getDataPointer());
       int len(input.getLength());
       float *op = (float *)Malloc(len*sizeof(float)*2);
       for (int i=0;i<2*len;i+=2) {
	 double t = exp(dp[i]);
	 op[i] = t*cos(dp[i+1]);
	 op[i+1] = t*sin(dp[i+1]);
       }
       output = Array(FM_COMPLEX,input.getDimensions(),op);
       break;      
     }
     case FM_DCOMPLEX: {
       const double *dp=((const double *)input.getDataPointer());
       int len(input.getLength());
       double *op = (double *)Malloc(len*sizeof(double)*2);
       for (int i=0;i<2*len;i+=2) {
	 double t = exp(dp[i]);
	 op[i] = t*cos(dp[i+1]);
	 op[i+1] = t*sin(dp[i+1]);
       }
       output = Array(FM_DCOMPLEX,input.getDimensions(),op);
       break;      
     }
     }
     ArrayVector retval;
     retval.push_back(output);
     return retval;
   }
}


