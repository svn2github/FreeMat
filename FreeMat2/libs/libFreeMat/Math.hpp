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

#ifndef __Math_hpp_
#define __Math_hpp_

#include "Array.hpp"

namespace FreeMat {
  /**
   * Add the two argument arrays together: $$C_n = A_n + B_n$$.
   */
  Array Add(Array A, Array B);
  /**
   * Subtract the second array from the first: $$C_n = A_n - B_n$$.
   */
  Array Subtract(Array A, Array B);
  /**
   * Element-wise multiply of two arrays: $$C_n = A_n B_n$$.
   */
  Array DotMultiply(Array A, Array B);
  /**
   * Element-wise divide of two arrays: $$C_n = \frac{A_n}{B_n}$$.
   */
  Array DotRightDivide(Array A, Array B);
  /**
   * Element-wise divide of two arrays: $$C_n = \frac{B_n}{A_n}$$.
   */
  Array DotLeftDivide(Array A, Array B);
  /**
   * Element-wise compare (lt) of two arrays: $$C_n = A_n < B_n$$.
   */ 
  Array LessThan(Array A, Array B);
  /**
   * Element-wise compare (le) of two arrays: $$C_n = A_n \leq B_n$$.
   */   
  Array LessEquals(Array A, Array B);
  /**
   * Element-wise compare (gt) of two arrays: $$C_n = A_n > B_n$$.
   */ 
  Array GreaterThan(Array A, Array B);
  /**
   * Element-wise compare (ge) of two arrays: $$C_n = A_n \geq B_n$$.
   */ 
  Array GreaterEquals(Array A, Array B);
  /**
   * Element-wise compare (eq) of two arrays: $$C_n = A_n == B_n$$.
   */ 
  Array Equals(Array A, Array B);
  /**
   * Element-wise compare (ne) of two arrays: $$C_n = A_n \neq B_n$$.
   */ 
  Array NotEquals(Array A, Array B);
  /**
   * Element-wise or of two arrays: $$C_n = A_n \or B_n$$.
   */
  Array Or(Array A, Array B);
  /**
   * Element-wise and of two arrays: $$C_n = A_n \and B_n$$.
   */
  Array And(Array A, Array B);
  /**
   * Element-wise not of argument: $$C_n = \not A_n$$.
   */
  Array Not(Array A);
  /**
   * Element-wise plus of argument: $$C_n = + A_n$$.
   */
  Array Plus(Array A);
  /**
   * Element-wise negate of argument: $$C_n = - A_n$$.
   */
  Array Negate(Array A) throw(Exception);
  /**
   * Element-wise power: $$C_n = A_n ^ {B_n}$$.
   */
  Array DotPower(Array A, Array B);
  /**
   * Matrix to matrix power.  The calculation performed
   * depends on the sizes of the arguments.
   *   - both are scalars, $$C = A^B$$.
   *   - $$A$$ is a square matrix, $$B$$ is a scalar,
   *     $$C = E V^{B} E^{-1}$$, where $$V$$ is the diagonal
   *     matrix of eigenvalues of $$A$$, and $$E$$ is the
   *     associated matrix of eigenvectors.  
   *   - $$A$$ is a scalar, $$B$$ is a matrix,
   *     $$C = E A^{V} E^{-1}$$, where $$V$$ is the diagonal
   *     matrix of eigenvalues of $$B$$, and $$E$$ is the 
   *     associated matrix of eigenvectors.  The quantity
   *     $$A^{V}$$ is evaluated along the diagonal.
   * Throws an exception if 
   *   - both arguments are matrices
   *   - either of the arguments is more than 2-dimensional
   *   - any of the arguments are rectangular.
   */
  Array Power(Array A, Array B) throw(Exception);
  /**
   * Transposes the argument (actually does a Hermitian transpose).
   * The output is $$C_{i,j} = \conj{A_{j,i}}$$.
   */
  Array Transpose(Array A);
  /**
   * Dot-transpose the argument, equivalent to $$C_{i,j} = A_{j,i}$$.
   */
  Array DotTranspose(Array);
  /**
   * Matrix multiply of the arguments.  For $$m \times n$$ matrix $$A$$,
   * and $$n \times k$$ matrix $$B$$, the output matrix $$C$$ is of 
   * size $$m \times k$$, defined by  $$C_{i,j} = \sum_{p=1}^{n} A_{i,p} B_{p,j}$$.
   * Throws an exception if the sizes are not conformant.
   */
  Array Multiply(Array A, Array B) throw(Exception);
  /**
   * The right divide operation is related to the left divide operation
   * via: B/A = (A'\B')'.
   */
  Array RightDivide(Array A, Array B);
  /**
   * The left divide operation is equivalent to solving the system of equations
   * $$A C = B$$ for the matrix $$C$$, where $$A$$ is of size $$m \times n$$,
   * $$B$$ is of size $$m \times k$$, and the output $$C$$ is of size $$n \times k$$.
   * Uses the linear equation solver from LAPACK to solve these equations.
   * They are effectively solved independently for each column of $$B$$.
   */
  Array LeftDivide(Array A, Array B) throw(Exception);
  /**
   * Compute the eigendecomposition of the matrix $$A$$, the two matrices
   * $$V$$ and $$D$$, where $$D$$ is diagonal, and $$V$$ has unit norm
   * columns.  If $$A$$ is real, the eigenvectors $$V$$ are real, and 
   * the eigenvalues come in conjugate pairs.  
   */
  void EigenDecomposeCompactSymmetric(Array A, Array& D);
  void EigenDecomposeFullSymmetric(Array A, Array& V, Array& D);
  void EigenDecomposeFullGeneral(Array A, Array& V, Array& D, bool balanceFlag);
  void EigenDecomposeCompactGeneral(Array A, Array& D, bool balanceFlag);
  bool GeneralizedEigenDecomposeCompactSymmetric(Array A, Array B, Array& D);
  bool GeneralizedEigenDecomposeFullSymmetric(Array A, Array B, Array& V, Array& D);
  void GeneralizedEigenDecomposeFullGeneral(Array A, Array B, Array& V, Array& D);
  void GeneralizedEigenDecomposeCompactGeneral(Array A, Array B, Array& D);
  /**
   * For scalars $$A$$ and $$B$$, the output is the row vector
   * $$[A,A+1,\ldots,A+n]$$, where $$n$$ is the largest integer
   * such that $$A+n < B$$.
   */
  Array UnitColon(Array A, Array B) throw(Exception);
  /**
   * For scalars $$A$$, $$B$$ and $$C$$, the output is the row vector
   * $$[A,A+B,\ldots,A+nB]$$, where $$n$$ is the largest integer
   * such that $$A+nB < C$$.
   */
  Array DoubleColon(Array A, Array B, Array C) throw(Exception);

  void TypeCheck(Array &A, Array &B, bool isDivOrMatrix);

  Array InvertMatrix(Array A);

  void power_zi(double *p, const double *a, int b);
  void power_zz(double *c, const double *a, const double *b);
  double power_di(double a, int b);
  double power_dd(double a, double b);
  template <class T>
  T complex_abs(T real, T imag);
#ifdef WIN32
  double rint(double);
#endif
}
#endif
