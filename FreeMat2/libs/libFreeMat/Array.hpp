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

#ifndef __Array_hpp__
#define __Array_hpp__
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "Dimensions.hpp"
#include "Types.hpp"
#include "Data.hpp"
#include "RefVec.hpp"
#include <QVector>
#include <QList>
#include <QSharedDataPointer>

class Array;
class Interpreter;

//typedef std::vector<Array> ArrayVector;
//typedef RefVec<Array> ArrayVector;

typedef QList<Array> ArrayVector;

// ArrayVector operator+(Array a, Array b);
// ArrayVector operator+(ArrayVector a, Array b);
// ArrayVector operator+(Array a, Array b);

//class ArrayVector;
ArrayVector singleArrayVector(Array);

//typedef std::vector<ArrayVector> ArrayMatrix;

// class ArrayVector : public QList<Array> {
// public:
//   ArrayVector(Array a) {push_back(a);}
//   ArrayVector(Array a, Array b) {push_back(a); push_back(b);}
//   ArrayVector(Array a, Array b, Array c) {push_back(a); push_back(b); push_back(c);}
// };

typedef QVector<ArrayVector> ArrayMatrix;

class FunctionDef;

static Dimensions zeroDim(0,0);

/** Ordered data array, the base FreeMat data type.
 * The Array class is the base class of all data types.  It represents
 * an ordered collection of data, indexed using an arbitrary number of
 * dimensions.  The Array class uses a seperate data class to store the
 * data.  It can contain an N-dimensional array of any of the following
 * data types:
 *   - FM_CELL_ARRAY - a heterogenous array - essentially an array of Arrays
 *   - FM_STRUCT_ARRAY - a structure array
 *   - FM_UINT8 - unsigned, 8-bit integers
 *   - FM_INT8  - signed, 8-bit integers
 *   - FM_UINT16 - unsigned, 16-bit integers
 *   - FM_INT16 - signed, 16-bit integers
 *   - FM_UINT32 - unsigned, 32-bit integers
 *   - FM_INT32 - signed, 32-bit integers
 *   - FM_UINT64 - unsigned, 64-bit integers
 *   - FM_INT64 - signed, 64-bit integers
 *   - FM_FLOAT - 32-bit floating point
 *   - FM_DOUBLE - 64-bit floating point
 *   - FM_COMPLEX - 32-bit complex floating point
 *   - FM_DCOMPLEX - 64-bit complex floating point
 *   - FM_STRING - a string class
 * 
 * The Dimensions class is used to record the dimensions of the given Array.
 * The Dimension class represents an n-tuple of integers that record the
 * number of elements along the array in each dimension.  The data is stored
 * in a generalization of "column-major" order.  Note that the array
 * class does \em not actually include the data - the data is stored in the
 * Data class, to which Array contains a pointer.  This design allows for
 * rapid copying of Array objects.
 */
