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

#include "SingularValueDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"

namespace FreeMat {

  void floatSVD(const int nrows, const int ncols, float *u, float *vt, 
		float *s, float *a, bool compact, bool vectors) {
    //      SUBROUTINE SGESDD( JOBZ, M, N, A, LDA, S, U, LDU, VT, LDVT, WORK,
    //     $                   LWORK, IWORK, INFO )
    //*
    //*  -- LAPACK driver routine (version 3.0) --
    //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
    //*     Courant Institute, Argonne National Lab, and Rice University
    //*     October 31, 1999
    //*
    //*     .. Scalar Arguments ..
    //      CHARACTER          JOBZ
    //      INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
    //*     ..
    //*     .. Array Arguments ..
    //      INTEGER            IWORK( * )
    //      REAL               A( LDA, * ), S( * ), U( LDU, * ),
    //     $                   VT( LDVT, * ), WORK( * )
    //*     ..
    //*
    //*  Purpose
    //*  =======
    //*
    //*  SGESDD computes the singular value decomposition (SVD) of a real
    //*  M-by-N matrix A, optionally computing the left and right singular
    //*  vectors.  If singular vectors are desired, it uses a
    //*  divide-and-conquer algorithm.
    //*
    //*  The SVD is written
    //*
    //*       A = U * SIGMA * transpose(V)
    //*
    //*  where SIGMA is an M-by-N matrix which is zero except for its
    //*  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
    //*  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
    //*  are the singular values of A; they are real and non-negative, and
    //*  are returned in descending order.  The first min(m,n) columns of
    //*  U and V are the left and right singular vectors of A.
    //*
    //*  Note that the routine returns VT = V**T, not V.
    //*
    //*  The divide and conquer algorithm makes very mild assumptions about
    //*  floating point arithmetic. It will work on machines with a guard
    //*  digit in add/subtract, or on those binary machines without guard
    //*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
    //*  Cray-2. It could conceivably fail on hexadecimal or decimal machines
    //*  without guard digits, but we know of none.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  JOBZ    (input) CHARACTER*1
    //*          Specifies options for computing all or part of the matrix U:
    //*          = 'A':  all M columns of U and all N rows of V**T are
    //*                  returned in the arrays U and VT;
    //*          = 'S':  the first min(M,N) columns of U and the first
    //*                  min(M,N) rows of V**T are returned in the arrays U
    //*                  and VT;
    //*          = 'O':  If M >= N, the first N columns of U are overwritten
    //*                  on the array A and all rows of V**T are returned in
    //*                  the array VT;
    //*                  otherwise, all columns of U are returned in the
    //*                  array U and the first M rows of V**T are overwritten
    //*                  in the array VT;
    //*          = 'N':  no columns of U or rows of V**T are computed.
    //*
    char JOBZ;
    if (!vectors)
      JOBZ = 'N';
    else {
      if (!compact)
	JOBZ = 'A';
      else
	JOBZ = 'S';
    }
    //*  M       (input) INTEGER
    //*          The number of rows of the input matrix A.  M >= 0.
    //*
    int M = nrows;
    //*  N       (input) INTEGER
    //*          The number of columns of the input matrix A.  N >= 0.
    //*
    int N = ncols;
    //*  A       (input/output) REAL array, dimension (LDA,N)
    //*          On entry, the M-by-N matrix A.
    //*          On exit,
    //*          if JOBZ = 'O',  A is overwritten with the first N columns
    //*                          of U (the left singular vectors, stored
    //*                          columnwise) if M >= N;
    //*                          A is overwritten with the first M rows
    //*                          of V**T (the right singular vectors, stored
    //*                          rowwise) otherwise.
    //*          if JOBZ .ne. 'O', the contents of A are destroyed.
    //*
    float *A;
    A = a;
    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,M).
    //*
    int LDA = nrows;
    //*  S       (output) REAL array, dimension (min(M,N))
    //*          The singular values of A, sorted so that S(i) >= S(i+1).
    //*
    float *S;
    S = s;
    //*  U       (output) REAL array, dimension (LDU,UCOL)
    //*          UCOL = M if JOBZ = 'A' or JOBZ = 'O' and M < N;
    //*          UCOL = min(M,N) if JOBZ = 'S'.
    //*          If JOBZ = 'A' or JOBZ = 'O' and M < N, U contains the M-by-M
    //*          orthogonal matrix U;
    //*          if JOBZ = 'S', U contains the first min(M,N) columns of U
    //*          (the left singular vectors, stored columnwise);
    //*          if JOBZ = 'O' and M >= N, or JOBZ = 'N', U is not referenced.
    //*
    float *U;
    U = u;
    //*  LDU     (input) INTEGER
    //*          The leading dimension of the array U.  LDU >= 1; if
    //*          JOBZ = 'S' or 'A' or JOBZ = 'O' and M < N, LDU >= M.
    //*
    int LDU = nrows;
    //*  VT      (output) REAL array, dimension (LDVT,N)
    //*          If JOBZ = 'A' or JOBZ = 'O' and M >= N, VT contains the
    //*          N-by-N orthogonal matrix V**T;
    //*          if JOBZ = 'S', VT contains the first min(M,N) rows of
    //*          V**T (the right singular vectors, stored rowwise);
    //*          if JOBZ = 'O' and M < N, or JOBZ = 'N', VT is not referenced.
    //*
    float *VT;
    VT = vt;
    //*  LDVT    (input) INTEGER
    //*          The leading dimension of the array VT.  LDVT >= 1; if
    //*          JOBZ = 'A' or JOBZ = 'O' and M >= N, LDVT >= N;
    //*          if JOBZ = 'S', LDVT >= min(M,N).
    //*
    int LDVT;
    if (!compact)
      LDVT = ncols;
    else
      LDVT = (ncols < nrows) ? ncols : nrows;
    //*  WORK    (workspace/output) REAL array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK;
    //*
    float *WORK;
    //*  LWORK   (input) INTEGER
    //*          The dimension of the array WORK. LWORK >= 1.
    //*          If JOBZ = 'N',
    //*            LWORK >= 3*min(M,N) + max(max(M,N),6*min(M,N)).
    //*          If JOBZ = 'O',
    //*            LWORK >= 3*min(M,N)*min(M,N) + 
    //*                     max(max(M,N),5*min(M,N)*min(M,N)+4*min(M,N)).
    //*          If JOBZ = 'S' or 'A'
    //*            LWORK >= 3*min(M,N)*min(M,N) +
    //*                     max(max(M,N),4*min(M,N)*min(M,N)+4*min(M,N)).
    //*          For good performance, LWORK should generally be larger.
    //*          If LWORK < 0 but other input arguments are legal, WORK(1)
    //*          returns the optimal LWORK.
    //*
    int LWORK;
    //*  IWORK   (workspace) INTEGER array, dimension (8*min(M,N))
    //*
    int *IWORK;
    int minMN;
    minMN = (M < N) ? M : N;
    IWORK = (int*) Malloc(8*minMN*sizeof(int));
    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit.
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
    //*          > 0:  SBDSDC did not converge, updating process failed.
    //*
    int INFO;
    float WORKSZE;
    LWORK = -1;
    sgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     &WORKSZE, &LWORK, IWORK,&INFO);
    LWORK = (int) WORKSZE;
    WORK = (float*) Malloc(LWORK*sizeof(float));
    sgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     WORK, &LWORK, IWORK,&INFO);
    Free(WORK);
    Free(IWORK);
  }

  void doubleSVD(const int nrows, const int ncols, double *u, double *vt,
		 double *s, double *a, bool compact, bool vectors) {
    char JOBZ;
    if (!vectors)
      JOBZ = 'N';
    else {
      if (!compact)
	JOBZ = 'A';
      else
	JOBZ = 'S';
    }
    int M = nrows;
    int N = ncols;
    double *A;
    A = a;
    int LDA = nrows;
    double *S;
    S = s;
    double *U;
    U = u;
    int LDU = nrows;
    double *VT;
    VT = vt;
    int LDVT;
    if (!compact)
      LDVT = ncols;
    else
      LDVT = (ncols < nrows) ? ncols : nrows;
    double *WORK;
    int LWORK;
    int *IWORK;
    int minMN;
    minMN = (M < N) ? M : N;
    IWORK = (int*) Malloc(8*minMN*sizeof(int));
    int INFO;
    double WORKSZE;
    LWORK = -1;
    dgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     &WORKSZE, &LWORK, IWORK,&INFO);
    LWORK = (int) WORKSZE;
    WORK = (double*) Malloc(LWORK*sizeof(double));
    dgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     WORK, &LWORK, IWORK,&INFO);
    Free(WORK);
    Free(IWORK);
  }

  void complexSVD(const int nrows, const int ncols, float *u, float *vt, 
		  float *s, float *a, bool compact, bool vectors) {
    //       SUBROUTINE CGESDD( JOBZ, M, N, A, LDA, S, U, LDU, VT, LDVT, WORK,
    //      $                   LWORK, RWORK, IWORK, INFO )
    // *
    // *  -- LAPACK driver routine (version 3.0) --
    // *     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
    // *     Courant Institute, Argonne National Lab, and Rice University
    // *     October 31, 1999
    // *
    // *     .. Scalar Arguments ..
    //       CHARACTER          JOBZ
    //       INTEGER            INFO, LDA, LDU, LDVT, LWORK, M, N
    // *     ..
    // *     .. Array Arguments ..
    //       INTEGER            IWORK( * )
    //       REAL               RWORK( * ), S( * )
    //       COMPLEX            A( LDA, * ), U( LDU, * ), VT( LDVT, * ),
    //      $                   WORK( * )
    // *     ..
    // *
    // *  Purpose
    // *  =======
    // *
    // *  CGESDD computes the singular value decomposition (SVD) of a complex
    // *  M-by-N matrix A, optionally computing the left and/or right singular
    // *  vectors, by using divide-and-conquer method. The SVD is written
    // *
    // *       A = U * SIGMA * conjugate-transpose(V)
    // *
    // *  where SIGMA is an M-by-N matrix which is zero except for its
    // *  min(m,n) diagonal elements, U is an M-by-M unitary matrix, and
    // *  V is an N-by-N unitary matrix.  The diagonal elements of SIGMA
    // *  are the singular values of A; they are real and non-negative, and
    // *  are returned in descending order.  The first min(m,n) columns of
    // *  U and V are the left and right singular vectors of A.
    // *
    // *  Note that the routine returns VT = V**H, not V.
    // *
    // *  The divide and conquer algorithm makes very mild assumptions about
    // *  floating point arithmetic. It will work on machines with a guard
    // *  digit in add/subtract, or on those binary machines without guard
    // *  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
    // *  Cray-2. It could conceivably fail on hexadecimal or decimal machines
    // *  without guard digits, but we know of none.
    // *
    // *  Arguments
    // *  =========
    // *
    // *  JOBZ    (input) CHARACTER*1
    // *          Specifies options for computing all or part of the matrix U:
    // *          = 'A':  all M columns of U and all N rows of V**H are
    // *                  returned in the arrays U and VT;
    // *          = 'S':  the first min(M,N) columns of U and the first
    // *                  min(M,N) rows of V**H are returned in the arrays U
    // *                  and VT;
    // *          = 'O':  If M >= N, the first N columns of U are overwritten
    // *                  on the array A and all rows of V**H are returned in
    // *                  the array VT;
    // *                  otherwise, all columns of U are returned in the
    // *                  array U and the first M rows of V**H are overwritten
    // *                  in the array VT;
    // *          = 'N':  no columns of U or rows of V**H are computed.
    // *
    char JOBZ;
    if (!vectors)
      JOBZ = 'N';
    else {
      if (!compact)
	JOBZ = 'A';
      else
	JOBZ = 'S';
    }
    // *  M       (input) INTEGER
    // *          The number of rows of the input matrix A.  M >= 0.
    // *
    int M = nrows;
    // *  N       (input) INTEGER
    // *          The number of columns of the input matrix A.  N >= 0.
    // *
    int N = ncols;
    // *  A       (input/output) COMPLEX array, dimension (LDA,N)
    // *          On entry, the M-by-N matrix A.
    // *          On exit,
    // *          if JOBZ = 'O',  A is overwritten with the first N columns
    // *                          of U (the left singular vectors, stored
    // *                          columnwise) if M >= N;
    // *                          A is overwritten with the first M rows
    // *                          of V**H (the right singular vectors, stored
    // *                          rowwise) otherwise.
    // *          if JOBZ .ne. 'O', the contents of A are destroyed.
    // *
    float *A;
    A = a;
    // *  LDA     (input) INTEGER
    // *          The leading dimension of the array A.  LDA >= max(1,M).
    // *
    int LDA = nrows;    
    // *  S       (output) REAL array, dimension (min(M,N))
    // *          The singular values of A, sorted so that S(i) >= S(i+1).
    // *
    float *S;
    S = s;    
    // *  U       (output) COMPLEX array, dimension (LDU,UCOL)
    // *          UCOL = M if JOBZ = 'A' or JOBZ = 'O' and M < N;
    // *          UCOL = min(M,N) if JOBZ = 'S'.
    // *          If JOBZ = 'A' or JOBZ = 'O' and M < N, U contains the M-by-M
    // *          unitary matrix U;
    // *          if JOBZ = 'S', U contains the first min(M,N) columns of U
    // *          (the left singular vectors, stored columnwise);
    // *          if JOBZ = 'O' and M >= N, or JOBZ = 'N', U is not referenced.
    // *
    float *U;
    U = u;    
    // *  LDU     (input) INTEGER
    // *          The leading dimension of the array U.  LDU >= 1; if
    // *          JOBZ = 'S' or 'A' or JOBZ = 'O' and M < N, LDU >= M.
    // *
    int LDU = nrows;    
    // *  VT      (output) COMPLEX array, dimension (LDVT,N)
    // *          If JOBZ = 'A' or JOBZ = 'O' and M >= N, VT contains the
    // *          N-by-N unitary matrix V**H;
    // *          if JOBZ = 'S', VT contains the first min(M,N) rows of
    // *          V**H (the right singular vectors, stored rowwise);
    // *          if JOBZ = 'O' and M < N, or JOBZ = 'N', VT is not referenced.
    // *
    float *VT;
    VT = vt;
    // *  LDVT    (input) INTEGER
    // *          The leading dimension of the array VT.  LDVT >= 1; if
    // *          JOBZ = 'A' or JOBZ = 'O' and M >= N, LDVT >= N;
    // *          if JOBZ = 'S', LDVT >= min(M,N).
    // *
    int LDVT;
    if (!compact)
      LDVT = ncols;
    else
      LDVT = (ncols < nrows) ? ncols : nrows;
    // *  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
    // *          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
    // *
    float *WORK;
    // *  LWORK   (input) INTEGER
    // *          The dimension of the array WORK. LWORK >= 1.
    // *          if JOBZ = 'N', LWORK >= 2*min(M,N)+max(M,N).
    // *          if JOBZ = 'O',
    // *                LWORK >= 2*min(M,N)*min(M,N)+2*min(M,N)+max(M,N).
    // *          if JOBZ = 'S' or 'A',
    // *                LWORK >= min(M,N)*min(M,N)+2*min(M,N)+max(M,N).
    // *          For good performance, LWORK should generally be larger.
    // *          If LWORK < 0 but other input arguments are legal, WORK(1)
    // *          returns the optimal LWORK.
    // *
    int LWORK;
    // *  RWORK   (workspace) REAL array, dimension (LRWORK)
    // *          If JOBZ = 'N', LRWORK >= 7*min(M,N).
    // *          Otherwise, LRWORK >= 5*min(M,N)*min(M,N) + 5*min(M,N)
    // *
    int minMN;
    minMN = (M < N) ? M : N;
    float *RWORK;
    RWORK = (float *) Malloc((5*minMN*minMN+5*minMN)*sizeof(float));
    // *  IWORK   (workspace) INTEGER array, dimension (8*min(M,N))
    // *
    int *IWORK;
    IWORK = (int*) Malloc(8*minMN*sizeof(int));
    // *  INFO    (output) INTEGER
    // *          = 0:  successful exit.
    // *          < 0:  if INFO = -i, the i-th argument had an illegal value.
    // *          > 0:  The updating process of SBDSDC did not converge.
    int INFO;
    float WORKSZE[2];
    LWORK = -1;
    cgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     WORKSZE, &LWORK, RWORK, IWORK, &INFO);
    LWORK = (int) WORKSZE[0];
    WORK = (float*) Malloc(2*LWORK*sizeof(float));
    cgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     WORK, &LWORK, RWORK, IWORK,&INFO);
    Free(RWORK);
    Free(WORK);
    Free(IWORK);
  }

  void dcomplexSVD(const int nrows, const int ncols, double *u, double *vt, 
		   double *s, double *a, bool compact, bool vectors) {
    char JOBZ;
    if (!vectors)
      JOBZ = 'N';
    else {
      if (!compact)
	JOBZ = 'A';
      else
	JOBZ = 'S';
    }
    int M = nrows;
    int N = ncols;
    double *A;
    A = a;
    int LDA = nrows;    
    double *S;
    S = s;    
    double *U;
    U = u;    
    int LDU = nrows;    
    double *VT;
    VT = vt; 
    int LDVT;
    if (!compact)
      LDVT = ncols;
    else
      LDVT = (ncols < nrows) ? ncols : nrows;
    double *WORK;
    int LWORK;
    int minMN;
    minMN = (M < N) ? M : N;
    double *RWORK;
    RWORK = (double *) Malloc((5*minMN*minMN+5*minMN)*sizeof(double));
    int *IWORK;
    IWORK = (int*) Malloc(8*minMN*sizeof(int));
    int INFO;
    double WORKSZE[2];
    LWORK = -1;
    zgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     WORKSZE, &LWORK, RWORK, IWORK, &INFO);
    LWORK = (int) WORKSZE[0];
    WORK = (double*) Malloc(2*LWORK*sizeof(double));
    zgesdd_( &JOBZ, &M, &N, A, &LDA, S, U, &LDU, VT, &LDVT, 
	     WORK, &LWORK, RWORK, IWORK,&INFO);
    Free(RWORK);
    Free(WORK);
    Free(IWORK);
  }
}
