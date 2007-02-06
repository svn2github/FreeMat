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

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif
#include <math.h>
#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Math.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "SingularValueDecompose.hpp"
#include "QRDecompose.hpp"
#include "System.hpp"
#include "Sparse.hpp"
#include "LUDecompose.hpp"
#include "Print.hpp"

#include <algorithm>
#undef max
#undef min

//!
//@Module DISP Display a Variable or Expression
//@@Section IO
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
ArrayVector DispFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  int length;
  Array C;
  ArrayVector retval;

  length = arg.size();
  for (int i=0;i<length;i++) {
    C = arg[i];
    PrintArrayClassic(C,eval->getPrintLimit(),eval,false);
  }
  //    retval.push_back(C);
  return retval;
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
//x = testeq(a',A);
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
//!
ArrayVector SparseFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 1) {
    Array r(arg[0]);
    if ((r.dataClass() != FM_LOGICAL) && (r.dataClass() < FM_INT32))
      r.promoteType(FM_INT32);
    r.makeSparse();
    return singleArrayVector(r);
  } else if (arg.size() == 2) {
    Array m_arg(arg[0]);
    Array n_arg(arg[1]);
    int m, n;
    m = m_arg.getContentsAsIntegerScalar();
    n = n_arg.getContentsAsIntegerScalar();
    Dimensions dim(m,n);
    return singleArrayVector(Array(FM_FLOAT,dim,SparseFloatZeros(m,n),true));
  } else if (arg.size() == 3) {
    Array i_arg(arg[0]);
    Array j_arg(arg[1]);
    Array v_arg(arg[2]);
    i_arg.promoteType(FM_UINT32);
    j_arg.promoteType(FM_UINT32);
    if (v_arg.dataClass() != FM_LOGICAL && v_arg.dataClass() < FM_INT32)
      v_arg.promoteType(FM_INT32);
    int ilen, jlen, vlen;
    ilen = i_arg.getLength();
    jlen = j_arg.getLength();
    vlen = v_arg.getLength();
    int olen;
    olen = ilen > jlen ? ilen : jlen;
    olen = vlen > olen ? vlen : olen;
    int istride, jstride, vstride;
    if (olen > 1) {
      if (ilen == 1)
	istride = 0;
      else if (ilen == olen)
	istride = 1;
      else
	throw Exception("in I, J, V format, all three vectors must be the same size or be scalars");
      if (jlen == 1)
	jstride = 0;
      else if (jlen == olen)
	jstride = 1;
      else
	throw Exception("in I, J, V format, all three vectors must be the same size or be scalars");
      if (vlen == 1)
	vstride = 0;
      else if (vlen == olen)
	vstride = 1;
      else
	throw Exception("in I, J, V format, all three vectors must be the same size or be scalars");
    }
    // Calculate the number of rows in the matrix
    uint32 *ip = (uint32*) i_arg.getDataPointer();
    int rows = 0;
    for (int i=0;i<ilen;i++)
      rows = (ip[i] > rows) ? ip[i] : rows;
    uint32 *jp = (uint32*) j_arg.getDataPointer();
    int cols = 0;
    for (int j=0;j<jlen;j++)
      cols = (jp[j] > cols) ? jp[j] : cols;
    Dimensions dim(rows,cols);
    return singleArrayVector(Array(v_arg.dataClass(),dim,
				   makeSparseFromIJV(v_arg.dataClass(),
						     rows,cols,olen,
						     ip,istride,jp,jstride,
						     v_arg.getDataPointer(),
						     vstride),
				   true));
  } else if (arg.size() >= 5) {
    if (arg.size() > 5)
      eval->warningMessage("extra arguments to sparse (nnz to reserve) ignored");
    Array i_arg(arg[0]);
    Array j_arg(arg[1]);
    Array v_arg(arg[2]);
    i_arg.promoteType(FM_UINT32);
    j_arg.promoteType(FM_UINT32);
    if (v_arg.dataClass() != FM_LOGICAL && v_arg.dataClass() < FM_INT32)
      v_arg.promoteType(FM_INT32);
    int ilen, jlen, vlen;
    ilen = i_arg.getLength();
    jlen = j_arg.getLength();
    vlen = v_arg.getLength();
    int olen;
    olen = ilen > jlen ? ilen : jlen;
    olen = vlen > olen ? vlen : olen;
    int istride, jstride, vstride;
    if (olen > 1) {
      if (ilen == 1)
	istride = 0;
      else if (ilen == olen)
	istride = 1;
      else
	throw Exception("in I, J, V format, all three vectors must be the same size or be scalars");
      if (jlen == 1)
	jstride = 0;
      else if (jlen == olen)
	jstride = 1;
      else
	throw Exception("in I, J, V format, all three vectors must be the same size or be scalars");
      if (vlen == 1)
	vstride = 0;
      else if (vlen == olen)
	vstride = 1;
      else
	throw Exception("in I, J, V format, all three vectors must be the same size or be scalars");
    }
    Array m_arg(arg[3]);
    Array n_arg(arg[4]);
    int rows = m_arg.getContentsAsIntegerScalar();
    int cols = n_arg.getContentsAsIntegerScalar();
    Dimensions dim(rows,cols);
    uint32 *ip = (uint32*) i_arg.getDataPointer();
    uint32 *jp = (uint32*) j_arg.getDataPointer();
    return singleArrayVector(Array(v_arg.dataClass(),dim,
				   makeSparseFromIJV(v_arg.dataClass(),
						     rows,cols,olen,
						     ip,istride,jp,jstride,
						     v_arg.getDataPointer(),
						     vstride),
				   true));
  }
  throw Exception("unrecognized form of sparse - see help for the allowed forms of sparse");
}

//!
//@Module INV Invert Matrix
//@@Section TRANSFORMS
//@@Usage
//Inverts the argument matrix, provided it is square and invertible.
//The syntax for its use is
//@[
//   y = inv(x)
//@]
//Internally, the @|inv| function uses the matrix divide operators.
//For sparse matrices, a sparse matrix solver is used.
//@@Example
//Here we invert some simple matrices
//@<
//a = randi(zeros(3),5*ones(3))
//b = inv(a)
//a*b
//b*a
//@>
//!
ArrayVector InvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("inv function needs at least one argument");
  Array r(arg[0]);
  return singleArrayVector(InvertMatrix(r));
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
//!
ArrayVector FullFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Need one argument to full function");
  Array r(arg[0]);
  r.makeDense();
  return singleArrayVector(r);
}

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
//@{ test_lu1.m
//% Test the LU decomposition with 2 argument return for full matrices
//function x = test_lu1
//A = float(randn(20));
//[L,U] = lu(A);
//x = testeq(20,L*U,A);
//A = double(randn(20));
//[L,U] = lu(A);
//x = x & testeq(20,L*U,A);
//A = complex(randn(20)+i*randn(20));
//[L,U] = lu(A);
//x = x & testeq(20,L*U,A);
//A = dcomplex(randn(20)+i*randn(20));
//[L,U] = lu(A);
//x = x & testeq(20,L*U,A);
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
//x = testeq(20,L*U,P*A);
//A = double(randn(20));
//[L,U,P] = lu(A);
//x = x & testeq(20,L*U,P*A);
//A = complex(randn(20)+i*randn(20));
//[L,U,P] = lu(A);
//x = x & testeq(20,L*U,P*A);
//A = dcomplex(randn(20)+i*randn(20));
//[L,U,P] = lu(A);
//x = x & testeq(20,L*U,P*A);
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
//[yi1,zi1] = test_sparse_mat('int32',300,400);
//[yf1,zf1] = test_sparse_mat('float',300,400);
//[yd1,zd1] = test_sparse_mat('double',300,400);
//[yc1,zc1] = test_sparse_mat('complex',300,400);
//[yz1,zz1] = test_sparse_mat('dcomplex',300,400);
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
//!
ArrayVector LUFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("lu function requires at least one argument - the matrix to decompose.");
  Array A(arg[0]);
  if (A.isReferenceType())
    throw Exception("cannot apply lu decomposition to reference types.");
  if (!A.is2D())
    throw Exception("cannot apply matrix operations to N-dimensional arrays");
  if (!A.sparse()) {
    if (A.anyNotFinite())
      throw Exception("lu decomposition only defined for matrices with finite entries.");
    return LUDecompose(nargout,A);
  } else {
    return SparseLUDecompose(nargout,A);
  }
  return ArrayVector();
}