class Array {
private:
  /**
   * This is a pointer to our data object - which is shared between
   * different Array objects.  It is essentially the memory block
   * directly associated with this data object.
   */
  QSharedDataPointer<Data> dp;
  /**
   * Add another fieldname to our structure array.
   */
  int insertFieldName(std::string fieldName);
  /** Get a binary map from an array over the given range.
   * This member function converts an array into a boolean vector, 
   * mathematically $$b(a(i)-1) = \mathrm{true}, 0 \leq i < \mathrm{maxD}$$, 
   * where $a(i)$ is the contents of the array (after calling 
   * Array::toOrdinal).  This is essentially a shortcut to converting
   * the array to a logical array and then converting to a boolean
   * array.  Throws an exception if $$a(i)$$ is outside the range
   * $$1,\ldots,\mathrm{maxD}$.
   */
  bool* getBinaryMap(uint32);
  /** Get the internal index corresponding to a given field name.
   * Get the internal index corresponding to a given field name.  This
   * is the index into the fieldname array of the argument.  If the
   * argument is not found, a value of -1 is returned.
   */
  int32 getFieldIndex(std::string fieldName);
public:
  /** Compute the maximum index.
   * This computes the maximum value of the array as an index (meaning
   * that it must be greater than 0.  Because this is an internal function, it
   * assumes that the variable has already been passed through toOrdinalType()
   * successfully.  Throws an exception if the maximum value is zero or
   * negative.
   */
  uint32 getMaxAsIndex();
  /**
   * Allocate an array.
   */
  static void* allocateArray(Class, uint32 length,rvstring names = rvstring());
  /** Convert us to an index type
   * Convert the current object to an ordinal one.  This has different
   * meanings for different data types.  
   *  - For string types, this conversion is not defined. 
   *  - For logical types, this is accomplished using a linear search (done in 
   *      two passes - one to identify the length of
   *      the final array, and another to identify the indices of non-zero values.
   *  - For double types, this is done by typecasting (truncation).  A warning
   *      is emitted if the source value is fractional (non-integer) or invalid
   *      (zero or negative).  
   *  - For complex types, this is done by typecasting.  The complex part 
   *      is ignored, and a warning that this is the case is emitted also.
   * Throws an exception for string, cell, structure types, or if a zero or 
   * negative index is encountered.
   */
  void toOrdinalType();
  /**
   * Default constructor.
   */
  inline Array() {
    dp = NULL;
  }
  /**
   * Create an empty Array of the specified type.
   */
  inline Array(Class type) {
    dp = new Data(type,Dimensions(),NULL,false,rvstring(),rvstring());
  }
  /**
   * Create an Array with the specified contents.
   */
  inline Array(Class type, const Dimensions& dims, void* data, bool sparse = false, 
	       rvstring fieldNames = rvstring(), rvstring classname = rvstring()) {
    dp = new Data(type, dims, data, sparse, fieldNames, classname);
  }
  /**
   * Create an Array with a default allocation of space - only useful for P.O.D. arrays
   */
  inline Array(Class type, const Dimensions& dims) {
    dp = new Data(type, dims, 
		  allocateArray(type,dims.getElementCount()), 
		  false, rvstring(), rvstring());
  }
  /**
   * Get the length of the array as a vector.  This is equivalent
   * to computing length(this(:)).
   */
  inline int getLength() const {
    if (dp)
      return dp->dimensions().getElementCount();
    else
      return 0;
  }
  /**
   * Return true if this is a user-defined class
   */    
  inline bool isUserClass() const {
    if (dp)
      return dp->isUserClass();
    else
      return false;
  }
  /**
   * Return name of user-defined class
   */
  inline rvstring className() const {
    if (dp)
      return dp->className();
    else
      return rvstring();
  }
  /**
   * Set classname tag - implies this is a structure array.
   */
  inline void setClassName(rvstring cname) {
    if (dataClass() != FM_STRUCT_ARRAY)
      throw Exception("cannot set class name for non-struct array");
    dp->setClassName(cname);
  }
  /**
   * Get a copy of our dimensions vector.
   */
  inline const Dimensions& dimensions() const {
    if (dp)
      return dp->dimensions();
    else
      return zeroDim;
  }
  /**
   * Get the fieldnames.
   */
  inline rvstring fieldNames() const {
    if (dp)
      return dp->fieldNames();
    else
      return rvstring();
  }
  /**
   * Get our length along the given dimension.
   */
  inline int getDimensionLength(int t) const {
    if (dp)
      return dp->dimensions().get(t);
    else
      return 0;
  }
  /** Get the contents of our data block as a (read-only) void* pointer.
   * Get the contents of our data block as a void* pointer.  The
   * resulting pointer is read only, so that no modifications can
   * be made to the contents of our array.  To modify
   * the contents, you must make a copy and use setDataPointer to replace the
   * current data.  This "copy-on-write" technique avoids copies on
   * references to variables -- a good thing in this interpreted
   * environment where read-references dominate the accesses to variables.
   * Another option is to use getReadWriteDataPointer, which returns a 
   * pointer that is free of object aliases.
   */
  inline const void* getDataPointer() const {
    if (sparse())
      throw Exception("operation does not support sparse matrix arguments.");
    if (dp)
      return dp->getData();
    else
      return NULL;
  }

  inline const void* getSparseDataPointer() const {
    if (dp)
      return dp->getData();
    else
      return NULL;
  }

  inline const void* data() const {
    if (dp)
      return dp->getData();
    else
      return NULL;
  }

  inline void setData(Class aClass, const Dimensions& dims, void *s, 
		      bool sparseflag = false, 
		      rvstring fields = rvstring(),
		      rvstring classname = rvstring()) {
    dp->putData(aClass,dims,s,sparseflag,fields,classname);
  }


