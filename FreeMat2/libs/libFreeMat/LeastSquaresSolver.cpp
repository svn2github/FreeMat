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

#include "LeastSquaresSolver.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"

#ifdef WIN32
#define snprintf _snprintf
#endif


namespace FreeMat {

#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

/***************************************************************************
 * Least-squares solver for double matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
void doubleSolveLeastSq(Interface* io,int m, int n, int k, double *c,
		      double *a, double *b) {
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE DGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //                   WORK, LWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	DOUBLE PRECISION   A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  DGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  double *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  double *B;
  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  B = (double*) Calloc(Bsize*NRHS*sizeof(double));
  changeStrideDouble(B,Bsize,b,m,m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.

  int *JPVT = (int*) Calloc(N*sizeof(int));

  //*  RCOND   (input) DOUBLE PRECISION
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  double RCOND = getEPS();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  double WORKSIZE;

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*             LWORK >= MAX( MN+3*N+1, 2*MN+NRHS ),
  //*          where MN = min( M, N ).
  //*          The block algorithm requires that:
  //*             LWORK >= MAX( MN+2*N+NB*(N+1), 2*MN+NB*NRHS ),
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines DGEQP3, DTZRZF, STZRQF, DORMQR,
  //*          and DORMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
  LWORK = -1;
  dgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
  	  &RANK, &WORKSIZE, &LWORK, &INFO);
  LWORK = (int) WORKSIZE;
  double *WORK = (double*) Malloc(LWORK*sizeof(double));
  dgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
	  &RANK, WORK, &LWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N) {
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
    }
  } else
    // Problem should be underderemined, rank should be M
    if (RANK < M) {
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
    }
  changeStrideDouble(c,n,B,Bsize,n,k);
  // Free the allocated arrays
  Free(B);
  Free(JPVT);
  Free(WORK);
}

/***************************************************************************
 * Least-squares solver for complex matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
void dcomplexSolveLeastSq(Interface* io,int m, int n, int k, double *c,
			  double *a, double *b) {
  //	SUBROUTINE ZGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //     $                   WORK, LWORK, RWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	DOUBLE PRECISION   RWORK( * )
  //	COMPLEX*16         A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  ZGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) COMPLEX*16 array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  double *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  double *B;
  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  B = (double*) Calloc(Bsize*NRHS*2*sizeof(double));
  changeStrideDouble(B,2*Bsize,b,2*m,2*m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.

  int *JPVT = (int*) Calloc(N*sizeof(int));

  //*  RCOND   (input) DOUBLE PRECISION
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  double RCOND = getEPS();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) COMPLEX*16 array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  double WORKSIZE;

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*            LWORK >= MN + MAX( 2*MN, N+1, MN+NRHS )
  //*          where MN = min(M,N).
  //*          The block algorithm requires that:
  //*            LWORK >= MN + MAX( 2*MN, NB*(N+1), MN+MN*NB, MN+NB*NRHS )
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines ZGEQP3, ZTZRZF, CTZRQF, ZUNMQR,
  //*          and ZUNMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  RWORK   (workspace) DOUBLE PRECISION array, dimension (2*N)

  double *RWORK = (double*) Malloc(2*N*sizeof(double));

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
  LWORK = -1;
  zgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
  	  &RANK, &WORKSIZE, &LWORK, RWORK, &INFO);
  LWORK = (int) WORKSIZE;
  double *WORK = (double*) Malloc(LWORK*sizeof(double));
  zgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
	  &RANK, WORK, &LWORK, RWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N)
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
  } else
    // Problem should be underderemined, rank should be M
    if (RANK < M) {
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
    }
  changeStrideDouble(c,2*n,B,2*Bsize,2*n,k);
  // Free the allocated arrays
  Free(B);
  Free(JPVT);
  Free(WORK);
  Free(RWORK);
}

/***************************************************************************
 * Least-squares solver for float matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
void floatSolveLeastSq(Interface* io,int m, int n, int k, float *c,
		       float *a, float *b) {
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE SGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //                   WORK, LWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	REAL   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	REAL   A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  float *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) REAL array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  float *B;
  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  B = (float*) Calloc(Bsize*NRHS*sizeof(float));
  changeStrideFloat(B,Bsize,b,m,m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.

  int *JPVT = (int*) Calloc(N*sizeof(int));

  //*  RCOND   (input) REAL
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  float RCOND = getFloatEPS();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) REAL array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  float WORKSIZE;

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*             LWORK >= MAX( MN+3*N+1, 2*MN+NRHS ),
  //*          where MN = min( M, N ).
  //*          The block algorithm requires that:
  //*             LWORK >= MAX( MN+2*N+NB*(N+1), 2*MN+NB*NRHS ),
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines DGEQP3, DTZRZF, STZRQF, DORMQR,
  //*          and DORMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
  LWORK = -1;
  sgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
  	  &RANK, &WORKSIZE, &LWORK, &INFO);
  LWORK = (int) WORKSIZE;
  float *WORK = (float*) Malloc(LWORK*sizeof(float));
  sgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
	  &RANK, WORK, &LWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N) {
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
    }
  } else
    // Problem should be underderemined, rank should be M
    if (RANK < M) {
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
    }
  changeStrideFloat(c,n,B,Bsize,n,k);
  // Free the allocated arrays
  Free(B);
  Free(JPVT);
  Free(WORK);
}

/***************************************************************************
 * Least-squares solver for complex matrices
 ***************************************************************************/

/**
 * Solve A * X = B in a least-squares sense, where A is m x n, and B is m x k.
 * C is n x k.
 */
void complexSolveLeastSq(Interface* io,int m, int n, int k, float *c,
			 float *a, float *b) {
  //	SUBROUTINE CGELSY( M, N, NRHS, A, LDA, B, LDB, JPVT, RCOND, RANK,
  //     $                   WORK, LWORK, RWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	INTEGER            INFO, LDA, LDB, LWORK, M, N, NRHS, RANK
  //	REAL   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            JPVT( * )
  //	REAL   RWORK( * )
  //	COMPLEX         A( LDA, * ), B( LDB, * ), WORK( * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  CGELSY computes the minimum-norm solution to a real linear least
  //*  squares problem:
  //*      minimize || A * X - B ||
  //*  using a complete orthogonal factorization of A.  A is an M-by-N
  //*  matrix which may be rank-deficient.
  //*
  //*  Several right hand side vectors b and solution vectors x can be
  //*  handled in a single call; they are stored as the columns of the
  //*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution
  //*  matrix X.
  //*
  //*  The routine first computes a QR factorization with column pivoting:
  //*      A * P = Q * [ R11 R12 ]
  //*                  [  0  R22 ]
  //*  with R11 defined as the largest leading submatrix whose estimated
  //*  condition number is less than 1/RCOND.  The order of R11, RANK,
  //*  is the effective rank of A.
  //*
  //*  Then, R22 is considered to be negligible, and R12 is annihilated
  //*  by orthogonal transformations from the right, arriving at the
  //*  complete orthogonal factorization:
  //*     A * P = Q * [ T11 0 ] * Z
  //*                 [  0  0 ]
  //*  The minimum-norm solution is then
  //*     X = P * Z' [ inv(T11)*Q1'*B ]
  //*                [        0       ]
  //*  where Q1 consists of the first RANK columns of Q.
  //*
  //*  This routine is basically identical to the original xGELSX except
  //*  three differences:
  //*    o The call to the subroutine xGEQPF has been substituted by the
  //*      the call to the subroutine xGEQP3. This subroutine is a Blas-3
  //*      version of the QR factorization with column pivoting.
  //*    o Matrix B (the right hand side) is updated with Blas-3.
  //*    o The permutation of matrix B (the right hand side) is faster and
  //*      more simple.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  M       (input) INTEGER
  //*          The number of rows of the matrix A.  M >= 0.

  int M = m;

  //*  N       (input) INTEGER
  //*          The number of columns of the matrix A.  N >= 0.

  int N = n;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of
  //*          columns of matrices B and X. NRHS >= 0.

  int NRHS = k;

  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the M-by-N matrix A.
  //*          On exit, A has been overwritten by details of its
  //*          complete orthogonal factorization.

  float *A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,M).