//!
//@Module GETLINE Get a Line of Input from User
//@@Section IO
//@@Usage
//Reads a line (as a string) from the user.  This function has
//two syntaxes.  The first is 
//@[
//  a = getline(prompt)
//@]
//where @|prompt| is a prompt supplied to the user for the query.
//The second syntax omits the @|prompt| argument:
//@[
//  a = getline
//@]
//Note that this function requires command line input, i.e., it 
//will only operate correctly for programs or scripts written
//to run inside the FreeMat GUI environment or from the X11 terminal.
//If you build a stand-alone application and expect it to operate 
//cross-platform, do not use this function (unless you include
//the FreeMat console in the final application).
//!
ArrayVector GetLineFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  char *prompt;
  if (arg.size() < 1)
    prompt = "";
  else {
    Array A(arg[0]);
    if (!A.isString())
      throw Exception("getline requires a string prompt");
    prompt = A.getContentsAsCString();
  }
  return singleArrayVector(Array::stringConstructor(eval->getLine(prompt)));
}

ArrayVector GenEigFunction(int nargout, const ArrayVector &arg) {
  Array A(arg[0]);
  Array B(arg[1]);
  if (!A.is2D() || !B.is2D())
    throw Exception("cannot apply matrix operations to N-dimensional arrays.");
  if (A.sparse() || B.sparse())
    throw Exception("eig only defined for full matrices.");

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
//  bnd = 4*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a);
//  e2 = max(abs(g-diag(d)));
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
//  e2 = max(abs(g-diag(d)));
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
//  e2 = max(abs(g-diag(d)));
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
//  e2 = max(abs(g-diag(d)));
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
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(abs(eig(b)))*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(abs(eig(b)))*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(abs(eig(b)))*max(diag(abs(d)))*feps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(g-diag(d)));
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
//  bnd = 4*max(abs(eig(b)))*max(diag(abs(d)))*eps*i;
//  t1 = (er < bnd);
//  if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',er,bnd,i); end
//  g = eig(a,b);
//  e2 = max(abs(g-diag(d)));
//  tb = e2<bnd;
//  if (~tb) printf('compact/full decomp mismatch: er = %e (num = %d)\n',e2,i); end
//  t4all = t4all & t1 & tb;
//end
//t = t1all & t2all & t3all & t4all;
//@}
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
//a = sparse(rand(9))
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
//!
ArrayVector EigsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("eigs function requires at least one argument");
  Array A(arg[0]);
  if (!A.sparse())
    throw Exception("eigs only applies to sparse matrix arguments");
  int k;
  if (A.getDimensionLength(0) != A.getDimensionLength(1))
    throw Exception("eigs can only be applied to square matrices.");
  if (arg.size() < 2) {
    k = 6;
    if (k >= A.getDimensionLength(0))
      k = A.getDimensionLength(0) - 1;
  } else {
    Array kval(arg[1]);
    k = kval.getContentsAsIntegerScalar();
  }
  if (A.isComplex())
    A.promoteType(FM_DCOMPLEX);
  else
    A.promoteType(FM_DOUBLE);
  bool shiftFlag;
  char *whichflag;
  double sigma[2];
  if (arg.size() < 3) {
    shiftFlag = false;
    whichflag = "LM";
  } else {
    Array S(arg[2]);
    if (S.isString()) {
      shiftFlag = false;
      char *stxt = S.getContentsAsCString();
      if ((strcmp(stxt,"LM")==0) ||
	  (strcmp(stxt,"lm")==0) ||
	  (strcmp(stxt,"Lm")==0) ||
	  (strcmp(stxt,"lM")==0)) whichflag = "LM";
      else if ((strcmp(stxt,"SM")==0) ||
	       (strcmp(stxt,"sm")==0) ||
	       (strcmp(stxt,"Sm")==0) ||
	       (strcmp(stxt,"sm")==0)) whichflag = "SM";
      else if ((strcmp(stxt,"LA")==0) ||
	       (strcmp(stxt,"la")==0) ||
	       (strcmp(stxt,"La")==0) ||
	       (strcmp(stxt,"la")==0)) whichflag = "LA";
      else if ((strcmp(stxt,"SA")==0) ||
	       (strcmp(stxt,"sa")==0) ||
	       (strcmp(stxt,"Sa")==0) ||
	       (strcmp(stxt,"sa")==0)) whichflag = "SA";
      else if ((strcmp(stxt,"BE")==0) ||
	       (strcmp(stxt,"be")==0) ||
	       (strcmp(stxt,"Be")==0) ||
	       (strcmp(stxt,"be")==0)) whichflag = "BE";
      else if ((strcmp(stxt,"LR")==0) ||
	       (strcmp(stxt,"lr")==0) ||
	       (strcmp(stxt,"Lr")==0) ||
	       (strcmp(stxt,"lr")==0)) whichflag = "LR";
      else if ((strcmp(stxt,"SR")==0) ||
	       (strcmp(stxt,"sr")==0) ||
	       (strcmp(stxt,"Sr")==0) ||
	       (strcmp(stxt,"sr")==0)) whichflag = "SR";
      else if ((strcmp(stxt,"LI")==0) ||
	       (strcmp(stxt,"li")==0) ||
	       (strcmp(stxt,"Li")==0) ||
	       (strcmp(stxt,"li")==0)) whichflag = "LI";
      else if ((strcmp(stxt,"SI")==0) ||
	       (strcmp(stxt,"si")==0) ||
	       (strcmp(stxt,"Si")==0) ||
	       (strcmp(stxt,"si")==0)) whichflag = "SI";
      else
	throw Exception("Unrecognized option for sigma - it must be either 'lm', 'sm', 'la', 'sa', 'be', 'lr', 'sr', 'li', or 'si'");
    } else {
      if (!S.isScalar())
	throw Exception("shift parameter sigma must be a scalar");
      if (S.isComplex()) {
	S.promoteType(FM_DCOMPLEX);
	double *cp = (double*) S.getDataPointer();
	sigma[0] = cp[0];
	sigma[1] = cp[1];
      } else {
	sigma[0] = S.getContentsAsDoubleScalar();
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

  

ArrayVector QRDNoPivotFunction(bool compactDec, Array A) {
  int nrows = A.getDimensionLength(0);
  int ncols = A.getDimensionLength(1);
  int orows = nrows;
  int ocols = ncols;
  if ((!compactDec) && (nrows > ncols)) {
    Dimensions newDim(nrows,nrows);
    A.resize(newDim);
    ncols = nrows;
  } else 
    compactDec = true;
  Class Aclass(A.dataClass());
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_FLOAT,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_FLOAT,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DOUBLE,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DOUBLE,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_COMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_COMPLEX,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DCOMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DCOMPLEX,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
    Dimensions newDim(nrows,nrows);
    A.resize(newDim);
    ncols = nrows;
  } else 
    compactDec = true;
  Class Aclass(A.dataClass());
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_FLOAT,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_FLOAT,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DOUBLE,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DOUBLE,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_COMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_COMPLEX,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DCOMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DCOMPLEX,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
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
//@@Section TRANSFORMS
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
//@@Tests
//@{ test_qr1.m
//% Test the QR decomposition (without pivoting, minimal decomposition, double)
//function test_val = test_qr1
//a = [1,2,3;4,5,6;7,8,9;10,0,5];
//% double precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr2.m
//% Test the QR decomposition (without pivoting, minimal decomposition, float)
//function test_val = test_qr2
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//% float precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr3.m
//% Test the QR decomposition (without pivoting, minimal decomposition, complex)
//function test_val = test_qr3
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr4.m
//% Test the QR decomposition (without pivoting, minimal decomposition, dcomplex)
//function test_val = test_qr4
//a = double([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr5.m
//% Test the QR decomposition (without pivoting, double)
//function test_val = test_qr5
//a = [1,2,3;4,5,6;7,8,9;10,0,5];
//% double precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr6.m
//% Test the QR decomposition (without pivoting, float)
//function test_val = test_qr6
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//% float precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr7.m
//% Test the QR decomposition (without pivoting, complex)
//function test_val = test_qr7
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr8.m
//% Test the QR decomposition (without pivoting, dcomplex)
//function test_val = test_qr8
//a = double([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr9.m
//% Test the QR decomposition (full, without pivoting)
//function test_val = test_qr9
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(n,2*n)+i*rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(n,2*n)+i*rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
//@{ test_qr10.m
//% Test the QR decomposition (compact, without pivoting)
//function test_val = test_qr10
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
//@{ test_qr11.m
//% Test the QR decomposition (compact, pivoting)
//function test_val = test_qr11
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 4*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
//@{ test_qr12.m
//% Test the QR decomposition (full, pivoting)
//function test_val = test_qr12
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 2*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 2*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(n,n)+i*rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 2*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(n,n)+i*rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 2*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
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
//@@Section TRANSFORMS
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
//@@Tests
//@{ test_svd1.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd1
//  a = [1,3;4,2];
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd2.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd2
//  a = double([1,3;4,2]);
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd3.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd3
//  a = complex([1,3;4,2]);
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd4.m
//% Test the svd with finite and infinite args (functional only)
//function test_val = test_svd4
//  a = dcomplex([1,3;4,2]);
//  [u,s,v] = svd(a);
//  res1 = 1;
//  res2 = 0;
//  res3 = 0;
//  res4 = 0;
//  try 
//    c = a;
//    c(2,2) = inf;
//    svd(c);
//  catch
//    res2 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = -inf;
//    svd(c);
//  catch
//    res3 = 1;
//  end
//  try
//    c = a;
//    c(2,2) = nan;
//    svd(c);
//  catch
//    res4 = 1;
//  end
//  
//  test_val = res1 & res2 & res3 & res4;
//@}
//@{ test_svd5.m
//% Test the full version of the svd with float vectors
//function test_val = test_svd5
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd6.m
//% Test the full version of the svd with complex vectors
//function test_val = test_svd6
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n))+i*float(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n))+i*float(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n))+i*float(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd7.m
//% Test the full version of the svd with double vectors
//function test_val = test_svd7
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = double(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 4*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 4*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 4*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd8.m
//% Test the full version of the svd with dcomplex vectors
//function test_val = test_svd8
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = double(rand(n,2*n))+i*double(rand(n,2*n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 2*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(2*n,n))+i*double(rand(2*n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = double(rand(n,n))+i*double(rand(n,n));
//    [u,s,v] = svd(a);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd9.m
//% Test the compact version of the svd with float vectors
//function test_val = test_svd9
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd10.m
//% Test the compact version of the svd with complex vectors
//function test_val = test_svd10
//  t1all = 1;
//  p = [];
//  for n=2:4:100
//    a = float(rand(n,2*n))+i*float(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 4*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(2*n,n))+i*float(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 4*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:4:100
//    a = float(rand(n,n))+i*float(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 4*max(abs(diag(s)))*feps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd11.m
//% Test the compact version of the svd with double vectors
//function test_val = test_svd11
//  t1all = 1;
//  p = [];
//  for n=2:100
//    a = double(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 2*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 2*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
//@{ test_svd12.m
//% Test the compact version of the svd with dcomplex vectors
//function test_val = test_svd12
//  t1all = 1;
//  p = [];
//  for n=2:100
//    a = double(rand(n,2*n))+i*double(rand(n,2*n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    p(n) = err;
//    bnd = 4*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(2*n,n))+i*double(rand(2*n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 4*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  for n=2:100
//    a = double(rand(n,n))+i*double(rand(n,n));
//    [u,s,v] = svd(a,0);
//    emat = abs(a-u*s*v');
//    err = max(emat(:));
//    bnd = 4*max(abs(diag(s)))*eps*n;
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;    
//  end
//  test_val = t1all;
//@}
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
  Class Aclass(A.dataClass());
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
	dim.set(0,mindim); dim.set(1,1);
	retval.push_back(Array(FM_FLOAT,dim,svals));
      } else {
	if (!compactform) {
	  dim.set(0,nrows); dim.set(1,nrows);
	  retval.push_back(Array(FM_FLOAT,dim,umat));
	  dim.set(0,nrows); dim.set(1,ncols);
	  float *smat = (float*) Malloc(nrows*ncols*sizeof(float));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_FLOAT,dim,smat));
	  dim.set(0,ncols); dim.set(1,ncols);
	  Array Utrans(FM_FLOAT,dim,vtmat);
	  Utrans.transpose();
	  retval.push_back(Utrans);
	  Free(svals);
	} else {
	  dim.set(0,nrows); dim.set(1,mindim);
	  retval.push_back(Array(FM_FLOAT,dim,umat));
	  dim.set(0,mindim); dim.set(1,mindim);
	  float *smat = (float*) Malloc(mindim*mindim*sizeof(float));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*mindim] = svals[i];
	  retval.push_back(Array(FM_FLOAT,dim,smat));
	  dim.set(0,mindim); dim.set(1,ncols);
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
	dim.set(0,mindim); dim.set(1,1);
	retval.push_back(Array(FM_DOUBLE,dim,svals));
	Free(umat);
	Free(vtmat);
      } else {
	if (!compactform) {
	  dim.set(0,nrows); dim.set(1,nrows);
	  retval.push_back(Array(FM_DOUBLE,dim,umat));
	  dim.set(0,nrows); dim.set(1,ncols);
	  double *smat = (double*) Malloc(nrows*ncols*sizeof(double));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_DOUBLE,dim,smat));
	  dim.set(0,ncols); dim.set(1,ncols);
	  Array Utrans(FM_DOUBLE,dim,vtmat);
	  Utrans.transpose();
	  retval.push_back(Utrans);
	  Free(svals);
	} else {
	  dim.set(0,nrows); dim.set(1,mindim);
	  retval.push_back(Array(FM_DOUBLE,dim,umat));
	  dim.set(0,mindim); dim.set(1,mindim);
	  double *smat = (double*) Malloc(mindim*mindim*sizeof(double));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*mindim] = svals[i];
	  retval.push_back(Array(FM_DOUBLE,dim,smat));
	  dim.set(0,mindim); dim.set(1,ncols);
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
	dim.set(0,mindim); dim.set(1,1);
	retval.push_back(Array(FM_FLOAT,dim,svals));
	Free(umat);
	Free(vtmat);
      } else {
	if (!compactform) {
	  dim.set(0,nrows); dim.set(1,nrows);
	  retval.push_back(Array(FM_COMPLEX,dim,umat));
	  dim.set(0,nrows); dim.set(1,ncols);
	  float *smat = (float*) Malloc(nrows*ncols*sizeof(float));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_FLOAT,dim,smat));
	  dim.set(0,ncols); dim.set(1,ncols);
	  Array Utrans(FM_COMPLEX,dim,vtmat);
	  Utrans.hermitian();
	  retval.push_back(Utrans);
	  Free(svals);
	} else {
	  dim.set(0,nrows); dim.set(1,mindim);
	  retval.push_back(Array(FM_COMPLEX,dim,umat));
	  dim.set(0,mindim); dim.set(1,mindim);
	  float *smat = (float*) Malloc(mindim*mindim*sizeof(float));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*mindim] = svals[i];
	  retval.push_back(Array(FM_FLOAT,dim,smat));
	  dim.set(0,mindim); dim.set(1,ncols);
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
	dim.set(0,mindim); dim.set(1,1);
	retval.push_back(Array(FM_DOUBLE,dim,svals));
	Free(umat);
	Free(vtmat);
      } else {
	if (!compactform) {
	  dim.set(0,nrows); dim.set(1,nrows);
	  retval.push_back(Array(FM_DCOMPLEX,dim,umat));
	  dim.set(0,nrows); dim.set(1,ncols);
	  double *smat = (double*) Malloc(nrows*ncols*sizeof(double));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*nrows] = svals[i];
	  retval.push_back(Array(FM_DOUBLE,dim,smat));
	  dim.set(0,ncols); dim.set(1,ncols);
	  Array Utrans(FM_DCOMPLEX,dim,vtmat);
	  Utrans.hermitian();
	  retval.push_back(Utrans);
	  Free(svals);
	} else {
	  dim.set(0,nrows); dim.set(1,mindim);
	  retval.push_back(Array(FM_DCOMPLEX,dim,umat));
	  dim.set(0,mindim); dim.set(1,mindim);
	  double *smat = (double*) Malloc(mindim*mindim*sizeof(double));
	  for (int i=0;i<mindim;i++)
	    smat[i+i*mindim] = svals[i];
	  retval.push_back(Array(FM_DOUBLE,dim,smat));
	  dim.set(0,mindim); dim.set(1,ncols);
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
//@@Section FLOW
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
			    Interpreter* eval) {
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
//@@Section FREEMAT
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
//@@Section ARRAY
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
//@@Tests
//@{ test_diag1.m
//% Test the diagonal extraction function
//function test_val = test_diag1
//a = [1,2,3,4;5,6,7,8;9,10,11,12];
//b = diag(a);
//test_val = test(b == [1;6;11]);
//@}
//@{ test_diag2.m
//% Test the diagonal extraction function with a non-zero diagonal
//function test_val = test_diag2
//a = [1,2,3,4;5,6,7,8;9,10,11,12];
//b = diag(a,1);
//test_val = test(b == [2;7;12]);
//@}
//@{ test_diag3.m
//% Test the diagonal creation function
//function test_val = test_diag3
//a = [2,3];
//b = diag(a);
//test_val = test(b == [2,0;0,3]);
//@}
//@{ test_diag4.m
//% Test the diagonal creation function with a non-zero diagonal
//function test_val = test_diag4
//a = [2,3];
//b = diag(a,-1);
//test_val = test(b == [0,0,0;2,0,0;0,3,0]);
//@}
//@{ test_diag5.m
//% Test the diagonal creation function with no arguments (bug 1620051)
//function test_val = test_diag5
//test_val = 1;
//try
//  b = diag;
//catch
//  test_val = 1;
//end
//@}
//@@Tests
//@{ test_sparse74.m
//% Test sparse matrix array diagonal extraction
//function x = test_sparse74
//[yi1,zi1] = test_sparse_mat('int32',300,400);
//[yf1,zf1] = test_sparse_mat('float',300,400);
//[yd1,zd1] = test_sparse_mat('double',300,400);
//[yc1,zc1] = test_sparse_mat('complex',300,400);
//[yz1,zz1] = test_sparse_mat('dcomplex',300,400);
//x = testeq(diag(yi1,30),diag(zi1,30)) & testeq(diag(yf1,30),diag(zf1,30)) & testeq(diag(yd1,30),diag(zd1,30)) & testeq(diag(yc1,30),diag(zc1,30)) & testeq(diag(yz1,30),diag(zz1,30));
//@}
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
  if (arg.size() == 0)
    throw Exception("diag requires at least one argument.\n");
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
//@@Section INSPECTION
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
//@@Tests
//@{ test_empty.m
//% Test the arithmetic operators with empty arguments
//function test_val = test_empty
//test_val = isempty([]+[]);
//test_val = test_val & isempty([]-[]);
//test_val = test_val & isempty(-[]);
//test_val = test_val & isempty([]*[]);
//test_val = test_val & isempty([]/[]);
//test_val = test_val & isempty([]\[]);
//test_val = test_val & isempty([].*[]);
//test_val = test_val & isempty([]./[]);
//test_val = test_val & isempty([].\[]);
//test_val = test_val & isempty([]^[]);
//test_val = test_val & isempty([].^[]);
//test_val = test_val & isempty([]>[]);
//test_val = test_val & isempty([]>=[]);
//test_val = test_val & isempty([]<[]);
//test_val = test_val & isempty([]<=[]);
//@}
//!
ArrayVector IsEmptyFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("isempty function requires at least input argument");
  ArrayVector retval;
  retval.push_back(Array::logicalConstructor(arg[0].isEmpty()));
  return retval;
}

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
//!
ArrayVector SponesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("spones function requires a sparse matrix template argument");
  Array tmp(arg[0]);
  if (tmp.isEmpty())
    return singleArrayVector(Array::emptyConstructor());
  if(tmp.isReferenceType())
    throw Exception("spones function requires a numeric sparse matrix argument");
  tmp.makeSparse();
  if (!tmp.sparse())
    throw Exception("spones function requires a sparse matrix template argument");
  return singleArrayVector(Array::Array(FM_FLOAT,Dimensions(tmp.getDimensionLength(0),tmp.getDimensionLength(1)),SparseOnesFunc(tmp.dataClass(),tmp.getDimensionLength(0),tmp.getDimensionLength(1),tmp.getSparseDataPointer()),true));
}

