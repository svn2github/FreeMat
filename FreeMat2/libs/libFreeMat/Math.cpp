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

#include "Array.hpp"
#include "Exception.hpp"
#include "Math.hpp"
#include "MatrixMultiply.hpp"
#include "LinearEqSolver.hpp"
#include "LeastSquaresSolver.hpp"
#include "EigenDecompose.hpp"
#include "Malloc.hpp"
#include "Sparse.hpp"
#include <math.h>

// Sparse ops: +, -, neg, *

void boolean_and(int N, logical* C, const logical *A, int Astride, 
		 const logical *B, int Bstride) {
  int m,p;
  m = 0;
  p = 0;
  for (int i=0;i<N;i++) {
    C[i] = A[m] && B[p];
    m += Astride;
    p += Bstride;
  }
}

void boolean_or(int N, logical* C, const logical *A, int Astride, 
		const logical *B, int Bstride) {
  int m,p;
  m = 0;
  p = 0;
  for (int i=0;i<N;i++) {
    C[i] = A[m] || B[p];
    m += Astride;
    p += Bstride;
  }
}

void boolean_not(int N, logical* C, const logical *A) {
  for (int i=0;i<N;i++)
    C[i] = !A[i];
}

template <class T>
void neg(int N, T* C, const T*A) {
  for (int i=0;i<N;i++)
    C[i] = -A[i];
}


template <class T>
void addfullreal(int N, T*C, const T*A, int stride1, 
		 const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = A[m] + B[p];
    m += stride1;
    p += stride2;
  }
}

template <class T>
void addfullcomplex(int N, T*C, const T*A, int stride1, 
		    const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[2*i] = A[2*m] + B[2*p];
    C[2*i+1] = A[2*m+1] + B[2*p+1];
    m += stride1;
    p += stride2;
  }
}

template <class T>
void subtractfullreal(int N, T*C, const T*A, int stride1, 
		      const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = A[m] - B[p];
    m += stride1;
    p += stride2;
  }
}

template <class T>
void subtractfullcomplex(int N, T*C, const T*A, int stride1, 
			 const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[2*i] = A[2*m] - B[2*p];
    C[2*i+1] = A[2*m+1] - B[2*p+1];
    m += stride1;
    p += stride2;
  }
}


template <class T>
void multiplyfullreal(int N, T*C, const T*A, int stride1, 
		      const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = A[m] * B[p];
    m += stride1;
    p += stride2;
  }
}

template <class T>
void multiplyfullcomplex(int N, T*C, const T*A, int stride1, 
			 const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[2*i] = A[2*m] * B[2*p] - A[2*m+1] * B[2*p+1];
    C[2*i+1] = A[2*m] * B[2*p+1] + A[2*m+1] * B[2*p];
    m += stride1;
    p += stride2;
  }
}


template <class T>
void dividefullreal(int N, T*C, const T*A, int stride1, 
		    const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = A[m] / B[p];
    m += stride1;
    p += stride2;
  }
}

template <class T>
void complex_divide(T* c, const T* a, const T* b) {
  double ratio, den;
  double abr, abi, cr;
    
  if( (abr = b[0]) < 0.)
    abr = - abr;
  if( (abi = b[1]) < 0.)
    abi = - abi;
  if( abr <= abi )
    {
      if(abi == 0) {
	if (a[1] != 0 || a[0] != 0)
	  abi = 1.;
	c[1] = c[0] = abi / abr;
	return;
      }
      ratio = b[0] / b[1] ;
      den = b[1] * (1 + ratio*ratio);
      cr = (a[0]*ratio + a[1]) / den;
      c[1] = (a[1]*ratio - a[0]) / den;
    }
  else
    {
      ratio = b[1] / b[0] ;
      den = b[0] * (1 + ratio*ratio);
      cr = (a[0] + a[1]*ratio) / den;
      c[1] = (a[1] - a[0]*ratio) / den;
    }
  c[0] = cr;
}

template <class T>
void dividefullcomplex(int N, T*C, const T*A, int stride1, const T*B, 
		       int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    complex_divide<T>(C+2*i,A+2*m,B+2*p);
    m += stride1;
    p += stride2;
  }
}
  
