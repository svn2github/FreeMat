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
#include <math.h>
#include "Utils.hpp"
#include "IEEEFP.hpp"

namespace FreeMat {
  ArrayVector CosFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Cosine Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to cosine must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = cos(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = cos(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      for (int i=0;i<2*len;i+=2) {
 	op[i] = cos(dp[i])*cosh(dp[i+1]);
	op[i+1] = -sin(dp[i])*sinh(dp[i+1]);
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      for (int i=0;i<2*len;i+=2) {
 	op[i] = cos(dp[i])*cosh(dp[i+1]);
	op[i+1] = -sin(dp[i])*sinh(dp[i+1]);
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector SinFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Sin Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to sine must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = sin(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = sin(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      for (int i=0;i<2*len;i+=2) {
	op[i] = sin(dp[i])*cosh(dp[i+1]);
	op[i+1] = cos(dp[i])*sinh(dp[i+1]);
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      for (int i=0;i<2*len;i+=2) {
	op[i] = sin(dp[i])*cosh(dp[i+1]);
	op[i+1] = cos(dp[i])*sinh(dp[i+1]);
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector TanFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Tangent Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to tangent must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = tan(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = tan(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      float den;
      for (int i=0;i<2*len;i+=2) {
	den = cos(2*dp[i]) + cosh(2*dp[i+1]);
 	op[i] = sin(2*dp[i])/den;
	op[i+1] = sinh(2*dp[i+1])/den;
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      double den;
      for (int i=0;i<2*len;i+=2) {
	den = cos(2*dp[i]) + cosh(2*dp[i+1]);
 	op[i] = sin(2*dp[i])/den;
	op[i+1] = sinh(2*dp[i+1])/den;
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector CscFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Cosecant Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to cosecant must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = 1.0f/sin(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = 1.0/sin(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      float re, im;
      for (int i=0;i<2*len;i+=2) {
	re = sin(dp[i])*cosh(dp[i+1]);
	im = cos(dp[i])*sinh(dp[i+1]);
	op[i] = re/(re*re + im*im);
	op[i+1] = -im/(re*re + im*im);
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      double re, im;
      for (int i=0;i<2*len;i+=2) {
	re = sin(dp[i])*cosh(dp[i+1]);
	im = cos(dp[i])*sinh(dp[i+1]);
	op[i] = re/(re*re + im*im);
	op[i+1] = -im/(re*re + im*im);
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector SecFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Secant Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to secant must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = 1.0f/cos(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = 1.0/cos(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      float re, im;
      for (int i=0;i<2*len;i+=2) {
	re = cos(dp[i])*cosh(dp[i+1]);
	im = -sin(dp[i])*sinh(dp[i+1]);
	op[i] = re/(re*re + im*im);
	op[i+1] = -im/(re*re + im*im);
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      double re, im;
      for (int i=0;i<2*len;i+=2) {
	re = cos(dp[i])*cosh(dp[i+1]);
	im = -sin(dp[i])*sinh(dp[i+1]);
	op[i] = re/(re*re + im*im);
	op[i+1] = -im/(re*re + im*im);
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector CotFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Cotangent Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to cotangent must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = 1.0/tan(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = 1.0/tan(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      float den;
      for (int i=0;i<2*len;i+=2) {
	den = - cos(2*dp[i]) + cosh(2*dp[i+1]);
 	op[i] = sin(2*dp[i])/den;
	op[i+1] = -sinh(2*dp[i+1])/den;
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      double den;
      for (int i=0;i<2*len;i+=2) {
	den = - cos(2*dp[i]) + cosh(2*dp[i+1]);
 	op[i] = sin(2*dp[i])/den;
	op[i+1] = -sinh(2*dp[i+1])/den;
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector ArccosFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Arccosine Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    // Check the range
    if (argType == FM_FLOAT) {
      int i;
      float rngeVal;
      int cnt;
      bool init;
      const float *dp = (const float *) input.getDataPointer();
      cnt = input.getLength();
      init = false;
      for (i=0;i<cnt;i++) {
	if (!IsNaN(dp[i])) {
	  if (init) {
	    rngeVal = (fabs(dp[i]) > rngeVal) ? fabs(dp[i]) : rngeVal;
	  } else {
	    rngeVal = fabs(dp[i]);
	    init = true;
	  }
	}
      }
      if (!init) {
	ArrayVector retval;
	retval.push_back(input);
	return retval;
      }
      if (rngeVal > 1.0f) {
	input.promoteType(FM_COMPLEX);
	argType = FM_COMPLEX;
      }
    } else if (argType == FM_DOUBLE) {
      int i;
      double rngeVal;
      int cnt;
      bool init;
      const double *dp = (const double *) input.getDataPointer();
      cnt = input.getLength();
      init = false;
      for (i=0;i<cnt;i++) {
	if (!IsNaN(dp[i])) {
	  if (init) {
	    rngeVal = (fabs(dp[i]) > rngeVal) ? fabs(dp[i]) : rngeVal;
	  } else {
	    rngeVal = fabs(dp[i]);
	    init = true;
	  }
	}
      }
      if (!init) {
	ArrayVector retval;
	retval.push_back(input);
	return retval;
      }
      if (rngeVal > 1.0) {
	input.promoteType(FM_DCOMPLEX);
	argType = FM_DCOMPLEX;
      }      
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to arccosine must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = acos(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = acos(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	float x_real = dp[i];
	float x_imag = dp[i+1];
	float xsq_real, xsq_imag;
	// Compute x^2
	csqr(x_real,x_imag,&xsq_real,&xsq_imag);
	// Compute 1-x^2
	xsq_real = 1.0 - xsq_real;
	xsq_imag = -xsq_imag;
	float xrt_real, xrt_imag;
	// Compute sqrt(1-x^2)
	csqrt(xsq_real,xsq_imag,&xrt_real,&xrt_imag);
	// Add i*x = i*(a+b*i) = -b+i*a
	xrt_real -= x_imag;
	xrt_imag += x_real;
	// Take the complex log
	float xlg_real, xlg_imag;
	clog(xrt_real,xrt_imag,&xlg_real,&xlg_imag);
	// Answer = pi/2
	op[i] = 2.0*atan(1.0f) - xlg_imag;
	op[i+1] = xlg_real;
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	double x_real = dp[i];
	double x_imag = dp[i+1];
	double xsq_real, xsq_imag;
	// Compute x^2
	zsqr(x_real,x_imag,&xsq_real,&xsq_imag);
	// Compute 1-x^2
	xsq_real = 1.0 - xsq_real;
	xsq_imag = -xsq_imag;
	double xrt_real, xrt_imag;
	// Compute sqrt(1-x^2)
	zsqrt(xsq_real,xsq_imag,&xrt_real,&xrt_imag);
	// Add i*x = i*(a+b*i) = -b+i*a
	xrt_real -= x_imag;
	xrt_imag += x_real;
	// Take the complex log
	double xlg_real, xlg_imag;
	zlog(xrt_real,xrt_imag,&xlg_real,&xlg_imag);
	// Answer = pi/2
	op[i] = 2.0*atan(1.0f) - xlg_imag;
	op[i+1] = xlg_real;
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector ArcsinFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Arcsine Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    // Check the range
    if (argType == FM_FLOAT) {
      int i;
      float rngeVal;
      int cnt;
      bool init;
      const float *dp = (const float *) input.getDataPointer();
      cnt = input.getLength();
      init = false;
      for (i=0;i<cnt;i++) {
	if (!IsNaN(dp[i])) {
	  if (init) {
	    rngeVal = (fabs(dp[i]) > rngeVal) ? fabs(dp[i]) : rngeVal;
	  } else {
	    rngeVal = fabs(dp[i]);
	    init = true;
	  }
	}
      }
      if (!init) {
	ArrayVector retval;
	retval.push_back(input);
	return retval;
      }
      if (rngeVal > 1.0f) {
	input.promoteType(FM_COMPLEX);
	argType = FM_COMPLEX;
      }
    } else if (argType == FM_DOUBLE) {
      int i;
      double rngeVal;
      int cnt;
      bool init;
      const double *dp = (const double *) input.getDataPointer();
      cnt = input.getLength();
      init = false;
      for (i=0;i<cnt;i++) {
	if (!IsNaN(dp[i])) {
	  if (init) {
	    rngeVal = (fabs(dp[i]) > rngeVal) ? fabs(dp[i]) : rngeVal;
	  } else {
	    rngeVal = fabs(dp[i]);
	    init = true;
	  }
	}
      }
      if (!init) {
	ArrayVector retval;
	retval.push_back(input);
	return retval;
      }
      if (rngeVal > 1.0) {
	input.promoteType(FM_DCOMPLEX);
	argType = FM_DCOMPLEX;
      }      
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to arcsine must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = asin(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = asin(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	float x_real = dp[i];
	float x_imag = dp[i+1];
	float xsq_real, xsq_imag;
	// Compute x^2
	csqr(x_real,x_imag,&xsq_real,&xsq_imag);
	// Compute 1-x^2
	xsq_real = 1.0 - xsq_real;
	xsq_imag = -xsq_imag;
	float xrt_real, xrt_imag;
	// Compute sqrt(1-x^2)
	csqrt(xsq_real,xsq_imag,&xrt_real,&xrt_imag);
	// Add i*x = i*(a+b*i) = -b+i*a
	xrt_real -= x_imag;
	xrt_imag += x_real;
	// Take the complex log
	float xlg_real, xlg_imag;
	clog(xrt_real,xrt_imag,&xlg_real,&xlg_imag);
	op[i] = xlg_imag;
	op[i+1] = - xlg_real;
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	double x_real = dp[i];
	double x_imag = dp[i+1];
	double xsq_real, xsq_imag;
	// Compute x^2
	zsqr(x_real,x_imag,&xsq_real,&xsq_imag);
	// Compute 1-x^2
	xsq_real = 1.0 - xsq_real;
	xsq_imag = -xsq_imag;
	double xrt_real, xrt_imag;
	// Compute sqrt(1-x^2)
	zsqrt(xsq_real,xsq_imag,&xrt_real,&xrt_imag);
	// Add i*x = i*(a+b*i) = -b+i*a
	xrt_real -= x_imag;
	xrt_imag += x_real;
	// Take the complex log
	double xlg_real, xlg_imag;
	zlog(xrt_real,xrt_imag,&xlg_real,&xlg_imag);
	// Answer = pi/2
	op[i] = xlg_imag;
	op[i+1] = - xlg_real;
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector ArctanFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Arctan Function takes exactly one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argType(input.getDataClass());
    if (argType < FM_FLOAT) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }
    if (input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(input);
      return retval;
    }
    if (argType > FM_DCOMPLEX)
      throw Exception("argument to arctan must be numeric");
    switch (argType) {
    case FM_FLOAT: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = atan(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = atan(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	float x_real = dp[i];
	float x_imag = dp[i+1];
	float a_real, a_imag;
	float b_real, b_imag;
	// a = log(1-i*x) = log(1-i*(xr+i*xi))
	//   = log(1 - i*xr -i^2*xi) = log(1+xi-i*xr)
	clog(1 + x_imag,-x_real,&a_real,&a_imag);
	// b = log(i*x+1) = log(i*(xr+i*xi)+1)
	//   = log(i*xr + i^2*xi + 1) = log(1-xi + i*xr)
	clog(1 - x_imag,x_real,&b_real,&b_imag);
	// atan = i/2*(a-b) = i/2*((a_r-b_r)+i*(a_i-b_i))
	//      = -1/2(a_i-b_i) + i/2*(a_r-b_r)
	op[i] = -0.5*(a_imag-b_imag);
	op[i+1] = 0.5*(a_real-b_real);
      }
      output = Array(FM_COMPLEX,input.getDimensions(),op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dp((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	double x_real = dp[i];
	double x_imag = dp[i+1];
	double a_real, a_imag;
	double b_real, b_imag;
	// a = log(1-i*x) = log(1-i*(xr+i*xi))
	//   = log(1 - i*xr -i^2*xi) = log(1+xi-i*xr)
	zlog(1 + x_imag,-x_real,&a_real,&a_imag);
	// b = log(i*x+1) = log(i*(xr+i*xi)+1)
	//   = log(i*xr + i^2*xi + 1) = log(1-xi + i*xr)
	zlog(1 - x_imag,x_real,&b_real,&b_imag);
	// atan = i/2*(a-b) = i/2*((a_r-b_r)+i*(a_i-b_i))
	//      = -1/2(a_i-b_i) + i/2*(a_r-b_r)
	op[i] = -0.5*(a_imag-b_imag);
	op[i+1] = 0.5*(a_real-b_real);
      }
      output = Array(FM_DCOMPLEX,input.getDimensions(),op);
      break;      
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }

  ArrayVector Arctan2Function(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("Arctan2 Function takes exactly two arguments");
    Array y(arg[0]);
    Array x(arg[1]);
    if (x.isEmpty() || y.isEmpty()) {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
    Array output;
    Class argTypex(x.getDataClass());
    Class argTypey(y.getDataClass());
    if (argTypex < FM_FLOAT) {
      x.promoteType(FM_DOUBLE);
      argTypex = FM_DOUBLE;
    }
    if (argTypey < FM_FLOAT) {
      y.promoteType(FM_DOUBLE);
      argTypey = FM_DOUBLE;
    }
    if (argTypex > FM_DCOMPLEX)
      throw Exception("arguments to arctan2 must be numeric");
    if (argTypey > FM_DCOMPLEX)
      throw Exception("arguments to arctan2 must be numeric");
    // Check for complex
    bool isComplex;
    isComplex = x.isComplex() || y.isComplex();
    // Check for 32 bits
    bool is32bits;
    is32bits = (argTypex == FM_FLOAT || argTypex == FM_COMPLEX);
    if (isComplex && is32bits) {
      x.promoteType(FM_COMPLEX);      
      y.promoteType(FM_COMPLEX);
    } else if (!isComplex && is32bits) {
      x.promoteType(FM_FLOAT);      
      y.promoteType(FM_FLOAT);
    } else if (isComplex && !is32bits) {
      x.promoteType(FM_DCOMPLEX);      
      y.promoteType(FM_DCOMPLEX);
    } else {
      x.promoteType(FM_DOUBLE);      
      y.promoteType(FM_DOUBLE);
    }
    Class argType(x.getDataClass());
    // Next check the sizes.
    Dimensions outputSize;
    Dimensions xDim(x.getDimensions());
    Dimensions yDim(y.getDimensions());
    int xStride, yStride;
    if (xDim.isScalar()) {
      outputSize = yDim;
      xStride = 0;
      yStride = 1;
    } else if (yDim.isScalar()) {
      outputSize = xDim;
      xStride = 1;
      yStride = 0;
    } else if (xDim.equals(yDim)) {
      outputSize = xDim;
      xStride = 1;
      yStride = 1;
    } else 
      throw Exception("Illegal combination of sizes for input to atan2 - either both arguments must be the same size, or one must be a scalar");
    
    switch (argType) {
    case FM_FLOAT: {
      const float *dpx((const float *)x.getDataPointer());
      const float *dpy((const float *)y.getDataPointer());
      int len(outputSize.getElementCount());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = atan2(dpy[i*yStride],dpx[i*xStride]);
      output = Array(FM_FLOAT,outputSize,op);
      break;
    }
    case FM_DOUBLE: {
      const double *dpx((const double *)x.getDataPointer());
      const double *dpy((const double *)y.getDataPointer());
      int len(outputSize.getElementCount());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = atan2(dpy[i*yStride],dpx[i*xStride]);
      output = Array(FM_DOUBLE,outputSize,op);
      break;
    }
    case FM_COMPLEX: {
      const float *dpx((const float *)x.getDataPointer());
      const float *dpy((const float *)y.getDataPointer());
      int len(outputSize.getElementCount());
      float *op = (float *)Malloc(len*sizeof(float)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	float x_real = dpx[xStride*i];
	float x_imag = dpx[xStride*i+1];
	float y_real = dpy[yStride*i];
	float y_imag = dpy[yStride*i+1];
	// a = x + i*y = (x_r + i*x_i) + i*(y_r + i*y_i)
	//             = x_r - y_i + i*(x_i + y_r)
	float a_real, a_imag;
	a_real = x_real - y_imag;
	a_imag = x_imag + y_real;
	// compute x_squared and y_squared
	float xsqr_real, xsqr_imag;
	float ysqr_real, ysqr_imag;
	csqr(x_real,x_imag,&xsqr_real,&xsqr_imag);
	csqr(y_real,y_imag,&ysqr_real,&ysqr_imag);
	float den_real, den_imag;
	den_real = xsqr_real + ysqr_real;
	den_imag = xsqr_imag + ysqr_imag;
	float den_sqrt_real, den_sqrt_imag;
	csqrt(den_real,den_imag,&den_sqrt_real,&den_sqrt_imag);
	// compute the log of the numerator
	float log_num_real, log_num_imag;
	clog(a_real,a_imag,&log_num_real,&log_num_imag);
	// compute the log of the denominator
	float log_den_real, log_den_imag;
	clog(den_sqrt_real,den_sqrt_imag,&log_den_real,&log_den_imag);
	// compute the num - den
	log_num_real -= log_den_real;
	log_num_imag -= log_den_imag;
	// compute -i * (c_r + i * c_i) = c_i - i * c_r
	op[i] = log_num_imag;
	op[i+1] = -log_num_real;
      }
      output = Array(FM_COMPLEX,outputSize,op);
      break;      
    }
    case FM_DCOMPLEX: {
      const double *dpx((const double *)x.getDataPointer());
      const double *dpy((const double *)y.getDataPointer());
      int len(outputSize.getElementCount());
      double *op = (double *)Malloc(len*sizeof(double)*2);
      for (int i=0;i<2*len;i+=2) {
	// Retrieve x
	double x_real = dpx[xStride*i];
	double x_imag = dpx[xStride*i+1];
	double y_real = dpy[yStride*i];
	double y_imag = dpy[yStride*i+1];
	// a = x + i*y = (x_r + i*x_i) + i*(y_r + i*y_i)
	//             = x_r - y_i + i*(x_i + y_r)
	double a_real, a_imag;
	a_real = x_real - y_imag;
	a_imag = x_imag + y_real;
	// compute x_squared and y_squared
	double xsqr_real, xsqr_imag;
	double ysqr_real, ysqr_imag;
	zsqr(x_real,x_imag,&xsqr_real,&xsqr_imag);
	zsqr(y_real,y_imag,&ysqr_real,&ysqr_imag);
	double den_real, den_imag;
	den_real = xsqr_real + ysqr_real;
	den_imag = xsqr_imag + ysqr_imag;
	double den_sqrt_real, den_sqrt_imag;
	zsqrt(den_real,den_imag,&den_sqrt_real,&den_sqrt_imag);
	// compute the log of the numerator
	double log_num_real, log_num_imag;
	zlog(a_real,a_imag,&log_num_real,&log_num_imag);
	// compute the log of the denominator
	double log_den_real, log_den_imag;
	zlog(den_sqrt_real,den_sqrt_imag,&log_den_real,&log_den_imag);
	// compute the num - den
	log_num_real -= log_den_real;
	log_num_imag -= log_den_imag;
	// compute -i * (c_r + i * c_i) = c_i - i * c_r
	op[i] = log_num_imag;
	op[i+1] = -log_num_real;
      }
      output = Array(FM_DCOMPLEX,outputSize,op);
      break;
    }
    }
    ArrayVector retval;
    retval.push_back(output);
    return retval;
  }
}