//!
//@Module WARNING Emits a Warning Message
//@@Section FLOW
//@@Usage
//The @|warning| function causes a warning message to be
//sent to the user.  The general syntax for its use is
//@[
//   warning(s)
//@]
//where @|s| is the string message containing the warning.
//!
ArrayVector WarningFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("Not enough inputs to warning function");
  if (!(arg[0].isString()))
    throw Exception("Input to error function must be a string");
  eval->warningMessage(arg[0].getContentsAsCString());
  return ArrayVector();
}

//!
//@Module ERROR Causes an Error Condition Raised
//@@Section FLOW
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
//  elseif (~isa(n,'int32'))
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
//@<2
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

//!
//@Module EVAL Evaluate a String
//@@Section FREEMAT
//@@Usage
//The @|eval| function evaluates a string.  The general syntax
//for its use is
//@[
//   eval(s)
//@]
//where @|s| is the string to evaluate.  If @|s| is an expression
//(instead of a set of statements), you can assign the output
//of the @|eval| call to one or more variables, via
//@[
//   x = eval(s)
//   [x,y,z] = eval(s)
//@]
//
//Another form of @|eval| allows you to specify an expression or
//set of statements to execute if an error occurs.  In this 
//form, the syntax for @|eval| is
//@[
//   eval(try_clause,catch_clause),
//@]
//or with return values,
//@[
//   x = eval(try_clause,catch_clause)
//   [x,y,z] = eval(try_clause,catch_clause)
//@]
//These later forms are useful for specifying defaults.  Note that
//both the @|try_clause| and @|catch_clause| must be expressions,
//as the equivalent code is
//@[
//  try
//    [x,y,z] = try_clause
//  catch
//    [x,y,z] = catch_clause
//  end
//@]
//so that the assignment must make sense in both cases.
//@@Example
//Here are some examples of @|eval| being used.
//@<
//eval('a = 32')
//b = eval('a')
//@>
//The primary use of the @|eval| statement is to enable construction
//of expressions at run time.
//@<
//s = ['b = a' ' + 2']
//eval(s)
//@>
//Here we demonstrate the use of the catch-clause to provide a 
//default value
//@<
//a = 32
//b = eval('a','1')
//b = eval('z','a+1')
//@>
//Note that in the second case, @|b| takes the value of 33, indicating
//that the evaluation of the first expression failed (because @|z| is
//not defined).
//!
static  char* PrePendCallVars(char *line, int nargout) {
  char *buf = (char*) malloc(strlen(line)+4096);
  char *gp = buf;
  if (nargout > 1)
    *gp++ = '[';
  for (int i=0;i<nargout-1;i++) {
    sprintf(gp,"t___%d,",i);
    gp += strlen(gp);
  }
  sprintf(gp,"t___%d",nargout-1);
  gp += strlen(gp);
  if (nargout > 1)
    sprintf(gp,"] = %s;\n",line);
  else
    sprintf(gp," = %s;\n",line);
  return buf;
}

