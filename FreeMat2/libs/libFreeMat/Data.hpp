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
private:
  /**
   * The Class of the data block.  Useful for determining how the data
   * block must be treated.
   */
  Class m_dataClass;
  /**
   * Sparsity flag - true if we are a sparse array.
   */
  bool m_sparse;
  /**
   * Pointer to the data block.
   */
  void *cp;
  /**
   * The dimensions of the data block.
   */
  Dimensions m_dimensions;
  /**
   * The field names of the array - used only for structure array types.
   */
  StringVector m_fieldNames;
  /**
   * The class name - only used for user-defined classes
   */
  StringVector m_className;

public:
  /**
   * Construct a Data object with the given arguments.
   * the owner count is initialized to 1.
   */
  inline Data(Class aClass, const Dimensions& dims, void *s, 
	      bool sparseflag = false, 
	      StringVector fields = StringVector(), 
	      StringVector classname = StringVector()) : 
    m_dataClass(aClass), cp(s), m_dimensions(dims)  {
    m_sparse = sparseflag;
    m_fieldNames = fields;
    m_sparse = sparseflag;
    m_className = classname;
  } 
  inline Data(const Data& copy) :
    m_dataClass(copy.m_dataClass),
    m_sparse(copy.m_sparse),
    m_dimensions(copy.m_dimensions),
    m_fieldNames(copy.m_fieldNames),
    m_className(copy.m_className) {
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
		      StringVector fields = StringVector(),
		      StringVector classname = StringVector()) {
    FreeData();
    cp = s;
    m_dataClass = aClass;
    m_dimensions = dims;
    m_fieldNames = fields;
    m_sparse = sparseflag;
    m_className = classname;
  }
  /**
   * Get a read-only pointer to the data.
   */
  inline const void* getData() const {return cp;}
  inline void* getWriteableData() {return cp;}
  inline void setData(void * p) {FreeData(); cp = p;}
  /**
   * Set and Get methods for members
   */
  inline Class dataClass() const {return m_dataClass;}
  inline void setDataClass(Class p) {m_dataClass = p;}
  inline bool sparse() const {return m_sparse;}
  inline void setSparse(bool p) {m_sparse = p;}
  inline const Dimensions& dimensions() const {return m_dimensions;}
  inline void setDimensions(const Dimensions& p) {m_dimensions = p;}
  inline const StringVector& fieldNames() const {return m_fieldNames;}
  inline void setFieldNames(const StringVector& p) {m_fieldNames = p;}
  inline const StringVector& className() const {return m_className;}
  inline void setClassName(const StringVector& p) {m_className = p;}
  /**
   * Return true if this is a user-defined class
   */
  inline bool isUserClass() const {return (!m_className.empty());}
};

#endif
