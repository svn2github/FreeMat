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

namespace FreeMat {
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
	 const float *dp((const float *)input.getDataPointer());
	 int len(input.getLength());
	 float *op = (float *)Malloc(len*sizeof(float));
	 for (int i=0;i<len;i++)
	   op[i] = log(dp[i]);
	 output = Array(FM_FLOAT,input.getDimensions(),op);
       } else {
	 const float *dp((const float *)input.getDataPointer());
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
	 const double *dp((const double *)input.getDataPointer());
	 int len(input.getLength());
	 double *op = (double *)Malloc(len*sizeof(double));
	 for (int i=0;i<len;i++)
	   op[i] = log(dp[i]);
	 output = Array(FM_DOUBLE,input.getDimensions(),op);
       } else {
	 const double *dp((const double *)input.getDataPointer());
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
       const float *dp((const float *)input.getDataPointer());
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
       const double *dp((const double *)input.getDataPointer());
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
       const float *dp((const float *)input.getDataPointer());
       int len(input.getLength());
       float *op = (float *)Malloc(len*sizeof(float));
       for (int i=0;i<len;i++)
	 op[i] = exp(dp[i]);
       output = Array(FM_FLOAT,input.getDimensions(),op);
       break;
     }
     case FM_DOUBLE: {
       const double *dp((const double *)input.getDataPointer());
       int len(input.getLength());
       double *op = (double *)Malloc(len*sizeof(double));
       for (int i=0;i<len;i++)
	 op[i] = exp(dp[i]);
       output = Array(FM_DOUBLE,input.getDimensions(),op);
       break;
     }
     case FM_COMPLEX: {
       const float *dp((const float *)input.getDataPointer());
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
       const double *dp((const double *)input.getDataPointer());
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