  int LDA = m;

  //*  B       (input/output) REAL array, dimension (LDB,NRHS)
  //*          On entry, the M-by-NRHS right hand side matrix B.
  //*          On exit, the N-by-NRHS solution matrix X.

  float *B;
  int Bsize = (M > N) ? M : N;
  // This passing convention requires that we copy our source matrix
  // into the destination array with the appropriate padding.
  B = (float*) Calloc(Bsize*NRHS*2*sizeof(float));
  changeStrideFloat(B,2*Bsize,b,2*m,2*m,NRHS);

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B. LDB >= max(1,M,N).

  int LDB = Bsize;

  //*  JPVT    (input/output) INTEGER array, dimension (N)
  //*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
  //*          to the front of AP, otherwise column i is a Free column.
  //*          On exit, if JPVT(i) = k, then the i-th column of AP
  //*          was the k-th column of A.

  int *JPVT = (int*) Calloc(N*sizeof(int));

  //*  RCOND   (input) REAL
  //*          RCOND is used to determine the effective rank of A, which
  //*          is defined as the order of the largest leading triangular
  //*          submatrix R11 in the QR factorization with pivoting of A,
  //*          whose estimated condition number < 1/RCOND.

  float RCOND = getFloatEPS();
  
  //*  RANK    (output) INTEGER
  //*          The effective rank of A, i.e., the order of the submatrix
  //*          R11.  This is the same as the order of the submatrix T11
  //*          in the complete orthogonal factorization of A.

