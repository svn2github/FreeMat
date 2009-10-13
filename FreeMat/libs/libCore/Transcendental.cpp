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

#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include "Utils.hpp"
#include "Math.hpp"
#include "Operators.hpp"
#include "Complex.hpp"
#include "IEEEFP.hpp"


//!
//@Module LOG1P Natural Logarithm of 1+P Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the @|log| function for one plus its argument.  The general
//syntax for its use is
//@[
//  y = log1p(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Tests
//@$near#y1=log1p(x1)
//@@Signature
//function log1p Log1PFunction
//inputs x
//output y
//!

struct OpLog1P {
  static inline float func(float x) {return log1pf(x);}
  static inline double func(double x) {return log1p(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = logf(complex_abs<float>(xr+1,xi));
    yi = atan2f(xi,xr+1);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = log(complex_abs<double>(xr+1,xi));
    yi = atan2(xi,xr+1);
  }
};

ArrayVector Log1PFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Log1p function takes exactly one argument");
  Array input(arg[0]);
  if (ArrayMin(input) < -1) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpLog1P>(input).toClass(input.dataClass()));
}

//!
//@Module LOG Natural Logarithm Function
//@@Section MATHFUNCTIONS
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
//mprint('logplot');
//@>
//@figure logplot
//@@Tests
//@$near#y1=log(x1)
//@@Signature
//function log LogFunction
//inputs x
//outputs y
//!

struct OpLog {
  static inline float func(float x) {return logf(x);}
  static inline double func(double x) {return log(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = logf(complex_abs<float>(xr,xi));
    yi = atan2f(xi,xr);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = log(complex_abs<double>(xr,xi));
    yi = atan2(xi,xr);
  }
};

ArrayVector LogFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Log function takes exactly one argument");
  Array input(arg[0]);
  if (!IsPositiveOrNaN(input)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpLog>(input));
}

//!
//@Module SQRT Square Root of an Array
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the square root of the argument matrix.  The general
//syntax for its use is
//@[
//   y = sqrt(x)
//@]
//where @|x| is an N-dimensional numerical array.
//@@Example
//Here are some examples of using @|sqrt|
//@<
//sqrt(9)
//sqrt(i)
//sqrt(-1)
//x = rand(4)
//sqrt(x)
//@>
//@@Tests
//@$near#y1=sqrt(x1)
//@@Signature
//function sqrt SqrtFunction
//inputs x
//outputs y
//!