  /** Get the contents of our data block as a read-write void* pointer.
   * Get the contents of our data block as a read-write void*
   * pointer.  It ensures that our data block is not aliased (meaning
   * that no other array objects share the data block), prior
   * to returning the pointer. To do this,
   * we have to go through the following steps:
   *   - Check the number of owners of our byte array.
   *   - If there is only one owner for the byte array, return 
   *      a non-const pointer to the data
   *   - If there is more than one owner, copy our data.
   */
  inline void* getReadWriteDataPointer() {
    if (sparse()) {
      makeDense();
    }
    return dp->getWriteableData();
  }
  /** Set the contents of our data block to the supplied pointer.
   * Set the contents of our data block to the supplied pointer.
   * Ownership of the data block is passed to the array, i.e., the
   * caller should not manipulate the block in any way after 
   * calling this function. To avoid recopying, ownership of 
   * the byte-array is passed
   * to the reference counting data object Data at this point.
   * That means that the caller is _not_ responsible for freeing
   * the memory block.
   */
  inline void setDataPointer(void* rp) {
    dp->setData(rp);
  }
  /** Resize an array.
   * Resize the array to a new set of dimensions.  This resize operation
   * puts the contents of the array at the (0,...,0) corner of the new
   * array.  
   */
  void resize(Dimensions& a);
  /** Resize an array based on a vector indexing expression.
   * This method resizes an object so that an index of the type this(n)
   * is valid.  In particular, if "this" is a scalar, then this(n) extends
   * the array in the column dimension.  If "this" is a column vector, then
   * this(n) extends the array in the row dimension.  If "this" is a row
   * vector, then this(n) extends the array in the column direction.
   * For an arbitrarily dimensioned array, this(n) makes the array into 
   * a row vector of length n.
   */
  void vectorResize(int);
  /** Reshape an array.
   * Reshape the array along a new set of dimensions.  Valid provided that
   * setting the dimensions of the array to a does not change the number of
   * elements in the array.
   * Throws an exception if the new dimension has a different number of elements
   * than we currently have.
   */
  void reshape(Dimensions& a);
  /**
   * Take the hermitian transpose of the array.  For non-complex data types,
   * this is equivalent to a transpose operation.
   * Throws an exception if we are not a 2D array.
   */
  void hermitian();
  /**
   * Transposes the array (provided the array is 2-Dimensional).
   * Throws an exception if we are not a 2D array.
   */
  void transpose();
  /**
   * Get our data class (of type Class).
   */
  Class dataClass() const {
    if (dp)
      return dp->dataClass();
    else
      return FM_DOUBLE;
  }
  /**
   * Calculate the size of each element in this array.
   */
  int getElementSize() const;
  /**
   * Calculate the bytes required to hold this array (element size * length)
   */
  inline int getByteSize() const {
    if (sparse())
      throw Exception("Byte size calculation not supported for sparse arrays.");
    return getElementSize()*getLength();
  }
  /**
   * Returns true if we are (meaningfully) positive.  For the unsigned integer types,
   * this is always true.  For complex types, this is false.  For the signed integer
   * types or the floating point types, the result is based on a linear scan through
   * the array.
   */
  const bool isPositive() const;
  /**
   * Returns true if we are symmetric, 2D and square.
   */
  const bool isSymmetric() const;
  /**
   * Returns true if our real part is all zeros.  For integer types, this is an
   * element-wise test.  For complex types, we check only the real part.
   * Throws an exception if we are a string, cell-array or struct-array type.
   */
  const bool isRealAllZeros() const;
  inline const bool sparse() const {
    return (dp->sparse());
  }
  void makeSparse();
  void makeDense();
  int getNonzeros() const;
  /**
   * Returns true if we match the scalar value in x.  For strings, this is done by
   * doing a string compare.  For numerical values, we promote to a common type
   * and do a comparison.
   */
  const bool testCaseMatchScalar(Array x) const;
  /**
   * Returns true if we match the argument x, or if x is a cell-array,
   * returns true if we match any of the cells in x.  Uses Array::testCaseMatchScalar
   * to do the actual testing.
   * Throws an exception for non-scalars (apart from strings) or reference types.
   * Also throws an exception if the argument is not either a scalar or a cell
   * array.
   */
  const bool testForCaseMatch(Array x) const;
  /**
   * Returns TRUE if we are empty (we have no elements).
   */
  inline const bool isEmpty() const {
    return ((dp == NULL) || (getLength() == 0) || 
	    (!dp->getData()));
  }
  /**
   * Returns TRUE if we have only a single element.
   */
  inline const bool isScalar() const {
    if (isEmpty()) return false;
    return dp->dimensions().isScalar();
  }
  /**
   * Returns TRUE if we are 2-Dimensional.
   */
  inline const bool is2D() const {
    return dp->dimensions().is2D();
  }
  /**
   * Returns TRUE if we are a vector.
   */
  inline const bool isVector() const {
    return dp->dimensions().isVector();
  }
  /**
   * Returns TRUE if we are a reference type (cell array or
   * struct array).
   */
  inline const bool isReferenceType() const {
    if (isEmpty())
      return false;
    return ((dataClass() == FM_CELL_ARRAY) ||
	    (dataClass() == FM_STRUCT_ARRAY) ||
	    (dataClass() == FM_FUNCPTR_ARRAY));
  }
  /**
   * Returns TRUE if we are a complex data type.
   */
  inline const bool isComplex() const {
    return (dp->dataClass() == FM_DCOMPLEX || dp->dataClass() == FM_COMPLEX);
  }
  /**
   * Returns TRUE if we are a real data type.
   */
  inline const bool isReal() const {
    return (!isComplex());
  }
  /**
   * Returns TRUE if we are a string.
   */
  inline const bool isString() const {
    return (dp && (dp->dataClass() == FM_STRING));
  }
  /**
   * Returns TRUE if we are an integer class.
   */
  inline const bool isIntegerClass() const {
    return (dp->dataClass() < FM_FLOAT);
  }
  inline bool isColumnVector() const {
    return (is2D() && columns() == 1);
  }
  inline bool isRowVector() const {
    return (is2D() && rows() == 1);
  }
  /**
   * Copy data from our data array to the specified array.  This is a 
   * deep copy, in the sense that pointers are copied by creating 
   * new objects.  Copy count elements, starting at index srcIndex 
   * to the destination address starting at index dstIndex.   The addresses
   * are in terms of indices, not bytes.
   */
  void copyElements(int srcIndex, void* dstPtr, int dstIndex, int count) const;
  /**
   * Promote our array to a new type.  For empty arrays, this type
   * promotion always succeeds.  For cell arrays, this does nothing (except
   * throw an error if we attempt to promote it to a different type).  For
   * structure arrays, promoting to a structure array has three possible outcomes:
   *   - If the fields match in order and contents then the promotion is successful.
   *   - If the fields match in contents but not in-order then the promotion involves
   *     reordering the data.
   *   - If the fields match in contents but the destination type has 
   *     additional fields, then the promotion involves reordering the data and 
   *     adding space.
   * Throws an exception if 
   *   - we try to convert a cell-array to another type.
   *   - we try to promote a structure-array to another array with 
   *     an incompatible field setup (i.e., not one of the three outcomes
   *     listed above).  
   *   - we try to convert a structure-array to a non-structure array type.
   *   - we try to convert any numerical types to a reference type.
   */
  void promoteType(Class new_type, rvstring fieldNames);
  /**
   * Promote our array to a new type.  This is a shortcut for when new_type is not
   * FM_STRUCT_ARRAY, so that the fieldNames argument is not needed.
   */
  void promoteType(Class new_type);
  /**
   * Permute our array according to the given permutation vector.  Note that
   * we assume that the permutation vector is of the correct size and is a
   * valid permutation.
   */
  void permute(const int32* permutation);
  /**
   * Diagonal constructor - construct an array from a given vector, with
   * the contents of the vector stored into the specified diagonal of the
   * matrix.
   * Throwsn an exception if the argument is not a vector.
   */
  static Array diagonalConstructor(Array src, int diagonalOrder);

