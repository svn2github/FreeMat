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

#ifndef __EigenDecompose_hpp__
#define __EigenDecompose_hpp__

namespace FreeMat {

  /**
   * Compute the eigen decomposition of an arbitrary, real valued
   * matrix of size $$n \times n$$.  The $$n \times n$$ matrix of
   * real eigenvectors are stored in the array v, which must be 
   * pre-allocated before the routine is called.  The $$n$$ complex 
   * eigenvalues are stored in the array d, with the real and 
   * imaginary parts stored sequentially.
   */
  void floatEigenDecompose(int n, float *v, float *d, float *a,
			   bool eigenvectors, bool balance);
  void floatEigenDecomposeSymmetric(int n, float *v, float *d, float *a,
				    bool eigenvectors);
  void floatGenEigenDecompose(int n, float *v, float *d, float *a,
			      float *b, bool eigenvectors);
  bool floatGenEigenDecomposeSymmetric(int n, float *v, float *d,
				       float *a, float *b, bool eigenvectors);
  /**
   * Compute the eigen decomposition of an arbitrary, complex valued
   * matrix of size $$n \times n$$.  The $$n \times n$$ matrix of
   * complex eigenvectors are stored in the array v, which must be 
   * pre-allocated before the routine is called.  The $$n$$ complex 
   * eigenvalues are stored in the array d.
   */
  void complexEigenDecompose(int n, float *v, float *d, float *a,
			     bool eigenvectors, bool balance);
  void complexEigenDecomposeSymmetric(int n, float *v, float *d, 
				      float *a, bool eigenvectors);
  void complexGenEigenDecompose(int n, float *v, float *d, float *a,
			      float *b, bool eigenvectors);
  bool complexGenEigenDecomposeSymmetric(int n, float *v, float *d,
					 float *a, float *b, 
					 bool eigenvectors);
  /**
   * Compute the eigen decomposition of an arbitrary, double valued
   * matrix of size $$n \times n$$.  The $$n \times n$$ matrix of
   * double eigenvectors are stored in the array v, which must be 
   * pre-allocated before the routine is called.  The $$n$$ dcomplex 
   * eigenvalues are stored in the array d, with the real and 
   * imaginary parts stored sequentially.
   */
  void doubleEigenDecompose(int n, double *v, double *d, double *a,
			    bool eigenvectors, bool balance);
  void doubleEigenDecomposeSymmetric(int n, double *v, double *d, 
				     double *a, bool eigenvectors);
  void doubleGenEigenDecompose(int n, double *v, double *d, double *a,
			      double *b, bool eigenvectors);
  bool doubleGenEigenDecomposeSymmetric(int n, double *v, double *d,
				       double *a, double *b, 
					bool eigenvectors);
  /**
   * Compute the eigen decomposition of an arbitrary, dcomplex valued
   * matrix of size $$n \times n$$.  The $$n \times n$$ matrix of
   * dcomplex eigenvectors are stored in the array v, which must be 
   * pre-allocated before the routine is called.  The $$n$$ dcomplex 
   * eigenvalues are stored in the array d.
   */
  void dcomplexEigenDecompose(int n, double *v, double *d, double *a,
			   bool eigenvectors, bool balance);
  void dcomplexEigenDecomposeSymmetric(int n, double *v, double *d, 
				       double *a, bool eigenvectors);
  void dcomplexGenEigenDecompose(int n, double *v, double *d, double *a,
				 double *b, bool eigenvectors);
  bool dcomplexGenEigenDecomposeSymmetric(int n, double *v, double *d,
					  double *a, double *b, 
					  bool eigenvectors);
}

#endif
