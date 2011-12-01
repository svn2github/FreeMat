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
#include "mathfunc5.hpp"

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
//function erfc ErfcFunction jitsafe
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

JitScalarFunc1(erfc,OpErfc::func);

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
//function erf ErfFunction jitsafe
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

JitScalarFunc1(erf,OpErf::func);
  
//!
//@Module ERFINV Inverse Error Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the inverse error function for each element of x.  The @|erf|
//function takes only a single argument
//@[
//  y = erfinv(x)
//@]
//where @|x| is either a @|float| or @|double| array.  The output
//vector @|y| is the same size (and type) as @|x|. For values outside the interval [-1, 1] function returns NaN.
//@@Example
//Here is a plot of the erf function over the range @|[-.9,.9]|.
//@<
//x = linspace(-.9,.9,100);
//y = erfinv(x);
//plot(x,y); xlabel('x'); ylabel('erfinv(x)');
//mprint erfinv1
//@>
//which results in the following plot.
//@figure erfinv1
//@@Tests
//@$near#y1=erfinv(x1)
//@@Signature
//function erfinv ErfInvFunction jitsafe
//inputs x
//outputs y
//!

struct OpErfInv{
  static inline float func(float x) {return erfinv(x);}
  static inline double func(double x) {return erfinv(x);}
  static inline void func(float, float, float&, float&) 
  { throw Exception("erf not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("erf not defined for complex types");}
};

ArrayVector ErfInvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erf requires at least one argument");
  return ArrayVector(UnaryOp<OpErfInv>(arg[0]));
}

JitScalarFunc1(erfinv,OpErfInv::func);

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
//function gamma GammaFunction jitsafe
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
  
  ArrayVector ret;
  try{
	ret = UnaryOp<OpGamma>(arg[0]);
  }
  catch(...){
	throw Exception("Error evaluating gamma function");
  }
  return ret;
}

JitScalarFunc1(gamma,OpGamma::func);

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
//function gammaln GammaLnFunction jitsafe
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

JitScalarFunc1(gammaln,OpGammaLn::func);

//!
//@Module BETAINC Incomplete Beta Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the incomplete beta function.  The @|betainc|
//function takes 3 or 4 arguments
//@[
//  A = betainc(X,Y,Z)
//@]
//@[
//  A = betainc(X,Y,Z,tail)
//@]
//where @|X| is either a @|float| or @|double| array with elements in [0,1] interval, @|Y| and @|Z| are real non-negative arrays. 
//@|tail| specifies the tail of the incomplete beta function. 
//If @|tail| is 'lower' (default) than the integral from 0 to x is computed. If @|tail| is 'upper' than the integral from x to 1 is computed.
//All arrays must be the same size or be scalar. The output
//vector @|A| is the same size (and type) as input arrays.
//@@Function Internals
//The incomplete beta function is defined by the integral:
//\[
//  BetaI_x(a,b)=B_x(a,b)/B(a,b) where B_x(a,b) = \int_0^x t^{a-1} (1-t)^{b-1} dt 
//  for 0 <= x <= 1. For a > 0, b > 0
//\]
//@@Example
//Here is a plot of the betainc function over the range @|[.2,.8]|.
//@<
//x=.2:.01:.8;
//y = betainc(x,5,3);
//plot(x,y); xlabel('x'); ylabel('betainc(x,5,3)');
//mprint betainc1
//@>
//which results in the following plot.
//@figure betainc1
//@@Tests
//@$near#y1=betainc(x1,5,3)
//@@Signature
//function betainc BetaIncFunction
//inputs varargin
//outputs y
//!
#ifdef HAVE_BOOST
#include <boost/math/special_functions/beta.hpp>

