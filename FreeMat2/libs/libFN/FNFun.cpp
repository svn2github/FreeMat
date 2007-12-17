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

#include "FN.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include <math.h>

//!
//@Module ERFC Complimentary Error Function
//@@Section MATHFUNCTIONS
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
//mprint erfc1
//@>
//which results in the following plot.
//@figure erfc1
//!
ArrayVector ErfcFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erfc requires at least one argument");
  Array tmp(arg[0]);
  if (tmp.dataClass() < FM_FLOAT)
    tmp.promoteType(FM_DOUBLE);
  if (tmp.isComplex())
    throw Exception("erfc does not work with complex arguments");
  if (tmp.isReferenceType() || tmp.isString())
    throw Exception("erfc function requires numerical arguments");
  if (tmp.dataClass() == FM_FLOAT) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    float *sp = (float*) tmp.getDataPointer();
    float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
    for (int i=0;i<olen;i++)
      dp[i] = erfcf(sp[i]);
    return SingleArrayVector(Array(FM_FLOAT,odims,dp));
  } else if (tmp.dataClass() == FM_DOUBLE) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    double *sp = (double*) tmp.getDataPointer();
    double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
    for (int i=0;i<olen;i++)
      dp[i] = erfc(sp[i]);
    return SingleArrayVector(Array(FM_DOUBLE,odims,dp));
  }
  return ArrayVector();
}

//!
//@Module ERF Error Function
//@@Section MATHFUNCTIONS
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
//mprint erf1
//@>
//which results in the following plot.
//@figure erf1
//!
ArrayVector ErfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erf requires at least one argument");
  Array tmp(arg[0]);
  if (tmp.dataClass() < FM_FLOAT)
    tmp.promoteType(FM_DOUBLE);
  if (tmp.isComplex())
    throw Exception("erf does not work with complex arguments");
  if (tmp.isReferenceType() || tmp.isString())
    throw Exception("erf function requires numerical arguments");
  if (tmp.dataClass() == FM_FLOAT) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    float *sp = (float*) tmp.getDataPointer();
    float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
    for (int i=0;i<olen;i++)
      dp[i] = erff(sp[i]);
    return SingleArrayVector(Array(FM_FLOAT,odims,dp));
  } else if (tmp.dataClass() == FM_DOUBLE) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    double *sp = (double*) tmp.getDataPointer();
    double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
    for (int i=0;i<olen;i++)
      dp[i] = erf(sp[i]);
    return SingleArrayVector(Array(FM_DOUBLE,odims,dp));
  }
  return ArrayVector();
}
  
//!
//@Module GAMMA Gamma Function
//@@Section MATHFUNCTIONS
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
//mprint gamma1
//@>
//which results in the following plot.
//@figure gamma1
//!
ArrayVector GammaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("gamma requires at least one argument");
  Array tmp(arg[0]);
  if (tmp.dataClass() < FM_FLOAT)
    tmp.promoteType(FM_DOUBLE);
  if (tmp.isComplex())
    throw Exception("gamma does not work with complex arguments");
  if (tmp.isReferenceType() || tmp.isString())
    throw Exception("gamma function requires numerical arguments");
  if (tmp.dataClass() == FM_FLOAT) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    float *sp = (float*) tmp.getDataPointer();
    float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
    for (int i=0;i<olen;i++) 
      dp[i] = tgammaf(sp[i]);
    return SingleArrayVector(Array(FM_FLOAT,odims,dp));
  } else if (tmp.dataClass() == FM_DOUBLE) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    double *sp = (double*) tmp.getDataPointer();
    double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
    for (int i=0;i<olen;i++) 
      dp[i] = tgamma(sp[i]);
    return SingleArrayVector(Array(FM_DOUBLE,odims,dp));
  }
  return ArrayVector();
}

//!
//@Module GAMMALN Log Gamma Function
//@@Section MATHFUNCTIONS
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
//mprint gammaln1
//@>
//which results in the following plot.
//@figure gammaln1
//!
ArrayVector GammaLnFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("gammaln requires at least one argument");
  Array tmp(arg[0]);
  if (tmp.dataClass() < FM_FLOAT)
    tmp.promoteType(FM_DOUBLE);
  if (tmp.isComplex())
    throw Exception("gammaln does not work with complex arguments");
  if (tmp.isReferenceType() || tmp.isString())
    throw Exception("gammaln function requires numerical arguments");
  if (tmp.dataClass() == FM_FLOAT) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    float *sp = (float*) tmp.getDataPointer();
    float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
    for (int i=0;i<olen;i++) 
      dp[i] = lgammaf(sp[i]);
    return SingleArrayVector(Array(FM_FLOAT,odims,dp));
  } else if (tmp.dataClass() == FM_DOUBLE) {
    Dimensions odims(tmp.dimensions());
    int olen(odims.getElementCount());
    double *sp = (double*) tmp.getDataPointer();
    double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
    for (int i=0;i<olen;i++) 
      dp[i] = lgamma(sp[i]);
    return SingleArrayVector(Array(FM_DOUBLE,odims,dp));
  }
  return ArrayVector();
}
