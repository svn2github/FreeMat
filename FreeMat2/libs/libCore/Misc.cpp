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

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif
#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Math.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "Command.hpp"
#include "SingularValueDecompose.hpp"
#include "QRDecompose.hpp"
#include "System.hpp"

namespace FreeMat {
  //!
  //@Module DISP Display a Variable or Expression
  //@@Usage
  //Displays the result of a set of expressions.  The @|disp| function
  //takes a variable number of arguments, each of which is an expression
  //to output:
  //@[
  //  disp(expr1,expr2,...,exprn)
  //@]
  //This is functionally equivalent to evaluating each of the expressions
  //without a semicolon after each.
  //@@Example
  //Here are some simple examples of using @|disp|.
  //@<
  //a = 32;
  //b = 1:4;
  //disp(a,b,pi)
  //@>
  //!
  ArrayVector DispFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    int length;
    Array C;
    ArrayVector retval;

    length = arg.size();
    for (int i=0;i<length;i++) {
      C = arg[i];
      C.printMe(eval->getPrintLimit());
    }
    retval.push_back(C);
    return retval;
  } 

  ArrayVector GetLineFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    char *prompt, *text;
    if (arg.size() < 1)
      prompt = "";
    else {
      Array A(arg[0]);
      if (!A.isString())
	throw Exception("getline requires a string prompt");
      prompt = A.getContentsAsCString();
    }
    text = eval->getInterface()->getLine(prompt);
    return singleArrayVector(Array::stringConstructor(text));
  }

  ArrayVector GenEigFunction(int nargout, const ArrayVector &arg) {
    Array A(arg[0]);
    Array B(arg[1]);
    if (!A.is2D() || !B.is2D())
      throw Exception("cannot apply matrix operations to N-dimensional arrays.");
    if (A.anyNotFinite() || B.anyNotFinite())
      throw Exception("eig only defined for matrices with finite entries.");

    ArrayVector retval;
    Array V, D;

    if (nargout > 1) {
      if (A.isSymmetric() && B.isSymmetric()) {
	if (!GeneralizedEigenDecomposeFullSymmetric(A,B,V,D))
	  GeneralizedEigenDecomposeFullGeneral(A,B,V,D);
      } else
	GeneralizedEigenDecomposeFullGeneral(A,B,V,D);
      retval.push_back(V);
      retval.push_back(D);
    } else {
      if (A.isSymmetric() && B.isSymmetric()) {
	if (!GeneralizedEigenDecomposeCompactSymmetric(A,B,D))
	  GeneralizedEigenDecomposeCompactGeneral(A,B,D);
      } else
	GeneralizedEigenDecomposeCompactGeneral(A,B,D);
      retval.push_back(D);
    }
    return retval;
  }
  //!
  //@Module EIG Eigendecomposition of a Matrix
  //@@Usage
  //Computes the eigendecomposition of a square matrix.  The @|eig| function
  //has several forms.  The first returns only the eigenvalues of the matrix:
  //@[
  //  s = eig(A)
  //@]
  //The second form returns both the eigenvectors and eigenvalues as two 
  //matrices (the eigenvalues are stored in a diagonal matrix):
  //@[
  //  [V,D] = eig(A)
  //@]
  //where @|D| is the diagonal matrix of eigenvalues, and @|V| is the
  //matrix of eigenvectors.
  //
  //Eigenvalues and eigenvectors for asymmetric matrices @|A| normally
  //are computed with balancing applied.  Balancing is a scaling step
  //that normaly improves the quality of the eigenvalues and eigenvectors.
  //In some instances (see the Function Internals section for more details)
  //it is necessary to disable balancing.  For these cases, two additional
  //forms of @|eig| are available:
  //@[
  //  s = eig(A,'nobalance'),
  //@]
  //which computes the eigenvalues of @|A| only, and does not balance
  //the matrix prior to computation.  Similarly,
  //@[
  //  [V,D] = eig(A,'nobalance')
  //@]
  //recovers both the eigenvectors and eigenvalues of @|A| without balancing.
  //Note that the 'nobalance' option has no affect on symmetric matrices.
  //
  //FreeMat also provides the ability to calculate generalized eigenvalues
  //and eigenvectors.  Similarly to the regular case, there are two forms
  //for @|eig| when computing generalized eigenvector (see the Function
  //Internals section for a description of what a generalized eigenvector is).
  //The first returns only the generalized eigenvalues of the matrix
  //pair @|A,B|
  //@[
  //  s = eig(A,B)
  //@]
  //The second form also computes the generalized eigenvectors, and is
  //accessible via
  //@[
  //  [V,D] = eig(A,B)
  //@]
  //@@Function Internals
  //Recall that @|v| is an eigenvector of @|A| with associated eigenvalue
  //@|d| if
  //\[
  //  A v = d v.
  //\]
  //This decomposition can be written in matrix form as
  //\[
  //  A V = V D
  //\]
  //where
  //\[
  //  V = [v_1,v_2,\ldots,v_n], D = \mathrm{diag}(d_1,d_2,\ldots,d_n).
  //\]
  //The @|eig| function uses the @|LAPACK| class of functions @|GEEVX|
  //to compute the eigenvalue decomposition for non-symmetric 
  //(or non-Hermitian) matrices @|A|.  For symmetric matrices, @|SSYEV|
  // and @|DSYEV| are used for @|float| and @|double| matrices (respectively).
  //For Hermitian matrices, @|CHEEV| and @|ZHEEV| are used for @|complex|
  //and @|dcomplex| matrices.
  //
  //For some matrices, the process of balancing (in which the rows and
  //columns of the matrix are pre-scaled to facilitate the search for
  //eigenvalues) is detrimental to the quality of the final solution.
  //This is particularly true if the matrix contains some elements on
  //the order of round off error.  See the Example section for an example.
  //
  //A generalized eigenvector of the matrix pair @|A,B| is simply a 
  //vector @|v| with associated eigenvalue @|d| such that
  //\[
  //  A v = d B v,
  //\]
  //where @|B| is a square matrix of the same size as @|A|.  This
  //decomposition can be written in matrix form as 
  //\[
  //  A V = B V D
  //\]
  //where
  //\[
  //  V = [v_1,v_2,\ldots,v_n], D = \mathrm{diag}(d_1,d_2,\ldots,d_n).
  //\]
  //For general matrices @|A| and @|B|, the @|GGEV| class of routines are
  //used to compute the generalized eigendecomposition.  If howevever,
  //@|A| and @|B| are both symmetric (or Hermitian, as appropriate),
  //Then FreeMat first attempts to use @|SSYGV| and @|DSYGV| for @|float|
  //and @|double| arguments and @|CHEGV| and @|ZHEGV| for @|complex|
  //and @|dcomplex| arguments (respectively).  These routines requires
  //that @|B| also be positive definite, and if it fails to be, FreeMat
  //will revert to the routines used for general arguments.
  //@@Example
  //Some examples of eigenvalue decompositions.  First, for a diagonal
  //matrix, the eigenvalues are the diagonal elements of the matrix.
  //@<
  //A = diag([1.02f,3.04f,1.53f])
  //eig(A)
  //@>
  //Next, we compute the eigenvalues of an upper triangular matrix, 
  //where the eigenvalues are again the diagonal elements.
  //@<
  //A = [1.0f,3.0f,4.0f;0,2.0f,6.7f;0.0f,0.0f,1.0f]
  //eig(A)
  //@>
  //Next, we compute the complete eigenvalue decomposition of
  //a random matrix, and then demonstrate the accuracy of the solution
  //@<
  //A = float(randn(2))
  //[V,D] = eig(A)
  //A*V - V*D
  //@>
  //Now, we consider a matrix that requires the nobalance option
  //to compute the eigenvalues and eigenvectors properly.  Here is
  //an example from MATLAB's manual.
  //@<
  //B = [3,-2,-.9,2*eps;-2,4,1,-eps;-eps/4,eps/2,-1,0;-.5,-.5,.1,1]
  //[VB,DB] = eig(B)
  //B*VB - VB*DB
  //[VN,DN] = eig(B,'nobalance')
  //B*VN - VN*DN
  //@>
  //!
  ArrayVector EigFunction(int nargout, const ArrayVector& arg) {
    bool balance;
    if (arg.size() == 1)
      balance = true;
    else {
      Array b(arg[1]);
      if (b.isString()) {
	char *b2 = b.getContentsAsCString();
	if ((strcmp(b2,"nobalance") == 0) ||
	    (strcmp(b2,"NoBalance") == 0) ||
	    (strcmp(b2,"NOBALANCE") == 0))
	  balance = false;
      }
      else
	return GenEigFunction(nargout, arg);
    }
    Array A(arg[0]);
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
    if (A.anyNotFinite())
      throw Exception("eig only defined for matrices with finite entries.");
    ArrayVector retval;
    Array V, D;
    if (nargout > 1) {
      if (A.isSymmetric())
	EigenDecomposeFullSymmetric(A,V,D);
      else
	EigenDecomposeFullGeneral(A,V,D,balance);
      retval.push_back(V);
      retval.push_back(D);
    } else {
      if (A.isSymmetric())
	EigenDecomposeCompactSymmetric(A,D);
      else
	EigenDecomposeCompactGeneral(A,D,balance);
      retval.push_back(D);
    }
    return retval;
  }
  

  ArrayVector QRDNoPivotFunction(bool compactDec, Array A) {
    int nrows = A.getDimensionLength(0);
    int ncols = A.getDimensionLength(1);
    int orows = nrows;
    int ocols = ncols;
    if ((!compactDec) && (nrows > ncols)) {
      Dimensions newDim(2);
      newDim[0] = nrows;
      newDim[1] = nrows;
      A.resize(newDim);
      ncols = nrows;
    } else 
      compactDec = true;
    Class Aclass(A.getDataClass());
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    int minmn = (nrows < ncols) ? nrows : ncols;
    ArrayVector retval;
    Array rmat, qmat;
    Dimensions dim;
    switch (Aclass) {
    case FM_FLOAT:
      {
	float *q = (float*) Malloc(nrows*minmn*sizeof(float));
	float *r = (float*) Malloc(ncols*minmn*sizeof(float));
	floatQRD(nrows,ncols,q,r,(float*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  float *r2 = (float*) Malloc(orows*ocols*sizeof(float));
	  memcpy(r2,r,orows*ocols*sizeof(float));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_FLOAT,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_FLOAT,dim,r);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_FLOAT,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	break;
      }
    case FM_DOUBLE:
      {
	double *q = (double*) Malloc(nrows*minmn*sizeof(double));
	double *r = (double*) Malloc(ncols*minmn*sizeof(double));
	doubleQRD(nrows,ncols,q,r,(double*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  double *r2 = (double*) Malloc(orows*ocols*sizeof(double));
	  memcpy(r2,r,orows*ocols*sizeof(double));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_DOUBLE,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_DOUBLE,dim,r);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_DOUBLE,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	break;
      }
    case FM_COMPLEX:
      {
	float *q = (float*) Malloc(2*nrows*minmn*sizeof(float));
	float *r = (float*) Malloc(2*ncols*minmn*sizeof(float));
	complexQRD(nrows,ncols,q,r,(float*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  float *r2 = (float*) Malloc(2*orows*ocols*sizeof(float));
	  memcpy(r2,r,2*orows*ocols*sizeof(float));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_COMPLEX,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_COMPLEX,dim,r);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_COMPLEX,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	break;
      }
    case FM_DCOMPLEX:
      {
	double *q = (double*) Malloc(2*nrows*minmn*sizeof(double));
	double *r = (double*) Malloc(2*ncols*minmn*sizeof(double));
	dcomplexQRD(nrows,ncols,q,r,(double*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  double *r2 = (double*) Malloc(2*orows*ocols*sizeof(double));
	  memcpy(r2,r,2*orows*ocols*sizeof(double));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_DCOMPLEX,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_DCOMPLEX,dim,r);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_DCOMPLEX,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	break;
      }
    }
    return retval;
  }

  ArrayVector QRDPivotFunction(bool compactDec, Array A) {
    int nrows = A.getDimensionLength(0);
    int ncols = A.getDimensionLength(1);
    int orows = nrows;
    int ocols = ncols;
    int i;
    bool compactSav = compactDec;
    if ((!compactDec) && (nrows > ncols)) {
      Dimensions newDim(2);
      newDim[0] = nrows;
      newDim[1] = nrows;
      A.resize(newDim);
      ncols = nrows;
    } else 
      compactDec = true;
    Class Aclass(A.getDataClass());
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    int minmn = (nrows < ncols) ? nrows : ncols;
    ArrayVector retval;
    Array rmat, qmat, pmat;
    Dimensions dim;
    switch (Aclass) {
    case FM_FLOAT:
      {
	float *q = (float*) Malloc(nrows*minmn*sizeof(float));
	float *r = (float*) Malloc(ncols*minmn*sizeof(float));
	int *p = (int*) Malloc(ncols*sizeof(int));
	floatQRDP(nrows,ncols,q,r,p,(float*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  float *r2 = (float*) Malloc(orows*ocols*sizeof(float));
	  memcpy(r2,r,orows*ocols*sizeof(float));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_FLOAT,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_FLOAT,dim,r);
	}
	if (!compactSav) {
	  int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	  for (i=0;i<ncols;i++) 
	    p2[p[i] + i*ncols - 1] = 1;
	  dim[0] = ncols;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p2);
	  Free(p);
	} else {
	  dim[0] = 1;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_FLOAT,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	retval.push_back(pmat);
	break;
      }
    case FM_DOUBLE:
      {
	double *q = (double*) Malloc(nrows*minmn*sizeof(double));
	double *r = (double*) Malloc(ncols*minmn*sizeof(double));
	int *p = (int*) Malloc(ncols*sizeof(int));
	doubleQRDP(nrows,ncols,q,r,p,(double*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  double *r2 = (double*) Malloc(orows*ocols*sizeof(double));
	  memcpy(r2,r,orows*ocols*sizeof(double));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_DOUBLE,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_DOUBLE,dim,r);
	}
	if (!compactSav) {
	  int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	  for (i=0;i<ncols;i++) 
	    p2[p[i] + i*ncols - 1] = 1;
	  dim[0] = ncols;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p2);
	  Free(p);
	} else {
	  dim[0] = 1;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_DOUBLE,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	retval.push_back(pmat);
	break;
      }
    case FM_COMPLEX:
      {
	float *q = (float*) Malloc(2*nrows*minmn*sizeof(float));
	float *r = (float*) Malloc(2*ncols*minmn*sizeof(float));
	int *p = (int*) Malloc(ncols*sizeof(int));
	complexQRDP(nrows,ncols,q,r,p,(float*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  float *r2 = (float*) Malloc(2*orows*ocols*sizeof(float));
	  memcpy(r2,r,2*orows*ocols*sizeof(float));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_COMPLEX,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_COMPLEX,dim,r);
	}
	if (!compactSav) {
	  int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	  for (i=0;i<ncols;i++) 
	    p2[p[i] + i*ncols - 1] = 1;
	  dim[0] = ncols;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p2);
	  Free(p);
	} else {
	  dim[0] = 1;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_COMPLEX,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	retval.push_back(pmat);
	break;
      }
    case FM_DCOMPLEX:
      {
	double *q = (double*) Malloc(2*nrows*minmn*sizeof(double));
	double *r = (double*) Malloc(2*ncols*minmn*sizeof(double));
	int *p = (int*) Malloc(ncols*sizeof(int));
	dcomplexQRDP(nrows,ncols,q,r,p,(double*) A.getReadWriteDataPointer());
	if (!compactDec) {
	  double *r2 = (double*) Malloc(2*orows*ocols*sizeof(double));
	  memcpy(r2,r,2*orows*ocols*sizeof(double));
	  dim[0] = orows;
	  dim[1] = ocols;
	  rmat = Array(FM_DCOMPLEX,dim,r2);
	  Free(r);
	} else {	  
	  dim[0] = minmn;
	  dim[1] = ncols;
	  rmat = Array(FM_DCOMPLEX,dim,r);
	}
	if (!compactSav) {
	  int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	  for (i=0;i<ncols;i++) 
	    p2[p[i] + i*ncols - 1] = 1;
	  dim[0] = ncols;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p2);
	  Free(p);
	} else {
	  dim[0] = 1;
	  dim[1] = ncols;
	  pmat = Array(FM_INT32,dim,p);
	}
	dim[0] = nrows;
	dim[1] = minmn;
	qmat = Array(FM_DCOMPLEX,dim,q);
	retval.push_back(qmat);
	retval.push_back(rmat);
	retval.push_back(pmat);
	break;
      }
    }
    return retval;
  }

  //!
  //@Module QR QR Decomposition of a Matrix
  //@@Usage
  //Computes the QR factorization of a matrix.  The @|qr| function has
  //multiple forms, with and without pivoting.  The non-pivot version
  //has two forms, a compact version and a full-blown decomposition
  //version.  The compact version of the decomposition of a matrix 
  //of size @|M x N| is
  //@[
  //  [q,r] = qr(a,0)
  //@]
  //where @|q| is a matrix of size @|M x L| and @|r| is a matrix of
  //size @|L x N| and @|L = min(N,M)|, and @|q*r = a|.  The QR decomposition is
  //such that the columns of @|Q| are orthonormal, and @|R| is upper
  //triangular.  The decomposition is computed using the LAPACK 
  //routine @|xgeqrf|, where @|x| is the precision of the matrix.  Unlike
  //MATLAB (and other MATLAB-compatibles), FreeMat supports decompositions
  //of all four floating point types, @|float, complex, double, dcomplex|.
  //
  //The second form of the non-pivot decomposition omits the second @|0|
  //argument:
  //@[
  //  [q,r] = qr(a)
  //@]
  //This second form differs from the previous form only for matrices
  //with more rows than columns (@|M > N|).  For these matrices, the
  //full decomposition is of a matrix @|Q| of size @|M x M| and 
  //a matrix @|R| of size @|M x N|.  The full decomposition is computed
  //using the same LAPACK routines as the compact decomposition, but
  //on an augmented matrix @|[a 0]|, where enough columns are added to
  //form a square matrix.
  //
  //Generally, the QR decomposition will not return a matrix @|R| with
  //diagonal elements in any specific order.  The remaining two forms 
  //of the @|qr| command utilize permutations of the columns of @|a|
  //so that the diagonal elements of @|r| are in decreasing magnitude.
  //To trigger this form of the decomposition, a third argument is
  //required, which records the permutation applied to the argument @|a|.
  //The compact version is
  //@[
  //  [q,r,e] = qr(a,0)
  //@]
  //where @|e| is an integer vector that describes the permutation of
  //the columns of @|a| necessary to reorder the diagonal elements of
  //@|r|.  This result is computed using the LAPACK routines @|(s,d)geqp3|.
  //In the non-compact version of the QR decomposition with pivoting,
  //@[
  //  [q,r,e] = qr(a)
  //@]
  //the returned matrix @|e| is a permutation matrix, such that 
  //@|q*r*e' = a|.
  //!
  ArrayVector QRDFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("qr function requires at least one argument - the matrix to decompose.");
    Array A(arg[0]);
    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply qr decomposition to reference types.");
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
    if (A.anyNotFinite())
      throw Exception("QR Decomposition only defined for matrices with finite entries.");
    int nrows = A.getDimensionLength(0);
    int ncols = A.getDimensionLength(1);
    bool compactDecomposition = false;
    if (arg.size() == 2) {
      Array cflag(arg[1]);
      int cflag_int = cflag.getContentsAsIntegerScalar();
      if (cflag_int == 0) compactDecomposition = true;
    }
    if (nargout == 3)
      return QRDPivotFunction(compactDecomposition, A);
    else
      return QRDNoPivotFunction(compactDecomposition, A);
  }

  //!
  //@Module SVD Singular Value Decomposition of a Matrix
  //@@Usage
  //Computes the singular value decomposition (SVD) of a matrix.  The 
  //@|svd| function has three forms.  The first returns only the singular
  //values of the matrix:
  //@[
  //  s = svd(A)
  //@]
  //The second form returns both the singular values in a diagonal
  //matrix @|S|, as well as the left and right eigenvectors.
  //@[
  //  [U,S,V] = svd(A)
  //@]
  //The third form returns a more compact decomposition, with the
  //left and right singular vectors corresponding to zero singular
  //values being eliminated.  The syntax is
  //@[
  //  [U,S,V] = svd(A,0)
  //@]
  //@@Function Internals
  //Recall that @|sigma_i| is a singular value of an @|M x N|
  //matrix @|A| if there exists two vectors @|u_i, v_i| where @|u_i| is
  //of length @|M|, and @|v_i| is of length @|u_i| and
  //\[
  //  A v_i = \sigma_i u_i
  //\]
  //and generally
  //\[
  //  A = \sum_{i=1}^{K} \sigma_i u_i*v_i',
  //\]
  //where @|K| is the rank of @|A|.  In matrix form, the left singular
  //vectors @|u_i| are stored in the matrix @|U| as
  //\[
  //  U = [u_1,\ldots,u_m], V = [v_1,\ldots,v_n]
  //\]
  //The matrix @|S| is then of size @|M x N| with the singular
  //values along the diagonal.  The SVD is computed using the 
  //@|LAPACK| class of functions @|GESDD|.
  //@@Examples
  //Here is an example of a partial and complete singular value
  //decomposition.
  //@<
  //A = float(randn(2,3))
  //[U,S,V] = svd(A)
  //U*S*V'
  //svd(A)
  //@>
  //!
  ArrayVector SVDFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() > 2)
      throw Exception("svd function takes at most two arguments");
    if (arg.size() < 1)
      throw Exception("svd function requries at least one argument - the matrix to decompose");
    Array A(arg[0]);
    bool compactform = false;
    if (arg.size() > 1) {
      Array flag(arg[1]);
      if (flag.getContentsAsIntegerScalar() == 0)
	compactform = true;
    }
    // Test for numeric
    if (A.isReferenceType())
      throw Exception("Cannot apply svd to reference types.");
    if (!A.is2D())
      throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");

    if (A.anyNotFinite())
      throw Exception("SVD only defined for matrices with finite entries.");
    int nrows = A.getDimensionLength(0);
    int ncols = A.getDimensionLength(1);
    Class Aclass(A.getDataClass());
    if (Aclass < FM_FLOAT) {
      A.promoteType(FM_DOUBLE);
      Aclass = FM_DOUBLE;
    }
    bool computevectors;
    computevectors = (nargout>1);
    ArrayVector retval;
    switch (Aclass) {
    case FM_FLOAT:
      {
	int mindim;
	mindim = (nrows < ncols) ? nrows : ncols;
	// A temporary vector to store the singular values
	float *svals = (float*) Malloc(mindim*sizeof(float));
	// A temporary vector to store the left singular vectors
	float *umat = NULL; 
	// A temporary vector to store the right singular vectors
	float *vtmat = NULL; 
	if (computevectors) 
	  if (!compactform) {
	    umat = (float*) Malloc(nrows*nrows*sizeof(float));
	    vtmat = (float*) Malloc(ncols*ncols*sizeof(float));
	  } else {
	    umat = (float*) Malloc(nrows*mindim*sizeof(float));
	    vtmat = (float*) Malloc(ncols*mindim*sizeof(float));
	  }
	floatSVD(nrows,ncols,umat,vtmat,svals,
		 (float*) A.getReadWriteDataPointer(),
		 compactform, computevectors);
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (!computevectors) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_FLOAT,dim,svals));
	} else {
	  if (!compactform) {
	    dim[0] = nrows; dim[1] = nrows;
	    retval.push_back(Array(FM_FLOAT,dim,umat));
	    dim[0] = nrows; dim[1] = ncols;
	    float *smat = (float*) Malloc(nrows*ncols*sizeof(float));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*nrows] = svals[i];
	    retval.push_back(Array(FM_FLOAT,dim,smat));
	    dim[0] = ncols; dim[1] = ncols;
	    Array Utrans(FM_FLOAT,dim,vtmat);
	    Utrans.transpose();
	    retval.push_back(Utrans);
	    Free(svals);
	  } else {
	    dim[0] = nrows; dim[1] = mindim;
	    retval.push_back(Array(FM_FLOAT,dim,umat));
	    dim[0] = mindim; dim[1] = mindim;
	    float *smat = (float*) Malloc(mindim*mindim*sizeof(float));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*mindim] = svals[i];
	    retval.push_back(Array(FM_FLOAT,dim,smat));
	    dim[0] = mindim; dim[1] = ncols;
	    Array Utrans(FM_FLOAT,dim,vtmat);
	    Utrans.transpose();
	    retval.push_back(Utrans);
	    Free(svals);
	  }
	}
	break;
      }
    case FM_DOUBLE:
      {
	int mindim;
	mindim = (nrows < ncols) ? nrows : ncols;
	// A temporary vector to store the singular values
	double *svals = (double*) Malloc(mindim*sizeof(double));
	// A temporary vector to store the left singular vectors
	double *umat = NULL;
	// A temporary vector to store the right singular vectors
	double *vtmat = NULL;
	if (computevectors) 
	  if (!compactform) {
	    umat = (double*) Malloc(nrows*nrows*sizeof(double));
	    vtmat = (double*) Malloc(ncols*ncols*sizeof(double));
	  } else {
	    umat = (double*) Malloc(nrows*mindim*sizeof(double));
	    vtmat = (double*) Malloc(ncols*mindim*sizeof(double));
	  }
	doubleSVD(nrows,ncols,umat,vtmat,svals,
		  (double*) A.getReadWriteDataPointer(),
		  compactform, computevectors);
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (!computevectors) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_DOUBLE,dim,svals));
	  Free(umat);
	  Free(vtmat);
	} else {
	  if (!compactform) {
	    dim[0] = nrows; dim[1] = nrows;
	    retval.push_back(Array(FM_DOUBLE,dim,umat));
	    dim[0] = nrows; dim[1] = ncols;
	    double *smat = (double*) Malloc(nrows*ncols*sizeof(double));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*nrows] = svals[i];
	    retval.push_back(Array(FM_DOUBLE,dim,smat));
	    dim[0] = ncols; dim[1] = ncols;
	    Array Utrans(FM_DOUBLE,dim,vtmat);
	    Utrans.transpose();
	    retval.push_back(Utrans);
	    Free(svals);
	  } else {
	    dim[0] = nrows; dim[1] = mindim;
	    retval.push_back(Array(FM_DOUBLE,dim,umat));
	    dim[0] = mindim; dim[1] = mindim;
	    double *smat = (double*) Malloc(mindim*mindim*sizeof(double));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*mindim] = svals[i];
	    retval.push_back(Array(FM_DOUBLE,dim,smat));
	    dim[0] = mindim; dim[1] = ncols;
	    Array Utrans(FM_DOUBLE,dim,vtmat);
	    Utrans.transpose();
	    retval.push_back(Utrans);
	    Free(svals);
	  }		  
	}
	break;
      }
    case FM_COMPLEX:
      {
	int mindim;
	mindim = (nrows < ncols) ? nrows : ncols;
	// A temporary vector to store the singular values
	float *svals = (float*) Malloc(mindim*sizeof(float));
	// A temporary vector to store the left singular vectors
	float *umat = NULL;
	// A temporary vector to store the right singular vectors
	float *vtmat = NULL;
	if (computevectors)
	  if (!compactform) {
	    umat = (float*) Malloc(2*nrows*nrows*sizeof(float));
	    vtmat = (float*) Malloc(2*ncols*ncols*sizeof(float));
	  } else {
	    umat = (float*) Malloc(2*nrows*mindim*sizeof(float));
	    vtmat = (float*) Malloc(2*ncols*mindim*sizeof(float));
	  }
	complexSVD(nrows,ncols,umat,vtmat,svals,
		   (float*) A.getReadWriteDataPointer(),
		   compactform, computevectors);
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (!computevectors) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_FLOAT,dim,svals));
	  Free(umat);
	  Free(vtmat);
	} else {
	  if (!compactform) {
	    dim[0] = nrows; dim[1] = nrows;
	    retval.push_back(Array(FM_COMPLEX,dim,umat));
	    dim[0] = nrows; dim[1] = ncols;
	    float *smat = (float*) Malloc(nrows*ncols*sizeof(float));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*nrows] = svals[i];
	    retval.push_back(Array(FM_FLOAT,dim,smat));
	    dim[0] = ncols; dim[1] = ncols;
	    Array Utrans(FM_COMPLEX,dim,vtmat);
	    Utrans.hermitian();
	    retval.push_back(Utrans);
	    Free(svals);
	  } else {
	    dim[0] = nrows; dim[1] = mindim;
	    retval.push_back(Array(FM_COMPLEX,dim,umat));
	    dim[0] = mindim; dim[1] = mindim;
	    float *smat = (float*) Malloc(mindim*mindim*sizeof(float));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*mindim] = svals[i];
	    retval.push_back(Array(FM_FLOAT,dim,smat));
	    dim[0] = mindim; dim[1] = ncols;
	    Array Utrans(FM_COMPLEX,dim,vtmat);
	    Utrans.hermitian();
	    retval.push_back(Utrans);
	    Free(svals);
	  }
	}
	break;
      }
    case FM_DCOMPLEX:
      {
	int mindim;
	mindim = (nrows < ncols) ? nrows : ncols;
	// A temporary vector to store the singular values
	double *svals = (double*) Malloc(mindim*sizeof(double));
	// A temporary vector to store the left singular vectors
	double *umat = NULL;
	// A temporary vector to store the right singular vectors
	double *vtmat = NULL;
	if (computevectors)
	  if (!compactform) {
	    umat = (double*) Malloc(2*nrows*nrows*sizeof(double));
	    vtmat = (double*) Malloc(2*ncols*ncols*sizeof(double));
	  } else {
	    umat = (double*) Malloc(2*nrows*mindim*sizeof(double));
	    vtmat = (double*) Malloc(2*ncols*mindim*sizeof(double));
	  }
	dcomplexSVD(nrows,ncols,umat,vtmat,svals,
		    (double*) A.getReadWriteDataPointer(),
		    compactform, computevectors);
	// Always transfer the singular values into an Array
	Dimensions dim;
	if (!computevectors) {
	  dim[0] = mindim; dim[1] = 1;
	  retval.push_back(Array(FM_DOUBLE,dim,svals));
	  Free(umat);
	  Free(vtmat);
	} else {
	  if (!compactform) {
	    dim[0] = nrows; dim[1] = nrows;
	    retval.push_back(Array(FM_DCOMPLEX,dim,umat));
	    dim[0] = nrows; dim[1] = ncols;
	    double *smat = (double*) Malloc(nrows*ncols*sizeof(double));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*nrows] = svals[i];
	    retval.push_back(Array(FM_DOUBLE,dim,smat));
	    dim[0] = ncols; dim[1] = ncols;
	    Array Utrans(FM_DCOMPLEX,dim,vtmat);
	    Utrans.hermitian();
	    retval.push_back(Utrans);
	    Free(svals);
	  } else {
	    dim[0] = nrows; dim[1] = mindim;
	    retval.push_back(Array(FM_DCOMPLEX,dim,umat));
	    dim[0] = mindim; dim[1] = mindim;
	    double *smat = (double*) Malloc(mindim*mindim*sizeof(double));
	    for (int i=0;i<mindim;i++)
	      smat[i+i*mindim] = svals[i];
	    retval.push_back(Array(FM_DOUBLE,dim,smat));
	    dim[0] = mindim; dim[1] = ncols;
	    Array Utrans(FM_DCOMPLEX,dim,vtmat);
	    Utrans.hermitian();
	    retval.push_back(Utrans);
	    Free(svals);
	  }
	}
      }
      break;
    }
    return retval;
  }

  //!
  //@Module LASTERR Retrieve Last Error Message
  //@@Usage
  //Either returns or sets the last error message.  The
  //general syntax for its use is either
  //@[
  //  msg = lasterr
  //@]
  //which returns the last error message that occured, or
  //@[
  //  lasterr(msg)
  //@]
  //which sets the contents of the last error message.
  //@@Example
  //Here is an example of using the @|error| function to
  //set the last error, and then retrieving it using
  //lasterr.
  //@<
  //try; error('Test error message'); catch; end;
  //lasterr
  //@>
  //Or equivalently, using the second form:
  //@<
  //lasterr('Test message');
  //lasterr
  //@>
  //!
  ArrayVector LasterrFunction(int nargout, const ArrayVector& arg,
			      WalkTree* eval) {
     ArrayVector retval;
     if (arg.size() == 0) {
       Array A = Array::stringConstructor(eval->getLastErrorString());
       retval.push_back(A);
     } else {
       Array tmp(arg[0]);
       eval->setLastErrorString(tmp.getContentsAsCString());
     }
     return retval;
   }

  //!
  //@Module SLEEP Sleep For Specified Number of Seconds
  //@@Usage
  //Suspends execution of FreeMat for the specified number
  //of seconds.  The general syntax for its use is
  //@[
  //  sleep(n),
  //@]
  //where @|n| is the number of seconds to wait.
  //!
  ArrayVector SleepFunction(int nargout, const ArrayVector& arg) {
     if (arg.size() != 1)
       throw Exception("sleep function requires 1 argument");
     int sleeptime;
     Array a(arg[0]);
     sleeptime = a.getContentsAsIntegerScalar();
#ifndef WIN32
     sleep(sleeptime);
#else
     Sleep(1000*sleeptime);
#endif
     return ArrayVector();
   }

  //!
  //@Module DIAG Diagonal Matrix Construction/Extraction
  //@@Usage
  //The @|diag| function is used to either construct a 
  //diagonal matrix from a vector, or return the diagonal
  //elements of a matrix as a vector.  The general syntax
  //for its use is
  //@[
  //  y = diag(x,n)
  //@]
  //If @|x| is a matrix, then @|y| returns the @|n|-th 
  //diagonal.  If @|n| is omitted, it is assumed to be
  //zero.  Conversely, if @|x| is a vector, then @|y|
  //is a matrix with @|x| set to the @|n|-th diagonal.
  //@@Examples
  //Here is an example of @|diag| being used to extract
  //a diagonal from a matrix.
  //@<
  //A = int32(10*rand(4,5))
  //diag(A)
  //diag(A,1)
  //@>
  //Here is an example of the second form of @|diag|, being
  //used to construct a diagonal matrix.
  //@<
  //x = int32(10*rand(1,3))
  //diag(x)
  //diag(x,-1)
  //@>
  //!
  ArrayVector DiagFunction(int nargout, const ArrayVector& arg) {
    Array a;
    Array b;
    Array c;
    ArrayVector retval;
    int32 *dp;
    int diagonalOrder;
    // First, get the diagonal order, and synthesize it if it was
    // not supplied
    if (arg.size() == 1) 
      diagonalOrder = 0;
    else {
      b = arg[1];
      if (!b.isScalar()) 
	throw Exception("second argument must be a scalar.\n");
      b.promoteType(FM_INT32);
      dp = (int32 *) b.getDataPointer();
      diagonalOrder = dp[0];
    }
    // Next, make sure the first argument is 2D
    a = arg[0];
    if (!a.is2D()) 
      throw Exception("First argument to 'diag' function must be 2D.\n");
    // Case 1 - if the number of columns in a is 1, then this is a diagonal
    // constructor call.
    if ((a.getDimensionLength(1) == 1) || (a.getDimensionLength(0) == 1))
      c = Array::diagonalConstructor(a,diagonalOrder);
    else
      c = a.getDiagonal(diagonalOrder);
    retval.push_back(c);
    return retval;
  }

  //!
  //@Module ISEMPTY Test For Variable Empty
  //@@Usage
  //The @|isempty| function returns a boolean that indicates
  //if the argument variable is empty or not.  The general
  //syntax for its use is
  //@[
  //  y = isempty(x).
  //@]
  //@@Examples
  //Here are some examples of the @|isempty| function
  //@<
  //a = []
  //isempty(a)
  //b = 1:3
  //isempty(b)
  //@>
  //Note that if the variable is not defined, @|isempty| 
  //does not return true.
  //@<
  //isempty(x)
  //@>
  //!
  ArrayVector IsEmptyFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("isempty function requires at least input argument");
    ArrayVector retval;
    retval.push_back(Array::logicalConstructor(arg[0].isEmpty()));
    return retval;
  }

  //!
  //@Module ERROR Causes an Error Condition Raised
  //@@Usage
  //The @|error| function causes an error condition (exception
  //to be raised).  The general syntax for its use is
  //@[
  //   error(s),
  //@]
  //where @|s| is the string message describing the error.  The
  //@|error| function is usually used in conjunction with @|try|
  //and @|catch| to provide error handling.
  //@@Example
  //Here is a simple example of an @|error| being issued by a function
  //@|evenoddtest|:
  //@{ evenoddtest.m
  //function evenoddtest(n)
  //  if (n==0)
  //    error('zero is neither even nor odd');
  //  elseif (typeof(n) ~= 'int32')
  //    error('expecting integer argument');
  //  end;
  //  if (n==int32(n/2)*2)
  //    printf('%d is even\n',n);
  //  else
  //    printf('%d is odd\n',n);
  //  end
  //@}
  //The normal command line prompt @|-->| simply prints the error
  //that occured.
  //@<
  //evenoddtest(4)
  //evenoddtest(5)
  //evenoddtest(0)
  //evenoddtest(pi)
  //@>
  //!
  ArrayVector ErrorFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() == 0)
      throw Exception("Not enough inputs to error function");
    if (!(arg[0].isString()))
      throw Exception("Input to error function must be a string");
    throw Exception(arg[0].getContentsAsCString());
  }


