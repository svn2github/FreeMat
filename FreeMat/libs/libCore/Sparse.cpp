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
#include "Array.hpp"
#include "Algorithms.hpp"
#include "Interpreter.hpp"
#include "SparseCCS.hpp"

//!
//@Module SPONES Sparse Ones Function
//@@Section SPARSE
//@@Usage
//Returns a sparse @|float| matrix with ones where the argument
//matrix has nonzero values.  The general syntax for it is
//@[
//  y = spones(x)
//@]
//where @|x| is a matrix (it may be full or sparse).  The output
//matrix @|y| is the same size as @|x|, has type @|float|, and contains
//ones in the nonzero positions of @|x|.
//@@Examples
//Here are some examples of the @|spones| function
//@<
//a = [1,0,3,0,5;0,0,2,3,0;1,0,0,0,1]
//b = spones(a)
//full(b)
//@>
//@@Signature
//function spones SponesFunction
//inputs x
//outputs y
//!

template <typename T>
static Array SponesSparse(const SparseMatrix<T> &A) {
  ConstSparseIterator<T> iter(&A);
  SparseMatrix<double> ret(NTuple(A.rows(),A.cols()));
  while (iter.isValid()) {
    ret[iter.pos()] = 1;
    iter.next();
  }
  return Array(ret);
}

template <typename T>
static Array SponesDense(const BasicArray<T> &A) {
  ConstBasicIterator<T> iter(&A,0);
  SparseMatrix<double> ret(NTuple(A.rows(),A.cols()));
  while (iter.isValid()) {
    for (index_t i=1;i<=iter.size();i++) {
      ret[iter.pos()] = 1.0;
      iter.next();
    }
    iter.nextSlice();
  }
  return Array(ret);
}

template <typename T>
static Array Spones(const Array &A) {
  if (A.isSparse())
    return SponesSparse(A.constRealSparse<T>());
  else {
    const Array &Adense(A.asDenseArray());
    return SponesDense(Adense.constReal<T>());
  }
}

#define MacroSpones(ctype,cls)				\
  case cls: return ArrayVector(Spones<ctype>(arg[0]));

ArrayVector SponesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("spones function requires a sparse matrix template argument");
  if (arg[0].isEmpty())
    return ArrayVector(EmptyConstructor());
  if(arg[0].isReferenceType())
    throw Exception("spones function requires a numeric sparse matrix argument");
  switch (arg[0].dataClass()) {
  default: throw Exception("Unhandled type for spones");
    MacroExpandCasesSimple(MacroSpones);
  }
}