template <class T>
void lessthanfuncreal(int N, logical* C, const T*A, int stride1, const T*B, 
		      int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (A[m] < B[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}



template <class T>
void lessthanfunccomplex(int N, logical* C, const T*A, int stride1, 
			 const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (complex_abs<T>(A[2*m],A[2*m+1]) < 
	    complex_abs<T>(B[2*p],B[2*p+1])) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void lessequalsfuncreal(int N, logical* C, const T*A, int stride1, const T*B, 
			int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (A[m] <= B[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void lessequalsfunccomplex(int N, logical* C, const T*A, int stride1, 
			   const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (complex_abs<T>(A[2*m],A[2*m+1]) <= 
	    complex_abs<T>(B[2*p],B[2*p+1])) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void equalsfuncreal(int N, logical* C, const T*A, int stride1, const T*B, 
		    int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (A[m] == B[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void equalsfunccomplex(int N, logical* C, const T*A, int stride1, 
		       const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = ((A[2*m] == B[2*p]) && 
	    (A[2*m+1] == B[2*p+1])) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void notequalsfuncreal(int N, logical* C, const T*A, int stride1, const T*B, 
		       int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (A[m] != B[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void notequalsfunccomplex(int N, logical* C, const T*A, int stride1, 
			  const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = ((A[2*m] != B[2*p]) ||
	    (A[2*m+1] != B[2*p+1])) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void greaterthanfuncreal(int N, logical* C, const T*A, int stride1, const T*B, 
			 int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (A[m] > B[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void greaterthanfunccomplex(int N, logical* C, const T*A, int stride1, 
			    const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (complex_abs<T>(A[2*m],A[2*m+1]) >
	    complex_abs<T>(B[2*p],B[2*p+1])) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void greaterequalsfuncreal(int N, logical* C, const T*A, int stride1, 
			   const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (A[m] >= B[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

template <class T>
void greaterequalsfunccomplex(int N, logical* C, const T*A, int stride1, 
			      const T*B, int stride2) {
  int m, p;
  m = 0; p = 0;
  for (int i=0;i<N;i++) {
    C[i] = (complex_abs<T>(A[2*m],A[2*m+1]) >= 
	    complex_abs<T>(B[2*p],B[2*p+1])) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}

/**
 * These are the power functions...
 */
// cicPower --> pow_zi
// cfcPower --> pow_zz
// zdzPower --> pow_zz
// cccPower --> pow_zz
// zzzPower --> pow_zz
// zizPower --> pow_zi
// didPower --> pow_di
// dddPower --> pow_dd
// fifPower --> pow_ri
// fffPower --> pow_dd

void power_zi(double *p, const double *a, int b) 	/* p = a**b  */
{
  int n;
  unsigned long u;
  double t;
  double q[2], x[2];
  static double one[2] = {1.0, 0.0};
  n = b;
  q[0] = 1;
  q[1] = 0;
    
  if(n == 0)
    goto done;
  if(n < 0)
    {
      n = -n;
      complex_divide<double>(x,one,a);
    }
  else
    {
      x[0] = a[0];
      x[1] = a[1];
    }
  for(u = n; ; )
    {
      if(u & 01)
	{
	  t = q[0] * x[0] - q[1] * x[1];
	  q[1] = q[0] * x[1] + q[1] * x[0];
	  q[0] = t;
	}
      if(u >>= 1)
	{
	  t = x[0] * x[0] - x[1] * x[1];
	  x[1] = 2 * x[0] * x[1];
	  x[0] = t;
	}
      else
	break;
    }
 done:
  p[1] = q[1];
  p[0] = q[0];
}

void power_zz(double *c, const double *a, const double *b) 
{
  double logr, logi, x, y;
  double mag = complex_abs<double>(a[0], a[1]);
  if (mag == 0) {
    c[0] = 0;
    c[1] = 0;
    return;
  }
  logr = log(mag);
  logi = atan2(a[1], a[0]);
    
  x = exp( logr * b[0] - logi * b[1] );
  y = logr * b[1] + logi * b[0];
    
  c[0] = x * cos(y);
  c[1] = x * sin(y);
}

double power_di(double a, int b) {
  double pow, x;
  int n;
  unsigned long u;
    
  pow = 1;
  x = a;
  n = b;

  if(n != 0)
    {
      if(n < 0)
	{
	  n = -n;
	  x = 1/x;
	}
      for(u = n; ; )
	{
	  if(u & 01)
	    pow *= x;
	  if(u >>= 1)
	    x *= x;
	  else
	    break;
	}
    }
  return(pow);
}  

double power_dd(double a, double b) {
  return pow(a,b);
}
  
  
void cicpower(int n, float *c, float *a, int stride1, int *b, 
	      int stride2) {
  int m, p;
  double z1[2];
  double z3[2];
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    z1[0] = a[2*m];
    z1[1] = a[2*m+1];
    power_zi(z3,z1,b[p]);
    c[2*i] = z3[0];
    c[2*i+1] = z3[1];
    m += stride1;
    p += stride2;
  }
}
  
void cfcpower(int n, float *c, float *a, int stride1, float *b, 
	      int stride2) {
  int m, p;
  double z1[2];
  double z2[2];
  double z3[2];
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    z1[0] = a[2*m];
    z1[1] = a[2*m+1];
    z2[0] = b[p];
    z2[1] = 0;
    power_zz(z3,z1,z2);
    c[2*i] = z3[0];
    c[2*i+1] = z3[1];
    m += stride1;
    p += stride2;
  }
} 
  
void zdzpower(int n, double *c, double *a, int stride1, 
	      double *b, int stride2) { 
  int m, p;
  double z1[2];
  double z2[2];
  double z3[2];
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    z1[0] = a[2*m];
    z1[1] = a[2*m+1];
    z2[0] = b[p];
    z2[1] = 0;
    power_zz(z3,z1,z2);
    c[2*i] = z3[0];
    c[2*i+1] = z3[1];
    m += stride1;
    p += stride2;
  }
}
  
void cccpower(int n, float *c, float *a, int stride1, float *b, 
	      int stride2) {
  int m, p;
  double z1[2];
  double z2[2];
  double z3[2];
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    z1[0] = a[2*m];
    z1[1] = a[2*m+1];
    z2[0] = b[2*p];
    z2[1] = b[2*p+1];
    power_zz(z3,z1,z2);
    c[2*i] = z3[0];
    c[2*i+1] = z3[1];
    m += stride1;
    p += stride2;
  }
}
  
void zzzpower(int n, double *c, double *a, int stride1, double *b, 
	      int stride2) {
  int m, p;
  double z1[2];
  double z2[2];
  double z3[2];
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    z1[0] = a[2*m];
    z1[1] = a[2*m+1];
    z2[0] = b[2*p];
    z2[1] = b[2*p+1];
    power_zz(z3,z1,z2);
    c[2*i] = z3[0];
    c[2*i+1] = z3[1];
    m += stride1;
    p += stride2;
  }
}

void zizpower(int n, double *c, double *a, int stride1, int *b,
	      int stride2) {
  int m, p;
  double z1[2];
  double z3[2];
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    z1[0] = a[2*m];
    z1[1] = a[2*m+1];
    power_zi(z3,z1,b[p]);
    c[2*i] = z3[0];
    c[2*i+1] = z3[1];
    m += stride1;
    p += stride2;
  }
}

void didpower(int n, double *c, double *a, int stride1, int *b,
	      int stride2) {
  int m, p;
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    c[i] = power_di(a[m],b[p]);
    m += stride1;
    p += stride2;
  }
}

void dddpower(int n, double *c, double *a, int stride1, double *b,
	      int stride2) {
  int m, p;
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    c[i] = power_dd(a[m],b[p]);
    m += stride1;
    p += stride2;
  }
}

void fifpower(int n, float *c, float *a, int stride1, int *b,
	      int stride2) {
  int m, p;
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    c[i] = power_di(a[m],b[p]);
    m += stride1;
    p += stride2;
  }
}

void fffpower(int n, float *c, float *a, int stride1, float *b,
	      int stride2) {
  int m, p;
  m = 0;
  p = 0;
  for (int i=0;i<n;i++) {
    c[i] = power_dd(a[m],b[p]);
    m += stride1;
    p += stride2;
  }
}

/**
 * This is the generic function interface into calculations
 * that can be performed on some type.
 */
typedef void (*vvfun) (const int length, 
		       void* result,
		       const void* arg1,
		       const int stride1,
		       const void* arg2,
		       const int stride2);

/**
 * This structure holds pointers to functions when
 * all types can be handled.
 */

typedef struct {
  bool  promoteInt32ToDouble;
  vvfun int32func;
  vvfun floatfunc;
  vvfun doublefunc;
  vvfun complexfunc;
  vvfun dcomplexfunc;
} packVectorVector;

/**
 * Check that both of the argument objects are numeric.
 */
inline void CheckNumeric(Array &A, Array &B, std::string opname){
  bool Anumeric, Bnumeric;

  Anumeric = !A.isReferenceType();
  Bnumeric = !B.isReferenceType();
  if (!(Anumeric && Bnumeric))
    throw Exception(std::string("Cannot apply numeric operation ") + 
		    opname + std::string(" to reference types."));
}

/**
 * Check that both of the argument objects are of the
 * same type.  If not, apply conversion.  Here are the
 * rules:
 *   Float operations produce float results.
 *   Double operations produce double results.
 *   Mixtures of float and double operations produce double results.
 *   Integer operations (of any kind) produce int results - 
 *     the question is what type do these objects get promoted too?  The
 *     answer is a signed int (32 bits).
 *   Mixtures of real and complex operations produce complex results.
 *   Integer constants are 32 bit by default
 *   Float constants are 64 bit by default
 *   Division operations lead to promotion 
 *   Character types are automatically (demoted) to 32 bit integers.
 *
 *   The way to accomplish this is as follows: compute the larger
 *   of the types of A and B.  Call this type t_max.  If t_max
 *   is not an integer type, promote both types to this type.
 *   If t_max is an integer type, promote both objects to an FM_INT32.
 *   If this is a division operation or a matrix operation, promote both 
 *    objects to an FM_DOUBLE64!
 *
 */

//!
//@Module TYPERULES Type Rules
//@@Section FreeMat
//@@Usage
//FreeMat follows an extended form of C's type rules (the extension
//is to handle complex data types.  The general rules are as follows:
//\begin{itemize}
//  \item Integer types are promoted to @|int32| types, except
//        for matrix operations and division operations.
//  \item Mixtures of @|float| and @|complex| types produce @|complex|
//        outputs.
//  \item Mixtures of @|double| or @|int32| types and @|dcomplex|
//        types produce @|dcomplex| outputs.
//  \item Arguments to operators are promoted to the largest type
//        present among the operands.
//  \item Type promotion is not allowed to reduce the information
//        content of the variable.  The only exception to this is
//        64-bit integers, which can lose information when they
//        are promoted to 64-bit @|double| values.  
//\end{itemize}
//These rules look tricky, but in reality, they are designed so that
//you do not actively have to worry about the types when performing
//mathematical operations in FreeMat.  The flip side of this, of course
//is that unlike C, the output of numerical operations is not automatically
//typecast to the type of the variable you assign the value to. 
//@@Tests
//@$"y=1+2","3","exact"
//@$"y=1f*i","i","exact"
//@$"y=pi+i","dcomplex(pi+i)","close"
//@$"y=1/2","0.5","exact"
//!
void TypeCheck(Array &A, Array &B, bool isDivOrMatrix) {
  Class Aclass(A.dataClass());
  Class Bclass(B.dataClass());
  Class Cclass;
  // First, anything less than an int32 is promoted to an int32
  // Strings are treates as int32.
  if ((Aclass < FM_INT32) || (Aclass == FM_STRING)) Aclass = FM_INT32;
  if ((Bclass < FM_INT32) || (Bclass == FM_STRING)) Bclass = FM_INT32;
  // Division or matrix operations do no allow integer
  // data types.  These must be promoted to doubles.
  if (isDivOrMatrix && (Aclass < FM_FLOAT)) Aclass = FM_DOUBLE;
  if (isDivOrMatrix && (Bclass < FM_FLOAT)) Bclass = FM_DOUBLE;
  // An integer or double mixed with a complex is promoted to a dcomplex type
  if ((Aclass == FM_COMPLEX) && ((Bclass == FM_DOUBLE) || (Bclass < FM_FLOAT))) Bclass = FM_DCOMPLEX;
  if ((Bclass == FM_COMPLEX) && ((Aclass == FM_DOUBLE) || (Aclass < FM_FLOAT))) Aclass = FM_DCOMPLEX;
  // The output class is now the dominant class remaining:
  Cclass = (Aclass > Bclass) ? Aclass : Bclass;
  A.promoteType(Cclass);
  B.promoteType(Cclass);
}

/**
 * We want to perform a matrix-matrix operation between two data objects.
 * The following checks are required:
 *  1. If A or B is a scalar, then return false - this is really a 
 *     vector operation, and the arguments should be passed to a 
 *     vector checker (like VectorCheck).
 *  2. Both A & B must be numeric
 *  3. Both A & B must be the same type (if not, the lesser type is
 *     automatically promoted).
 *  4. Both A & B must be 2-Dimensional.
 *  5. A & B must be conformant, i.e. the number of columns in A must
 *     match the number of rows in B.
 */
inline bool MatrixCheck(Array &A, Array &B, std::string opname){
  // Test for either a scalar (test 1)
  if (A.isScalar() || B.isScalar())
    return false;

  // Test for A & B numeric
  CheckNumeric(A,B,opname);

  // Test for 2D
  if (!A.is2D() || !B.is2D()) 
    throw Exception(std::string("Cannot apply matrix operation ") + 
		    opname + std::string(" to N-Dimensional arrays."));
  
  // Test the types
  TypeCheck(A,B,true);
  return true;
}


/*
 * Check to see if two dimensions (when treated as vectors) are equivalent in size.
 */
bool SameSizeCheck(Dimensions Adim, Dimensions Bdim) {
  Adim.simplify();
  Bdim.simplify();
  return (Adim.equals(Bdim));
}

/**
 * We want to perform a vector operation between two data objects.
 * The following checks are required:
 *  1. Both A & B must be numeric
 *  2. Either A & B are the same size or
 *      A is a scalar or B is a scalar.
 */
inline void VectorCheck(Array& A, Array& B, bool promote, std::string opname){
  StringVector dummySV;

  // Check for numeric types
  CheckNumeric(A,B,opname);
  
  if (!(SameSizeCheck(A.dimensions(),B.dimensions()) || A.isScalar() || B.isScalar()))
    throw Exception(std::string("Size mismatch on arguments to arithmetic operator ") + opname);
  
  // Test the types.
  TypeCheck(A,B,promote);
}

/**
 * We want to perform a vector operator between two logical data objects.
 * The following operations are performed:
 *  1. Both A & B are converted to logical types.
 *  2. Either A & B must be the same size, or A is a
 *     scalar or B is a scalar.
 */
inline void BoolVectorCheck(Array& A, Array& B,std::string opname){
  A.promoteType(FM_LOGICAL);
  B.promoteType(FM_LOGICAL);

  if (!(SameSizeCheck(A.dimensions(),B.dimensions()) || A.isScalar() || B.isScalar()))
    throw Exception(std::string("Size mismatch on arguments to ") + opname);
}


/**
 * Handle a particular type case in the power operator.
 */
inline Array doPowerAssist(Array A, Class AClass, 
			   Array B, Class BClass,
			   Class CClass, vvfun exec) {
  Array C;
  A.promoteType(AClass);
  B.promoteType(BClass);

  // S^F, F^S, S^S are all done via full intermediate products
  // Only S^scalar is done using a sparse algorithm
  if (B.isScalar() && A.sparse())
    return SparsePowerFunc(A,B);
  A.makeDense();
  B.makeDense();

  if (A.isScalar()) {
    int Blen(B.getLength());
    C = Array(CClass,B.dimensions(),NULL);
    void *Cp = Malloc(Blen*C.getElementSize());
    exec(Blen,Cp,A.getDataPointer(),0,B.getDataPointer(),1);
    C.setDataPointer(Cp);
  } else if (B.isScalar()) {
    int Alen(A.getLength());
    C = Array(CClass,A.dimensions(),NULL);
    void *Cp = Malloc(Alen*C.getElementSize());
    exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),0);
    C.setDataPointer(Cp);
  } else {
    int Alen(A.getLength());
    C = Array(CClass,A.dimensions(),NULL);
    void *Cp = Malloc(Alen*C.getElementSize());
    exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),1);
    C.setDataPointer(Cp);
  }
  return C;
}

// Invert a square matrix - Should check for diagonal matrices
// as a special case
Array InvertMatrix(Array a, Interpreter* m_eval) {
  if (!a.is2D())
    throw Exception("Cannot invert N-dimensional arrays.");
  if (a.isReferenceType())
    throw Exception("Cannot invert reference-type arrays.");
  if (a.isScalar())
    return DotPower(a,Array::floatConstructor(-1),m_eval);
  int N(a.getDimensionLength(0));
  if (a.sparse()) {
    uint32 *I = (uint32*) Malloc(sizeof(uint32)*N);
    for (int k=0;k<N;k++)
      I[k] = k+1;
    float v = 1.0f;
    Array B(FM_FLOAT,a.dimensions(),
	    makeSparseFromIJV(FM_FLOAT,N,N,N,I,1,I,1,&v,0),true);
    Free(I);
    Array c(LeftDivide(a,B,m_eval));
    c.makeSparse();
    return c;
  } else {
    Array ones(Array::floatVectorConstructor(N));
    float *dp = (float*) ones.getReadWriteDataPointer();
    for (int i=0;i<N;i++) dp[i] = 1.0f;
    Array B(Array::diagonalConstructor(ones,0));
    return LeftDivide(a,B,m_eval);
  }
}
  
// Handle matrix powers for sparse matrices
Array MatrixPowerSparse(Array a, Array b, Interpreter* m_eval) {
  // The expression a^B where a is a scalar, and B is sparse is not handled
  if (a.isScalar() && !b.isScalar())
    throw Exception("expression a^B, where a is a scalar and B is a sparse matrix is not supported (use full to convert B to non-sparse matrix");
  // The expression A^B is not supported
  if (!a.isScalar() && !b.isScalar())
    throw Exception("expression A^B where A and B are both sparse matrices is not supported (or defined)");
  // The expression A^b where b is not an integer is not supported
  if (!b.isReal())
    throw Exception("expression A^b where b is complex and A is sparse is not supported (use full to convert A to a non-sparse matrix)");
  b.promoteType(FM_DOUBLE);
  const double*dp = (const double*) b.getDataPointer();
  if ((*dp) != rint(*dp))
    throw Exception("expression A^b where b is non-integer and A is sparse is not supported (use full to convert A to a non-sparse matrix)");
  if (a.getDimensionLength(0) != a.getDimensionLength(1))
    throw Exception("expression A^b requires A to be square.");
  int power = (int) *dp;
  if (power < 0) {
    a = InvertMatrix(a,m_eval);
    power = -power;
  }
  if (power == 0) {
    Array r(FM_FLOAT,a.dimensions(),
	    SparseOnesFunc(a.dataClass(),a.getDimensionLength(0),a.getDimensionLength(1),
			   a.getSparseDataPointer()),true);
    r.promoteType(a.dataClass());
    return r;
  }
  Array c(a);
  for (int i=1;i<power;i++)
    c = Multiply(c,a,m_eval);
  return c;
}

/**
 * The power function is a special case of the vector-vector operations.  
 * In this case, if the real vector contains any negative elements
 * A real vector can be written as (r*e^(j\theta))^p = r^p*e^(j*\theta*p)
 * If there are any components of A negative raised to a non-integer power,
 * then A and B are promoted to complex types.  If A is a double, then they
 * are promoted to dcomplex types.  So the rules are as follows:
 * A complex, B arbitrary -> output is complex, unless B is double or dcomplex
 * A dcomplex, B arbitrary -> output is dcomplex
 * A double, arbitrary, B integer -> output is double
 * A double, negative, B noninteger -> output is dcomplex
 * A float, arbitrary, B integer -> output is float
 * A float, negative, B noninteger -> output is complex, unless B is double or dcomplex
 *
 * Some simplification can take place - the following type combinations
 * are possible - with the unique combinations numbered
 * 1. complex, integer   -> complex
 * 2. complex, float     -> complex
 * 3. complex, double    >> dcomplex, double   -> dcomplex
 * 4. complex, complex   -> complex
 * 5. complex, dcomplex  >> dcomplex, dcomplex -> dcomplex
 * 6. dcomplex, integer  -> dcomplex
 *    dcomplex, float    >> dcomplex, double   -> dcomplex (3)
 *    dcomplex, double   >> dcomplex, double   -> dcomplex (3)
 *    dcomplex, complex  >> dcomplex, dcomplex -> dcomplex (5)
 *    dcomplex, dcomplex >> dcomplex, dcomplex -> dcomplex (5)
 * 7. double, integer    -> double
 *    double-, float     >> dcomplex, dcomplex -> dcomplex (5)
 *    double-, double    >> dcomplex, dcomplex -> dcomplex (5)
 *    double-, complex   >> dcomplex, dcomplex -> dcomplex (5)
 *    double-, dcomplex  >> dcomplex, dcomplex -> dcomplex (5)
 * 8. double+, float     >> double, double -> double
 *    double+, double    >> double, double -> double (8)
 *    double+, complex   >> dcomplex, dcomplex -> dcomplex (5)
 *    double+, dcomplex  >> dcomplex, dcomplex -> dcomplex (5)
 * 9. float, integer     -> float
 *    float-, float      >> complex, complex -> complex (4)
 *    float-, double     >> dcomplex, dcomplex -> dcomplex (5)
 *    float-, complex    >> complex, complex -> complex (4)
 *    float-, dcomplex   >> dcomplex, dcomplex -> dcomplex (5)
 *10. float+, float      >> float, float -> float
 *    float+, double     >> double, double -> double (8)
 *    float+, complex    >> complex, complex -> complex (4)
 *    float+, dcomplex   >> dcomplex, dcomplex -> dcomplex (5)
 *
 * This type scheme is complicated, but workable.  If A is an 
 * integer type, it is first promoted to double, before the
 * rules are applied.  Then, we note that the rules can be cleaved
 * along the lines of B being integer or not.  If B is integer,
 * we apply the appropriate version of 1., 6., 7., 9..  This
 * covers 4 of the 6 cases.  Next, we determine if A is complex.
 * If A is complex, we check if B is double or dcomplex. If either
 * is the case, we promote A to dcomplex.  If A is dcomplex,
 * we promote B to double or dcomplex.  Next, we test for
 * rules 2., 3., 4., 5.  
 */

#define OPCASE(t,o) case t: opType = o; break;
#define MAPOP(o,a,b,c,f) case o: return doPowerAssist(A,a,B,b,c,f);
inline Array DoPowerTwoArgFunction(Array A, Array B){
  Array C;
  bool Anegative;
  StringVector dummySV;
  Class AClass, BClass;
  int opType;

  if (A.isEmpty() || B.isEmpty())
    return Array::emptyConstructor();
  CheckNumeric(A,B,"^");
  if (!(SameSizeCheck(A.dimensions(),B.dimensions()) || A.isScalar() || B.isScalar()))
    throw Exception("Size mismatch on arguments to power (^) operator.");
  // If A is not at least a float type, promote it to double
  AClass = A.dataClass();
  BClass = B.dataClass();
  if (AClass < FM_FLOAT) AClass = FM_DOUBLE;
  if (BClass < FM_INT32) BClass = FM_INT32;
  // Get a read on if A is positive
  Anegative = !(A.isPositive());
  // Check through the different type cases...
  opType = 0;
  if (AClass == FM_COMPLEX) {
    switch (BClass) {
    default: throw Exception("Unhandled type for second argument to A^B");
      OPCASE(FM_INT32,1);
      OPCASE(FM_FLOAT,2);
      OPCASE(FM_DOUBLE,3);
      OPCASE(FM_COMPLEX,4);
      OPCASE(FM_DCOMPLEX,5);
    }
  } else if (AClass == FM_DCOMPLEX) {
    switch(BClass) {
    default: throw Exception("Unhandled type for second argument to A^B");
      OPCASE(FM_INT32,6);
      OPCASE(FM_FLOAT,3);
      OPCASE(FM_DOUBLE,3);
      OPCASE(FM_COMPLEX,5);
      OPCASE(FM_DCOMPLEX,5);
    }
  } else if (AClass == FM_DOUBLE && Anegative) {
    switch(BClass) {
    default: throw Exception("Unhandled type for second argument to A^B");
      OPCASE(FM_INT32,7);
      OPCASE(FM_FLOAT,5);
      OPCASE(FM_DOUBLE,5);
      OPCASE(FM_COMPLEX,5);
      OPCASE(FM_DCOMPLEX,5);
    }      
  } else if (AClass == FM_DOUBLE && (!Anegative)){
    switch(BClass) {
    default: throw Exception("Unhandled type for second argument to A^B");
      OPCASE(FM_INT32,7);
      OPCASE(FM_FLOAT,8);
      OPCASE(FM_DOUBLE,8);
      OPCASE(FM_COMPLEX,5);
      OPCASE(FM_DCOMPLEX,5);
    }      
  } else if (AClass == FM_FLOAT && Anegative) {
    switch(BClass) {
    default: throw Exception("Unhandled type for second argument to A^B");
      OPCASE(FM_INT32,9);
      OPCASE(FM_FLOAT,4);
      OPCASE(FM_DOUBLE,5);
      OPCASE(FM_COMPLEX,4);
      OPCASE(FM_DCOMPLEX,5);
    }      
  } else if (AClass == FM_FLOAT && (!Anegative)){
    switch(BClass) {
    default: throw Exception("Unhandled type for second argument to A^B");
      OPCASE(FM_INT32,9);
      OPCASE(FM_FLOAT,10);
      OPCASE(FM_DOUBLE,8);
      OPCASE(FM_COMPLEX,4);
      OPCASE(FM_DCOMPLEX,5);
    }
  }
  // Invoke the appropriate case
  switch(opType) {
    default: throw Exception("Unhandled type combination for A^B");
    MAPOP(1,FM_COMPLEX,FM_INT32,FM_COMPLEX,(vvfun) cicpower);
    MAPOP(2,FM_COMPLEX,FM_FLOAT,FM_COMPLEX,(vvfun) cfcpower);
    MAPOP(3,FM_DCOMPLEX,FM_DOUBLE,FM_DCOMPLEX,(vvfun) zdzpower);
    MAPOP(4,FM_COMPLEX,FM_COMPLEX,FM_COMPLEX,(vvfun) cccpower);
    MAPOP(5,FM_DCOMPLEX,FM_DCOMPLEX,FM_DCOMPLEX,(vvfun) zzzpower);
    MAPOP(6,FM_DCOMPLEX,FM_INT32,FM_DCOMPLEX,(vvfun) zizpower);
    MAPOP(7,FM_DOUBLE,FM_INT32,FM_DOUBLE,(vvfun) didpower);
    MAPOP(8,FM_DOUBLE,FM_DOUBLE,FM_DOUBLE,(vvfun) dddpower);
    MAPOP(9,FM_FLOAT,FM_INT32,FM_FLOAT,(vvfun) fifpower);
    MAPOP(10,FM_FLOAT,FM_FLOAT,FM_FLOAT,(vvfun) fffpower);
  }
}


/**
 * For all of the Vector-Vector operations that have vector and scalar versions,
 * the general behavior is the same.  We require 3 functions to handle the 3
 * cases that generally arise:
 *   logical vector vector
 *   logical vector scalar
 *   logical scalar vector
 * The remaining 3 function placeholders in the packVectorVector are unused.
 */
inline Array DoBoolTwoArgFunction(Array A, Array B, vvfun exec, std::string opname) {
  Array C;
  
  BoolVectorCheck(A,B,opname);
  if (A.isScalar()) {
    int Blen(B.getLength());
    C = Array(FM_LOGICAL,B.dimensions(),NULL);
    void *Cp = Malloc(Blen*C.getElementSize());
    exec(Blen,Cp,A.getDataPointer(),0,B.getDataPointer(),1);
    C.setDataPointer(Cp);
  } else if (B.isScalar()) {
    int Alen(A.getLength());
    C = Array(FM_LOGICAL,A.dimensions(),NULL);
    void *Cp = Malloc(Alen*C.getElementSize());
    exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),0);
    C.setDataPointer(Cp);
  } else {
    int Alen(A.getLength());
    C = Array(FM_LOGICAL,A.dimensions(),NULL);
    void *Cp = Malloc(Alen*C.getElementSize());
    exec(Alen,Cp,A.getDataPointer(),1,B.getDataPointer(),1);
    C.setDataPointer(Cp);
  }
  return C;
}

/**
 * Add two objects.
 */
//!
//@Module PLUS Addition Operator
//@@Section OPERATORS
//@@Usage
//Adds two numerical arrays (elementwise) together.  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a + b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise the sum 
//of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the sum of the scalar added to each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using the type 
//promotion rules.  The types are ordered as:
//\begin{enumerate}
//\item @|uint8| - unsigned, 8-bit integers range @|[0,255]|
//\item @|int8| - signed, 8-bit integers @|[-127,128]|
//\item @|uint16| - unsigned, 16-bit integers @|[0,65535]|
//\item @|int16| - signed, 16-bit integers @|[-32768,32767]|
//\item @|uint32| - unsigned, 32-bit integers @|[0,4294967295]|
//\item @|int32| - signed, 32-bit integers @|[-2147483648,2147483647]|
//\item @|float| - 32-bit floating point
//\item @|double| - 64-bit floating point
//\item @|complex| - 32-bit complex floating point
//\item @|dcomplex| - 64-bit complex floating point
//\end{enumerate}
//Note that the type promotion and combination rules work similar to 
//@|C|.  Numerical overflow rules are also the same as @|C|.
//@@Function Internals
//There are three formulae for the addition operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) + b(m_1,\ldots,m_d)
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a + b(m_1,\ldots,m_d).
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) + b.
//\]
//@@Examples
//Here are some examples of using the addition operator.  First, a 
//straight-forward usage of the plus operator.  The first example
//is straightforward - the @|int32| is the default type used for
//integer constants (same as in @|C|), hence the output is the
//same type:
//@<
//3 + 8
//@>
//Next, we use the floating point syntax to force one of the arguments
//to be a @|double|, which results in the output being @|double|:
//@<
//3.1 + 2
//@>
//Note that if one of the arguments is complex-valued, the output will be
//complex also.
//@<
//a = 3 + 4*i
//b = a + 2.0f
//@>
//If a @|complex| value is added to a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a + 2.0
//@>
//We can also demonstrate the three forms of the addition operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a + b
//@>
//Then the scalar versions
//@<
//c = a + 1
//c = 1 + b
//@>
//@@Tests
//@{ test_sparse37.m
//function x = test_sparse37
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//b = [1 0 0 0 0 0
//     0 1 0 0 0 0 
//     0 0 1 0 0 0 
//     0 0 0 1 0 0];
//A = sparse(a);
//B = sparse(b);
//C = A + B;
//c = a + b;
//x = testeq(c,C);
//@}
//@{ test_sparse38.m
//function x = test_sparse38
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//br = [1 0 0 0 0 0
//      0 1 0 0 0 0 
//      0 0 1 0 0 0 
//      0 0 0 1 0 0];
//
//bi = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//a = ar+i*ai;
//b = br+i*bi;
//
//A = sparse(a);
//B = sparse(b);
//
//C = A + B;
//c = a + b;
//x = testeq(c,C);
//@}
//@{ test_sparse78.m
//% Test sparse-sparse matrix array add
//function x = test_sparse78
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',300,400);
//[yf2,zf2] = sparse_test_mat('float',300,400);
//[yd2,zd2] = sparse_test_mat('double',300,400);
//[yc2,zc2] = sparse_test_mat('complex',300,400);
//[yz2,zz2] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1+yi2,zi1+zi2) & testeq(yf1+yf2,zf1+zf2) & testeq(yd1+yd2,zd1+zd2) & testeq(yc1+yc2,zc1+zc2) & testeq(yz1+yz2,zz1+zz2);
//@}
//@@Tests
//@$"y=zeros(3,0,4)+zeros(3,0,4)","zeros(3,0,4)","exact"
//!
Array Add(Array A, Array B, Interpreter* m_eval) { 
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,"+");
  // Get a pointer to the function we ultimately need to execute
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  bool sparse;
  Dimensions Cdim;
  if (A.isScalar() && B.isScalar()) {
    Clen = 1;
    Astride = 0;
    Bstride = 0;
    Cdim = Dimensions(1,1);
  } else if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
    Clen = B.getLength();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
    Clen = A.getLength();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
    Clen = A.getLength();
  }
  if (!Astride || !Bstride) {
    A.makeDense();
    B.makeDense();
  }
  if (A.sparse() && !B.sparse()) 
    A.makeDense();
  if (!A.sparse() && B.sparse()) 
    B.makeDense();
  if (A.sparse()) {
    sparse = true;
    Cp = SparseSparseAdd(A.dataClass(),
			 A.getSparseDataPointer(),
			 A.getDimensionLength(0),
			 A.getDimensionLength(1),
			 B.getSparseDataPointer());
  } else {
    sparse = false;
    Cp = Malloc(Clen*B.getElementSize());
    switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A+B");
    case FM_INT32:
      addfullreal<int32>(Clen,(int32*) Cp, 
			 (int32*) A.getDataPointer(), Astride,
			 (int32*) B.getDataPointer(), Bstride);
      break;
    case FM_INT64:
      addfullreal<int64>(Clen,(int64*) Cp, 
			 (int64*) A.getDataPointer(), Astride,
			 (int64*) B.getDataPointer(), Bstride);
      break;
    case FM_FLOAT:
      addfullreal<float>(Clen,(float*) Cp, 
			 (float*) A.getDataPointer(), Astride,
			 (float*) B.getDataPointer(), Bstride);
      break;
    case FM_DOUBLE:
      addfullreal<double>(Clen,(double*) Cp, 
			  (double*) A.getDataPointer(), Astride,
			  (double*) B.getDataPointer(), Bstride);
      break;
    case FM_COMPLEX:
      addfullcomplex<float>(Clen,(float*) Cp, 
			    (float*) A.getDataPointer(), Astride,
			    (float*) B.getDataPointer(), Bstride);
      break;
    case FM_DCOMPLEX:
      addfullcomplex<double>(Clen,(double*) Cp, 
			     (double*) A.getDataPointer(), Astride,
			     (double*) B.getDataPointer(), Bstride);
      break;			 
    }
  }
  return Array(B.dataClass(),Cdim,Cp,sparse);
}

/**
 * Subtract two objects.
 */
//!
//@Module MINUS Subtraction Operator
//@@Section OPERATORS
//@@Usage
//Subtracts two numerical arrays (elementwise).  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a - b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//difference of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the difference of the scalar to each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using the type 
//promotion rules.  The types are ordered as:
//\begin{enumerate}
//\item @|uint8| - unsigned, 8-bit integers range @|[0,255]|
//\item @|int8| - signed, 8-bit integers @|[-127,128]|
//\item @|uint16| - unsigned, 16-bit integers @|[0,65535]|
//\item @|int16| - signed, 16-bit integers @|[-32768,32767]|
//\item @|uint32| - unsigned, 32-bit integers @|[0,4294967295]|
//\item @|int32| - signed, 32-bit integers @|[-2147483648,2147483647]|
//\item @|float| - 32-bit floating point
//\item @|double| - 64-bit floating point
//\item @|complex| - 32-bit complex floating point
//\item @|dcomplex| - 64-bit complex floating point
//\end{enumerate}
//Note that the type promotion and combination rules work similar to 
//@|C|.  Numerical overflow rules are also the same as @|C|.
//@@Function Internals
//There are three formulae for the subtraction operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) - b(m_1,\ldots,m_d)
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a - b(m_1,\ldots,m_d).
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) - b.
//\]
//@@Examples
//Here are some examples of using the subtraction operator.  First, a 
//straight-forward usage of the minus operator.  The first example
//is straightforward - the @|int32| is the default type used for
//integer constants (same as in @|C|), hence the output is the
//same type:
//@<
//3 - 8
//@>
//Next, we use the floating point syntax to force one of the arguments
//to be a @|double|, which results in the output being @|double|:
//@<
//3.1 - 2
//@>
//Note that if one of the arguments is complex-valued, the output will be
//complex also.
//@<
//a = 3 + 4*i
//b = a - 2.0f
//@>
//If a @|double| value is subtracted from a @|complex|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a - 2.0
//@>
//We can also demonstrate the three forms of the subtraction operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a - b
//@>
//Then the scalar versions
//@<
//c = a - 1
//c = 1 - b
//@>
//@@Tests
//@{ test_sparse39.m
//function x = test_sparse39
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//
//b = [1 0 0 0 0 0
//     0 1 0 0 0 0 
//     0 0 1 0 0 0 
//       0 0 0 1 0 0];
//
//A = sparse(a);
//B = sparse(b);
//
//C = A - B;
//c = a - b;
//x = testeq(c,C);
//@}
//@{ test_sparse40.m
//function x = test_sparse40
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//br = [1 0 0 0 0 0
//      0 1 0 0 0 0 
//      0 0 1 0 0 0 
//      0 0 0 1 0 0];
//
//bi = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//a = ar+i*ai;
//b = br+i*bi;
//
//A = sparse(a);
//B = sparse(b);
//
//C = A - B;
//c = a - b;
//x = testeq(c,C);
//@}
//@{ test_sparse79.m
//% Test sparse-sparse matrix array subtract
//function x = test_sparse79
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',300,400);
//[yf2,zf2] = sparse_test_mat('float',300,400);
//[yd2,zd2] = sparse_test_mat('double',300,400);
//[yc2,zc2] = sparse_test_mat('complex',300,400);
//[yz2,zz2] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1-yi2,zi1-zi2) & testeq(yf1-yf2,zf1-zf2) & testeq(yd1-yd2,zd1-zd2) & testeq(yc1-yc2,zc1-zc2) & testeq(yz1-yz2,zz1-zz2);
//@}
//!
Array Subtract(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,"-");
  // Get a pointer to the function we ultimately need to execute
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;
  bool sparse;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
    Clen = B.getLength();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
    Clen = A.getLength();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
    Clen = A.getLength();
  }
  if (!Astride || !Bstride) {
    A.makeDense();
    B.makeDense();
  }
  if (A.sparse() && !B.sparse()) 
    A.makeDense();
  if (!A.sparse() && B.sparse()) 
    B.makeDense();
  if (A.sparse()) {
    sparse = true;
    Cp = SparseSparseSubtract(A.dataClass(),
			      A.getSparseDataPointer(),
			      A.getDimensionLength(0),
			      A.getDimensionLength(1),
			      B.getSparseDataPointer());
  } else {
    sparse = false;
    Cp = Malloc(Clen*B.getElementSize());
    switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A-B");
    case FM_INT32:
      subtractfullreal<int32>(Clen,(int32*) Cp, 
			      (int32*) A.getDataPointer(), Astride,
			      (int32*) B.getDataPointer(), Bstride);
      break;
    case FM_INT64:
      subtractfullreal<int64>(Clen,(int64*) Cp, 
			      (int64*) A.getDataPointer(), Astride,
			      (int64*) B.getDataPointer(), Bstride);
      break;
    case FM_FLOAT:
      subtractfullreal<float>(Clen,(float*) Cp, 
			      (float*) A.getDataPointer(), Astride,
			      (float*) B.getDataPointer(), Bstride);
      break;
    case FM_DOUBLE:
      subtractfullreal<double>(Clen,(double*) Cp, 
			       (double*) A.getDataPointer(), Astride,
			       (double*) B.getDataPointer(), Bstride);
      break;
    case FM_COMPLEX:
      subtractfullcomplex<float>(Clen,(float*) Cp, 
				 (float*) A.getDataPointer(), Astride,
				 (float*) B.getDataPointer(), Bstride);
      break;
    case FM_DCOMPLEX:
      subtractfullcomplex<double>(Clen,(double*) Cp, 
				  (double*) A.getDataPointer(), Astride,
				  (double*) B.getDataPointer(), Bstride);
      break;			 
    }
  }
  return Array(B.dataClass(),Cdim,Cp,sparse);
}

