// Copyright (c) 2004 Samit Basu
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

#include "FN.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"

extern "C" {
  double dexpei_(double*);
  double deone_(double*);
  double dei_(double*);
  double derfcx_(double*);
  double derfc_(double*);
  double derf_(double*);
  double ddaw_(double*);
  double dpsi_(double*);
  double dgamma_(double*);
  double dlgama_(double*);
  float expei_(float*);
  float eone_(float*);
  float ei_(float*);
  float erfcx_(float*);
  float erfc_(float*);
  float erf_(float*);
  float daw_(float*);
  float psi_(float*);
  float gamma_(float*);
  float algama_(float*);
}

namespace FreeMat {
  //!
  //@Module EXPEI Exponential Weighted Integral Function
  //@@Usage
  //Computes the exponential weighted integral function for real arguments.  The @|expei|
  //function takes only a single argument
  //@[
  //  y = expei(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The expei function is defined by the integral:
  //\[
  //  \mathrm{expei}(x) = - e^{-x} \int_{-x}^{\infty} \frac{e^{-t}\,dt}{t}.
  //\]
  //@@Example
  //Here is a plot of the @|expei| function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = expei(x);
  //plot(x,y); xlabel('x'); ylabel('expei(x)');
  //mprintplot expei1
  //@>
  //which results in the following plot.
  //@figure expei1
  //!
  ArrayVector ExpeiFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("expei requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("expei does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("expei function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = expei_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = dexpei_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }

  //!
  //@Module EONE Exponential Integral Function
  //@@Usage
  //Computes the exponential integral function for real arguments.  The @|eone|
  //function takes only a single argument
  //@[
  //  y = eone(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The eone function is defined by the integral:
  //\[
  //  \mathrm{eone}(x) = \int_{x}^{\infty} \frac{e^{-u}\,du}{u}.
  //\]
  //@@Example
  //Here is a plot of the @|eone| function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = eone(x);
  //plot(x,y); xlabel('x'); ylabel('eone(x)');
  //mprintplot eone1
  //@>
  //which results in the following plot.
  //@figure eone1
  //!
  ArrayVector EoneFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("eone requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("eone does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("eone function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = eone_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = deone_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }

  //!
  //@Module EI Exponential Integral Function
  //@@Usage
  //Computes the exponential integral function for real arguments.  The @|ei|
  //function takes only a single argument
  //@[
  //  y = ei(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The ei function is defined by the integral:
  //\[
  //  \mathrm{ei}(x) = -\int_{-x}^{\infty} \frac{e^{-t}\,dt}{t}.
  //\]
  //@@Example
  //Here is a plot of the @|ei| function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = ei(x);
  //plot(x,y); xlabel('x'); ylabel('ei(x)');
  //mprintplot ei1
  //@>
  //which results in the following plot.
  //@figure ei1
  //!
  ArrayVector EiFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("ei requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("ei does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("ei function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = ei_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = dei_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }

  //!
  //@Module ERFCX Complimentary Weighted Error Function
  //@@Usage
  //Computes the complimentary error function for real arguments.  The @|erfcx|
  //function takes only a single argument
  //@[
  //  y = erfcx(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The erfcx function is defined by the integral:
  //\[
  //  \mathrm{erfcx}(x) = \frac{2e^{x^2}}{\sqrt{\pi}}\int_{x}^{\infty} e^{-t^2} \, dt,
  //\]
  //and is an exponentially weighted integral of the normal distribution.
  //@@Example
  //Here is a plot of the @|erfcx| function over the range @|[-5,5]|.
  //@<
  //x = linspace(0,5);
  //y = erfcx(x);
  //plot(x,y); xlabel('x'); ylabel('erfcx(x)');
  //mprintplot erfcx1
  //@>
  //which results in the following plot.
  //@figure erfcx1
  //!
  ArrayVector ErfcxFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("erfcx requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("erfcx does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("erfcx function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = erfcx_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = derfcx_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }

  //!
  //@Module ERFC Complimentary Error Function
  //@@Usage
  //Computes the complimentary error function for real arguments.  The @|erfc|
  //function takes only a single argument
  //@[
  //  y = erfc(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The erfc function is defined by the integral:
  //\[
  //  \mathrm{erfc}(x) = \frac{2}{\sqrt{\pi}}\int_{x}^{\infty} e^{-t^2} \, dt,
  //\]
  //and is the integral of the normal distribution.
  //@@Example
  //Here is a plot of the @|erfc| function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = erfc(x);
  //plot(x,y); xlabel('x'); ylabel('erfc(x)');
  //mprintplot erfc1
  //@>
  //which results in the following plot.
  //@figure erfc1
  //!
  ArrayVector ErfcFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("erfc requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("erfc does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("erfc function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = erfc_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = derfc_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }

  //!
  //@Module ERF Error Function
  //@@Usage
  //Computes the error function for real arguments.  The @|erf|
  //function takes only a single argument
  //@[
  //  y = erf(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The erf function is defined by the integral:
  //\[
  //  \mathrm{erf}(x) = \frac{2}{\sqrt{\pi}}\int_{0}^{x} e^{-t^2} \, dt,
  //\]
  //and is the integral of the normal distribution.
  //@@Example
  //Here is a plot of the erf function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = erf(x);
  //plot(x,y); xlabel('x'); ylabel('erf(x)');
  //mprintplot erf1
  //@>
  //which results in the following plot.
  //@figure erf1
  //!
  ArrayVector ErfFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("erf requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("erf does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("erf function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = erf_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = derf_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }

  //!
  //@Module DAWSON Dawson Integral Function
  //@@Usage
  //Computes the dawson function for real arguments.  The @|dawson|
  //function takes only a single argument
  //@[
  //  y = dawson(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The dawson function is defined as
  //\[
  //  \mathrm{dawson}(x) = e^{-x^2} \int_{0}^{x} e^{t^2} \, dt
  //\]
  //@@Example
  //Here is a plot of the dawson function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = dawson(x);
  //plot(x,y); xlabel('x'); ylabel('dawson(x)');
  //mprintplot dawson1
  //@>
  //which results in the following plot.
  //@figure dawson1
  //!
  ArrayVector DawsonFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("dawson requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("dawson does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("dawson function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = daw_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = ddaw_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }
  
  //!
  //@Module PSI Psi Function
  //@@Usage
  //Computes the psi function for real arguments.  The @|psi|
  //function takes only a single argument
  //@[
  //  y = psi(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The psi function is defined as
  //\[
  //  \frac{d}{dx} \ln \gamma(x)
  //\]
  //and for integer arguments, is equivalent to the factorial function.
  //@@Example
  //Here is a plot of the psi function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = psi(x);
  //plot(x,y); xlabel('x'); ylabel('psi(x)');
  //mprintplot psi1
  //@>
  //which results in the following plot.
  //@figure psi1
  //!
  ArrayVector PsiFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("psi requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("psi does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("psi function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++) {
	float t;
	t = psi_(sp+i);
	if (t == 1.70e38)
	  t = atof("inf");
	dp[i] = t;
      }
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++) {
	double t;
	t = dpsi_(sp+i);
	if (t == 1.70E38)
	  t = atof("inf");	
	dp[i] = t;
      }
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }
  
  //!
  //@Module GAMMA Gamma Function
  //@@Usage
  //Computes the gamma function for real arguments.  The @|gamma|
  //function takes only a single argument
  //@[
  //  y = gamma(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The gamma function is defined by the integral:
  //\[
  //  \Gamma(x) = \int_{0}^{\infty} e^{-t} t^{x-1} \, dt
  //\]
  //The gamma function obeys the interesting relationship
  //\[
  //  \Gamma(x) = (x-1)\Gamma(x-1),
  //\]
  //and for integer arguments, is equivalent to the factorial function.
  //@@Example
  //Here is a plot of the gamma function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = gamma(x);
  //plot(x,y); xlabel('x'); ylabel('gamma(x)');
  //axis([-5,5,-5,5]);
  //mprintplot gamma1
  //@>
  //which results in the following plot.
  //@figure gamma1
  //!
  ArrayVector GammaFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("gamma requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("gamma does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("gamma function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++) {
	float t;
	t = gamma_(sp+i);
	if (t == 3.4e38)
	  t = atof("inf");
	dp[i] = t;
      }
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++) {
	double t;
	t = dgamma_(sp+i);
	if (t == 1.79E308)
	  t = atof("inf");
	dp[i] = t;
      }
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }

  //!
  //@Module GAMMALN Log Gamma Function
  //@@Usage
  //Computes the natural log of the gamma function for real arguments.  The @|gammaln|
  //function takes only a single argument
  //@[
  //  y = gammaln(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Example
  //Here is a plot of the @|gammaln| function over the range @|[-5,5]|.
  //@<
  //x = linspace(0,10);
  //y = gammaln(x);
  //plot(x,y); xlabel('x'); ylabel('gammaln(x)');
  //mprintplot gammaln1
  //@>
  //which results in the following plot.
  //@figure gammaln1
  //!
  ArrayVector GammaLnFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("gammaln requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("gammaln does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("gammaln function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++) {
	float t;
	t = algama_(sp+i);
	if (t == 3.4e38)
	  t = atof("inf");
	dp[i] = t;
      }
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odims.getElementCount());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++) {
	double t;
	t = dlgama_(sp+i);
	if (t == 1.79E308)
	  t = atof("inf");
	dp[i] = t;
      }
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }
}
