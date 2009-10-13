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
#include <math.h>
#include "Operators.hpp"

#if defined(_MSC_VER )
    float erff(float x);
    float erfcf(float x);
    double erf(double x);
    double erfc(double x);
    double tgamma(double x);
    float tgammaf(float x);
    double lgamma(double x);
    float lgammaf(float x);
	double trunc( double x );
	float truncf( float x );
#endif 

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
//@@Tests
//@$near#y1=erfc(x1)
//@@Signature
//function erfc ErfcFunction
//inputs x
//outputs y
//!

struct OpErfc {
  static inline float func(float x) {return erfcf(x);}
  static inline double func(double x) {return erfc(x);}
  static inline void func(float, float, float&, float&) 
  { throw Exception("erfc not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("erfc not defined for complex types");}
};

ArrayVector ErfcFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erfc requires at least one argument");
  return ArrayVector(UnaryOp<OpErfc>(arg[0]));
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
//@@Tests
//@$near#y1=erf(x1)
//@@Signature
//function erf ErfFunction
//inputs x
//outputs y
//!

struct OpErf {
  static inline float func(float x) {return erff(x);}
  static inline double func(double x) {return erf(x);}
  static inline void func(float, float, float&, float&) 
  { throw Exception("erf not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("erf not defined for complex types");}
};

ArrayVector ErfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erf requires at least one argument");
  return ArrayVector(UnaryOp<OpErf>(arg[0]));
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
//@@Tests
//@$near#y1=gamma(x1)
//@@Signature
//function gamma GammaFunction
//inputs x
//outputs y
//!

struct OpGamma {
  static inline float func(float x) {
    if ((x < 0) && (x == truncf(x))) return Inf();
    return tgammaf(x);
  }
  static inline double func(double x) {
    if ((x < 0) && (x == trunc(x))) return Inf();
    return tgamma(x);
  }
  static inline void func(float, float, float&, float&) 
  { throw Exception("gamma not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("gamma not defined for complex types");}
};

ArrayVector GammaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("gamma requires at least one argument");
  return ArrayVector(UnaryOp<OpGamma>(arg[0]));
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
//@@Tests
//@$near#y1=gammaln(x1)
//@@Signature
//function gammaln GammaLnFunction
//inputs x
//outputs y
//!

struct OpGammaLn {
  static inline float func(float x) {
    if (x < 0) return Inf();
    return lgammaf(x);
  }
  static inline double func(double x) {
    if (x < 0) return Inf();
    return lgamma(x);
  }
  static inline void func(float, float, float&, float&) 
  { throw Exception("gammaln not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("gammaln not defined for complex types");}
};

ArrayVector GammaLnFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("gammaln requires at least one argument");
  return ArrayVector(UnaryOp<OpGammaLn>(arg[0]));
}