//   ArrayVector PrintStats(int nargout, const ArrayVector& arg) {
//     printObjectBalance();
//     printExceptionCount();
//     ArrayVector retval;
//     return retval;
//   }

//   ArrayVector PrintArrays(int nargout, const ArrayVector& arg) {
//     dumpAllArrays();
//     ArrayVector retval;
//     return retval;
//   }

//   ArrayVector TestFunction(int nargout, const ArrayVector& arg) {
//     if (arg.size() != 1)
//       throw Exception("test function requires exactly one argument");
//     ArrayVector retval;
//     Array A(arg[0]);
//     bool alltrue = true;
//     if (A.isEmpty())
//       alltrue = false;
//     else {
//       A.promoteType(FM_LOGICAL);
//       const logical* dp = (const logical *) A.getDataPointer();
//       int length = A.getLength();
//       int i = 0;
//       while (alltrue && (i<length)) {
// 	alltrue &= (dp[i]);
// 	i++;
//       }
//     }
//     retval.push_back(Array::logicalConstructor(alltrue));
//     return retval;
//   }

//   ArrayVector ClockFunction(int nargout, const ArrayVector& arg) {
//     ArrayVector retval;
// #ifndef WIN32
//     retval.push_back(Array::uint32Constructor(clock()));
// #else
// 	throw Exception("Clock function not available under win32");
// #endif
//     return retval;
//   }

  //!
  //@Module EVAL Evaluate a String
  //@@Usage
  //The @|eval| function evaluates a string.  The general syntax
  //for its use is
  //@[
  //   eval(s)
  //@]
  //where @|s| is the string to evaluate.
  //@@Example
  //Here are some examples of @|eval| being used.
  //@<
  //eval('a = 32')
  //@>
  //The primary use of the @|eval| statement is to enable construction
  //of expressions at run time.
  //@<
  //s = ['b = a' ' + 2']
  //eval(s)
  //@>
  //!
  ArrayVector EvalFunction(int nargout, const ArrayVector& arg,WalkTree* eval){
    if (arg.size() != 1)
      throw Exception("eval function takes exactly one argument - the string to execute");
    char *line = arg[0].getContentsAsCString();
    char *buf = (char*) malloc(strlen(line)+2);
    sprintf(buf,"%s\n",line);
    eval->evaluateString(buf);
    free(buf);
    return ArrayVector();
  }
  
  //!
  //@Module SOURCE Execute an Arbitrary File
  //@@Usage
  //The @|source| function executes the contents of the given
  //filename one line at a time (as if it had been typed at
  //the @|-->| prompt).  The @|source| function syntax is
  //@[
  //  source(filename)
  //@]
  //where @|filename| is a @|string| containing the name of
  //the file to process.
  //@@Example
  //First, we write some commands to a file (note that it does
  //not end in the usual @|.m| extension):
  //@<
  //fp = fopen('source_test','w');
  //fprintf(fp,'a = 32;\n');
  //fprintf(fp,'b = a;\n');
  //fclose(fp);
  //@>
  //Now we source the resulting file.
  //@<
  //clear all
  //source source_test
  //who
  //@>
  //!
  ArrayVector SourceFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("source function takes exactly one argument - the filename of the script to execute");
    char *line = arg[0].getContentsAsCString();
    FILE *fp;
    fp = fopen(line,"r");
    if (!fp)
      throw Exception(std::string("unable to open file ") + line + " for reading");
    while (!feof(fp)) {
      char buffer[4096];
      fgets(buffer,sizeof(buffer),fp);
      if (!feof(fp))
	eval->evaluateString(buffer);
    }
    fclose(fp);
    return ArrayVector();
  }

