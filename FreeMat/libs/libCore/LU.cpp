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

#include "LUDecompose.hpp"
#include "Algorithms.hpp"

//!
//@Module LU LU Decomposition for Matrices
//@@Section TRANSFORMS
//@@Usage
//Computes the LU decomposition for a matrix.  The form of the
//command depends on the type of the argument.  For full (non-sparse)
//matrices, the primary form for @|lu| is
//@[
//   [L,U,P] = lu(A),
//@]
//where @|L| is lower triangular, @|U| is upper triangular, and
//@|P| is a permutation matrix such that @|L*U = P*A|.  The second form is
//@[
//   [V,U] = lu(A),
//@]
//where @|V| is @|P'*L| (a row-permuted lower triangular matrix), 
//and @|U| is upper triangular.  For sparse, square matrices,
//the LU decomposition has the following form:
//@[
//   [L,U,P,Q,R] = lu(A),
//@]
//where @|A| is a sparse matrix of either @|double| or @|dcomplex| type.
//The matrices are such that @|L*U=P*R*A*Q|, where @|L| is a lower triangular
//matrix, @|U| is upper triangular, @|P| and @|Q| are permutation vectors
//and @|R| is a diagonal matrix of row scaling factors.  The decomposition
// is computed using UMFPACK for sparse matrices, and LAPACK for dense
// matrices.
//@@Example
//First, we compute the LU decomposition of a dense matrix.
//@<
//a = float([1,2,3;4,5,8;10,12,3])
//[l,u,p] = lu(a)
//l*u
//p*a
//@>
//Now we repeat the exercise with a sparse matrix, and demonstrate
//the use of the permutation vectors.
//@<
//a = sparse([1,0,0,4;3,2,0,0;0,0,0,1;4,3,2,4])
//[l,u,p,q,r] = lu(a)
//full(l*a)
//b = r*a
//full(b(p,q))
//@>
//@@Tests
//@$near#y1=lu(x1)#(true)
//@$near#[y1,y2]=lu(x1)#(any(loopi==[50:52]))
//@$near#[y1,y2,y3]=lu(x1)#(any(loopi==[50:52]))
//@$near#[y1,y2,y3,y4,y5]=lu(x1)
//@{ test_lu1.m
//% Test the LU decomposition with 2 argument return for full matrices
//function x = test_lu1
//A = float(randn(20));
//[L,U] = lu(A);
//x = testeq(100,L*U,A);
//A = double(randn(20));
//[L,U] = lu(A);
//x = x & testeq(100,L*U,A);
//A = complex(randn(20)+i*randn(20));
//[L,U] = lu(A);
//x = x & testeq(100,L*U,A);
//A = dcomplex(randn(20)+i*randn(20));
//[L,U] = lu(A);
//x = x & testeq(100,L*U,A);
//
//function x = testeq(tmag,a,b)
//  d = full(a)-full(b);
//  if (strcmp(typeof(d),'double') | strcmp(typeof(d),'dcomplex'))
//    x = isempty(find(abs(d)>tmag*eps));
//  else
//    x = isempty(find(abs(d)>tmag*feps));
//  end
//@}
//@{ test_lu2.m
//% Test the LU decomposition with 3 argument return for full matrices
//function x = test_lu2
//A = float(randn(20));
//[L,U,P] = lu(A);
//x = testeq(100,L*U,P*A);
//A = double(randn(20));
//[L,U,P] = lu(A);
//x = x & testeq(100,L*U,P*A);
//A = complex(randn(20)+i*randn(20));
//[L,U,P] = lu(A);
//x = x & testeq(100,L*U,P*A);
//A = dcomplex(randn(20)+i*randn(20));
//[L,U,P] = lu(A);
//x = x & testeq(100,L*U,P*A);
//
//function x = testeq(tmag,a,b)
//  d = full(a)-full(b);
//  if (strcmp(typeof(d),'double') | strcmp(typeof(d),'dcomplex'))
//    x = isempty(find(abs(d)>tmag*eps));
//  else
//    x = isempty(find(abs(d)>tmag*feps));
//  end
//@}
//@{ test_sparse75.m
//% Test sparse matrix array finite test
//function x = test_sparse75
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = 1;
//yi1(150,200) = inf;
//try
//  [l,u] = lu(yi1);
//  x = 0;
//catch
//  x = x & 1;
//end
//try
//  [l,u] = lu(yf1);
//  x = 0;
//catch
//  x = x & 1;
//end
//try
//  [l,u] = lu(yd1);
//  x = 0;
//catch
//  x = x & 1;
//end
//try
//  [l,u] = lu(yc1);
//  x = 0;
//catch
//  x = x & 1;
//end
//try
//  [l,u] = lu(yz1);
//  x = 0;
//catch
//  x = x & 1;
//end
//@}
//@@Signature
//function lu LUFunction
//inputs x
//outputs l u p q r
//!
ArrayVector LUFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("lu function requires at least one argument - the matrix to decompose.");
  Array A(arg[0]);
  if (A.isReferenceType())
    throw Exception("cannot apply lu decomposition to reference types.");
  if (!A.is2D())
    throw Exception("cannot apply matrix operations to N-dimensional arrays");
  if (!A.isSparse()) {
    if (AnyNotFinite(A))
      throw Exception("lu decomposition only defined for matrices with finite entries.");
    return LUDecompose(nargout,A);
  } else {
    return SparseLUDecompose(nargout,A);
  }
  return ArrayVector();
}
