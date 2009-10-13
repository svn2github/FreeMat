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
#include "SparseCCS.hpp"
#include "Algorithms.hpp"
#include "Math.hpp"

static inline index_t CountNonZeros(const BasicArray<bool> &dp) {
  index_t nonZero = 0;
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) nonZero++;
  return nonZero;
}

static ArrayVector SingleFindModeFull(const Array &arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D()))
    return ArrayVector(Array(Double));
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> sp(retdims);
  index_t j = 1;
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) 
      sp[j++] = i;
  return ArrayVector(Array(sp));
}
  
static ArrayVector RCFindModeFull(const Array &arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D())) {
    ArrayVector retval;
    retval.push_back(Array(Double));
    retval.push_back(Array(Double));
    return retval;
  }
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> rp(retdims);
  BasicArray<index_t> cp(retdims);
  index_t j = 1;
  index_t rows = arg.rows();
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) {
      rp[j] = (uint64(i-1) % uint64(rows)) + 1;
      cp[j] = (uint64(i-1) / uint64(rows)) + 1;
      j++;
    }
  ArrayVector retval;
  retval.push_back(Array(rp));
  retval.push_back(Array(cp));
  return retval;
}

template <class T>
static ArrayVector RCVFindModeFullReal(Array arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  const BasicArray<T> &vp(arg.constReal<T>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D())) {
    ArrayVector retval;
    retval.push_back(Array(Double));
    retval.push_back(Array(Double));
    Array wp(GetDataClass(T(0)));
    if ((wp.dataClass() != Bool) && (wp.dataClass() != Float)
	&& (wp.dataClass() != Double))
      wp = wp.toClass(Double);
    retval.push_back(wp);
    return retval;
  }
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> rp(retdims);
  BasicArray<index_t> cp(retdims);
  BasicArray<T> qp(retdims);
  index_t j = 1;
  index_t rows = arg.rows();
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) {
      rp[j] = (uint64(i-1) % uint64(rows)) + 1;
      cp[j] = (uint64(i-1) / uint64(rows)) + 1;
      qp[j] = vp[i];
      j++;
    }
  ArrayVector retval;
  retval.push_back(Array(rp));
  retval.push_back(Array(cp));
  Array zp(qp);
  if ((zp.dataClass() != Bool) && (zp.dataClass() != Float)
      && (zp.dataClass() != Double))
    zp = zp.toClass(Double);
  retval.push_back(zp);
  return retval;
}  

template <class T>
static ArrayVector RCVFindModeFullComplex(Array arg) {
  Array x(arg.toClass(Bool).asDenseArray());
  const BasicArray<bool> &dp(x.constReal<bool>());
  const BasicArray<T> &vp(arg.constReal<T>());
  const BasicArray<T> &zp(arg.constImag<T>());
  index_t nonZero = CountNonZeros(dp);
  if ((nonZero == 0) && (x.is2D())) {
    ArrayVector retval;
    retval.push_back(Array(Double));
    retval.push_back(Array(Double));
    Array wp(GetDataClass(T(0)));
    if ((wp.dataClass() != Bool) && (wp.dataClass() != Float)
	&& (wp.dataClass() != Double))
      wp = wp.toClass(Double);
    retval.push_back(wp);
    return retval;
  }
  NTuple retdims;
  if (x.isRowVector())
    retdims = NTuple(1,nonZero);
  else
    retdims = NTuple(nonZero,1);
  BasicArray<index_t> rp(retdims);
  BasicArray<index_t> cp(retdims);
  BasicArray<T> qp(retdims);
  BasicArray<T> ip(retdims);
  index_t j = 1;
  index_t rows = arg.rows();
  for (index_t i=1;i<=dp.length();i++)
    if (dp[i]) {
      rp[j] = (uint64(i-1) % uint64(rows)) + 1;
      cp[j] = (uint64(i-1) / uint64(rows)) + 1;
      qp[j] = vp[i];
      ip[j] = zp[i];
      j++;
    }
  ArrayVector retval;
  retval.push_back(Array(rp));
  retval.push_back(Array(cp));
  Array wp(qp,ip);
  if ((wp.dataClass() != Bool) && (wp.dataClass() != Float)
      && (wp.dataClass() != Double))
    wp = wp.toClass(Double);
  retval.push_back(wp);
  return retval;
}

