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

#ifndef __LinearEqSolver_hpp__
#define __LinearEqSolver_hpp__

#include "Interface.hpp"

/**
 * Matrix-matrix divides (i.e., equation solvers).
 */

namespace FreeMat {
  /**
   * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
   * $$m \times n$$.  All quantities are double precision.  Uses the
   * LAPACK function dgesvx.
   */
  void doubleSolveLinEq(Interface* io, const int m, const int n, double *c, double* a, double *b);
  /**
   * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
   * $$m \times n$$.  All quantities are double-complex precision.  Uses the
   * LAPACK function zgesvx.
   */
  void dcomplexSolveLinEq(Interface* io, const int m, const int n, double *c, double* a, double *b);
  /**
   * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
   * $$m \times n$$.  All quantities are single precision.  Uses the
   * LAPACK function sgesvx.
   */
  void floatSolveLinEq(Interface* io, const int m, const int n, float *c, float* a, float *b);
  /**
   * Solve $$A C = B$$, where $$A$$ is $$m \times m$$, and $$B$$ is
   * $$m \times n$$.  All quantities are complex precision.  Uses the
   * LAPACK function cgesvx.
   */
  void complexSolveLinEq(Interface* io, const int m, const int n, float *c, float* a, float *b);
}

#endif