//!
//@Module SPARSE Construct a Sparse Matrix
//@@Section SPARSE
//@@Usage
//Creates a sparse matrix using one of several formats.  The 
//first creates a sparse matrix from a full matrix
//@[
//   y = sparse(x).
//@]
//The second form creates a sparse matrix containing all zeros
//that is of the specified size (the sparse equivalent of
//@|zeros|).
//@[
//   y = sparse(m,n)
//@]
//where @|m| and @|n| are integers.  Just like the @|zeros| function,
//the sparse matrix returned is of type @|float|.  The third form 
//constructs a sparse matrix from the IJV syntax.  It has two forms.  The
//first version autosizes the sparse matrix 
//@[
//   y = sparse(i,j,v)
//@]
//while the second version uses an explicit size specification
//@[
//   y = sparse(i,j,v,m,n)
//@]
//@@Tests
//@{ test_sparse1.m
//function x = test_sparse1
//a = [0,0,2,0;0,0,1,1;1,0,0,0];
//A = sparse(a);
//b = float(a);
//B = sparse(b);
//c = double(a);
//C = sparse(c);
//d = complex(a + i*a);
//D = sparse(d);
//e = dcomplex(d);
//E = sparse(e);
//f = complex(a);
//F = complex(A);
//g = dcomplex(b);
//G = dcomplex(B);
//h = float(d);
//H = float(D);
//x = testeq(a,A) & testeq(b,B) & testeq(c,C) & testeq(d,D) & testeq(e,E) & testeq(f,F) & testeq(g,G) & testeq(h,H);
//@}
//@{ test_sparse2.m
//function x = test_sparse2
//a = [0,0,2,0;0,0,1,1;1,0,0,0];
//b = [4,5,0,3];
//c = [0,0];
//d = [2,3];
//A = sparse(a);
//B = sparse(b);
//C = sparse(c);
//D = sparse(d);
//j = [a;c,d;b];
//J = [A;C,D;B];
//x = testeq(j,J);
//@}
//@{ test_sparse3.m
//function x = test_sparse3
//a = float([0,0,2,0;0,0,1,1;1,0,0,0]);
//b = float([4,5,0,3]);
//c = [0,0];
//d = [2,3];
//A = sparse(a);
//B = sparse(b);
//C = sparse(c);
//D = sparse(d);
//j = [a;c,d;b];
//J = [A;C,D;B];
//x = testeq(j,J);
//@}
//@{ test_sparse4.m
//function x = test_sparse4
//a = double([0,0,2,0;0,0,1,1;1,0,0,0]);
//b = [4,5,0,3];
//c = double([0,0]);
//d = [2,3];
//A = sparse(a);
//B = sparse(b);
//C = sparse(c);
//D = sparse(d);
//j = [a;c,d;b];
//J = [A;C,D;B];
//x = testeq(j,J);
//@}
//@{ test_sparse5.m
//function x = test_sparse5
//a = complex([0,0,2,0;0,0,1,1;1,0,0,0] + i*[3,0,3,0;0,0,1,0;0,3,5,2]);
//b = float([4,5,0,3]);
//c = float([0,0]);
//d = float([2+5*i,3]);
//A = sparse(a);
//B = sparse(b);
//C = sparse(c);
//D = sparse(d);
//j = [a;c,d;b];
//J = [A;C,D;B];
//x = testeq(j,J);
//@}
//@{ test_sparse6.m
//function x = test_sparse6
//a = dcomplex([0,0,2,0;0,0,1,1;1,0,0,0] + i*[3,0,3,0;0,0,1,0;0,3,5,2]);
//b = float([4,5,0,3]);
//c = double([0,0]);
//d = float([2+5*i,3]);
//A = sparse(a);
//B = sparse(b);
//C = sparse(c);
//D = sparse(d);
//j = [a;c,d;b];
//J = [A;C,D;B];
//x = testeq(j,J);
//@}
//@{ test_sparse7.m
//function x = test_sparse7
//a = [0,0,2,0,0;0,1,0,1,0;0,0,0,1,3];
//A = sparse(a);
//b = [1,4,5;2,5,15;3,6,11];
//C = A(b);
//c = a(b);
//x = testeq(c,C);
//@}
//@{ test_sparse8.m
//function x = test_sparse8
//ar = [0,0,2,0,0;0,1,0,1,0;0,0,0,1,0];
//ai = [1,0,3,0,0;0,2,3,-1,3;0,1,0,4,3];
//a = complex(ar+i*ai);
//A = sparse(a);
//b = [1,4,5;2,5,15;3,6,11];
//C = A(b);
//c = a(b);
//x = testeq(c,C);
//@}
//@{ test_sparse9.m
//function x = test_sparse9
//ar = [0,0,2,0,0;0,1,0,1,0;0,0,0,1,0];
//ai = [1,0,3,0,0;0,2,3,-1,3;0,1,0,4,3];
//a = dcomplex(ar+i*ai);
//A = sparse(a);
//b = [1,4,5;2,5,15;3,6,11];
//C = A(b);
//c = a(b);
//x = testeq(c,C);
//@}
//@{ test_sparse10.m
//function x = test_sparse10
//a = int32(10*rand(8));
//a(a<7) = 0;
//A = sparse(a);
//b = [5;2;3;3;1];
//c = [1,4,6,8];
//A = sparse(a);
//C = A(b,c);
//c = a(b,c);
//x = testeq(c,C);
//@}
//@{ test_sparse11.m
//function x = test_sparse11
//a = float(10*rand(8));
//a(a<7) = 0;
//A = sparse(a);
//b = [5;2;3;3;1];
//c = [1,4,6,8];
//A = sparse(a);
//C = A(b,c);
//c = a(b,c);
//x = testeq(c,C);
//@}
//@{ test_sparse12.m
//function x = test_sparse12
//a = double(10*rand(8));
//a(a<7) = 0;
//A = sparse(a);
//b = [5;2;3;3;1];
//c = [1,4,6,8];
//A = sparse(a);
//C = A(b,c);
//c = a(b,c);
//x = testeq(c,C);
//@}
//@{ test_sparse13.m
//function x = test_sparse13
//ai = int32(10*rand(8));
//ar = int32(10*rand(8));
//ai(ai<7) = 0;
//ar(ar<7) = 0;
//a = complex(ar+i*ai);
//A = sparse(a);
//b = [5;2;3;3;1];
//d = [1,4,6,8];
//A = sparse(a);
//C = A(b,d);
//c = a(b,d);
//x = testeq(c,C);
//@}
//@{ test_sparse14.m
//function x = test_sparse14
//ai = int32(10*rand(8));
//ar = int32(10*rand(8));
//ai(ai<7) = 0;
//ar(ar<7) = 0;
//a = dcomplex(ar+i*ai);
//A = sparse(a);
//b = [5;2;3;3;1];
//c = [1,4,6,8];
//A = sparse(a);
//C = A(b,c);
//c = a(b,c);
//x = testeq(c,C);
//@}
//@{ test_sparse15.m
//function x = test_sparse15
//a = [1,2,0,0,4;3,2,0,0,5;0,0,3,0,2];
//A = sparse(a);
//c = a(2,5);
//C = A(2,5);
//x = testeq(c,C);
//@}
//@{ test_sparse16.m
//function x = test_sparse16
//a = float([1,2,0,0,4;3,2,0,0,5;0,0,3,0,2]);
//A = sparse(a);
//c = a(2,5);
//C = A(2,5);
//x = testeq(c,C);
//@}
//@{ test_sparse17.m
//function x = test_sparse17
//a = double([1,2,0,0,4;3,2,0,0,5;0,0,3,0,2]);
//A = sparse(a);
//c = a(2,5);
//C = A(2,5);
//x = testeq(c,C);
//@}
//@{ test_sparse18.m
//function x = test_sparse18
//ai = [0,2,6,0,1;3,0,3,0,2;0,0,3,0,2];
//ar = [1,2,0,0,4;3,2,0,0,5;0,0,3,0,2];
//a = complex(ar+i*ai);
//A = sparse(a);
//c = a(2,5);
//C = A(2,5);
//x = testeq(c,C);
//@}
//@{ test_sparse19.m
//function x = test_sparse19
//ai = [0,2,6,0,1;3,0,3,0,2;0,0,3,0,2];
//ar = [1,2,0,0,4;3,2,0,0,5;0,0,3,0,2];
//a = dcomplex(ar+i*ai);
//A = sparse(a);
//c = a(2,5);
//C = A(2,5);
//x = testeq(c,C);
//@}
//@{ test_sparse22.m
//function x = test_sparse22
//a = randn(1000);
//A = sparse(a);
//rc = int32(999*rand(7000,1))+1;
//cc = int32(999*rand(7000,1))+1;
//lc = rc+1000*(cc-1);
//b = a(lc);
//B = A(lc);
//x = testeq(b,B);
//@}
//@{ test_sparse23.m
//function x = test_sparse23
//N = 1000;
//M = 7000;
//a = double(40*randn(N));
//A = sparse(a);
//rc = int32((N-1)*rand(M,1))+1;
//cc = int32((N-1)*rand(M,1))+1;
//lc = rc+N*(cc-1);
//b = a(lc);
//B = A(lc);
//x = testeq(b,B);
//@}
//@{ test_sparse24.m
//function x = test_sparse24
//N = 10;
//M = 70;
//a = double(40*randn(N));
//A = sparse(a);
//rc = int32((N-1)*rand(M,1))+1;
//cc = int32((N-1)*rand(M,1))+1;
//lc = rc+N*(cc-1);
//b = a(lc);
//B = A(lc);
//x = testeq(b,B);
//@}
//@{ test_sparse25.m
//function x = test_sparse25
//N = 1000;
//M = 7000;
//a = complex(40*randn(N)+32*randn(N));
//A = sparse(a);
//rc = int32((N-1)*rand(M,1))+1;
//cc = int32((N-1)*rand(M,1))+1;
//lc = rc+N*(cc-1);
//b = a(lc);
//B = A(lc);
//x = testeq(b,B);
//@}
//@{ test_sparse26.m
//function x = test_sparse26
//N = 1000;
//M = 7000;
//a = dcomplex(40*randn(N)+32*randn(N));
//A = sparse(a);
//rc = int32((N-1)*rand(M,1))+1;
//cc = int32((N-1)*rand(M,1))+1;
//lc = rc+N*(cc-1);
//b = a(lc);
//B = A(lc);
//x = testeq(b,B);
//@}
//@{ test_sparse27.m
//function x = test_sparse27
//a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2];
//A = sparse(a);
//p = [3;4;5;9;10];
//a(p) = 7;
//A(p) = 7;
//x = testeq(a,A);
//@}
//@{ test_sparse28.m
//function x = test_sparse28
//a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2];
//A = sparse(a);
//p = [3;4;5;9;10];
//a(p) = [4,6,8,0,3];
//A(p) = [4,6,8,0,3];
//x = testeq(a,A);
//@}
//@{ test_sparse29.m
//function x = test_sparse29
//a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2];
//A = sparse(a);
//p = [3;4;5;9;10;20];
//a(p) = [4,6,8,0,3,7];
//A(p) = [4,6,8,0,3,7];
//x = testeq(a,A);
//@}
//@{ test_sparse30.m
//function x = test_sparse30
//ar = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2];
//ai = [0,0,2,0,4;1,0,2,3,0;1,0,0,0,3];
//a = complex(ar+ai*i);
//A = sparse(a);
//p = [3;4;5;9;10];
//a(p) = complex(7+2*i);
//A(p) = complex(7+2*i);
//x = testeq(a,A);
//@}
//@{ test_sparse31.m
//function x = test_sparse31
//a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2;0,4,5,0,3];
//A = sparse(a);
//b = [3;2];
//c = [2,4,3];
//d = [9,3,2,4,9,0];
//a(b,c) = d;
//A(b,c) = d;
//x = testeq(a,A);
//@}
//@{ test_sparse32.m
//function x = test_sparse32
//a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2;0,4,5,0,3];
//A = sparse(a);
//b = [3;2;5];
//c = [2,4,3];
//d = [9,3,2,4,9,0,5,4,2];
//a(b,c) = d;
//A(b,c) = d;
//x = testeq(a,A);
//@}
//@{ test_sparse33.m
//function x = test_sparse33
//ar = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2;0,4,5,0,3];
//ai = [0,2,9,0,2;0,0,0,2,0;1,3,4,0,2;1,0,0,0,2];
//a = complex(ar+ai);
//A = sparse(a);
//b = [3;2;5];
//c = [2,4,3];
//d = complex([9,3,2,4,9,0,5,4,2] + 3*i);
//a(b,c) = d;
//A(b,c) = d;
//x = testeq(a,A);
//@}
//@{ test_sparse34.m
//function x = test_sparse34
//a = sparse([0;0;2;3;0;0;4;5;0;0;0;2]);
//n = [7,9,1,3,2,3,1,2,2];
//b = full(a);
//a(n) = [];
//b(n) = [];
//x = testeq(a,b);
//@}
//@{ test_sparse35.m
//function x = test_sparse35
//s = [0,3,4,5;0,0,0,0;0,2,3,0];
//a = sparse(s);
//x = testeq(full(a),s) & testeq(full(float(a)),s) & testeq(full(double(a)),s) & testeq(full(complex(a)),s) & testeq(full(dcomplex(a)),s);
//@}
//@{ test_sparse36.m
//function x = test_sparse36
//a = rand(8) + i*rand(8);
//A = sparse(a);
//B = float(A);
//b = full(B);
//x = testeq(b,B);
//@}
//@{ sparse_test_mat.m
//function [y,z] = sparse_test_mat(typ, rows, cols)
//  if (~exist('cols')) cols = rows; end
//  a = rand(rows,cols);
//  a(a>0.1) = 0;
//  b = rand(rows,cols);
//  b(b>0.1) = 0;
//  switch(typ)
//    case 'int32'
//      z = (int32(100*a));
//    case 'float'
//      z = (float(a));
//    case 'double'
//      z = (double(a));
//    case 'complex'
//      z = (float(a) + i*float(b));
//    case 'dcomplex'
//      z = (a + i*b);
//  end
//  y = sparse(z);
//@}
//@{ test_sparse62.m
//% Test sparse matrix array constructor
//function x = test_sparse62
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',300,450);
//[yf2,zf2] = sparse_test_mat('float',300,450);
//[yd2,zd2] = sparse_test_mat('double',300,450);
//[yc2,zc2] = sparse_test_mat('complex',300,450);
//[yz2,zz2] = sparse_test_mat('dcomplex',300,450);
//[yi3,zi3] = sparse_test_mat('int32',30,400);
//[yf3,zf3] = sparse_test_mat('float',30,400);
//[yd3,zd3] = sparse_test_mat('double',30,400);
//[yc3,zc3] = sparse_test_mat('complex',30,400);
//[yz3,zz3] = sparse_test_mat('dcomplex',30,400);
//[yi4,zi4] = sparse_test_mat('int32',30,450);
//[yf4,zf4] = sparse_test_mat('float',30,450);
//[yd4,zd4] = sparse_test_mat('double',30,450);
//[yc4,zc4] = sparse_test_mat('complex',30,450);
//[yz4,zz4] = sparse_test_mat('dcomplex',30,450);
//a1 = [yi1,yi2;yi3,yi4];
//b1 = [zi1,zi2;zi3,zi4];
//a2 = [yf1,yf2;yf3,yf4];
//b2 = [zf1,zf2;zf3,zf4];
//a3 = [yd1,yd2;yd3,yd4];
//b3 = [zd1,zd2;zd3,zd4];
//a4 = [yc1,yc2;yc3,yc4];
//b4 = [zc1,zc2;zc3,zc4];
//a5 = [yz1,yz2;yz3,yz4];
//b5 = [zz1,zz2;zz3,zz4];
//x = testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
//@}
//@@Signature
//sfunction sparse SparseFunction
//inputs i j v m n
//outputs y
//!
ArrayVector SparseFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 1) 
    return ArrayVector(ToSparse(arg[0]));
  else if (arg.size() == 2) 
    return ArrayVector(Array(SparseMatrix<double>(NTuple(arg[0].asInteger(),
							 arg[1].asInteger()))));
  else if (arg.size() == 3) {
    const Array &ip_typed(arg[0].asDenseArray().toClass(Index));
    const Array &jp_typed(arg[1].asDenseArray().toClass(Index));
    const BasicArray<index_t> &ip(ip_typed.constReal<index_t>());
    const BasicArray<index_t> &jp(jp_typed.constReal<index_t>());
    return ArrayVector(IJVToSparse(ip,jp,arg[2]));
  } else if (arg.size() >= 5) {
    if (arg.size() > 5)
      eval->warningMessage("extra arguments to sparse (nnz to reserve) ignored");
    const Array &ip_typed(arg[0].asDenseArray().toClass(Index));
    const Array &jp_typed(arg[1].asDenseArray().toClass(Index));
    const BasicArray<index_t> &ip(ip_typed.constReal<index_t>());
    const BasicArray<index_t> &jp(jp_typed.constReal<index_t>());
    return ArrayVector(IJVToSparse(ip,jp,arg[2],arg[3].asDouble(),arg[4].asDouble()));
  }
  return ArrayVector();
}