//
//static - having a static here seems to break cross compiling
//
ArrayVector RetrieveCallVars(Interpreter *eval, int nargout) {
  ArrayVector retval;
  for (int i=0;i<nargout;i++) {
    char tname[4096];
    Array tval;
    sprintf(tname,"t___%d",i);
    ArrayReference ptr = eval->getContext()->lookupVariable(tname);
    if (!ptr.valid())
      tval = Array::emptyConstructor();
    else
      tval = *ptr;
    eval->getContext()->deleteVariable(tname);
    retval.push_back(tval);
  }
  return retval;
}

ArrayVector EvalTryFunction(int nargout, const ArrayVector& arg, Interpreter* eval, int popSpec) {
  if (nargout > 0) {
    char *try_line = arg[0].getContentsAsCString();
    char *try_buf = PrePendCallVars(try_line,nargout);
    char *catch_line = arg[1].getContentsAsCString();
    char *catch_buf = PrePendCallVars(catch_line,nargout);
    ArrayVector retval;
    bool autostop;
    autostop = eval->AutoStop();
    eval->AutoStop(false);
    try {
      eval->getContext()->bypassScope(popSpec);
      eval->evaluateString(try_buf,true);
      retval = RetrieveCallVars(eval,nargout);
      eval->getContext()->restoreBypassedScopes();
    } catch (Exception &e) {
      eval->getContext()->restoreBypassedScopes();
      eval->evaluateString(catch_buf,false);
      retval = RetrieveCallVars(eval,nargout);
    }
    eval->AutoStop(true);
    free(try_buf);
    free(catch_buf);
    return retval;
  } else {
    char *try_line = arg[0].getContentsAsCString();
    char *catch_line = arg[1].getContentsAsCString();
    char *try_buf = (char*) malloc(strlen(try_line)+2);
    char *catch_buf = (char*) malloc(strlen(catch_line)+2);
    sprintf(try_buf,"%s\n",try_line);
    sprintf(catch_buf,"%s\n",catch_line);
    bool autostop;
    autostop = eval->AutoStop();
    eval->AutoStop(false);
    try {
      eval->getContext()->bypassScope(popSpec);
      eval->evaluateString(try_buf,true);
      eval->getContext()->restoreBypassedScopes();
    } catch (Exception &e) {
      eval->getContext()->restoreBypassedScopes();
      eval->evaluateString(catch_buf,false);
    }
    eval->AutoStop(true);
    free(try_buf);
    free(catch_buf);
    return ArrayVector();
  }
}

