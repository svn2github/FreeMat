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
#include "Struct.hpp"
#include "MemPtr.hpp"
#include <QtCore>
#include "Algorithms.hpp"
#include "FuncPtr.hpp"
#include "AnonFunc.hpp"

//!
//@Module PERMUTE Array Permutation Function
//@@Section ARRAY
//@@Usage
//The @|permute| function rearranges the contents of an array according
//to the specified permutation vector.  The syntax for its use is
//@[
//    y = permute(x,p)
//@]
//where @|p| is a permutation vector - i.e., a vector containing the 
//integers @|1...ndims(x)| each occuring exactly once.  The resulting
//array @|y| contains the same data as the array @|x|, but ordered
//according to the permutation.  This function is a generalization of
//the matrix transpose operation.
//@@Example
//Here we use @|permute| to transpose a simple matrix (note that permute
//also works for sparse matrices):
//@<
//A = [1,2;4,5]
//permute(A,[2,1])
//A'
//@>
//Now we permute a larger n-dimensional array:
//@<
//A = randn(13,5,7,2);
//size(A)
//B = permute(A,[3,4,2,1]);
//size(B)
//@>
//@@Tests
//@$exact#y1=permute(x1,[2,1])
//@$exact#y1=size(permute(x1,[3,4,2,1]))
//@{ test_permute1.m
//function test_val = test_permute1
//z = rand(3,5,2,4,7);
//perm = [3,5,1,4,2];
//sizez = size(z);
//y = permute(z,perm);
//sizey = size(y);
//test_val = all(sizey == sizez(perm));
//@}
//@{ test_permute2.m
//function test_val = test_permute2
//z = rand(3,5,2,4,7);
//perm = [3,5,1,4,2];
//y = ipermute(permute(z,perm),perm);
//test_val = all(y == z);
//@}
//@@Signature
//function permute PermuteFunction jitsafe
//inputs x p
//outputs y
//!
ArrayVector PermuteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("permute requires 2 inputs, the array to permute, and the permutation vector");
  Array permutation(arg[1].asDenseArray().toClass(UInt32));
  const BasicArray<uint32> &perm_dp(permutation.constReal<uint32>());
  uint32 max_perm_value = MaxValue(perm_dp);
  uint32 min_perm_value = MinValue(perm_dp);
  if ((max_perm_value != permutation.length()) || (min_perm_value != 1))
    throw Exception("second argument is not a valid permutation");
  MemBlock<bool> p(max_perm_value);
  bool *d = &p;
  for (index_t i=1;i<=perm_dp.length();i++) 
    d[perm_dp[i]-1] = true;
  for (uint32 i=0;i<max_perm_value;i++)
    if (!d[i]) throw Exception("second argument is not a valid permutation");
  // Convert to an N-Tuple
  NTuple perm(ConvertArrayToNTuple(permutation));
  // Post-fill the N-Tuple so that the permutation covers all of the dimensions
  for (int i=permutation.length();i<NDims;i++)
    perm[i] = (i+1);
  return ArrayVector(Permute(arg[0],perm));
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
//@@Tests
//@$exact#y1=repmat(x1,[1,1,3])
//@$exact#y1=repmat(x1,[5,1])
//@$exact#y1=repmat(x1,1,2)
//@$exact#y1=repmat(x1,2,1)
//@{ test_repmat1.m
//function test_val = test_repmat1
//  s = ones(2,2,1);
//  p = repmat(s,[2 2]);
//  test_val = all(p == ones(4));
//@}
//@{ test_repmat2.m
//function test_val = test_repmat2
//  s = ones(2,2);
//  p = repmat(s,[2 2 1]);
//  test_val = all(p == ones(4));
//@}
//@{ test_repmat3.m
//function test_val = test_repmat3
//  s = ones(2,2,2);
//  p = repmat(s,[2 2 1]);
//  test_val = all(p == ones(4,4,2));
//@}
//@@Signature
//function repmat RepMatFunction jitsafe
//inputs x rows cols
//outputs y
//!

