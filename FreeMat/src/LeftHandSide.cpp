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

#include "LeftHandSide.hpp"
#include "Exception.hpp"

namespace FreeMat {

  /**
   * This object represents delayed manipulations to a Left Hand Side (LHS)
   * object.  The argument to the constructor must not be deleted, as it is
   * changed as a result of the assign() operation that ultimately takes
   * place.
   */
  LeftHandSide::LeftHandSide(Array object) {
    data = object;
    IndexClass n;
    n.type = NDX_ALL;
    index.push_back(n);
    dataStack.push_back(object);
    vectorExpansion = false;
    currentDims = data.getDimensions();
  }

  LeftHandSide::~LeftHandSide() {
  }

  void LeftHandSide::subIndexVectorSubset(Array ndx) throw(Exception){
    if (vectorExpansion)
      throw Exception("Cannot apply () to a left hand side with multiple values.");
    dataStack.push_back(data);
    IndexClass n;
    n.type = NDX_VECTOR_SUBSET;
    n.a = ndx;
    index.push_back(n);
    currentDims = ndx.getDimensions();
    try {
      data = data.getVectorSubset(ndx);
    } catch (Exception &e) {
      data = Array::emptyConstructor();
    }
  }

  void LeftHandSide::subIndexNDimSubset(ArrayVector ndx) throw(Exception){
    if (vectorExpansion)
      throw Exception("Cannot apply () to a left hand side with multiple values.");
    dataStack.push_back(data);
    IndexClass n;
    n.type = NDX_NDIM_SUBSET;
    n.v = ndx;
    index.push_back(n);
    currentDims.reset();
    for (int i=0;i<ndx.size();i++) {
      ndx[i].toOrdinalType();
      currentDims[i] = ndx[i].getLength();
    }
    try {
      data = data.getNDimSubset(ndx);
    } catch (Exception &e) {
      data = Array::emptyConstructor();
    }
  }

  void LeftHandSide::subIndexVectorContents(Array ndx) throw(Exception){
    if (vectorExpansion)
      throw Exception("Cannot apply {} to a left hand side with multiple values.");
    dataStack.push_back(data);
    IndexClass n;
    n.type = NDX_VECTOR_CONTENTS;
    n.a = ndx;
    index.push_back(n);
    currentDims = ndx.getDimensions();
    if (ndx.getLength() > 1)
      vectorExpansion = true;
    else {
      try {
	data = data.getVectorContents(ndx);
      } catch (Exception &e) {
	data = Array::emptyConstructor();
      }
    }
  }

  void LeftHandSide::subIndexNDimContents(ArrayVector ndx) throw(Exception){
    if (vectorExpansion)
      throw Exception("Cannot apply {} to a left hand side with multiple values.");
    dataStack.push_back(data);
    IndexClass n;
    n.type = NDX_NDIM_CONTENTS;
    n.v = ndx;
    index.push_back(n);
    int i=0;
    currentDims.reset();
    while (i<ndx.size() && !vectorExpansion) {
      ndx[i].toOrdinalType();
      vectorExpansion = (ndx[i].getLength()) > 1;
      currentDims[i] = ndx[i].getLength();
      i++;
    }
    if (!vectorExpansion) {
      try {
	data = data.getNDimContents(ndx);
      } catch (Exception &e) {
	data = Array::emptyConstructor();
      }
    }
  }

  void LeftHandSide::subIndexField(std::string fieldName) throw(Exception){
    if (vectorExpansion)
      throw Exception("Cannot apply . to a left hand side with multiple values.");
    dataStack.push_back(data);
    IndexClass n;
    n.type = NDX_FIELD;
    n.s = fieldName;
    index.push_back(n);
    if (currentDims.getElementCount() > 1)
      vectorExpansion = true;
    else {
      currentDims = data.getDimensions();
      try {
	data = data.getField(fieldName);
      } catch (Exception &e) {
	data = Array::emptyConstructor();
      }
    }
  }

  /**
   * To understand how assign functions, consider the following somewhat
   * complicated assignment statement:
   * A complicated indexing setup like 
   * A{3}(2).foo(9) = RHS
   * 
   * Can be written as:
   * 
   * B = A->getVectorContents(3);
   * C = B->getVectorSubset(2);
   * D = C->getField("foo");
   * E = D->getVectorSubset(9);
   * 
   * D->setVectorSubset(9,RHS);
   * C->setField("foo",D);
   * B->setVectorSubset(2,C);
   * A->setVectorContents(3,B);
   *
   * The construction of A, B, C, D, & E has already taken place via the 
   * sub*() member funtions.  And these occupy the stack.  The variable
   * "data" is set to E, so we can delete this first.
   *
   * For multiple LHS objects, the procedure is similar:
   *
   * A(1:3).foo = RHS
   * B = A->getVectorSubset(1:3);
   * C = B->getField("foo");
   * B->setField("foo",RHS);
   *
   */ 
  Array LeftHandSide::assign(ArrayVector& copy) throw(Exception){
    IndexClass indx;
    // Initialize the RHS pointer to copy
    Array lhs;
    Array rhs;
    if (!vectorExpansion)  {
      if (copy.size() == 0)
	throw Exception("Not enough right hand side values to satisfy left hand side expression");
      rhs = copy[0];
      //      copy.pop_front();
      copy.erase(copy.begin());
    }
    int i = 0;
    // While the stack is not empty
    while(index.size() > 0) {
      i++;
      //Pop the next index off the stack
      indx = index.back();
      index.pop_back();
      // Pop the data object off the stack also
      lhs = dataStack.back();
      dataStack.pop_back();
      // Do what it says...
      switch(indx.type) {
      case NDX_ALL:
	lhs = rhs;
	break;
      case NDX_VECTOR_SUBSET:
	lhs.setVectorSubset(indx.a,rhs);
	break;
      case NDX_NDIM_SUBSET:
	lhs.setNDimSubset(indx.v,rhs);
	break;
      case NDX_VECTOR_CONTENTS:
	if (vectorExpansion) {
	  lhs.setVectorContentsAsList(indx.a,copy);
	  vectorExpansion = false;
	} else 
	  lhs.setVectorContents(indx.a,rhs);
	break;
      case NDX_NDIM_CONTENTS:
	if (vectorExpansion) {
	  lhs.setNDimContentsAsList(indx.v,copy);
	  vectorExpansion = false;
	} else
	  lhs.setNDimContents(indx.v,rhs);
	break;
      case NDX_FIELD:
	if (vectorExpansion) {
	  if (lhs.isEmpty()) {
	    stringVector dumVec;
	    dumVec.push_back(indx.s);
	    lhs.promoteType(FM_STRUCT_ARRAY,dumVec);
	    lhs.resize(currentDims);
	  }
	  lhs.setFieldAsList(indx.s,copy);
	  vectorExpansion = false;
	} else 
	  lhs.setField(indx.s,rhs);
	break;
      }
      rhs = lhs;
    }
    return lhs;
  }

  int LeftHandSide::getLength() {
    return currentDims.getElementCount();
  }

  int LeftHandSide::getDimensionLength(int dim) {
    return data.getDimensionLength(dim);
  }

  Dimensions LeftHandSide::getDimensions() {
    return currentDims;
  }
}