ArrayVector EvalNoTryFunction(int nargout, const ArrayVector& arg, Interpreter* eval, int popSpec) {
  if (nargout > 0) {
    char *line = arg[0].getContentsAsCString();
    char *buf = PrePendCallVars(line,nargout);
    eval->getContext()->bypassScope(popSpec);
    eval->evaluateString(buf);
    ArrayVector retval(RetrieveCallVars(eval,nargout));
    eval->getContext()->restoreBypassedScopes();
    free(buf);
    return retval;
  } else {
    char *line = arg[0].getContentsAsCString();
    char *buf = (char*) malloc(strlen(line)+2);
    sprintf(buf,"%s\n",line);
    eval->getContext()->bypassScope(popSpec);
    eval->evaluateString(buf);
    eval->getContext()->restoreBypassedScopes();
    free(buf);
    return ArrayVector();
  }
}

ArrayVector EvalFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("eval function takes at least one argument - the string to execute");
  if (arg.size() == 2)
    return EvalTryFunction(nargout, arg, eval, 0);
  return EvalNoTryFunction(nargout, arg, eval, 0);
}

//!
//@Module EVALIN Evaluate a String in Workspace
//@@Section FREEMAT
//@@Usage
//The @|evalin| function is similar to the @|eval| function, with an additional
//argument up front that indicates the workspace that the expressions are to 
//be evaluated in.  The various syntaxes for @|evalin| are:
//@[
//   evalin(workspace,expression)
//   x = evalin(workspace,expression)
//   [x,y,z] = evalin(workspace,expression)
//   evalin(workspace,try_clause,catch_clause)
//   x = evalin(workspace,try_clause,catch_clause)
//   [x,y,z] = evalin(workspace,try_clause,catch_clause)
//@]
//The argument @|workspace| must be either 'caller' or 'base'.  If it is
//'caller', then the expression is evaluated in the caller's work space.
//That does not mean the caller of @|evalin|, but the caller of the current
//function or script.  On the other hand if the argument is 'base', then
//the expression is evaluated in the base work space.   See @|eval| for
//details on the use of each variation.
//!
ArrayVector EvalInFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 2)
    throw Exception("evalin function requires a workspace (scope) specifier (either 'caller' or 'base') and an expression to evaluate");
  Array spec(arg[0]);
  char *spec_str = spec.getContentsAsCString();
  int popspec = 0;
  if (strcmp(spec_str,"base")==0) 
    popspec = -1;
  else if (strcmp(spec_str,"caller")==0) 
    popspec = 1;
  else
    throw Exception("evalin function requires the first argument to be either 'caller' or 'base'");
  ArrayVector argcopy(arg);
  argcopy.pop_front();
  if (arg.size() == 3)
    return EvalTryFunction(nargout,argcopy,eval,popspec);
  else
    return EvalNoTryFunction(nargout,argcopy,eval,popspec);
}

//!
//@Module ASSIGNIN Assign Variable in Workspace
//@@Section FREEMAT
//@@Usage
//The @|assignin| function allows you to assign a value to a variable
//in either the callers work space or the base work space.  The syntax
//for @|assignin| is
//@[
//   assignin(workspace,variablename,value)
//@]
//The argument @|workspace| must be either 'caller' or 'base'.  If it is
//'caller' then the variable is assigned in the caller's work space.
//That does not mean the caller of @|assignin|, but the caller of the
//current function or script.  On the other hand if the argument is 'base',
//then the assignment is done in the base work space.  Note that the
//variable is created if it does not already exist.
//!
ArrayVector AssignInFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 3)
    throw Exception("assignin function requires a workspace (scope) specifier (either 'caller' or 'base') a variable name and a value to assign");
  Array spec(arg[0]);
  char *spec_str = spec.getContentsAsCString();
  int popspec = 0;
  if (strcmp(spec_str,"base")==0) 
    popspec = -1;
  else if (strcmp(spec_str,"caller")==0) 
    popspec = 1;
  else
    throw Exception("evalin function requires the first argument to be either 'caller' or 'base'");
  const char *varname = ArrayToString(arg[1]);
  Array varvalue = arg[2];
  eval->getContext()->bypassScope(popspec);
  eval->getContext()->insertVariable(varname,varvalue);
  eval->getContext()->restoreBypassedScopes();
  return ArrayVector();
}