/**
 * Element-wise multiplication.
 */
//!
//@Module DOTTIMES Element-wise Multiplication Operator
//@@Section OPERATORS
//@@Usage
//Multiplies two numerical arrays (elementwise).  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a .* b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//product of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the product of the scalar with each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using type 
//promotion rules. All of the types are preserved under multiplication except
// for integer types, which are promoted to @|int32| prior to 
//multiplication (same as @|C|).
//@@Function Internals
//There are three formulae for the dot-times operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b(m_1,\ldots,m_d)
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a \times b(m_1,\ldots,m_d).
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b.
//\]
//@@Examples
//Here are some examples of using the dottimes operator.  First, a 
//straight-forward usage of the @|.*| operator.  The first example
//is straightforward:
//@<
//3 .* 8
//@>
//Note, however, that because of the way that input is parsed, eliminating
//the spaces @|3.*8| results in the input being parsed as @|3. * 8|,
//which yields a @|double| result:
//@<
//3.*8
//@>
//This is really an invokation of the @|times| operator.
//
//Next, we use the floating point syntax to force one of the arguments
//to be a @|double|, which results in the output being @|double|:
//@<
//3.1 .* 2
//@>
//Note that if one of the arguments is complex-valued, the output will be
//complex also.
//@<
//a = 3 + 4*i
//b = a .* 2.0f
//@>
//If a @|complex| value is multiplied by a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a .* 2.0
//@>
//We can also demonstrate the three forms of the dottimes operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a .* b
//@>
//Then the scalar versions
//@<
//c = a .* 3
//c = 3 .* a
//@>
//@@Tests
//@{ test_sparse41.m
//function x = test_sparse41
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//
//b = [1 0 0 0 0 0
//     0 1 0 0 0 0 
//     0 0 1 0 0 0 
//       0 0 0 1 0 0];
//
//A = sparse(a);
//B = sparse(b);
//
//C = A .* B;
//c = a .* b;
//x = testeq(c,C);
//@}
//@{ test_sparse42.m
//function x = test_sparse42
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//br = [1 0 0 0 0 0
//      0 1 0 0 0 0 
//      0 0 1 0 0 0 
//      0 0 0 1 0 0];
//
//bi = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//a = ar+i*ai;
//b = br+i*bi;
//
//A = sparse(a);
//B = sparse(b);
//
//C = A .* B;
//c = a .* b;
//x = testeq(c,C);
//@}
//@{ test_sparse43.m
//function x = test_sparse43
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//
//b = pi;
//
//A = sparse(a);
//
//C = A .* b;
//c = a .* b;
//x = testeq(c,C);
//@}
//@{ test_sparse44.m
//function x = test_sparse44
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//a = ar + i * ai;
//b = 3 + 4*i;
//
//A = sparse(a);
//
//C = A .* b;
//c = a .* b;
//x = testeq(c,C);
//@}
//!
Array DotMultiply(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,".*");
  // Get a pointer to the function we ultimately need to execute
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;
  bool sparse;
  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
    Clen = B.getLength();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
    Clen = A.getLength();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
    Clen = A.getLength();
  }
  //FIXME - these rules don't apply for multiplication!!
  if (A.sparse() && B.isScalar()) {
    sparse = true;
    B.makeDense();
    Cp = SparseScalarMultiply(A.dataClass(),
			      A.getSparseDataPointer(),
			      A.getDimensionLength(0),
			      A.getDimensionLength(1),
			      B.getDataPointer());
  } else if (B.sparse() && A.isScalar()) {
    sparse = true;
    A.makeDense();
    Cp = SparseScalarMultiply(B.dataClass(),
			      B.getSparseDataPointer(),
			      B.getDimensionLength(0),
			      B.getDimensionLength(1),
			      A.getDataPointer());
  } else {
    if (A.sparse() && !B.sparse()) 
      A.makeDense();
    if (!A.sparse() && B.sparse()) 
      B.makeDense();
    if (A.sparse()) {
      sparse = true;
      Cp = SparseSparseMultiply(A.dataClass(),
				A.getSparseDataPointer(),
				A.getDimensionLength(0),
				A.getDimensionLength(1),
				B.getSparseDataPointer());
    } else {
      sparse = false;
      Cp = Malloc(Clen*B.getElementSize());
      switch(B.dataClass()) {
      default: throw Exception("Unhandled type for second argument to A.*B");
      case FM_INT32:
	multiplyfullreal<int32>(Clen,(int32*) Cp, 
				(int32*) A.getDataPointer(), Astride,
				(int32*) B.getDataPointer(), Bstride);
	break;
      case FM_INT64:
	multiplyfullreal<int64>(Clen,(int64*) Cp, 
				(int64*) A.getDataPointer(), Astride,
				(int64*) B.getDataPointer(), Bstride);
	break;
      case FM_FLOAT:
	multiplyfullreal<float>(Clen,(float*) Cp, 
				(float*) A.getDataPointer(), Astride,
				(float*) B.getDataPointer(), Bstride);
	break;
      case FM_DOUBLE:
	multiplyfullreal<double>(Clen,(double*) Cp, 
				 (double*) A.getDataPointer(), Astride,
				 (double*) B.getDataPointer(), Bstride);
	break;
      case FM_COMPLEX:
	multiplyfullcomplex<float>(Clen,(float*) Cp, 
				   (float*) A.getDataPointer(), Astride,
				   (float*) B.getDataPointer(), Bstride);
	break;
      case FM_DCOMPLEX:
	multiplyfullcomplex<double>(Clen,(double*) Cp, 
				    (double*) A.getDataPointer(), Astride,
				    (double*) B.getDataPointer(), Bstride);
	break;			 
      }
    }
  }
  return Array(B.dataClass(),Cdim,Cp,sparse);
}
 
/**
 * Element-wise right divide.
 */