template <typename T>
static BasicArray<T> RepMat(const BasicArray<T> &dp, const NTuple &outdim, const NTuple &repcount) {
  // Copy can work by pushing or by pulling.  I have opted for
  // pushing, because we can push a column at a time, which might
  // be slightly more efficient.
  index_t colsize = dp.rows();
  index_t colcount = dp.length()/colsize;
  // copySelect stores which copy we are pushing.
  NTuple originalSize(dp.dimensions());
  NTuple copySelect(1,1);
  // anchor is used to calculate where this copy lands in the output matrix
  // sourceAddress is used to track which column we are pushing in the
  // source matrix
  index_t copyCount = repcount.count();
  BasicArray<T> x(outdim);
  for (index_t i=1;i<=copyCount;i++) {
    // Reset the source address
    NTuple sourceAddress(1,1);
    // Next, we loop over the columns of the source matrix
    for (index_t j=1;j<=colcount;j++) {
      NTuple anchor;
      // We can calculate the anchor of this copy by multiplying the source
      // address by the copySelect vector
      for (int k=0;k<NDims;k++)
	anchor[k] = (copySelect[k]-1)*originalSize[k]+sourceAddress[k];
      // Now, we map this to a point in the destination array
      index_t destanchor = outdim.map(anchor);
      // And copy the elements
      for (index_t n=1;n<=colsize;n++)
	x[destanchor+n-1] = dp[(j-1)*colsize+n];
      // Now increment the source address
      originalSize.increment(sourceAddress,0);
    }
    repcount.increment(copySelect);
  }
  return x;
}

template <typename T>
static SparseMatrix<T> RepMat(const SparseMatrix<T>& dp, const NTuple &outdim, 
			      const NTuple &repcount) {
  if (repcount.lastNotOne() > 2)
    throw Exception("repmat cannot create N-dimensional sparse arrays");
  SparseMatrix<T> retvec(outdim);
  for (int rowcopy=0;rowcopy < repcount[0];rowcopy++)
    for (int colcopy=0;colcopy < repcount[1];colcopy++) {
      ConstSparseIterator<T> iter(&dp);
      while (iter.isValid()) {
	retvec.set(NTuple(iter.row()+rowcopy*dp.rows(),
			  iter.col()+colcopy*dp.cols()),
		   iter.value());
	iter.next();
      }
    }
  return retvec;
}

template <typename T>
static Array RepMat(const Array &dp, const NTuple &outdim, const NTuple &repcount) {
  if (dp.isScalar()) {
    if (dp.allReal()) 
      return Array(Uniform(outdim,dp.constRealScalar<T>()));
    else
      return Array(Uniform(outdim,dp.constRealScalar<T>()),
		   Uniform(outdim,dp.constImagScalar<T>()));
  }
  if (dp.isSparse()) {
    if (dp.allReal())
      return Array(RepMat(dp.constRealSparse<T>(),outdim,repcount));
    else
      return Array(RepMat(dp.constRealSparse<T>(),outdim,repcount),
		   RepMat(dp.constImagSparse<T>(),outdim,repcount));
  }
  if (dp.allReal())
    return Array(RepMat(dp.constReal<T>(),outdim,repcount));
  else
    return Array(RepMat(dp.constReal<T>(),outdim,repcount),
		 RepMat(dp.constImag<T>(),outdim,repcount));
}

static Array RepMatCell(const Array &dp, const NTuple &outdim, const NTuple &repcount) {
  return Array(RepMat<Array>(dp.constReal<Array>(),outdim,repcount));
}

static Array RepMatStruct(const StructArray& dp, const NTuple &outdim, const NTuple &repcount) {
  StructArray ret(dp);
  for (int i=0;i<ret.fieldCount();i++)
    ret[i] = RepMat<Array>(ret[i],outdim,repcount);
  ret.updateDims();
  return Array(ret);
}

#define MacroRepMat(ctype,cls)					\
  case cls: return ArrayVector(RepMat<ctype>(x,outdims,repcount));

