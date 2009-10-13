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
#include "Operators.hpp"
#include "Math.hpp"
#include "Complex.hpp"

struct OpVecVar {
  template <typename T>
  static inline void func(const ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest) {
    throw Exception("Variance not implemented for sparse matrices");
  }
  template <typename T>
  static inline void func(const ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    throw Exception("Variance not implemented for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest) {
    if ((src.length() == 1) && (!IsFinite(src[1]))) {
      dest[1] = NaN();
      return;
    }
    T accum = 0;
    for (index_t i=1;i<=src.length();i++)
      accum += src[i];
    T mean = accum/src.length();
    accum = 0;
    if (src.length() > 1) {
      T normalizer = 1.0/(src.length()-1.0);
      for (index_t i=1;i<=src.length();i++) {
	T tmp = src[i] - mean;
	accum += tmp*tmp*normalizer;
      }
    }
    dest[1] = accum;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag) {
    if ((src_real.length() == 1) && 
	(!IsFinite(src_real[1]) || !IsFinite(src_imag[1]))) {
      dest_real[1] = NaN();
      dest_imag[1] = 0;
      return;
    }
    T accum_real = 0;
    T accum_imag = 0;
    for (index_t i=1;i<=src_real.length();i++) {
      accum_real += src_real[i];
      accum_imag += src_imag[i];
    }
    T mean_real = accum_real/src_real.length();
    T mean_imag = accum_imag/src_imag.length();
    T accum = 0;
    if (src_real.length() > 1) {
      T normalizer = 1.0/(src_real.length()-1.0);
      for (index_t i=1;i<=src_real.length();i++) {
	T tmp_real = src_real[i] - mean_real;
	T tmp_imag = src_imag[i] - mean_imag;
	T tmp_val = complex_abs(tmp_real,tmp_imag);
	accum += tmp_val*tmp_val*normalizer;
      }
    }
    dest_real[1] = accum;
    dest_imag[1] = 0;
  }
};

//!
//@Module VAR Variance Function
//@@Section ELEMENTARY
//@@Usage
//Computes the variance of an array along a given dimension.  The general
//syntax for its use is
//@[
//  y = var(x,d)
//@]
//where @|x| is an @|n|-dimensions array of numerical type.
//The output is of the same numerical type as the input.  The argument
//@|d| is optional, and denotes the dimension along which to take
//the variance.  The output @|y| is the same size as @|x|, except
//that it is singular along the mean direction.  So, for example,
//if @|x| is a @|3 x 3 x 4| array, and we compute the mean along
//dimension @|d=2|, then the output is of size @|3 x 1 x 4|.
//@@Function Internals
//The output is computed via
//\[
//y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = \frac{1}{N-1}
//\sum_{k=1}^{N} \left(x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) 
// - \bar{x}\right)^2,
//\]
//where 
//\[
//\bar{x}  = \frac{1}{N}
//\sum_{k=1}^{N} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
//\]
//If @|d| is omitted, then the mean is taken along the 
//first non-singleton dimension of @|x|. 
//@@Example
//The following piece of code demonstrates various uses of the var
//function
//@<
//A = [5,1,3;3,2,1;0,3,1]
//@>
//We start by calling @|var| without a dimension argument, in which 
//case it defaults to the first nonsingular dimension (in this case, 
//along the columns or @|d = 1|).
//@<
//var(A)
//@>
//Next, we take the variance along the rows.
//@<
//var(A,2)
//@>
//@@Tests
//@$near#y1=var(x1)
//@$near#y1=var(x1,2)
//@@Signature
//function var VarFunction
//inputs x dim
//outputs y
//!
ArrayVector VarFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("mean requires at least one argument");
  Array input(arg[0]);
  if (input.dimensions() == NTuple(0,0)) 
    return ArrayVector(Array(NaN()));
  int dim;
  if (arg.size() > 1)
    dim = arg[1].asInteger()-1;
  else
    dim = input.dimensions().firstNonsingular();
  return ArrayVector(VectorOp<OpVecVar>(input,1,dim));
}
