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

#include "Operators.hpp"

struct OpVecCumProd {
  template <typename T>
  static inline void func(const ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest) {
    throw Exception("cumprod not supported for sparse matrices");
  }
  template <typename T>
  static inline void func(const ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    throw Exception("cumprod not supported for sparse matrices");
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest) {
    if (src.length() == 0) return;
    dest[1] = src[1];
    for (index_t i=2;i<=src.length();i++)
      dest[i] = src[i]*dest[i-1];
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag) {
    if (src_real.length() == 0) return;
    dest_real[1] = src_real[1];
    dest_imag[1] = src_imag[1];
    for (index_t i=2;i<=src_real.length();i++) {
      dest_real[i] = src_real[i]*dest_real[i-1] - src_imag[i]*dest_imag[i-1];
      dest_imag[i] = src_real[i]*dest_imag[i-1] + src_imag[i]*dest_real[i-1];
    }
  }
};

//!
//@Module CUMPROD Cumulative Product Function
//@@Section ELEMENTARY
//@@Usage
//Computes the cumulative product of an n-dimensional array along a given
//dimension.  The general syntax for its use is
//@[
//  y = cumprod(x,d)
//@]
//where @|x| is a multidimensional array of numerical type, and @|d|
//is the dimension along which to perform the cumulative product.  The
//output @|y| is the same size of @|x|.  Integer types are promoted
//to @|int32|. If the dimension @|d| is not specified, then the
//cumulative sum is applied along the first non-singular dimension.
//@@Function Internals
//The output is computed via
//\[
//  y(m_1,\ldots,m_{d-1},j,m_{d+1},\ldots,m_{p}) = 
//  \prod_{k=1}^{j} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}).
//\]
//@@Example
//The default action is to perform the cumulative product along the
//first non-singular dimension.
//@<
//A = [5,1,3;3,2,1;0,3,1]
//cumprod(A)
//@>
//To compute the cumulative product along the columns:
//@<
//cumprod(A,2)
//@>
//The cumulative product also works along arbitrary dimensions
//@<
//B(:,:,1) = [5,2;8,9];
//B(:,:,2) = [1,0;3,0]
//cumprod(B,3)
//@>
//@@Tests
//@$exact#y1=cumprod(x1)
//@@Signature
//function cumprod CumprodFunction
//inputs x dimensions
//outputs y
//!

ArrayVector CumprodFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("cumprod requires at least one argument");
  Array input(arg[0]);
  int dim;
  if (arg.size() > 1)
    dim = arg[1].asInteger()-1;
  else
    dim = input.dimensions().firstNonsingular();
  return ArrayVector(VectorOp<OpVecCumProd>(input,
					    int(input.dimensions()[dim]),
					    dim));
}