template <class T>
static ArrayVector RCVFindModeFull(Array x) {
  if (x.allReal())
    return RCVFindModeFullReal<T>(x);
  else
    return RCVFindModeFullComplex<T>(x);
}

#define MacroFindFull(ctype,cls) \
  case cls: return RCVFindModeFull<ctype>(x);
  
static ArrayVector RCVFindModeFull(Array x) {
  switch (x.dataClass()) {
  default: throw Exception("find not defined for reference types");
    MacroExpandCasesAll(MacroFindFull);
  }
}

static ArrayVector FindModeSparse(Array x, int nargout) {
  Array xrows, xcols, xdata;
  xdata = SparseToIJV(x,xrows,xcols);
  NTuple retDim;
  index_t nnz = xdata.length();
  if (x.isRowVector())
    retDim = NTuple(1,nnz);
  else
    retDim = NTuple(nnz,1);
  ArrayVector retval;
  if (nargout == 3) {
    retval.push_back(xrows);
    retval.push_back(xcols);
    retval.push_back(xdata);
  } else if (nargout == 2) {
    retval.push_back(xrows);
    retval.push_back(xcols);
  } else {
    retval.push_back(Add(Multiply(Subtract(xcols,Array(double(1))),
				  Array(double(x.rows()))),xrows));
  }
  return retval;
}
  
//!
//@Module FIND Find Non-zero Elements of An Array
//@@Section ARRAY
//@@Usage
//Returns a vector that contains the indicies of all non-zero elements 
//in an array.  The usage is
//@[
//   y = find(x)
//@]
//The indices returned are generalized column indices, meaning that if 
//the array @|x| is of size @|[d1,d2,...,dn]|, and the
//element @|x(i1,i2,...,in)| is nonzero, then @|y|
//will contain the integer
//\[
//   i_1 + (i_2-1) d_1 + (i_3-1) d_1 d_2 + \dots
//\]
//The second syntax for the @|find| command is
//@[
//   [r,c] = find(x)
//@]
//which returns the row and column index of the nonzero entries of @|x|.
//The third syntax for the @|find| command also returns the values
//@[
//   [r,c,v] = find(x).
//@]
//Note that if the argument is a row vector, then the returned vectors
//are also row vectors. This form is particularly useful for converting 
//sparse matrices into IJV form.
//
//The @|find| command also supports some additional arguments.  Each of the
//above forms can be combined with an integer indicating how many results
//to return:
//@[
//   y = find(x,k)
//@]
//where @|k| is the maximum number of results to return.  This form will return
//the first @|k| results.  You can also specify an optional flag indicating 
//whether to take the first or last @|k| values:
//@[
//   y = find(x,k,'first')
//   y = find(x,k,'last')
//@]
//in the case of the @|'last'| argument, the last @|k| values are returned.
//@@Example
//Some simple examples of its usage, and some common uses of @|find| in FreeMat programs.
//@<
//a = [1,2,5,2,4];
//find(a==2)
//@>
//Here is an example of using find to replace elements of @|A| that are @|0| with the number @|5|.
//@<
//A = [1,0,3;0,2,1;3,0,0]
//n = find(A==0)
//A(n) = 5
//@>
//Incidentally, a better way to achieve the same concept is:
//@<
//A = [1,0,3;0,2,1;3,0,0]
//A(A==0) = 5
//@>
//Now, we can also return the indices as row and column indices using the two argument
//form of @|find|:
//@<
//A = [1,0,3;0,2,1;3,0,0]
//[r,c] = find(A)
//@>
//Or the three argument form of @|find|, which returns the value also:
//@<
//[r,c,v] = find(A)
//@>
//@@Tests
//@$exact#y1=find(x1)
//@$exact#y1=find(x1,2)
//@$exact#y1=find(x1,2,'first')
//@$exact#y1=find(x1,2,'last')
//@$exact#[y1,y2]=find(x1)
//@$exact#[y1,y2]=find(x1,2)
//@$exact#[y1,y2,y3]=find(x1)
//@$exact#[y1,y2,y3]=find(x1,2)
//@{ test_sparse20.m
//function x = test_sparse20
//a = [0,0,3,1,3;1,0,0,0,2;4,3,0,2,0];
//[i,j,v] = find(a);
//B = sparse(i,j,v);
//x = testeq(a,B);
//@}
//@{ test_sparse21.m
//function x = test_sparse21
//ai = [0,2,6,0,1;3,0,3,0,2;0,0,3,0,2];
//ar = [1,2,0,0,4;3,2,0,0,5;0,0,3,0,2];
//a = complex(ar+i*ai);
//[i,j,v] = find(a);
//B = sparse(i,j,v);
//x = testeq(a,B);
//@}
//@{ test_sparse68.m
//% Test sparse to IJV conversion
//function x = test_sparse68
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[iyi1,jyi1,vyi1] = find(yi1);
//[izi1,jzi1,vzi1] = find(zi1);
//[iyf1,jyf1,vyf1] = find(yf1);
//[izf1,jzf1,vzf1] = find(zf1);
//[iyd1,jyd1,vyd1] = find(yd1);
//[izd1,jzd1,vzd1] = find(zd1);
//[iyc1,jyc1,vyc1] = find(yc1);
//[izc1,jzc1,vzc1] = find(zc1);
//[iyz1,jyz1,vyz1] = find(yz1);
//[izz1,jzz1,vzz1] = find(zz1);
//x = testeq(iyi1,izi1) & testeq(jyi1,jzi1) & testeq(vyi1,vzi1);
//x = x & testeq(iyf1,izf1) & testeq(jyf1,jzf1) & testeq(vyf1,vzf1);
//x = x & testeq(iyd1,izd1) & testeq(jyd1,jzd1) & testeq(vyd1,vzd1);
//x = x & testeq(iyc1,izc1) & testeq(jyc1,jzc1) & testeq(vyc1,vzc1);
//x = x & testeq(iyz1,izz1) & testeq(jyz1,jzz1) & testeq(vyz1,vzz1);
//@}
//@{ test_sparse70.m
//% Test sparse to IJV to sparse conversion
//function x = test_sparse70
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[iyi1,jyi1,vyi1] = find(yi1);
//[iyf1,jyf1,vyf1] = find(yf1);
//[iyd1,jyd1,vyd1] = find(yd1);
//[iyc1,jyc1,vyc1] = find(yc1);
//[iyz1,jyz1,vyz1] = find(yz1);
//x = testeq(sparse(iyi1,jyi1,vyi1),zi1);
//x = x & testeq(sparse(iyf1,jyf1,vyf1),zf1);
//x = x & testeq(sparse(iyd1,jyd1,vyd1),zd1);
//x = x & testeq(sparse(iyc1,jyc1,vyc1),zc1);
//x = x & testeq(sparse(iyz1,jyz1,vyz1),zz1);
//@}
//@@Signature
//function find FindFunction
//inputs x keep flag
//outputs row col vals
//!  

