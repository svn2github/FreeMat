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
#include <QSharedData>

/**
 * This is a helper class that is used by the Array class to 
 * support the reference counting scheme used by the Array objects.
 * It essentially stores blocks of data along with a count of
 * how many owners of the data block there are (standard reference
 * counting).  The Array class is declared a friend, as all the
 * methods (including the constructors) are private.
 */
class FunctionDef;
class Array;

class Data : public QSharedData {
public:
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
   * The dimensions of the data block.
   */
  Dimensions dimensions;
  /**
   * The field names of the array - used only for structure array types.
   */
  rvstring fieldNames;
  /**
   * The class name - only used for user-defined classes
   */
  rvstring className;

  /**
   * Construct a Data object with the given arguments.
   * the owner count is initialized to 1.
   */
  inline Data(Class aClass, const Dimensions& dims, void *s, 
	      bool sparseflag = false, 
	      rvstring fields = rvstring(), 
	      rvstring classname = rvstring()) : 
    cp(s), dimensions(dims), dataClass(aClass) {
    sparse = sparseflag;
    fieldNames = fields;
    sparse = sparseflag;
    className = classname;
  } 
  inline Data(const Data& copy) :
    dataClass(copy.dataClass),
    dimensions(copy.dimensions),
    sparse(copy.sparse),
    fieldNames(copy.fieldNames),
    className(copy.className) {
    cp = copyDataBlock(copy.cp);
  }
  void* copyDataBlock(void *dp);
  void FreeData();
  /**
   * The destructor.  Calls freeDataBlock member function.
   */
  inline ~Data() {
    FreeData();
  }
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
  inline void putData(Class aClass, const Dimensions& dims, void *s, 
		      bool sparseflag = false, 
		      rvstring fields = rvstring(),
		      rvstring classname = rvstring()) {
    FreeData();
    cp = s;
    dataClass = aClass;
    dimensions = dims;
    fieldNames = fields;
    sparse = sparseflag;
    className = classname;
  }
  /**
   * Get a read-only pointer to the data.
   */
  inline const void* getData() const {return cp;}
  /**
   * Get the dimensions for the data block.
   */
  inline const Dimensions& getDimensions() const {return dimensions;}
  /**
   * Get the field names for the data block
   */
  inline rvstring getFieldNames() const {return fieldNames;}
  /**
   * Return true if this is a user-defined class
   */
  inline bool isUserClass() const {return (!className.empty());}
  /**
   * Return name of user-defined class
   */
  inline rvstring getClassName() const {return className;}
  /**
   * Set the dimensions for the data block.
   */
  inline void setDimensions(const Dimensions& dim) {dimensions = dim;}
  /**
   * Set the field names for the data block.
   */
  inline void setFieldNames(rvstring fields) {fieldNames = fields;}
  /**
   * Get a read-write pointer to the data. 
   */
  inline void* getWriteableData() {return cp;}
  /**
   * Check sparsity.
   */
  inline bool isSparse() const {return sparse;}
};

#endif
