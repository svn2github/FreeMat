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

#ifndef __FMLAPACK_hpp__
#define __FMLAPACK_hpp__

#include <string.h>

// Lapack routines used are declared here
extern "C" {
  double dlamch_(char * CMACH);
  
  double slamch_(char * CMACH);
  
  void dgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       double *A, int * LDA, double * AF, int * LDAF, 
	       int * IPIV, char * EQUED, double * R, double * C, 
	       double * B, int * LDB, double * X, int * LDX, 
	       double * RCOND, double * FERR, double * BERR,
	       double * WORK, int * IWORK, int * INFO);

  void zgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       double *A, int * LDA, double * AF, int * LDAF, 
	       int * IPIV, char * EQUED, double * R, double * C, 
	       double * B, int * LDB, double * X, int * LDX, 
	       double * RCOND, double * FERR, double * BERR,
	       double * WORK, double * RWORK, int * INFO);

  void sgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       float *A, int * LDA, float * AF, int * LDAF, 
	       int * IPIV, char * EQUED, float * R, float * C, 
	       float * B, int * LDB, float * X, int * LDX, 
	       float * RCOND, float * FERR, float * BERR,
	       float * WORK, int * IWORK, int * INFO);

  void cgesvx_(char* FACT, char* TRANS, int * N, int * NRHS, 
	       float *A, int * LDA, float * AF, int * LDAF, 
	       int * IPIV, char * EQUED, float * R, float * C, 
	       float * B, int * LDB, float * X, int * LDX, 
	       float * RCOND, float * FERR, float * BERR,
	       float * WORK, float * RWORK, int * INFO);

  void dgelsy_(int* M, int *N, int *NRHS, double* A, int *LDA,
	       double *B, int *LDB, int *JPVT, double* RCOND,
	       int *RANK, double *WORK, int* LWORK, int* INFO);

  void zgelsy_(int* M, int *N, int *NRHS, double* A, int *LDA,
	       double *B, int *LDB, int *JPVT, double* RCOND,
	       int *RANK, double *WORK, int* LWORK, double* RWORK,
	       int* INFO);

  void sgelsy_(int* M, int *N, int *NRHS, float* A, int *LDA,
	       float *B, int *LDB, int *JPVT, float* RCOND,
	       int *RANK, float *WORK, int* LWORK, int* INFO);

  void cgelsy_(int* M, int *N, int *NRHS, float* A, int *LDA,
	       float *B, int *LDB, int *JPVT, float* RCOND,
	       int *RANK, float *WORK, int* LWORK, float* RWORK,
	       int* INFO);

  void sgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, float* A, int* LDA, float* WR, float* WI,
	       float* VL, int *LDVL, float* VR, int *LDVR, int *ILO,
	       int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	       float* RCONDV, float *WORK, int *LWORK, int *IWORK,
	       int *INFO);

  void dgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, double* A, int* LDA, double* WR, double* WI,
	       double* VL, int *LDVL, double* VR, int *LDVR, int *ILO,
	       int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	       double* RCONDV, double *WORK, int *LWORK, int *IWORK,
	       int *INFO);

  void cgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, float* A, int* LDA, float* W, float* VL, 
	       int *LDVL, float* VR, int *LDVR, int *ILO,
	       int *IHI, float* SCALE, float* ABNRM, float* RCONDE,
	       float* RCONDV, float *WORK, int *LWORK, float *RWORK,
	       int *INFO);

  void zgeevx_(char* BALANC, char* JOBVL, char* JOBVR, char* SENSE, 
	       int* N, double* A, int* LDA, double* W, double* VL, 
	       int *LDVL, double* VR, int *LDVR, int *ILO,
	       int *IHI, double* SCALE, double* ABNRM, double* RCONDE,
	       double* RCONDV, double *WORK, int *LWORK, double *RWORK,
	       int *INFO);

//   void sgecon_(char* NORM, int* N, float *A, int *LDA, float *ANORM,
// 	       float *RCOND, float *WORK, int* IWORK, int *INFO);
}

namespace FreeMat {
  /**
   * Call the LAPACK slamch function to get the $$\epsilon$$ value for
   * a floating point number.
   */
  double getEPS();
  /**
   * Call the LAPACK dlamch function to get the $$\epsilon$$ value for
   * a double-precision floating point number.
   */
  float getFloatEPS();
  /**
   * This function effectively pads each column in the source matrix by a
   * number of zeros.  It actually allows us to change the stride of the
   * matrix (amount that must be added to an index to move from one column
   * to the next).  
   */
  void changeStrideDouble(double*dst, int dstStride, double*src, int srcStride, int rowCount, int colCount);
  /**
   * This function effectively pads each column in the source matrix by a
   * number of zeros.  It actually allows us to change the stride of the
   * matrix (amount that must be added to an index to move from one column
   * to the next).  
   */
  void changeStrideFloat(float*dst, int dstStride, float*src, int srcStride, int rowCount, int colCount);

}

#endif
