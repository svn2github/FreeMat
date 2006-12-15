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
    dim.set(i,array_ptr->dims[i]);
  int count = mxGetNumberOfElements(array_ptr);
  switch (array_ptr->classID) {
  case mxUINT32_CLASS:
    cls = FM_UINT32;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<uint32,uint32>((uint32*)array_ptr->realdata,
				      (uint32*)dp,count);
    break;
  case mxINT32_CLASS:
    cls = FM_INT32;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<int32,int32>((int32*)array_ptr->realdata,
				    (int32*)dp,count);
    break;
  case mxUINT16_CLASS:
    cls = FM_UINT16;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<uint16,uint16>((uint16*)array_ptr->realdata,
				      (uint16*)dp,count);
    break;
  case mxINT16_CLASS:
    cls = FM_INT16;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<int16,int16>((int16*)array_ptr->realdata,
				    (int16*)dp,count);
    break;
  case mxUINT8_CLASS:
    cls = FM_UINT8;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<uint8,uint8>((uint8*)array_ptr->realdata,
				    (uint8*)dp,count);
    break;
  case mxINT8_CLASS:
    cls = FM_INT8;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<int8,int8>((int8*)array_ptr->realdata,
				  (int8*)dp,count);
    break;
  case mxSINGLE_CLASS:
    cls = FM_FLOAT;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<float,float>((float*)array_ptr->realdata,
				    (float*)dp,count);
    break;
  case mxDOUBLE_CLASS:
    cls = FM_DOUBLE;
    dp = Array::allocateArray(cls,count);
    MexRealToArrayReal<double,double>((double*)array_ptr->realdata,
				      (double*)dp,count);
  }
  return Array::Array(cls,dim,dp);
}

Array ArrayFromMexArrayComplex(mxArray *array_ptr) {
  Class cls;
  Dimensions dim;
  void *dp;
  for (int i=0;i<array_ptr->number_of_dims;i++)
    dim.set(i,array_ptr->dims[i]);
  int count = mxGetNumberOfElements(array_ptr);
  if (array_ptr->classID != mxDOUBLE_CLASS) {
    cls = FM_COMPLEX;
    switch(array_ptr->classID) {
    case mxUINT32_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<uint32,float>((uint32*)array_ptr->realdata,
					     (uint32*)array_ptr->imagdata,
					     (float*)dp,count);
      break;
    case mxINT32_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<int32,float>((int32*)array_ptr->realdata,
					    (int32*)array_ptr->imagdata,
					    (float*)dp,count);
    case mxUINT16_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<uint16,float>((uint16*)array_ptr->realdata,
					     (uint16*)array_ptr->imagdata,
					     (float*)dp,count);
      break;
    case mxINT16_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<int16,float>((int16*)array_ptr->realdata,
					    (int16*)array_ptr->imagdata,
					    (float*)dp,count);
    case mxUINT8_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<uint8,float>((uint8*)array_ptr->realdata,
					    (uint8*)array_ptr->imagdata,
					    (float*)dp,count);
      break;
    case mxINT8_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<int8,float>((int8*)array_ptr->realdata,
					   (int8*)array_ptr->imagdata,
					   (float*)dp,count);
      break;
    case mxSINGLE_CLASS:
      dp = Array::allocateArray(FM_COMPLEX,count);
      MexComplexToArrayComplex<float,float>((float*)array_ptr->realdata,
					    (float*)array_ptr->imagdata,
					    (float*)dp,count);
    }
  } else {
    cls = FM_DCOMPLEX;
    dp = Array::allocateArray(FM_DCOMPLEX,count);
    MexComplexToArrayComplex<double,double>((double*)array_ptr->realdata,
					    (double*)array_ptr->imagdata,
					    (double*)dp,count);
  }
  return Array::Array(cls,dim,dp);
}

Array ArrayFromMexArray(mxArray *array_ptr) {
  if (array_ptr->classID == mxCELL_CLASS) {
    Dimensions dim;
    for (int i=0;i<array_ptr->number_of_dims;i++)
      dim.set(i,array_ptr->dims[i]);
    int N = mxGetNumberOfElements(array_ptr);
    mxArray** dp = (mxArray**) array_ptr->realdata;
    Array* cp = new Array[N];
    for (int i=0;i<N;i++)
      cp[i] = ArrayFromMexArray(dp[i]);
    return Array(FM_CELL_ARRAY,dim,cp);
  } else {
    if (array_ptr->iscomplex) {
      return ArrayFromMexArrayComplex(array_ptr);
    } else {
      return ArrayFromMexArrayReal(array_ptr);
    }
  }
}