//!
//@Module FULL Convert Sparse Matrix to Full Matrix
//@@Section SPARSE
//@@Usage
//Converts a sparse matrix to a full matrix.  The syntax for
//its use is
//@[
//   y = full(x)
//@]
//The type of @|x| is preserved.  Be careful with the function.
//As a general rule of thumb, if you can work with the @|full|
//representation of a function, you probably do not need the
//sparse representation.
//@@Example
//Here we convert a full matrix to a sparse one, and back again.
//@<
//a = [1,0,4,2,0;0,0,0,0,0;0,1,0,0,2]
//A = sparse(a)
//full(A)
//@>
//@@Tests
//@{ test_sparse57.m
//% Test makeDenseArray function
//function x = test_sparse57
//[yi,zi] = sparse_test_mat('int32',400);
//[yf,zf] = sparse_test_mat('float',400);
//[yd,zd] = sparse_test_mat('double',400);
//[yc,zc] = sparse_test_mat('complex',400);
//[yz,zz] = sparse_test_mat('dcomplex',400);
//x = testeq(full(yi),zi) & testeq(full(yf),zf) & testeq(full(yd),zd) & testeq(full(yc),zc) & testeq(full(yz),zz);
//@}
//@@Signature
//function full FullFunction
//inputs x
//outputs y
//!
ArrayVector FullFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Need one argument to full function");
  return ArrayVector(arg[0].asDenseArray());
}
