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
#include "IEEEFP.hpp"
#include "Math.hpp"
#include "Complex.hpp"

// The GreaterThan function is called by Max
struct OpGreaterThan {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if (IsNaN(v1)) return v2;
    if (IsNaN(v2)) return v1;
    return (v1 > v2) ? v1 : v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    if (IsNaN(ar) || IsNaN(ai)) {
      cr = br; ci = bi;
      return;
    }
    if (IsNaN(br) || IsNaN(bi)) {
      cr = ar; ci = ai;
      return;
    }
    if (complex_gt(ar,ai,br,bi)) {
      cr = ar; ci = ai;
    } else {
      cr = br; ci = bi;
    }
  }
};

struct OpVecMax {
  template <typename T>
  static inline void func(ConstSparseIterator<T> & src, 
			  SparseSlice<T>& dest,
			  SparseSlice<index_t>& dest_index) {
    bool init = false;
    T result = 0;
    index_t count = 0;
    index_t zero_index = 0;
    index_t index = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      if (src.row() != count) zero_index = src.row();
      if (!IsNaN(src.value())) {
	if (!init) {
	  init = true;
	  result = src.value();
	  index = src.row();
	} else {
	  if (result < src.value()) {
	    index = src.row();
	    result = src.value();
	  }
	}
      }
      src.next();
    }
    if (count < src.rows()) {
      if (!init) {
	result = 0;
	index = zero_index;
      } else {
	if (result > 0) {
	  result = 0;
	  index = zero_index;
	}
      }
    }
    dest[1] = result;
    dest_index[1] = index;
  }
  template <typename T>
  static inline void func(ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag,
			  SparseSlice<index_t>& dest_index) {
    bool init = false;
    T result_real = 0;
    T result_imag = 0;
    index_t count = 0;
    index_t zero_index = 0;
    index_t index = 0;
    index_t col = src.col();
    while (src.col() == col) {
      count++;
      if (src.row() != count) zero_index = src.row();
      if (!IsNaN(src.realValue()) && !IsNaN(src.imagValue())) {
	if (!init) {
	  init = true;
	  result_real = src.realValue();
	  result_imag = src.imagValue();
	  index = src.row();
	} else 
	  if (complex_gt(src.realValue(),src.imagValue(),
			 result_real,result_imag)) {
	    result_real = src.realValue();
	    result_imag = src.imagValue();
	    index = src.row();
	  }
      }
      src.next();
    }
    if (count < src.rows()) {
      result_real = 0;
      result_imag = 0;
      index = zero_index;
    }
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
    dest_index[1] = index;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src, 
			  BasicArray<T>& dest,
			  BasicArray<index_t>& dest_index) {
    bool init = false;
    T result = 0;
    index_t index = 0;
    for (index_t i=1;i<=src.length();i++) {
      if (!IsNaN(src[i])) {
	if (!init) {
	  init = true;
	  result = src[i];
	  index = i;
	} else {
	  if (src[i] > result) {
	    index = i;
	    result = src[i];
	  }
	}
      }
    }
    if (!init) {
      result = NaN();
      index = 1;
    }
    dest[1] = result;
    dest_index[1] = index;
  }
  template <typename T>
  static inline void func(const BasicArray<T> & src_real,
			  const BasicArray<T> & src_imag,
			  BasicArray<T>& dest_real,
			  BasicArray<T>& dest_imag,
			  BasicArray<index_t>& dest_index) {
    bool init = false;
    T result_real = 0;
    T result_imag = 0;
    index_t index = 0;
    for (index_t i=1;i<=src_real.length();i++) {
      if (!IsNaN(src_real[i]) && !IsNaN(src_imag[i])) {
	if (!init) {
	  init = true;
	  result_real = src_real[i];
	  result_imag = src_imag[i];
	  index = i;
	} else {
	  if (complex_gt(src_real[i],src_imag[i],
			 result_real,result_imag)) {
	    result_real = src_real[i];
	    result_imag = src_imag[i];
	    index = i;
	  }
	}
      }
    }
    if (!init) {
      result_real = NaN();
      result_imag = NaN();
      index = 1;
    }
    dest_real[1] = result_real;
    dest_imag[1] = result_imag;
    dest_index[1] = index;
  }
};