ArrayVector RepMatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("repmat function requires at least two arguments");
  Array x(arg[0]);
  NTuple repcount;
  // Case 1, look for a scalar second argument
  if ((arg.size() == 2) && (arg[1].isScalar())) {
    Array t(arg[1]);
    repcount[0] = t.asInteger();
    repcount[1] = t.asInteger();
  } 
  // Case 2, look for two scalar arguments
  else if ((arg.size() == 3) && (arg[1].isScalar()) && (arg[2].isScalar())) {
    repcount[0] = arg[1].asInteger();
    repcount[1] = arg[2].asInteger();
  }
  // Case 3, look for a vector second argument
  else {
    if (arg.size() > 2) throw Exception("unrecognized form of arguments for repmat function");
    repcount = ConvertArrayToNTuple(arg[1]);
  }
  if (!repcount.isValid())
    throw Exception("negative replication counts not allowed in argument to repmat function");
  // All is peachy.  Allocate an output array of sufficient size.
  NTuple outdims;
  for (int i=0;i<NDims;i++)
    outdims[i] = x.dimensions()[i]*repcount[i];
  if (x.isEmpty()) {
    Array p(arg[0]);
    p.reshape(outdims);
    return ArrayVector(p);
  }
  switch (x.dataClass()) {
  default: throw Exception("Unhandled type for repmat");
    MacroExpandCasesNoCell(MacroRepMat);
  case CellArray:
    return ArrayVector(RepMatCell(x,outdims,repcount));
  case Struct:
    return ArrayVector(RepMatStruct(x.constStructPtr(),outdims,repcount));
  }
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
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = testeq(diag(yi1,30),diag(zi1,30)) & testeq(diag(yf1,30),diag(zf1,30)) & testeq(diag(yd1,30),diag(zd1,30)) & testeq(diag(yc1,30),diag(zc1,30)) & testeq(diag(yz1,30),diag(zz1,30));
//@}
//@@Signature
//function diag DiagFunction jitsafe
//inputs x n
//outputs y
//!
ArrayVector DiagFunction(int nargout, const ArrayVector& arg) {
  // First, get the diagonal order, and synthesize it if it was
  // not supplied
  int diagonalOrder;
  if (arg.size() == 0)
    throw Exception("diag requires at least one argument.\n");
  if (arg.size() == 1) 
    diagonalOrder = 0;
  else {
    if (!arg[1].isScalar()) 
      throw Exception("second argument must be a scalar.\n");
    diagonalOrder = arg[1].asInteger();
  }
  // Next, make sure the first argument is 2D
  if (!arg[0].is2D()) 
    throw Exception("First argument to 'diag' function must be 2D.\n");
  // Case 1 - if the number of columns in a is 1, then this is a diagonal
  // constructor call.
  if (arg[0].isVector())
    {
      Array a = arg[0];
      a.ensureNotScalarEncoded();
      return ArrayVector(DiagonalArray(a,diagonalOrder));
    }
  else
    return ArrayVector(GetDiagonal(arg[0],diagonalOrder));
}

//!
//@Module CELLFUN Appy a Function To Elements of a Cell Array
//@@Section ARRAY
//@@Usage
//The @|cellfun| function is used to apply a function handle
//(or anonymous function) to each element of a cell array and to
//collect the outputs into an array.  The general syntax for its
//use is 
//@[
//   y = cellfun(fun, x)
//@]
//where @|x| is an N-dimensional array.  In this case, each
//element of the output @|y_i| is defined as @|fun(x{i})|.  You can
//also supply multiple arguments to @|cellfun|, provided all of the
//arguments are the same size
//@[
//   y = cellfun(fun, x, z, ...)
//@]
//in which case each output @|y_i| is defined as @|fun(x{i},z{i},...)|.
//Note that unlike @|arrayfun|, the @|cellfun| function will allow for
//different types (if there are overloaded versions of the function
//@|fun|) for each element.
//
//If the function returns multiple outputs, then @|arrayfun| can be
//called with multiple outputs, in which case each output goes to
//a separate array output
//@[
//   [y1,y2,...] = cellfun(fun, x, z, ...)
//@]
//The assumption is that the output types for each call to @|fun| is
//the same across the inputs.
//
//Finally, some hints can be provided to @|cellfun| using the syntax
//@[
//   [y1,y2,...] = cellfun(fun, x, z, ..., 'param', value, 'param', value)
//@]
//where @|param| and @|value| take on the following possible values:
//\begin{itemize}
//  \item @|'UniformOutput'| - if the @|value| is @|true| then each output of @|fun|
//   must be a scalar, and the outputs are concatenated into an array the same size
//   as the input arrays.  If the @|value| is @|false| then the outputs are encapsulated
//   into a cell array, with each entry in the cell array containing the call to 
//   @|fun(x_i,z_i,...)|.
//  \item @|'ErrorHandler'| - in this case @|value| is a function handle that gets called
//  when @|fun| throws an error.  If @|'ErrorHandler'| is not specified, then @|arrayfun|
//  allows the error to propogate (i.e., and exception is thrown).
//\end{itemize}
//@@Signature
//sfunction cellfun CellFunFunction
//inputs varargin
//output varargout
//!
static ArrayVector CellFunNonuniformAnon(int nargout, const ArrayVector &arg,
					  Interpreter *eval, NTuple argdims,
					  int argcount, Array fun)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector CellFunNonuniform(int nargout, const ArrayVector &arg,
				      Interpreter *eval, NTuple argdims,
				      int argcount, FuncPtr fptr)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = fptr->evaluateFunc(eval,input,fptr->outputArgCount());
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector CellFunUniformAnon(int nargout, const ArrayVector &arg,
				       Interpreter *eval, NTuple argdims,
				       int argcount, Array fun)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

