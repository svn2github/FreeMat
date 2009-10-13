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
#include "Interpreter.hpp"
#include "IEEEFP.hpp"
#include "Operators.hpp"
#include "Struct.hpp"
#include <math.h>
#include <float.h>

//!
//@Module ISSAME Test If Two Arrays Are Identical
//@@Section INSPECTION
//@@Usage
//Tests for two arrays to be identical.  The syntax
//for its use is
//@[
//   y = issame(a,b)
//@]
//where @|a| and @|b| are two arrays to compare.  This
//comparison succeeds only if @|a| and @|b| are of the
//same data type, size, and contents.  Unlike numerical
//equivalence tests, the @|issame| function considers
//@|NaN| to be equal in both arguments.
//@@Signature
//function issame IsSameFunction
//inputs a  b
//outputs y
//!


template <typename T>
static bool IsSame(const T& a, const T& b);

#define MacroDefIsSame(ctype)			       \
  template <>					       \
  bool IsSame(const ctype& a, const ctype& b) {	       \
    return (a == b);				       \
  }

MacroDefIsSame(bool);
MacroDefIsSame(int8);
MacroDefIsSame(uint8);
MacroDefIsSame(int16);
MacroDefIsSame(uint16);
MacroDefIsSame(int32);
MacroDefIsSame(uint32);
MacroDefIsSame(int64);
MacroDefIsSame(uint64);
MacroDefIsSame(QChar);

template <>
bool IsSame(const float& a, const float& b) {
  return ((a == b) || (IsNaN(a) && IsNaN(b)) || 
	  (fabsf(b-a) <= 100*fepsf(qMax(fabsf(a),fabsf(b)))));
}

template <>
bool IsSame(const double& a, const double& b) {
  return ((a == b) || (IsNaN(a) && IsNaN(b)) || 
	  (fabs(b-a) <= 100*feps(qMax(fabs(a),fabs(b)))));
}

template <>
bool IsSame(const Array&, const Array&);

template <typename T>
static bool IsSame(const BasicArray<T>& ar, const BasicArray<T>& br) {
  if (ar.dimensions() != br.dimensions()) return false;
  for (index_t i=1;i<=ar.length();i++)
    if (!IsSame(ar[i],br[i])) 
      return false;
  return true;
}

static bool IsSameStruct(const Array& a, const Array& b) {
  const StructArray &ar(a.constStructPtr());
  const StructArray &br(b.constStructPtr());
  if (ar.fieldNames() != br.fieldNames()) return false;
  if (ar.classPath() != br.classPath()) return false;
  for (int i=0;i<ar.fieldCount();i++)
    if (!IsSame(ar[i],br[i])) return false;
  return true;
}

#define MacroIsSame(ctype,cls)						\
  case cls:								\
  { 									\
  if (ad.allReal() && bd.allReal())					\
    return IsSame(ad.constReal<ctype>(),bd.constReal<ctype>());		\
  else {								\
    Array ac(ad); ac.forceComplex();					\
    Array bc(bd); bc.forceComplex();					\
    return (IsSame(ac.constReal<ctype>(),bc.constReal<ctype>()) &&	\
	    IsSame(ac.constImag<ctype>(),bc.constImag<ctype>()));	\
  }									\
  }									

template <>
bool IsSame(const Array& a, const Array& b) {
  if (a.dataClass() != b.dataClass()) return false;
  if (a.dimensions() != b.dimensions()) return false;
  Array ad(a.asDenseArray());
  Array bd(b.asDenseArray());
  switch (ad.dataClass()) {
  default:
    return false;
  MacroExpandCasesAll(MacroIsSame);
  case Struct:
    return IsSameStruct(a,b);
  }
}

ArrayVector IsSameFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    return ArrayVector(Array(false));
  return ArrayVector(Array(IsSame(arg[0],arg[1])));
}

//!
//@Module ISSET Test If Variable Set
//@@Section INSPECTION
//@@Usage
//Tests for the existence and non-emptiness of a variable.
//the general syntax for its use is
//@[
//   y = isset('name')
//@]
//where @|name| is the name of the variable to test.  This
//is functionally equivalent to 
//@[
//   y = exist('name','var') & ~isempty(name)
//@]
//It returns a @|logical| 1 if the variable is defined 
//in the current workspace, and is not empty, and returns
//a 0 otherwise.
//@@Example
//Some simple examples of using @|isset|
//@<
//who
//isset('a')
//a = [];
//isset('a')
//a = 2;
//isset('a')
//@>
//@@Tests
//@{ test_isset1.m
//function test_val = test_isset1
//  a = []; b = 1;
//  test_val = ~isset('c') && ~isset('a') && isset('b');
//@}
//@@Signature
//sfunction isset IsSetFunction
//inputs name
//outpus flag
//!
ArrayVector IsSetFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("isset function takes at least one argument - the name of the variable to check for");
  QString fname = arg[0].asString();
  bool isDefed;
  ParentScopeLocker lock(eval->getContext());
  ArrayReference d = eval->getContext()->lookupVariable(fname);
  isDefed = (d.valid());
  if (isDefed && !d->isEmpty())
    return ArrayVector(Array(bool(true)));
  else
    return ArrayVector(Array(bool(false)));
}

