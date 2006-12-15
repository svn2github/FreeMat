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
#include "LAPACK.hpp"
#include "LUDecompose.hpp"
#include "Malloc.hpp"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

int* FixupPivotVector(int nrows, int *piv, int p) {
  // Adjust the pivot vector
  int *fullpivot = (int*) malloc(sizeof(int)*nrows);
  int i;
  for (i=0;i<nrows;i++)
    fullpivot[i] = i;
  for (i=0;i<p;i++) 
    if (piv[i] != (i+1)) {
      int tmp = fullpivot[i];
      fullpivot[i] = fullpivot[piv[i]-1];
      fullpivot[piv[i]-1] = tmp;
    }
  free(piv);
  return fullpivot;
}

// In all cases, l is m x n  and   u is n x n if m > n
//               l is m x m  and   u is m x n if m < n
//           or  l is m x p  and   u is p x n

template <class T>
void RealLUP(int nrows, int ncols, T *l, T *u,
	     T *pmat, T *a, 
	     void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  //  sgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  for (i=0;i<nrows;i++)
    pmat[i+fullpivot[i]*nrows] = 1;
  free(fullpivot);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++)
      l[i+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++)
	l[i+j*lrows] = a[i+j*nrows];
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++)
	u[i+j*urows] = a[i+j*nrows];
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++)
      l[i+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++)
	l[i+j*lrows] = a[i+j*nrows];
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++)
	u[i+j*urows] = a[i+j*nrows];
  }
}

template <class T>
void RealLU(int nrows, int ncols, T *l, T *u, T*a, 
	    void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  //  dgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
  // Adjust the pivot vector
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++)
      l[fullpivot[i]+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++)
	l[fullpivot[i]+j*lrows] = a[i+j*nrows];
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++)
	u[i+j*urows] = a[i+j*nrows];
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++)
      l[fullpivot[i]+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++)
	l[fullpivot[i]+j*lrows] = a[i+j*nrows];
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++)
	u[i+j*urows] = a[i+j*nrows];
  }
  free(fullpivot);
}

template <class T>
void ComplexLUP(int nrows, int ncols, T *l, T *u,
		T *pmat, T *a, 
		void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  for (i=0;i<nrows;i++)
    pmat[i+fullpivot[i]*nrows] = 1;
  free(fullpivot);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++) {
      l[2*(i+i*lrows)] = 1.0;
      l[2*(i+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++) {
	l[2*(i+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(i+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++) {
      l[2*(i+i*lrows)] = 1.0;
      l[2*(i+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++) {
	l[2*(i+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(i+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  }
}

template <class T>
void ComplexLU(int nrows, int ncols, T *l, T *u, T*a, 
	       void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  // Adjust the pivot vector
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++) {
      l[2*(fullpivot[i]+i*lrows)] = 1.0;
      l[2*(fullpivot[i]+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++) {
	l[2*(fullpivot[i]+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(fullpivot[i]+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++) {
      l[2*(fullpivot[i]+i*lrows)] = 1.0;
      l[2*(fullpivot[i]+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++) {
	l[2*(fullpivot[i]+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(fullpivot[i]+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  }
  free(fullpivot);
}


ArrayVector LUDecompose(int nargout, Array A) {
  if (nargout > 3)
    throw Exception("illegal usage of lu function - for dense matrices, maximum number of outputs is 3");
  int nrows = A.getDimensionLength(0);
  int ncols = A.getDimensionLength(1);
  int p = min(nrows,ncols);
  if (A.isIntegerClass())
    A.promoteType(FM_DOUBLE);
  ArrayVector retval;
  switch (A.dataClass()) {
  case FM_FLOAT: {
    float *l = (float*) Malloc(sizeof(float)*nrows*p);
    float *u = (float*) Malloc(sizeof(float)*p*ncols);
    if (nargout <= 2) {
      RealLU<float>(nrows,ncols,l,u,
		     (float*) A.getReadWriteDataPointer(),sgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
    } else if (nargout == 3) {
      float *piv = (float*) Malloc(sizeof(float)*nrows*nrows);
      RealLUP<float>(nrows,ncols,l,u,piv,
		     (float*) A.getReadWriteDataPointer(),sgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,nrows),piv));
    }
    return retval;
  }
  case FM_DOUBLE: {
    double *l = (double*) Malloc(sizeof(double)*nrows*p);
    double *u = (double*) Malloc(sizeof(double)*p*ncols);
    if (nargout <= 2) {
      RealLU<double>(nrows,ncols,l,u,
		     (double*) A.getReadWriteDataPointer(),dgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
    } else if (nargout == 3) {
      double *piv = (double*) Malloc(sizeof(double)*nrows*nrows);
      RealLUP<double>(nrows,ncols,l,u,piv,
		      (double*) A.getReadWriteDataPointer(),dgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,nrows),piv));
    }
    return retval;
  }
  case FM_COMPLEX: {
    float *l = (float*) Malloc(2*sizeof(float)*nrows*p);
    float *u = (float*) Malloc(2*sizeof(float)*p*ncols);
    if (nargout <= 2) {
      ComplexLU<float>(nrows,ncols,l,u,
		       (float*) A.getReadWriteDataPointer(),cgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
    } else if (nargout == 3) {
      float *piv = (float*) Malloc(sizeof(float)*nrows*nrows);
      ComplexLUP<float>(nrows,ncols,l,u,piv,
			(float*) A.getReadWriteDataPointer(),cgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
      retval.push_back(Array(FM_FLOAT,Dimensions(nrows,nrows),piv));
    }
    return retval;
  }
  case FM_DCOMPLEX: {
    double *l = (double*) Malloc(2*sizeof(double)*nrows*p);
    double *u = (double*) Malloc(2*sizeof(double)*p*ncols);
    if (nargout <= 2) {
      ComplexLU<double>(nrows,ncols,l,u,
			(double*) A.getReadWriteDataPointer(),zgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
    } else if (nargout == 3) {
      double *piv = (double*) Malloc(sizeof(double)*nrows*nrows);
      ComplexLUP<double>(nrows,ncols,l,u,piv,
			 (double*) A.getReadWriteDataPointer(),zgetrf_);
      retval.push_back(Array(A.dataClass(),Dimensions(nrows,p),l));
      retval.push_back(Array(A.dataClass(),Dimensions(p,ncols),u));
      retval.push_back(Array(FM_DOUBLE,Dimensions(nrows,nrows),piv));
    }
    return retval;
  }
  }
}

