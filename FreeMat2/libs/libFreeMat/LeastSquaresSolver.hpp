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

#ifndef __LeastSquaresSolver_hpp__
#define __LeastSquaresSolver_hpp__

#include "Interface.hpp"

namespace FreeMat {
  /**
   * Solve $$A  X = B$$ in a least-squares sense, where $$A$$ is $$m \times n$$, 
   * and $$B$$ is $$m \times k$$. $$C$$ is $$n \times k$$, and all of the terms 
   * are double precision.  Uses the LAPACK routine dgelsy.
   */
  void doubleSolveLeastSq(Interface* io, const int m, const int n, const int k, double *c,
			  double *a, double *b);
  /**
   * Solve $$A  X = B$$ in a least-squares sense, where $$A$$ is $$m \times n$$, 
   * and $$B$$ is $$m \times k$$. $$C$$ is $$n \times k$$, and all of the terms 
   * are dcomplex precision.  Uses the LAPACK routine zgelsy.
   */
  void dcomplexSolveLeastSq(Interface* io,const int m, const int n, const int k, double *c,
			    double *a, double *b);
  /**
   * Solve $$A  X = B$$ in a least-squares sense, where $$A$$ is $$m \times n$$, 
   * and $$B$$ is $$m \times k$$. $$C$$ is $$n \times k$$, and all of the terms 
   * are float precision.  Uses the LAPACK routine sgelsy.
   */
  void floatSolveLeastSq(Interface* io,const int m, const int n, const int k, float *c,
			 float *a, float *b);
  /**
   * Solve $$A  X = B$$ in a least-squares sense, where $$A$$ is $$m \times n$$, 
   * and $$B$$ is $$m \times k$$. $$C$$ is $$n \times k$$, and all of the terms 
   * are complex precision.  Uses the LAPACK routine cgelsy.
   */
  void complexSolveLeastSq(Interface* io,const int m, const int n, const int k, float *c,
			   float *a, float *b);
}
#endif
