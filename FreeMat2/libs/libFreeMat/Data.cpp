// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Data.hpp"
#include "Malloc.hpp"
#include "Sparse.hpp"

namespace FreeMat {

  Data::Data(Class aClass, const Dimensions& dims, void *s, bool sparseflag, 
	     const stringVector& fields, stringVector classname): 
    cp(s), owners(1), dimensions(dims), fieldNames(fields), dataClass(aClass), className(classname) {
      sparse = sparseflag;
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
}