static ArrayVector CellFunUniform(int nargout, const ArrayVector &arg,
				   Interpreter *eval, NTuple argdims,
				   int argcount, FuncPtr fptr)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(ArrayFromCellArray(arg[j].get(i+1)));
      ArrayVector ret = fptr->evaluateFunc(eval,input,nargout);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

ArrayVector CellFunFunction(int nargout, const ArrayVector& arg, 
			     Interpreter*eval) {
  if (arg.size() < 2) return ArrayVector(); // Don't bother
  // Remove the key/value properties
  int argcount = arg.size();
  Array errorHandler;
  bool uniformOutput = true; // We assume this to be the case
  bool foundNVP = true;
  bool customEH = false;
  while (foundNVP && (argcount >=2))
    {
      foundNVP = false;
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "UniformOutput"))
	{
	  uniformOutput = arg[argcount-1].asInteger();
	  argcount-=2;
	  foundNVP = true;
	}
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "ErrorHandler"))
	{
	  errorHandler = arg[argcount-1];
	  customEH = true;
	  argcount-=2;
	  foundNVP = true;
	}
    }
  if (argcount < 2) return ArrayVector();
  NTuple argdims = arg[1].dimensions();
  for (int i=1;i<argcount;i++)
    {
      if (arg[i].dimensions() != argdims)
	throw Exception("All arguments must match dimensions");
      if (arg[i].dataClass() != CellArray)
	throw Exception("All arguments must be cell arrays");
    }
  FuncPtr eh;
  if (customEH) 
    {
      eh = FuncPtrLookup(eval,errorHandler);
      eh->updateCode(eval);
      eval->setTryCatchActive(true);
    }
  try
    {
      if (arg[0].className() == "anonfunction")
	{
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return CellFunUniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	  return CellFunNonuniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	}
      else
	{
	  // Map the first argument to a function ptr
	  FuncPtr fptr = FuncPtrLookup(eval,arg[0]);
	  fptr->updateCode(eval);
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return CellFunUniform(nargout,arg,eval,argdims,argcount,fptr);
	  return CellFunNonuniform(nargout,arg,eval,argdims,argcount,fptr);
	}
    }
  catch (Exception &e)
    {
      if (customEH)
	{
	  ArrayVector input;
	  return eh->evaluateFunc(eval,input,1);
	}
      else
	throw;
    }
}


//!
//@Module ARRAYFUN Apply a Function To Elements of an Array
//@@Section ARRAY
//@@Usage
//The @|arrayfun| function is used to apply a function handle
//to each element of an input array (or arrays), and to collect
//the outputs into an array.  The general syntax for its use is
//@[
//   y = arrayfun(fun, x)
//@]
//where @|x| is an N-dimensional array.  In this case, each 
//element of the output @|y_i| is defined as @|fun(x_i)|.  You can
//also supply multiple arguments to @|arrayfun|, provided all of the
//arguments are the same size
//@[
//   y = arrayfun(fun, x, z,...)
//@]
//in which case each output @|y_i = fun(x_i,z_i,...)|.
//
//If the function returns multiple outputs, then @|arrayfun| can be
//called with multiple outputs, in which case each output goes to
//a separate array output
//@[
//   [y1,y2,...] = arrayfun(fun, x, z, ...)
//@]
//The assumption is that the output types for each call to @|fun| is
//the same across the inputs.
//
//Finally, some hints can be provided to @|arrayfun| using the syntax
//@[
//   [y1,y2,...] = arrayfun(fun, x, z, ..., 'param', value, 'param', value)
//@]
//where @|param| and @|value| take on the following possible values:
//\begin{itemize}
//  \item @|'UniformOutput'| - if the @|value| is @|true| then each output of @|fun|
//   must be a scalar, and the outputs are concatenated into an array the same size
//   as the input arrays.  If the @|value| is @|false| then the outputs are encapsulated
//   into a cell array, with each entry in the cell array containing the call to 
//   @|fun(x_i,z_i,...)|.
//  \item @|'ErrorHandler'| - in this case @|value| is a function handle that gets called
//  when @|fun| throws an error.  If @|'ErrorHandler'| is not specified, then @|arrayfun|
//  allows the error to propogate (i.e., and exception is thrown).
//\end{itemize}
//@@Signature
//sfunction arrayfun ArrayFunFunction
//inputs varargin
//output varargout
//!

