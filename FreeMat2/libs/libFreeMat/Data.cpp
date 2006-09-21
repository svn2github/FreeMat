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

#include "Data.hpp"
#include "Malloc.hpp"
#include "Sparse.hpp"

int DataMakeCount = 0;

Data::Data(Class aClass, const Dimensions& dims, void *s, bool sparseflag, 
	   const stringVector& fields, stringVector classname): 
  cp(s), owners(1), dimensions(dims), fieldNames(fields), dataClass(aClass), className(classname) {
  sparse = sparseflag;
  DataMakeCount++;
} 

Data::~Data() { 
  freeDataBlock(); 
}

Data* Data::getCopy() { 
  owners++; 
  return this; 
}

Data* Data::putData(Class aClass, const Dimensions& dims, void *s, 
		    bool sparseflag, const stringVector& fields, 
		    stringVector classname) {
  if ((owners <= 1)) {
    freeDataBlock();
    cp = s;
    dataClass = aClass;
    dimensions = dims;
    fieldNames = fields;
    sparse = sparseflag;
    className = classname;
    owners = 1;
    return this;
  } else {
    owners--;
    return new Data(aClass,dims,s,sparseflag,fields,classname);
  }
}

int Data::deleteCopy() { 
  return owners--; 
}

const void* Data::getData() const {
  return cp;
}

void* Data::getWriteableData() {
  return cp;
}

const Dimensions& Data::getDimensions() const {
  return dimensions;
}

const stringVector& Data::getFieldNames() const {
  return fieldNames;
}

stringVector Data::getClassName() const {
  return className;
}

bool Data::isUserClass() const {
  return (!className.empty());
}

void Data::setDimensions(const Dimensions& dim) {
  dimensions = dim;
}

void Data::setFieldNames(const stringVector& fields) {
  fieldNames = fields;
}

int Data::numberOfOwners() const { 
  return owners; 
}

void Data::freeDataBlock() {
  if (cp) {
    if (dataClass == FM_FUNCPTR_ARRAY) {
      FunctionDef **dp = (FunctionDef**) cp;
      delete[] dp;
    } else if (Array::isDataClassReferenceType(dataClass)) {
      Array* rp = (Array*) cp;
      delete [] rp;
    } else if (sparse) {
      DeleteSparseMatrix(dataClass,dimensions[1],cp);
    } else 
      Free(cp);
  }
}
  
bool Data::isSparse() {
  return sparse;
}