//!
//@Module SOURCE Execute an Arbitrary File
//@@Section FREEMAT
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
//@@Tests
//@{ test_source.m
//% Check that the source function does not double-execute the last line of the script
//function test_val = test_source
//source('source_test_script.m')
//test_val = test(n == 2);
//@}
//@{ source_test_script.m
//n = 1;
//n = n + 1;
//@}
//!
ArrayVector SourceFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
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

//!
//@Module DBDELETE Delete a Breakpoint
//@@Section DEBUG
//@@Usage
//The @|dbdelete| function deletes a breakpoint.  The syntax
//for the @|dbdelete| function is
//@[
//  dbdelete(num)
//@]
//where @|num| is the number of the breakpoint to delete.
//!
ArrayVector DbDeleteFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("dbdelete requires an argument (number of breakpoint to delete)");
  Array tmp(arg[0]);
  int bpnum;
  bpnum = tmp.getContentsAsIntegerScalar();
  eval->deleteBreakpoint(bpnum);
  return ArrayVector();
}

//!
//@Module DBLIST List Breakpoints
//@@Section DEBUG
//@@Usage
//List the current set of breakpoints.  The syntax for the
//@|dblist| is simply
//@[
//  dblist
//@]
//!
ArrayVector DbListFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  eval->listBreakpoints();
  return ArrayVector();
}

//!
//@Module DBSTEP Step N Statements
//@@Section DEBUG
//@@Usage
//Step @|N| statements during debug mode.  The synax for this is
//either
//@[
//  dbstep(N)
//@]
//to step @|N| statements, or
//@[
//  dbstep
//@]
//to step one statement.
//!
// ArrayVector DbStepFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
//   int linesToSkip;
//   if (arg.size() == 0)
//     linesToSkip = 1;
//   else {
//     Array tmp(arg[0]);
//     linesToSkip = tmp.getContentsAsIntegerScalar();
//   }
//   eval->dbstep(linesToSkip);
//   return ArrayVector();
// }

//!
//@Module DBSTOP
//@@Section DEBUG
//@@Usage
//Set a breakpoint.  The syntax for this is:
//@[
//  dbstop(funcname,linenumber)
//@]
//where @|funcname| is the name of the function where we want
//to set the breakpoint, and @|linenumber| is the line number.
//!

ArrayVector DbStopFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 2)
    throw Exception("dbstop function requires at least two arguments");
  if (!(arg[0].isString()))
    throw Exception("first argument to dbstop must be the name of routine where to stop");
  const char *cname = ArrayToString(arg[0]);
  int line = ArrayToInt32(arg[1]);
  eval->addBreakpoint(cname,line);
  return ArrayVector();
}
  
ArrayVector FdumpFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("fdump function requires at least one argument");
  if (!(arg[0].isString()))
    throw Exception("first argument to fdump must be the name of a function (i.e., a string)");
  char *fname = arg[0].getContentsAsCString();
  Context *context = eval->getContext();
  FunctionDef *funcDef;
  if (!context->lookupFunction(fname,funcDef))
    throw Exception(std::string("function ") + fname + " undefined!");
  funcDef->updateCode();
  funcDef->printMe(eval);
  return ArrayVector();
}


//!
//@Module BUILTIN Evaulate Builtin Function
//@@Section FREEMAT
//@@Usage
//The @|builtin| function evaluates a built in function
//with the given name, bypassing any overloaded functions.
//The syntax of @|builtin| is
//@[
//  [y1,y2,...,yn] = builtin(fname,x1,x2,...,xm)
//@]
//where @|fname| is the name of the function to call.  Apart
//from the fact that @|fname| must be a string, and that @|builtin|
//always calls the non-overloaded method, it operates exactly like
//@|feval|.  Note that unlike MATLAB, @|builtin| does not force
//evaluation to an actual compiled function.  It simply subverts
//the activation of overloaded method calls.
//!
ArrayVector BuiltinFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("builtin function requires at least one argument");
  if (!(arg[0].isString()))
    throw Exception("first argument to builtin must be the name of a function (i.e., a string)");
  FunctionDef *funcDef;
  char *fname = arg[0].getContentsAsCString();
  Context *context = eval->getContext();
  if (!context->lookupFunction(fname,funcDef))
    throw Exception(std::string("function ") + fname + " undefined!");
  funcDef->updateCode();
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  ArrayVector newarg(arg);
  newarg.pop_front();
  bool flagsave = eval->getStopOverload();
  eval->setStopOverload(true);
  ArrayVector tmp(funcDef->evaluateFunction(eval,newarg,nargout));
  eval->setStopOverload(flagsave);
  return tmp;
}
  
  
//!
//@Module FEVAL Evaluate a Function
//@@Section FREEMAT
//@@Usage
//The @|feval| function executes a function using its name.
//The syntax of @|feval| is
//@[
//  [y1,y2,...,yn] = feval(f,x1,x2,...,xm)
//@]
//where @|f| is the name of the function to evaluate, and
//@|xi| are the arguments to the function, and @|yi| are the
//return values.
//
//Alternately, @|f| can be a function handle to a function
//(see the section on @|function handles| for more information).
//@@Example
//Here is an example of using @|feval| to call the @|cos| 
//function indirectly.
//@<
//feval('cos',pi/4)
//@>
//Now, we call it through a function handle
//@<
//c = @cos
//feval(c,pi/4)
//@>
//!
ArrayVector FevalFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("feval function requires at least one argument");
  if (!(arg[0].isString()) && (arg[0].dataClass() != FM_FUNCPTR_ARRAY))
    throw Exception("first argument to feval must be the name of a function (i.e., a string) or a function handle");
  FunctionDef *funcDef;
  if (arg[0].isString()) {
    char *fname = arg[0].getContentsAsCString();
    Context *context = eval->getContext();
    if (!context->lookupFunction(fname,funcDef))
      throw Exception(std::string("function ") + fname + " undefined!");
  } else {
    if (!arg[0].isScalar())
      throw Exception("function handle argument to feval must be a scalar");
    const FunctionDef **fp = (const FunctionDef **) arg[0].getDataPointer();
    funcDef = (FunctionDef*) fp[0];
    if (!funcDef) return ArrayVector();
  }
  funcDef->updateCode();
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  ArrayVector newarg(arg);
  newarg.pop_front();
  return(funcDef->evaluateFunction(eval,newarg,nargout));
}

//!
//@Module DOCLI Start a Command Line Interface
//@@Section FREEMAT
//@@Usage
//The @|docli| function is the main function that you interact with 
//when you run FreeMat.  I am not sure why you would want to use
//it, but hey - its there if you want to use it.
//!
ArrayVector DoCLIFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  eval->doCLI();
  return ArrayVector();
}