template <class mxType, class fmType>
mxArray* MexArrayFromRealArray(Array array, mxClassID classID) {
  // Convert array dimensions into a simple integer array
  int num_dim = array.dimensions().getLength();
  int *dim_vec = (int*) malloc(sizeof(int)*num_dim);
  for (int i=0;i<num_dim;i++)
    dim_vec[i] = array.dimensions().get(i);
  mxArray *ret = mxCreateNumericArray(num_dim,dim_vec,classID,mxREAL);
  fmType *sp = (fmType*) array.getDataPointer();
  mxType *dp = (mxType*) ret->realdata;
  int N = mxGetNumberOfElements(ret);
  for (int i=0;i<N;i++)
    dp[i] = (mxType) sp[i];
  free(dim_vec);
  return ret;
}

template <class mxType, class fmType>
mxArray* MexArrayFromComplexArray(Array array, mxClassID classID) {
  // Convert array dimensions into a simple integer array
  int num_dim = array.dimensions().getLength();
  int *dim_vec = (int*) malloc(sizeof(int)*num_dim);
  for (int i=0;i<num_dim;i++)
    dim_vec[i] = array.dimensions().get(i);
  mxArray *ret = mxCreateNumericArray(num_dim,dim_vec,classID,mxCOMPLEX);
  fmType *sp = (fmType*) array.getDataPointer();
  mxType *dp_r = (mxType*) ret->realdata;
  mxType *dp_i = (mxType*) ret->imagdata;
  int N = mxGetNumberOfElements(ret);
  for (int i=0;i<N;i++) {
    dp_r[i] = (mxType) sp[2*i];
    dp_i[i] = (mxType) sp[2*i+1];
  }
  free(dim_vec);
  return ret;
}

mxArray* MexArrayFromCellArray(Array array) {
  // Convert array dimensions into a simple integer array
  int num_dim = array.dimensions().getLength();
  int *dim_vec = (int*) malloc(sizeof(int)*num_dim);
  for (int i=0;i<num_dim;i++)
    dim_vec[i] = array.dimensions().get(i);
  mxArray *ret = mxCreateCellArray(num_dim,dim_vec);
  Array *sp = (Array*) array.getDataPointer();
  mxArray **dp = (mxArray **) ret->realdata;
  int N = mxGetNumberOfElements(ret);
  for (int i=0;i<N;i++) 
    dp[i] = MexArrayFromArray(sp[i]);
  free(dim_vec);
  return ret;
}

mxArray* MexArrayFromArray(Array array) {
  switch(array.dataClass()) {
  case FM_FUNCPTR_ARRAY:
    return NULL;
  case FM_CELL_ARRAY:
    return MexArrayFromCellArray(array);
  case FM_STRUCT_ARRAY:
    return NULL;
  case FM_LOGICAL:
    return MexArrayFromRealArray<mxLogical,logical>(array,mxLOGICAL_CLASS);
  case FM_UINT8:
    return MexArrayFromRealArray<uint8,uint8>(array,mxUINT8_CLASS);
  case FM_INT8:
    return MexArrayFromRealArray<int8,int8>(array,mxINT8_CLASS);
  case FM_UINT16:
    return MexArrayFromRealArray<uint16,uint16>(array,mxUINT16_CLASS);
  case FM_INT16:
    return MexArrayFromRealArray<int16,int16>(array,mxINT16_CLASS);
  case FM_UINT32:
    return MexArrayFromRealArray<uint32,uint32>(array,mxUINT32_CLASS);
  case FM_INT32:
    return MexArrayFromRealArray<int32,int32>(array,mxINT32_CLASS);
  case FM_FLOAT:
    return MexArrayFromRealArray<float,float>(array,mxSINGLE_CLASS);
  case FM_DOUBLE:
    return MexArrayFromRealArray<double,double>(array,mxDOUBLE_CLASS);
  case FM_COMPLEX:
    return MexArrayFromComplexArray<float,float>(array,mxSINGLE_CLASS);
  case FM_DCOMPLEX:
    return MexArrayFromComplexArray<double,double>(array,mxDOUBLE_CLASS);
  case FM_STRING:
    return MexArrayFromRealArray<mxChar,char>(array,mxCHAR_CLASS);
  }
  return NULL;
}