//   ArrayVector FdumpFunction(int nargout, const ArrayVector& arg,WalkTree* eval){
//     if (arg.size() == 0)
//       throw Exception("fdump function requires at least one argument");
//     if (!(arg[0].isString()))
//       throw Exception("first argument to fdump must be the name of a function (i.e., a string)");
//     char *fname = arg[0].getContentsAsCString();
//     Context *context = eval->getContext();
//     FunctionDef *funcDef;
//     if (!context->lookupFunction(fname,funcDef))
//       throw Exception(std::string("function ") + fname + " undefined!");
//     funcDef->updateCode();
//     funcDef->printMe(eval->getInterface());
//     return ArrayVector();
//   }

  //!
  //@Module FEVAL Evaluate a Function
  //@@Usage
  //The @|feval| function executes a function using its name.
  //The syntax of @|feval| is
  //@[
  //  [y1,y2,...,yn] = feval(fname,x1,x2,...,xm)
  //@]
  //where @|fname| is the name of the function to evaluate, and
  //@|xi| are the arguments to the function, and @|yi| are the
  //return values.
  //@@Example
  //Here is an example of using @|feval| to call the @|cos| 
  //function indirectly.
  //@<
  //feval('cos',pi/4)
  //@>
  //!
  ArrayVector FevalFunction(int nargout, const ArrayVector& arg,WalkTree* eval){
    if (arg.size() == 0)
      throw Exception("feval function requires at least one argument");
    if (!(arg[0].isString()))
      throw Exception("first argument to feval must be the name of a function (i.e., a string)");
    char *fname = arg[0].getContentsAsCString();
    Context *context = eval->getContext();
    FunctionDef *funcDef;
    if (!context->lookupFunction(fname,funcDef))
      throw Exception(std::string("function ") + fname + " undefined!");
    funcDef->updateCode();
    if (funcDef->scriptFlag)
      throw Exception("cannot use feval on a script");
    ArrayVector newarg(arg);
    //    newarg.pop_front();
    newarg.erase(newarg.begin());
    return(funcDef->evaluateFunction(eval,newarg,nargout));
  }

  //!
  //@Module REPMAT Array Replication Function
  //@@Usage
  //The @|repmat| function replicates an array the specified
  //number of times.  The source and destination arrays may
  //be multidimensional.  There are three distinct syntaxes for
  //the @|repmap| function.  The first form:
  //@[
  //  y = repmat(x,n)
  //@]
  //replicates the array @|x| on an @|n-times-n| tiling, to create
  //a matrix @|y| that has @|n| times as many rows and columns
  //as @|x|.  The output @|y| will match @|x| in all remaining
  //dimensions.  The second form is
  //@[
  //  y = repmat(x,m,n)
  //@]
  //And creates a tiling of @|x| with @|m| copies of @|x| in the
  //row direction, and @|n| copies of @|x| in the column direction.
  //The final form is the most general
  //@[
  //  y = repmat(x,[m n p...])
  //@]
  //where the supplied vector indicates the replication factor in 
  //each dimension.  
  //@@Example
  //Here is an example of using the @|repmat| function to replicate
  //a row 5 times.  Note that the same effect can be accomplished
  //(although somewhat less efficiently) by a multiplication.
  //@<
  //x = [1 2 3 4]
  //y = repmat(x,[5,1])
  //@>
  //The @|repmat| function can also be used to create a matrix of scalars
  //or to provide replication in arbitrary dimensions.  Here we use it to
  //replicate a 2D matrix into a 3D volume.
  //@<
  //x = [1 2;3 4]
  //y = repmat(x,[1,1,3])
  //@>
  //!