struct OpSqrt {
  static inline float func(float x) {return sqrtf(x);}
  static inline double func(double x) {return sqrt(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    complex_sqrt<float>(xr,xi,yr,yi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    complex_sqrt<double>(xr,xi,yr,yi);
  }
};

ArrayVector SqrtFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Sqrt function takes exactly one argument");
  Array input(arg[0]);
  if (!IsPositiveOrNaN(input)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpSqrt>(input));
}


//!
//@Module TANH Hyperbolic Tangent Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the hyperbolic tangent of the argument.
//The syntax for its use is
//@[
//   y = tanh(x)
//@]
//@@Function Internals
//The @|tanh| function is computed from the formula
//\[
//   \tanh(x) = \frac{\sinh(x)}{\cosh(x)}
//\]
//@@Examples
//Here is a simple plot of the hyperbolic tangent function
//@<
//x = linspace(-5,5);
//plot(x,tanh(x)); grid('on');
//mprint('tanhplot');
//@>
//@figure tanhplot
//@@Tests
//@$near#y1=tanh(x1)
//@@Signature
//function tanh TanhFunction
//inputs x
//outputs y
//!

//  e^r*c(i)+e^(-r)*c(-i)/2 + i

struct OpTanh {
  static inline float func(float x) {return tanhf(x);}
  static inline double func(double x) {return tanh(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    float cr = (expf(xr)*cosf(xi)+expf(-xr)*cosf(-xi))/2;
    float ci = (expf(xr)*sinf(xi)+expf(-xr)*sinf(-xi))/2;
    float sr = (expf(xr)*cosf(xi)-expf(-xr)*cosf(-xi))/2;
    float si = (expf(xr)*sinf(xi)-expf(-xr)*sinf(-xi))/2;
    complex_divide(sr,si,cr,ci,yr,yi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    double cr = (exp(xr)*cos(xi)+exp(-xr)*cos(-xi))/2;
    double ci = (exp(xr)*sin(xi)+exp(-xr)*sin(-xi))/2;
    double sr = (exp(xr)*cos(xi)-exp(-xr)*cos(-xi))/2;
    double si = (exp(xr)*sin(xi)-exp(-xr)*sin(-xi))/2;
    //     double cr = (exp(xr)*cos(xi)+exp(-xr)*cos(-xi))/2;
    //     double ci = (exp(xr)*sin(xi)+exp(-xr)*sin(-xi))/2;
    //     double sr = (exp(xr)*cos(xi)-exp(-xr)*cos(-xi))/2;
    //     double si = (exp(xr)*sin(xi)-exp(-xr)*sin(-xi))/2;
    complex_divide(sr,si,cr,ci,yr,yi);
  }
};

ArrayVector TanhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Tanh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpTanh>(arg[0]));
}

//!
//@Module ACOSH Inverse Hyperbolic Cosine Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the inverse hyperbolic cosine of its argument.  The general
//syntax for its use is
//@[
//  y = acosh(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Function Internals
//The @|acosh| function is computed from the formula
//\[
//   \cosh^{-1}(x) = \log\left(x + (x^2 - 1)^0.5\right)
//\]
//where the @|log| (and square root) is taken in its most general sense.
//@@Examples
//Here is a simple plot of the inverse hyperbolic cosine function
//@<
//x = linspace(1,pi);
//plot(x,acosh(x)); grid('on');
//mprint('acoshplot');
//@>
//@figure acoshplot
//@@Tests
//@$near#y1=acosh(x1)
//@@Signature
//function acosh ArccoshFunction
//inputs x
//outputs y
//!

// log(x+sqrt(x^2-1))
struct OpArccosh {
  static inline float func(float x) {return acoshf(x);}
  static inline double func(double x) {return acosh(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    if (xr == -Inf() && xi == 0) {
      yr = Inf();
      yi = M_PI;
      return;
    }
    T xrt_real1, xrt_imag1;
    T xrt_real2, xrt_imag2;
    complex_sqrt(xr+1,xi,xrt_real1,xrt_imag1);
    complex_sqrt(xr-1,xi,xrt_real2,xrt_imag2);
    T y_real, y_imag;
    complex_multiply(xrt_real1,xrt_imag1,
		     xrt_real2,xrt_imag2,
		     y_real,y_imag);
    y_real += xr;
    y_imag += xi;
    complex_log(y_real,y_imag,yr,yi);
  }
};

ArrayVector ArccoshFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("acosh function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && (ArrayMin(input) <= 1)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpArccosh>(input));
}

//!
//@Module ASINH Inverse Hyperbolic Sine Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the inverse hyperbolic sine of its argument.  The general
//syntax for its use is
//@[
//  y = asinh(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Function Internals
//The @|asinh| function is computed from the formula
//\[
//   \sinh^{-1}(x) = \log\left(x + (x^2 + 1)^0.5\right)
//\]
//where the @|log| (and square root) is taken in its most general sense.
//@@Examples
//Here is a simple plot of the inverse hyperbolic sine function
//@<
//x = -5:.01:5;
//plot(x,asinh(x)); grid('on');
//mprint('asinhplot');
//@>
//@figure asinhplot
//@@Tests
//@$near#y1=asinh(x1)
//@@Signature
//function asinh ArcsinhFunction
//inputs x
//outputs y
//!

// log(x+sqrt(x^2-1))
struct OpArcsinh {
  static inline float func(float x) {return asinhf(x);}
  static inline double func(double x) {return asinh(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    T xsq_real, xsq_imag;
    complex_square(xr,xi,xsq_real,xsq_imag);
    xsq_real += 1;
    T xrt_real, xrt_imag;
    complex_sqrt(xsq_real,xsq_imag,xrt_real,xrt_imag);
    xrt_real += xr;
    xrt_imag += xi;
    complex_log(xrt_real,xrt_imag,yr,yi);
  }
};

ArrayVector ArcsinhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("acosh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpArcsinh>(arg[0]));
}


//!
//@Module ASECH Inverse Hyperbolic Secant Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the inverse hyperbolic secant of its argument.  The general
//syntax for its use is
//@[
//  y = asech(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Function Internals
//The @|asech| function is computed from the formula
//\[
//   \mathrm{sech}^{-1}(x) = \cosh^{-1}\left(\frac{1}{x}\right)
//\]
//@@Examples
//Here is a simple plot of the inverse hyperbolic secant function
//@<
//x1 = -20:.01:-1;
//x2 = 1:.01:20;
//plot(x1,imag(asech(x1)),x2,imag(asech(x2))); grid('on');
//mprint('asechplot');
//@>
//@figure asechplot
//@@Tests
//@$near#y1=asech(x1)
//@@Signature
//function asech ArcsechFunction
//inputs x
//outputs y
//!
struct OpArcsech {
  template <typename T>
  static inline T func(T x) {
    if (x == 0)
      return Inf();
    return log(sqrt(1/x-1)*sqrt(1/x+1)+1/x);
  }
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    if ((xr == 0) && (xi == 0)) {
      yr = Inf();
      yi = 0;
      return;
    }
    T xrp_real, xrp_imag;
    complex_recip(xr,xi,xrp_real,xrp_imag);
    OpArccosh::func(xrp_real,xrp_imag,yr,yi);
  }
};

ArrayVector ArcsechFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("asech function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && ((ArrayMin(input) < 0) || (ArrayMax(input) > 1))) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpArcsech>(input));
}



//!
//@Module ATANH Inverse Hyperbolic Tangent Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the inverse hyperbolic tangent of its argument.  The general
//syntax for its use is
//@[
//  y = atanh(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Function Internals
//The @|atanh| function is computed from the formula
//\[
//   \tanh^{-1}(x) = \frac{1}{2}\log\left(\frac{1+x}{1-x}\right)
//\]
//where the @|log| (and square root) is taken in its most general sense.
//@@Examples
//Here is a simple plot of the inverse hyperbolic tangent function
//@<
//x = -0.99:.01:0.99;
//plot(x,atanh(x)); grid('on');
//mprint('atanhplot');
//@>
//@figure atanhplot
//@@Tests
//@$near#y1=atanh(x1)
//@@Signature
//function atanh ArctanhFunction
//inputs x
//outputs y
//!
struct OpArctanh {
  static inline float func(float x) {return atanhf(x);}
  static inline double func(double x) {return atanh(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
//     if ((xr == 1) && (xi == 0)) {
//       yr = Inf();
//       yi = 0;
//       return;
//     }
    if ((xr == Inf()) && (xi == 0)) {
      yr = 0;
      yi = M_PI/2.0;
      return;
    }
    if ((xr == -Inf()) && (xi == 0)) {
      yr = 0;
      yi = -M_PI/2.0;
      return;
    }
    T xa, xb;
    T ya, yb;
    complex_log(xr+1,xi,xa,xb);
    complex_log(1-xr,-xi,ya,yb);
    yr = (xa-ya)/2;
    yi = (xb-yb)/2;
  }
};

ArrayVector ArctanhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("atanh function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && (ArrayRange(input) >= 1)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpArctanh>(input));
}

//!
//@Module COSH Hyperbolic Cosine Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the hyperbolic cosine of the argument.
//The syntax for its use is
//@[
//   y = cosh(x)
//@]
//@@Function Internals
//The @|cosh| function is computed from the formula
//\[
//   \cosh(x) = \frac{e^x+e^{-x}}{2}
//\]
//For @|x| complex, it follows that
//\[
//   \cosh(a+i*b) = \frac{e^a(\cos(b)+i*\sin(b)) + e^{-a}(\cos(-b)+i*\sin(-b))}{2}
//\]
//@@Examples
//Here is a simple plot of the hyperbolic cosine function
//@<
//x = linspace(-5,5);
//plot(x,cosh(x)); grid('on');
//mprint('coshplot');
//@>
//@figure coshplot
//@@Tests
//@$near#y1=cosh(x1)
//@@Signature
//function cosh CoshFunction
//inputs x
//outputs y
//!

struct OpCosh {
  static inline float func(float x) {return coshf(x);}
  static inline double func(double x) {return cosh(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = (expf(xr)*cosf(xi)+expf(-xr)*cosf(-xi))/2;
    yi = (expf(xr)*sinf(xi)+expf(-xr)*sinf(-xi))/2;
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = (exp(xr)*cos(xi)+exp(-xr)*cos(-xi))/2;
    yi = (exp(xr)*sin(xi)+exp(-xr)*sin(-xi))/2;    
  }
};

ArrayVector CoshFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Cosh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCosh>(arg[0]));
}

//!
//@Module SINH Hyperbolic Sine Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes the hyperbolic sine of the argument.
//The syntax for its use is
//@[
//   y = sinh(x)
//@]
//@@Function Internals
//The @|sinh| function is computed from the formula
//\[
//   \sinh(x) = \frac{e^x-e^{-x}}{2}
//\]
//@@Examples
//Here is a simple plot of the hyperbolic sine function
//@<
//x = linspace(-5,5);
//plot(x,sinh(x)); grid('on');
//mprint('sinhplot');
//@>
//@figure sinhplot
//@@Tests
//@$near#y1=sinh(x1)
//@@Signature
//function sinh SinhFunction
//inputs x
//outputs y
//!

struct OpSinh {
  static inline float func(float x) {return sinhf(x);}
  static inline double func(double x) {return sinh(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = (expf(xr)*cosf(xi)-expf(-xr)*cosf(-xi))/2;
    yi = (expf(xr)*sinf(xi)-expf(-xr)*sinf(-xi))/2;    
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = (exp(xr)*cos(xi)-exp(-xr)*cos(-xi))/2;
    yi = (exp(xr)*sin(xi)-exp(-xr)*sin(-xi))/2; 
  }
};

ArrayVector SinhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Sinh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpSinh>(arg[0]));
}


//!
//@Module EXP Exponential Function
//@@Section MATHFUNCTIONS
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
//mprint('expplot1');
//@>
//@figure expplot1
//In the second example, we plot the unit circle in the 
//complex plane @|e^{i 2 pi x}| for @|x in [-1,1]|.
//@<
//x = linspace(-1,1);
//plot(exp(-i*x*2*pi))
//mprint('expplot2');
//@>
//@figure expplot2
//@@Tests
//@$near#y1=exp(x1)
//@@Signature
//function exp ExpFunction
//inputs x
//outputs y
//!

struct OpExp {
  static inline float func(float x) {return expf(x);}
  static inline double func(double x) {return exp(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    yr = expf(xr)*cosf(xi);
    yi = expf(xr)*sinf(xi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    yr = exp(xr)*cos(xi);
    yi = exp(xr)*sin(xi);
  }
};

ArrayVector ExpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Exp function takes exactly one argument");
  return ArrayVector(UnaryOp<OpExp>(arg[0]));
}


//!
//@Module EXPM1 Exponential Minus One Function
//@@Section MATHFUNCTIONS
//@@Usage
//Computes @|exp(x)-1| function accurately for @|x|
//small.  The syntax for its use is
//@[
//   y = expm1(x)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//@@Tests
//@$near#y1=expm1(x1)
//@@Signature
//function expm1 ExpM1Function
//inputs x
//outputs y
//!

// exp(x)-1 for x = xr+i*xi
// exp(xr+i*xi) - 1 = exp(xr)*cos(xi) + i*exp(xr)*sin(xi) - 1

struct OpExpM1 {
  static inline float func(float x) {return expm1f(x);}
  static inline double func(double x) {return expm1(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    yr = expf(xr)*cosf(xi) - 1;
    yi = expf(xr)*sinf(xi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    yr = exp(xr)*cos(xi) - 1;
    yi = exp(xr)*sin(xi);
  }
};

ArrayVector ExpM1Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("ExpM1 function takes exactly one argument");
  return ArrayVector(UnaryOp<OpExpM1>(arg[0]));
}


