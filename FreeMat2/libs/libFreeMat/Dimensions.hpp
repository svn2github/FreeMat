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

#ifndef __Dimensions_hpp__
#define __Dimensions_hpp__

#include <iostream>
#include "Exception.hpp"
#include "Types.hpp"

class Interpreter;
const unsigned int maxDims = 6;

/**
 * The Dimensions class is used to keep track of the number of
 * dimensions for a given Array.  The Dimensions class represents
 * an N-tuple of integers (each non-negative).  N can be made
 * arbitrarily large.  The '[]' operator is overloaded to return
 * a reference to the dimension of interest.  Indexing beyond
 * the number of current dimensions automatically increases the 
 * length of the dimension vector, and pads it with 1's.  For example
 * if our current contents are $$[3,5,2,1]$$, and we index element
 * 5, the dimension vector is automatically padded to $$[3,5,2,1,1,1]$$.
 * Calling simplify() removes the trailing '1's.
 */
class Dimensions {
private:
  /**
   * The data array of dimensions.
   */
  int data[maxDims];
  /**
   * The number of dimensions currently allocated.
   */
  int length;
  /**
   * Cache values for frequently called functions.
   */
  int m_cache_getElementCount;
  bool m_cache_isScalar;
  int m_cache_getRows;
  int m_cache_getColumns;
  bool m_cache_is2D;
  bool m_cache_isVector;
public:
  /**
   * The default constructor - length is set to zero.
   */
  inline Dimensions() {
    length = 0;
    m_cache_getElementCount = 0;
    m_cache_isScalar = false;
    m_cache_getRows = 0;
    m_cache_getColumns = 0;
    m_cache_is2D = true;
    m_cache_isVector = true;
  }
  /**
   * Initialize the object with the given number of dimensions 
   * and all contents set to zero.
   * Throws an Exception if the dimension count is negative.
   */
  inline Dimensions(int dimCount) {
    if (dimCount < 0) 
      throw Exception("Illegal argument to Dimensions constructor");
    memset(data, 0, sizeof(int)*dimCount);
    length = dimCount;
    updateCacheVariables();
  }
  /**
   * Return a 2-D dimension object with the specified number of
   * rows and columns.
   */
  inline Dimensions(int rows, int cols) {
    data[0] = rows;
    data[1] = cols;
    length = 2;
    m_cache_getElementCount = rows*cols;
    m_cache_isScalar = ((rows==1)&&(cols==1));
    m_cache_getRows = rows;
    m_cache_getColumns = cols;
    m_cache_is2D = true;
    m_cache_isVector = ((rows==1)||(cols==1));
  }
  /**
   * Return a reference to the ith dimension.  This member function
   * will adjust the number of dimensions present if the argument
   * exceeds the current number allocated.  The extra dimensions are
   * padded with 1's.  Thus, if our contents are $$[2,3]$$, then 
   * a reference to dimension 2 (these are zero-based indexing) converts
   * our contents to $$[2,3,1]$$.
   * Throws an exception if the argument is negative.
   */
  //  int& operator[](unsigned i) throw(Exception);
  /**
   * Get the number of currently allocated dimensions.
   */
  inline int getLength() const {
    return length;
  }
  /**
   * Update the cache variables.  Call this when
   * finished mucking with dimension data!
   */
  void updateCacheVariables();
  /**
   * Get the pointer to the dimension data.
   */
  //  int* getDimensionData() const;
  /**
   * Return the total number of elements in the associated Array object.
   * calculated via $$\Prod_{i=0}^{L-1} a_i$$, where $$L$$ is the value
   * of length, and $$a_i$$ is equivalent to data[i].
   */
  inline int getElementCount() const {
    return m_cache_getElementCount;
  }
  /**
   * Map the given point using the current Dimensions.  If the argument
   * values are denoted as $$b_i$$, and our dimensions are $$a_i$$, then
   * this member function computes $$\Sum_{i=0}^{L-1} b_i \Prod_{j=0}^{j=i} a_j$$.
   * Throws an exception if:
   *   - the argument has more dimensions than the current object
   *   - any of the arguments are outside the valid range, i.e., 
   *     $$b_i < 0$$ or $$b_i >= a_i$$.
   */
  int mapPoint(const Dimensions& point) const;
  /**
   * Returns the first dimension value (or zero if no dimensions have
   * been defined yet).
   */
  inline int getRows() const {
    return m_cache_getRows;    
  }
  /**
   * Returns the second dimension value (or zero if no dimensions have
   * been defined yet).
   */
  inline int getColumns() const {
    return m_cache_getColumns;
  }
  /**
   * Returns the requested dimension, or a 1 if the requested dimension
   * exceeds the currently allocated number of dimensions.  Unlike
   * the access operator, this call does not modify the contents of
   * the class.
   */
  inline int getDimensionLength(int arg) const {
    if (length <= arg)
      return 1;
    else
      return data[arg];
  }
  /**
   * Less verbose synonym
   */
  inline int get(int arg) const {
    return getDimensionLength(arg);
  }
  /**
   * A synonym for (*this)[dim] = len.
   */
  void setDimensionLength(int dim, int len);
  /**
   * Less verbose synonym
   */
  inline void set(int dim, int len) {
    setDimensionLength(dim,len);
  }
  /**
   * Expand our dimensions so as to include the given point.  This operation
   * involves a sequence of operations.  Let $$b$$ denote the argument, and
   * $$a$$ denote this object.  Then first we update the length of $$a$$ 
   * as $$ |a| \leftarrow \max(|a|,|b|) $$.  Next, we loop over each dimension
   * and apply the following algorithm:
   *    - if $$a_i$$ is undefined but $$b_i$$ is defined, $$a_i \leftarrow b_i$$.
   *    - if $$a_i$$ is defined but $$b_i$$ is undefined, $$a_i \leftarrow a_i$$.
   *    - if $$a_i$$ and $$b_i$$ are both defined, $$a_i \leftarrow \max(a_i,b_i)$$.
   */
  void expandToCover(const Dimensions& resize);
  /**
   * Increment our current value in the given dimension (ordinal), 
   * modulo the limit.  This is equivalent to first incrementing
   * the $$n$$-th dimension (where $$n$$ is the second argument) via
   * $$a_n \leftarrow a_n + 1$$.  Then, $$a_n$$ is checked against
   * the limit $$b_n$$.  If $$a_n >= b_n$$, then $$a_n \leftarrow 0$$,
   * and the algorithm is repeated with ordinal+1.
   */
  void incrementModulo(const Dimensions& limit, int ordinal);
  /**
   * Returns comparison of $$a_n < b_n$$, where $$n$$ is the maximum defined
   * dimension in $$a$$ (this object), and $$b$$ is the given argument.
   */
  inline bool inside(const Dimensions& limit) const {
    return (data[length-1] < limit.data[length-1]);
  }
  /**
   * Returns true if we match the argument (exactly).
   */
  bool equals(const Dimensions& alt) const;
  /**
   * This member function trims any excess singleton (length 1) dimensions
   * from our data array after the second dimension.  Thus, the dimension
   * vector $$[3,4,5,1,1]$$, will become $$[3,4,5]$$ after simplify() is
   * called.
   */
  void simplify();
  /**
   * Print some representation of this object as a row vector, i.e., 
   * $$[a_1 a_2 \ldots a_n]$$.
   */
  void printMe(Interpreter *io) const;
  /**
   * Returns a string containing the dimensions as a row vector.
   */
  std::string asString() const;
  /**
   * Reset the number of allocated dimensions to zero, and reset the
   * data pointer to NULL.
   */
  void reset();
  /**
   * Set all of the allocated dimensions to zero.
   */
  void zeroOut();
  /**
   * Force the contents to be $$[1,1]$$.
   */
  void makeScalar();
  /**
   * Get the largest dimension value, i.e., $$\max_{i=1,\ldots,n} \{a_i\}$$.
   */
  int getMax();
  /**
   * Returns true if and only if we are equal to $$[1,1]$$.
   */
  inline bool isScalar() const {
    return m_cache_isScalar;
  }
  /**
   * Returns true if and only if we are equal to $$[1,n]$$ or $$[n,1]$$ for
   * some value of $$n$$.
   */
  inline bool isVector() const {
    return m_cache_isVector;
  }
  /**
   * Returns true if we have exactly 2 dimensions allocated.
   */
  inline bool is2D() const {
    return m_cache_is2D;
  }
  /**
   * Applies a permutation (assumed legal) to this dimension vector, and returns the permuted vector
   */
  Dimensions permute(const int32* permutation) const;
};

#endif
