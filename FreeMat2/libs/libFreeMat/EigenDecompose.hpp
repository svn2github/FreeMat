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
