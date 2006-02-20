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

#include "Array.hpp"
#include "mex.h"
#include "MexInterface.hpp"

template <class T, class S>
void ArrayComplexToMexComplex(T*src, S*dst_real, S*dst_imag, int count) {
  for (int i=0;i<count;i++) {
    dst_real[i] = (S) src[2*i];
    dst_imag[i] = (S) src[2*i+1];
  }
}

template <class T, class S>
void MexComplexToArrayComplex(T*src_real, T*src_imag, S*dst, int count) {
  for (int i=0;i<count;i++) {
    dst[2*i] = (S) src_real[i];
    dst[2*i+1] = (S) src_imag[i];
  }
}

template <class T, class S>
void ArrayRealToMexReal(T*src, S*dst, int count) {
  for (int i=0;i<count;i++)
    dst[i] = (S) src[i];
}

template <class T, class S>
void MexRealToArrayReal(T*src, S*dst, int count) {
  for (int i=0;i<count;i++)
    dst[i] = (S) src[i];
}

Array ArrayFromMexArrayReal(mxArray *array_ptr) {
  Class cls;
  Dimensions dim;
  void *dp;
  for (int i=0;i<array_ptr->number_of_dims;i++)
    dim[array_ptr->dims[i]];
  int count = mxGetNumberOfElements(array_ptr);
  switch (array_ptr->classID) {
  case mxUINT32_CLASS:
    cls = FM_UINT32;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<uint32,uint32>(array_ptr->realdata,
				      (uint32*)dp,count);
    break;
  case mxINT32_CLASS:
    cls = FM_INT32;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<int32,int32>(array_ptr->realdata,
				    (int32*)dp,count);
    break;
  case mxUINT16_CLASS:
    cls = FM_UINT16;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<uint16,uint16>(array_ptr->realdata,
				      (uint16*)dp,count);
    break;
  case mxINT16_CLASS:
    cls = FM_INT16;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<int16,int16>(array_ptr->realdata,
				    (int16*)dp,count);
    break;
  case mxUINT8_CLASS:
    cls = FM_UINT8;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<uint8,uint8>(array_ptr->realdata,
				    (uint8*)dp,count);
    break;
  case mxINT8_CLASS:
    cls = FM_INT8;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<int8,int8>(array_ptr->realdata,
				      (int8*)dp,count);
    break;
  case mxSINGLE_CLASS:
    cls = FM_FLOAT;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<float,float>(array_ptr->realdata,
				    (float*)dp,count);
    break;
  case mxDOUBLE_CLASS:
    cls = FM_DOUBLE;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<double,double>(array_ptr->realdata,
				      (double*)dp,count);
  }
  return Array::Array(cls,dim,dp);
}

Array ArrayFromMexArrayComplex(mxArray *array_ptr) {
  Class cls;
  Dimensions dim;
  void *dp;
  for (int i=0;i<array_ptr->number_of_dims;i++)
    dim[array_ptr->dims[i]];
  int count = mxGetNumberOfElements(array_ptr);
  if (array_ptr->classID != mxDOUBLE_CLASS) {
    cls = FM_COMPLEX;
    switch(array_ptr->classID) {
    case mxUINT32_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<uint32,float>(array_ptr->realdata,
					     array_ptr->imagdata,
					     (float*)dp,count);
      break;
    case mxINT32_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<int32,float>(array_ptr->realdata,
					    array_ptr->imagdata,
					    (float*)dp,count);
    case mxUINT16_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<uint16,float>(array_ptr->realdata,
					     array_ptr->imagdata,
					     (float*)dp,count);
      break;
    case mxINT16_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<int16,float>(array_ptr->realdata,
					    array_ptr->imagdata,
					    (float*)dp,count);
    case mxUINT8_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<uint8,float>(array_ptr->realdata,
					    array_ptr->imagdata,
					    (float*)dp,count);
      break;
    case mxINT8_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<int8,float>(array_ptr->realdata,
					   array_ptr->imagdata,
					   (float*)dp,count);
      break;
    case mxFLOAT_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<float,float>(array_ptr->realdata,
					    array_ptr->imagdata,
					    (float*)dp,count);
    }
  } else {
    cls = FM_DCOMPLEX;
    dp = Array::allocateArray(FM_DCOMPLEX,count);
    MexComplexToArrayComplex<double,double>(array_ptr->realdata,
					    array_ptr->imagdata,
					    (float*)dp,count);
  }
  return Array::Array(cls,dim,dp);
}

Array ArrayFromMexArray(mxArray *array_ptr) {
  if (array_ptr->classID == mxCELL_CLASS) {
    Dimensions dim;
    for (int i=0;i<array_ptr->number_of_dims;i++)
      dim[array_ptr->dims[i]];
    int N = mxGetNumberOfElements(array_ptr);
    mxArray** dp = array_ptr->realdata;
    Array* cp = new Array[N];
    for (int i=0;i<N;i++)
      cp[i] = ArrayFromMexArray(dp[i]);
    return Array(FM_CELL,dim,cp);
  } else {
    if (array_ptr->isComplex) {
      return ArrayFromMexArrayComplex(array_ptr);
    } else {
      return ArrayFromMexArrayReal(array_ptr);
    }
  }
}

mxArray* MexArrayFromArray(Array array) {
}