//!
//@Module DOTRIGHTDIVIDE Element-wise Right-Division Operator
//@@Section OPERATORS
//@@Usage
//Divides two numerical arrays (elementwise).  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a ./ b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//division of @|b| by @|a|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the division of the scalar with each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using type 
//promotion rules, with one important exception: unlike @|C|, integer
//types are promoted to @|double| prior to division.
//@@Function Internals
//There are three formulae for the dot-right-divide operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = \frac{a(m_1,\ldots,m_d)}{b(m_1,\ldots,m_d)}
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{a}{b(m_1,\ldots,m_d)}
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{a(m_1,\ldots,m_d)}{b}.
//\]
//@@Examples
//Here are some examples of using the dot-right-divide operator.  First, a 
//straight-forward usage of the @|./| operator.  The first example
//is straightforward:
//@<
//3 ./ 8
//@>
//Note that this is not the same as evaluating @|3/8| in @|C| - there,
//the output would be @|0|, the result of the integer division.
//
//We can also divide complex arguments:
//@<
//a = 3 + 4*i
//b = 5 + 8*i
//c = a ./ b
//@>
//If a @|complex| value is divided by a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a ./ 2.0
//@>
//We can also demonstrate the three forms of the dot-right-divide operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a ./ b
//@>
//Then the scalar versions
//@<
//c = a ./ 3
//c = 3 ./ a
//@>
//!
Array DotRightDivide(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,true,"./");
  // Get a pointer to the function we ultimately need to execute
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
    Clen = B.getLength();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
    Clen = A.getLength();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
    Clen = A.getLength();
  }
  Cp = Malloc(Clen*B.getElementSize());
  switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A./B");
  case FM_INT32:
    dividefullreal<int32>(Clen,(int32*) Cp, 
			  (int32*) A.getDataPointer(), Astride,
			  (int32*) B.getDataPointer(), Bstride);
    break;
  case FM_INT64:
    dividefullreal<int64>(Clen,(int64*) Cp, 
			  (int64*) A.getDataPointer(), Astride,
			  (int64*) B.getDataPointer(), Bstride);
    break;
  case FM_FLOAT:
    dividefullreal<float>(Clen,(float*) Cp, 
			  (float*) A.getDataPointer(), Astride,
			  (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DOUBLE:
    dividefullreal<double>(Clen,(double*) Cp, 
			   (double*) A.getDataPointer(), Astride,
			   (double*) B.getDataPointer(), Bstride);
    break;
  case FM_COMPLEX:
    dividefullcomplex<float>(Clen,(float*) Cp, 
			     (float*) A.getDataPointer(), Astride,
			     (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DCOMPLEX:
    dividefullcomplex<double>(Clen,(double*) Cp, 
			      (double*) A.getDataPointer(), Astride,
			      (double*) B.getDataPointer(), Bstride);
    break;			 
  }
  return Array(B.dataClass(),Cdim,Cp);
}

/**
 * Element-wise left divide.
 */
//!
//@Module DOTLEFTDIVIDE Element-wise Left-Division Operator
//@@Section OPERATORS
//@@Usage
//Divides two numerical arrays (elementwise) - gets its name from the 
//fact that the divisor is on the left.  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a .\ b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//division of @|b| by @|a|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the division of the scalar with each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using type 
//promotion rules, with one important exception: unlike @|C|, integer
//types are promoted to @|double| prior to division.
//@@Function Internals
//There are three formulae for the dot-left-divide operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = \frac{b(m_1,\ldots,m_d)}{a(m_1,\ldots,m_d)}
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{b(m_1,\ldots,m_d)}{a}
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{b}{a(m_1,\ldots,m_d)}.
//\]
//@@Examples
//Here are some examples of using the dot-left-divide operator.  First, a 
//straight-forward usage of the @|.\\| operator.  The first example
//is straightforward:
//@<
//3 .\ 8
//@>
//Note that this is not the same as evaluating @|8/3| in @|C| - there,
//the output would be @|2|, the result of the integer division.
//
//We can also divide complex arguments:
//@<
//a = 3 + 4*i
//b = 5 + 8*i
//c = b .\ a
//@>
//If a @|complex| value is divided by a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a .\ 2.0
//@>
//We can also demonstrate the three forms of the dot-left-divide operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a .\ b
//@>
//Then the scalar versions
//@<
//c = a .\ 3
//c = 3 .\ a
//@>
//!
Array DotLeftDivide(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,true,".\\");
  return DotRightDivide(B,A,m_eval);
}

/**
 * Element-wise power.
 */
//!
//@Module DOTPOWER Element-wise Power Operator
//@@Section OPERATORS
//@@Usage
//Raises one numerical array to another array (elementwise).  There are three operators all with the same general syntax:
//@[
//  y = a .^ b
//@]
//The result @|y| depends on which of the following three situations applies to the arguments @|a| and @|b|:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is an arbitrary @|n|-dimensional numerical array, in which case the output is @|a| raised to the power of each element of @|b|, and the output is the same size as @|b|.
//  \item @|a| is an @|n|-dimensional numerical array, and @|b| is a scalar, then the output is the same size as @|a|, and is defined by each element of @|a| raised to the power @|b|.
//  \item @|a| and @|b| are both @|n|-dimensional numerical arrays of \emph{the same size}.  In this case, each element of the output is the corresponding element of @|a| raised to the power defined by the corresponding element of @|b|.
//\end{enumerate}
//The output follows the standard type promotion rules, although types are not generally preserved under the power operation.  In particular, integers are automatically converted to @|double| type, and negative numbers raised to fractional powers can return complex values.
//@@Function Internals
//There are three formulae for this operator.  For the first form
//\[
//y(m_1,\ldots,m_d) = a^{b(m_1,\ldots,m_d)},
//\]
//and the second form
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d)^b,
//\]
//and in the third form
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d)^{b(m_1,\ldots,m_d)}.
//\]
//@@Examples
//We demonstrate the three forms of the dot-power operator using some simple examples.  First, the case of a scalar raised to a series of values.
//@<
//a = 2
//b = 1:4
//c = a.^b
//@>
//The second case shows a vector raised to a scalar.
//@<
//c = b.^a
//@>
//The third case shows the most general use of the dot-power operator.
//@<
//A = [1,2;3,2]
//B = [2,1.5;0.5,0.6]
//C = A.^B
//@>
//@@Tests
//@$"y=2.^[1:5]","[2,4,8,16,32]","exact"
//@$"y=2..^[1:5]","[2,4,8,16,32]","exact"
//@{ test_power1.m
//function x = test_power1
//invD_11 = [0.3529    0.4028    0.5812    0.3333];
//invD_12 = [0.0707   -0.0334   -0.8818    0.0000];
//invD_22 = [0.4942    0.4028    2.3447    0.5000];
//d1_k = [-1.2335   -0.3520   -1.5988   -1.8315];
//d2_k = [1.5265   -1.5861    0.0067    1.0221];
//vec_max = d1_k.^2.0.*invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2.0.*invD_22;
//[value, pos1] = max(vec_max);
//vec_max = d1_k  .^  2.0   .*invD_11+2*d1_k.*d2_k.*invD_12+d2_k  .^  2.0   .*invD_22;
//[value, pos2] = max(vec_max);
//vec_max = d1_k.^2.*invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2.*invD_22;
//[value, pos3] = max(vec_max);
//vec_max = d1_k.^2    .* invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2   .*invD_22;
//[value, pos4] = max(vec_max);
//x = (pos1 == 4) && (pos2 == 4) && (pos3 == 4) && (pos4 == 4);
//@}
//!

static Array ScreenIntegerScalars(Array B) {
  if (B.isScalar() && (B.dataClass() >= FM_FLOAT)
      && (B.dataClass() <= FM_DOUBLE)) {
    if (B.getContentsAsIntegerScalar() ==
	B.getContentsAsDoubleScalar())
      return Array::int32Constructor(B.getContentsAsIntegerScalar());
  }
  return B;
}

Array DotPower(Array A, Array B, Interpreter* m_eval) {
  // Special case -- kludge to fix bug 1804267
  B = ScreenIntegerScalars(B);
  Array C(DoPowerTwoArgFunction(A,B));
  if (A.sparse())
    C.makeSparse();
  return(C);
}

/**
 * Element-wise less than.
 */
//!
//@Module COMPARISONOPS Array Comparison Operators
//@@Section OPERATORS
//@@Usage
//There are a total of six comparison operators available in FreeMat, all of which are binary operators with the following syntax
//@[
//  y = a < b
//  y = a <= b
//  y = a > b
//  y = a >= b
//  y = a ~= b
//  y = a == b
//@]
//where @|a| and @|b| are numerical arrays or scalars, and @|y| is a @|logical| array of the appropriate size.  Each of the operators has three modes of operation, summarized in the following list:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is an n-dimensional array - the output is then the same size as @|b|, and contains the result of comparing each element in @|b| to the scalar @|a|.
//  \item @|a| is an n-dimensional array, @|b| is a scalar - the output is the same size as @|a|, and contains the result of comparing each element in @|a| to the scalar @|b|.
//  \item @|a| and @|b| are both n-dimensional arrays of the same size - the output is then the same size as both @|a| and @|b|, and contains the result of an element-wise comparison between @|a| and @|b|.
//\end{enumerate}
//The operators behave the same way as in @|C|, with unequal types meing promoted using the standard type promotion rules prior to comparisons.  The only difference is that in FreeMat, the not-equals operator is @|~=| instead of @|!=|.
//@@Examples
//Some simple examples of comparison operations.  First a comparison with a scalar:
//@<
//a = randn(1,5)
//a>0
//@>
//Next, we construct two vectors, and test for equality:
//@<
//a = [1,2,5,7,3]
//b = [2,2,5,9,4]
//c = a == b
//@>
//@@Tests
//@{ test_sparse84.m
//function x = test_sparse84
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse85.m
//function x = test_sparse85
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse86.m
//function x = test_sparse86
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse87.m
//function x = test_sparse87
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse88.m
//function x = test_sparse88
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse89.m
//function x = test_sparse89
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse90.m
//function x = test_sparse90
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse91.m
//function x = test_sparse91
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse92.m
//function x = test_sparse92
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse93.m
//function x = test_sparse93
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse94.m
//function x = test_sparse94
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse95.m
//function x = test_sparse95
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse96.m
//function x = test_sparse96
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse97.m
//function x = test_sparse97
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse98.m
//function x = test_sparse98
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse99.m
//function x = test_sparse99
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse100.m
//function x = test_sparse100
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse101.m
//function x = test_sparse101
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse102.m
//function x = test_sparse102
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse103.m
//function x = test_sparse103
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse104.m
//function x = test_sparse104
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse105.m
//function x = test_sparse105
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse106.m
//function x = test_sparse106
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse107.m
//function x = test_sparse107
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse108.m
//function x = test_sparse108
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse109.m
//function x = test_sparse109
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse110.m
//function x = test_sparse110
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse111.m
//function x = test_sparse111
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse112.m
//function x = test_sparse112
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse113.m
//function x = test_sparse113
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//!
Array LessThan(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,"<");
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }

  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_LT),true);
  } else if (Astride && !Bstride && A.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_LT),true);
  } else if (!Astride && Bstride && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_GT),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*sizeof(logical));
  switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A<B");
  case FM_INT32:
    lessthanfuncreal<int32>(Clen,(logical*) Cp, 
			    (int32*) A.getDataPointer(), Astride,
			    (int32*) B.getDataPointer(), Bstride);
    break;
  case FM_INT64:
    lessthanfuncreal<int64>(Clen,(logical*) Cp, 
			    (int64*) A.getDataPointer(), Astride,
			    (int64*) B.getDataPointer(), Bstride);
    break;
  case FM_FLOAT:
    lessthanfuncreal<float>(Clen,(logical*) Cp, 
			    (float*) A.getDataPointer(), Astride,
			    (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DOUBLE:
    lessthanfuncreal<double>(Clen,(logical*) Cp, 
			     (double*) A.getDataPointer(), Astride,
			     (double*) B.getDataPointer(), Bstride);
    break;
  case FM_COMPLEX:
    lessthanfunccomplex<float>(Clen,(logical*) Cp, 
			       (float*) A.getDataPointer(), Astride,
			       (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DCOMPLEX:
    lessthanfunccomplex<double>(Clen,(logical*) Cp, 
				(double*) A.getDataPointer(), Astride,
				(double*) B.getDataPointer(), Bstride);
    break;			 
  }
  return Array(FM_LOGICAL,Cdim,Cp);
}

/**
 * Element-wise less equals.
 */
Array LessEquals(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,"<=");
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }
  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_LE),true);
  } else if (Astride && !Bstride && A.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_LE),true);
  } else if (!Astride && Bstride && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_GE),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*sizeof(logical));
  switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A<=B");
  case FM_INT32:
    lessequalsfuncreal<int32>(Clen,(logical*) Cp, 
			      (int32*) A.getDataPointer(), Astride,
			      (int32*) B.getDataPointer(), Bstride);
    break;
  case FM_INT64:
    lessequalsfuncreal<int64>(Clen,(logical*) Cp, 
			      (int64*) A.getDataPointer(), Astride,
			      (int64*) B.getDataPointer(), Bstride);
    break;
  case FM_FLOAT:
    lessequalsfuncreal<float>(Clen,(logical*) Cp, 
			      (float*) A.getDataPointer(), Astride,
			      (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DOUBLE:
    lessequalsfuncreal<double>(Clen,(logical*) Cp, 
			       (double*) A.getDataPointer(), Astride,
			       (double*) B.getDataPointer(), Bstride);
    break;
  case FM_COMPLEX:
    lessequalsfunccomplex<float>(Clen,(logical*) Cp, 
				 (float*) A.getDataPointer(), Astride,
				 (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DCOMPLEX:
    lessequalsfunccomplex<double>(Clen,(logical*) Cp, 
				  (double*) A.getDataPointer(), Astride,
				  (double*) B.getDataPointer(), Bstride);
    break;			 
  }
  return Array(FM_LOGICAL,Cdim,Cp);
}
  
/**
 * Element-wise greater than.
 */
Array GreaterThan(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,">");
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }
  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_GT),true);
  } else if (Astride && !Bstride && A.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_GT),true);
  } else if (!Astride && Bstride && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_LT),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*sizeof(logical));
  switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A>B");
  case FM_INT32:
    greaterthanfuncreal<int32>(Clen,(logical*) Cp, 
			       (int32*) A.getDataPointer(), Astride,
			       (int32*) B.getDataPointer(), Bstride);
    break;
  case FM_INT64:
    greaterthanfuncreal<int64>(Clen,(logical*) Cp, 
			       (int64*) A.getDataPointer(), Astride,
			       (int64*) B.getDataPointer(), Bstride);
    break;
  case FM_FLOAT:
    greaterthanfuncreal<float>(Clen,(logical*) Cp, 
			       (float*) A.getDataPointer(), Astride,
			       (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DOUBLE:
    greaterthanfuncreal<double>(Clen,(logical*) Cp, 
				(double*) A.getDataPointer(), Astride,
				(double*) B.getDataPointer(), Bstride);
    break;
  case FM_COMPLEX:
    greaterthanfunccomplex<float>(Clen,(logical*) Cp, 
				  (float*) A.getDataPointer(), Astride,
				  (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DCOMPLEX:
    greaterthanfunccomplex<double>(Clen,(logical*) Cp, 
				   (double*) A.getDataPointer(), Astride,
				   (double*) B.getDataPointer(), Bstride);
    break;			 
  }
  return Array(FM_LOGICAL,Cdim,Cp);
}

/**
 * Element-wise greater equals.
 */
Array GreaterEquals(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,">=");
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }
  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_GE),true);
  } else if (Astride && !Bstride && A.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_GE),true);
  } else if (!Astride && Bstride && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_LE),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*sizeof(logical));
  switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A>=B");
  case FM_INT32:
    greaterequalsfuncreal<int32>(Clen,(logical*) Cp, 
				 (int32*) A.getDataPointer(), Astride,
				 (int32*) B.getDataPointer(), Bstride);
    break;
  case FM_INT64:
    greaterequalsfuncreal<int64>(Clen,(logical*) Cp, 
				 (int64*) A.getDataPointer(), Astride,
				 (int64*) B.getDataPointer(), Bstride);
    break;
  case FM_FLOAT:
    greaterequalsfuncreal<float>(Clen,(logical*) Cp, 
				 (float*) A.getDataPointer(), Astride,
				 (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DOUBLE:
    greaterequalsfuncreal<double>(Clen,(logical*) Cp, 
				  (double*) A.getDataPointer(), Astride,
				  (double*) B.getDataPointer(), Bstride);
    break;
  case FM_COMPLEX:
    greaterequalsfunccomplex<float>(Clen,(logical*) Cp, 
				    (float*) A.getDataPointer(), Astride,
				    (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DCOMPLEX:
    greaterequalsfunccomplex<double>(Clen,(logical*) Cp, 
				     (double*) A.getDataPointer(), Astride,
				     (double*) B.getDataPointer(), Bstride);
    break;			 
  }
  return Array(FM_LOGICAL,Cdim,Cp);
}

/**
 * Element-wise equals.
 */
Array Equals(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,"==");
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }
  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_EQ),true);
  } else if (Astride && !Bstride && A.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_EQ),true);
  } else if (!Astride && Bstride && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_EQ),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*sizeof(logical));
  switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A==B");
  case FM_INT32:
    equalsfuncreal<int32>(Clen,(logical*) Cp, 
			  (int32*) A.getDataPointer(), Astride,
			  (int32*) B.getDataPointer(), Bstride);
    break;
  case FM_INT64:
    equalsfuncreal<int64>(Clen,(logical*) Cp, 
			  (int64*) A.getDataPointer(), Astride,
			  (int64*) B.getDataPointer(), Bstride);
    break;
  case FM_FLOAT:
    equalsfuncreal<float>(Clen,(logical*) Cp, 
			  (float*) A.getDataPointer(), Astride,
			  (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DOUBLE:
    equalsfuncreal<double>(Clen,(logical*) Cp, 
			   (double*) A.getDataPointer(), Astride,
			   (double*) B.getDataPointer(), Bstride);
    break;
  case FM_COMPLEX:
    equalsfunccomplex<float>(Clen,(logical*) Cp, 
			     (float*) A.getDataPointer(), Astride,
			     (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DCOMPLEX:
    equalsfunccomplex<double>(Clen,(logical*) Cp, 
			      (double*) A.getDataPointer(), Astride,
			      (double*) B.getDataPointer(), Bstride);
    break;			 
  }
  return Array(FM_LOGICAL,Cdim,Cp);
}

/**
 * Element-wise notEquals.
 */
Array NotEquals(Array A, Array B, Interpreter* m_eval) {
  // Process the two arguments through the type check and dimension checks...
  VectorCheck(A,B,false,"~=");
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }
  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_NE),true);
  } else if (Astride && !Bstride && A.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_NE),true);
  } else if (!Astride && Bstride && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_NE),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*sizeof(logical));
  switch(B.dataClass()) {
    default: throw Exception("Unhandled type for second argument to A~=B");
  case FM_INT32:
    notequalsfuncreal<int32>(Clen,(logical*) Cp, 
			     (int32*) A.getDataPointer(), Astride,
			     (int32*) B.getDataPointer(), Bstride);
    break;
  case FM_INT64:
    notequalsfuncreal<int64>(Clen,(logical*) Cp, 
			     (int64*) A.getDataPointer(), Astride,
			     (int64*) B.getDataPointer(), Bstride);
    break;
  case FM_FLOAT:
    notequalsfuncreal<float>(Clen,(logical*) Cp, 
			     (float*) A.getDataPointer(), Astride,
			     (float*) B.getDataPointer(), Bstride);
    break;
  case FM_DOUBLE:
    notequalsfuncreal<double>(Clen,(logical*) Cp, 
			      (double*) A.getDataPointer(), Astride,
			      (double*) B.getDataPointer(), Bstride);
    break;
  case FM_COMPLEX:
    notequalsfunccomplex<float>(Clen,(logical*) Cp, 
				(float*) A.getDataPointer(), Astride,
				(float*) B.getDataPointer(), Bstride);
    break;
  case FM_DCOMPLEX:
    notequalsfunccomplex<double>(Clen,(logical*) Cp, 
				 (double*) A.getDataPointer(), Astride,
				 (double*) B.getDataPointer(), Bstride);
    break;			 
  }
  return Array(FM_LOGICAL,Cdim,Cp);
}

