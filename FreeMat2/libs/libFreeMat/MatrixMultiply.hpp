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
  void floatMatrixMatrixMultiply(const int m, const int n, const int k,
				 float* c, const float* a, const float *b);
  /**
   * Compute $$C = A B$$, where $$A$$ is $$m \times n$$ and $$B$$ is
   * $$n \times k$$, and all three are complex precision.  Uses the 
   * BLAS function cgemm.
   */
  void complexMatrixMatrixMultiply(const int m, const int n, const int k,
				   float* c, const float* a, const float *b);
  /**
   * Compute $$C = A B$$, where $$A$$ is $$m \times n$$ and $$B$$ is
   * $$n \times k$$, and all three are double precision.  Uses the 
   * BLAS function dgemm.
   */
  void doubleMatrixMatrixMultiply(const int m, const int n, const int k,
				  double* c, const double* a, const double *b);
  /**
   * Compute $$C = A B$$, where $$A$$ is $$m \times n$$ and $$B$$ is
   * $$n \times k$$, and all three are double complex precision.  Uses the 
   * BLAS function zgemm.
   */
  void dcomplexMatrixMatrixMultiply(const int m, const int n, const int k,
				    double* c, const double* a, const double *b);
}
#endif