static ArrayVector ArrayFunNonuniformAnon(int nargout, const ArrayVector &arg,
					  Interpreter *eval, NTuple argdims,
					  int argcount, Array fun)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector ArrayFunNonuniform(int nargout, const ArrayVector &arg,
				      Interpreter *eval, NTuple argdims,
				      int argcount, FuncPtr fptr)
{
  Array outputs(CellArray, argdims);
  BasicArray<Array> &op = outputs.real<Array>();
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = fptr->evaluateFunc(eval,input,fptr->outputArgCount());
      Array g = CellArrayFromArrayVector(ret);
      op[i+1] = g;
    }
  return outputs;  
}

static ArrayVector ArrayFunUniformAnon(int nargout, const ArrayVector &arg,
				       Interpreter *eval, NTuple argdims,
				       int argcount, Array fun)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      input.push_back(fun);
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = AnonFuncFevalFunction(nargout,input,eval);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

static ArrayVector ArrayFunUniform(int nargout, const ArrayVector &arg,
				   Interpreter *eval, NTuple argdims,
				   int argcount, FuncPtr fptr)
{
  ArrayVector outputs;
  for (int i=0;i<argdims.count();i++)
    {
      ArrayVector input;
      for (int j=1;j<argcount;j++)
	input.push_back(arg[j].get(i+1));
      ArrayVector ret = fptr->evaluateFunc(eval,input,nargout);
      if (ret.size() < nargout)
	throw Exception("function returned fewer outputs than expected");
      if (i==0)
	for (int j=0;j<nargout;j++)
	  {
	    if (!ret[j].isScalar()) throw Exception("function returned non-scalar result");
	    outputs.push_back(ret[j]);
	    outputs[j].resize(argdims);
	  }
      else
	for (int j=0;j<nargout;j++)
	  outputs[j].set(i+1,ret[j]);
    }
  return outputs;
}

ArrayVector ArrayFunFunction(int nargout, const ArrayVector& arg, 
			     Interpreter*eval) {
  if (arg.size() < 2) return ArrayVector(); // Don't bother
  // Remove the key/value properties
  int argcount = arg.size();
  Array errorHandler;
  bool uniformOutput = true; // We assume this to be the case
  bool foundNVP = true;
  bool customEH = false;
  while (foundNVP && (argcount >=2))
    {
      foundNVP = false;
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "UniformOutput"))
	{
	  uniformOutput = arg[argcount-1].asInteger();
	  argcount-=2;
	  foundNVP = true;
	}
      if (arg[argcount-2].isString() &&
	  (arg[argcount-2].asString() == "ErrorHandler"))
	{
	  errorHandler = arg[argcount-1];
	  customEH = true;
	  argcount-=2;
	  foundNVP = true;
	}
    }
  if (argcount < 2) return ArrayVector();
  NTuple argdims = arg[1].dimensions();
  for (int i=1;i<argcount;i++)
    if (arg[i].dimensions() != argdims)
      throw Exception("All arguments must match dimensions");
  FuncPtr eh;
  if (customEH) 
    {
      eh = FuncPtrLookup(eval,errorHandler);
      eh->updateCode(eval);
      eval->setTryCatchActive(true);
    }
  try
    {
      if (arg[0].className() == "anonfunction")
	{
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return ArrayFunUniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	  return ArrayFunNonuniformAnon(nargout,arg,eval,argdims,argcount,arg[0]);
	}
      else
	{
	  // Map the first argument to a function ptr
	  FuncPtr fptr = FuncPtrLookup(eval,arg[0]);
	  fptr->updateCode(eval);
	  if (nargout == 0) nargout = 1;
	  if (uniformOutput)
	    return ArrayFunUniform(nargout,arg,eval,argdims,argcount,fptr);
	  return ArrayFunNonuniform(nargout,arg,eval,argdims,argcount,fptr);
	}
    }
  catch (Exception &e)
    {
      if (customEH)
	{
	  ArrayVector input;
	  return eh->evaluateFunc(eval,input,1);
	}
      else
	throw;
    }
}
