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

#include "Array.hpp"
#include "Exception.hpp"
#include "Data.hpp"
#include "Malloc.hpp"
#include "IEEEFP.hpp"
#include "Sparse.hpp"
#include <math.h>
#include <stdio.h>
#include <set>

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace FreeMat {

  static int objectBalance;
#define MSGBUFLEN 2048
  static char msgBuffer[MSGBUFLEN];
  static Interface *io;

  typedef std::set<uint32, std::less<uint32> > intSet;
  intSet addresses;

  ArrayVector singleArrayVector(Array a) {
    ArrayVector retval;
    retval.push_back(a);
    return retval;
  }

  void Array::setArrayIOInterface(Interface *a_io) {
    io = a_io;
  }

  Interface* Array::getArrayIOInterface() {
    return io;
  }

  void outputDoublePrecisionFloat(char *buf, double num) {
    char temp_buf[100];
    char *tbuf;
    int len;
    
    tbuf = temp_buf;
    
    if (num>=0) {
      sprintf(tbuf," ");
      tbuf++;
    }
    if (IsNaN(num))
      sprintf(tbuf,"   nan");
    else if (fabs(num)>=0.1f && fabs(num)<1.0f || num == 0.0f)
      sprintf(tbuf,"  %0.15f",num);
    else if (fabs(num)>=0.01f && fabs(num)<0.1f)
      sprintf(tbuf,"  %0.16f",num);
    else if (fabs(num)>=0.001f && fabs(num)<0.01f)
      sprintf(tbuf,"  %0.17f",num);
    else if (fabs(num)>=1.0f && fabs(num)<10.0f)
      sprintf(tbuf,"  %1.15f",num);
    else if (fabs(num)>=10.0f && fabs(num)<100.0f)
      sprintf(tbuf," %2.13f",num);
    else if (fabs(num)>=100.0f && fabs(num)<1000.0f)
      sprintf(tbuf,"%3.12f",num);
    else 
      sprintf(tbuf,"  %1.14e",num);
    len = strlen(temp_buf);
    memcpy(buf,temp_buf,len);
    memset(buf+len,' ',24-len);
    buf[24] = 0;
  }
  
  void outputSinglePrecisionFloat(char *buf, float num) {
    char temp_buf[100];
    char *tbuf;
    int len;
    
    tbuf = temp_buf;
    
    if (num>=0) {
      sprintf(tbuf," ");
      tbuf++;
    }
    if (IsNaN(num))
      sprintf(tbuf,"   nan");
    else if (fabs(num)>=0.1f && fabs(num)<1.0f || num == 0.0f)
      sprintf(tbuf,"  %0.8f",num);
    else if (fabs(num)>=0.01f && fabs(num)<0.1f)
      sprintf(tbuf,"  %0.9f",num);
    else if (fabs(num)>=0.001f && fabs(num)<0.01f)
      sprintf(tbuf,"  %0.10f",num);
    else if (fabs(num)>=1.0f && fabs(num)<10.0f)
      sprintf(tbuf,"  %1.7f",num);
    else if (fabs(num)>=10.0f && fabs(num)<100.0f)
      sprintf(tbuf," %2.6f",num);
    else if (fabs(num)>=100.0f && fabs(num)<1000.0f)
      sprintf(tbuf,"%3.5f",num);
    else
      sprintf(tbuf,"  %1.7e",num);
    len = strlen(temp_buf);
    memcpy(buf,temp_buf,len);
    memset(buf+len,' ',17-len);
    buf[17] = 0;
  }

  void printObjectBalance() {
    sprintf(msgBuffer,"Object balance is : %d\n",objectBalance);
    io->outputMessage(msgBuffer);
  }
  
  void dumpAllArrays() {
    intSet::iterator i = addresses.begin();
    int j = 0;
    while (i != addresses.end()) {
      uint32 addr = *i;
      Array *aptr = (Array *) addr;
      std::cout << "Array Number " << j << " = " << aptr->getReferenceCount() << "\n";
      printf("  Address = %08x\n",addr);
      aptr->printMe(1000,80);
      i++;
      j++;
    }
  }

  inline void Array::copyObject(const Array& copy) {
    if (copy.dp) 
      dp = copy.dp->getCopy();
    else
      dp = NULL;
  }

  inline void Array::deleteContents(void) {
    if (dp) {
      int m;
      m = dp->deleteCopy();
      if (m <= 1)
	delete dp;
      dp = NULL;
    }   
  }
  
  void* Array::allocateArray(Class type, uint32 length, const stringVector& names) {
    switch(type) {
    case FM_CELL_ARRAY: {
      Array *dp = new Array[length];
      for (int i=0;i<length;i++)
	dp[i] = Array(FM_DOUBLE);
      return dp;
    }
    case FM_STRUCT_ARRAY: {
      Array *dp = new Array[length*names.size()];
      for (int i=0;i<length*names.size();i++)
	dp[i] = Array(FM_DOUBLE);
      return dp;
    }
    case FM_LOGICAL:
      return Malloc(sizeof(logical)*length);
    case FM_UINT8:
      return Malloc(sizeof(uint8)*length);
    case FM_INT8:
      return Malloc(sizeof(int8)*length);
    case FM_UINT16:
      return Malloc(sizeof(uint16)*length);
    case FM_INT16:
      return Malloc(sizeof(int16)*length);
    case FM_UINT32:
      return Malloc(sizeof(uint32)*length);
    case FM_INT32:
      return Malloc(sizeof(int32)*length);
    case FM_FLOAT:
      return Malloc(sizeof(float)*length);
    case FM_DOUBLE:
      return Malloc(sizeof(double)*length);
    case FM_COMPLEX:
      return Malloc(2*sizeof(float)*length);
    case FM_DCOMPLEX:
      return Malloc(2*sizeof(double)*length);
    case FM_STRING:
      return Malloc(sizeof(char)*length);
    }
    return 0;
  }


  bool* Array::getBinaryMap(uint32 maxD) {
    bool* map = (bool*) Malloc(maxD*sizeof(bool));
    int N = getLength();
    constIndexPtr rp = (constIndexPtr) dp->getData();
    for (int i=0;i<N;i++) {
      indexType n = (rp[i]-1);
      if (n < 0 || n >= maxD) {
	Free(map);
	throw Exception("Array index exceeds bounds");
      }
      map[n] = true;
    }
    return map;
  }

  uint32 Array::getMaxAsIndex()  {
    indexType maxval;
    int k;
    constIndexPtr rp = (constIndexPtr) dp->getData();
    int K = getLength();
    maxval = rp[0];
    for (k=1;k<K;k++)
      if (rp[k] > maxval) maxval = rp[k];
    if (maxval <= 0) throw Exception("Illegal zero or negative index");
    return maxval;
  }

  void Array::toOrdinalType()  {
    if (isSparse())
      makeDense();
    switch(dp->dataClass) {
    case FM_LOGICAL:
      {
	// We make a first pass through the array, and count the number of
	// non-zero entries.
	const logical *rp = (const logical *) dp->getData();
	int indexCount = 0;
	int len = getLength();
	int i;
	for (i=0;i<len;i++)
	  if (rp[i] != 0) indexCount++;
	// Allocate space to hold the new type.
	indexType *lp = (indexType *) Malloc(indexCount*sizeof(indexType));
	indexType *qp = lp;
	for (i=0;i<len;i++) 
	  if (rp[i] != 0) *qp++ = (indexType) (i+1);
	// Reset our data pointer to the new vector.
	Dimensions dimensions;
	dimensions[1] = 1;
	dimensions[0] = indexCount;
	// Change the class to an FM_UINT32.
	dp = dp->putData(FM_UINT32,dimensions,lp);
      }
      break;
    case FM_STRING:
      {
	throw Exception("Cannot convert string data types to indices.");
      }
      break;
    case FM_DCOMPLEX:
      {
	io->warningMessage("Imaginary part of complex index ignored.\n");
	// We convert complex values into real values
	const double *rp = (const double *) dp->getData();
	int len = getLength();
	indexType ndx;
	bool fractionalWarning = false;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = (indexType) rp[i<<1];
	  if (!fractionalWarning && (double) ndx != rp[i<<1]) {
	    io->warningMessage("Fractional part of index ignored.\n");
	    fractionalWarning = true;
	  }
	  if (ndx <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_COMPLEX:
      {
	io->warningMessage("Imaginary part of complex index ignored.\n");
	// We convert complex values into real values
	const float *rp = (const float *) dp->getData();
	int len = getLength();
	indexType ndx;
	bool fractionalWarning = false;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = (indexType) rp[i<<1];
	  if (!fractionalWarning && (double) ndx != rp[i<<1]) {
	    io->warningMessage("Fractional part of index ignored.\n");
	    fractionalWarning = true;
	  }
	  if (ndx <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_DOUBLE:
      {
	const double *rp = (const double *) dp->getData();
	int len = getLength();
	indexType ndx;
	bool fractionalWarning = false;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = (indexType) rp[i];
	  if (!fractionalWarning && (double) ndx != rp[i]) {
	    io->warningMessage("Fractional part of index ignored.\n");
	    fractionalWarning = true;
	  }
	  if (ndx <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_FLOAT:
      {
	const float *rp = (const float *) dp->getData();
	int len = getLength();
	indexType ndx;
	bool fractionalWarning = false;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = (indexType) rp[i];
	  if (!fractionalWarning && (double) ndx != rp[i]) {
	    io->warningMessage("Fractional part of index ignored.\n");
	    fractionalWarning = true;
	  }
	  if (ndx <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_INT32:
      {
	const int32 *rp = (const int32 *) dp->getData();
	int len = getLength();
	indexType ndx;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = rp[i];
	  if (rp[i] <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_UINT32:
      {
	const uint32 *rp = (const uint32 *) dp->getData();
	int len = getLength();
	// Allocate space to hold the new type
	for (int i=0;i<len;i++) 
	  if (rp[i] <= 0)
	    throw Exception("Zero or negative index encountered.");
      }
      break;
    case FM_INT16:
      {
	const int16 *rp = (const int16 *) dp->getData();
	int len = getLength();
	indexType ndx;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = rp[i];
	  if (rp[i] <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_UINT16:
      {
	const uint16 *rp = (const uint16 *) dp->getData();
	int len = getLength();
	indexType ndx;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = rp[i];
	  if (rp[i] <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_INT8:
      {
	const int8 *rp = (const int8 *) dp->getData();
	int len = getLength();
	indexType ndx;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = rp[i];
	  if (rp[i] <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_UINT8:
      {
	const uint8 *rp = (const uint8 *) dp->getData();
	int len = getLength();
	indexType ndx;
	// Allocate space to hold the new type
	indexType *lp = (indexType *) Malloc(len*sizeof(indexType));
	for (int i=0;i<len;i++) {
	  ndx = rp[i];
	  if (rp[i] <= 0)
	    throw Exception("Zero or negative index encountered.");
	  lp[i] = ndx;
	}
	dp = dp->putData(FM_UINT32,dp->getDimensions(),lp);
      }
      break;
    case FM_CELL_ARRAY:
      {
	throw Exception("Cannot convert cell arrays to indices.");
      }
      break;
    case FM_STRUCT_ARRAY:
      {
	throw Exception("Cannot convert structure arrays to indices.");
      }
      break;
    }
  }

  /**
   * Compute the ordinal index for a given field name from a
   * structure when the list of field names is given.
   */
  int32 getFieldIndexFromList(std::string fName,const stringVector& fieldNames) {
    bool foundName = false;
    uint32 i;
    i = 0;
    while (i<fieldNames.size() && !foundName) {
      foundName = (fieldNames[i] == fName);
      if (!foundName) i++;
    }
    if (foundName) 
      return i;
    else
      return -1;
  }
  
  /**
   * Compute the ordinal index for a given field name from a 
   * structure using the current set of field names.
   */
  int32 Array::getFieldIndex(std::string fName) {
    return getFieldIndexFromList(fName,dp->fieldNames);
  }

  Array::Array() {
    dp = NULL;
  }

  /**
   * Create a variable with the specified contents.
   */
  Array::Array(Class type, const Dimensions& dims, void* data, bool sparse, const stringVector& fnames) {
    dp = new Data(type, dims, data, sparse, fnames);
  }

  Array::Array(Class type) {
    Dimensions dims;
    dp = new Data(type, dims, NULL);
  }

  /**
   * Destructor - free the data object.
   */
  Array::~Array() {
    if (dp) {
      int m;
      m = dp->deleteCopy();
      if (m <= 1)
      	delete dp;
      dp = NULL;
    }   
  }

  void Array::operator=(const Array &copy) {
    if (this == &copy) return;
    if (dp) {
      int m;
      m = dp->deleteCopy();
      if (m <= 1)
	delete dp;
      dp = NULL;
    }   
    if (copy.dp) 
      dp = copy.dp->getCopy();
    else
      dp = NULL;
  }

  const int Array::getReferenceCount() const {
    if (dp)
      return dp->numberOfOwners();
    else
      return 0;
  }

  const int Array::getLength() const {
    if (dp)
      return dp->dimensions.getElementCount();
    else
      return 0;
  }

  Dimensions Array::getDimensions() const {
    if (dp)
      return dp->dimensions;
    else
      return Dimensions();
  }

  stringVector Array::getFieldNames() const {
    if (dp)
      return dp->fieldNames;
    else
      return stringVector();
  }

  const int Array::getDimensionLength(int t) const {
    if (dp)
      return dp->dimensions[t];
    else
      return 0;
  }

  const void *Array::getSparseDataPointer() const {
    if (dp)
      return dp->getData();
    else
      return NULL;
  }

  const void *Array::getDataPointer() const {
    if (isSparse())
      throw Exception("operation does not support sparse matrix arguments.");
    if (dp)
      return dp->getData();
    else
      return NULL;
  }

  void Array::ensureSingleOwner() {
    if (dp->numberOfOwners() > 1) {
      if (!dp->sparse) {
	void *np = allocateArray(dp->dataClass,getLength(),dp->fieldNames);
	copyElements(0,np,0,getLength());
	dp = dp->putData(dp->dataClass,dp->dimensions,np,
			 dp->sparse,dp->fieldNames);
      } else {
	dp = dp->putData(dp->dataClass,dp->dimensions,
			 CopySparseMatrix(dp->dataClass,
					  dp->dimensions[0],
					  dp->dimensions[1],
					  dp->getData()),
			 dp->sparse,dp->fieldNames);	
      }
    }
  }

  void* Array::getReadWriteDataPointer() {
    if (isSparse()) {
      io->warningMessage("Warning: sparse matrix converted to full for operation.");
      makeDense();
    }
    ensureSingleOwner();
    return dp->getWriteableData();
  }

  void Array::setDataPointer(void* rp) {
    dp = dp->putData(dp->dataClass,dp->dimensions,rp,
		     dp->sparse,dp->fieldNames);
  }

  void Array::resize(Dimensions& a) {
    Dimensions newSize;
    // Make a copy of the current dimension vector, and
    // compute the new dimension size.
    newSize = dp->dimensions;
    newSize.expandToCover(a);
    // Check to see if the dimensions are unchanged.
    if (newSize.equals(dp->dimensions)) return;
    // Check to see if the total number of elements is unchanged.
    if (newSize.getElementCount() == getLength()) {
      ensureSingleOwner();
      dp->dimensions = newSize;
      return;
    }
    if (isSparse()) throw Exception("Cannot resize sparse arrays.");
    // Allocate space for our new size.
    void *dst_data = allocateArray(dp->dataClass,newSize.getElementCount(),dp->fieldNames);
    if (!isEmpty()) {
      // Initialize a pointer to zero.
      Dimensions curPos(dp->dimensions.getLength());
      // Because we copy & convert data a column at a time, we retrieve
      // the number of rows in each column.
      int rowCount = dp->dimensions[0];
      // Track our offset into the original data.
      int srcIndex = 0;
      // Loop until we have exhausted the original data.
      int dstIndex;
      while (curPos.inside(dp->dimensions)) {
	// Get the destination index for the current source position.
	dstIndex = newSize.mapPoint(curPos);
	// Copy the data from our original data structure to the
	// new data structure, starting from the source index
	// srcIndex, and moving to dstIndex.
	copyElements(srcIndex,dst_data,dstIndex,rowCount);
	// Update the column number (as we have just copied an
	// entire column).
	curPos.incrementModulo(dp->dimensions,1);
	// Advance the source data pointer so that it points to the
	// start of the next column.
	srcIndex += rowCount;
      }
    } 
    dp = dp->putData(dp->dataClass,newSize,dst_data,
		     dp->sparse,dp->fieldNames);
  }

  void Array::vectorResize(int max_index) {
    if (max_index > getLength()) {
      Dimensions newDim;
      if (isEmpty() || dp->dimensions.isScalar()) {
	newDim.reset();
	newDim[0] = 1;
	newDim[1] = max_index;
      } else if (dp->dimensions.isVector()) {
	newDim = dp->dimensions;
	if (dp->dimensions[0] != 1)
	  newDim[0] = max_index;
	else
	  newDim[1] = max_index;
      } else {
	// First reshape it
	Dimensions tDim(2);
	tDim[0] = 1;
	tDim[1] = getLength();
	reshape(tDim);
	newDim.reset();
	newDim[0] = 1;
	newDim[1] = max_index;
      }
      resize(newDim);
    }
  }

  /**
   * Reshape an array.  This is only legal if the number of 
   * elements remains the same after reshaping.
   */
  void Array::reshape(Dimensions& a)  {
    if (isSparse())
      throw Exception("reshape not supported for sparse arrays.");
    if (a.getElementCount() != getLength())
      throw Exception("Reshape operation cannot change the number of elements in array.");
    ensureSingleOwner();
    dp->dimensions = a;
  }

  /**
   * Hermitian transpose our array.  By default, this is just a transpose
   * operation.  The distinction is made in complex classed by overriding
   * this method.
   */
  void Array::hermitian()  {
    if (!is2D())
      throw Exception("Cannot apply Hermitian transpose operation to multi-dimensional array.");
    if (isEmpty())
      return;
    if (!isComplex())
      transpose();
    else {
      if (isSparse()) {
	int rows = getDimensionLength(0);
	int cols = getDimensionLength(1);
	void *qp = SparseArrayHermitian(dp->dataClass, rows, cols, dp->getData());
	Dimensions newDim(2);
	newDim[0] = cols;
	newDim[1] = rows;
	dp = dp->putData(dp->dataClass,newDim,qp,true);
	return;	
      }
      if (dp->dataClass == FM_COMPLEX) {
	// Allocate space for our transposed array
	void *dstPtr = allocateArray(dp->dataClass,getLength());
	float *qp, *sp;
	int i, j;
	int rowCount;
	int colCount;
      
	rowCount = dp->dimensions[0];
	colCount = dp->dimensions[1];
	int ptr;
	qp = (float*) dstPtr;
	sp = (float*) dp->getData();
	ptr = 0;
	for (i=0;i<rowCount;i++)
	  for (j=0;j<colCount;j++) {
	    qp[2*ptr] = sp[2*(i + j*rowCount)];
	    qp[2*ptr+1] = -sp[2*(i + j*rowCount) + 1];
	    ptr++;
	  }
	Dimensions newDim(2);
	newDim[0] = colCount;
	newDim[1] = rowCount;
	dp = dp->putData(FM_COMPLEX,newDim,dstPtr);
      } else {
	// Allocate space for our transposed array
	void *dstPtr = allocateArray(dp->dataClass,getLength());
	double *qp, *sp;
	int i, j;
	int rowCount;
	int colCount;
      
	rowCount = dp->dimensions[0];
	colCount = dp->dimensions[1];
	int ptr;
	qp = (double*) dstPtr;
	sp = (double*) dp->getData();
	ptr = 0;
	for (i=0;i<rowCount;i++)
	  for (j=0;j<colCount;j++) {
	    qp[2*ptr] = sp[2*(i + j*rowCount)];
	    qp[2*ptr+1] = -sp[2*(i + j*rowCount) + 1];
	    ptr++;
	  }
	Dimensions newDim(2);
	newDim[0] = colCount;
	newDim[1] = rowCount;
	dp = dp->putData(FM_DCOMPLEX,newDim,dstPtr);
     }
    }
  }

  /**
   * Transpose our array.
   */
  void Array::transpose()  {
    if (!is2D())
      throw Exception("Cannot apply transpose operation to multi-dimensional array.");
    if (isEmpty())
      return; 
    if (isSparse()) {
      int rows = getDimensionLength(0);
      int cols = getDimensionLength(1);
      void *qp = SparseArrayTranspose(dp->dataClass, rows, cols, dp->getData());
      Dimensions newDim(2);
      newDim[0] = cols;
      newDim[1] = rows;
      dp = dp->putData(dp->dataClass,newDim,qp,true);
      return;
    }
    // Allocate space for our transposed array
    void *dstPtr = allocateArray(dp->dataClass,getLength(),dp->fieldNames);
    int i, j;
    int rowCount;
    int colCount;
  
    rowCount = dp->dimensions[0];
    colCount = dp->dimensions[1];
    int ptr;
    ptr = 0;
    for (i=0;i<rowCount;i++)
      for (j=0;j<colCount;j++) {
	copyElements(i+j*rowCount,dstPtr,ptr,1);
	ptr++;
      }
    Dimensions newDim(2);
    newDim[0] = colCount;
    newDim[1] = rowCount;
    dp = dp->putData(dp->dataClass,newDim,dstPtr,
		     dp->sparse,dp->fieldNames);
  }

  /**
   * Get our data class (of type Class).
   */
  Class Array::getDataClass() const {
    if (dp)
      return dp->dataClass;
    else
      return FM_DOUBLE;
  }

  /**
   * Calculate the size of each element in this array.
   */
  int Array::getElementSize() const {
    switch(dp->dataClass) {
    case FM_CELL_ARRAY:
      return sizeof(Array);
    case FM_STRUCT_ARRAY:
      return (sizeof(Array)*dp->fieldNames.size());
    case FM_LOGICAL:
      return sizeof(logical);
    case FM_UINT8:
      return sizeof(uint8);
    case FM_INT8:
      return sizeof(int8);
    case FM_UINT16:
      return sizeof(uint16);
    case FM_INT16:
      return sizeof(int16);
    case FM_UINT32:
      return sizeof(uint32);
    case FM_INT32:
      return sizeof(int32);
    case FM_FLOAT:
      return sizeof(float);
    case FM_DOUBLE:
      return sizeof(double);
    case FM_COMPLEX:
      return sizeof(float)*2;
    case FM_DCOMPLEX:
      return sizeof(double)*2;
    case FM_STRING:
      return sizeof(char);
    }
    return 0;
  }

  /**
   * Calculate the total number of bytes required to store this array.
   */
  int Array::getByteSize() const {
    if (isSparse())
      throw Exception("Byte size calculation not supported for sparse arrays.");
    return getElementSize()*getLength();
  }

#define caseReal(caseLabel,dpType) \
  case caseLabel:\
  {\
    const dpType* qp = (const dpType*) dp->getData();\
    bool allPositive = true;\
    int N = dp->dimensions[0];\
    int i, j;\
    for (i=0;i<N;i++)\
      for (j=1;j<N;j++)\
        if (qp[i+j*N] != qp[j+i*N]) return false;\
    return true;\
    break;\
  }

#define caseComplex(caseLabel,dpType) \
  case caseLabel:\
  {\
    const dpType* qp = (const dpType*) dp->getData();\
    bool allPositive = true;\
    int N = dp->dimensions[0];\
    int i, j;\
    for (i=0;i<N;i++)\
      for (j=1;j<N;j++) {\
        if (qp[2*(i+j*N)] != qp[2*(j+i*N)]) return false;\
        if (qp[2*(i+j*N)+1] != -qp[2*(j+i*N)+1]) return false;\
      }\
    return true;\
    break;\
  }

  const bool Array::isSymmetric() const {
    if (!is2D()) return false;
    if (isReferenceType()) return false;
    if (isSparse())
      throw Exception("Cannot determine symmetry of sparse arrays");
    switch(dp->dataClass) {
      caseReal(FM_INT8,int8);
      caseReal(FM_INT16,int16);
      caseReal(FM_INT32,int32);
      caseReal(FM_UINT8,uint8);
      caseReal(FM_UINT16,uint16);
      caseReal(FM_UINT32,uint32);
      caseReal(FM_FLOAT,float);
      caseReal(FM_DOUBLE,double);
      caseComplex(FM_COMPLEX,float);
      caseComplex(FM_DCOMPLEX,double);
    }
  }
 
#undef caseReal
#undef caseComplex

  /**
   * Returns true if we are positive.
   */
#define caseMacro(caseLabel,dpType) \
   case caseLabel:\
   {\
    const dpType* qp = (const dpType*) dp->getData();\
    bool allPositive = true;\
    int len = getLength();\
    int i = 0;\
    while (allPositive && (i<len)) {\
      allPositive = allPositive && (qp[i] >= 0);\
      i++;\
    }\
    return allPositive;\
   }

  const bool Array::isPositive() const {
    if (dp->dataClass == FM_UINT8 || dp->dataClass == FM_UINT16 || dp->dataClass == FM_UINT32)
      return true;
    if (dp->dataClass == FM_COMPLEX || dp->dataClass == FM_DCOMPLEX)
      return false;
    if (isSparse())
      throw Exception("isPositive not supported for sparse arrays.");
    switch (dp->dataClass) {
      caseMacro(FM_FLOAT,float);
      caseMacro(FM_DOUBLE,double);
      caseMacro(FM_INT8,int8);
      caseMacro(FM_INT16,int16);
      caseMacro(FM_INT32,int32);
    }
    return false;
  }
#undef caseMacro

  const bool Array::isRealAllZeros() const  {
    bool allZeros;
    int len = getLength();
    int i;

#define caseMacro(caseLabel,dpType,testcode) \
  case caseLabel:\
  {\
     const dpType* qp = (const dpType*) dp->getData();\
     while (allZeros && (i<len)) {\
       allZeros = allZeros && (testcode);\
       i++;\
     }\
     return allZeros;\
  }

    allZeros = true;
    i = 0;
    if (isSparse())
      throw Exception("isPositive not supported for sparse arrays.");
    switch (dp->dataClass) {
      caseMacro(FM_LOGICAL,logical,qp[i]==0);
      caseMacro(FM_UINT8,uint8,qp[i]==0);
      caseMacro(FM_INT8,int8,qp[i]==0);
      caseMacro(FM_UINT16,uint16,qp[i]==0);
      caseMacro(FM_INT16,int16,qp[i]==0);
      caseMacro(FM_UINT32,uint32,qp[i]==0);
      caseMacro(FM_INT32,int32,qp[i]==0);
      caseMacro(FM_FLOAT,float,qp[i]==0.0f);
      caseMacro(FM_DOUBLE,double,qp[i]==0.0);
      caseMacro(FM_COMPLEX,float,qp[i<<1]==0.0f);
      caseMacro(FM_DCOMPLEX,double,qp[i<<1]==0.0);
    default:
      throw Exception("Unable to convert variable type to test for if/while statement");
    }
#undef caseMacro
  }


#define caseMacroReal(caseLabel,type) \
  case caseLabel:\
    retval = (*((const type*) x_dp) == *((const type*) y_dp)); \
    break;

#define caseMacroComplex(caseLabel,type) \
  case caseLabel:\
    retval = (((const type*) x_dp)[0] == ((const type*) y_dp)[0]) && \
             (((const type*) x_dp)[1] == ((const type*) y_dp)[1]); \
    break;

  const bool Array::testCaseMatchScalar(Array x) const {
    if (isSparse())
      throw Exception("isPositive not supported for sparse arrays.");
    // Now we have to compare ourselves to the argument.  Check for the
    // case that we are a string type
    if (isString()) {
      // If x is not a string, we cannot match
      if (!x.isString())
	return false;
      // if x is a string do a string, string compare.
      char *s1 = getContentsAsCString();
      char *s2 = x.getContentsAsCString();
      bool retval = strcmp(s1,s2) == 0;
      Free(s1);
      Free(s2);
      return retval;
    }
    if (!x.isScalar())
      return false;
   //  OK - we are not a string, so we have a numerical comparison.  To do this,
    // we have to make both objects the same type.
    Array y = *this;
    if (x.getDataClass() > y.getDataClass())
      y.promoteType(x.getDataClass());
    else
      x.promoteType(y.getDataClass());
    // Finally, we can do a compare....
    const void *x_dp = x.dp->getData();
    const void *y_dp = y.dp->getData();
    bool retval;
    switch(x.dp->dataClass) {
      caseMacroReal(FM_LOGICAL,logical);
      caseMacroReal(FM_UINT8,uint8);
      caseMacroReal(FM_INT8,int8);
      caseMacroReal(FM_UINT16,uint16);
      caseMacroReal(FM_INT16,int16);
      caseMacroReal(FM_UINT32,uint32);
      caseMacroReal(FM_INT32,int32);
      caseMacroReal(FM_FLOAT,float);
      caseMacroReal(FM_DOUBLE,double);
      caseMacroComplex(FM_COMPLEX,float);
      caseMacroComplex(FM_DCOMPLEX,double);
    }
    return retval;
  }
#undef caseMacroReal
#undef caseMacroComplex

  const bool Array::testForCaseMatch(Array x) const  {
    if (isSparse())
      throw Exception("isPositive not supported for sparse arrays.");
    // We had better be a scalar
    if (!(isScalar() || isString()))
      throw Exception("Switch argument must be a scalar or a string");
    // And we had better not be a reference type
    if (isReferenceType())
      throw Exception("Switch argument cannot be a reference type (struct or cell array)");
    // If x is a scalar, we just need to call the scalar version
    if (x.isScalar() || x.isString())
      return testCaseMatchScalar(x);
    if (x.dp->dataClass != FM_CELL_ARRAY)
      throw Exception("Case arguments must either be a scalar or a cell array");
    const Array* qp = (const Array*) x.dp->getData();
    int len;
    len = x.getLength();
    bool foundMatch = false;
    int i = 0;
    while (i<len && !foundMatch) {
      foundMatch = testCaseMatchScalar(qp[i]);
      i++;
    }
    return foundMatch;
  }

  /**
   * Returns TRUE if we are empty (we have no elements).
   */
  const bool Array::isEmpty() const {
    return ((getLength() == 0) || (dp == NULL) || 
	    (!dp->getData()));
  }

  /*
   * Returns TRUE if we have only a single element.
   */
  const bool Array::isScalar() const {
    return dp->dimensions.isScalar();
  }

  /**
   * Returns TRUE if we are 2-Dimensional.
   */
  const bool Array::is2D() const {
    return dp->dimensions.is2D();
  }

  /**
   * Returns TRUE if we are a vector.
   */
  const bool Array::isVector() const {
    return dp->dimensions.isVector();
  }

  /**
   * Returns TRUE if we are a reference type (cell array or
   * struct array).
   */
  const bool Array::isReferenceType() const {
    return ((dp->dataClass == FM_CELL_ARRAY) ||
	    (dp->dataClass == FM_STRUCT_ARRAY));
  }

  /**
   * Returns TRUE if we are a complex data type.
   */
  const bool Array::isComplex() const {
    return (dp->dataClass == FM_DCOMPLEX || dp->dataClass == FM_COMPLEX);
  }

  /**
   * Returns TRUE if we are a real data type.
   */
  const bool Array::isReal() const {
    return (!isComplex());
  }

  /**
   * Returns TRUE if we are a string.
   */
  const bool Array::isString() const {
    return (dp->dataClass == FM_STRING);
  }

  const bool Array::isSparse() const {
    return (dp->sparse);
  }

  void Array::copyElements(int srcIndex, void* dstPtr, int dstIndex, 
			   int count) {
    int elSize(getElementSize());
    if (isSparse())
      throw Exception("copyElements not supported for sparse arrays.");
    switch(dp->dataClass) {
    case FM_CELL_ARRAY:
      {
	const Array* sp = (const Array*) dp->getData();
	Array* qp = (Array*) dstPtr;
	for (int i=0;i<count;i++)
	  qp[dstIndex+i] = sp[srcIndex+i];
      }
      break;
    case FM_STRUCT_ARRAY:
      {
	const Array* sp = (const Array*) dp->getData();
	Array* qp = (Array*) dstPtr;
	int fieldCount(dp->fieldNames.size());
	for (int i=0;i<count;i++)
	  for (int j=0;j<fieldCount;j++) 
	    qp[(dstIndex+i)*fieldCount+j] = sp[(srcIndex+i)*fieldCount+j];
      }
      break;
    default:
      {
	const char* sp = (const char*) dp->getData();
	char* qp = (char *) dstPtr;
	memcpy(qp + dstIndex*elSize, sp + srcIndex*elSize, count*elSize);
      }
      break;
    }
  }

  /**
   * Promote our data to a new type.
   *
   * Copy data from our data array to the specified
   * array, converting the data as we go.  We can only
   * convert data to or from base types.  So if the source
   * or destination types are reference types, we cannot
   * perform the conversion.
   *
   * For the remaining types, we have a matrix of
   * possibilities.  Here we list the conversion rules.
   *
   * Source type
   *  - string
   *    - logical dest = (source == 0) ? 0 : 1
   *    - real dest = (double) source
   *    - complex dest = (double) source
   *  - logical
   *    - string dest = (char) source
   *    - real   dest = (double) source
   *    - complex dest = (double) source
   *  - real
   *    - string dest = (char) source
   *    - logical dest = (source == 0) ? 0 : 1
   *    - complex dest = (double) source
   *  - complex
   *    - string dest = (char) real(source)
   *    - logical dest = (real(source) == 0 && imag(source) == 0) ? 0:1
   *    - real dest = real(source)
   */
  void Array::promoteType(Class dstClass, stringVector fNames) {
    int elCount;
    int elSize;
    void *dstPtr;

    if (isEmpty()) {
      dp = dp->putData(dstClass,dp->dimensions,NULL,false,fNames);
      return;
    }
    // Handle the reference types.
    // Cell arrays can be promoted with no effort to cell arrays.
    if (dp->dataClass == FM_CELL_ARRAY) 
      if (dstClass == FM_CELL_ARRAY)
	return;
      else
	throw Exception("Cannot convert cell-arrays to any other type.");
    // Structure arrays can be promoted to structure arrays with different
    // field structures, but have to be rearranged.
    if (dp->dataClass == FM_STRUCT_ARRAY)
      if (dstClass == FM_STRUCT_ARRAY) {
	// TODO: Generalize this code to allow for one more field in destination
	// than in source...
	if (dp->fieldNames.size() >  fNames.size())
	  throw Exception("Cannot combine structures with different fields if the combination requires fields to be deleted from one of the structures.");
	// We are promoting a struct array to a struct array.
	// To do so, we have to make sure that the field names work out.
	// The only thing we must check for is that every field name
	// in fieldnames is present in fnames.
	int extraCount = 0;
	int matchCount = 0;
	int ndx;
	int i;
	for (i=0;i<fNames.size();i++) {
	  ndx = getFieldIndex(fNames[i]);
	  if (ndx == -1)
	    extraCount++;
	  else
	    matchCount++;
	}
	// Now, matchCount should be equal to the size of fieldNames
	if (matchCount != dp->fieldNames.size())
	  throw Exception("Cannot combine structures with different fields if the combination requires fields to be deleted from one of the structures.");
	void *dstPtr = allocateArray(dp->dataClass,getLength(),fNames);
	const Array *src_rp = (const Array*) dp->getData();
	Array * dst_rp = (Array*) dstPtr;
	int elCount(getLength());
	int fieldCount(dp->fieldNames.size());
	int newFieldCount(fNames.size());;
	// Now we have to copy our existing fields into the new order...
	for (i=0;i<fieldCount;i++) {
	  int newNdx = getFieldIndexFromList(dp->fieldNames[i],fNames);
	  for (int j=0;j<elCount;j++)
	    dst_rp[j*newFieldCount + newNdx] = src_rp[j*fieldCount + i];
	}
	dp = dp->putData(dp->dataClass,dp->dimensions,dstPtr,false,fNames);
	return;
      }
      else
	throw Exception("Cannot convert struct-arrays to any other type.");
    // Catch attempts to convert data types to reference types.
    if ((dstClass == FM_CELL_ARRAY) || (dstClass == FM_STRUCT_ARRAY)) 
      throw Exception("Cannot convert base types to reference types.");
    // Do nothing for promoting to same class (no-op).
    if (dstClass == dp->dataClass) return;
    if (isSparse()) {
      dp = dp->putData(dstClass,dp->dimensions,
		       TypeConvertSparse(dp->dataClass,
					 dp->dimensions[0],
					 dp->dimensions[1],
					 dp->getData(),
					 dstClass),
		       true);
      return;
    }
    elCount = getLength();
    // We have to promote...
    dstPtr = allocateArray(dstClass,elCount);
    int count = elCount;
    switch (dp->dataClass) {

#define caseMacro(caseLabel,dpType,convCode) \
case caseLabel: \
{ dpType* qp = (dpType*) dstPtr; \
  for (int i=0;i<count;i++) convCode; \
} \
break;

    case FM_STRING:
      {
	const char* sp = (const char *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);
	}
      }
      break;
    case FM_LOGICAL:
      {
	const logical* sp = (const logical *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);
	}
      }
      break;
    case FM_UINT8:
      {
	const uint8* sp = (const uint8 *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_INT8:
      {
	const int8* sp = (const int8 *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_UINT16:
      {
	const uint16* sp = (const uint16 *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_INT16:
      {
	const int16* sp = (const int16 *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_UINT32:
      {
	const uint32* sp = (const uint32 *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_INT32:
      {
	const int32* sp = (const int32 *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_FLOAT:
      {
	const float* sp = (const float *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_DOUBLE:
      {
	const double* sp = (const double *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = (sp[i]==0) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i]);
	  caseMacro(FM_COMPLEX,float,qp[i<<1] = (float) sp[i]);
	  caseMacro(FM_DCOMPLEX,double,qp[i<<1] = (double) sp[i]);	
	}
      }
      break;
    case FM_COMPLEX:
      {
	const float* sp = (const float *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i<<1]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = ((sp[i<<1]==0.0) && (sp[(i<<1) + 1] == 0.0)) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i<<1]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i<<1]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i<<1]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i<<1]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i<<1]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i<<1]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i<<1]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i<<1]);
	  caseMacro(FM_DCOMPLEX,double,{qp[i<<1]=(double)sp[i<<1];qp[(i<<1)+1]=(double)sp[(i<<1)+1];});
	}
      }
      break;
    case FM_DCOMPLEX:
      {
	const double* sp = (const double *) dp->getData();
	switch (dstClass) {
	  caseMacro(FM_STRING,char,qp[i] = (char) sp[i<<1]);
	  caseMacro(FM_LOGICAL,logical,qp[i] = ((sp[i<<1]==0.0) && (sp[(i<<1) + 1] == 0.0)) ? 0 : 1);
	  caseMacro(FM_UINT8,uint8,qp[i] = (uint8) sp[i<<1]);
	  caseMacro(FM_INT8,int8,qp[i] = (int8) sp[i<<1]);
	  caseMacro(FM_UINT16,uint16,qp[i] = (uint16) sp[i<<1]);
	  caseMacro(FM_INT16,int16,qp[i] = (int16) sp[i<<1]);
	  caseMacro(FM_UINT32,uint32,qp[i] = (uint32) sp[i<<1]);
	  caseMacro(FM_INT32,int32,qp[i] = (int32) sp[i<<1]);
	  caseMacro(FM_FLOAT,float,qp[i] = (float) sp[i<<1]);
	  caseMacro(FM_DOUBLE,double,qp[i] = (double) sp[i<<1]);
	  caseMacro(FM_COMPLEX,float,{qp[i<<1]=(float)sp[i<<1];qp[(i<<1)+1]=(float)sp[(i<<1)+1];});
	}
      }
      break;
    }
    dp = dp->putData(dstClass,dp->dimensions,dstPtr);
  }

#undef caseMacro

  void Array::promoteType(Class dstClass) {
    stringVector dummy;

    promoteType(dstClass,dummy);
  }

  /********************************************************************************
   * Constructors                                                                 *
   ********************************************************************************/

  Array::Array(const Array &copy) {
    copyObject(copy);
  }

  Array Array::diagonalConstructor(Array src, int diagonalOrder)  {
    Array retval;
    if (!src.isVector())
      throw Exception("Argument to diagonal constructor must by a vector!\n");
    int length;
    length = src.getLength();
    int M;
    // Calculate the size of the output matrix (square of size outLen + abs(diagonalOrder)).
    M = length + abs(diagonalOrder);
    Dimensions dims;
    dims[0] = M;
    dims[1] = M;
    // Allocate space for the output
    void *rp = allocateArray(src.dp->dataClass,dims.getElementCount(),src.dp->fieldNames);
    int i;
    int dstIndex;
    if (diagonalOrder < 0) {
      for (i=0;i<length;i++) {
	dstIndex = -diagonalOrder + i * (M+1);
	src.copyElements(i,rp,dstIndex,1);
      }
    } else {
      for (i=0;i<length;i++) {
	dstIndex = diagonalOrder*M + i * (M+1);
	src.copyElements(i,rp,dstIndex,1);
      }
    }
    return Array(src.dp->dataClass,dims,rp,false,src.dp->fieldNames);
  }

  Array Array::logicalConstructor(bool aval) {
    Dimensions dim;
    dim.makeScalar();
    logical *data = (logical *) allocateArray(FM_LOGICAL,1);
    *data = (logical) aval;
    return Array(FM_LOGICAL,dim,data);
  }
  
  Array Array::uint8Constructor(uint8 aval) {
    Dimensions dim;
    dim.makeScalar();
    uint8 *data = (uint8 *) allocateArray(FM_UINT8,1);
    *data = aval;
    return Array(FM_UINT8,dim,data);
  }

  Array Array::int8Constructor(int8 aval) {
    Dimensions dim;
    dim.makeScalar();
    int8 *data = (int8 *) allocateArray(FM_INT8,1);
    *data = aval;
    return Array(FM_INT8,dim,data);
  }

  Array Array::uint16Constructor(uint16 aval) {
    Dimensions dim;
    dim.makeScalar();
    uint16 *data = (uint16 *) allocateArray(FM_UINT16,1);
    *data = aval;
    return Array(FM_UINT16,dim,data);
  }

  Array Array::int16Constructor(int16 aval) {
    Dimensions dim;
    dim.makeScalar();
    int16 *data = (int16 *) allocateArray(FM_INT16,1);
    *data = aval;
    return Array(FM_INT16,dim,data);
  }

  Array Array::uint32Constructor(uint32 aval) {
    Dimensions dim;
    dim.makeScalar();
    uint32 *data = (uint32 *) allocateArray(FM_UINT32,1);
    *data = aval;
    return Array(FM_UINT32,dim,data);
  }

  Array Array::int32Constructor(int32 aval) {
    Dimensions dim;
    dim.makeScalar();
    int32 *data = (int32 *) allocateArray(FM_INT32,1);
    *data = aval;
    return Array(FM_INT32,dim,data);
  }

  Array Array::floatConstructor(float aval) {
    Dimensions dim;
    dim.makeScalar();
    float *data = (float *) allocateArray(FM_FLOAT,1);
    *data = aval;
    return Array(FM_FLOAT,dim,data);
  }

  Array Array::doubleConstructor(double aval) {
    Dimensions dim;
    dim.makeScalar();
    double *data = (double *) allocateArray(FM_DOUBLE,1);
    *data = aval;
    return Array(FM_DOUBLE,dim,data);
  }

  Array Array::doubleVectorConstructor(int len) {
    Dimensions dim;
    dim.makeScalar();
    dim[1] = len;
    double *data = (double*) allocateArray(FM_DOUBLE,len);
    return Array(FM_DOUBLE,dim,data);
  }

  Array Array::complexConstructor(float aval, float bval) {
    Dimensions dim;
    dim.makeScalar();
    float *data = (float *) allocateArray(FM_COMPLEX,1);
    data[0] = aval;
    data[1] = bval;
    return Array(FM_COMPLEX,dim,data);
  }

  Array Array::dcomplexConstructor(double aval, double bval) {
    Dimensions dim;
    dim.makeScalar();
    double *data = (double *) allocateArray(FM_DCOMPLEX,1);
    data[0] = aval;
    data[1] = bval;
    return Array(FM_DCOMPLEX,dim,data);
  }

  Array Array::emptyConstructor() {
    Dimensions dim;
    return Array(FM_DOUBLE,dim,NULL);
  }

  //!
  //@Module STRING String Arrays
  //@@Section VARIABLES
  //@@Usage
  //FreeMat supports a @|string| array type that operates very
  //much as you would expect.  Strings are stored internally as
  //8-bit values, and are otherwise similar to numerical arrays in
  //all respects.  In some respects, this makes strings arrays
  //less useful than one might imagine.  For example, numerical
  //arrays in 2-D are rectangular.  Thus, each row in the array
  //must have the same number of columns.  This requirement is
  //natural for numerical arrays and matrices, but consider a
  //string array.  If one wants to store multiple strings in
  //a single data structure, they must all be the
  //same length (unlikely).  The alternative is to use a cell
  //array of strings, in which case, each string can be of
  //arbitrary length.  Most of the functions that support 
  //strings in a set-theoretic way, like @|unique| and @|sort|
  //operate on cell-arrays of strings instead of string arrays.
  //Just to make the example concrete, here is the old way of
  //storing several strings in an array:
  //@<
  //% This is an error
  //A = ['hello';'bye']
  //% This is OK, but awkward
  //A = ['hello';'bye  ']
  //% This is the right way to do it
  //A = {'hello','bye'}
  //@>
  //
  //One important (tricky) point in FreeMat is the treatment
  //of escape sequences.  Recall that in @|C| programming, an
  //escape sequence is a special character that causes the output
  //to do something unusual.  FreeMat supports the following
  //escape sequences:
  //\begin{itemize}
  //\item @|\t| - causes a tab to be output
  //\item @|\r| - causes a carriage return (return to the beginning
  //     of the line of output, and overwrite the text)
  //\item @|\n| - causes a linefeed (advance to next line)
  //\end{itemize}
  //FreeMat follows the @|Unix/Linux| convention, that a @|\n|
  //causes both a carriage return and a linefeed.  
  //To put a single quote into a string use the MATLAB convention
  //of two single quotes, not the @|\'| sequence.  Here is an
  //example of a string containing some escape sequences:
  //@<
  //a = 'I can''t use contractions\n\tOr can I?\n'
  //@>
  //Now, note that the string itself still contains the @|\n|
  //characters.  With the exception of the @|\'|, the escape 
  //sequences do not affect the output unless the strings are 
  //put through @|printf| or @|fprintf|.  For example, if we 
  //@|printf| the variable @|a|, we see the @|\n| and @|\t| 
  //take effect:
  //@<
  //printf(a);
  //@>
  //The final complicating factor is on @|MSWin| systems.  There,
  //filenames regularly contain @|\| characters.  Thus, if you
  //try to print a string containing the filename 
  //@|C:\redball\timewarp\newton.txt|, the output will be mangled
  //because FreeMat thinks the @|\r|, @|\t| and @|\n| are escape
  //sequences.  You have two options.  You can use @|disp| to show
  //the filename (@|disp| does not do escape translation to be 
  //compatible with MATLAB).  The second option is to escape the
  //backslashes in the string, so that the string you send to 
  //@|printf| contains @|C:\\redball\\timewarp\\newton.txt|.
  //@<
  //% disp displays it ok
  //a = 'C:\redball\timewarp\newton.txt'
  //% printf makes a mess
  //printf(a)
  //% If we double up the slashes it works fine
  //a = 'C:\\redball\\timewarp\\newton.txt'
  //printf(a)
  //@>
  //!

  Array Array::stringConstructor(std::string astr) {
    int length;
    char *cp;
    length = astr.length();
    Dimensions dim;
    dim[0] = 1;
    dim[1] = length;
    cp = (char *) allocateArray(FM_STRING,length);
    memcpy(cp,astr.c_str(),length);
    return Array(FM_STRING,dim,cp);
  }

  Array Array::int32RangeConstructor(int32 minval, int32 stepsize, 
				     int32 maxval, bool vert) {
    Dimensions dim;
    int32 *rp = NULL;
    if (stepsize == 0) throw Exception("step size must be nonzero in colon expression");
    int scount = (int) floor((float)((maxval-minval)/stepsize)) + 1;
    if (scount<=0) 
      dim.reset();
    else {
      if (vert) {
	dim[0] = scount;
	dim[1] = 1;
      } else {
	dim[0] = 1;
	dim[1] = scount;
      }
      rp = (int32 *) allocateArray(FM_INT32,scount);
      for (int i=0;i<scount;i++)
	rp[i] = minval + i*stepsize;
    }
    return Array(FM_INT32,dim,rp);
  }

  Array Array::floatRangeConstructor(float minval, float stepsize, 
				     float maxval, bool vert) {
    Dimensions dim;
    float *rp = NULL;
    if (stepsize == 0) throw Exception("step size must be nonzero in colon expression");
    int scount = (int) floor((maxval-minval)/stepsize) + 1;
    if (scount<=0) 
      dim.reset();
    else {
      if (vert) {
	dim[0] = scount;
	dim[1] = 1;
      } else {
	dim[0] = 1;
	dim[1] = scount;
      }
      rp = (float *) allocateArray(FM_FLOAT,scount);
      for (int i=0;i<scount;i++)
	rp[i] = minval + i*stepsize;
    }
    return Array(FM_FLOAT,dim,rp);
  }

  Array Array::doubleRangeConstructor(double minval, double stepsize, 
				       double maxval, bool vert) {
    Dimensions dim;
    double *rp = NULL;
    if (stepsize == 0) throw Exception("step size must be nonzero in colon expression");
    int scount = (int) floor((maxval-minval)/stepsize) + 1;
    if (scount<=0) 
      dim.reset();
    else {
      if (vert) {
	dim[0] = scount;
	dim[1] = 1;
      } else {
	dim[0] = 1;
	dim[1] = scount;
      }
      rp = (double *) allocateArray(FM_DOUBLE,scount);
      for (int i=0;i<scount;i++)
	rp[i] = minval + i*stepsize;
    }
    return Array(FM_DOUBLE,dim,rp);
  }

  Array Array::matrixConstructor(ArrayMatrix& m) {
    Dimensions mat_dims;
    Dimensions row_dims;
    Class maxType, minType;
    Class retType;
    stringVector retNames;
    Dimensions retDims;
    void *dstPtr = NULL;
    bool sparseArg = false;

    try {
      maxType = FM_CELL_ARRAY;
      minType = FM_STRING;
      ArrayMatrix::iterator i = m.begin();
      bool firstNonzeroColumn = true;
      bool firstNonzeroRow = true;
      while (i != m.end()) {
	ArrayVector ptr = (ArrayVector) *i;
	for (int j=0;j<ptr.size();j++) {
	  const Array& d = ptr[j];
	  if (d.isSparse())
	    sparseArg = true;
	  if (!d.isEmpty()) {
	    if (maxType < d.dp->dataClass) maxType = d.dp->dataClass;
	    if (minType > d.dp->dataClass) minType = d.dp->dataClass;
	    if (firstNonzeroColumn) {
	      /**
	       * For the first element in the row, we copy
	       * the dimensions into the row buffer.
	       */
	      row_dims = d.dp->dimensions;
	      /**
	       * Because we are concatenating in columns,
	       * we must have at least 1 column in the data.
	       * The dimension array can be thought of as being
	       * infinitely extendable with 1's for all higher
	       * dimensions.  We will take advantage of this to
	       * force the elements to have at least one column.
	       */
	      firstNonzeroColumn = false;
	    } else {
	      /**
	       * Cycle through the elements of the row.
	       * Check these elements against the dimension
	       * buffer, and update the second dimension (column
	       * count).
	       */
	      if ((d.dp->dimensions.getLength() != row_dims.getLength()) && 
		  (d.dp->dimensions.getLength()>1))
		throw Exception("Number of dimensions must match for each element in a row definition");
	      if (d.dp->dimensions[0] != row_dims[0])
		throw Exception("Mismatch in first dimension for elements in row definition");
	      for (int k=2;k<row_dims.getLength();k++)
		if (d.dp->dimensions[k] != row_dims[k]) 
		  throw Exception("Mismatch in dimension for elements in row definition");
	      row_dims[1] += d.dp->dimensions[1];
	    }
	  }
	}
	/**
	 * For the first row, we copy the row dimensions into
	 * the column buffer.
	 */
	if (!firstNonzeroColumn) {
	  if (firstNonzeroRow) {
	    mat_dims = row_dims;
	    firstNonzeroRow = false;
	  } else {
	    if (mat_dims.getLength() != row_dims.getLength())	    
	      throw Exception("Number of dimensions must match for each row in a matrix definition");
	    for (int k=1;k<row_dims.getLength();k++)
	      if (row_dims[k] != mat_dims[k]) 
		throw Exception("Mismatch in dimension for rows in matrix definition");
	    mat_dims[0] += row_dims[0];
	  }
	  firstNonzeroColumn = true;
	}
	i++;
      }

      retType = maxType;
      retDims = mat_dims;

      /**
       * Check for the special case of a struct array - if the min_type 
       * is not also a struct array, then we have an error condition.
       * if min_type and max_type are the same, we have a special case.
       */
      if ((maxType == FM_STRUCT_ARRAY) && (minType != maxType))
	throw Exception("Cannot convert cell arrays to struct arrays");
  
      /**
       * If the final type is a struct array, we can get the field names from
       * any of the elements in the ArrayMatrix pointer argument.  
       */
      if (maxType == FM_STRUCT_ARRAY) {
	ArrayMatrix::iterator i = m.begin();
	ArrayVector ptr = *i;
	const Array& d = ptr.front();
	retNames = d.dp->fieldNames;
      }

      /**
       * Check for the sparse case - if any of the elements are sparse,
       * the output is sparse.
       */
      if (sparseArg) {
	if (retType < FM_INT32) retType = FM_INT32;
	return Array(retType,retDims,
		     SparseMatrixConstructor(retType,
					     retDims[0],
					     retDims[1],
					     m),
		     true);
      }

      /**
       * At this point, we now have the dimensions of the output
       * dataset, as well as the type of the output data.
       *
       * Next, we allocate space for the output array.
       */
      dstPtr = allocateArray(retType,retDims.getElementCount(),retNames);

      /**
       * Now, we have to copy the data from the source
       * vector to the destination vector.  Consider first
       * the matrix case.  We have a matrix
       * A = [B,C;D,E] - we start at (0,0), and copy in
       * B, one column at a time.  
       *
       * Start with a pointer to the origin.
       */

      /**
       * We only need to track the row and column displacement for each
       * block.
       */
      int row_corner = 0;
      int column_corner = 0;
      int dim_count(mat_dims.getLength());
      Dimensions aptr(dim_count);
      Dimensions bptr(dim_count);
      int el_size;
      int row_count;

      i = m.begin();
      while (i != m.end()) {
	ArrayVector ptr = *i;
	row_count = 0;
	for (int j=0;j<ptr.size();j++) {
	  Array d(ptr[j]);
	  if (!d.isEmpty()) {
	    /**
	     * Initialize the aptr to [0,...,0]
	     */
	    aptr.zeroOut();
	    /**
	     * Get the source pointer and a destination pointer
	     */
	    int dstIndex;
	    int srcIndex;
	    bptr = d.dp->dimensions;
	    row_count = d.dp->dimensions[0];
	    // Promote d to our ultimate type
	    d.promoteType(retType,retNames);
	    srcIndex = 0;
	    while(aptr.inside(bptr)) {
	      /**
	       * Convert aptr to an index into the destination array.
	       * We do so by using the mapping vector to map aptr
	       * to an index, and then adjust the index using the row
	       * and column corners.
	       */
	      dstIndex = mat_dims.mapPoint(aptr);
	      dstIndex += row_corner + column_corner*mat_dims[0];
	      /**
	       * Copy the column from src to dst
	       */
	      d.copyElements(srcIndex,dstPtr,dstIndex,row_count);
	      /**
	       * Advance the column index
	       */
	      aptr.incrementModulo(bptr,1);
	      /**
	       * Advance the source pointer
	       */
	      srcIndex += row_count;
	    }
	    /**
	     * Update the column corner pointer by the width of d.
	     */
	    column_corner += bptr[1];
	  }
	}
	/**
	 * Reset the column corner
	 */
	column_corner = 0;
	/**
	 * Advance the row corner
	 */
	row_corner += row_count;
	i++;
      }
      return Array(retType,retDims,dstPtr,false,retNames);
    } catch (Exception &e) {
      Free(dstPtr);
      throw e;
    }
  }

  Array Array::cellConstructor(ArrayMatrix& m) {
    int columnCount, rowCount;
    Array* qp = NULL;

    try {
      ArrayMatrix::iterator i = m.begin();
      while (i != m.end()) {
	ArrayVector ptr = *i;
	/**
	 * If this is the first row in the matrix def, then we 
	 * record its size in columnCount.
	 */
	if (i == m.begin()) 
	  columnCount = ptr.size();
	else {
	  /**
	   * Otherwise, make sure the column counts are all the same...
	   */
	  if (ptr.size() != columnCount)
	    throw Exception("Cell definition must have same number of elements in each row");
	}
	i++;
      }
      /**
       * At this point, we know how many columns our cell array has,
       * and the number of rows is also known (size of m).  So, set
       * up our dimensions, and allocate the output.
       */
      rowCount = m.size();
      Dimensions retDims(2);
      retDims[0] = rowCount;
      retDims[1] = columnCount;

      /**
       * Allocate storage space for the contents.
       */
      qp = (Array *) allocateArray(FM_CELL_ARRAY,retDims.getElementCount());
      Array *cp;
      Array *sp;
      /**
       * Loop through the rows.
       */
      sp = qp;
      i = m.begin();
      while (i != m.end()) {
	ArrayVector ptr = *i;
	cp = sp;
	for (int j=0;j<ptr.size();j++) {
	  *cp = ptr[j];
	  cp += rowCount;
	}
	i++;
	sp++;
      }
      return Array(FM_CELL_ARRAY,retDims,qp);
    } catch (Exception &e) {
      Free(qp);
      throw e;
    }
  }

  Array Array::structConstructor(stringVector fNames, ArrayVector& values)  {
    const Array* rptr;
    Dimensions dims;
    bool nonSingularFound;
    int i, j;
    Array *qp = NULL;
    
    try {
      if (fNames.size() != values.size())
	throw Exception("Number of field names must match number of values in structure constructor.");
      /**
       * First, we have to make sure that each entry of "values" have 
       *  1.  cell arrays of the same size,
       *  2.  single element cell arrays,
       *  3.  single values.
       */
      nonSingularFound = false;
      for (i=0;i<values.size();i++) {
	/**
	 * Check the type of the entry.  If its a non-cell array, then
	 * then ignore this entry.
	 */
	if (values[i].dp->dataClass == FM_CELL_ARRAY) {
	  /**
	   * This is a cell-array, so look for non-scalar cell-arrays.
	   */
	  if (!values[i].isScalar()) {
	    if (!nonSingularFound) {
	      nonSingularFound = true;
	      dims = values[i].dp->dimensions;
	    } else
	      if (!dims.equals(values[i].dp->dimensions))
		throw Exception("Array dimensions of non-scalar entries must agree in structure construction.");
	  }
	}
      }
    
      /**
       * At this point we can construct the dimensions of the output.
       */
      if (!nonSingularFound) {
	dims.reset();
	dims[0] = 1;
	dims[1] = 1;
      }
    
      /**
       * The dimensions of the object have been identified.  Set the
       * dimensions of the object and the field names.  Then allocate
       * the space.
       */
      qp = (Array*) allocateArray(FM_STRUCT_ARRAY,dims.getElementCount(),fNames);
      /**
       * Work through the values, and copy the values back one at a time.
       */
      int length = dims.getElementCount();
      int offset;
    
      offset = 0;
      for (j=0;j<length;j++)
	for (i=0;i<fNames.size();i++) {
	  Array rval = values[i];
	  if (rval.isSparse())
	    throw Exception("sparse arrays not supported for struct constructor.");
	  rptr = (const Array*) rval.dp->getData();
	  if (rval.dp->dataClass == FM_CELL_ARRAY) {
	    if (rval.isScalar())
	      qp[offset] = rptr[0];
	    else
	      qp[offset] = rptr[j];
	  } else 
	    qp[offset] = rval;
	  offset++;
	}
      return Array(FM_STRUCT_ARRAY,dims,qp,false,fNames);
    } catch (Exception &e) {
      Free(qp);
      throw;
    }
  }

  /********************************************************************************
   * Get functions                                                                *
   ********************************************************************************/

  /**
   * Take the current variable, and return a new array consisting of
   * the elements in source indexed by the index argument.  Indexing
   * is done using vector ordinals.
   */
  Array Array::getVectorSubset(Array& index)  {
    void *qp = NULL;
    try {
      if (index.isEmpty()) {
	return Array(dp->dataClass,index.dp->dimensions,
		     NULL,false,dp->fieldNames);
      }
      if (isEmpty()) 
	throw Exception("Cannot index into empty variable.");
      index.toOrdinalType();
      Dimensions retdims(index.dp->dimensions);
      retdims.simplify();
      if (isSparse()) {
	if (index.getLength() == 1) {
	  int indx;
	  indx = index.getContentsAsIntegerScalar()-1;
	  int row = indx % getDimensionLength(0);
	  int col = indx / getDimensionLength(0);
	  return Array(dp->dataClass,retdims,
		       GetSparseScalarElement(dp->dataClass,
					      getDimensionLength(0),
					      getDimensionLength(1),
					      dp->getData(),
					      row+1, col+1),
		       false);
	} else
	  return Array(dp->dataClass,retdims,
		       GetSparseVectorSubsets(dp->dataClass,
					      getDimensionLength(0),
					      getDimensionLength(1),
					      dp->getData(),
					      (const indexType*) 
					      index.dp->getData(),
					      index.getDimensionLength(0),
					      index.getDimensionLength(1)),
		     true);
      }
      //
      // The output is the same size as the _index_, not the
      // source variable (neat, huh?).  But it inherits the
      // type of the source variable.
      int length = index.getLength();
      qp = allocateArray(dp->dataClass,index.getLength(),dp->fieldNames);
      // Get a pointer to the index data set
      const indexType *index_p = (const indexType *) index.dp->getData();
      int bound = getLength();
      int ndx;
      for (int i=0;i<length;i++) {
	ndx = (int) index_p[i] - 1;
	if (ndx < 0 || ndx >= bound)
	  throw Exception("Index exceeds variable dimensions");
	copyElements(ndx,qp,i,1);
      }
      return Array(dp->dataClass,retdims,qp,dp->sparse,dp->fieldNames);
    } catch (Exception &e) {
      Free(qp);
      throw;
    }
  }

  /**
   * Take the current variable, and return a new array consisting of
   * the elements in source indexed by the index argument.  Indexing
   * is done using ndimensional indices.
   */
  Array Array::getNDimSubset(ArrayVector& index)  {
    constIndexPtr* indx = NULL;  
    void *qp = NULL;
    int i;

    if (isEmpty())
      throw Exception("Cannot index into empty variable.");
    try {
      int L = index.size();
      // Convert the indexing variables into an ordinal type.
      // We don't catch any exceptions - let them propogate up the
      // call chain.
      for (i=0;i<L;i++) {
	if (index[i].isEmpty()) return Array::emptyConstructor();
	index[i].toOrdinalType();
      }
      // Set up data pointers
      indx = (constIndexPtr *) Malloc(sizeof(constIndexPtr*)*L);
      // Calculate the size of the output.
      Dimensions outDims(L);
      for (i=0;i<L;i++) {
	outDims[i] = (index[i].getLength());
	indx[i] = (constIndexPtr) index[i].dp->getData();
      }
      if (isSparse()) {
	if (L > 2)
	  throw Exception("multidimensional indexing (more than 2 dimensions) not legal for sparse arrays");
	if ((outDims[0] == 1) && (outDims[1] == 1))
	  return Array(dp->dataClass,outDims,
		       GetSparseScalarElement(dp->dataClass,
					      getDimensionLength(0),
					      getDimensionLength(1),
					      dp->getData(),
					      *((const indexType*) indx[0]),
					      *((const indexType*) indx[1])),
		       false);
	else
	  return Array(dp->dataClass,outDims,
		       GetSparseNDimSubsets(dp->dataClass,
					    getDimensionLength(0),
					    getDimensionLength(1),
					    dp->getData(),
					    (const indexType*) indx[0],
					    outDims[0],
					    (const indexType*) indx[1],
					    outDims[1]),
		       true);
      }
      qp = allocateArray(dp->dataClass,outDims.getElementCount(),dp->fieldNames);
      Dimensions argPointer(L);
      Dimensions currentIndex(L);
      int srcindex;
      int dstindex;
      dstindex = 0;
      while (argPointer.inside(outDims)) {
	for (int i=0;i<L;i++)
	  currentIndex[i] = (int) indx[i][argPointer[i]] - 1;
	srcindex = dp->dimensions.mapPoint(currentIndex);
	copyElements(srcindex,qp,dstindex,1);
	dstindex++;
	argPointer.incrementModulo(outDims,0);
      }
      Free(indx);
      outDims.simplify();
      return Array(dp->dataClass,outDims,qp,dp->sparse,dp->fieldNames);
    } catch (Exception &e) {
      Free(indx);
      Free(qp);
      throw e;
    }
  }

  /**
   * Return the contents of a cell array - must be a scalar...
   */
  Array Array::getVectorContents(Array& indexing)  {
    if (dp->dataClass != FM_CELL_ARRAY)
      throw Exception("Attempt to apply contents-indexing to non-cell array object.");
    if (indexing.isEmpty())
      throw Exception("Empty contents indexing is not defined.");
    if (isSparse())
      throw Exception("getVectorContents not supported for sparse arrays.");
    indexing.toOrdinalType();
    //
    // The output is the same size as the _index_, not the
    // source variable (neat, huh?).  But it inherits the
    // type of the source variable.
      //
      // The index HAS to be a scalar for contents-based addressing
    if (indexing.getLength() != 1) 
      throw Exception("Content indexing must return a single value.");
    constIndexPtr index_p = (constIndexPtr) indexing.dp->getData();
    indexType ndx = *index_p - 1;
    int bound = getLength();
    if (ndx < 0 || ndx >= bound)
      throw Exception("Index exceeds cell array dimensions");
    const Array* srcPart = (const Array*) dp->getData();
    // Make a source of whatever is in that index, and return it.
    return srcPart[ndx];
  }

  /**
   * Return the contents of a cell array - indexed via a multi-dim index.
   */
  Array Array::getNDimContents(ArrayVector& indexing)  {
    if (dp->dataClass != FM_CELL_ARRAY)
      throw Exception("Attempt to apply contents-indexing to non-cell array object.");
    if (isSparse())
      throw Exception("getNDimContents not supported for sparse arrays.");
    int L = indexing.size();
    Dimensions outPos(L);
    
    // Convert each of the indexing variables into an ordinal type.
    // We don't catch any exceptions - let them propogate up the
    // call chain.
    // The index HAS to be a scalar for contents-based addressing.
    for (int i=0;i<L;i++) {
      indexing[i].toOrdinalType();
      if (indexing[i].getLength() != 1)
	throw Exception("Content indexing must return a single value.");
      constIndexPtr sp = (constIndexPtr) indexing[i].dp->getData();
      outPos[i] = *sp - 1;
    }
    int j;
    j = dp->dimensions.mapPoint(outPos);
    const Array* qp = (const Array*) dp->getData();
    return qp[j];
  }

  Array Array::getField(std::string fieldName)  {
    // First make sure that we are a scalar value.
    if (!isScalar())
      throw Exception("Cannot dereference a field of a multi-element structure array.");
    if (isSparse())
      throw Exception("getField not supported for sparse arrays.");
    // Then, find the field index.
    int ndx = getFieldIndex(fieldName);
    if (ndx < 0)
      throw Exception(std::string("Reference to non-existent field ") + 
		      fieldName);
    // Cast real part into a data pointer, and return a copy of what the caller asked for!
    const Array* qp = (const Array*) dp->getData();
    return qp[ndx];
  }

  Array Array::getDiagonal(int diagonalOrder)  {
    if (!is2D()) 
      throw Exception("Cannot take diagonal of N-dimensional array.");
    int rows = dp->dimensions.getRows();
    int cols = dp->dimensions.getColumns();
    int outLen;
    Dimensions outDims;
    int i;
    int srcIndex;
    if (diagonalOrder < 0) {
      outLen = (rows+diagonalOrder) < cols ? (rows+diagonalOrder) : cols;
      outLen = (outLen  < 0) ? 0 : outLen;
      if (outLen == 0)
	return Array::emptyConstructor();
      outDims[0] = outLen;
      outDims[1] = 1;
      void *qp;
      if (isSparse()) {
	qp = GetSparseDiagonal(dp->dataClass, rows, cols, dp->getData(), diagonalOrder);
	return Array(dp->dataClass,outDims,qp,true);
      } else {
	qp = allocateArray(dp->dataClass,outLen,dp->fieldNames);
	for (i=0;i<outLen;i++) {
	  srcIndex = -diagonalOrder + i*(rows+1);
	  copyElements(srcIndex,qp,i,1);
	}
	return Array(dp->dataClass,outDims,qp,dp->sparse,dp->fieldNames);
      }
    } else {
      outLen = rows < (cols-diagonalOrder) ? rows : (cols-diagonalOrder);
      outLen = (outLen  < 0) ? 0 : outLen;
      if (outLen == 0)
	return Array::emptyConstructor();
      outDims[0] = outLen;
      outDims[1] = 1;
      void *qp;
      if (isSparse()) {
	qp = GetSparseDiagonal(dp->dataClass, rows, cols, dp->getData(), diagonalOrder);
	return Array(dp->dataClass,outDims,qp,true);
      } else {
	qp = allocateArray(dp->dataClass,outLen,dp->fieldNames);
	for (i=0;i<outLen;i++) {
	  srcIndex = diagonalOrder*rows + i*(rows+1);
	  copyElements(srcIndex,qp,i,1);
	}
      }
      return Array(dp->dataClass,outDims,qp,dp->sparse,dp->fieldNames);
    }
  }

  ArrayVector Array::getFieldAsList(std::string fieldName)  {
    if (dp->dataClass != FM_STRUCT_ARRAY)
      throw Exception("Attempt to apply field-indexing to non structure-array object.");
    if (isSparse())
      throw Exception("getFieldAsList not supported for sparse arrays.");
    ArrayVector m;
    const Array *qp = (const Array*) dp->getData();
    int N = getLength();
    int fieldCount = dp->fieldNames.size();
    int ndx = getFieldIndex(fieldName);
    if (ndx < 0)
      throw Exception(std::string("Reference to non-existent field ") + fieldName);
    int i;
    for (i=0;i<N;i++)
      m.push_back(qp[i*fieldCount+ndx]);
    return m;
  }

  /**
   * Return a subset of a cell array as a list.
   */
  ArrayVector Array::getVectorContentsAsList(Array& index)  {
    ArrayVector m;
    if (dp->dataClass != FM_CELL_ARRAY)
      throw Exception("Attempt to apply contents-indexing to non cell-array object.");
    if (isSparse())
      throw Exception("getVectorContentsAsList not supported for sparse arrays.");
    if (index.isEmpty()) return ArrayVector();
    index.toOrdinalType();
    // Get the maximum index
    indexType max_index = index.getMaxAsIndex();
    // Get our length
    int bound = getLength();
    if (max_index > bound)  throw Exception("Array index exceeds bounds of cell-array");
    // Get the length of the index object
    int index_length = index.getLength();
    // Get a pointer to the index data set
    constIndexPtr index_p = (constIndexPtr) index.dp->getData();
    // Get a pointer to our data
    const Array* qp = (const Array*) dp->getData();
    // Now we copy data from dp to m
    for (int i=0;i<index_length;i++)
      m.push_back(qp[index_p[i]-1]);
    return m;
  }

  /**
   * Return the contents of an cell array as a list.
   */
  ArrayVector Array::getNDimContentsAsList(ArrayVector& index)  {
    if (dp->dataClass != FM_CELL_ARRAY)
      throw Exception("Attempt to apply contents-indexing to non cell-array object.");
    if (isSparse())
      throw Exception("getNDimContentsAsList not supported for sparse arrays.");
    // Store the return value here
    ArrayVector m;
    // Get the number of indexing dimensions
    int L = index.size();
    // Setup the data pointers
    Dimensions outDims(L);
    int i;
    for (i=0;i<L;i++)
      index[i].toOrdinalType();
    constIndexPtr* indx = (constIndexPtr *) Malloc(sizeof(constIndexPtr*)*L);
    for (i=0;i<L;i++) {
      outDims[i] = (index[i].getLength());
      indx[i] = (constIndexPtr) index[i].dp->getData();
    }
    Dimensions argPointer(L);
    Dimensions currentIndex(L);
    const Array *qp = (const Array*) dp->getData();
    int srcindex;
    while (argPointer.inside(outDims)) {
      for (int i=0;i<L;i++)
	currentIndex[i] = (int) indx[i][argPointer[i]] - 1;
      srcindex = dp->dimensions.mapPoint(currentIndex);
      m.push_back(qp[srcindex]);
      argPointer.incrementModulo(outDims,0);
    }
    Free(indx);
    return m;
  }

  /********************************************************************************
   * Set functions                                                                *
   ********************************************************************************/

  /**
   * 
   * This is the vector version of the multidimensional replacement function.
   *
   * This requires the following steps:
   *  1. Compute the maximum along each dimension
   *  2. Check that data is either scalar or the right size.
   */
  void Array::setVectorSubset(Array& index, Array& data) {
    if (index.isEmpty())
      return;
    // Check the right-hand-side - if it is empty, then
    // we have a delete command in disguise.
    if (data.isEmpty()) {
      deleteVectorSubset(index);
      return;
    }
    // Make sure the index is an ordinal type
    index.toOrdinalType();
    int index_length = index.getLength();
    if (index_length == 0) return;
    // Get a pointer to the index data set
    constIndexPtr index_p = (constIndexPtr) index.dp->getData();
    int advance;
    // Set the right hand side advance pointer to 
    //  - 0 if the rhs is a scalar
    //  - 1 else
    if (data.isSparse())
      data.makeDense();
    if (data.isScalar())
      advance = 0;
    else if (data.getLength() == index_length)
      advance = 1;
    else
      throw Exception("Size mismatch in assignment A(I) = B.\n");
    // Compute the maximum index
    indexType max_index = index.getMaxAsIndex();
    // If the RHS type is superior to ours, we 
    // force our type to agree with the inserted data.
    // Also, if we are empty, we promote ourselves (regardless of
    // our type).
    if (!isEmpty() &&
	(data.getDataClass() == FM_STRUCT_ARRAY) &&
	(getDataClass() == FM_STRUCT_ARRAY)) {
      if (data.dp->fieldNames.size() > dp->fieldNames.size())
	promoteType(FM_STRUCT_ARRAY,data.dp->fieldNames);
      else
	data.promoteType(FM_STRUCT_ARRAY,dp->fieldNames);
    } else {
      if (isEmpty() || data.getDataClass() > getDataClass())
	promoteType(data.getDataClass(),data.dp->fieldNames);
      // If our type is superior to the RHS, we convert
      // the RHS to our type
      else if (data.getDataClass() <= dp->dataClass)
      data.promoteType(dp->dataClass,dp->fieldNames);
    }
    if (isSparse()) {
      int rows = getDimensionLength(0);
      int cols = getDimensionLength(1);
      void *qp = SetSparseVectorSubsets(dp->dataClass,rows,cols,dp->getData(),
					(const indexType*) index.dp->getData(),
					index.getDimensionLength(0),
					index.getDimensionLength(1),
					data.getDataPointer(),
					advance);
      Dimensions newdim;
      newdim[0] = rows;
      newdim[1] = cols;
      dp = dp->putData(dp->dataClass,newdim,qp,true);
      return;
    }
    // If the max index is larger than our current length, then
    // we have to resize ourselves - but this is only legal if we are
    // a vector.
    vectorResize(max_index);
    // Get a writable data pointer
    void *qp = getReadWriteDataPointer();
    // Now, we copy data from the RHS to our real part,
    // computing indices along the way.
    indexType srcIndex = 0;
    indexType j;
    for (int i=0;i<index_length;i++) {
      j = index_p[i] - 1;
      data.copyElements(srcIndex,qp,j,1);
      srcIndex += advance;
    }
  }

  /**
   * Take the contents of data, and insert this data.
   *
   * This requires the following steps:
   *  1. Compute the maximum along each dimension
   *  2. Compute the dimensions of the right hand side
   *  3. Check that data is either a scalar or the right size
   *  4. If necessary, zero-extend the variable.
   *  5. Copy in the result.
   *
   * This is true for integer arguments - not for logical ones.
   * Logical indices need to be converted into integer lists
   * before they can be used.
   */
  void Array::setNDimSubset(ArrayVector& index, Array& data) {
    constIndexPtr* indx = NULL;
    // If the RHS is empty, then we really want to do a delete...
    if (data.isEmpty()) {
      deleteNDimSubset(index);
      return;
    }
    try {
      int L = index.size();
      int i;
      // Convert the indexing variables into an ordinal type.
      for (i=0;i<L;i++){
	if (index[i].isEmpty()) return;
	index[i].toOrdinalType();
      }
      // Check to see if any of the index variables are empty - 
      bool anyEmpty = false;
      for (i=0;i<L;i++)
	anyEmpty = anyEmpty | (index[i].isEmpty());
      // If any of the dimensions are empty, this entire method
      // is a NOP.  The reason we don't just return is because
      // of clean up.
      if (anyEmpty) return;
      // Set up data pointers
      indx = (constIndexPtr *) Malloc(sizeof(constIndexPtr)*L);
      Dimensions a(L);
      // First, we compute the maximum along each dimension.
      for (i=0;i<L;i++) {
	a[i] = index[i].getMaxAsIndex();
	indx[i] = (constIndexPtr) index[i].dp->getData();
      }
      // Next, we compute the number of entries in each component.
      Dimensions argLengths(L);
      Dimensions argPointer(L);
      int dataCount = 1;
      for (i=0;i<L;i++) {
	argLengths[i] = index[i].getLength();
	dataCount *= argLengths[i];
      }
      // Next, we compute the dimensions of the right hand side
      indexType advance;
      if (data.isSparse())
	data.makeDense();
      if (data.isScalar()) {
	advance = 0;
      } else if (!isEmpty() && (data.getLength() == dataCount)) {
	advance = 1;
      } else if (!isEmpty())
	throw Exception("Size mismatch in assignment A(I1,I2,...,In) = B.\n");
      else
	advance = 1;

      // If the RHS type is superior to ours, we 
      // force our type to agree with the inserted data.
      if (!isEmpty() &&
	  (data.getDataClass() == FM_STRUCT_ARRAY) &&
	  (getDataClass() == FM_STRUCT_ARRAY)) {
	if (data.dp->fieldNames.size() > dp->fieldNames.size())
	  promoteType(FM_STRUCT_ARRAY,data.dp->fieldNames);
	else
	  data.promoteType(FM_STRUCT_ARRAY,dp->fieldNames);
      } else {
	if (isEmpty() || data.getDataClass() > getDataClass())
	  promoteType(data.dp->dataClass,data.dp->fieldNames);
	// If our type is superior to the RHS, we convert
	// the RHS to our type
	else if (data.dp->dataClass <= dp->dataClass)
	  data.promoteType(dp->dataClass,dp->fieldNames);
      }
      if (isSparse()) {
	if (L > 2)
	  throw Exception("multidimensional indexing (more than 2 dimensions) not legal for sparse arrays in assignment A(I1,I2,...,IN) = B");
	int rows = getDimensionLength(0);
	int cols = getDimensionLength(1);
	void *qp = SetSparseNDimSubsets(dp->dataClass, rows, cols, 
					dp->getData(), 
					(const indexType*) indx[0], 
					argLengths[0],
					(const indexType*) indx[1], 
					argLengths[1],
					data.getDataPointer(),advance);
	Dimensions newdim;
	newdim[0] = rows;
	newdim[1] = cols;
	dp = dp->putData(dp->dataClass,newdim,qp,true);
	return;
      }
      // Now, resize us to fit this data
      resize(a);
      // Get a writable data pointer
      void *qp = getReadWriteDataPointer();
      // Now, we copy data from dp to our real part,
      // computing indices along the way.
      Dimensions currentIndex(dp->dimensions.getLength());
      indexType srcIndex = 0;
      indexType j;
      while (argPointer.inside(argLengths)) {
	for (int i=0;i<L;i++) 
	  currentIndex[i] = (indexType) indx[i][argPointer[i]] - 1;
	j = dp->dimensions.mapPoint(currentIndex);
	data.copyElements(srcIndex,qp,j,1);
	srcIndex += advance;
	argPointer.incrementModulo(argLengths,0);
      }
      Free(indx);
      dp->dimensions.simplify();
    } catch (Exception &e) {
      Free(indx);
      throw e;
    }
  }

  /**
   * This is the vector version of the multidimensional cell-replacement function.
   * This is for content-based indexing (curly brackets).  Two points that make
   * this function different than replaceData are
   *   1. If the index is larger than the size, we resize to a vector of sufficient
   *      length.
   *   2. Deletions do not occur.
   */
  void Array::setVectorContents(Array& index, Array& data) {
    promoteType(FM_CELL_ARRAY,data.dp->fieldNames);
    if (isSparse())
      throw Exception("setVectorContents not supported for sparse arrays.");
    index.toOrdinalType();
    if (index.getLength() == 0)
      return;
    if (index.getLength() != 1)
      throw Exception("In expression A{I} = B, I must reference a single element of cell-array A.");
    constIndexPtr index_p = (constIndexPtr) index.dp->getData();
    indexType ndx = *index_p - 1;
    int bound = getLength();
    if (ndx < 0)
      throw Exception("Illegal negative index in expression A{I} = B.");
    vectorResize(ndx+1);
    Array *qp = (Array*) getReadWriteDataPointer();
    qp[ndx] = data;
    dp->dimensions.simplify();
  }

  /**
   * This is the multidimensional cell-replacement function.
   * This is for content-based indexing (curly brackets).
   */
  void Array::setNDimContents(ArrayVector& index, Array& data) {
    promoteType(FM_CELL_ARRAY,data.dp->fieldNames);
    if (isSparse())
      throw Exception("setNDimContents not supported for sparse arrays.");
    int L = index.size();
    Dimensions outPos(L);
    int i;
    for (i=0;i<L;i++) {
      index[i].toOrdinalType();
      if (!index[i].isScalar()) 
	throw Exception("In expression A{I1,I2,...,IN} = B, (I1,...,IN) must reference a single element of cell-array A.");
      constIndexPtr sp = (constIndexPtr) index[i].dp->getData();
      outPos[i] = *sp;
    }
    resize(outPos);
    for (i=0;i<L;i++)
      outPos[i] = outPos[i] - 1;
    int j;
    j = dp->dimensions.mapPoint(outPos);
    Array *qp = (Array*) getReadWriteDataPointer();
    qp[j] = data;
    dp->dimensions.simplify();
  }

  /**
   * Set the contents of a field in a structure.
   */
  void Array::setField(std::string fieldName, Array& data)  {
    if (isEmpty()) {
      stringVector newNames(dp->fieldNames);
      newNames.push_back(fieldName);
      promoteType(FM_STRUCT_ARRAY,newNames);
      Dimensions a;
      a[0] = 1;
      a[1] = 1;
      resize(a);
    }
    if (isSparse())
      throw Exception("setField not supported for sparse arrays.");
    if (dp->dataClass != FM_STRUCT_ARRAY)
      throw Exception("Cannot apply A.field_name = B to non struct-array object A.");
    if (!isScalar())
      throw Exception("Cannot apply A.field_name = B to multi-element structure array A.");
    int field_ndx = getFieldIndex(fieldName);
    if (field_ndx == -1) 
      field_ndx = insertFieldName(fieldName);
    Array *sp = (Array*) getReadWriteDataPointer();
    sp[field_ndx] = data;
    dp->dimensions.simplify();
  }

  /**
   * This is the vector version of the multidimensional cell-replacement function.
   * This is for content-based indexing (curly brackets).  Two points that make
   * this function different than replaceData are
   *   1. If the index is larger than the size, we resize to a vector of sufficient
   *      length.
   *   2. Deletions do not occur.
   */
  void Array::setVectorContentsAsList(Array& index, ArrayVector& data) {
    if (isSparse())
      throw Exception("setVectorContentsAsList not supported for sparse arrays.");
    promoteType(FM_CELL_ARRAY);
    index.toOrdinalType();
    if (data.size() < index.getLength())
      throw Exception("Not enough right hand side values to satisy left hand side expression.");
    // Get the maximum index
    indexType max_index = index.getMaxAsIndex();
    // Resize us as necessary.
    vectorResize(max_index);
    // Get a pointer to the dataset
    Array *qp = (Array*) getReadWriteDataPointer();
    // Get a pointer to the index data set
    constIndexPtr index_p = (constIndexPtr) index.dp->getData();
    // Get the length of the index object
    int index_length = index.getLength();
    // Copy in the data
    for (int i=0;i<index_length;i++) {
      int ndx = index_p[i]-1;
      qp[ndx] = data.front();
      //      data.pop_front();
      data.erase(data.begin());
    }
    dp->dimensions.simplify();
  }

  /**
   * This is the multidimensional cell-replacement function.
   * This is for content-based indexing (curly brackets).
   */
  void Array::setNDimContentsAsList(ArrayVector& index, ArrayVector& data) {
    if (isSparse())
      throw Exception("setNDimContentsAsList not supported for sparse arrays.");
    promoteType(FM_CELL_ARRAY);
    int L = index.size();
    // Convert the indexing variables into an ordinal type.
    int i;
    for (i=0;i<L;i++)
      index[i].toOrdinalType();
    
    // Set up data pointers
    constIndexPtr* indx = (constIndexPtr *) Malloc(sizeof(constIndexPtr)*L);
    try {
      Dimensions a(L);
      // First, we compute the maximum along each dimension.
      // We also get pointers to each of the index pointers.
      for (i=0;i<L;i++) {
	a[i] = index[i].getMaxAsIndex();
	indx[i] = (constIndexPtr) index[i].dp->getData();
      }
      // Next, we compute the number of entries in each component.
      Dimensions argLengths(L);
      Dimensions argPointer(L);
      int dataCount = 1;
      for (i=0;i<L;i++) {
	argLengths[i] = index[i].getLength();
	dataCount *= argLengths[i];
      }
      if (data.size() < dataCount)
	throw Exception("Not enough right hand side values to satisfy left hand side expression");
      // Resize us as necessary
      resize(a);
      // Get a writable data pointer
      Array *qp = (Array*) getReadWriteDataPointer();
      // Now, we copy data from dp to our real part,
      // computing indices along the way.
      Dimensions currentIndex(dp->dimensions.getLength());
      indexType j;
      while (argPointer.inside(argLengths)) {
	for (i=0;i<L;i++) 
	  currentIndex[i] = (indexType) indx[i][argPointer[i]] - 1;
	j = dp->dimensions.mapPoint(currentIndex);
	qp[j] = data.front();
	//	data.pop_front();
	data.erase(data.begin());
	argPointer.incrementModulo(argLengths,0);
      }
      Free(indx);
      indx = NULL;
      dp->dimensions.simplify();
    } catch (Exception &e) {
      Free(indx);
      throw e;
    }
  }

  /**
   * Set the contents of a field in a structure.
   */
  void Array::setFieldAsList(std::string fieldName, ArrayVector& data)  {
    if (isSparse())
      throw Exception("setFieldAsList not supported for sparse arrays.");
    Array *rp = NULL;
    if (isEmpty()) {
      stringVector names(dp->fieldNames);
      names.push_back(fieldName);
      promoteType(FM_STRUCT_ARRAY,names);
      Dimensions a;
      a[0] = 1;
      a[1] = 1;
      resize(a);
//       dp = dp->putData(FM_STRUCT_ARRAY,dp->getDimensions(),NULL,names);
//       return;
    }
    if (dp->dataClass != FM_STRUCT_ARRAY)
      throw Exception("Cannot apply A.field_name = B to non struct-array object A.");
    if (data.size() < getLength())
      throw Exception("Not enough right hand values to satisfy left hand side expression.");
    int indexLength = getLength();
    int field_ndx = getFieldIndex(fieldName);
    if (field_ndx == -1)
      field_ndx = insertFieldName(fieldName);
    int fieldCount = dp->fieldNames.size();
    Array *qp = (Array*) getReadWriteDataPointer();
    for (int i=0;i<indexLength;i++) {
      qp[i*fieldCount+field_ndx] = data.front();
      //      data.pop_front();
      data.erase(data.begin());
    }
    dp->dimensions.simplify();
  }

  /**
   * Add another fieldname to our structure array.
   */
  int Array::insertFieldName(std::string fieldName) {
    if (isSparse())
      throw Exception("insertFieldName not supported for sparse arrays.");
    stringVector names(dp->fieldNames);
    names.push_back(fieldName);
    const Array* qp = (const Array*) dp->getData();
    Array *rp = (Array*) allocateArray(dp->dataClass,getLength(),names);
    int fN = names.size();
    for (int i=0;i<fN-1;i++)
      rp[i] = qp[i];
    dp = dp->putData(FM_STRUCT_ARRAY,dp->dimensions,rp,false,names);
    return (fN-1);
  }

  /********************************************************************************
   * Delete functions                                                             *
   ********************************************************************************/

  /**
   * Delete a vector subset of a variable.
   */
  void Array::deleteVectorSubset(Array& arg) {
    void *qp = NULL;
    bool *deletionMap = NULL;
    try {
      // First convert arg to an ordinal type.
      arg.toOrdinalType();
      if (isSparse()) {
	int rows = getDimensionLength(0);
	int cols = getDimensionLength(1);
	void *cp = DeleteSparseMatrixVectorSubset(dp->dataClass,rows,cols,
						  dp->getData(),
						  (const indexType *)
						  arg.getDataPointer(),
						  arg.getLength());
	Dimensions newdim;
	newdim[0] = rows;
	newdim[1] = cols;
	dp = dp->putData(dp->dataClass,newdim,cp,true);
	return;
      }
      // Next, build a deletion map.
      int N = getLength();
      int i;
      deletionMap = arg.getBinaryMap(N);
      // Now, we count up the number of elements that remain after deletion.
      int newSize = 0;
      for (i=0;i<N;i++) 
	if (!deletionMap[i]) newSize++;
      // Allocate a new space to hold the data.
      qp = allocateArray(dp->dataClass,newSize,dp->fieldNames);
      // Loop through the indices - copy elements in that 
      // have not been deleted.
      int dstIndex = 0;
      for (i=0;i<N;i++)
	if (!deletionMap[i])
	  copyElements(i,qp,dstIndex++,1);
      Free(deletionMap);
      deletionMap = NULL;
      Dimensions newDim;
      if (dp->dimensions.isScalar()) {
	newDim.reset();
	newDim[0] = 1;
	newDim[1] = newSize;
      } else if (dp->dimensions.isVector()) {
	newDim = dp->dimensions;
	if (dp->dimensions[0] != 1)
	  newDim[0] = newSize;
	else
	  newDim[1] = newSize;
      } else {
	newDim.reset();
	newDim[0] = 1;
	newDim[1] = newSize;
      }
      dp = dp->putData(dp->dataClass,newDim,qp,dp->sparse,dp->fieldNames);
    } catch (Exception &e) {
      Free(qp);
      Free(deletionMap);
      throw e;
    }
  }

  void Array::makeSparse() {
    if (!is2D())
      throw Exception("Cannot make n-dimensional arrays sparse.");
    if (isEmpty())
      return;
    if (isReferenceType() || isString())
      throw Exception("Cannot make strings or reference types sparse.");
    if (isSparse()) return;
    if (dp->dataClass < FM_INT32)
      promoteType(FM_INT32);
    ensureSingleOwner();
    dp = dp->putData(dp->dataClass,dp->dimensions,
		     makeSparseArray(dp->dataClass,
				     dp->dimensions[0],
				     dp->dimensions[1],
				     dp->getData()),
		     true,
		     dp->fieldNames);
  }

  int Array::getNonzeros() const {
    if (!isSparse())
      return (dp->dimensions.getElementCount());
    if (isEmpty())
      return 0;
    return CountNonzeros(dp->dataClass,dp->dimensions[0],dp->dimensions[1],
			 dp->getData());
  }

  void Array::makeDense() {
    if (!isSparse())
      return;
    if (isEmpty()) {
      dp->sparse = false;
      return;
    }
    ensureSingleOwner();
    dp = dp->putData(dp->dataClass,dp->dimensions,
		     makeDenseArray(dp->dataClass,
				    dp->dimensions[0],
				    dp->dimensions[1],
				    dp->getData()),
		     false,
		     dp->fieldNames);
  }

  /**
   * Delete a subset of a variable.  
   */
  void Array::deleteNDimSubset(ArrayVector& args)  {
    int singletonReferences = 0;
    int singletonDimension = 0;
    int i;
    Array qp;
    bool *indxCovered = NULL;
    bool *deletionMap = NULL;
    void *cp = NULL;
    try {
      // Our strategy is as follows.  To make the deletion, we need
      // one piece of information: the dimension to delete.
      // To do so, we first make a pass through the set of arguments,
      // checking each one to see if it "covers" its index set.
      //
      // However, to simplify the testing of
      // conditions later on, we must make sure that the length of
      // the index list matches our number of dimensions.  We extend
      // it using 1 references, and throw an exception if there are
      // more indices than our dimension set.
      for (i=0;i<args.size();i++)
	args[i].toOrdinalType();
      // First, add enough "1" singleton references to pad the
      // index set out to the size of our variable.
      if (args.size() < dp->dimensions.getLength())
	for (i = args.size();i<dp->dimensions.getLength();i++)
	  args.push_back(Array::uint32Constructor(1));
      // Now cycle through indices one at a time.  Count
      // the number of non-covering indices.  Also track the
      // location of the last-occurring non-covering index.
      for (i=0;i<args.size();i++) {
	qp = args[i];
	// Get a binary representation of each index over the range [0,dimensions[i]-1]
	indxCovered = qp.getBinaryMap(dp->dimensions[i]);
	// Scan the array, and make sure all elements are true.  If not,
	// then this is the "singleton" dimension.  Kick the singleton
	// reference counter, and record the current dimension.
	bool allCovered = true; 
	for (int k=0;allCovered && (k<dp->dimensions[i]);k++)
	  allCovered = allCovered && indxCovered[k];
	Free(indxCovered);
	indxCovered = NULL;
	if (!allCovered) {
	  singletonReferences++;
	  singletonDimension = i;
	}
      }
      // Now, we check the number of singleton references we
      // encountered.  There are three cases to check:
      //  Case 1. No singleton references.  This is OK - it
      //	      means we wish to delete the entire variable.
      //	      We set a flag, and proceed to validate the covering
      //	      of each dimension by its corresponding index set.
      //  Case 2. One singleton reference.  This is OK - it
      //	      means we wish to delete a single plane of
      //	      data.  Retrieve the index of the plane, and store
      //	      it in singletonIndex.
      //  Case 3. Two or more singleton references.  Can't do it - 
      //	      throw an error.
      if (singletonReferences > 1) 
	throw Exception("Statement A(...) = [] can only contain one non-colon index.\n");
      if (singletonReferences == 0)
	singletonDimension = -1;
      // If we got this far, the user either entered an expression like
      // A(:,:,...,:,s,:,...,:) = [], or something numerically equivalent,
      // or the user entered something like A(:,...,:) = [].
      // In the latter case (indicated by singletonReferences = 0), we simply
      // delete the entire variable, and make it an empty type.
      // In the former case, we will have more work to do...
      if (singletonReferences != 0) {
	// We have to rescan our (now-identified) singleton
	// dimension to build a deletion map.  The map is
	// marked true for each plane we wish to delete.
	// The map is the size of the _data_'s dimension.
	int M = dp->dimensions[singletonDimension];
	deletionMap = args[singletonDimension].getBinaryMap(M);
	// We can now calculate the new size of the variable in the singletonDimension
	// by counting the number of "false" entries in deletionMap.
	int newSize = 0;
	for (i=0;i<M;i++)
	  if (!deletionMap[i]) newSize++;
	int rowCount = dp->dimensions[0];
	Dimensions retDims;
	// Copy our current dimensions to the output dimensions.
	retDims = dp->dimensions;
	// Update the singleton dimension to the new size.
	retDims[singletonDimension] = newSize;
	// For sparse matrices, we branch here to call the sparse matrix deletion code
	if (isSparse()) {
	  int rows = getDimensionLength(0);
	  int cols = getDimensionLength(1);
	  if (singletonDimension == 0)
	    dp = dp->putData(dp->dataClass,retDims,
			     DeleteSparseMatrixRows(dp->dataClass,rows,cols,
						    dp->getData(),deletionMap),true);
	  else if (singletonDimension == 1)
	    dp = dp->putData(dp->dataClass,retDims,
			     DeleteSparseMatrixCols(dp->dataClass,rows,cols,
						    dp->getData(),deletionMap),true);
	  else
	    throw Exception("sparse matrices do not support deleting n-dimensional planes - they are only 2-D");
	  Free(deletionMap);
	  Free(indxCovered);
	  return;
	}
	// Allocate space for the return objects data
	cp = allocateArray(dp->dataClass,retDims.getElementCount(),dp->fieldNames);
	// Track our offset into the original data & our offset into
	// the truncated data.
	int srcIndex = 0;
	int dstIndex = 0;
	// Inintialize an ND pointer to the first element in the
	// current data structure.
	int L = dp->dimensions.getLength();
	Dimensions curPos(L);
	// Loop until we have exhausted the original data.
	while (curPos.inside(dp->dimensions)) {
	  // Check to see if this column is to be skipped
	  if (!deletionMap[curPos[singletonDimension]]) {
	    // Copy the data from our original data structure to the
	    // new data structure, starting from srcIndex, and
	    // copying to dstIndex.
	    copyElements(srcIndex,cp,dstIndex,1);
	    // Advance the destination pointer. - we only do this on a copy
	    dstIndex ++;
	  }
	  // Advance the source pointer - we always do this
	  srcIndex ++;
	  curPos.incrementModulo(dp->dimensions,0);
	}
	Free(deletionMap);
	retDims.simplify();
	dp = dp->putData(dp->dataClass,retDims,cp,dp->sparse,dp->fieldNames);
      } else {
	Dimensions newDims;
	dp = dp->putData(dp->dataClass,newDims,NULL,false,dp->fieldNames);
      }
    } catch (Exception &e) {
      Free(deletionMap);
      Free(cp);
      Free(indxCovered);
      throw e;
    }
  }

  /********************************************************************************
   * Display functions                                                            *
   ********************************************************************************/

  /**
   * Print this object when it is an element of a cell array.  This is
   * generally a shorthand summary of the description of the object.
   */
  void Array::summarizeCellEntry() const {
    if (isEmpty()) 
      io->outputMessage("[]");
    else {
      switch(dp->dataClass) {
      case FM_CELL_ARRAY:
	io->outputMessage("{");
	dp->dimensions.printMe(io);
	io->outputMessage(" cell }");
	break;
      case FM_STRUCT_ARRAY:
	io->outputMessage(" ");
	dp->dimensions.printMe(io);
	io->outputMessage(" struct array");
	break;
      case FM_STRING:
	{
	  const char *ap =(const char*) dp->getData();
	  if (dp->dimensions.getRows() == 1) {
	    int columns(dp->dimensions.getColumns());
		memcpy(msgBuffer,ap,columns);
		msgBuffer[columns] = 0;
	    io->outputMessage(msgBuffer);
	  } else {
	    io->outputMessage("[");
	    dp->dimensions.printMe(io);
	    io->outputMessage(" string ]");
	  }
	}
	break;
      case FM_LOGICAL:
	if (dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%d]",*((const logical*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  io->outputMessage(" logical]");	  
	}
	break;
      case FM_UINT8:
	if (dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%d]",*((const uint8*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  io->outputMessage(" uint8]");
	}
	break;
      case FM_INT8:
	if (dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%d]",*((const int8*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  io->outputMessage(" int8]");
	}
	break;
      case FM_UINT16:
	if (dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%d]",*((const uint16*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  io->outputMessage(" uint16]");
	}
	break;
      case FM_INT16:
	if (dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%d]",*((const int16*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  io->outputMessage(" int16]");
	}
	break;
      case FM_UINT32:
	if (dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%d]",*((const uint32*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  io->outputMessage(" uint32]");
	}
	break;
      case FM_INT32:
	if (!isSparse() && dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%d]",*((const int32*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  if (isSparse())
	    io->outputMessage(" sparse");
	  io->outputMessage(" int32]");
	}
	break;
      case FM_DOUBLE:
	if (!isSparse() && dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%lf]",*((const double*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  if (isSparse())
	    io->outputMessage(" sparse");
	  io->outputMessage(" double]");
	}
	break;
      case FM_DCOMPLEX:
	if (!isSparse() && dp->dimensions.isScalar()) {
	  const double *ap = (const double*) dp->getData();
	  snprintf(msgBuffer,MSGBUFLEN,"[%lf+%lfi]",ap[0],ap[1]);
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  if (isSparse())
	    io->outputMessage(" sparse");
	  io->outputMessage(" dcomplex]");
	}
	break;
      case FM_FLOAT:
	if (!isSparse() && dp->dimensions.isScalar()) {
	  snprintf(msgBuffer,MSGBUFLEN,"[%f]",*((const float*) dp->getData()));
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  if (isSparse())
	    io->outputMessage(" sparse");
	  io->outputMessage(" float]");
	}
	break;
      case FM_COMPLEX:
	if (!isSparse() && dp->dimensions.isScalar()) {
	  const float *ap = (const float*) dp->getData();
	  snprintf(msgBuffer,MSGBUFLEN,"[%f+%fi]",ap[0],ap[1]);
	  io->outputMessage(msgBuffer);
	} else {
	  io->outputMessage("[");
	  dp->dimensions.printMe(io);
	  if (isSparse())
	    io->outputMessage(" sparse");
	  io->outputMessage(" complex]");
	}
	break;
      }
    }
  }

  void emitElement(char *msgBuffer, const void *dp, int num, Class dcls) {
    switch (dcls) {
    case FM_INT8: {
      const int8 *ap;
      ap = (const int8*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"% 4d",ap[num]);
      io->outputMessage(msgBuffer);
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_UINT8: {
      const uint8 *ap;
      ap = (const uint8*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%3u",ap[num]);
      io->outputMessage(msgBuffer);
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_INT16: {
      const int16 *ap;
      ap = (const int16*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"% 6d",ap[num]);
      io->outputMessage(msgBuffer);
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_UINT16: {
      const uint16 *ap;
      ap = (const uint16*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%5u",ap[num]);
      io->outputMessage(msgBuffer);
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_INT32: {
      const int32 *ap;
      ap = (const int32*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%13d",ap[num]);
      io->outputMessage(msgBuffer);
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_UINT32: {
      const uint32 *ap;
      ap = (const uint32*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%12u",ap[num]);
      io->outputMessage(msgBuffer);
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_LOGICAL: {
      const logical *ap;
      ap = (const logical*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%d  ",ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_STRING: {
      const char *ap;
      ap = (const char*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%c\0",ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_FLOAT: {
      const float *ap;
      ap = (const float*) dp;
      outputSinglePrecisionFloat(msgBuffer,ap[num]);
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN); 
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_DOUBLE: {
      const double *ap;
      ap = (const double*) dp;
      outputDoublePrecisionFloat(msgBuffer,ap[num]);
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN);
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_COMPLEX: {
      const float *ap;
      ap = (const float*) dp;
      outputSinglePrecisionFloat(msgBuffer,ap[2*num]);
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN);
      snprintf(msgBuffer,MSGBUFLEN," ");
      io->outputMessage(msgBuffer);
      outputSinglePrecisionFloat(msgBuffer,ap[2*num+1]);
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN);      
      snprintf(msgBuffer,MSGBUFLEN,"i  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_DCOMPLEX: {
      const double *ap;
      ap = (const double*) dp;
      outputDoublePrecisionFloat(msgBuffer,ap[2*num]);
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN);
      snprintf(msgBuffer,MSGBUFLEN," ");
      io->outputMessage(msgBuffer);
      outputDoublePrecisionFloat(msgBuffer,ap[2*num+1]);
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN);      
      snprintf(msgBuffer,MSGBUFLEN,"i  ");
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_CELL_ARRAY: {
      Array *ap;
      ap = (Array*) dp;
      if (ap == NULL)
	io->outputMessage("[]");
      else
	ap[num].summarizeCellEntry();
      io->outputMessage("  ");
    }
    }
  }

  /**
   * Display this variable on the given output stream.
   */
  void Array::printMe(int printLimit, int termWidth) const {
    int nominalWidth;
    // Print the class...
    switch(dp->dataClass) {
    case FM_UINT8:
      io->outputMessage("  <uint8>  ");
      nominalWidth = 5;
      break;
    case FM_INT8:
      io->outputMessage("  <int8>  ");
      nominalWidth = 6;
      break;
    case FM_UINT16:
      io->outputMessage("  <uint16>  ");
      nominalWidth = 7;
      break;
    case FM_INT16:
      io->outputMessage("  <int16>  ");
      nominalWidth = 8;
      break;
    case FM_UINT32:
      io->outputMessage("  <uint32>  ");
      nominalWidth = 14;
      break;
    case FM_INT32:
      io->outputMessage("  <int32>  ");
      nominalWidth = 15;
      break;
    case FM_FLOAT:
      io->outputMessage("  <float>  ");
      nominalWidth = 20;
      break;
    case FM_DOUBLE:
      io->outputMessage("  <double>  ");
      nominalWidth = 30;
      break;
    case FM_LOGICAL:
      io->outputMessage("  <logical>  ");
      nominalWidth = 2;
      break;
    case FM_STRING:
      io->outputMessage("  <string>  ");
      nominalWidth = 1;
      break;
    case FM_COMPLEX:
      io->outputMessage("  <complex>  ");
      nominalWidth = 36;
      break;
    case FM_DCOMPLEX:
      io->outputMessage("  <dcomplex>  ");
      nominalWidth = 54;
      break;
    case FM_CELL_ARRAY:
      io->outputMessage("  <cell array> ");
      nominalWidth = 10;
      break;
    case FM_STRUCT_ARRAY:
      io->outputMessage("  <structure array> ");
      nominalWidth = 10;
      break;
    }
    io->outputMessage("- size: ");
    dp->dimensions.printMe(io);
    io->outputMessage("\n");
    if (isEmpty()) {
      io->outputMessage("  []\n");
      return;
    }
    if (isSparse()) {
      sprintf(msgBuffer,"\tMatrix is sparse with %d nonzeros\n",
	      getNonzeros());
      io->outputMessage(msgBuffer);
      return;
    }
    if (dp->dataClass == FM_STRUCT_ARRAY) {
      if (dp->dimensions.isScalar()) {
	Array *ap;
	ap = (Array *) dp->getData();
	for (int n=0;n<dp->fieldNames.size();n++) {
	  io->outputMessage("    ");
	  io->outputMessage(dp->fieldNames[n].c_str());
	  io->outputMessage(": ");
	  ap[n].summarizeCellEntry();
	  io->outputMessage("\n");
	}
      } else {
	io->outputMessage("  Fields\n");
	for (int n=0;n<dp->fieldNames.size();n++) {
	  io->outputMessage("    ");
	  io->outputMessage(dp->fieldNames[n].c_str());
	  io->outputMessage("\n");
	}
      }
    } else {
      const void *ap = dp->getData();
      if (dp->dimensions.getLength() == 2) {
	int rows = dp->dimensions.getRows();
	int columns = dp->dimensions.getColumns();
	int items_printed;
	items_printed = 0;
	// Determine how many columns will fit across
	// the terminal width
	int colsPerPage;
	colsPerPage = (int) floor((termWidth-1)/((float) nominalWidth));
	int pageCount;
	pageCount = (int) ceil(columns/((float)colsPerPage));
	for (int k=0;k<pageCount && (items_printed<printLimit);k++) {
	  int colsInThisPage;
	  colsInThisPage = columns - colsPerPage*k;
	  colsInThisPage = (colsInThisPage > colsPerPage) ? 
	    colsPerPage : colsInThisPage;
	  if (dp->dimensions.getElementCount() > 1 && 
	      dp->dataClass != FM_STRING) {
	    snprintf(msgBuffer,MSGBUFLEN," \nColumns %d to %d\n",
		     k*colsPerPage+1,k*colsPerPage+colsInThisPage);
	    io->outputMessage(msgBuffer);
	  }
	  memset(msgBuffer,0,MSGBUFLEN);
	  for (int i=0;i<rows && (items_printed<printLimit);i++) {
	    snprintf(msgBuffer,MSGBUFLEN," ");
	    io->outputMessage(msgBuffer);
	    memset(msgBuffer,0,MSGBUFLEN);
	    for (int j=0;j<colsInThisPage && (items_printed<printLimit);j++) {
	      emitElement(msgBuffer,
			  ap,i+(k*colsPerPage+j)*rows,
			  dp->dataClass);
	      items_printed++;
	    }
	    snprintf(msgBuffer,MSGBUFLEN,"\n");
	    io->outputMessage(msgBuffer);
	    memset(msgBuffer,0,MSGBUFLEN);
	  }
	}
	if (items_printed >= printLimit) 
	  io->outputMessage("\n... Output truncated - use setprintlimit function to see more of the output ...\n");
      } else if (dp->dimensions.getLength() > 2) {
	/**
	 * For N-ary arrays, data slice  -  start with 
	 * [1,1,1,...,1].  We keep doing the matrix
	 * print , incrementing from the highest dimension,
	 * and rolling downwards.
	 */
	Dimensions wdims(dp->dimensions.getLength());
	int rows(dp->dimensions.getRows());
	int columns(dp->dimensions.getColumns());
	int items_printed;
	items_printed = 0;
	int offset = 0;
	while (wdims.inside(dp->dimensions) && (items_printed<printLimit)) {
	  snprintf(msgBuffer,MSGBUFLEN,"(:,:");
	  io->outputMessage(msgBuffer);
	  for (int m=2;m<dp->dimensions.getLength();m++) {
	    snprintf(msgBuffer,MSGBUFLEN,",%d",wdims[m]+1);
	    io->outputMessage(msgBuffer);
	  }
	  snprintf(msgBuffer,MSGBUFLEN,") = \n");
	  io->outputMessage(msgBuffer);
	  // Determine how many columns will fit across
	  // the terminal width
	  int colsPerPage;
	  colsPerPage = (int) floor((termWidth-1)/((float) nominalWidth));
	  int pageCount;
	  pageCount = (int) ceil(columns/((float)colsPerPage));
	  for (int k=0;k<pageCount && (items_printed<printLimit);k++) {
	    int colsInThisPage;
	    colsInThisPage = columns - colsPerPage*k;
	    colsInThisPage = (colsInThisPage > colsPerPage) ? 
	      colsPerPage : colsInThisPage;
	    snprintf(msgBuffer,MSGBUFLEN," \nColumns %d to %d\n",
		     k*colsPerPage+1,k*colsPerPage+colsInThisPage);
	    io->outputMessage(msgBuffer);
	    memset(msgBuffer,0,MSGBUFLEN);
	    for (int i=0;i<rows && (items_printed<printLimit);i++) {
	      snprintf(msgBuffer,MSGBUFLEN," ");
	      io->outputMessage(msgBuffer);
	      memset(msgBuffer,0,MSGBUFLEN);
	      for (int j=0;j<colsInThisPage && (items_printed<printLimit);j++) {
		emitElement(msgBuffer,
			    ap,i+(k*colsPerPage+j)*rows+offset,
			    dp->dataClass);
		items_printed++;
	      }
	      snprintf(msgBuffer,MSGBUFLEN,"\n");
	      io->outputMessage(msgBuffer);
	      memset(msgBuffer,0,MSGBUFLEN);
	    }
	  }
	  offset += rows*columns;
	  wdims.incrementModulo(dp->dimensions,2);
	}
	if (items_printed >= printLimit) 
	  io->outputMessage("\n... Output truncated - use setprintlimit function to see more of the output ...\n");
      }
    }
  }

  char* Array::getContentsAsCString() const  {
    char *buffer;
    const char*qp;
    if (dp->dataClass != FM_STRING)
      throw Exception("Unable to convert supplied object to a string!\n");
    int M = getLength();
    buffer = (char*)Malloc((M+1)*sizeof(char));
    qp = (const char*)dp->getData();
    memcpy(buffer,qp,M);
    buffer[M] = 0;
    return buffer;
  }

  int32 Array::getContentsAsIntegerScalar()  {
    int32 *qp;
    if (getLength() != 1)
      throw Exception("Expected a scalar!\n");
    promoteType(FM_INT32);
    qp = (int32*) dp->getData();
    return (*qp);
  }

  double Array::getContentsAsDoubleScalar()  {
    double *qp;
    if (isComplex() || isReferenceType() || isString())
      throw Exception("Expected a real valued scalar");
    promoteType(FM_DOUBLE);
    qp = (double*) dp->getData();
    return (*qp);
  }

  bool Array::isDataClassReferenceType(Class cls) {
    return (cls == FM_CELL_ARRAY || cls == FM_STRUCT_ARRAY);
  }

  template <class T>
  int32 DoCountNNZReal(const void* dp, int len) {
    int32 accum = 0;
    const T* cp = (const T*) dp;
    for (int i=0;i<len;i++)
      if (cp[i]) accum++;
    return accum;
  }

  template <class T>
  int32 DoCountNNZComplex(const void* dp, int len) {
    int32 accum = 0;
    const T* cp = (const T*) dp;
    for (int i=0;i<len;i++)
      if (cp[2*i] || cp[2*i+1]) accum++;
    return accum;
  }

  int32 Array::nnz() {
    if (isEmpty()) return 0;
    if (isSparse())
      return CountNonzeros(dp->dataClass,
			   getDimensionLength(0),
			   getDimensionLength(1),
			   dp->getData());
    // OK - its not sparse... now what?
    switch (dp->dataClass) {
    case FM_LOGICAL:
      return DoCountNNZReal<logical>(dp->getData(),getLength());
    case FM_INT8:
      return DoCountNNZReal<int8>(dp->getData(),getLength());
    case FM_UINT8:
    case FM_STRING:
      return DoCountNNZReal<uint8>(dp->getData(),getLength());
    case FM_INT16:
      return DoCountNNZReal<int16>(dp->getData(),getLength());
    case FM_UINT16:
      return DoCountNNZReal<uint16>(dp->getData(),getLength());
    case FM_INT32:
      return DoCountNNZReal<int32>(dp->getData(),getLength());
    case FM_UINT32:
      return DoCountNNZReal<uint32>(dp->getData(),getLength());
    case FM_FLOAT:
      return DoCountNNZReal<float>(dp->getData(),getLength());
    case FM_DOUBLE:
      return DoCountNNZReal<double>(dp->getData(),getLength());
    case FM_COMPLEX:
      return DoCountNNZComplex<float>(dp->getData(),getLength());
    case FM_DCOMPLEX:
      return DoCountNNZComplex<double>(dp->getData(),getLength());
    case FM_CELL_ARRAY:
      return DoCountNNZReal<void*>(dp->getData(),getLength());
    case FM_STRUCT_ARRAY:
      return DoCountNNZReal<void*>(dp->getData(),getLength()*dp->fieldNames.size());
    }
  }

  bool Array::anyNotFinite() {
    if (isSparse())
      return SparseAnyNotFinite(dp->dataClass,
				getDimensionLength(0),
				getDimensionLength(1),
				dp->getData());
    switch(dp->dataClass) {
    case FM_FLOAT: 
      {
	const float *sp = (const float *) dp->getData();
	int len = getLength();
	int i=0;
	while (i<len)
	  if (!IsFinite(sp[i++])) return true;
	return false;
      }
    case FM_DOUBLE:
      {
	const double *sp = (const double *) dp->getData();
	int len = getLength();
	int i=0;
	while (i<len)
	  if (!IsFinite(sp[i++])) return true;
	return false;
      }
    case FM_COMPLEX:
      {
	const float *sp = (const float *) dp->getData();
	int len = getLength();
	int i=0;
	while (i<2*len)
	  if (!IsFinite(sp[i++])) return true;
	return false;
      }
    case FM_DCOMPLEX:
      {
	const double *sp = (const double *) dp->getData();
	int len = getLength();
	int i=0;
	while (i<2*len)
	  if (!IsFinite(sp[i++])) return true;
	return false;
      }
    default:
      return false;
    }
  }
}