/**
 * Element-wise and
 */
//!
//@Module LOGICALOPS Logical Array Operators
//@@Section OPERATORS
//@@Usage
//There are three Boolean operators available in FreeMat.  The syntax for their use is:
//@[
//  y = ~x
//  y = a & b
//  y = a | b
//@]
//where @|x|, @|a| and @|b| are @|logical| arrays.  The operators are
//\begin{itemize}
//\item NOT (@|~|) - output @|y| is true if the corresponding element of @|x| is false, and ouput @|y| is false if the corresponding element of @|x| is true.
//\item OR (@|||) - output @|y| is true if corresponding element of @|a| is true or if corresponding element of @|b| is true (or if both are true).
//\item AND (@|\&|) - output @|y| is true only if both the corresponding elements of @|a| and @|b| are both true.
//\end{itemize}
//The binary operators AND and OR can take scalar arguments as well as vector arguments, in which case, the scalar is operated on with each element of the vector.
//As of version 1.10, FreeMat supports @|shortcut| evaluation.  This means that
//if we have two expressions
//@[
//  if (expr1 & expr2)
//@]
//then if @|expr1| evaluates to @|false|, then @|expr2| is not evaluated at all.
//Similarly, for the expression
//@[
//  if (expr1 | expr2)
//@]
//then if @|expr1| evaluates to @|true|, then @|expr2| is not evaluated at all.
//Shortcut evaluation is useful for doing a sequence of tests, each of which is
//not valid unless the prior test is successful.  For example,
//@[
//  if isa(p,'string') & strcmp(p,'fro')
//@]
//is not valid without shortcut evaluation (if @|p| is an integer, for example,
//the first test returns false, and an attempt to evaluate the second expression
//would lead to an error).  Note that shortcut evaluation only works with scalar
//expressions.
//@@Examples
//Some simple examples of logical operators.  Suppose we want to calculate the exclusive-or (XOR) of two vectors of logical variables.  First, we create a pair of vectors to perform the XOR operation on:
//@<
//a = (randn(1,6)>0)
//b = (randn(1,6)>0)
//@>
//Next, we can compute the OR of @|a| and @|b|:
//@<
//c = a | b
//@>
//However, the XOR and OR operations differ on the fifth entry - the XOR would be false, since it is true if and only if exactly one of the two inputs is true.  To isolate this case, we can AND the two vectors, to find exactly those entries that appear as true in both @|a| and @|b|:
//@<
//d = a & b
//@>
//At this point, we can modify the contents of @|c| in two ways -- the Boolean way is to AND @|\sim d| with @|c|, like so
//@<
//xor = c & (~d)
//@>
//The other way to do this is simply force @|c(d) = 0|, which uses the logical indexing mode of FreeMat (see the chapter on indexing for more details).  This, however, will cause @|c| to become an @|int32| type, as opposed to a logical type.
//@<
//c(d) = 0
//@>
//@@Tests
//@{ test_sparse114.m
//function x = test_sparse114
//A = sprandn(10,10,0.3)>0;
//B = sprandn(10,10,0.3)>0;
//C = A & B;
//c = full(A) & full(B);
//x = testeq(c,C);
//C = A & 0;
//c = full(A) & 0;
//x = x & testeq(c,C);
//C = 1 & A;
//c = 1 & full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse115.m
//function x = test_sparse115
//A = sprandn(10,10,0.3)>0;
//B = sprandn(10,10,0.3)>0;
//C = A | B;
//c = full(A) | full(B);
//x = testeq(c,C);
//C = A | 0;
//c = full(A) | 0;
//x = x | testeq(c,C);
//C = 1 | A;
//c = 1 | full(A);
//x = x | testeq(c,C);
//@}
//!
Array And(Array A, Array B, Interpreter* m_eval) {
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  BoolVectorCheck(A,B,"&");

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }

  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_AND),true);
  } else if (Astride && !Bstride && A.sparse()) {
    B.promoteType(FM_UINT32);
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_AND),true);
  } else if (!Astride && Bstride && B.sparse()) {
    A.promoteType(FM_UINT32);
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_AND),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*B.getElementSize());
  boolean_and(Clen, (logical*) Cp, (const logical*) A.getDataPointer(), Astride, 
	      (const logical*) B.getDataPointer(), Bstride);
  return Array(FM_LOGICAL,Cdim,Cp);
}

/**
 * Element-wise or
 */
Array Or(Array A, Array B, Interpreter* m_eval) {
  int Astride, Bstride;
  void *Cp = NULL;
  int Clen;
  Dimensions Cdim;

  BoolVectorCheck(A,B,"|");

  if (A.isScalar()) {
    Astride = 0;
    Bstride = 1;
    Cdim = B.dimensions();
  } else if (B.isScalar()) {
    Astride = 1;
    Bstride = 0;
    Cdim = A.dimensions();
  } else {
    Astride = 1;
    Bstride = 1;
    Cdim = A.dimensions();
  }
  // Check for sparse arguments
  if (Astride && Bstride && A.sparse() && B.sparse()) {
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseSparseLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getSparseDataPointer(),
				       SLO_OR),true);
  } else if (Astride && !Bstride && A.sparse()) {
    B.promoteType(FM_UINT32);
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(A.dataClass(),
				       A.getDimensionLength(0),
				       A.getDimensionLength(1),
				       A.getSparseDataPointer(),
				       B.getDataPointer(),
				       SLO_OR),true);
  } else if (!Astride && Bstride && B.sparse()) {
    A.promoteType(FM_UINT32);
    return Array(FM_LOGICAL,
		 Cdim,
		 SparseScalarLogicalOp(B.dataClass(),
				       B.getDimensionLength(0),
				       B.getDimensionLength(1),
				       B.getSparseDataPointer(),
				       A.getDataPointer(),
				       SLO_OR),true);
  }

  A.makeDense();
  B.makeDense();

  Clen = Cdim.getElementCount();
  Cp = Malloc(Clen*B.getElementSize());
  boolean_or(Clen, (logical*) Cp, (const logical*) A.getDataPointer(), Astride, 
	     (const logical*) B.getDataPointer(), Bstride);
  return Array(FM_LOGICAL,Cdim,Cp);
}

/**
 * Element-wise not
 */
Array Not(Array A, Interpreter* m_eval) {
  Array C;

  A.promoteType(FM_LOGICAL);
  C = Array(FM_LOGICAL,A.dimensions(),NULL);
  void *Cp = Malloc(A.getLength()*C.getElementSize());
  boolean_not(A.getLength(),(logical*)Cp,(const logical*) A.getDataPointer());
  C.setDataPointer(Cp);
  return C;
}

Array Plus(Array A, Interpreter* m_eval) {
  return A;
}

/**
 * Element-wise negate - this one is a custom job, so to speak.
 * 
 */
Array Negate(Array A, Interpreter* m_eval){
  Array C;
  Class Aclass;

  if (A.isReferenceType())
    throw Exception("Cannot negate non-numeric types.");
  Aclass = A.dataClass();
  
  if (Aclass == FM_STRING)
    Aclass = FM_INT32;
  else if (Aclass < FM_INT32)
    Aclass = FM_INT32;

  A.promoteType(Aclass);

  C = Array(Aclass,A.dimensions(),NULL);
  void *Cp = Malloc(A.getLength()*C.getElementSize());
  switch (Aclass) {
  default: throw Exception("Unhandled type for -A");
  case FM_INT32:
    neg<int32>(A.getLength(),(int32*)Cp,(int32*)A.getDataPointer());
    break;
  case FM_INT64:
    neg<int64>(A.getLength(),(int64*)Cp,(int64*)A.getDataPointer());
    break;
  case FM_FLOAT:
    neg<float>(A.getLength(),(float*)Cp,(float*)A.getDataPointer());
    break;
  case FM_DOUBLE:
    neg<double>(A.getLength(),(double*)Cp,(double*)A.getDataPointer());
    break;
  case FM_COMPLEX:
    neg<float>(2*A.getLength(),(float*)Cp,(float*)A.getDataPointer());
    break;
  case FM_DCOMPLEX:
    neg<double>(2*A.getLength(),(double*)Cp,(double*)A.getDataPointer());
    break;
  }
  C.setDataPointer(Cp);
  return C;
}


/**
 * Matrix-matrix multiply
 */
//!
//@Module TIMES Matrix Multiply Operator
//@@Section OPERATORS
//@@Usage
//Multiplies two numerical arrays.  This operator is really a combination
//of three operators, all of which have the same general syntax:
//@[
//  y = a * b
//@]
//where @|a| and @|b| are arrays of numerical type.  The result @|y| depends
//on which of the following three situations applies to the arguments
//@|a| and @|b|:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is an arbitrary @|n|-dimensional numerical array, in which case the output is the element-wise product of @|b| with the scalar @|a|.
//  \item @|b| is a scalar, @|a| is an arbitrary @|n|-dimensional numerical array, in which case the output is the element-wise product of @|a| with the scalar @|b|.
//  \item @|a,b| are conformant matrices, i.e., @|a| is of size @|M x K|, and @|b| is of size @|K x N|, in which case the output is of size @|M x N| and is the matrix product of @|a|, and @|b|.
//\end{enumerate}
//The output follows the standard type promotion rules, although in the first two cases, if @|a| and @|b| are integers, the output is an integer also, while in the third case if @|a| and @|b| are integers, ,the output is of type @|double|.
//@@Function Internals
//There are three formulae for the times operator.  For the first form
//\[
//y(m_1,\ldots,m_d) = a \times b(m_1,\ldots,m_d),
//\]
//and the second form
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b.
//\]
//In the third form, the output is the matrix product of the arguments
//\[
//y(m,n) = \sum_{k=1}^{K} a(m,k) b(k,n)
//\]
//@@Examples
//Here are some examples of using the matrix multiplication operator.  First,
//the scalar examples (types 1 and 2 from the list above):
//@<
//a = [1,3,4;0,2,1]
//b = a * 2
//@>
//The matrix form, where the first argument is @|2 x 3|, and the
//second argument is @|3 x 1|, so that the product is size 
//@|2 x 1|.
//@<
//a = [1,2,0;4,2,3]
//b = [5;3;1]
//c = a*b
//@>
//Note that the output is double precision.
//@@Tests
//@{ test_sparse50.m
//function x = test_sparse50
//a = rand(200,100);
//b = rand(100,300);
//a(a>0.1) = 0;
//b(b>0.1) = 0;
//c = a*b;
//A = sparse(a);
//B = sparse(b);
//C = A*B;
//x = testeq(c,C);
//@}
//@{ test_sparse51.m
//function x = test_sparse51
//a = rand(200,100);
//b = rand(100,300);
//c = rand(200,100);
//d = rand(100,300);
//a(a>0.1) = 0;
//b(b>0.1) = 0;
//c(c>0.1) = 0;
//d(d>0.1) = 0;
//f = a + i*c;
//g = b + i*d;
//h = f*g;
//F = sparse(f);
//G = sparse(g);
//H = F*G;
//x = testeq(h,H);
//@}
//@{ test_sparse52.m
//function x = test_sparse52
//a = rand(200,100);
//b = rand(100,300);
//a(a>0.1) = 0;
//c = a*b;
//A = sparse(a);
//C = A*b;
//x = testeq(c,C);
//@}
//@{ test_sparse53.m
//function x = test_sparse53
//a = rand(200,100);
//b = rand(100,300);
//c = rand(200,100);
//d = rand(100,300);
//a(a>0.1) = 0;
//b(b>0.1) = 0;
//c(c>0.1) = 0;
//d(d>0.1) = 0;
//f = a + i*c;
//g = b + i*d;
//h = f*g;
//F = sparse(f);
//H = F*g;
//x = testeq(h,H);
//@}
//@{ test_sparse54.m
//function x = test_sparse54
//a = rand(200,100);
//b = rand(100,300);
//b(b>0.1) = 0;
//c = a*b;
//B = sparse(b);
//C = a*B;
//x = testeq(c,C);
//@}
//@{ test_sparse55.m
//function x = test_sparse55
//a = randn(200,100);
//b = randn(100,300);
//c = randn(200,100);
//d = randn(100,300);
//c(c>0.1) = 0;
//d(d>0.1) = 0;
//f = a + i*c;
//g = b + i*d;
//h = f*g;
//G = sparse(g);
//H = f*G;
//e = h - H;
//t = max(abs(e(:)));
//x = (t < eps*600);
//@}
//@{ test_sparse59.m
//% Test sparse-dense matrix multiplication
//function x = test_sparse59
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,450);
//[yf2,zf2] = sparse_test_mat('float',400,450);
//[yd2,zd2] = sparse_test_mat('double',400,450);
//[yc2,zc2] = sparse_test_mat('complex',400,450);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,450);
//x = testeq(yi1*zi2,zi1*zi2) & ...
//    testeq(yf1*zf2,zf1*zf2) & ...
//    testeq(yd1*zd2,zd1*zd2) & ...
//    testeq(yc1*zc2,zc1*zc2) & ...
//    testeq(yz1*zz2,zz1*zz2);
//@}
//@{ test_sparse60.m
//% Test dense-sparse matrix multiplication
//function x = test_sparse60
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,450);
//[yf2,zf2] = sparse_test_mat('float',400,450);
//[yd2,zd2] = sparse_test_mat('double',400,450);
//[yc2,zc2] = sparse_test_mat('complex',400,450);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,450);
//x = testeq(zi1*yi2,zi1*zi2) & ...
//    testeq(zf1*yf2,zf1*zf2) & ...
//    testeq(zd1*yd2,zd1*zd2) & ...
//    testeq(zc1*yc2,zc1*zc2) & ...
//    testeq(zz1*yz2,zz1*zz2);
//@}
//@{ test_sparse61.m
//% Test sparse-sparse matrix multiplication
//function x = test_sparse61
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,450);
//[yf2,zf2] = sparse_test_mat('float',400,450);
//[yd2,zd2] = sparse_test_mat('double',400,450);
//[yc2,zc2] = sparse_test_mat('complex',400,450);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,450);
//x = testeq(yi1*yi2,zi1*zi2) & ...
//    testeq(yf1*yf2,zf1*zf2) & ...
//    testeq(yd1*yd2,zd1*zd2) & ...
//    testeq(yc1*yc2,zc1*zc2) & ...
//    testeq(yz1*yz2,zz1*zz2);
//@}
//@{ test_sparse80.m
//% Test sparse-sparse matrix array multiply
//function x = test_sparse80
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,600);
//[yf2,zf2] = sparse_test_mat('float',400,600);
//[yd2,zd2] = sparse_test_mat('double',400,600);
//[yc2,zc2] = sparse_test_mat('complex',400,600);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,600);
//x = testeq(yi1*yi2,zi1*zi2) & testeq(yf1*yf2,zf1*zf2) & testeq(yd1*yd2,zd1*zd2) & testeq(yc1*yc2,zc1*zc2) & testeq(yz1*yz2,zz1*zz2);
//@}
//!
Array Multiply(Array A, Array B, Interpreter* m_eval){
  // Process our arguments
  if (!MatrixCheck(A,B,"*"))
    // Its really a vector product, pass...
    return DotMultiply(A,B,m_eval);
  
  // Test for conformancy
  if (A.getDimensionLength(1) != B.getDimensionLength(0)) 
    throw Exception("Requested matrix multiplication requires arguments to be conformant.");

  int Arows, Acols;
  int Bcols;
  
  Arows = A.getDimensionLength(0);
  Acols = A.getDimensionLength(1);
  Bcols = B.getDimensionLength(1);

  Dimensions outDim(Arows,Bcols);
  
  // Check for sparse multiply case
  if (A.sparse() && !B.sparse())
    return Array(A.dataClass(),
		 outDim,
		 SparseDenseMatrixMultiply(A.dataClass(),
					   Arows,Acols,Bcols,
					   A.getSparseDataPointer(),
					   B.getDataPointer()),
		 false);
  if (!A.sparse() && B.sparse())
    return Array(A.dataClass(),
		 outDim,
		 DenseSparseMatrixMultiply(A.dataClass(),
					   Arows,Acols,Bcols,
					   A.getDataPointer(),
					   B.getSparseDataPointer()),
		 false);
  if (A.sparse() && B.sparse())
    return Array(A.dataClass(),
		 outDim,
		 SparseSparseMatrixMultiply(A.dataClass(),
					    Arows,Acols,Bcols,
					    A.getSparseDataPointer(),
					    B.getSparseDataPointer()),
		 true);

  // Its really a matrix-matrix operation, and the arguments are
  // satisfactory.  Check for the type.
  void *Cp = Malloc(Arows*Bcols*A.getElementSize());
  
  if (A.dataClass() == FM_FLOAT)
    floatMatrixMatrixMultiply(Arows,Bcols,Acols,(float*)Cp,
			      (const float*)A.getDataPointer(),
			      (const float*)B.getDataPointer());
  else if (A.dataClass() == FM_DOUBLE)
    doubleMatrixMatrixMultiply(Arows,Bcols,Acols,(double*)Cp,
			       (const double*)A.getDataPointer(),
			       (const double*)B.getDataPointer());
  else if (A.dataClass() == FM_COMPLEX)
    complexMatrixMatrixMultiply(Arows,Bcols,Acols,(float*)Cp,
				(const float*)A.getDataPointer(),
				(const float*)B.getDataPointer());
  else if (A.dataClass() == FM_DCOMPLEX)
    dcomplexMatrixMatrixMultiply(Arows,Bcols,Acols,(double*)Cp,
				 (const double*)A.getDataPointer(),
				 (const double*)B.getDataPointer());
  return Array(A.dataClass(),outDim,Cp);
}
    
