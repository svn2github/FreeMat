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

#include "Math.hpp"
#include "EigenDecompose.hpp"
#include "Algorithms.hpp"

static ArrayVector GenEigFunction(int nargout, const ArrayVector &arg) {
  Array A(arg[0]);
  Array B(arg[1]);
  if (A.isSparse() || B.isSparse())
    throw Exception("eig only defined for full matrices.");  
  if (AnyNotFinite(A) || AnyNotFinite(B))
    throw Exception("eig only defined for matrices with finite entries.");
  if (A.isReferenceType() || B.isReferenceType())
    throw Exception("Cannot apply eigendecomposition to reference types.");
  if (!A.is2D() || !B.is2D())
    throw Exception("Cannot apply eigendecomposition to N-Dimensional arrays.");
  if (!A.isSquare())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  if (!B.isSquare())
    throw Exception("Cannot eigendecompose a non-square matrix.");
  if (A.rows() != B.rows())
    throw Exception("B and A must be the same size when computing a generalized eigendecomposition");

  // Handle the type of A - if it is an integer type, then promote to double
  if (A.dataClass() != B.dataClass())
    throw Exception("B and A must be the same data class when computing a generalized eigendecomposition");

  ArrayVector retval;
  Array V, D;

  if (nargout > 1) {
    if (IsSymmetric(A) && IsSymmetric(B)) {
      if (!GeneralizedEigenDecomposeFullSymmetric(A,B,V,D))
	GeneralizedEigenDecomposeFullGeneral(A,B,V,D);
    } else
      GeneralizedEigenDecomposeFullGeneral(A,B,V,D);
    retval.push_back(V);
    retval.push_back(D);
  } else {
    if (IsSymmetric(A) && IsSymmetric(B)) {
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
//@@Section TRANSFORMS
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
//@@Tests
//@{ test_eig1.m
//% Test eigenvalue function - general matrices
//function t = test_eig1
//% First the float version
//t1all = 1;
//for i=2:4:100
//  a = float(randn(i)); 
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 100*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('full: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t1all = t1all & t1 & tb;
//end
//% Now the double version
//t2all = 1;
//for i=2:4:100
//  a = double(randn(i)); 
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 100*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('double: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t2all = t2all & t1 & tb;
//end
//% Now the complex version
//t3all = 1;
//for i=2:4:100
//  a = complex(randn(i)+j*randn(i)); 
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 100*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('complex: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t3all = t3all & t1 & tb;
//end
//% Now the double version
//t4all = 1;
//for i=2:4:100
//  a = dcomplex(randn(i)+j*randn(i)); 
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 100*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('dcomplex: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t4all = t4all & t1 & tb;
//end
//t = t1all & t2all & t3all & t4all;
//@}
//@{ test_eig2.m
//% Test eigenvalue function - symmetric matrices
//function t = test_eig2
//% First the float version
//t1all = 1;
//for i=2:4:100
//  a = float(randn(i)); a = a + a';
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t1all = t1all & t1 & tb;
//end
//% Now the double version
//t2all = 1;
//for i=2:4:100
//  a = double(randn(i)); a = a + a';
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t2all = t2all & t1 & tb;
//end
//% Now the complex version
//t3all = 1;
//for i=2:4:100
//  a = complex(randn(i)+j*randn(i)); a = a + a';
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t3all = t3all & t1 & tb;
//end
//% Now the double version
//t4all = 1;
//for i=2:4:100
//  a = dcomplex(randn(i)+j*randn(i)); a = a + a';
//  [v,d] = eig(a);
//  emat = a*v - v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t4all = t4all & t1 & tb;
//end
//t = t1all & t2all & t3all & t4all;
//@}
//@{ test_eig3.m
//% Test the 'nobalance' option for eig
//function t = test_eig3
//B = [3,-2,-.9,2*eps;-2,4,1,-eps;-eps/4,eps/2,-1,0;-.5,-.5,.1,1];
//[VN,DN] = eig(B,'nobalance');
//er = B*VN - VN*DN;
//er = max(abs(er(:)));
//bnd = 1.2*max(diag(DN))*eps*8;
//t = (er < bnd);
//@}
//@{ test_eig4.m
//% Test generalized eigenvalue function - general matrices 
//function t = test_eig4
//% First the float version
//t1all = 1;
//for i=2:4:100
//  a = float(randn(i)); 
//  b = float(randn(i)); 
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 8*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('float: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t1all = t1all & t1 & tb;
//end
//% Now the double version
//t2all = 1;
//for i=2:4:100
//  a = double(randn(i)); 
//  b = double(randn(i)); 
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 8*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('double: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t2all = t2all & t1 & tb;
//end
//% Now the complex version
//t3all = 1;
//for i=2:4:100
//  a = complex(randn(i)+j*randn(i)); 
//  b = complex(randn(i)+j*randn(i)); 
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 8*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('complex: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t3all = t3all & t1 & tb;
//end
//% Now the double version
//t4all = 1;
//for i=2:4:100
//  a = dcomplex(randn(i)+j*randn(i)); 
//  b = dcomplex(randn(i)+j*randn(i)); 
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 8*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('dcomplex: compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t4all = t4all & t1 & tb;
//end
//t = t1all & t2all & t3all & t4all;
//@}
//@{ test_eig5.m
//% Test generalized eigenvalue function - symmetric matrices
//function t = test_eig5
//% First the float version
//t1all = 1;
//for i=2:4:100
//  a = float(randn(i)); a = a + a'; 
//  b = float(randn(i)); b = b*b';
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(abs(eig(b)))*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t1all = t1all & t1 & tb;
//end
//% Now the double version
//t2all = 1;
//for i=2:4:100
//  a = double(randn(i)); a = a + a';
//  b = double(randn(i)); b = b*b';
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(abs(eig(b)))*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t2all = t2all & t1 & tb;
//end
//% Now the complex version
//t3all = 1;
//for i=2:4:100
//  a = complex(randn(i)+j*randn(i)); a = a + a';
//  b = complex(randn(i)+j*randn(i)); b = b*b';
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(abs(eig(b)))*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t3all = t3all & t1 & tb;
//end
//% Now the double version
//t4all = 1;
//for i=2:4:100
//  a = dcomplex(randn(i)+j*randn(i)); a = a + a';
//  b = dcomplex(randn(i)+j*randn(i)); b = b*b';
//  [v,d] = eig(a,b);
//  emat = a*v - b*v*d;
//  er = max(abs(emat(:)));
//  bnd = 10*max(abs(eig(b)))*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(sort(g)-sort(diag(d))));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t4all = t4all & t1 & tb;
//end
//t = t1all & t2all & t3all & t4all;
//@}
//@{ test_eig6.m
//function t = test_eig6
//  try
//    eig([1,2;3,4],eye(1));
//  catch
//  end
//  t = true;
//@}
//@$near#y1=eig(x1)
//@$near#[y1,y2]=eig(x1)
//@$near#y1=eig(x1,'nobalance')
//@$near#[y1,y2]=eig(x1,'nobalance')#(any(loopi==[14,59,64]))
//@@Signature
//function eig EigFunction
//inputs A flag
//outputs V D
//!
ArrayVector EigFunction(int nargout, const ArrayVector& arg) {
  bool balance;
  if (arg.size() == 0)
    throw Exception("eig function requires at least one argument");
  if (arg.size() == 1)
    balance = true;
  else {
    Array b(arg[1]);
    if (b.isString()) {
      QString b2 = b.asString().toUpper();
      if (b2=="NOBALANCE")
	balance = false;
    }
    else
      return GenEigFunction(nargout, arg);
  }
  Array A(arg[0]);
  A.ensureNotScalarEncoded();
  if (!A.is2D())
    throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
  if (AnyNotFinite(A))
    throw Exception("eig only defined for matrices with finite entries.");
  ArrayVector retval;
  if (A.isEmpty()) {
    if (nargout > 1) {
      retval.push_back(A);
      retval.push_back(A);
    } else {
      retval.push_back(Array(Double,NTuple(0,1)));
    }
    return retval;
  }
  
  Array V, D;
  if (nargout > 1) {
    if (IsSymmetric(A))
      EigenDecomposeFullSymmetric(A,V,D);
    else
      EigenDecomposeFullGeneral(A,V,D,balance);
    retval.push_back(V);
    retval.push_back(D);
  } else {
    if (IsSymmetric(A))
      EigenDecomposeCompactSymmetric(A,D);
    else
      EigenDecomposeCompactGeneral(A,D,balance);
    retval.push_back(D);
  }
  return retval;
}


//!
//@Module EIGS Sparse Matrix Eigendecomposition
//@@Section SPARSE
//@@Usage
//Computes the eigendecomsition of a sparse square matrix.  The
//@|eigs| function has several forms.  The most general form is
//@[
//  [V,D] = eigs(A,k,sigma)
//@]
//where @|A| is the matrix to analyze, @|k| is the number of
//eigenvalues to compute and @|sigma| determines which eigenvallues
//to solve for.  Valid values for @|sigma| are
//   'lm' - largest magnitude 
//   'sm' - smallest magnitude
//   'la' - largest algebraic (for real symmetric problems)
//   'sa' - smallest algebraic (for real symmetric problems)
//   'be' - both ends (for real symmetric problems)
//   'lr' - largest real part 
//   'sr' - smallest real part
//   'li' - largest imaginary part
//   'si' - smallest imaginary part
// scalar - find the eigenvalues closest to @|sigma|.
//The returned matrix @|V| contains the eigenvectors, and @|D|
//stores the eigenvalues.  The related form
//@[
//   d = eigs(A,k,sigma)
//@]
//computes only the eigenvalues and not the eigenvectors.  If @|sigma|
//is omitted, as in the forms
//@[
//  [V,D] = eigs(A,k)
//@]
//and
//@[
//  d = eigs(A,k)
//@]
//then @|eigs| returns the largest magnitude eigenvalues (and optionally
//the associated eigenvectors).  As an even simpler form, the forms
//@[
//  [V,D] = eigs(A)
//@]
//and
//@[
//  d = eigs(A)
//@]
//then @|eigs| returns the six largest magnitude eigenvalues of @|A| and
//optionally the eigenvectors.  The @|eigs| function uses ARPACK to
//compute the eigenvectors and/or eigenvalues.  Note that due to a 
//limitation in the interface into ARPACK from FreeMat, the number of
//eigenvalues that are to be computed must be strictly smaller than the
//number of columns (or rows) in the matrix.
//@@Example
//Here is an example of using @|eigs| to calculate eigenvalues
//of a matrix, and a comparison of the results with @|eig|
//@<
//a = sparse(rand(9));
//eigs(a)
//eig(full(a))
//@>
//Next, we exercise some of the variants of @|eigs|:
//@<
//eigs(a,4,'sm')
//eigs(a,4,'lr')
//eigs(a,4,'sr')
//@>
//@@Tests
//@{ test_sparse45.m
//function x = test_sparse45
//a = rand(10);
//A = sparse(a);
//p = eigs(A,4,0.634);
//d = eig(a);
//e = min(abs(d*ones(1,4) - ones(10,1)*p.'));
//x = max(e) < 100*eps;
//@}
//@@Signature
//function eigs EigsFunction
//inputs A k sigma
//outputs V D
//!
ArrayVector EigsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("eigs function requires at least one argument");
  Array A(arg[0]);
  if (!A.isSparse())
    throw Exception("eigs only applies to sparse matrix arguments");
  int k;
  if (!A.isSquare())
    throw Exception("eigs can only be applied to square matrices.");
  if (arg.size() < 2) {
    k = 6;
    if (k >= (int)A.rows())
      k = int(A.rows() - 1);
  } else {
    Array kval(arg[1]);
    k = kval.asInteger();
  }
  if (A.dataClass() != Double) 
    throw Exception("eigs only works on double data class");
  bool shiftFlag;
  QString whichflag;
  double sigma[2];
  if (arg.size() < 3) {
    shiftFlag = false;
    whichflag = "LM";
  } else {
    Array S(arg[2]);
    if (S.isString()) {
      shiftFlag = false;
      QString stxt = S.asString().toUpper();
      if ((stxt == "LM") || (stxt == "SM") || (stxt == "LA") || (stxt == "SA") ||
	  (stxt == "BE") || (stxt == "LR") || (stxt == "SR") || (stxt == "LI") ||
	  (stxt == "SI"))
	whichflag = stxt;
      else
	throw Exception("Unrecognized option for sigma - it must be either 'lm', 'sm', 'la', 'sa', 'be', 'lr', 'sr', 'li', or 'si'");
    } else {
      if (!S.isScalar())
	throw Exception("shift parameter sigma must be a scalar");
      if (S.dataClass() != Double) throw Exception("shift parameter must be a double dataclass");
      if (!S.allReal()) {
	sigma[0] = S.constRealScalar<double>();
	sigma[1] = S.constImagScalar<double>();
      } else {
	sigma[0] = S.constRealScalar<double>();
	sigma[1] = 0;
      }
      shiftFlag = true;
    }
  }
  if (!shiftFlag)
    return SparseEigDecompose(nargout,A,k,whichflag);
  else 
    return SparseEigDecomposeShifted(nargout,A,k,sigma);
}