//!
//@Module REPMAT Array Replication Function
//@@Section ARRAY
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
  int i, j, k;
  if (arg.size() < 2)
    throw Exception("repmat function requires at least two arguments");
  Array x(arg[0]);
  if (x.isEmpty()) return singleArrayVector(Array::emptyConstructor());
  Dimensions repcount;
  // Case 1, look for a scalar second argument
  if ((arg.size() == 2) && (arg[1].isScalar())) {
    Array t(arg[1]);
    repcount.set(0,t.getContentsAsIntegerScalar());
    repcount.set(1,t.getContentsAsIntegerScalar());
  } 
  // Case 2, look for two scalar arguments
  else if ((arg.size() == 3) && (arg[1].isScalar()) && (arg[2].isScalar())) {
    Array t(arg[1]);
    repcount.set(0,t.getContentsAsIntegerScalar());
    t = arg[2];
    repcount.set(1,t.getContentsAsIntegerScalar());
  }
  // Case 3, look for a vector second argument
  else {
    if (arg.size() > 2) throw Exception("unrecognized form of arguments for repmat function");
    Array t(arg[1]);
    t.promoteType(FM_INT32);
    if (t.getLength() > maxDims) throw Exception("replication request exceeds maxDims constant - either rebuild FreeMat with a higher maxDims constant, or shorten the requested replication array");
    int32 *dp = (int32*) t.getDataPointer();
    for (i=0;i<t.getLength();i++)
      repcount.set(i,dp[i]);
  }
  // Check for a valid replication count
  for (i=0;i<repcount.getLength();i++)
    if (repcount.get(i) < 0) throw Exception("negative replication counts not allowed in argument to repmat function");
  // All is peachy.  Allocate an output array of sufficient size.
  Dimensions originalSize(x.dimensions());
  Dimensions outdims;
  int outdim;
  outdim = MAX(repcount.getLength(),originalSize.getLength());
  for (i=0;i<outdim;i++)
    outdims.set(i,originalSize.get(i)*repcount.get(i));
  outdims.simplify();
  void *dp = Array::allocateArray(x.dataClass(),
				  outdims.getElementCount(),
				  x.fieldNames());
  // Copy can work by pushing or by pulling.  I have opted for
  // pushing, because we can push a column at a time, which might
  // be slightly more efficient.
  int colsize = originalSize.get(0);
  int outcolsize = outdims.get(0);
  int colcount = originalSize.getElementCount()/colsize;
  // copySelect stores which copy we are pushing.
  Dimensions copySelect(outdim);
  // anchor is used to calculate where this copy lands in the output matrix
  Dimensions anchor(outdim);
  // sourceAddress is used to track which column we are pushing in the
  // source matrix
  Dimensions sourceAddress(outdims.getLength());
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
	anchor.set(k,copySelect.get(k)*originalSize.get(k)+sourceAddress.get(k));
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
  retval.push_back(Array(x.dataClass(),outdims,dp,false,x.fieldNames()));
  return retval;
}

//!
//@Module SYSTEM Call an External Program
//@@Section OS
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
  Dimensions dim(cp.size(),1);
  Array ret(FM_CELL_ARRAY,dim,np);
  retval.push_back(ret);
  return retval;
}

//!
//@Module CONV2 Matrix Convolution
//@@Section SIGNAL
//@@Usage
//The @|conv2| function performs a two-dimensional convolution of
//matrix arguments.  The syntax for its use is
//@[
//    Z = conv2(X,Y)
//@]
//which performs the full 2-D convolution of @|X| and @|Y|.  If the 
//input matrices are of size @|[xm,xn]| and @|[ym,yn]| respectively,
//then the output is of size @|[xm+ym-1,xn+yn-1]|.  Another form is
//@[
//    Z = conv2(hcol,hrow,X)
//@]
//where @|hcol| and @|hrow| are vectors.  In this form, @|conv2|
//first convolves @|Y| along the columns with @|hcol|, and then 
//convolves @|Y| along the rows with @|hrow|.  This is equivalent
//to @|conv2(hcol(:)*hrow(:)',Y)|.
//
//You can also provide an optional @|shape| argument to @|conv2|
//via either
//@[
//    Z = conv2(X,Y,'shape')
//    Z = conv2(hcol,hrow,X,'shape')
//@]
//where @|shape| is one of the following strings
//\begin{itemize}
//\item @|'full'| - compute the full convolution result - this is the default if no @|shape| argument is provided.
//\item @|'same'| - returns the central part of the result that is the same size as @|X|.
//\item @|'valid'| - returns the portion of the convolution that is computed without the zero-padded edges.  In this situation, @|Z| has 
//size @|[xm-ym+1,xn-yn+1]| when @|xm>=ym| and @|xn>=yn|.  Otherwise
//@|conv2| returns an empty matrix.
//\end{itemize}
//@@Function Internals
//The convolution is computed explicitly using the definition:
//\[
//  Z(m,n) = \sum_{k} \sum_{j} X(k,j) Y(m-k,n-j)
//\]
//If the full output is requested, then @|m| ranges over @|0 <= m < xm+ym-1|
//and @|n| ranges over @|0 <= n < xn+yn-1|.  For the case where @|shape|
//is @|'same'|, the output ranges over @|(ym-1)/2 <= m < xm + (ym-1)/2|
//and @|(yn-1)/2 <= n < xn + (yn-1)/2|.  
//!
  
  

// Summation limits...
//  sum a(i,j) b(m-i,n-j)
//  Need
//    0 <= m-i <= Bm-1
//  Can rewrite this as:
//    -m <= -i <= Bm-1-m
//    m >= i >= m+1-Bm
//    m+1-Bm <= i <= m
//    0 <= n-j <= Bn-1
//    n >= j >= n+1-Bn
//    n+1-Bn <= j <= n
//  And
//    0 <= i <= Am-1
//    0 <= j <= An-1
//  So  
//    m+1-Bm <= i <= m
//    0 <= i <= Am-1
//    max(0,m+1-Bm) <= i <= min(m,Am-1)
//
//    n+1-Bn <= j <= n
//    0 <= j <= An-1
//    max(0,n+1-Bn) <= j <= min(n,An-1)
//
//
//    max(0,m+1-Bm) <= i <= min(m,Am-1)
//    max(0,n+1-Bn) <= j <= min(n,An-1)

template <class T>
static void Conv2MainReal(T* C, const T* A, const T*B,
			  int Am, int An, int Bm, int Bn, 
			  int Cm, int Cn, int Cm_offset, int Cn_offset) {
  for (int n=0;n<Cn;n++)
    for (int m=0;m<Cm;m++) {
      T accum = 0;
      int iMin, iMax;
      int jMin, jMax;
      iMin = std::max(0,m+Cm_offset-Bm+1);
      iMax = std::min(Am-1,m+Cm_offset);
      jMin = std::max(0,n+Cn_offset-Bn+1);
      jMax = std::min(An-1,n+Cn_offset);
      for (int j=jMin;j<=jMax;j++)
	for (int i=iMin;i<=iMax;i++)
	  accum += A[i+j*Am]*B[(m+Cm_offset-i)+(n+Cn_offset-j)*Bm];
      C[m+n*Cm] = accum;
    }
}

template <class T>
static void Conv2MainComplex(T* C, const T* A, const T*B,
			     int Am, int An, int Bm, int Bn, 
			     int Cm, int Cn, int Cm_offset, int Cn_offset) {
  for (int n=0;n<Cn;n++)
    for (int m=0;m<Cm;m++) {
      T accum_r = 0;
      T accum_i = 0;
      int iMin, iMax;
      int jMin, jMax;
      iMin = std::max(0,m+Cm_offset-Bm+1);
      iMax = std::min(Am-1,m+Cm_offset);
      jMin = std::max(0,n+Cn_offset-Bn+1);
      jMax = std::min(An-1,n+Cn_offset);
      for (int j=jMin;j<=jMax;j++)
	for (int i=iMin;i<=iMax;i++) {
	  accum_r += A[2*(i+j*Am)]*
	    B[2*((m+Cm_offset-i)+(n+Cn_offset-j)*Bm)]
	    - A[2*(i+j*Am)+1]*B[2*((m+Cm_offset-i)+(n+Cn_offset-j)*Bm)+1];
	  accum_i += A[2*(i+j*Am)]*
	    B[2*((m+Cm_offset-i)+(n+Cn_offset-j)*Bm)+1]
	    - A[2*(i+j*Am)+1]*B[2*((m+Cm_offset-i)+(n+Cn_offset-j)*Bm)];
	}
      C[2*(m+n*Cm)] = accum_r;
      C[2*(m+n*Cm)] = accum_i;
    }
}

