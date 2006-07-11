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

#ifndef __Data_hpp__
#define __Data_hpp__

#include "Types.hpp"
#include "Dimensions.hpp"

/**
 * This is a helper class that is used by the Array class to 
 * support the reference counting scheme used by the Array objects.
 * It essentially stores blocks of data along with a count of
 * how many owners of the data block there are (standard reference
 * counting).  The Array class is declared a friend, as all the
 * methods (including the constructors) are private.
 */
class Data {
  friend class Array;
private:
  /**
   * The Class of the data block.  Useful for determining how the data
   * block must be treated.
   */
  Class dataClass;
  /**
   * Sparsity flag - true if we are a sparse array.
   */
  bool sparse;
  /**
   * Pointer to the data block.
   */
  void *cp;
  /**
   * Number of owners for the data block.
   */
  int owners;
  /**
   * The dimensions of the data block.
   */
  Dimensions dimensions;
  /**
   * The field names of the array - used only for structure array types.
   */
  stringVector fieldNames;
  /**
   * The class name - only used for user-defined classes
   */
  stringVector className;
  /**
   * Construct a Data object with the given arguments.
   * the owner count is initialized to 1.
   */
  Data(Class aClass, const Dimensions& dims, void *s, 
       bool sparseflag = false, 
       const stringVector& fields = stringVector(), 
       stringVector classname = stringVector());
  /**
   * The destructor.  Calls freeDataBlock member function.
   */
  ~Data();
  /**
   * Get a copy to us - increments the data counter by one, and
   * returns the "this" pointer.
   */
  Data* getCopy();
  /**
   * Change the contents of the Data object.  The data pointer,
   * class and size are given.  The behavior/return value depends
   * on the number of owners:
   *   - If there is only one owner for this Data block, then
   *     freeDataBlock is called to delete the current data, and
   *     the members are simply reassigned to the given arguments.
   *     The return value is the "this" pointer.
   *   - Otherwise, the number of owners for the current block
   *     is decreased by one, and a new Data object is returned
   *     with the given contents.
   */
  Data* putData(Class aClass, const Dimensions& dims, void *s, 
		bool sparseflag = false, 
		const stringVector& fields = stringVector(),
		stringVector classname = stringVector());
  /**
   * Decrement the reference count (owners) by one.
   */
  int deleteCopy();
  /**
   * Get a read-only pointer to the data.
   */
  const void* getData() const;
  /**
   * Get the dimensions for the data block.
   */
  const Dimensions& getDimensions() const;
  /**
   * Get the field names for the data block
   */
  const stringVector& getFieldNames() const;
  /**
   * Return true if this is a user-defined class
   */
  bool isUserClass() const;
  /**
   * Return name of user-defined class
   */
  stringVector getClassName() const;
  /**
   * Set the dimensions for the data block.
   */
  void setDimensions(const Dimensions&);
  /**
   * Set the field names for the data block.
   */
  void setFieldNames(const stringVector& fields);
  /**
   * Get a read-write pointer to the data. 
   */
  void* getWriteableData();
  /**
   * Get the number of owners.
   */
  int numberOfOwners() const;
  /**
   * If the data pointer is non-null, we take one of
   * the following options:
   *   - For reference types, a deep delete is performed
   *   - For non-reference types, the data block is 
   *     simply freed.
   */
  void freeDataBlock();
  /**
   * Check sparsity.
   */
  bool isSparse();
};

#endif
