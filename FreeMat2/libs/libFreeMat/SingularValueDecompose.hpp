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

#ifndef __SingularValueDecompose_hpp__
#define __SingularValueDecompose_hpp__

namespace FreeMat {
  void floatSVD(const int nrows, const int ncols, float *u, float *vt, 
		float *s, float *a, bool compact, bool vectors);
  void doubleSVD(const int nrows, const int ncols, double *u, double *vt, 
		 double *s, double *a, bool compact, bool vectors);
  void complexSVD(const int nrows, const int ncols, float *u, float *vt, 
		  float *s, float *a, bool compact, bool vectors);
  void dcomplexSVD(const int nrows, const int ncols, double *u, double *vt, 
		   double *s, double *a, bool compact, bool vectors);
}

#endif
