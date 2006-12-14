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

#include "Dimensions.hpp"
#include "Exception.hpp"
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include "Malloc.hpp"
#include "Interpreter.hpp"

#ifdef WIN32
#define snprintf _snprintf
#endif

#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

//Slimming down Dimensions...  In demo(6), the Dimension related
//codes occupy 14+6.5+4+3.5+3+2.65+2.45+2.35 = 37% of the time!
//I want this factor back.  One issue that is clear is that the
//Dimension class itself is relatively clean.  But because of the
//operator[] call, Dimensions does not know its state (e.g., am I
//a vector?).  So what I want to do is move the cache of the dimension
//quantities from Data to Dimensions.  And to have Dimensions be
//responsible for cacheing the quantities of interest.

int Dimensions::getMax() {
  int maxL;
  maxL = 0;
  for (int i=0;i<length;i++)
    maxL = (maxL > data[i]) ? maxL : data[i];
  return maxL;
}

void Dimensions::updateCacheVariables() {
  m_cache_getElementCount = 0;
  if (length == 0) 
    m_cache_getElementCount = 0;
  else {
    m_cache_getElementCount = 1;
    for (int i=0;i<length;i++)
      m_cache_getElementCount *= data[i];
  }
  m_cache_isScalar = (m_cache_getElementCount == 1);
  if (length == 0)
    m_cache_getRows = 0;
  else
    m_cache_getRows = data[0];
  if (length == 0)
    m_cache_getColumns = 0;
  else if (length == 1)
    m_cache_getColumns = 1;
  else
    m_cache_getColumns = data[1];
  m_cache_is2D = (m_cache_getRows*m_cache_getColumns==m_cache_getElementCount);
  
  m_cache_isVector = (m_cache_getRows==m_cache_getElementCount) ||
    (m_cache_getColumns == m_cache_getElementCount);
}

void Dimensions::setDimensionLength(int dim, int len) {
  if (dim >= maxDims )
    throw Exception("Too many dimensions! Current limit is 6.");
  if (dim >= length) {
    int new_length = dim+1;
    for (int j=length;j<new_length;j++)
      data[j] = 1;
    length = new_length;
  }
  data[dim] = len;
  updateCacheVariables();
}

int Dimensions::mapPoint(const Dimensions& point) const {
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
      set(i,dimensions.get(i));
    /**
     * Case 2:
     */
    else if (i>=dimensions.length)
      set(i,a.data[i]);
    else 
      set(i,((a.data[i] > dimensions.get(i)) ? a.data[i] : dimensions.get(i)));
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
  updateCacheVariables();
}

void Dimensions::simplify() {
  if (length <= 2) return;
  int trimcount = 0;
  int i = length-1;
  while (i>1 && data[i] == 1) i--;
  length = i+1;
  updateCacheVariables();
}

bool Dimensions::equals(const Dimensions &alt) const {
  bool retval;
  retval = (length == alt.length);
  for (int i=0;i<length;i++)
    retval = retval && (data[i] == alt.data[i]);
  return retval;
}

std::string Dimensions::asString() const {
  std::string output;
  output.append("[");
  for (int i=0;i<length-1;i++) {
    snprintf(msgBuffer,MSGBUFLEN,"%d ",data[i]);
    output.append(msgBuffer);;
  }
  if (length >= 1)
    snprintf(msgBuffer,MSGBUFLEN,"%d]",data[length-1]);
  else
    snprintf(msgBuffer,MSGBUFLEN,"]");
  output.append(msgBuffer);
  return output;
}

void Dimensions::printMe(Interpreter* eval) const {
  snprintf(msgBuffer,MSGBUFLEN,"[");
  eval->outputMessage(msgBuffer);
  for (int i=0;i<length-1;i++) {
    snprintf(msgBuffer,MSGBUFLEN,"%d ",data[i]);
    eval->outputMessage(msgBuffer);
  }
  if (length >= 1)
    snprintf(msgBuffer,MSGBUFLEN,"%d]",data[length-1]);
  else
    snprintf(msgBuffer,MSGBUFLEN,"]");
  eval->outputMessage(msgBuffer);
}

void Dimensions::reset() {
  length = 0;
  memset(data, 0, sizeof(int)*maxDims);
  updateCacheVariables();
}

void Dimensions::zeroOut() {
  for (int i=0;i<length;i++)
    data[i] = 0;
  updateCacheVariables();
}

void Dimensions::makeScalar() {
  reset();
  length = 2;
  data[0] = 1;
  data[1] = 1;
  updateCacheVariables();
}

