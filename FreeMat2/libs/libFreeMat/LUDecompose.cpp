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

namespace FreeMat {
  // In all cases, l is m x n  and   u is n x n if m > n
  //               l is m x m  and   u is m x n if m < n
  //           or  l is m x p  and   u is p x n
  void floatLUP(int nrows, int ncols, float *l, float *u,
		float *pmat, float *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    sgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    for (i=0;i<nrows;i++)
      pmat[i+fullpivot[i]*nrows] = 1;
    free(piv);
    free(fullpivot);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++)
	l[i+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++)
	  l[i+j*nrows] = a[i+j*nrows];
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*nrows] = a[i+j*nrows];
    } else {
      for (i=0;i<nrows;i++)
	l[i+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++)
	  l[i+j*nrows] = a[i+j*nrows];
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*ncols] = a[i+j*nrows];
    }
  }

  // In all cases, l is m x n  and   u is n x n if m > n
  //               l is m x m  and   u is m x n if m < n
  //           or  l is m x p  and   u is p x n
  void floatLU(int nrows, int ncols, float *l, float *u, float *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    sgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    free(piv);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++)
	l[fullpivot[i]+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++)
	  l[fullpivot[i]+j*nrows] = a[i+j*nrows];
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*nrows] = a[i+j*nrows];
    } else {
      for (i=0;i<nrows;i++)
	l[fullpivot[i]+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++)
	  l[fullpivot[i]+j*nrows] = a[i+j*nrows];
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*ncols] = a[i+j*nrows];
    }
    free(fullpivot);
  }

  void doubleLUP(int nrows, int ncols, double *l, double *u,
		 double *pmat, double *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    dgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    for (i=0;i<nrows;i++)
      pmat[i+fullpivot[i]*nrows] = 1;
    free(piv);
    free(fullpivot);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++)
	l[i+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++)
	  l[i+j*nrows] = a[i+j*nrows];
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*nrows] = a[i+j*nrows];
    } else {
      for (i=0;i<nrows;i++)
	l[i+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++)
	  l[i+j*nrows] = a[i+j*nrows];
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*ncols] = a[i+j*nrows];
    }
  }

  // In all cases, l is m x n  and   u is n x n if m > n
  //               l is m x m  and   u is m x n if m < n
  //           or  l is m x p  and   u is p x n
  void doubleLU(int nrows, int ncols, double *l, double *u, double *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    dgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    free(piv);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++)
	l[fullpivot[i]+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++)
	  l[fullpivot[i]+j*nrows] = a[i+j*nrows];
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*nrows] = a[i+j*nrows];
    } else {
      for (i=0;i<nrows;i++)
	l[fullpivot[i]+i*nrows] = 1.0;
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++)
	  l[fullpivot[i]+j*nrows] = a[i+j*nrows];
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++)
	  u[i+j*ncols] = a[i+j*nrows];
    }
    free(fullpivot);
  }

  void complexLUP(int nrows, int ncols, float *l, float *u,
		float *pmat, float *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    cgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    for (i=0;i<nrows;i++)
      pmat[i+fullpivot[i]*nrows] = 1;
    free(piv);
    free(fullpivot);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++) {
	l[2*(i+i*nrows)] = 1.0;
	l[2*(i+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++) {
	  l[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++) {
	  u[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  u[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
    } else {
      for (i=0;i<nrows;i++) {
	l[2*(i+i*nrows)] = 1.0;
	l[2*(i+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++) {
	  l[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++) {
	  u[2*(i+j*ncols)] = a[2*(i+j*nrows)];
	  u[2*(i+j*ncols)+1] = a[2*(i+j*nrows)+1];
	}
    }
  }

  // In all cases, l is m x n  and   u is n x n if m > n
  //               l is m x m  and   u is m x n if m < n
  //           or  l is m x p  and   u is p x n
  void complexLU(int nrows, int ncols, float *l, float *u, float *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    cgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    // Create the inverse pivot vector
    free(piv);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++) {
	l[2*(fullpivot[i]+i*nrows)] = 1.0;
	l[2*(fullpivot[i]+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++) {
	  l[2*(fullpivot[i]+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(fullpivot[i]+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++){
	  u[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  u[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
    } else {
      for (i=0;i<nrows;i++) {
	l[2*(fullpivot[i]+i*nrows)] = 1.0;
	l[2*(fullpivot[i]+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++) {
	  l[2*(fullpivot[i]+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(fullpivot[i]+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++) {
	  u[2*(i+j*ncols)] = a[2*(i+j*nrows)];
	  u[2*(i+j*ncols)+1] = a[2*(i+j*nrows)+1];
	}
    }
    free(fullpivot);
  }

  void dcomplexLUP(int nrows, int ncols, double *l, double *u,
		double *pmat, double *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    zgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    for (i=0;i<nrows;i++)
      pmat[i+fullpivot[i]*nrows] = 1;
    free(piv);
    free(fullpivot);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++) {
	l[2*(i+i*nrows)] = 1.0;
	l[2*(i+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++) {
	  l[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++) {
	  u[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  u[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
    } else {
      for (i=0;i<nrows;i++) {
	l[2*(i+i*nrows)] = 1.0;
	l[2*(i+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++) {
	  l[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++) {
	  u[2*(i+j*ncols)] = a[2*(i+j*nrows)];
	  u[2*(i+j*ncols)+1] = a[2*(i+j*nrows)+1];
	}
    }
  }

  // In all cases, l is m x n  and   u is n x n if m > n
  //               l is m x m  and   u is m x n if m < n
  //           or  l is m x p  and   u is p x n
  void dcomplexLU(int nrows, int ncols, double *l, double *u, double *a) {
    int info;
    int i, j;
    int *piv;
    int p = min(nrows,ncols);
    piv = (int*) malloc(sizeof(int)*p);
    zgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
    // Adjust the pivot vector
    int *fullpivot = (int*) malloc(sizeof(int)*nrows);
    for (i=0;i<nrows;i++)
      fullpivot[i] = i;
    for (i=0;i<p;i++) 
      if (piv[i] != (i+1)) {
	int tmp = fullpivot[i];
	fullpivot[i] = fullpivot[piv[i]-1];
	fullpivot[piv[i]-1] = tmp;
      }
    free(piv);
    if (nrows > ncols) {
      for (i=0;i<ncols;i++) {
	l[2*(fullpivot[i]+i*nrows)] = 1.0;
	l[2*(fullpivot[i]+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<min(i,ncols);j++) {
	  l[2*(fullpivot[i]+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(fullpivot[i]+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<ncols;i++)
	for (j=i;j<ncols;j++){
	  u[2*(i+j*nrows)] = a[2*(i+j*nrows)];
	  u[2*(i+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
    } else {
      for (i=0;i<nrows;i++) {
	l[2*(fullpivot[i]+i*nrows)] = 1.0;
	l[2*(fullpivot[i]+i*nrows)+1] = 0.0;
      }
      for (i=1;i<nrows;i++)
	for (j=0;j<i;j++) {
	  l[2*(fullpivot[i]+j*nrows)] = a[2*(i+j*nrows)];
	  l[2*(fullpivot[i]+j*nrows)+1] = a[2*(i+j*nrows)+1];
	}
      for (i=0;i<nrows;i++)
	for (j=i;j<ncols;j++) {
	  u[2*(i+j*ncols)] = a[2*(i+j*nrows)];
	  u[2*(i+j*ncols)+1] = a[2*(i+j*nrows)+1];
	}
    }
  }


  ArrayVector LUDecompose(int nargout, Array A) {
    if (nargout > 3)
      throw Exception("illegal usage of lu function - for dense matrices, maximum number of outputs is 3");
    int nrows = A.getDimensionLength(0);
    int ncols = A.getDimensionLength(1);
    int p = max(nrows,ncols);
    int q = min(nrows,ncols);
    if (A.getDataClass() == FM_INT32)
      A.promoteType(FM_DOUBLE);
    ArrayVector retval;
    switch (A.getDataClass()) {
    case FM_FLOAT: {
      float *l = (float*) Malloc(sizeof(float)*nrows*p);
      float *u = (float*) Malloc(sizeof(float)*p*ncols);
      if (nargout <= 2) {
	floatLU(nrows,ncols,l,u,(float*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
      } else if (nargout == 3) {
	float *piv = (float*) Malloc(sizeof(float)*nrows*nrows);
	floatLUP(nrows,ncols,l,u,piv,(float*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,nrows),piv));
      }
      return retval;
    }
    case FM_DOUBLE: {
      double *l = (double*) Malloc(sizeof(double)*nrows*p);
      double *u = (double*) Malloc(sizeof(double)*p*ncols);
      if (nargout <= 2) {
	doubleLU(nrows,ncols,l,u,(double*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
      } else if (nargout == 3) {
	double *piv = (double*) Malloc(sizeof(double)*nrows*nrows);
	doubleLUP(nrows,ncols,l,u,piv,(double*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,nrows),piv));
      }
      return retval;
    }
    case FM_COMPLEX: {
      float *l = (float*) Malloc(2*sizeof(float)*nrows*p);
      float *u = (float*) Malloc(2*sizeof(float)*p*ncols);
      if (nargout <= 2) {
	complexLU(nrows,ncols,l,u,(float*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
      } else if (nargout == 3) {
	float *piv = (float*) Malloc(sizeof(float)*nrows*nrows);
	complexLUP(nrows,ncols,l,u,piv,(float*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
	retval.push_back(Array(FM_FLOAT,Dimensions(nrows,nrows),piv));
      }
      return retval;
    }
    case FM_DCOMPLEX: {
      double *l = (double*) Malloc(2*sizeof(double)*nrows*p);
      double *u = (double*) Malloc(2*sizeof(double)*p*ncols);
      if (nargout <= 2) {
	dcomplexLU(nrows,ncols,l,u,(double*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
      } else if (nargout == 3) {
	double *piv = (double*) Malloc(sizeof(double)*nrows*nrows);
	dcomplexLUP(nrows,ncols,l,u,piv,(double*) A.getReadWriteDataPointer());
	retval.push_back(Array(A.getDataClass(),Dimensions(nrows,p),l));
	retval.push_back(Array(A.getDataClass(),Dimensions(p,ncols),u));
	retval.push_back(Array(FM_DOUBLE,Dimensions(nrows,nrows),piv));
      }
      return retval;
    }
    }
  }
}

