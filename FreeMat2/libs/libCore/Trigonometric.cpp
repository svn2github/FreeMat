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
  //!
  //@Module COS Trigonometric Cosine Function
  //@@Usage
  //Computes the @|cos| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = cos(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|cos| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|cos| function is defined for all real
  //valued arguments @|x| by the infinite summation
  //\[
  //  \cos x \equiv \sum_{n=0}^{\infty} \frac{(-1)^n x^{2n}}{(2n)!}.
  //\]
  //For complex valued arguments @|z|, the cosine is computed via
  //\[
  //  \cos z \equiv \cos \Re z \cosh \Im z - \sin \Re z
  //  \sinh \Im z.
  //\]
  //@@Example
  //The following piece of code plots the real-valued @|cos(2 pi x)|
  //function over one period of @|[0,1]|:
  //@<
  //x = linspace(0,1);
  //plot(x,cos(2*pi*x))
  //mprintplot('cosplot');
  //@>
  //@figure cosplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = cos(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = cos(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module SIN Trigonometric Sine Function
  //@@Usage
  //Computes the @|sin| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = sin(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|sin| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|sin| function is defined for all real
  //valued arguments @|x| by the infinite summation
  //\[
  //  \sin x \equiv \sum_{n=1}^{\infty} \frac{(-1)^{n-1} x^{2n-1}}{(2n-1)!}.
  //\]
  //For complex valued arguments @|z|, the sine is computed via
  //\[
  //  \sin z \equiv \sin \Re z \cosh \Im z - i \cos \Re z
  //  \sinh \Im z.
  //\]
  //@@Example
  //The following piece of code plots the real-valued @|sin(2 pi x)|
  //function over one period of @|[0,1]|:
  //@<
  //x = linspace(0,1);
  //plot(x,sin(2*pi*x))
  //mprintplot('sinplot')
  //@>
  //@figure sinplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = sin(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = sin(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module TAN Trigonometric Tangent Function
  //@@Usage
  //Computes the @|tan| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = tan(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|tan| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|tan| function is defined for all real
  //valued arguments @|x| by the infinite summation
  //\[
  //  \tan x \equiv x + \frac{x^3}{3} + \frac{2x^5}{15} + \cdots,
  //\]
  //or alternately by the ratio
  //\[
  //  \tan x \equiv \frac{\sin x}{\cos x}
  //\]
  //For complex valued arguments @|z|, the tangent is computed via
  //\[
  //  \tan z \equiv \frac{\sin 2 \Re z + i \sinh 2 \Im z}
  //                     {\cos 2 \Re z + \cosh 2 \Im z}.
  //\]
  //@@Example
  //The following piece of code plots the real-valued @|tan(x)|
  //function over the interval @|[-1,1]|:
  //@<
  //t = linspace(-1,1);
  //plot(t,tan(t))
  //mprintplot('tanplot');
  //@>
  //@figure tanplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = tan(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = tan(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module CSC Trigonometric Cosecant Function
  //@@Usage
  //Computes the @|csc| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = csc(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|csc| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|csc| function is defined for all arguments
  //as
  //\[
  //   \csc x \equiv \frac{1}{\sin x}.
  //\]
  //@@Example
  //The following piece of code plots the real-valued @|csc(2 pi x)|
  //function over the interval of @|[-1,1]|:
  //@<
  //t = linspace(-1,1,1000);
  //plot(t,csc(2*pi*t))
  //axis([-1,1,-10,10])
  //mprintplot('cscplot');
  //@>
  //@figure cscplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = 1.0f/sin(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = 1.0/sin(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module SEC Trigonometric Secant Function
  //@@Usage
  //Computes the @|sec| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = sec(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|sec| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|sec| function is defined for all arguments
  //as
  //\[
  //   \sec x \equiv \frac{1}{\cos x}.
  //\]
  //@@Example
  //The following piece of code plots the real-valued @|sec(2 pi x)|
  //function over the interval of @|[-1,1]|:
  //@<
  //t = linspace(-1,1,1000);
  //plot(t,sec(2*pi*t))
  //axis([-1,1,-10,10])
  //mprintplot('secplot');
  //@>
  //@figure secplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = 1.0f/cos(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = 1.0/cos(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module COT Trigonometric Cotangent Function
  //@@Usage
  //Computes the @|cot| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = cot(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|cot| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|cot| function is defined for all 
  //arguments @|x| as
  //\[
  //  \cot x \equiv \frac{\cos x}{\sin x}
  //\]
  //For complex valued arguments @|z|, the cotangent is computed via
  //\[
  //  \cot z \equiv \frac{\cos 2 \Re z + \cosh 2 \Im z}{\sin 2 \Re z + 
  //  i \sinh 2 \Im z}.
  //\]
  //@@Example
  //The following piece of code plots the real-valued @|cot(x)|
  //function over the interval @|[-1,1]|:
  //@<
  //t = linspace(-1,1);
  //plot(t,cot(t))
  //mprintplot('cotplot');
  //@>
  //@figure cotplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = 1.0/tan(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = 1.0/tan(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module ACOS Inverse Trigonometric Arccosine Function
  //@@Usage
  //Computes the @|acos| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = acos(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|acos| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|acos| function is defined for all 
  //arguments @|x| as
  //\[
  // \mathrm{acos} x \equiv \frac{pi}{2} + i \log \left(i x + 
  //  \sqrt{1-x^2}\right).
  //\]
  //For real valued variables @|x| in the range @|[-1,1]|, the function is
  //computed directly using the standard C library's numerical @|acos|
  //function. For both real and complex arguments @|x|, note that generally
  //\[
  //  \mathrm{acos}(\cos(x)) \neq x,
  //\] due to the periodicity of @|cos(x)|.
  //@@Example
  //The following code demonstates the @|acos| function over the range 
  //@|[-1,1]|.
  //@<
  //t = linspace(-1,1);
  //plot(t,acos(t))
  //mprintplot('acosplot');
  //@>
  //@figure acosplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = acos(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = acos(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module ASIN Inverse Trigonometric Arcsine Function
  //@@Usage
  //Computes the @|asin| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = asin(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|asin| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|asin| function is defined for all 
  //arguments @|x| as
  //\[ 
  //   \mathrm{asin} x \equiv - i \log \left(i x + 
  //   \sqrt{1-x^2}\right).
  //\]
  //For real valued variables @|x| in the range @|[-1,1]|, the function is
  //computed directly using the standard C library's numerical @|asin|
  //function. For both real and complex arguments @|x|, note that generally
  //\[
  //   \mathrm{asin}(\sin(x)) \neq x,
  //\] 
  //due to the periodicity of @|sin(x)|.
  //@@Example
  //The following code demonstates the @|asin| function over the range 
  //@|[-1,1]|.
  //@<
  //t = linspace(-1,1);
  //plot(t,asin(t))
  //mprintplot('asinplot');
  //@>
  //@figure asinplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = asin(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = asin(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module ATAN Inverse Trigonometric Arctangent Function
  //@@Usage
  //Computes the @|atan| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = atan(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|atan| function.  Output @|y| is of the
  //same size and type as the input @|x|, (unless @|x| is an
  //integer, in which case @|y| is a @|double| type).  
  //@@Function Internals
  //Mathematically, the @|atan| function is defined for all 
  //arguments @|x| as
  //\[ 
  //   \mathrm{atan} x \equiv \frac{i}{2}\left(\log(1-i x) - \log(i x + 1)\right).
  //\]
  //For real valued variables @|x|, the function is computed directly using 
  //the standard C library's numerical @|atan| function. For both 
  //real and complex arguments @|x|, note that generally
  //
  //\[
  //    \mathrm{atan}(\tan(x)) \neq x,
  //\]
  // due to the periodicity of @|tan(x)|.
  //@@Example
  //The following code demonstates the @|atan| function over the range 
  //@|[-1,1]|.
  //@<
  //t = linspace(-1,1);
  //plot(t,atan(t))
  //mprintplot('atanplot');
  //@>
  //@figure atanplot
  //!
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
      const float *dp = ((const float *)input.getDataPointer());
      int len(input.getLength());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = atan(dp[i]);
      output = Array(FM_FLOAT,input.getDimensions(),op);
      break;
    }
    case FM_DOUBLE: {
      const double *dp = ((const double *)input.getDataPointer());
      int len(input.getLength());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = atan(dp[i]);
      output = Array(FM_DOUBLE,input.getDimensions(),op);
      break;
    }
    case FM_COMPLEX: {
      const float *dp = ((const float *)input.getDataPointer());
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
      const double *dp = ((const double *)input.getDataPointer());
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

  //!
  //@Module ATAN2 Inverse Trigonometric 4-Quadrant Arctangent Function
  //@@Usage
  //Computes the @|atan2| function for its argument.  The general
  //syntax for its use is
  //@[
  //  y = atan2(y,x)
  //@]
  //where @|x| and @|y| are @|n|-dimensional arrays of numerical type.
  //Integer types are promoted to the @|double| type prior to
  //calculation of the @|atan2| function. The size of the output depends
  //on the size of @|x| and @|y|.  If @|x| is a scalar, then @|z|
  //is the same size as @|y|, and if @|y| is a scalar, then @|z|
  //is the same size as @|x|.  The type of the output is equal to the type of
  //|y/x|.  
  //@@Function Internals
  //The function is defined (for real values) to return an 
  //angle between @|-pi| and @|pi|.  The signs of @|x| and @|y|
  //are used to find the correct quadrant for the solution.  For complex
  //arguments, the two-argument arctangent is computed via
  //\[
  //  \mathrm{atan2}(y,x) \equiv -i \log\left(\frac{x+i y}{\sqrt{x^2+y^2}} \right)
  //\]
  //For real valued arguments @|x,y|, the function is computed directly using 
  //the standard C library's numerical @|atan2| function. For both 
  //real and complex arguments @|x|, note that generally
  //\[
  //  \mathrm{atan2}(\sin(x),\cos(x)) \neq x,
  //\]
  //due to the periodicities of  @|cos(x)| and @|sin(x)|.
  //@@Example
  //The following code demonstates the difference between the @|atan2| 
  //function and the @|atan| function over the range @|[-pi,pi]|.
  //@<
  //x = linspace(-pi,pi);
  //sx = sin(x); cx = cos(x);
  //plot(x,atan(sx./cx),x,atan2(sx,cx))
  //mprintplot('atan2plot');
  //@>
  //@figure atan2plot
  //Note how the two-argument @|atan2| function (green line) 
  //correctly ``unwraps'' the phase of the angle, while the @|atan| 
  //function (red line) wraps the angle to the interval @|[-\pi/2,\pi/2]|.
  //!
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
      const float *dpx = ((const float *)x.getDataPointer());
      const float *dpy = ((const float *)y.getDataPointer());
      int len(outputSize.getElementCount());
      float *op = (float *)Malloc(len*sizeof(float));
      for (int i=0;i<len;i++)
	op[i] = atan2(dpy[i*yStride],dpx[i*xStride]);
      output = Array(FM_FLOAT,outputSize,op);
      break;
    }
    case FM_DOUBLE: {
      const double *dpx = ((const double *)x.getDataPointer());
      const double *dpy = ((const double *)y.getDataPointer());
      int len(outputSize.getElementCount());
      double *op = (double *)Malloc(len*sizeof(double));
      for (int i=0;i<len;i++)
	op[i] = atan2(dpy[i*yStride],dpx[i*xStride]);
      output = Array(FM_DOUBLE,outputSize,op);
      break;
    }
    case FM_COMPLEX: {
      const float *dpx = ((const float *)x.getDataPointer());
      const float *dpy = ((const float *)y.getDataPointer());
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
      const double *dpx = ((const double *)x.getDataPointer());
      const double *dpy = ((const double *)y.getDataPointer());
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
