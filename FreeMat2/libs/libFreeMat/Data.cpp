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

namespace FreeMat {

  Data::Data(Class aClass, const Dimensions& dims, void *s, const stringVector& fields): 
    cp(s), owners(1), dimensions(dims), fieldNames(fields), dataClass(aClass) {
  } 

  Data::~Data() { 
    freeDataBlock(); 
  }

  Data* Data::getCopy() { 
    owners++; 
    return this; 
  }

  Data* Data::putData(Class aClass, const Dimensions& dims, void *s, const stringVector& fields) {
    if ((owners <= 1)) {
      freeDataBlock();
      cp = s;
      dataClass = aClass;
      dimensions = dims;
      fieldNames = fields;
      owners = 1;
      return this;
    } else {
      owners--;
      return new Data(aClass,dims,s,fields);
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
      if (Array::isDataClassReferenceType(dataClass)) {
	Array* rp = (Array*) cp;
	delete [] rp;
      } else 
	Free(cp);
    }
  }
}