/**
 * Matrix-matrix divide
 */
//!
//@Module LEFTDIVIDE Matrix Equation Solver/Divide Operator
//@@Section OPERATORS
//@@Usage
//The divide operator @|\| is really a combination of three
//operators, all of which have the same general syntax:
//@[
//  Y = A \ B
//@]
//where @|A| and @|B| are arrays of numerical type.  The result @|Y| depends
//on which of the following three situations applies to the arguments
//@|A| and @|B|:
//\begin{enumerate}
//  \item @|A| is a scalar, @|B| is an arbitrary @|n|-dimensional numerical array, in which case the output is each element of @|B| divided by the scalar @|A|.
//  \item @|B| is a scalar, @|A| is an arbitrary @|n|-dimensional numerical array, in which case the output is the scalar @|B| divided by each element of @|A|.
//  \item @|A,B| are matrices with the same number of rows, i.e., @|A| is of size @|M x K|, and @|B| is of size @|M x L|, in which case the output is of size @|K x L|.
//\end{enumerate}
//The output follows the standard type promotion rules, although in the first two cases, if @|A| and @|B| are integers, the output is an integer also, while in the third case if @|A| and @|B| are integers, the output is of type @|double|.
//
//A few additional words about the third version, in which @|A| and @|B| are matrices.  Very loosely speaking, @|Y| is the matrix that satisfies @|A * Y = B|.  In cases where such a matrix exists.  If such a matrix does not exist, then a matrix @|Y| is returned that approximates @|A * Y \approx B|.
//@@Function Internals
//There are three formulae for the times operator.  For the first form
//\[
//Y(m_1,\ldots,m_d) = \frac{B(m_1,\ldots,m_d)}{A},
//\]
//and the second form
//\[
//Y(m_1,\ldots,m_d) = \frac{B}{A(m_1,\ldots,m_d)}.
//\]
//In the third form, the calculation of the output depends on the size of @|A|. Because each column of @|B| is treated independantly, we can rewrite the equation @|A Y = B| as
//\[
//  A [y_1, y_2,\ldots, y_l] = [b_1, b_2, \ldots, b_l]
//\]
//where @|y_i| are the columns of @|Y|, and @|b_i| are the columns of the matrix @|B|. If @|A| is a square matrix, then the LAPACK routine @|*gesvx| (where the @|*| is replaced with @|sdcz| depending on the type of the arguments) is used, which uses an LU decomposition of @|A| to solve the sequence of equations sequentially.  If @|A| is singular, then a warning is emitted. 
//
//On the other hand, if @|A| is rectangular, then the LAPACK routine @|*gelsy| is used.  Note that these routines are designed to work with matrices @|A| that are full rank - either full column rank or full row rank.  If @|A| fails to satisfy this assumption, a warning is emitted.  If @|A| has full column rank (and thus necessarily has more rows than columns), then theoretically, this operator finds the columns @|y_i| that satisfy:
//\[
//  y_i = \arg \min_y \| A y - b_i \|_2
//\]
//and each column is thus the Least Squares solution of @|A y = b_i|.  On the other hand, if @|A| has full row rank (and thus necessarily has more columns than rows), then theoretically, this operator finds the columns @|y_i| that satisfy
//\[
//  y_i = \arg \min_{A y = b_i} \| y \|_2
//\]
//and each column is thus the Minimum Norm vector @|y_i| that satisfies @|A y_i = b_i|.  
//In the event that the matrix @|A| is neither full row rank nor full column rank, a solution is returned, that is the minimum norm least squares solution.  The solution is computed using an orthogonal factorization technique that is documented in the LAPACK User's Guide (see the References section for details).
//@@Examples
//Here are some simple examples of the divide operator.  We start with a simple example of a full rank, square matrix:
//@<
//A = [1,1;0,1]
//@>
//Suppose we wish to solve
//\[
//  \begin{bmatrix} 1 & 1 \\ 0 & 1 \end{bmatrix}
//  \begin{bmatrix} y_1 \\ y_2 \end{bmatrix}
// = 
//  \begin{bmatrix} 3 \\ 2 \end{bmatrix}
//\]
//(which by inspection has the solution @|y_1 = 1|, @|y_2 = 2|).  Thus we compute:
//@<
//B = [3;2]
//Y = A\B
//@>
//
//Suppose we wish to solve a trivial Least Squares (LS) problem.  We want to find a simple scaling of the vector @|[1;1]| that is closest to the point @|[2,1]|.  This is equivalent to solving
//\[
//\begin{bmatrix} 1 \\ 1 \end{bmatrix} y = \begin{bmatrix} 2 \\ 1 \end{bmatrix}
//\]
//in a least squares sense.  For fun, we can calculate the solution using calculus by hand.  The error we wish to minimize is
//\[
//  \varepsilon(y) = (y - 2)^2 + (y-1)^2.
//\]
//Taking a derivative with respect to @|y|, and setting to zero (which we must have for an extrema when @|y| is unconstrained)
//\[
//  2 (y-2) + 2 (y-1) = 0
//\]
//which we can simplify to @|4y = 6| or @|y = 3/2| (we must, technically, check to make sure this is a minimum, and not a maximum or an inflection point).  Here is the same calculation performed using FreeMat:
//@<
//A = [1;1]
//B = [2;1]
//A\B
//@>
//which is the same solution.
//!
Array LeftDivide(Array A, Array B, Interpreter* m_eval) {
//   if (A.isEmpty() || B.isEmpty())
//     return Array::emptyConstructor();
  StringVector dummySV;
  // Process our arguments
  if (!MatrixCheck(A,B,"\\"))
    // Its really a vector product, pass...
    return DotLeftDivide(A,B,m_eval);
  
  // Test for conformancy
  if (A.getDimensionLength(0) != B.getDimensionLength(0)) 
    throw Exception("Requested divide operation requires arguments to have correct dimensions.");

  int Arows, Acols;
  int Brows, Bcols;
  
  Arows = A.getDimensionLength(0);
  Acols = A.getDimensionLength(1);
  Brows = B.getDimensionLength(0);
  Bcols = B.getDimensionLength(1);

  // Check for sparse case...
  if (A.sparse()) {
    // Make sure B is _not_ sparse
    B.makeDense();
    // Make sure A is square
    if (Arows != Acols)
      throw Exception("FreeMat currently only supports A\\b for square matrices A");
    // Make sure A is either double or dcomplex
    if ((A.dataClass() == FM_FLOAT) || (A.dataClass() == FM_COMPLEX))
      throw Exception("FreeMat currently only supports A\\b for double and dcomplex matrices A");
    Dimensions outDim(Arows,Bcols);
    return Array(A.dataClass(),outDim,
		 SparseSolveLinEq(A.dataClass(),Arows,Acols,A.getSparseDataPointer(),
				  Brows,Bcols,B.getDataPointer()),false);
  }
  
  // Its really a matrix-matrix operation, and the arguments are
  // satisfactory.  Check for the type.
  void *Cp;
  Dimensions outDim(2);
  if (Arows == Acols) {
    // Square matrix case - A is N x N, B is N x K - use 
    // linear equation solver.  Output is N x K.
    Cp = Malloc(Arows*Bcols*A.getElementSize());
    if (A.dataClass() == FM_FLOAT)
      floatSolveLinEq(m_eval,
		      Arows,Bcols,(float*)Cp,
		      (float*)A.getReadWriteDataPointer(),
		      (float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_COMPLEX)
      complexSolveLinEq(m_eval,
			Arows,Bcols,(float*)Cp,
			(float*)A.getReadWriteDataPointer(),
			(float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DOUBLE)
      doubleSolveLinEq(m_eval,
		       Arows,Bcols,(double*)Cp,
		       (double*)A.getReadWriteDataPointer(),
		       (double*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DCOMPLEX)
      dcomplexSolveLinEq(m_eval,
			 Arows,Bcols,(double*)Cp,
			 (double*)A.getReadWriteDataPointer(),
			 (double*)B.getReadWriteDataPointer());
    outDim = Dimensions(Arows,Bcols);
  } else {
    // Rectangular matrix case - A is M x N, B must be M x K - use
    // lease squares equation solver.  Output is N x K.
    Cp = Malloc(Acols*Bcols*A.getElementSize());
    if (A.dataClass() == FM_FLOAT)
      floatSolveLeastSq(m_eval,
			Arows,Acols,Bcols,(float*)Cp,
			(float*)A.getReadWriteDataPointer(),
			(float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_COMPLEX)
      complexSolveLeastSq(m_eval,
			  Arows,Acols,Bcols,(float*)Cp,
			  (float*)A.getReadWriteDataPointer(),
			  (float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DOUBLE)
      doubleSolveLeastSq(m_eval,
			 Arows,Acols,Bcols,(double*)Cp,
			 (double*)A.getReadWriteDataPointer(),
			 (double*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DCOMPLEX)
      dcomplexSolveLeastSq(m_eval,
			   Arows,Acols,Bcols,(double*)Cp,
			   (double*)A.getReadWriteDataPointer(),
			   (double*)B.getReadWriteDataPointer());
    outDim = Dimensions(Acols,Bcols);
  }
  return Array(A.dataClass(),outDim,Cp);
}
    
/**
 * Matrix-matrix divide
 */
//!
//@Module RIGHTDIVIDE Matrix Equation Solver/Divide Operator
//@@Section OPERATORS
//@@Usage
//The divide operator @|/| is really a combination of three
//operators, all of which have the same general syntax:
//@[
//  Y = A / B
//@]
//where @|A| and @|B| are arrays of numerical type.  The result @|Y| depends
//on which of the following three situations applies to the arguments
//@|A| and @|B|:
//\begin{enumerate}
//  \item @|A| is a scalar, @|B| is an arbitrary @|n|-dimensional numerical array, in which case the output is the scalar @|A| divided into each element of @|B|.
//  \item @|B| is a scalar, @|A| is an arbitrary @|n|-dimensional numerical array, in which case the output is each element of @|A| divided by the scalar @|B|.
//  \item @|A,B| are matrices with the same number of columns, i.e., @|A| is of size @|K x M|, and @|B| is of size @|L x M|, in which case the output is of size @|K x L|.
//\end{enumerate}
//The output follows the standard type promotion rules, although in the first two cases, if @|A| and @|B| are integers, the output is an integer also, while in the third case if @|A| and @|B| are integers, the output is of type @|double|.
//
//@@Function Internals
//There are three formulae for the times operator.  For the first form
//\[
//Y(m_1,\ldots,m_d) = \frac{A}{B(m_1,\ldots,m_d)},
//\]
//and the second form
//\[
//Y(m_1,\ldots,m_d) = \frac{A(m_1,\ldots,m_d)}{B}.
//\]
//In the third form, the output is defined as:
//\[
//  Y = (B' \backslash A')'
//\]
//and is used in the equation @|Y B = A|.
//@@Examples
//The right-divide operator is much less frequently used than the left-divide operator, but the concepts are similar.  It can be used to find least-squares and minimum norm solutions.  It can also be used to solve systems of equations in much the same way.  Here's a simple example:
//@<
//B = [1,1;0,1];
//A = [4,5]
//A/B
//@>
//@@Tests
//@{ test_sparse82.m
//% Test sparse-sparse matrix array solution
//function x = test_sparse82
//[yd1,zd1] = sparse_test_mat('double',100,100);
//[yz1,zz1] = sparse_test_mat('dcomplex',100,100);
//[yd2,zd2] = sparse_test_mat('double',100,60);
//[yz2,zz2] = sparse_test_mat('dcomplex',100,60);
//x = testeq3(yd1\yd2,zd1\zd2) & testeq3(yz1\yz2,zz1\zz2);
//
//function x = testeq3(a,b)
//  k = abs(a-b);
//  x = max(k(:)) < (size(a,2)*size(a,1)*eps*4);
//@}
//!
Array RightDivide(Array A, Array B, Interpreter* m_eval) {
  Array C;

  // Process our arguments
  if (!MatrixCheck(A,B,"/"))
    // Its really a vector product, pass...
    return DotRightDivide(A,B,m_eval);

  A.transpose();
  B.transpose();

  C = LeftDivide(B,A,m_eval);
  C.transpose();

  return C;
}

/**
 * Eigen decomposition, symmetric matrix, compact decomposition case
 */
void EigenDecomposeCompactSymmetric(Array A, Array& D, Interpreter* m_eval) {
  Class Aclass;

  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply eigendecomposition to reference types.");
  
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");

  if (A.getDimensionLength(0) != A.getDimensionLength(1))
    throw Exception("Cannot eigendecompose a non-square matrix.");

  int N = A.getDimensionLength(0);

  // Create one square matrix to store the eigenvectors
  Dimensions Vdims(N,1);

  // Handle the type of A - if it is an integer type, then promote to double
  Aclass = A.dataClass();
  if (Aclass < FM_FLOAT) {
    A.promoteType(FM_DOUBLE);
    Aclass = FM_DOUBLE;
  }

  // Select the eigenvector decomposition routine based on A's type
  Dimensions VDims(N,1);
  switch (Aclass) {
  default: throw Exception("Unhandled type for symmetric eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      floatEigenDecomposeSymmetric(N, NULL, eigenvals, 
				   (float*)A.getReadWriteDataPointer(),
				   false);
      // Copy the eigenvalues into a diagonal (float) matrix
      D = Array(FM_FLOAT,Vdims,eigenvals);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      doubleEigenDecomposeSymmetric(N, NULL, eigenvals, 
				    (double*)A.getReadWriteDataPointer(),
				    false);
      // Copy the eigenvalues into a diagonal (double) matrix
      D = Array(FM_DOUBLE,Vdims,eigenvals);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      complexEigenDecomposeSymmetric(N, NULL, eigenvals, 
				     (float*)A.getReadWriteDataPointer(),
				     false);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_FLOAT,Vdims,eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      dcomplexEigenDecomposeSymmetric(N, NULL, eigenvals, 
				      (double*)A.getReadWriteDataPointer(),
				      false);
      // Copy the eigenvalues into a diagonaal (complex) matrix
      D = Array(FM_DOUBLE,Vdims,eigenvals);
    }
    break;
  }
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */
void EigenDecomposeFullSymmetric(Array A, Array& V, Array& D, Interpreter* m_eval) {
  int i;
  Class Aclass;

  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply eigendecomposition to reference types.");
  
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");

  if (A.getDimensionLength(0) != A.getDimensionLength(1))
    throw Exception("Cannot eigendecompose a non-square matrix.");

  int N = A.getDimensionLength(0);

  // Create one square matrix to store the eigenvectors
  Dimensions Vdims(N,N);

  // Handle the type of A - if it is an integer type, then promote to double
  Aclass = A.dataClass();
  if (Aclass < FM_FLOAT) {
    A.promoteType(FM_DOUBLE);
    Aclass = FM_DOUBLE;
  }

  // Select the eigenvector decomposition routine based on A's type
  switch (Aclass) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      floatEigenDecomposeSymmetric(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
				   true);
      // Copy the eigenvalues into a diagonal (float) matrix
      D = Array(FM_FLOAT,Vdims,NULL);
      float *Dp = (float*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++)
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_FLOAT,Vdims,Vp);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      doubleEigenDecomposeSymmetric(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
				    true);
      // Copy the eigenvalues into a diagonal (double) matrix
      D = Array(FM_DOUBLE,Vdims,NULL);
      double *Dp = (double*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++)
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_DOUBLE,Vdims,Vp);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      complexEigenDecomposeSymmetric(N, Vp, eigenvals, 
				     (float*)A.getReadWriteDataPointer(),
				     true);
      // Copy the eigenvalues into a diagonal (real) matrix
      D = Array(FM_FLOAT,Vdims,NULL);
      float *Dp = (float*) Malloc(N*N*sizeof(float));
      for (i=0;i<N;i++) 
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_COMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      dcomplexEigenDecomposeSymmetric(N, Vp, eigenvals, 
				      (double*)A.getReadWriteDataPointer(),
				      true);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_DOUBLE,Vdims,NULL);
      double *Dp = (double*) Malloc(N*N*sizeof(double));
      for (i=0;i<N;i++) 
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_DCOMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */
void EigenDecomposeFullGeneral(Array A, Array& V, Array& D, bool balanceFlag, Interpreter* m_eval) {
  int i, j;
  Class Aclass;

  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply eigendecomposition to reference types.");
  
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");

  if (A.getDimensionLength(0) != A.getDimensionLength(1))
    throw Exception("Cannot eigendecompose a non-square matrix.");

  int N = A.getDimensionLength(0);

  // Create one square matrix to store the eigenvectors
  Dimensions Vdims(N,N);

  // Handle the type of A - if it is an integer type, then promote to double
  Aclass = A.dataClass();
  if (Aclass < FM_FLOAT) {
    A.promoteType(FM_DOUBLE);
    Aclass = FM_DOUBLE;
  }

  // Select the eigenvector decomposition routine based on A's type
  switch (Aclass) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      // For a real matrix, the eigenvectors are stored in a packed
      // format - complex eigenvectors are stored as two successive 
      // columns, corresponding to the real and imaginary parts of
      // the vector.  Successive columns can be used because the 
      // eigenvalues occur in conjugate pairs.
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      floatEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
			  true, balanceFlag);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a diagonal (float) matrix
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i+N*i] = eigenvals[2*i];
	D.setDataPointer(Dp);
	V = Array(FM_FLOAT,Vdims,Vp);
      } else {
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_COMPLEX,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_COMPLEX,Vdims,NULL);
	// We have one or more complex eigenvalues.  Allocate
	// space for a complex eigenvector matrix
	float *Vpc = (float*) Malloc(N*N*V.getElementSize());
	// Loop through the columns of Vpc
	i = 0;
	while (i<N) {
	  // Is this eigenvalue complex?
	  if ((i<N-1) && (eigenvals[2*i+1] != 0)) {
	    // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	    //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	    for (j=0;j<N;j++) {
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	      Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
	      Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
	      Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	    }
	    i += 2;
	  } else {
	    for (j=0;j<N;j++)
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	    i++;
	  }
	}
	Free(Vp);
	V.setDataPointer(Vpc);
      }
      Free(eigenvals);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      // For a real matrix, the eigenvectors are stored in a packed
      // format - complex eigenvectors are stored as two successive 
      // columns, corresponding to the real and imaginary parts of
      // the vector.  Successive columns can be used because the 
      // eigenvalues occur in conjugate pairs.
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      doubleEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
			   true, balanceFlag);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a diagonal (double) matrix
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) 
	  Dp[i+N*i] = eigenvals[2*i];
	D.setDataPointer(Dp);
	V = Array(FM_DOUBLE,Vdims,Vp);
      } else {
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DCOMPLEX,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_DCOMPLEX,Vdims,NULL);
	// We have one or more complex eigenvalues.  Allocate
	// space for a complex eigenvector matrix
	double *Vpc = (double*) Malloc(N*N*V.getElementSize());
	// Loop through the columns of Vpc
	i = 0;
	while (i<N) {
	  // Is this eigenvalue complex?
	  if ((i<N-1) && (eigenvals[2*i+1] != 0)) {
	    // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	    //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	    for (j=0;j<N;j++) {
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	      Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
	      Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
	      Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	    }
	    i += 2;
	  } else {
	    for (j=0;j<N;j++)
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	    i++;
	  }
	}
	Free(Vp);
	V.setDataPointer(Vpc);
      }
      Free(eigenvals);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      complexEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
			    true, balanceFlag);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_COMPLEX,Vdims,NULL);
      float *Dp = (float*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++) {
	Dp[2*(i+N*i)] = eigenvals[2*i];
	Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
      }
      D.setDataPointer(Dp);
      V = Array(FM_COMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      dcomplexEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
			     true, balanceFlag);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_DCOMPLEX,Vdims,NULL);
      double *Dp = (double*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++) {
	Dp[2*(i+N*i)] = eigenvals[2*i];
	Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
      }
      D.setDataPointer(Dp);
      V = Array(FM_DCOMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
void EigenDecomposeCompactGeneral(Array A, Array& D, bool balanceFlag, Interpreter* m_eval) {
  int i;
  Class Aclass;

  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply eigendecomposition to reference types.");
  
  if (!A.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");

  if (A.getDimensionLength(0) != A.getDimensionLength(1))
    throw Exception("Cannot eigendecompose a non-square matrix.");

  int N = A.getDimensionLength(0);

  // Create one square matrix to store the eigenvectors
  Dimensions Vdims(N,1);

  // Handle the type of A - if it is an integer type, then promote to double
  Aclass = A.dataClass();
  if (Aclass < FM_FLOAT) {
    A.promoteType(FM_DOUBLE);
    Aclass = FM_DOUBLE;
  }

  // Select the eigenvector decomposition routine based on A's type
  switch (Aclass) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      floatEigenDecompose(N, NULL, eigenvals, 
			  (float*)A.getReadWriteDataPointer(),
			  false, balanceFlag);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a real (float) vector
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i] = eigenvals[2*i];
	D.setDataPointer(Dp);
      } else {
	// Copy the eigenvalues into a complex vector
	D = Array(FM_COMPLEX,Vdims,NULL);
	float *Dp = (float*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*i] = eigenvals[2*i];
	  Dp[2*i+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
      }
      Free(eigenvals);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      doubleEigenDecompose(N, NULL, eigenvals, 
			   (double*)A.getReadWriteDataPointer(),
			   false, balanceFlag);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a real (double) vector
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i] = eigenvals[2*i];
	D.setDataPointer(Dp);
      } else {
	// Copy the eigenvalues into a complex vector
	D = Array(FM_DCOMPLEX,Vdims,NULL);
	double *Dp = (double*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*i] = eigenvals[2*i];
	  Dp[2*i+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
      }
      Free(eigenvals);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      complexEigenDecompose(N, NULL, eigenvals, 
			    (float*)A.getReadWriteDataPointer(),
			    false, balanceFlag);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_COMPLEX,Vdims,eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      dcomplexEigenDecompose(N, NULL, eigenvals, 
			     (double*)A.getReadWriteDataPointer(),
			     false, balanceFlag);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_DCOMPLEX,Vdims,eigenvals);
    }
    break;
  }
}