  /**
   * Empty constructor
   */
  static Array emptyConstructor();
  static Array funcPtrConstructor(FunctionDef *fptr);
  /**
   * Scalar constructor - Construct an FM_LOGICAL object with a scalar
   * value.
   */
  static Array logicalConstructor(bool aval);
  /**
   * Scalar constructor - Construct an FM_UINT8 object with a scalar
   * value.
   */
  static Array uint8Constructor(uint8 aval);
  /**
   * Scalar constructor - Construct an FM_INT8 object with a scalar
   * value.
   */
  static Array int8Constructor(int8 aval);
  /**
   * Scalar constructor - Construct an FM_UINT16 object with a scalar
   * value.
   */
  static Array uint16Constructor(uint16 aval);
  /**
   * Scalar constructor - Construct an FM_INT16 object with a scalar
   * value.
   */
  static Array int16Constructor(int16 aval);
  /**
   * Scalar constructor - Construct an FM_UINT32 object with a scalar
   * value.
   */
  static Array uint32Constructor(uint32 aval);
  /**
   * Scalar constructor - Construct an FM_INT32 object with a scalar
   * value.
   */
  static Array int32Constructor(int32 aval);
  /**
   * Scalar constructor - Construct an FM_UINT64 object with a scalar
   * value.
   */
  static Array uint64Constructor(uint64 aval);
  /**
   * Scalar constructor - Construct an FM_INT64 object with a scalar
   * value.
   */
  static Array int64Constructor(int64 aval);
  /**
   * Scalar constructor - Construct an FM_FLOAT object with a scalar
   * value.
   */
  static Array floatConstructor(float aval);
  /**
   * Scalar constructor - Construct an FM_DOUBLE object with a scalar
   * value.
   */
  static Array doubleConstructor(double aval);
  /**
   * Double matrix constructor - Construct an FM_DOUBLE object with the
   * specified dimenstions.
   */
  static Array doubleMatrixConstructor(int rows, int cols);
  /**
   * Complex constructor - Construct an FM_COMPLEX object with a 
   * complex scalar value.
   */
  static Array complexConstructor(float aval, float bval);
  /**
   * Complex constructor - Construct an FM_DCOMPLEX object with a 
   * complex scalar value.
   */
  static Array dcomplexConstructor(double aval, double bval);
  /**
   * String constructor - Construct an FM_STRING object with the given
   * string as a value.
   */
  static Array stringConstructor(std::string aval);
  /**
   * Double vector constructor - Construct an FM_DOUBLE object
   * that is a (row) vector with the given length.
   */
  static Array doubleVectorConstructor(int len);
  /**
   * Float vector constructor - Construct an FM_FLOAT object
   * that is a (row) vector with the given length.
   */
  static Array floatVectorConstructor(int len);
  /**
   * Unsigned int vector constructor - Construct an FM_INT32 object
   * that is a (row) vector with the given length.
   */
  static Array uint32VectorConstructor(int len);
  /**
   * Construct a FM_INT32 vector (either vertical or horizontal) corresponding to 
   * minval:stepsize:maxval, with an optional transpose.
   */
  static Array int32RangeConstructor(int32 minval, int32 stepsize, int32 maxval, bool vertical);
  /**
   * Construct a FM_INT64 vector (either vertical or horizontal) corresponding to 
   * minval:stepsize:maxval, with an optional transpose.
   */
  static Array int64RangeConstructor(int64 minval, int64 stepsize, int64 maxval, bool vertical);
  /**
   * Construct a FM_FLOAT vector (either vertical or horizontal) corresponding to 
   * minval:stepsize:maxval, with an optional transpose.
   */
  static Array floatRangeConstructor(float minval, float stepsize, float maxval, bool vertical);
  /**
   * Construct a FM_DOUBLE vector (either vertical or horizontal) corresponding to 
   * minval:stepsize:maxval, with an optional transpose.
   */
  static Array doubleRangeConstructor(double minval, double stepsize, double maxval, bool vertical);

