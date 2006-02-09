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

#ifndef __MatrixMultiply_hpp__
#define __MatrixMultiply_hpp__

/**
 * Matrix-matrix multiplies.
 */

namespace FreeMat {
  /**
   * Compute $$C = A B$$, where $$A$$ is $$m \times n$$ and $$B$$ is
   * $$n \times k$$, and all three are single precision.  Uses the 
   * BLAS function sgemm.
   */
  void floatMatrixMatrixMultiply(int m, int n, int k,
				 float* c, const float* a, const float *b);
  /**
   * Compute $$C = A B$$, where $$A$$ is $$m \times n$$ and $$B$$ is
   * $$n \times k$$, and all three are complex precision.  Uses the 
   * BLAS function cgemm.
   */
  void complexMatrixMatrixMultiply(int m, int n, int k,
				   float* c, const float* a, const float *b);
  /**
   * Compute $$C = A B$$, where $$A$$ is $$m \times n$$ and $$B$$ is
   * $$n \times k$$, and all three are double precision.  Uses the 
   * BLAS function dgemm.
   */
  void doubleMatrixMatrixMultiply(int m, int n, int k,
				  double* c, const double* a, const double *b);
  /**
   * Compute $$C = A B$$, where $$A$$ is $$m \times n$$ and $$B$$ is
   * $$n \times k$$, and all three are double complex precision.  Uses the 
   * BLAS function zgemm.
   */
  void dcomplexMatrixMatrixMultiply(int m, int n, int k,
				    double* c, const double* a, const double *b);
}
#endif