//!
//@Module MAX Maximum Function
//@@Section ELEMENTARY
//@@Usage
//Computes the maximum of an array along a given dimension, or alternately, 
//computes two arrays (entry-wise) and keeps the smaller value for each array.
//As a result, the @|max| function has a number of syntaxes.  The first
//one computes the maximum of an array along a given dimension.
//The first general syntax for its use is either
//@[
//   [y,n] = max(x,[],d)
//@]
//where @|x| is a multidimensional array of numerical type, in which case the
//output @|y| is the maximum of @|x| along dimension @|d|.  
//The second argument @|n| is the index that results in the maximum.
//In the event that multiple maxima are present with the same value,
//the index of the first maximum is used. 
//The second general syntax for the use of the @|max| function is
//@[
//   [y,n] = max(x)
//@] 
//In this case, the maximum is taken along the first non-singleton 
//dimension of @|x|.  For complex data types,
//the maximum is based on the magnitude of the numbers.  NaNs are
//ignored in the calculations.
//The third general syntax for the use of the @|max| function is as 
//a comparison function for pairs of arrays.  Here, the general syntax is
//@[
//   y = max(x,z)
//@]
//where @|x| and @|z| are either both numerical arrays of the same dimensions,
//or one of the two is a scalar.  In the first case, the output is the 
//same size as both arrays, and is defined elementwise by the smaller of the
//two arrays.  In the second case, the output is defined elementwise by the 
//smaller of the array entries and the scalar.
//@@Function Internals
//In the general version of the @|max| function which is applied to
//a single array (using the @|max(x,[],d)| or @|max(x)| syntaxes),
//The output is computed via
//\[
//y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
//\max_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}),
//\]
//and the output array @|n| of indices is calculated via
//\[
//n(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = \arg
//\max_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
//\]
//In the two-array version (@|max(x,z)|), the single output is computed as
//\[
//  y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
//\begin{cases}
//  x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) & x(\cdots) \leq z(\cdots) \\    z(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) & z(\cdots) < x(\cdots).
//\end{cases}
//\]
//@@Example
//The following piece of code demonstrates various uses of the maximum
//function.  We start with the one-array version.
//@<
//A = [5,1,3;3,2,1;0,3,1]
//@>
//We first take the maximum along the columns, resulting in a row vector.
//@<
//max(A)
//@>
//Next, we take the maximum along the rows, resulting in a column vector.
//@<
//max(A,[],2)
//@>
//When the dimension argument is not supplied, @|max| acts along the first non-singular dimension.  For a row vector, this is the column direction:
//@<
//max([5,3,2,9])
//@>
//
//For the two-argument version, we can compute the smaller of two arrays,
//as in this example:
//@<
//a = int8(100*randn(4))
//b = int8(100*randn(4))
//max(a,b)
//@>
//Or alternately, we can compare an array with a scalar
//@<
//a = randn(2)
//max(a,0)
//@>
//@@Tests
//@$exact#y1=max(x1)
//@$exact#y1=max(x1,[],2)
//@$exact#y1=max(x1,x2)
//@$exact#[y1,y2]=max(x1)
//@$exact#[y1,y2]=max(x1,[],2)
//@$exact#[y1,y2]=max(x1,x2)
//@@Signature
//function max MaxFunction jitsafe
//input x z dim
//output y n
//!
ArrayVector MaxFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1 || arg.size() > 3)
    throw Exception("max requires at least one argument, and at most three arguments");
  // Determine if this is a call to the Max function or the LessThan function
  // (the internal version of the two array min function)
  if (arg.size() == 2) {
    Array ret(DotOp<OpGreaterThan>(arg[0],arg[1]));
    if ((arg[0].dataClass() == Bool) && (arg[1].dataClass() == Bool))
      return ArrayVector(ret.toClass(Bool));
    return ArrayVector(ret);
  }
  int dim;
  if (arg.size() > 2)
    dim = arg[2].asInteger()-1;
  else
    dim = arg[0].dimensions().firstNonsingular();
  if (arg[0].isEmpty()) {
    NTuple dims(arg[0].dimensions());
    if (dims == NTuple(0,0)) return ArrayVector(arg[0]);
    if (dims[dim] != 0)
      dims[dim] = 1;
    Array ret(arg[0].dataClass(),dims);
    ArrayVector p;
    p.push_back(ret); p.push_back(ret);
    return p;
  }
  return BiVectorOp<OpVecMax>(arg[0],1,dim);
}

JitScalarFunc2(max,OpGreaterThan::func);