bool GeneralizedEigenDecomposeCompactSymmetric(Array A, Array B, Array& D, Interpreter* m_eval) {
  Class Aclass(A.dataClass());
  int N = A.getDimensionLength(0);
  // Select the eigenvector decomposition routine based on A's type
  Dimensions Vdims(N,1);
  switch (Aclass) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      if (!floatGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
					   (float*)A.getReadWriteDataPointer(),
					   (float*)B.getReadWriteDataPointer(),
					   false)) {
	fflush(stdout);
	Free(eigenvals);
	return false;
      }
      // Copy the eigenvalues into a diagonal (float) matrix
      D = Array(FM_FLOAT,Vdims,eigenvals);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      if (!doubleGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
					    (double*)A.getReadWriteDataPointer(),
					    (double*)B.getReadWriteDataPointer(),
					    false)) {
	Free(eigenvals);
	return false;
      }
      // Copy the eigenvalues into a diagonal (double) matrix
      D = Array(FM_DOUBLE,Vdims,eigenvals);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      if (!complexGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
					     (float*)A.getReadWriteDataPointer(),
					     (float*)B.getReadWriteDataPointer(),
					     false)) {
	Free(eigenvals);
	return false;
      }
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_FLOAT,Vdims,eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      if (!dcomplexGenEigenDecomposeSymmetric(N, NULL, eigenvals, 
					      (double*)A.getReadWriteDataPointer(),
					      (double*)B.getReadWriteDataPointer(),
					      false)) {
	Free(eigenvals);
	return false;
      }
      // Copy the eigenvalues into a diagonaal (complex) matrix
      D = Array(FM_DOUBLE,Vdims,eigenvals);
    }
    break;
  }
  return true;
}

/**
 * Eigen decomposition, symmetric matrix, full decomposition case
 */
bool GeneralizedEigenDecomposeFullSymmetric(Array A, Array B, Array& V, Array& D, Interpreter* m_eval) {
  int i;
  Class Aclass(A.dataClass());
  int N = A.getDimensionLength(0);

  // Create one square matrix to store the eigenvectors
  Dimensions Vdims(N,N);
  // Select the eigenvector decomposition routine based on A's type
  switch (Aclass) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      if (!floatGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
					   (float*)A.getReadWriteDataPointer(),
					   (float*)B.getReadWriteDataPointer(),
					   true)) {
	Free(eigenvals);
	Free(Vp);
	return false;
      }
      // Copy the eigenvalues into a diagonal (float) matrix
      D = Array(FM_FLOAT,Vdims,NULL);
      float *Dp = (float*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++)
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_FLOAT,Vdims,Vp);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      if (!doubleGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
					    (double*)A.getReadWriteDataPointer(),
					    (double*)B.getReadWriteDataPointer(),
					    true)) {
	Free(eigenvals);
	Free(Vp);	  
	return false;
      }
      // Copy the eigenvalues into a diagonal (double) matrix
      D = Array(FM_DOUBLE,Vdims,NULL);
      double *Dp = (double*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++)
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_DOUBLE,Vdims,Vp);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(N*sizeof(float));
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      if (!complexGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
					     (float*)A.getReadWriteDataPointer(),
					     (float*)B.getReadWriteDataPointer(),
					     true)) {
	Free(eigenvals);
	Free(Vp);
	return false;
      }
      // Copy the eigenvalues into a diagonal (real) matrix
      D = Array(FM_FLOAT,Vdims,NULL);
      float *Dp = (float*) Malloc(N*N*sizeof(float));
      for (i=0;i<N;i++) 
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_COMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(N*sizeof(double));
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      if (!dcomplexGenEigenDecomposeSymmetric(N, Vp, eigenvals, 
					      (double*)A.getReadWriteDataPointer(),
					      (double*)B.getReadWriteDataPointer(),
					      true)) {
	Free(eigenvals);
	Free(Vp);
	return false;
      }
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_DOUBLE,Vdims,NULL);
      double *Dp = (double*) Malloc(N*N*sizeof(double));
      for (i=0;i<N;i++) 
	Dp[i+N*i] = eigenvals[i];
      D.setDataPointer(Dp);
      V = Array(FM_DCOMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  }
  return true;
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvectors, and returns the eigenvalues in a diagonal matrix
 */
void GeneralizedEigenDecomposeFullGeneral(Array A, Array B, Array& V, Array& D, Interpreter* m_eval) {
  int i, j;
  Class Aclass(A.dataClass());
  // Create one square matrix to store the eigenvectors
  int N = A.getDimensionLength(0);  
  Dimensions Vdims(N,N);
  // Select the eigenvector decomposition routine based on A's type
  switch (Aclass) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      // For a real matrix, the eigenvectors are stored in a packed
      // format - complex eigenvectors are stored as two successive 
      // columns, corresponding to the real and imaginary parts of
      // the vector.  Successive columns can be used because the 
      // eigenvalues occur in conjugate pairs.
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      floatGenEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
			     (float*)B.getReadWriteDataPointer(), true);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a diagonal (float) matrix
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i+N*i] = eigenvals[2*i];
	D.setDataPointer(Dp);
	V = Array(FM_FLOAT,Vdims,Vp);
      } else {
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_COMPLEX,Vdims,NULL);
	float *Dp = (float*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_COMPLEX,Vdims,NULL);
	// We have one or more complex eigenvalues.  Allocate
	// space for a complex eigenvector matrix
	float *Vpc = (float*) Malloc(N*N*V.getElementSize());
	// Loop through the columns of Vpc
	i = 0;
	while (i<N) {
	  // Is this eigenvalue complex?
	  if ((i<N-1) && (eigenvals[2*i+1] != 0)) {
	    // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	    //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	    for (j=0;j<N;j++) {
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	      Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
	      Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
	      Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	    }
	    i += 2;
	  } else {
	    for (j=0;j<N;j++)
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	    i++;
	  }
	}
	Free(Vp);
	V.setDataPointer(Vpc);
      }
      Free(eigenvals);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      // For a real matrix, the eigenvectors are stored in a packed
      // format - complex eigenvectors are stored as two successive 
      // columns, corresponding to the real and imaginary parts of
      // the vector.  Successive columns can be used because the 
      // eigenvalues occur in conjugate pairs.
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      doubleGenEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
			      (double*)B.getReadWriteDataPointer(), true);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a diagonal (double) matrix
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) 
	  Dp[i+N*i] = eigenvals[2*i];
	D.setDataPointer(Dp);
	V = Array(FM_DOUBLE,Vdims,Vp);
      } else {
	// Copy the eigenvalues into a diagonal (complex) matrix
	D = Array(FM_DCOMPLEX,Vdims,NULL);
	double *Dp = (double*) Malloc(N*N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*(i+N*i)] = eigenvals[2*i];
	  Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
	V = Array(FM_DCOMPLEX,Vdims,NULL);
	// We have one or more complex eigenvalues.  Allocate
	// space for a complex eigenvector matrix
	double *Vpc = (double*) Malloc(N*N*V.getElementSize());
	// Loop through the columns of Vpc
	i = 0;
	while (i<N) {
	  // Is this eigenvalue complex?
	  if ((i<N-1) && (eigenvals[2*i+1] != 0)) {
	    // Yes - Vpc[:,i] = V[:,i] + I*V[:,i+1]
	    //       Vpc[:,i+1] = V[:,i] - I*V[:,i+1]
	    for (j=0;j<N;j++) {
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	      Vpc[2*(j+N*i)+1] = Vp[j+N*(i+1)];
	      Vpc[2*(j+N*(i+1))] = Vp[j+N*i];
	      Vpc[2*(j+N*(i+1))+1] = -Vp[j+N*(i+1)];
	    }
	    i += 2;
	  } else {
	    for (j=0;j<N;j++)
	      Vpc[2*(j+N*i)] = Vp[j+N*i];
	    i++;
	  }
	}
	Free(Vp);
	V.setDataPointer(Vpc);
      }
      Free(eigenvals);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      float *Vp = (float*) Malloc(N*N*A.getElementSize());
      complexGenEigenDecompose(N, Vp, eigenvals, (float*)A.getReadWriteDataPointer(),
			       (float*)B.getReadWriteDataPointer(), true);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_COMPLEX,Vdims,NULL);
      float *Dp = (float*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++) {
	Dp[2*(i+N*i)] = eigenvals[2*i];
	Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
      }
      D.setDataPointer(Dp);
      V = Array(FM_COMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      double *Vp = (double*) Malloc(N*N*A.getElementSize());
      dcomplexGenEigenDecompose(N, Vp, eigenvals, (double*)A.getReadWriteDataPointer(),
				(double*)B.getReadWriteDataPointer(), true);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_DCOMPLEX,Vdims,NULL);
      double *Dp = (double*) Malloc(N*N*D.getElementSize());
      for (i=0;i<N;i++) {
	Dp[2*(i+N*i)] = eigenvals[2*i];
	Dp[2*(i+N*i)+1] = eigenvals[2*i+1];
      }
      D.setDataPointer(Dp);
      V = Array(FM_DCOMPLEX,Vdims,Vp);
      Free(eigenvals);
    }
    break;
  }
}