  /**
   * There are two things to check when constructing a data object
   * from a matrix of data objects.  The first is the sizes of the
   * objects being composited.  The second is the type of the components.
   * We assume that the Character type is "dominant" in the sense that 
   * if any of the subobjects is a character type, the final type is
   * a character.  Hence, we first construct the size of the final
   * object, and then check its type. This type rule is handled using 
   * the natural ordering of the enumerated types.  The dimensions
   * rule for elements that make up a matrix are straightforward:
   *    - All elements in one row must have the same number of rows,
   *      can have an arbitrary number of columns, and must match in 
   *      all remaining dimensions (dims 3,4,etc.)
   *    - The total number of columns (dim 2) in each row must match
   * Throws an exception if 
   *    - the dimensions of the arguments do not match up properly 
   *      (i.e., do not satisfy the two rules listed above).
   *    - the types are incompatible (e.g., cell and struct arrays are
   *      being combined).
   */
  static Array matrixConstructor(ArrayMatrix& m);
  /**
   * The constructor for a cell array is significantly simpler than
   * the matrix constructor.  The argument is a list of rowdefs.  Each
   * element in the rowdef is a Array pointer that we assign to our
   * data pointer.  The only thing we need to make sure of is that 
   * each row has the same number of elements in it.
   * Throws an exception if the geometry of the argumens is incompatible.
   */
  static Array cellConstructor(ArrayMatrix& m);
  static Array cellConstructor(ArrayVector& m);
  static Array cellConstructor(Array m);
  /**
   * Structure constructor - this is equivalent to the built in struct command.
   * First, we have to make sure that each entry of "values" have 
   *  -  cell arrays of the same size (say MxN)
   *  -  single element cell arrays,
   *  -  single values.
   * With such a setup, the output is a structure array of size MxN.  Elements
   * which are defined by a single value or a single-element cell array are
   * replicated throughout all MxN entries.  Remaining elements take their
   * values from the cell-array.
   * Throws an exception if
   *  - the number of entries in the fieldnames vector does not match
   *    the number of entries in the values vector
   *  - the non-scalar values do not agree in dimension
   */
  static Array structConstructor(rvstring fNames, ArrayVector& values);
  /**
   * Get a subset of an Array.  This is for vector-indexing, meaning that
   * the argument is assumed to refer to the elements in their order as a vector.
   * So, x(10) is equivalent to x(:)(10), even if, say, x is 3 x 4.
   * Throws an exception if
   *  - the variable is empty
   *  - the argument subset exceeds our valid domain
   */
  Array getVectorSubset(Array& index);
  /**
   * Get a subset of an Array.  This if for n-Dimensional-indexing, meaning
   * that x(10) is really x(10,1).
   * Throws an exception if the variable is empty.
   */
  Array getNDimSubset(ArrayVector& index);
  Array getNDimSubsetScalars(ArrayVector& index);
  /**
   * Get the diagonal elements of an array.  Only applicable to 2-dimensional arrays.
   * The diagonal part of a rectangular matrix
   * is a vector of length K.  For an M x N matrix, the L order diagonal has a length
   * that can be calculated as:
   *    K = min(M,N-L) for L > 0 or
   *    K = min(M+L,N) for L < 0
   * Throws an exception for multi-dimensional arrays.
   */
  Array getDiagonal(int diagonalOrder);
  /**
   * Get the contents of a field as an array from its field name.  This is used 
   * when a structure array is used to supply a list of expressions.
   * Throws an exception if
   *   - we are not a structure array
   *   - the field does not exist
   */
  Array getField(std::string fieldName);
  ArrayVector getFieldAsList(std::string fieldName);
  /**
   * Get a subset of a (cell) Array using contents-addressing.  This is used when a 
   * cell array is used to supply a list of expressions.
   * Throws an exception if
   *   - we are not a cell-array
   *   - the indices exceed the array bounds
   */
  Array getVectorContents(Array& index);
  ArrayVector getVectorContentsAsList(Array& index);
  /**
   * Get a subset of an Array using contents-addressing.  This is used when a cell array
   * is used to supply a list of expressions.
   * Throws an exception if we are not a cell-array.
   */  
  Array getNDimContents(ArrayVector& index);
  ArrayVector getNDimContentsAsList(ArrayVector& index);
  void setValue(const Array &x);
  /**
   * Set a subset of an Array.  Uses vector-indexing, meaning that the
   * argument is assumed to refer to the elements in their order as a vector.
   * So, x(10) is equivalent to x(:)(10), even if, say, x is 3 x 4.
   * Throws an exception if there is a size mismatch between the index and the data.
   */
  void setVectorSubset(Array& index, Array& data);
  /**
   * Set a subset of an Array.   This if for n-Dimensional-indexing, meaning
   * that x(10) is really x(10,1).
   * Throws an exception if there is a size mismatch between the index and the data.
   */
  void setNDimSubset(ArrayVector& index, Array& data);
  void setNDimSubsetScalars(ArrayVector& index, const Array& data);
  /**
   * Set a subset of an Array using contents-indexing, meaning that the
   * argument is assumed to refer to the elements in their order as a vector.
   * So, x{10} is equivalent to x(:){10}, even if, say, x is 3 x 4. 
   * This is used when a cell-array is used as the return of a 
   * multi-function call, i.e.: [x{3:5}] = foo.
   * Throws an exception if the number of elements in data do not match
   * the number of indices in index.
   */
  void setVectorContentsAsList(Array& index, ArrayVector& data);
  /**
   * Set a subset of an Array.   This if for n-Dimensional-indexing, meaning
   * that x{10} is really x{10,1}.  This is used when a cell-array is used
   * as the return of a multi-function call, i.e.: [x{1,3:5}] = foo.
   * Throws an exception if the number of elements in data do not match
   * the number of indices covered by index (which is the product of the
   * number of elements in each dimension of index).
   */
  void setNDimContentsAsList(ArrayVector& index, ArrayVector& data);
  /**
   * Replace the contents of a field with the supplied array.  This is used
   * when a structure array is used to hold the return of a multi-function
   * call, i.e.: [x.foo] = foo
   * Throws an exception if
   *   - we are not a structure array
   *   - the number of elements in data is not equal to the number of elements in 
   *     our array.
   */
  void setFieldAsList(std::string fieldName, ArrayVector& data);
  /**
   * Delete a subset of this array using the argument for vector indexing.
   * This is _much_ simpler than the planar case.  Here, we simply:
   *   -  Create a deletion map from the index variable.
   *   -  Adjust the size of the output, and reshape to
   *       a vector.
   *   -  Copy (and skip) as necessary.
   * The result is then resized using the same rules as in vectorResize.
   */ 
  void deleteVectorSubset(Array& ndx);
  /**
   * Delete a subset of this array using the arguments for n-Dimensional
   * indexing.  This method is the "planar" delete, meaning that its 
   * designed to delete all the entries in an N-ary array in one dimension.
   * It cannot be used to create "holes" in an array.
   * Throws an exception if the argument contains more than one non-colon index
   */
  void deleteNDimSubset(ArrayVector& args);
  /**
   * Summarize this array when it appears in a Cell array.
   */
  void summarizeCellEntry() const;
  /**
   * Print some reasonable representation of this array to the
   * the supplied stream.
   */
  //    void printMe(int printLimit, int termWidth = 80) const;
  /**
   * Return the number of rows.
   */
  inline int rows() const {
    return getDimensionLength(0);
  }
  /**
   * Return the number of columns.
   */
  inline int columns() const {
    return getDimensionLength(1);
  }
  /**
   * Get our contents as a C-string.  Only works for _STRING types.
   * Throws an exception for non-string types.
   */
  char* getContentsAsCString() const;
  /**
   * Get our contents as an integer scalar.
   * Throws an exception if we are not a scalar integer type.
   */
  int32 getContentsAsIntegerScalar();
  /**
   * Get our contents as a double scalar.
   * Throws an exception if we are not scalar or cannot meaningfully
   * be converted to a double precision value.
   */
  double getContentsAsDoubleScalar();
  /**
   * Returns true if the given Class is either FM_CELL_ARRAY or
   * FM_STRUCT_ARRAY.
   */
  static bool isDataClassReferenceType(Class);
  /**
   * Sets the IO interface used by instances of the Array class.
   */
  static void setArrayInterpreter(Interpreter* eval);
  /**
   * Returns the IO interface being used by instances of the Array class.
   */
  static Interpreter* getArrayInterpreter();
  /**
   * Returns true if any of the entries in the array are not finite
   * (always false for reference and integer types)
   */
  bool anyNotFinite();
  /**
   * Returns the number of nonzero elements in the array.  For reference
   * types, this is a best-guess.
   */
  int32 nnz() const;
};

bool isColonOperator(Array& a);
int32 ArrayToInt32(const Array& a);
double ArrayToDouble(const Array& a);
const char* ArrayToString(const Array& a);
Array ToSingleArray(const ArrayVector& a);

void printObjectBalance();
void dumpAllArrays();

uint32 TypeSize(Class cls);

string operator+(string a, int d);
string operator+(int d, string a);

#endif
