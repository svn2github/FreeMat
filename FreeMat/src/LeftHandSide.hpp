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

#ifndef _LeftHandSide_hpp_
#define _LeftHandSide_hpp_

#include "Array.hpp"

namespace FreeMat {

  /**
   * These are the supported indexing types.  Each indexing type
   * is encapsulated inside an object.
   */
  typedef enum {
    NDX_ALL,
    NDX_VECTOR_SUBSET,
    NDX_NDIM_SUBSET,
    NDX_VECTOR_CONTENTS,
    NDX_NDIM_CONTENTS,
    NDX_FIELD,
  } IndexType;


  class IndexClass {
  public:
    IndexType type;
    Array a;
    ArrayVector v;
    std::string s;
  };

  typedef std::vector<IndexClass> indexList;

  /**
   * This object represents the left hand side of an assignment - it is akin to
   * a reference or pointer to a data object that includes multiple references, etc.
   * The argument to the constructor must not be deleted, as it is
   * changed as a result of the assign() operation that ultimately takes
   * place. 
   * A complicated indexing setup like
   * A{3}(2).foo(9) = RHS
   * 
   * Can be written as:
   * 
   * B = A->getVectorContents(3);
   * C = B->getVectorSubset(2);
   * D = C->getField("foo");
   * E = D->getField(9);
   * 
   * D->setVectorSubset(9,RHS);
   * C->setField("foo",D);
   * B->setVectorSubset(2,C);
   * A->setVectorContents(3,B);
   * 
   * The temporaries B, C, D, E are sequentially computed using the Array->get
   * functions and then pushed onto the dataStack each time an indexing member 
   * function (e.g., this->subIndexVectorSubset) is called.  When assign() is called,
   * the stack is popped, and the Array->set functions are called, until ultimately
   * the original data object is modified.
   * For multiple LHS objects, the procedure is similar:
   *
   * A(1:3).foo = RHS
   * B = A->getVectorSubset(1:3);
   * C = B->getField("foo");
   * B->setField("foo",RHS);
   */
  class LeftHandSide {
    /**
     *  This stores the stack of objects that have been traced.
     */
    ArrayVector dataStack;
    /**
     *  This stores the pointer to the data object.
     */
    Array data;
    /** 
     * This stores the vector of pointers to index vectors.
     */
    indexList index;
    /**
     * This boolean flag indicates whether a multi-left hand side
     * expansion has taken place.  A multi-left hand side expansion
     * is equivalent to A.foo, where A is a multi-element struct
     * array, or A{1:3}, where A is a cell-array.
     */
    bool vectorExpansion;
    /**
     * Current LHS dimension - stores the size of the left hand
     * side after all of the subindexing expressions encountered
     * so far.
     */
    Dimensions currentDims;

  public:
    /**
     * Initial constructor
     */
    LeftHandSide(Array object);
    /**
     * Default destructor
     */
    ~LeftHandSide();
    /**
     * Subindex the current object 
     *  - this is a vector subset, i.e. "(ndx)"
     * Throws an exception if a vectorExpansion has already
     * taken place (for example, A{1:3}(4) is illegal).
     */
    void subIndexVectorSubset(Array ndx) throw(Exception);
    /**
     * Subindex the current object
     *  - this is a NDIM subset, i.e., "(ndx1,ndx2,...)"
     * Throws an exception if a vectorExpansion has already
     * taken place (for example, A{1:3}(4,5) is illegal).
     */
    void subIndexNDimSubset(ArrayVector ndx) throw(Exception);
    /**
     * Subindex the current object
     *  - this is a vector contents, i.e., "{ndx}"
     * Throws an exception if a vectorExpansion has already
     * taken place (for example, A{1:3}{4} is illegal).
     */
    void subIndexVectorContents(Array ndx) throw(Exception);
    /**
     * Subindex the current object
     *  - this is a NDIM contents, i.e., "{ndx1,ndx2,...}"
     * Throws an exception if a vectorExpansion has already
     * taken place (for example, A{1:3}{4,5} is illegal).
     */
    void subIndexNDimContents(ArrayVector ndx) throw(Exception);
    /**
     * Subindex the current object
     *  - this is a field subset, i.e. ".IDENT"
     * Throws an exception if a vectorExpansion has already
     * taken place (for example, A{1:3}.foo is illegal).
     */
    void subIndexField(std::string fieldName) throw(Exception);
    /**
     * This member function unwinds the assignment stack,
     * and is equivalent to *LHS = expression.  Throws an
     * exception if the internal left hand side expression
     * has expanded to a non-empty lvalue, and the argument
     * is empty.
     */
    Array assign(ArrayVector& copy) throw(Exception);
    /**
     * Get the number of elements in the current LHS expression.
     */
    int getLength();
    /**
     * Get the size of the current LHS expression along the given
     * dimension.
     */
    int getDimensionLength(int);
    /**
     * Get the current dimensions for the LHS expression.
     */
    Dimensions getDimensions();
  };
}
#endif