//!
//@Module ISEQUAL Test For Matrix Equality
//@@Section INSPECTION
//@@Usage
//Test two arrays for equality.  The general format
//for its use is
//@[
//   y = isequal(a,b)
//@]
//This function returns true if the two arrays are
//equal (compared element-wise).  Unlike @|issame|
//the @|isequal| function will type convert where
//possible to do the comparison.
//!

//!
//@Module ISSPARSE Test for Sparse Matrix
//@@Section INSPECTION
//@@Usage
//Test a matrix to see if it is sparse or not.  The general
//format for its use is 
//@[
//   y = issparse(x)
//@]
//This function returns true if @|x| is encoded as a sparse
//matrix, and false otherwise.
//@@Example
//Here is an example of using @|issparse|:
//@<
//a = [1,0,0,5;0,3,2,0]
//issparse(a)
//A = sparse(a)
//issparse(A)
//@>
//@@Signature
//function issparse IsSparseFunction
//inputs x
//outputs flag
//!
ArrayVector IsSparseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("issparse function takes one argument - the array to test");
  return ArrayVector(Array(arg[0].isSparse()));
}

struct OpIsNaN {
  static inline float func(float t) {return (IsNaN(t) ? 1.0 : 0.0);}
  static inline double func(double t) {return (IsNaN(t) ? 1.0 : 0.0);}
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = (IsNaN(x) || IsNaN(y)) ? 1.0 : 0.0; ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = (IsNaN(x) || IsNaN(y)) ? 1.0 : 0.0; ry = 0;
  }
};


//!
//@Module ISNAN Test for Not-a-Numbers
//@@Section INSPECTION
//@@Usage
//Returns true for entries of an array that are NaN's (i.e.,
//Not-a-Numbers).  The usage is
//@[
//   y = isnan(x)
//@]
//The result is a logical array of the same size as @|x|,
//which is true if @|x| is not-a-number, and false otherwise.
//Note that for complex data types that
//the result is true if either the real or imaginary parts
//are NaNs.
//@@Example
//Suppose we have an array of floats with one element that
//is @|nan|:
//@<
//a = [1.2 3.4 nan 5]
//isnan(a)
//@>
//@@Tests
//@$exact#y1=isnan(x1)
//@@Signature
//function isnan IsNaNFunction
//inputs x
//outputs y
//@@Signature
//function IsNaN IsNaNFunction
//inputs x
//outputs y
//!
ArrayVector IsNaNFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
   throw Exception("isnan function takes one argument - the array to test");
  return ArrayVector(UnaryOp<OpIsNaN>(arg[0]).toClass(Bool));
}


struct OpIsInf {
  static inline float func(float t) {return (IsInfinite(t) ? 1.0 : 0.0);}
  static inline double func(double t) {return (IsInfinite(t) ? 1.0 : 0.0);}
  static inline void func(float x, float y, float &rx, float &ry) {
    rx = (IsInfinite(x) || IsInfinite(y)) ? 1.0 : 0.0; ry = 0;
  }
  static inline void func(double x, double y, double &rx, double &ry) {
    rx = (IsInfinite(x) || IsInfinite(y)) ? 1.0 : 0.0; ry = 0;
  }
};

//!
//@Module ISINF Test for infinities
//@@Section INSPECTION
//@@Usage
//Returns true for entries of an array that are infs (i.e.,
//infinities).  The usage is
//@[
//   y = isinf(x)
//@]
//The result is a logical array of the same size as @|x|,
//which is true if @|x| is not-a-number, and false otherwise.
//Note that for @|complex| or @|dcomplex| data types that
//the result is true if either the real or imaginary parts
//are infinite.
//@@Example
//Suppose we have an array of floats with one element that
//is @|inf|:
//@<
//a = [1.2 3.4 inf 5]
//isinf(a)
//b = 3./[2 5 0 3 1]
//@>
//@@Tests
//@$exact#y1=isinf(x1)
//@@Signature
//function isinf IsInfFunction
//inputs x
//outputs y
//@@Signature
//function IsInf IsInfFunction
//inputs x
//outputs y
//!
ArrayVector IsInfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isinf function takes one argument - the array to test");
  return ArrayVector(UnaryOp<OpIsInf>(arg[0]).toClass(Bool));
}

//!
//@Module ISREAL Test For Real Array
//@@Section INSPECTION
//@@Usage
//The syntax for @|isreal| is 
//@[
//   x = isreal(y)
//@]
//and it returns a logical 1 if the argument is real valued
//and a logical 0 otherwise.
//@@Tests
//@$exact#y1=isreal(x1)
//@@Signature
//function isreal IsRealFunction
//inputs x
//outputs y
//!
ArrayVector IsRealFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isreal function takes one argument - the array to test");
  return ArrayVector(Array((arg[0].dataClass() != CellArray) && 
			   (arg[0].allReal())));
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
//@<1
//clear x
//isempty(x)
//@>
//@@Tests
//@$exact#y1=isempty(x1)
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
//@@Signature
//function isempty IsEmptyFunction
//inputs x
//outputs flag
//!
ArrayVector IsEmptyFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("isempty function requires at least input argument");
  return ArrayVector(Array(bool(arg[0].isEmpty())));
}