  int RANK;

  //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
  //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

  float WORKSIZE;

  //*  LWORK   (input) INTEGER
  //*          The dimension of the array WORK.
  //*          The unblocked strategy requires that:
  //*            LWORK >= MN + MAX( 2*MN, N+1, MN+NRHS )
  //*          where MN = min(M,N).
  //*          The block algorithm requires that:
  //*            LWORK >= MN + MAX( 2*MN, NB*(N+1), MN+MN*NB, MN+NB*NRHS )
  //*          where NB is an upper bound on the blocksize returned
  //*          by ILAENV for the routines ZGEQP3, ZTZRZF, CTZRQF, ZUNMQR,
  //*          and ZUNMRZ.
  //*
  //*          If LWORK = -1, then a workspace query is assumed; the routine
  //*          only calculates the optimal size of the WORK array, returns
  //*          this value as the first entry of the WORK array, and no error
  //*          message related to LWORK is issued by XERBLA.

  int LWORK;

  //*  RWORK   (workspace) REAL array, dimension (2*N)

  float *RWORK = (float*) Malloc(2*N*sizeof(float));

  //*  INFO    (output) INTEGER
  //*          = 0: successful exit
  //*          < 0: If INFO = -i, the i-th argument had an illegal value.

  int INFO;

  //*  Further Details
  //*  ===============
  //*
  //*  Based on contributions by
  //*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA
  //*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain
  //*
  //*  =====================================================================
  //
  LWORK = -1;
  cgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
  	  &RANK, &WORKSIZE, &LWORK, RWORK, &INFO);
  LWORK = (int) WORKSIZE;
  float *WORK = (float*) Malloc(LWORK*sizeof(float));
  cgelsy_(&M, &N, &NRHS, A, &LDA, B, &LDB, JPVT, &RCOND,
	  &RANK, WORK, &LWORK, RWORK, &INFO);
  // Check the rank...
  if (M > N) {
    // Problem should be overdetermined, rank should be N
    if (RANK < N) {
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
    }
  } else
    // Problem should be underderemined, rank should be M
    if (RANK < M) {
      snprintf(msgBuffer,MSGBUFLEN,"Matrix is rank deficient to machine precision.  RANK = %d\n",RANK);
      io->warningMessage(msgBuffer);
    }
  changeStrideFloat(c,2*n,B,2*Bsize,2*n,k);
  // Free the allocated arrays
  Free(B);
  Free(JPVT);
  Free(WORK);
  Free(RWORK);
}

}