static ArrayVector FindTrim(ArrayVector a, int cnt, bool first_flag) {
  if (cnt < 0) return a;
  if (a.size() == 0) return a;
  int N = int(a[0].length());
  if (N == 0) return a;
  if (cnt > N) return a;
  ArrayVector ret;
  Array ndx;
  bool vertflag = !(a[0].isRowVector());
  if (first_flag)
    ndx = RangeConstructor(1,1,cnt,vertflag);
  else
    ndx = RangeConstructor((N-cnt)+1,1,N,vertflag);
  for (int i=0;i<a.size();i++) 
    ret.push_back(a[i].get(ndx));
  return ret;
}

ArrayVector FindFunction(int nargout, const ArrayVector& arg) {
  // Detect the Find mode...
  if (arg.size() < 1)
    throw Exception("find function takes at least one argument");
  Array tmp(arg[0]);
  if (tmp.isScalar()) tmp = tmp.asDenseArray();
  int k = -1;
  bool first_flag = true;
  if (arg.size() > 1)
    k  = arg[1].asInteger();
  if (arg.size() == 3) {
    QString flag = arg[2].asString().toLower();
    if (flag=="first")
      first_flag = true;
    else if (flag=="last")
      first_flag = false;
    else
      throw Exception("third option to find must be either 'first' or 'last'");
  }
  if ((nargout <= 1) && !tmp.isSparse())
    return FindTrim(SingleFindModeFull(tmp),k,first_flag);
  if ((nargout == 2) && !tmp.isSparse())
    return FindTrim(RCFindModeFull(tmp),k,first_flag);
  if ((nargout == 3) && !tmp.isSparse())
    return FindTrim(RCVFindModeFull(tmp),k,first_flag);
  if (nargout > 3)
    throw Exception("Do not understand syntax of find call (too many output arguments).");
  return FindTrim(FindModeSparse(tmp,nargout),k,first_flag);
}
