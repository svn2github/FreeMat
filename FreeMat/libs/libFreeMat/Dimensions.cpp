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

#include "Dimensions.hpp"
#include "Exception.hpp"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include "Malloc.hpp"

namespace FreeMat {

#define MSGBUFLEN 2048
  static char msgBuffer[MSGBUFLEN];

  Dimensions::Dimensions() {
    length = 0;
  }

  Dimensions::Dimensions(int dimCount) throw (Exception) {
    if (dimCount < 0) 
      throw Exception("Illegal argument to Dimensions constructor");
    memset(data, 0, sizeof(int)*dimCount);
    length = dimCount;
  }

  int Dimensions::getMax() {
    int maxL;
    maxL = 0;
    for (int i=0;i<length;i++)
      maxL = (maxL > data[i]) ? maxL : data[i];
    return maxL;
  }

  int& Dimensions::operator[](unsigned i) throw (Exception){
    if (i < 0)
      throw Exception("Negative argument to [] operator of Dimensions class.");
    if (i >= maxDims )
      throw Exception("Too many dimensions! Current limit is 6.");
    if (i >= length) {
      int new_length = i+1;
      for (int j=length;j<new_length;j++)
	data[j] = 1;
      length = new_length;
    }
    return data[i];
  }

  const int Dimensions::getLength() const{
    return length;
  }

  const int Dimensions::getElementCount() const {
    int retval;

    if (length == 0) return 0;
    retval = 1;
    for (int i=0;i<length;i++)
      retval *= data[i];
    return retval;
  }

  const int Dimensions::getRows() const {
    if (length == 0) 
      return 0;
    else
      return data[0];
  }

  const int Dimensions::getColumns() const {
    if (length == 0)
      return 0;
    else if (length == 1)
      return 1;
    else
      return data[1];
  }

  const int Dimensions::getDimensionLength(int arg) const {
    if (length <= arg)
      return 1;
    else
      return data[arg];
  }

  void Dimensions::setDimensionLength(int dim, int len) {
    data[dim] = len;
  }

  const int Dimensions::mapPoint(const Dimensions& point) throw (Exception) {
    int retval;
    int nextCoeff;
    int testableDims;

    retval = 0;
    nextCoeff = 1;
    testableDims = (point.length < length) ? point.length : length;
    for (int i=0;i<testableDims;i++) {
      if ((point.data[i] < 0) || (point.data[i] >= data[i]))
	throw Exception("Array index out of bounds");
      retval += nextCoeff*point.data[i];
      nextCoeff *= data[i];
    }
    for (int j=testableDims;j<point.length;j++) {
      if (point.data[j] != 0)
	throw Exception("Array index out of bounds");
    }
    return retval;
  }

  void Dimensions::expandToCover(const Dimensions& a) {
    int sze;
    int i;
    Dimensions dimensions(*this);

    /**
     * First, compute the larger of the two: the number of current dimensions
     * and the number of requested dimensions.
     */
    sze = (a.length > length) ? a.length : dimensions.length;

    /**
     * Allocate a dimension vector to hold the new dimensions.  It should
     * be of size sze.
     */
    reset();
  
    /**
     * Now we loop over the dimensions.  For each dimensions, we could have
     * three cases to deal with:
     *   1. a[i] is undefined but dimensions[i] is -> newsize[i] = dimensions[i];
     *   2. a[i] is defined but dimensions[i] is not -> newsize[i] = a[i];
     *   3. a[i] and dimensions[i] are both defined -> 
     *                                newsize[i] = max(a[i],dimensions[i]);
     */
    for (i=0;i<sze;i++) {
      /**
       * Case 1:
       */
      if (i>=a.length)
	(*this)[i] = dimensions[i];
      /**
       * Case 2:
       */
      else if (i>=dimensions.length)
	(*this)[i] = a.data[i];
      else 
	(*this)[i] = (a.data[i] > dimensions[i]) ? a.data[i] : dimensions[i];
    }
  }

  void Dimensions::incrementModulo(const Dimensions& limit, int ordinal) {
    int n;

    data[ordinal]++;
    for (n=ordinal;n<length-1;n++)
      if (data[n] >= limit.data[n]) {
	data[n] = 0;
	data[n+1]++;
      }
  }

  bool Dimensions::inside(const Dimensions& limit) {
    return (data[length-1] < limit.data[length-1]);
  }

  void Dimensions::simplify() {
    if (length <= 2) return;
    int trimcount = 0;
    int i = length-1;
    while (i>1 && data[i] == 1) i--;
    length = i+1;
  }

  bool Dimensions::equals(const Dimensions &alt) {
    bool retval;
    retval = (length == alt.length);
    for (int i=0;i<length;i++)
      retval = retval && (data[i] == alt.data[i]);
    return retval;
  }

  void Dimensions::printMe(Interface*io) const {
    snprintf(msgBuffer,MSGBUFLEN,"[");
    io->outputMessage(msgBuffer);
    for (int i=0;i<length-1;i++) {
      snprintf(msgBuffer,MSGBUFLEN,"%d ",data[i]);
      io->outputMessage(msgBuffer);
    }
    if (length >= 1)
      snprintf(msgBuffer,MSGBUFLEN,"%d]",data[length-1]);
    else
      snprintf(msgBuffer,MSGBUFLEN,"]");
    io->outputMessage(msgBuffer);
  }

  void Dimensions::reset() {
    length = 0;
    memset(data, 0, sizeof(int)*maxDims);
  }

  void Dimensions::zeroOut() {
    for (int i=0;i<length;i++)
      data[i] = 0;
  }

  void Dimensions::makeScalar() {
    reset();
    length = 2;
    data[0] = 1;
    data[1] = 1;
  }

  const bool Dimensions::isScalar() const {
    return (getElementCount() == 1);
  }

  const bool Dimensions::isVector() const {
     return ((getElementCount() == data[0]) || (getElementCount() == data[1]));
  }

  const bool Dimensions::is2D() const {
     return (getElementCount() == (getRows()*getColumns()));
  }
}