Array Conv2FunctionDispatch(Array X,Array Y,int Cm,int Cn,
			    int Cm_offset, int Cn_offset) {
  switch (X.dataClass()) {
  case FM_FLOAT: {
    float *cp = (float*) Array::allocateArray(FM_FLOAT,Cm*Cn);
    Conv2MainReal<float>(cp,
			 (const float*) X.getDataPointer(),
			 (const float*) Y.getDataPointer(),
			 X.getDimensionLength(0),
			 X.getDimensionLength(1),
			 Y.getDimensionLength(0),
			 Y.getDimensionLength(1),
			 Cm, Cn, Cm_offset, Cn_offset);
    return Array(FM_FLOAT,Dimensions(Cm,Cn),cp);
  }
  case FM_DOUBLE: {
    double *cp = (double*) Array::allocateArray(FM_DOUBLE,Cm*Cn);
    Conv2MainReal<double>(cp,
			  (const double*) X.getDataPointer(),
			  (const double*) Y.getDataPointer(),
			  X.getDimensionLength(0),
			  X.getDimensionLength(1),
			  Y.getDimensionLength(0),
			  Y.getDimensionLength(1),
			  Cm, Cn, Cm_offset, Cn_offset);
    return Array(FM_DOUBLE,Dimensions(Cm,Cn),cp);
  }
  case FM_INT32: {
    int32 *cp = (int32*) Array::allocateArray(FM_INT32,Cm*Cn);
    Conv2MainReal<int32>(cp,
			 (const int32*) X.getDataPointer(),
			 (const int32*) Y.getDataPointer(),
			 X.getDimensionLength(0),
			 X.getDimensionLength(1),
			 Y.getDimensionLength(0),
			 Y.getDimensionLength(1),
			 Cm, Cn, Cm_offset, Cn_offset);
    return Array(FM_INT32,Dimensions(Cm,Cn),cp);
  }
  case FM_INT64: {
    int64 *cp = (int64*) Array::allocateArray(FM_INT64,Cm*Cn);
    Conv2MainReal<int64>(cp,
			 (const int64*) X.getDataPointer(),
			 (const int64*) Y.getDataPointer(),
			 X.getDimensionLength(0),
			 X.getDimensionLength(1),
			 Y.getDimensionLength(0),
			 Y.getDimensionLength(1),
			 Cm, Cn, Cm_offset, Cn_offset);
    return Array(FM_INT64,Dimensions(Cm,Cn),cp);
  }
  case FM_COMPLEX: {
    float *cp = (float*) Array::allocateArray(FM_COMPLEX,Cm*Cn);
    Conv2MainComplex<float>(cp,
			    (const float*) X.getDataPointer(),
			    (const float*) Y.getDataPointer(),
			    X.getDimensionLength(0),
			    X.getDimensionLength(1),
			    Y.getDimensionLength(0),
			    Y.getDimensionLength(1),
			    Cm, Cn, Cm_offset, Cn_offset);
    return Array(FM_COMPLEX,Dimensions(Cm,Cn),cp);
  }
  case FM_DCOMPLEX: {
    double *cp = (double*) Array::allocateArray(FM_DCOMPLEX,Cm*Cn);
    Conv2MainComplex<double>(cp,
			     (const double*) X.getDataPointer(),
			     (const double*) Y.getDataPointer(),
			     X.getDimensionLength(0),
			     X.getDimensionLength(1),
			     Y.getDimensionLength(0),
			     Y.getDimensionLength(1),
			     Cm, Cn, Cm_offset, Cn_offset);
    return Array(FM_DCOMPLEX,Dimensions(Cm,Cn),cp);
  }
  }
}

static ArrayVector Conv2FunctionFullXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = X.getDimensionLength(0) + Y.getDimensionLength(0) - 1;
  Cn = X.getDimensionLength(1) + Y.getDimensionLength(1) - 1;
  Cm_offset = 0;
  Cn_offset = 0;
  return singleArrayVector(Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset));
}

static ArrayVector Conv2FunctionSameXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = X.getDimensionLength(0);
  Cn = X.getDimensionLength(1);
  Cm_offset = (int) floor((double)((Y.getDimensionLength(0)-1)/2));
  Cn_offset = (int) floor((double)((Y.getDimensionLength(1)-1)/2));
  return singleArrayVector(Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset));
}

static ArrayVector Conv2FunctionValidXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = X.getDimensionLength(0)-Y.getDimensionLength(0)+1;
  Cn = X.getDimensionLength(1)-Y.getDimensionLength(1)+1;
  if ((Cm <= 0) || (Cn <= 0))
    return singleArrayVector(Array::emptyConstructor());
  Cm_offset = Y.getDimensionLength(0)-1;
  Cn_offset = Y.getDimensionLength(1)-1;
  return singleArrayVector(Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset));    
}

ArrayVector Conv2FunctionXY(Array X, Array Y, char* type) {
  // Check the arguments
  if (X.isReferenceType() || Y.isReferenceType())
    throw Exception("cannot apply conv2 to reference types.");
  if (!X.is2D() || !Y.is2D())
    throw Exception("arguments must be matrices, not n-dimensional arrays.");
  TypeCheck(X,Y,false);
  if ((strcmp(type,"full") == 0) || (strcmp(type,"FULL") == 0))
    return Conv2FunctionFullXY(X,Y);
  if ((strcmp(type,"same") == 0) || (strcmp(type,"SAME") == 0))
    return Conv2FunctionSameXY(X,Y);
  if ((strcmp(type,"valid") == 0) || (strcmp(type,"VALID") == 0))
    return Conv2FunctionValidXY(X,Y);
  throw Exception("count not recognize the arguments to conv2");
}

ArrayVector Conv2FunctionRCX(Array hcol, Array hrow, Array X, char *type) {
  if (hcol.isReferenceType() || hrow.isReferenceType() ||
      X.isReferenceType())
    throw Exception("cannot apply conv2 to reference types.");
  if (!hcol.is2D() || !hrow.is2D() || !X.is2D())
    throw Exception("arguments must be matrices, not n-dimensional arrays.");
  Dimensions t(hcol.getLength(),1);
  hcol.reshape(t);
  t = Dimensions(1,hrow.getLength());
  hrow.reshape(t);
  ArrayVector rvec;
  rvec = Conv2FunctionXY(X,hcol,type);
  rvec = Conv2FunctionXY(rvec.back(),hrow,type);
  return rvec;
}

ArrayVector Conv2Function(int nargout, const ArrayVector& arg) {
  // Call the right function based on the arguments
  if (arg.size() < 2) 
    throw Exception("conv2 requires at least 2 arguments");
  if (arg.size() == 2)
    return Conv2FunctionXY(arg[0],arg[1],"full");
  if ((arg.size() == 3) && (arg[2].isString()))
    return Conv2FunctionXY(arg[0],arg[1],arg[2].getContentsAsCString());
  if (arg.size() == 3)
    return Conv2FunctionRCX(arg[0],arg[1],arg[2],"full");
  if ((arg.size() == 4) && (arg[3].isString()))
    return Conv2FunctionRCX(arg[0],arg[1],arg[2],
			    arg[3].getContentsAsCString());
  throw Exception("could not recognize which form of conv2 was requested - check help conv2 for details");
}