/**
 * Perform an eigen decomposition of the matrix A - This version computes the 
 * eigenvalues only in a vector
 */
void GeneralizedEigenDecomposeCompactGeneral(Array A, Array B, Array& D, Interpreter* m_eval) {
  int i;
  Class Aclass(A.dataClass());
  int N = A.getDimensionLength(0);
  // Create one square matrix to store the eigenvectors
  Dimensions Vdims(N,1);
  // Select the eigenvector decomposition routine based on A's type
  switch (Aclass) {
  default: throw Exception("Unhandled type for eigendecomposition");
  case FM_FLOAT: 
    {
      // A temporary vector to store the eigenvalues
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      floatGenEigenDecompose(N, NULL, eigenvals, 
			     (float*)A.getReadWriteDataPointer(),
			     (float*)B.getReadWriteDataPointer(),
			     false);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a real (float) vector
	D = Array(FM_FLOAT,Vdims,NULL);
	float *Dp = (float*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i] = eigenvals[2*i];
	D.setDataPointer(Dp);
      } else {
	// Copy the eigenvalues into a complex vector
	D = Array(FM_COMPLEX,Vdims,NULL);
	float *Dp = (float*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*i] = eigenvals[2*i];
	  Dp[2*i+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
      }
      Free(eigenvals);
    }
    break;
  case FM_DOUBLE: 
    {
      // A temporary vector to store the eigenvalues
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      doubleGenEigenDecompose(N, NULL, eigenvals, 
			      (double*)A.getReadWriteDataPointer(),
			      (double*)B.getReadWriteDataPointer(),
			      false);
      // Make a pass through the eigenvals, and look for complex eigenvalues.
      bool complexEigenvalues = false;
      for (i=0;(i<N) && !complexEigenvalues;i++)
	complexEigenvalues = (eigenvals[2*i+1] != 0);
      if (!complexEigenvalues) {
	// Copy the eigenvalues into a real (double) vector
	D = Array(FM_DOUBLE,Vdims,NULL);
	double *Dp = (double*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++)
	  Dp[i] = eigenvals[2*i];
	D.setDataPointer(Dp);
      } else {
	// Copy the eigenvalues into a complex vector
	D = Array(FM_DCOMPLEX,Vdims,NULL);
	double *Dp = (double*) Malloc(N*D.getElementSize());
	for (i=0;i<N;i++) {
	  Dp[2*i] = eigenvals[2*i];
	  Dp[2*i+1] = eigenvals[2*i+1];
	}
	D.setDataPointer(Dp);
      }
      Free(eigenvals);
    }
    break;
  case FM_COMPLEX:
    {
      float *eigenvals = (float*) Malloc(2*N*sizeof(float));
      complexGenEigenDecompose(N, NULL, eigenvals, 
			       (float*)A.getReadWriteDataPointer(),
			       (float*)B.getReadWriteDataPointer(),
			       false);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_COMPLEX,Vdims,eigenvals);
    }
    break;
  case FM_DCOMPLEX:
    {
      double *eigenvals = (double*) Malloc(2*N*sizeof(double));
      dcomplexGenEigenDecompose(N, NULL, eigenvals, 
				(double*)A.getReadWriteDataPointer(),
				(double*)B.getReadWriteDataPointer(),
				false);
      // Copy the eigenvalues into a diagonal (complex) matrix
      D = Array(FM_DCOMPLEX,Vdims,eigenvals);
    }
    break;
  }
}

//!
//@Module HERMITIAN Matrix Hermitian (Conjugate Transpose) Operator
//@@Section OPERATORS
//@@Usage
//Computes the Hermitian of the argument (a 2D matrix).  The syntax for its use is
//@[
//  y = a';
//@]
//where @|a| is a @|M x N| numerical matrix.  The output @|y| is a numerical matrix
//of the same type of size @|N x M|.  This operator is the conjugating transpose,
//which is different from the transpose operator @|.'| (which does not 
//conjugate complex values).
//@@Function Internals
//The Hermitian operator is defined simply as
//\[
//  y_{i,j} = \overline{a_{j,i}}
//\]
//where @|y_ij| is the element in the @|i|th row and @|j|th column of the output matrix @|y|.
//@@Examples
//A simple transpose example:
//@<
//A = [1,2,0;4,1,-1]
//A'
//@>
//Here, we use a complex matrix to demonstrate how the Hermitian operator conjugates the entries.
//@<
//A = [1+i,2-i]
//A.'
//@>
//@@Tests
//@{ test_sparse76.m
//% Test sparse matrix array hermitian 
//function x = test_sparse76
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1',zi1') & testeq(yf1',zf1') & testeq(yd1',zd1') & testeq(yc1',zc1') & testeq(yz1',zz1');
//@}
//!
Array Transpose(Array A, Interpreter* m_eval) {
  A.hermitian();
  return A;
}

//!
//@Module TRANSPOSE Matrix Transpose Operator
//@@Section OPERATORS
//@@Usage
//Performs a transpose of the argument (a 2D matrix).  The syntax for its use is
//@[
//  y = a.';
//@]
//where @|a| is a @|M x N| numerical matrix.  The output @|y| is a numerical matrix
//of the same type of size @|N x M|.  This operator is the non-conjugating transpose,
//which is different from the Hermitian operator @|'| (which conjugates complex values).
//@@Function Internals
//The transpose operator is defined simply as
//\[
//  y_{i,j} = a_{j,i}
//\]
//where @|y_ij| is the element in the @|i|th row and @|j|th column of the output matrix @|y|.
//@@Examples
//A simple transpose example:
//@<
//A = [1,2,0;4,1,-1]
//A.'
//@>
//Here, we use a complex matrix to demonstrate how the transpose does \emph{not} conjugate the entries.
//@<
//A = [1+i,2-i]
//A.'
//@>
//@@Tests
//@{ test_sparse77.m
//% Test sparse matrix array transpose
//function x = test_sparse77
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1.',zi1.') & testeq(yf1.',zf1.') & testeq(yd1.',zd1.') & testeq(yc1.',zc1.') & testeq(yz1.',zz1.');
//@}
//!
Array DotTranspose(Array A, Interpreter* m_eval) {
  A.transpose();
  return A;
}


inline Array PowerScalarMatrix(Array A, Array B, Interpreter* m_eval) {
  // Do an eigendecomposition of B
  Array V, D;
  if (B.isSymmetric())
    EigenDecomposeFullSymmetric(B,V,D,m_eval);
  else
    EigenDecomposeFullGeneral(B,V,D,false,m_eval);
  // Get the diagonal part of D
  Array E = D.getDiagonal(0);
  // Call the vector version of the exponential
  Array F = DoPowerTwoArgFunction(A,E); // B, V, D, E, F
  // Construct a diagonal matrix from F
  Array G = Array::diagonalConstructor(F,0); // B, V, D, G, E, F
  // The output is (V*G)/V
  E = Multiply(V,G,m_eval); // B, V, D, E, F
  return RightDivide(E,V,m_eval); // B, D, F
}

inline Array PowerMatrixScalar(Array A, Array B, Interpreter* m_eval) {
  // Do an eigendecomposition of A
  Array V, D;
  if (A.isSymmetric())
    EigenDecomposeFullSymmetric(A,V,D,m_eval); //A, B, V, D
  else
    EigenDecomposeFullGeneral(A,V,D,false,m_eval);
  // Get the diagonal part of D
  Array E = D.getDiagonal(0); // A, B, V, D, E
  // Call the vector version of the exponential
  Array F = DoPowerTwoArgFunction(E,B); // F, A, V, D
  // Construct a diagonal matrix from F
  Array G = Array::diagonalConstructor(F,0); // G, A, V, D, F
  // The output is (V*G)/V
  E = Multiply(V,G,m_eval); // A, V, D, E, F
  return RightDivide(E,V,m_eval); // C, A, D, F
}

/**
 * Matrix-matrix power - These are the cases to consider:
 *   1. scalar-scalar defer to dotpower
 *   2. square-scalar - if an integer, iteratively multiply
 *          To raise A^N power, we do a successive factorization
 *          A -> A^2 -> A^4
 * 13 -> 8 + 4 + 2 + 1
 *  5 -> 4 + 2 + 1
 *  1 -> 1
 *
 * 23 -> 16 + 8 + 4 + 2 + 1
 *  7 -> 4 + 2 + 1
 *  3 -> 2 + 1
 *  1 -> 1
 *
 * 
 *   3. scalar-square - Decompose 
 */
//!
//@Module POWER Matrix Power Operator
//@@Section OPERATORS
//@@Usage
//The power operator for scalars and square matrices.  This operator is really a 
//combination of two operators, both of which have the same general syntax:
//@[
//  y = a ^ b
//@]
//The exact action taken by this operator, and the size and type of the output, 
//depends on which of the two configurations of @|a| and @|b| is present:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is a square matrix
//  \item @|a| is a square matrix, @|b| is a scalar
//\end{enumerate}
//@@Function Internals
//In the first case that @|a| is a scalar, and @|b| is a square matrix, the matrix power is defined in terms of the eigenvalue decomposition of @|b|.  Let @|b| have the following eigen-decomposition (problems arise with non-symmetric matrices @|b|, so let us assume that @|b| is symmetric):
//\[
//  b = E \begin{bmatrix} \lambda_1 & 0          & \cdots  & 0 \\                            0   & \lambda_2  &  \ddots & \vdots \\                                         \vdots & \ddots & \ddots & 0 \\                                                 0   & \hdots & 0 & \lambda_n \end{bmatrix}
//      E^{-1}
//\]
//Then @|a| raised to the power @|b| is defined as
//\[
//  a^{b} = E \begin{bmatrix} a^{\lambda_1} & 0          & \cdots  & 0 \\                                0   & a^{\lambda_2}  &  \ddots & \vdots \\                                    \vdots & \ddots & \ddots & 0 \\                                             0   & \hdots & 0 & a^{\lambda_n} \end{bmatrix}
//      E^{-1}
//\]
//Similarly, if @|a| is a square matrix, then @|a| has the following eigen-decomposition (again, suppose @|a| is symmetric):
//\[
//  a = E \begin{bmatrix} \lambda_1 & 0          & \cdots  & 0 \\                                0   & \lambda_2  &  \ddots & \vdots \\                                         \vdots & \ddots & \ddots & 0 \\                                          0   & \hdots & 0 & \lambda_n \end{bmatrix}
//      E^{-1}
//\]
//Then @|a| raised to the power @|b| is defined as
//\[
//  a^{b} = E \begin{bmatrix} \lambda_1^b & 0          & \cdots  & 0 \\                              0   & \lambda_2^b  &  \ddots & \vdots \\                              \vdots & \ddots & \ddots & 0 \\                              0   & \hdots & 0 & \lambda_n^b \end{bmatrix}
//      E^{-1}
//\]
//@@Examples
//We first define a simple @|2 x 2| symmetric matrix
//@<
//A = 1.5
//B = [1,.2;.2,1]
//@>
//First, we raise @|B| to the (scalar power) @|A|:
//@<
//C = B^A
//@>
//Next, we raise @|A| to the matrix power @|B|:
//@<
//C = A^B
//@>
//!

Array Power(Array A, Array B, Interpreter* m_eval){

  if (A.isEmpty() || B.isEmpty())
    return Array::emptyConstructor();

  // Special case -- kludge to fix bug 1804267
  B = ScreenIntegerScalars(B);

  if (A.isScalar() && B.isScalar()) return DotPower(A,B,m_eval);

  // Check for A & B numeric
  CheckNumeric(A,B,"^");

  // Test for 2D on both A & B
  if (!A.is2D() || !B.is2D())
    throw Exception("Cannot apply exponential operation to N-Dimensional arrays.");

  if (B.isReal() && B.isScalar()) {
    Array C(B);
    C.promoteType(FM_DOUBLE);
    const double*dp = (const double*) C.getDataPointer();
    if (*dp == rint(*dp) && (*dp == -1))
      return InvertMatrix(A,m_eval);
  }

  // Test the types
  TypeCheck(A,B,true);

  // Both arguments must be square
  if ((A.getDimensionLength(0) != A.getDimensionLength(1)) ||
      (B.getDimensionLength(0) != B.getDimensionLength(1)))
    throw Exception("Power (^) operator can only be applied to scalar and square arguments.");

  if (A.sparse() || B.sparse())
    return MatrixPowerSparse(A,B,m_eval);

  // OK - check for A a scalar - if so, do a decomposition of B
  if (A.isScalar())
    return PowerScalarMatrix(A,B,m_eval);
  else if (B.isScalar())
    return PowerMatrixScalar(A,B,m_eval);
  else 
    throw Exception("One of the arguments to (^) must be a scalar.");
}

Array UnitColon(Array A, Array B) {
  Array C;
  if (!A.isScalar() || !B.isScalar())
    throw Exception("Both arguments to (:) operator must be scalars.");
  if (A.isComplex() || B.isComplex())
    throw Exception("Both arguments to (:) operator must be real.");
  // Make sure A and B are the same type - at least INT32
  Class Aclass, Bclass, Cclass;
  Aclass = A.dataClass();
  Bclass = B.dataClass();
  Cclass = (Aclass > Bclass) ? Aclass : Bclass;
  Cclass = (FM_INT32 > Cclass) ? FM_INT32 : Cclass;
  A.promoteType(Cclass);
  B.promoteType(Cclass);
  switch (Cclass) {
  default: throw Exception("Unhandled type for A:B");
  case FM_INT32:
    C = Array::int32RangeConstructor(*((int32*)A.getDataPointer()),
				     1,
				     *((int32*)B.getDataPointer()),
				     false);
    break;
  case FM_INT64:
    C = Array::int64RangeConstructor(*((int64*)A.getDataPointer()),
				     1,
				     *((int64*)B.getDataPointer()),
				     false);
    break;
  case FM_FLOAT: 
    C = Array::floatRangeConstructor(*((float*)A.getDataPointer()),
				     1,
				     *((float*)B.getDataPointer()),
				     false);
    break;
  case FM_DOUBLE:
    C = Array::doubleRangeConstructor(*((double*)A.getDataPointer()),
				      1,
				      *((double*)B.getDataPointer()),
				      false);
    break;
  }
  return C;
}

Array DoubleColon(Array A, Array B, Array C){
  Array D;
  if (!A.isScalar() || !B.isScalar() || !C.isScalar())
    throw Exception("All three arguments to (:) operator must be scalars.");
  if (A.isComplex() || B.isComplex() || C.isComplex())
    throw Exception("All arguments to (:) operator must be real.");
  // Make sure A and B are the same type - at least INT32
  Class Aclass, Bclass, Cclass, Dclass;
  Aclass = A.dataClass();
  Bclass = B.dataClass();
  Cclass = C.dataClass();
  Dclass = (Aclass > Bclass) ? Aclass : Bclass;
  Dclass = (Dclass > Cclass) ? Dclass : Cclass;
  Dclass = (FM_INT32 > Dclass) ? FM_INT32 : Dclass;
  A.promoteType(Dclass);
  B.promoteType(Dclass);
  C.promoteType(Dclass);
  switch (Dclass) {
  default: throw Exception("Unhandled type for A:B:C");
  case FM_INT32:
    D = Array::int32RangeConstructor(*((int32*)A.getDataPointer()),
				     *((int32*)B.getDataPointer()),
				     *((int32*)C.getDataPointer()),
				     false);
    break;
  case FM_INT64:
    D = Array::int64RangeConstructor(*((int64*)A.getDataPointer()),
				     *((int64*)B.getDataPointer()),
				     *((int64*)C.getDataPointer()),
				     false);
    break;
  case FM_FLOAT: 
    D = Array::floatRangeConstructor(*((float*)A.getDataPointer()),
				     *((float*)B.getDataPointer()),
				     *((float*)C.getDataPointer()),
				     false);
    break;
  case FM_DOUBLE:
    D =  Array::doubleRangeConstructor(*((double*)A.getDataPointer()),
				       *((double*)B.getDataPointer()),
				       *((double*)C.getDataPointer()),
				       false);
    break;
  }
  return D;
}
