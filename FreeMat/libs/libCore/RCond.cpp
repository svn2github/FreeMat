/*
 * Copyright (c) 2009 Samit Basu
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

#include "LAPACK.hpp"
#include "Array.hpp"
#include "MemPtr.hpp"
#include "Algorithms.hpp"

template <class T>
static bool isFiniteNonzero(T a) {
  return ((a != 0) && IsFinite(a));
}

static float complexRecipCond(int m, int n, float *a) {
  if ((m == 1) && (n == 1) && (isFiniteNonzero(a[0]) ||
			       isFiniteNonzero(a[1]))) return 1;
  // Getting the estimated reciprocal condition number involves
  // three steps:
  //    1. Compute the 1-norm of a
  float Anorm = 0;
  float rcond = 0;
  {
    char NORM = '1';
    Anorm = clange_(&NORM,&m,&n,a,&m,NULL);
  }
  //    2. Compute the LU-decomposition of a
  {
    MemBlock<int> ipiv(std::min(m,n));
    int info;
    cgetrf_(&m,&n,a,&m,&ipiv,&info);
  }
  //    3. Call sgecon to compute rcond
  {
    char NORM = '1';
    int info;
    MemBlock<float> work(4*n);
    MemBlock<float> rwork(2*n);
    cgecon_(&NORM,&n,a,&m,&Anorm,&rcond,&work,&rwork,&info);
  }
  return rcond;
}

static float floatRecipCond(int m, int n, float *a) {
  if ((m == 1) && (n == 1) && isFiniteNonzero(a[0])) return 1;
  // Getting the estimated reciprocal condition number involves
  // three steps:
  //    1. Compute the 1-norm of a
  float Anorm = 0;
  float rcond = 0;
  {
    char NORM = '1';
    Anorm = slange_(&NORM,&m,&n,a,&m,NULL);
  }
  //    2. Compute the LU-decomposition of a
  {
    MemBlock<int> ipiv(std::min(m,n));
    int info;
    sgetrf_(&m,&n,a,&m,&ipiv,&info);
  }
  //    3. Call sgecon to compute rcond
  {
    char NORM = '1';
    int info;
    MemBlock<float> work(4*n);
    MemBlock<int> iwork(n);
    sgecon_(&NORM,&n,a,&m,&Anorm,&rcond,&work,&iwork,&info);
  }
  return rcond;
}

static double dcomplexRecipCond(int m, int n, double *a) {
  if ((m == 1) && (n == 1) && (isFiniteNonzero(a[0]) || isFiniteNonzero(a[1])))
    return 1;
  // Getting the estimated reciprocal condition number involves
  // three steps:
  //    1. Compute the 1-norm of a
  double Anorm = 0;
  double rcond = 0;
  {
    char NORM = '1';
    Anorm = zlange_(&NORM,&m,&n,a,&m,NULL);
  }
  //    2. Compute the LU-decomposition of a
  {
    MemBlock<int> ipiv(std::min(m,n));
    int info;
    zgetrf_(&m,&n,a,&m,&ipiv,&info);
  }
  //    3. Call sgecon to compute rcond
  {
    char NORM = '1';
    int info;
    MemBlock<double> work(4*n);
    MemBlock<double> rwork(2*n);
    zgecon_(&NORM,&n,a,&m,&Anorm,&rcond,&work,&rwork,&info);
  }
  return rcond;
}

static double doubleRecipCond(int m, int n, double *a) {
  if ((m == 1) && (n == 1) && isFiniteNonzero(a[0])) return 1;
  // Getting the estimated reciprocal condition number involves
  // three steps:
  //    1. Compute the 1-norm of a
  double Anorm = 0;
  double rcond = 0;
  {
    char NORM = '1';
    Anorm = dlange_(&NORM,&m,&n,a,&m,NULL);
  }
  //    2. Compute the LU-decomposition of a
  {
    MemBlock<int> ipiv(std::min(m,n));
    int info;
    dgetrf_(&m,&n,a,&m,&ipiv,&info);
  }
  //    3. Call sgecon to compute rcond
  {
    char NORM = '1';
    int info;
    MemBlock<double> work(4*n);
    MemBlock<int> iwork(n);
    dgecon_(&NORM,&n,a,&m,&Anorm,&rcond,&work,&iwork,&info);
  }
  return rcond;
}

//!
//@Module RCOND Reciprocal Condition Number Estimate
//@@Section ARRAY
//@@Usage
//The @|rcond| function is a FreeMat wrapper around LAPACKs
//function @|XGECON|, which estimates the 1-norm condition
//number (reciprocal).  For the details of the algorithm see
//the LAPACK documentation.  The syntax for its use is
//@[
//   x = rcond(A)
//@]
//where @|A| is a matrix.
//@@Example
//Here is the reciprocal condition number for a random square
//matrix
//@<
//A = rand(30);
//rcond(A)
//@>
//And here we calculate the same value using the definition of
//(reciprocal) condition number
//@<
//1/(norm(A,1)*norm(inv(A),1))
//@>
//Note that the values are very similar.  LAPACKs @|rcond|
//function is far more efficient than the explicit calculation
//(which is also used by the @|cond| function.
//@@Tests
//@$near#y1=rcond(x1)
//@@Signature
//function rcond RcondFunction jitsafe
//inputs A
//outputs x
//!
ArrayVector RcondFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("rcond function requires at least one argument - the matrix to compute the condition number for.");
  Array A(arg[0].asDenseArray());
  if (!A.is2D()) throw Exception("Cannot apply matrix operations to N-dimensional arrays.");
  if (!A.isSquare()) throw Exception("rcond only applies to square matrices");
  if (A.isEmpty())
    return ArrayVector(Array(Inf()));
  if (AnyNaN(A))
    return ArrayVector(Array(NaN()));
  switch (A.dataClass()) {
  default: throw Exception("Unsupported type for rcond operation");
  case Float:
    if (A.allReal())
      return ArrayVector(Array(floatRecipCond(int(A.rows()),
					      int(A.cols()),
					      A.real<float>().data())));
    else
      return ArrayVector(Array(complexRecipCond(int(A.rows()),
						int(A.cols()),
						A.fortran<float>().data())));
  case Double:
    if (A.allReal())
      return ArrayVector(Array(doubleRecipCond(int(A.rows()),
					       int(A.cols()),
					       A.real<double>().data())));
    else
      return ArrayVector(Array(dcomplexRecipCond(int(A.rows()),
						 int(A.cols()),
						 A.fortran<double>().data())));
  }
}