#define MAX(a,b) ((a) > (b) ? (a) : (b))
  ArrayVector RepMatFunction(int nargout, const ArrayVector& arg) {
#if 0
    Dimensions outdm(2);
    outdm[0] = 2;
    outdm[1] = 6;
    void *dp;
    dp = Malloc(48);
    ArrayVector retval;
    Array t(FM_CELL_ARRAY,outdm, dp);
    dp = Malloc(48);
    t = Array(FM_CELL_ARRAY,outdm, dp);
    Array s = t;
    s = Array::int32Constructor(534);
    s = t;
    s = Array::int32Constructor(534);
    retval.push_back(t);
    return retval;
#endif
    int i, j, k;
    if (arg.size() < 2)
      throw Exception("repmat function requires at least two arguments");
    Array x(arg[0]);
    Dimensions repcount;
    // Case 1, look for a scalar second argument
    if ((arg.size() == 2) && (arg[1].isScalar())) {
      Array t(arg[1]);
      repcount[0] = t.getContentsAsIntegerScalar();
      repcount[1] = t.getContentsAsIntegerScalar();
    } 
    // Case 2, look for two scalar arguments
    else if ((arg.size() == 3) && (arg[1].isScalar()) && (arg[2].isScalar())) {
      Array t(arg[1]);
      repcount[0] = t.getContentsAsIntegerScalar();
      t = arg[2];
      repcount[1] = t.getContentsAsIntegerScalar();
    }
    // Case 3, look for a vector second argument
    else {
      if (arg.size() > 2) throw Exception("unrecognized form of arguments for repmat function");
      Array t(arg[1]);
      t.promoteType(FM_INT32);
      if (t.getLength() > maxDims) throw Exception("replication request exceeds maxDims constant - either rebuild FreeMat with a higher maxDims constant, or shorten the requested replication array");
      int32 *dp = (int32*) t.getDataPointer();
      for (i=0;i<t.getLength();i++)
	repcount[i] = dp[i];
    }
    // Check for a valid replication count
    for (i=0;i<repcount.getLength();i++)
      if (repcount[i] < 0) throw Exception("negative replication counts not allowed in argument to repmat function");
    // All is peachy.  Allocate an output array of sufficient size.
    Dimensions originalSize(x.getDimensions());
    Dimensions outdims;
    int outdim;
    outdim = MAX(repcount.getLength(),originalSize.getLength());
    for (i=0;i<outdim;i++)
      outdims[i] = originalSize[i]*repcount[i];
    outdims.simplify();
    void *dp = Array::allocateArray(x.getDataClass(),
				    outdims.getElementCount(),
				    x.getFieldNames());
    // Copy can work by pushing or by pulling.  I have opted for
    // pushing, because we can push a column at a time, which might
    // be slightly more efficient.
    int colsize = originalSize[0];
    int outcolsize = outdims[0];
    int colcount = originalSize.getElementCount()/colsize;
    // copySelect stores which copy we are pushing.
    Dimensions copySelect(outdim);
    // anchor is used to calculate where this copy lands in the output matrix
    Dimensions anchor(outdim);
    // sourceAddress is used to track which column we are pushing in the
    // source matrix
    Dimensions sourceAddress(originalSize.getLength());
    int destanchor;
    int copyCount;
    copyCount = repcount.getElementCount();
    for (i=0;i<copyCount;i++) {
      // Reset the source address
      sourceAddress.zeroOut();
      // Next, we loop over the columns of the source matrix
      for (j=0;j<colcount;j++) {
	// We can calculate the anchor of this copy by multiplying the source
	// address by the copySelect vector
	for (k=0;k<outdim;k++)
	  anchor[k] = copySelect[k]*originalSize[k]+sourceAddress[k];
	// Now, we map this to a point in the destination array
	destanchor = outdims.mapPoint(anchor);
	// And copy the elements
	x.copyElements(j*colsize,dp,destanchor,colsize);
	// Now increment the source address
	sourceAddress.incrementModulo(originalSize,1);
      }
      copySelect.incrementModulo(repcount,0);
    }
    ArrayVector retval;
    retval.push_back(Array(x.getDataClass(),outdims,dp,x.getFieldNames()));
    return retval;
  }

  //!
  //@Module SYSTEM Call an External Program
  //@@Usage
  //The @|system| function allows you to call an external
  //program from within FreeMat, and capture the output.
  //The syntax of the @|system| function is
  //@[
  //  y = system(cmd)
  //@]
  //where @|cmd| is the command to execute.  The return
  //array @|y| is of type @|cell-array|, where each entry
  //in the array corresponds to a line from the output.
  //@@Example
  //Here is an example of calling the @|ls| function (the
  //list files function under Un*x-like operating system).
  //@<
  //y = system('ls m*.m')
  //y{1}
  //@>
  //!
  ArrayVector SystemFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1) 
      throw Exception("System function takes one string argument");
    char *systemArg = arg[0].getContentsAsCString();
    ArrayVector retval;
    if (strlen(systemArg) == 0) 
      return retval;
    stringVector cp(DoSystemCallCaptured(systemArg));
    Array* np = new Array[cp.size()];
    for (int i=0;i<cp.size();i++)
      np[i] = Array::stringConstructor(cp[i]);
    Dimensions dim(2);
    dim[0] = cp.size();
    dim[1] = 1;
    Array ret(FM_CELL_ARRAY,dim,np);
    retval.push_back(ret);
    return retval;
  }
}