ArrayVector BetaIncFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("betainc requires at least three arguments");
  ArrayVector retVec;
  Array X( arg[0] );
  Array Y( arg[1] );
  Array Z( arg[2] );
  int maxLen = std::max( X.length(), std::max( Y.length(), Z.length() ) );
  NTuple retDims = max( X.dimensions(), max( Y.dimensions(), Z.dimensions() ) );
  
  if( !(X.isScalar()) && retDims != X.dimensions() )
    throw Exception("wrong size of the first argument");
  if( !(Y.isScalar()) && retDims != Y.dimensions() )
    throw Exception("wrong size of the second argument");
  if( !(Z.isScalar()) && retDims != Z.dimensions() )
    throw Exception("wrong size of the third argument");
  
  if( X.dataClass() == Double && Y.dataClass() == Double && Z.dataClass() == Double ){
    BasicArray< double > result( retDims );
    for( int i = 1; i <= maxLen; ++i ){
      double x,y,z,r;
      x = (X.isScalar()) ? X.constRealScalar<double>() : X.real<double>()[i];
      y = (Y.isScalar()) ? Y.constRealScalar<double>() : Y.real<double>()[i];
      z = (Z.isScalar()) ? Z.constRealScalar<double>() : Z.real<double>()[i];
	  try{
		result[i] = boost::math::ibeta( y, z, x );
	  }
	  catch(...){
		throw Exception("Error evaluating ibeta");
      }
	}
    retVec.push_back( result );
  }
  else if( X.dataClass() == Float && Y.dataClass() == Float && Z.dataClass() == Float ){
    BasicArray< float > result( retDims );
    for( int i = 1; i <= maxLen; ++i ){
      float x,y,z,r;
      x = (X.isScalar()) ? X.realScalar<float>() : X.real<float>()[i];
      y = (Y.isScalar()) ? Y.realScalar<float>() : Y.real<float>()[i];
      z = (Z.isScalar()) ? Z.realScalar<float>() : Z.real<float>()[i];
	  try{
		result[i] = boost::math::ibeta( y, z, x );
	  }
	  catch(...){
		throw Exception("Error evaluating ibeta");
	  }
    }
    retVec.push_back( result );
  }
  else{
    throw Exception("Inputs must be either double or single");
  }
  return retVec;
}
#else
ArrayVector BetaIncFunction(int nargout, const ArrayVector& arg) {
    throw Exception("FreeMat must be compiled with boost to enable betainc");
    return ArrayVector();
}
#endif

//!
//@Module LEGENDRE Associated Legendre Polynomial
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the associated Legendre function of degree n. 
//@[
//  y = legendre(n,x)
//@]
//where @|x| is either a @|float| or @|double| array in range @|[-1,1]|, @|n| is integer scalar.  The output
//vector @|y| is the same size (and type) as @|x|.
//@@Example
//Here is a plot of the @|legendre| function over the range @|[-1,1]|.
//@<
//x = linspace(-1,1,30);
//y = legendre(4,x);
//plot(x,y); xlabel('x'); ylabel('legendre(4,x)');
//mprint legendre
//@>
//which results in the following plot.
//@figure legendre
//@@Tests
//@$near#y1=legendre(3,x1)
//@@Signature
//function legendre LegendreFunction
//inputs varargin
//outputs y
//!

#ifdef HAVE_BOOST
#include <boost/math/special_functions/legendre.hpp>
ArrayVector LegendreFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("Legendre function requires at least two argument");
  Array x( arg[1] );
  Array n( arg[0] );
  NTuple retDims;
  ArrayVector retVec;
  
  if( x.isComplex() )
    throw Exception("Second argument must be real");

  if( n.isComplex() )
    throw Exception("First argument must be real integer");
  
  if( x.dimensions().count() > 1 && !n.isScalar() )
    throw Exception("If second argument is a not a vector or scalar, then first argument must be scalar");
  
  if( (n.dimensions().count() == 2 && n.dimensions()[1] != x.length()) || (n.dimensions().count() > 2) )
    throw Exception("Incompatible dimensions between first and second argument");
  
  if( n.isScalar() )
    retDims = x.dimensions();
  else
    retDims = n.dimensions();
  
  if( x.dataClass() == Double ){
    BasicArray< double > result( retDims );
    if( n.isScalar() ){
      for( int i=1; i<=x.length(); ++i ){
	double xt = (x.isScalar()) ? x.constRealScalar<double>() : x.real<double>()[i];
	try{
		result[i]=boost::math::legendre_p<double>(n.constRealScalar<double>(), xt);
	}
	catch(...){
		throw Exception("Error evaluating legendre");
	}
      }
    }
    retVec.push_back(result); 
  }
  else if( x.dataClass() == Float ){
    BasicArray< float > result( retDims );
    if( n.isScalar() ){
      for( int i=1; i<=x.length(); ++i ){
	float xt = (x.isScalar()) ? x.constRealScalar<float>() : x.real<float>()[i];
	try{
		result[i]=boost::math::legendre_p<float>(n.constRealScalar<float>(), xt);
	}
	catch(...){
		throw Exception("Error evaluating legendre");
	}
      }
    }
    retVec.push_back(result); 
  }
  else
    throw Exception("Second argument must be double or single");
  return retVec;
}
#else
ArrayVector LegendreFunction(int nargout, const ArrayVector& arg) {
    throw Exception("FreeMat must be compiled with boost to enable legendre");
    return ArrayVector();
}
#endif
