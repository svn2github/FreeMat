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

#include "LinearEqSolver.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Malloc.hpp"

#ifdef WIN32
#define snprintf _snprintf
#endif

#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

/***************************************************************************
 * Linear equation solver for real matrices
 ***************************************************************************/

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are real.
void doubleSolveLinEq(Interpreter* eval, int m, int n, double *c, double* a, double *b) {
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE DGESVX( FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV,
  //$                   EQUED, R, C, B, LDB, X, LDX, RCOND, FERR, BERR,
  //$                   WORK, IWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	CHARACTER          EQUED, FACT, TRANS
  //	INTEGER            INFO, LDA, LDAF, LDB, LDX, N, NRHS
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            IPIV( * ), IWORK( * )
  //	DOUBLE PRECISION   A( LDA, * ), AF( LDAF, * ), B( LDB, * ),
  //     $                   BERR( * ), C( * ), FERR( * ), R( * ),
  //     $                   WORK( * ), X( LDX, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  DGESVX uses the LU factorization to compute the solution to a real
  //*  system of linear equations
  //*     A * X = B,
  //*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
  //*
  //*  Error bounds on the solution and a condition estimate are also
  //*  provided.
  //*
  //*  Description
  //*  ===========
  //*
  //*  The following steps are performed:
  //*
  //*  1. If FACT = 'E', real scaling factors are computed to equilibrate
  //*     the system:
  //*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B
  //*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B
  //*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B
  //*     Whether or not the system will be equilibrated depends on the
  //*     scaling of the matrix A, but if equilibration is used, A is
  //*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N')
  //*     or diag(C)*B (if TRANS = 'T' or 'C').
  //*
  //*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the
  //*     matrix A (after equilibration if FACT = 'E') as
  //*        A = P * L * U,
  //*     where P is a permutation matrix, L is a unit lower triangular
  //*     matrix, and U is upper triangular.
  //*
  //*  3. If some U(i,i)=0, so that U is exactly singular, then the routine
  //*     returns with INFO = i. Otherwise, the factored form of A is used
  //*     to estimate the condition number of the matrix A.  If the
  //*     reciprocal of the condition number is less than machine precision,
  //*     INFO = N+1 is returned as a warning, but the routine still goes on
  //*     to solve for X and compute error bounds as described below.
  //*
  //*  4. The system of equations is solved for X using the factored form
  //*     of A.
  //*
  //*  5. Iterative refinement is applied to improve the computed solution
  //*     matrix and calculate error bounds and backward error estimates
  //*     for it.
  //*
  //*  6. If equilibration was used, the matrix X is premultiplied by
  //*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so
  //*     that it solves the original system before equilibration.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  FACT    (input) CHARACTER*1
  //*          Specifies whether or not the factored form of the matrix A is
  //*          supplied on entry, and if not, whether the matrix A should be
  //*          equilibrated before it is factored.
  //*          = 'F':  On entry, AF and IPIV contain the factored form of A.
  //*                  If EQUED is not 'N', the matrix A has been
  //*                  equilibrated with scaling factors given by R and C.
  //*                  A, AF, and IPIV are not modified.
  //*          = 'N':  The matrix A will be copied to AF and factored.
  //*          = 'E':  The matrix A will be equilibrated if necessary, then
  //*                  copied to AF and factored.

  char FACT = 'E';

  //*
  //*  TRANS   (input) CHARACTER*1
  //*          Specifies the form of the system of equations:
  //*          = 'N':  A * X = B     (No transpose)
  //*          = 'T':  A**T * X = B  (Transpose)
  //*          = 'C':  A**H * X = B  (Transpose)

  char TRANS = 'N';

  //*
  //*  N       (input) INTEGER
  //*          The number of linear equations, i.e., the order of the
  //*          matrix A.  N >= 0.
  //*

  int N = m;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of columns
  //*          of the matrices B and X.  NRHS >= 0.

  int NRHS = n;
  
  //*
  //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
  //*          On entry, the N-by-N matrix A.  If FACT = 'F' and EQUED is
  //*          not 'N', then A must have been equilibrated by the scaling
  //*          factors in R and/or C.  A is not modified if FACT = 'F' or
  //*          'N', or if FACT = 'E' and EQUED = 'N' on exit.
  //*
  //*          On exit, if EQUED .ne. 'N', A is scaled as follows:
  //*          EQUED = 'R':  A := diag(R) * A
  //*          EQUED = 'C':  A := A * diag(C)
  //*          EQUED = 'B':  A := diag(R) * A * diag(C).
  
  double* A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,N).
  //*

  int LDA = m;

  //*  AF      (input or output) DOUBLE PRECISION array, dimension (LDAF,N)
  //*          If FACT = 'F', then AF is an input argument and on entry
  //*          contains the factors L and U from the factorization
  //*          A = P*L*U as computed by DGETRF.  If EQUED .ne. 'N', then
  //*          AF is the factored form of the equilibrated matrix A.
  //*
  //*          If FACT = 'N', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the equilibrated matrix A (see the description of A for
  //*          the form of the equilibrated matrix).

  double *AF = (double*) Malloc(sizeof(double)*LDA*N);

  //*  LDAF    (input) INTEGER
  //*          The leading dimension of the array AF.  LDAF >= max(1,N).
  //*

  int LDAF = m;

  //*  IPIV    (input or output) INTEGER array, dimension (N)
  //*          If FACT = 'F', then IPIV is an input argument and on entry
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          as computed by DGETRF; row i of the matrix was interchanged
  //*          with row IPIV(i).
  //*
  //*          If FACT = 'N', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the equilibrated matrix A.

  int *IPIV = (int*) Malloc(sizeof(int)*N);

  //*  EQUED   (input or output) CHARACTER*1
  //*          Specifies the form of equilibration that was done.
  //*          = 'N':  No equilibration (always true if FACT = 'N').
  //*          = 'R':  Row equilibration, i.e., A has been premultiplied by
  //*                  diag(R).
  //*          = 'C':  Column equilibration, i.e., A has been postmultiplied
  //*                  by diag(C).
  //*          = 'B':  Both row and column equilibration, i.e., A has been
  //*                  replaced by diag(R) * A * diag(C).
  //*          EQUED is an input argument if FACT = 'F'; otherwise, it is an
  //*          output argument.
  
  char EQUED;

  //*  R       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The row scale factors for A.  If EQUED = 'R' or 'B', A is
  //*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R
  //*          is not accessed.  R is an input argument if FACT = 'F';
  //*          otherwise, R is an output argument.  If FACT = 'F' and
  //*          EQUED = 'R' or 'B', each element of R must be positive.

  double *R = (double*) Malloc(sizeof(double)*N);

  //*  C       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The column scale factors for A.  If EQUED = 'C' or 'B', A is
  //*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C
  //*          is not accessed.  C is an input argument if FACT = 'F';
  //*          otherwise, C is an output argument.  If FACT = 'F' and
  //*          EQUED = 'C' or 'B', each element of C must be positive.
  
  double *C = (double*) Malloc(sizeof(double)*N);

  //*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS)
  //*          On entry, the N-by-NRHS right hand side matrix B.
  //*          On exit,
  //*          if EQUED = 'N', B is not modified;
  //*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by
  //*          diag(R)*B;
  //*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is
  //*          overwritten by diag(C)*B.

  double *B = b;

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B.  LDB >= max(1,N).

  int LDB = m;

  //*  X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS)
  //*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X
  //*          to the original system of equations.  Note that A and B are
  //*          modified on exit if EQUED .ne. 'N', and the solution to the
  //*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and
  //*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C'
  //*          and EQUED = 'R' or 'B'.
  
  double *X = c;

  //*  LDX     (input) INTEGER
  //*          The leading dimension of the array X.  LDX >= max(1,N).
  
  int LDX = m;

  //*  RCOND   (output) DOUBLE PRECISION
  //*          The estimate of the reciprocal condition number of the matrix
  //*          A after equilibration (if done).  If RCOND is less than the
  //*          machine precision (in particular, if RCOND = 0), the matrix
  //*          is singular to working precision.  This condition is
  //*          indicated by a return code of INFO > 0.

  double RCOND;
  
  //*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The estimated forward error bound for each solution vector
  //*          X(j) (the j-th column of the solution matrix X).
  //*          If XTRUE is the true solution corresponding to X(j), FERR(j)
  //*          is an estimated upper bound for the magnitude of the largest
  //*          element in (X(j) - XTRUE) divided by the magnitude of the
  //*          largest element in X(j).  The estimate is as reliable as
  //*          the estimate for RCOND, and is almost always a slight
  //*          overestimate of the true error.

  double *FERR  = (double*) Malloc(n*sizeof(double));

  //*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The componentwise relative backward error of each solution
  //*          vector X(j) (i.e., the smallest relative change in
  //*          any element of A or B that makes X(j) an exact solution).

  double *BERR = (double*) Malloc(n*sizeof(double));

  //*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (4*N)
  //*          On exit, WORK(1) contains the reciprocal pivot growth
  //*          factor norm(A)/norm(U). The "max absolute element" norm is
  //*          used. If WORK(1) is much less than 1, then the stability
  //*          of the LU factorization of the (equilibrated) matrix A
  //*          could be poor. This also means that the solution X, condition
  //*          estimator RCOND, and forward error bound FERR could be
  //*          unreliable. If factorization fails with 0<INFO<=N, then
  //*          WORK(1) contains the reciprocal pivot growth factor for the
  //*          leading INFO columns of A.
 
  double *WORK = (double*) Malloc(4*N*sizeof(double));

  //*  IWORK   (workspace) INTEGER array, dimension (N)

  int *IWORK = (int*) Malloc(4*N*sizeof(int));

  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  //*          > 0:  if INFO = i, and i is
  //*                <= N:  U(i,i) is exactly zero.  The factorization has
  //*                       been completed, but the factor U is exactly
  //*                       singular, so the solution and error bounds
  //*                       could not be computed. RCOND = 0 is returned.
  //*                = N+1: U is nonsingular, but RCOND is less than machine
  //*                       precision, meaning that the matrix is singular
  //*                       to working precision.  Nevertheless, the
  //*                       solution and error bounds are computed because
  //*                       there are a number of situations where the
  //*                       computed solution can be more accurate than the
  //*                       value of RCOND would suggest.
  //*
  //*  =====================================================================

  int INFO;

  dgesvx_(&FACT, &TRANS, &N, &NRHS, A, &LDA, AF, &LDAF, IPIV, &EQUED, R, C, B,
	  &LDB, X, &LDX, &RCOND, FERR, BERR, WORK, IWORK, &INFO);
  if ((INFO == N) || (INFO == N+1) || (RCOND < getEPS())) {
    snprintf(msgBuffer,MSGBUFLEN,"Matrix is singular to working precision.  RCOND = %e\n",RCOND);
    eval->warningMessage(msgBuffer);
  }
  // Free the allocated arrays...
  Free(AF);
  Free(IPIV);
  Free(R);
  Free(C);
  Free(WORK);
  Free(IWORK);
  Free(FERR);
  Free(BERR);
}

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are real.
void dcomplexSolveLinEq(Interpreter* eval, int m, int n, double *c, double* a, double *b) {
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE ZGESVX( FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV,
  //$                   EQUED, R, C, B, LDB, X, LDX, RCOND, FERR, BERR,
  //$                   WORK, IWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	CHARACTER          EQUED, FACT, TRANS
  //	INTEGER            INFO, LDA, LDAF, LDB, LDX, N, NRHS
  //	DOUBLE PRECISION   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //      INTEGER            IPIV( * )
  //      DOUBLE PRECISION   BERR( * ), C( * ), FERR( * ), R( * ),
  //     $                   RWORK( * )
  //      COMPLEX*16         A( LDA, * ), AF( LDAF, * ), B( LDB, * ),
  //     $                   WORK( * ), X( LDX, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  ZGESVX uses the LU factorization to compute the solution to a real
  //*  system of linear equations
  //*     A * X = B,
  //*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
  //*
  //*  Error bounds on the solution and a condition estimate are also
  //*  provided.
  //*
  //*  Description
  //*  ===========
  //*
  //*  The following steps are performed:
  //*
  //*  1. If FACT = 'E', real scaling factors are computed to equilibrate
  //*     the system:
  //*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B
  //*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B
  //*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B
  //*     Whether or not the system will be equilibrated depends on the
  //*     scaling of the matrix A, but if equilibration is used, A is
  //*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N')
  //*     or diag(C)*B (if TRANS = 'T' or 'C').
  //*
  //*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the
  //*     matrix A (after equilibration if FACT = 'E') as
  //*        A = P * L * U,
  //*     where P is a permutation matrix, L is a unit lower triangular
  //*     matrix, and U is upper triangular.
  //*
  //*  3. If some U(i,i)=0, so that U is exactly singular, then the routine
  //*     returns with INFO = i. Otherwise, the factored form of A is used
  //*     to estimate the condition number of the matrix A.  If the
  //*     reciprocal of the condition number is less than machine precision,
  //*     INFO = N+1 is returned as a warning, but the routine still goes on
  //*     to solve for X and compute error bounds as described below.
  //*
  //*  4. The system of equations is solved for X using the factored form
  //*     of A.
  //*
  //*  5. Iterative refinement is applied to improve the computed solution
  //*     matrix and calculate error bounds and backward error estimates
  //*     for it.
  //*
  //*  6. If equilibration was used, the matrix X is premultiplied by
  //*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so
  //*     that it solves the original system before equilibration.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  FACT    (input) CHARACTER*1
  //*          Specifies whether or not the factored form of the matrix A is
  //*          supplied on entry, and if not, whether the matrix A should be
  //*          equilibrated before it is factored.
  //*          = 'F':  On entry, AF and IPIV contain the factored form of A.
  //*                  If EQUED is not 'N', the matrix A has been
  //*                  equilibrated with scaling factors given by R and C.
  //*                  A, AF, and IPIV are not modified.
  //*          = 'N':  The matrix A will be copied to AF and factored.
  //*          = 'E':  The matrix A will be equilibrated if necessary, then
  //*                  copied to AF and factored.

  char FACT = 'E';

  //*
  //*  TRANS   (input) CHARACTER*1
  //*          Specifies the form of the system of equations:
  //*          = 'N':  A * X = B     (No transpose)
  //*          = 'T':  A**T * X = B  (Transpose)
  //*          = 'C':  A**H * X = B  (Transpose)

  char TRANS = 'N';

  //*
  //*  N       (input) INTEGER
  //*          The number of linear equations, i.e., the order of the
  //*          matrix A.  N >= 0.
  //*

  int N = m;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of columns
  //*          of the matrices B and X.  NRHS >= 0.

  int NRHS = n;
  
  //*  A       (input/output) COMPLEX*16 array, dimension (LDA,N)
  //*          On entry, the N-by-N matrix A.  If FACT = 'F' and EQUED is
  //*          not 'N', then A must have been equilibrated by the scaling
  //*          factors in R and/or C.  A is not modified if FACT = 'F' or
  //*          'N', or if FACT = 'E' and EQUED = 'N' on exit.
  //*
  //*          On exit, if EQUED .ne. 'N', A is scaled as follows:
  //*          EQUED = 'R':  A := diag(R) * A
  //*          EQUED = 'C':  A := A * diag(C)
  //*          EQUED = 'B':  A := diag(R) * A * diag(C).
  
  double* A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,N).
  //*

  int LDA = m;

  //*  AF      (input or output) COMPLEX*16 array, dimension (LDAF,N)
  //*          If FACT = 'F', then AF is an input argument and on entry
  //*          contains the factors L and U from the factorization
  //*          A = P*L*U as computed by DGETRF.  If EQUED .ne. 'N', then
  //*          AF is the factored form of the equilibrated matrix A.
  //*
  //*          If FACT = 'N', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the equilibrated matrix A (see the description of A for
  //*          the form of the equilibrated matrix).

  double *AF = (double*) Malloc(2*sizeof(double)*LDA*N);

  //*  LDAF    (input) INTEGER
  //*          The leading dimension of the array AF.  LDAF >= max(1,N).
  //*

  int LDAF = m;

  //*  IPIV    (input or output) INTEGER array, dimension (N)
  //*          If FACT = 'F', then IPIV is an input argument and on entry
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          as computed by DGETRF; row i of the matrix was interchanged
  //*          with row IPIV(i).
  //*
  //*          If FACT = 'N', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the equilibrated matrix A.

  int *IPIV = (int*) Malloc(sizeof(int)*N);

  //*  EQUED   (input or output) CHARACTER*1
  //*          Specifies the form of equilibration that was done.
  //*          = 'N':  No equilibration (always true if FACT = 'N').
  //*          = 'R':  Row equilibration, i.e., A has been premultiplied by
  //*                  diag(R).
  //*          = 'C':  Column equilibration, i.e., A has been postmultiplied
  //*                  by diag(C).
  //*          = 'B':  Both row and column equilibration, i.e., A has been
  //*                  replaced by diag(R) * A * diag(C).
  //*          EQUED is an input argument if FACT = 'F'; otherwise, it is an
  //*          output argument.
  
  char EQUED;

  //*  R       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The row scale factors for A.  If EQUED = 'R' or 'B', A is
  //*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R
  //*          is not accessed.  R is an input argument if FACT = 'F';
  //*          otherwise, R is an output argument.  If FACT = 'F' and
  //*          EQUED = 'R' or 'B', each element of R must be positive.

  double *R = (double*) Malloc(sizeof(double)*N);

  //*  C       (input or output) DOUBLE PRECISION array, dimension (N)
  //*          The column scale factors for A.  If EQUED = 'C' or 'B', A is
  //*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C
  //*          is not accessed.  C is an input argument if FACT = 'F';
  //*          otherwise, C is an output argument.  If FACT = 'F' and
  //*          EQUED = 'C' or 'B', each element of C must be positive.
  
  double *C = (double*) Malloc(sizeof(double)*N);

  //*  B       (input/output) COMPLEX*16 array, dimension (LDB,NRHS)
  //*          On entry, the N-by-NRHS right hand side matrix B.
  //*          On exit,
  //*          if EQUED = 'N', B is not modified;
  //*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by
  //*          diag(R)*B;
  //*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is
  //*          overwritten by diag(C)*B.

  double *B = b;

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B.  LDB >= max(1,N).

  int LDB = m;

  //*  X       (output) COMPLEX*16 array, dimension (LDX,NRHS)
  //*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X
  //*          to the original system of equations.  Note that A and B are
  //*          modified on exit if EQUED .ne. 'N', and the solution to the
  //*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and
  //*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C'
  //*          and EQUED = 'R' or 'B'.
  
  double *X = c;

  //*  LDX     (input) INTEGER
  //*          The leading dimension of the array X.  LDX >= max(1,N).
  
  int LDX = m;

  //*  RCOND   (output) DOUBLE PRECISION
  //*          The estimate of the reciprocal condition number of the matrix
  //*          A after equilibration (if done).  If RCOND is less than the
  //*          machine precision (in particular, if RCOND = 0), the matrix
  //*          is singular to working precision.  This condition is
  //*          indicated by a return code of INFO > 0.

  double RCOND;
  
  //*  FERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The estimated forward error bound for each solution vector
  //*          X(j) (the j-th column of the solution matrix X).
  //*          If XTRUE is the true solution corresponding to X(j), FERR(j)
  //*          is an estimated upper bound for the magnitude of the largest
  //*          element in (X(j) - XTRUE) divided by the magnitude of the
  //*          largest element in X(j).  The estimate is as reliable as
  //*          the estimate for RCOND, and is almost always a slight
  //*          overestimate of the true error.

  double *FERR = (double*) Malloc(n*sizeof(double));

  //*  BERR    (output) DOUBLE PRECISION array, dimension (NRHS)
  //*          The componentwise relative backward error of each solution
  //*          vector X(j) (i.e., the smallest relative change in
  //*          any element of A or B that makes X(j) an exact solution).

  double *BERR = (double*) Malloc(n*sizeof(double));

  //*  WORK    (workspace) COMPLEX*16 array, dimension (2*N)

  double *WORK = (double*) Malloc(2*(2*N)*sizeof(double));
  
  //*  RWORK   (workspace/output) DOUBLE PRECISION array, dimension (2*N)
  //*          On exit, RWORK(1) contains the reciprocal pivot growth
  //*          factor norm(A)/norm(U). The "max absolute element" norm is
  //*          used. If RWORK(1) is much less than 1, then the stability
  //*          of the LU factorization of the (equilibrated) matrix A
  //*          could be poor. This also means that the solution X, condition
  //*          estimator RCOND, and forward error bound FERR could be
  //*          unreliable. If factorization fails with 0<INFO<=N, then
  //*          RWORK(1) contains the reciprocal pivot growth factor for the
  //*          leading INFO columns of A.

  double *RWORK = (double*) Malloc(2*N*sizeof(double));

  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  //*          > 0:  if INFO = i, and i is
  //*                <= N:  U(i,i) is exactly zero.  The factorization has
  //*                       been completed, but the factor U is exactly
  //*                       singular, so the solution and error bounds
  //*                       could not be computed. RCOND = 0 is returned.
  //*                = N+1: U is nonsingular, but RCOND is less than machine
  //*                       precision, meaning that the matrix is singular
  //*                       to working precision.  Nevertheless, the
  //*                       solution and error bounds are computed because
  //*                       there are a number of situations where the
  //*                       computed solution can be more accurate than the
  //*                       value of RCOND would suggest.
  //*
  //*  =====================================================================

  int INFO;

  zgesvx_(&FACT, &TRANS, &N, &NRHS, A, &LDA, AF, &LDAF, IPIV, &EQUED, R, C, B,
	  &LDB, X, &LDX, &RCOND, FERR, BERR, WORK, RWORK, &INFO);
  if ((INFO == N) || (INFO == N+1) || (RCOND < getEPS())) {
    snprintf(msgBuffer,MSGBUFLEN,"Matrix is singular to working precision.  RCOND = %e\n",RCOND);
    eval->warningMessage(msgBuffer);
  }
  // Free the allocated arrays...
  Free(AF);
  Free(IPIV);
  Free(R);
  Free(C);
  Free(WORK);
  Free(RWORK);
  Free(FERR);
  Free(BERR);
}

/***************************************************************************
 * Linear equation solver for float matrices
 ***************************************************************************/

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are real.
void floatSolveLinEq(Interpreter* eval, int m, int n, float *c, float* a, float *b) {
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE SGESVX( FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV,
  //$                   EQUED, R, C, B, LDB, X, LDX, RCOND, FERR, BERR,
  //$                   WORK, IWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	CHARACTER          EQUED, FACT, TRANS
  //	INTEGER            INFO, LDA, LDAF, LDB, LDX, N, NRHS
  //	REAL   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //	INTEGER            IPIV( * ), IWORK( * )
  //	REAL   A( LDA, * ), AF( LDAF, * ), B( LDB, * ),
  //     $                   BERR( * ), C( * ), FERR( * ), R( * ),
  //     $                   WORK( * ), X( LDX, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  SGESVX uses the LU factorization to compute the solution to a real
  //*  system of linear equations
  //*     A * X = B,
  //*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
  //*
  //*  Error bounds on the solution and a condition estimate are also
  //*  provided.
  //*
  //*  Description
  //*  ===========
  //*
  //*  The following steps are performed:
  //*
  //*  1. If FACT = 'E', real scaling factors are computed to equilibrate
  //*     the system:
  //*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B
  //*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B
  //*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B
  //*     Whether or not the system will be equilibrated depends on the
  //*     scaling of the matrix A, but if equilibration is used, A is
  //*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N')
  //*     or diag(C)*B (if TRANS = 'T' or 'C').
  //*
  //*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the
  //*     matrix A (after equilibration if FACT = 'E') as
  //*        A = P * L * U,
  //*     where P is a permutation matrix, L is a unit lower triangular
  //*     matrix, and U is upper triangular.
  //*
  //*  3. If some U(i,i)=0, so that U is exactly singular, then the routine
  //*     returns with INFO = i. Otherwise, the factored form of A is used
  //*     to estimate the condition number of the matrix A.  If the
  //*     reciprocal of the condition number is less than machine precision,
  //*     INFO = N+1 is returned as a warning, but the routine still goes on
  //*     to solve for X and compute error bounds as described below.
  //*
  //*  4. The system of equations is solved for X using the factored form
  //*     of A.
  //*
  //*  5. Iterative refinement is applied to improve the computed solution
  //*     matrix and calculate error bounds and backward error estimates
  //*     for it.
  //*
  //*  6. If equilibration was used, the matrix X is premultiplied by
  //*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so
  //*     that it solves the original system before equilibration.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  FACT    (input) CHARACTER*1
  //*          Specifies whether or not the factored form of the matrix A is
  //*          supplied on entry, and if not, whether the matrix A should be
  //*          equilibrated before it is factored.
  //*          = 'F':  On entry, AF and IPIV contain the factored form of A.
  //*                  If EQUED is not 'N', the matrix A has been
  //*                  equilibrated with scaling factors given by R and C.
  //*                  A, AF, and IPIV are not modified.
  //*          = 'N':  The matrix A will be copied to AF and factored.
  //*          = 'E':  The matrix A will be equilibrated if necessary, then
  //*                  copied to AF and factored.

  char FACT = 'E';

  //*
  //*  TRANS   (input) CHARACTER*1
  //*          Specifies the form of the system of equations:
  //*          = 'N':  A * X = B     (No transpose)
  //*          = 'T':  A**T * X = B  (Transpose)
  //*          = 'C':  A**H * X = B  (Transpose)

  char TRANS = 'N';

  //*
  //*  N       (input) INTEGER
  //*          The number of linear equations, i.e., the order of the
  //*          matrix A.  N >= 0.
  //*

  int N = m;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of columns
  //*          of the matrices B and X.  NRHS >= 0.

  int NRHS = n;
  
  //*
  //*  A       (input/output) REAL array, dimension (LDA,N)
  //*          On entry, the N-by-N matrix A.  If FACT = 'F' and EQUED is
  //*          not 'N', then A must have been equilibrated by the scaling
  //*          factors in R and/or C.  A is not modified if FACT = 'F' or
  //*          'N', or if FACT = 'E' and EQUED = 'N' on exit.
  //*
  //*          On exit, if EQUED .ne. 'N', A is scaled as follows:
  //*          EQUED = 'R':  A := diag(R) * A
  //*          EQUED = 'C':  A := A * diag(C)
  //*          EQUED = 'B':  A := diag(R) * A * diag(C).
  
  float* A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,N).
  //*

  int LDA = m;

  //*  AF      (input or output) REAL array, dimension (LDAF,N)
  //*          If FACT = 'F', then AF is an input argument and on entry
  //*          contains the factors L and U from the factorization
  //*          A = P*L*U as computed by DGETRF.  If EQUED .ne. 'N', then
  //*          AF is the factored form of the equilibrated matrix A.
  //*
  //*          If FACT = 'N', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the equilibrated matrix A (see the description of A for
  //*          the form of the equilibrated matrix).

  float *AF = (float*) Malloc(sizeof(float)*LDA*N);

  //*  LDAF    (input) INTEGER
  //*          The leading dimension of the array AF.  LDAF >= max(1,N).
  //*

  int LDAF = m;

  //*  IPIV    (input or output) INTEGER array, dimension (N)
  //*          If FACT = 'F', then IPIV is an input argument and on entry
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          as computed by DGETRF; row i of the matrix was interchanged
  //*          with row IPIV(i).
  //*
  //*          If FACT = 'N', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the equilibrated matrix A.

  int *IPIV = (int*) Malloc(sizeof(int)*N);

  //*  EQUED   (input or output) CHARACTER*1
  //*          Specifies the form of equilibration that was done.
  //*          = 'N':  No equilibration (always true if FACT = 'N').
  //*          = 'R':  Row equilibration, i.e., A has been premultiplied by
  //*                  diag(R).
  //*          = 'C':  Column equilibration, i.e., A has been postmultiplied
  //*                  by diag(C).
  //*          = 'B':  Both row and column equilibration, i.e., A has been
  //*                  replaced by diag(R) * A * diag(C).
  //*          EQUED is an input argument if FACT = 'F'; otherwise, it is an
  //*          output argument.
  
  char EQUED;

  //*  R       (input or output) REAL array, dimension (N)
  //*          The row scale factors for A.  If EQUED = 'R' or 'B', A is
  //*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R
  //*          is not accessed.  R is an input argument if FACT = 'F';
  //*          otherwise, R is an output argument.  If FACT = 'F' and
  //*          EQUED = 'R' or 'B', each element of R must be positive.

  float *R = (float*) Malloc(sizeof(float)*N);

  //*  C       (input or output) REAL array, dimension (N)
  //*          The column scale factors for A.  If EQUED = 'C' or 'B', A is
  //*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C
  //*          is not accessed.  C is an input argument if FACT = 'F';
  //*          otherwise, C is an output argument.  If FACT = 'F' and
  //*          EQUED = 'C' or 'B', each element of C must be positive.
  
  float *C = (float*) Malloc(sizeof(float)*N);

  //*  B       (input/output) REAL array, dimension (LDB,NRHS)
  //*          On entry, the N-by-NRHS right hand side matrix B.
  //*          On exit,
  //*          if EQUED = 'N', B is not modified;
  //*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by
  //*          diag(R)*B;
  //*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is
  //*          overwritten by diag(C)*B.

  float *B = b;

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B.  LDB >= max(1,N).

  int LDB = m;

  //*  X       (output) REAL array, dimension (LDX,NRHS)
  //*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X
  //*          to the original system of equations.  Note that A and B are
  //*          modified on exit if EQUED .ne. 'N', and the solution to the
  //*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and
  //*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C'
  //*          and EQUED = 'R' or 'B'.
  
  float *X = c;

  //*  LDX     (input) INTEGER
  //*          The leading dimension of the array X.  LDX >= max(1,N).
  
  int LDX = m;

  //*  RCOND   (output) REAL
  //*          The estimate of the reciprocal condition number of the matrix
  //*          A after equilibration (if done).  If RCOND is less than the
  //*          machine precision (in particular, if RCOND = 0), the matrix
  //*          is singular to working precision.  This condition is
  //*          indicated by a return code of INFO > 0.

  float RCOND;
  
  //*  FERR    (output) REAL array, dimension (NRHS)
  //*          The estimated forward error bound for each solution vector
  //*          X(j) (the j-th column of the solution matrix X).
  //*          If XTRUE is the true solution corresponding to X(j), FERR(j)
  //*          is an estimated upper bound for the magnitude of the largest
  //*          element in (X(j) - XTRUE) divided by the magnitude of the
  //*          largest element in X(j).  The estimate is as reliable as
  //*          the estimate for RCOND, and is almost always a slight
  //*          overestimate of the true error.

  float *FERR = (float*) Malloc(n*sizeof(float));

  //*  BERR    (output) REAL array, dimension (NRHS)
  //*          The componentwise relative backward error of each solution
  //*          vector X(j) (i.e., the smallest relative change in
  //*          any element of A or B that makes X(j) an exact solution).

  float *BERR = (float*) Malloc(n*sizeof(float));

  //*  WORK    (workspace/output) REAL array, dimension (4*N)
  //*          On exit, WORK(1) contains the reciprocal pivot growth
  //*          factor norm(A)/norm(U). The "max absolute element" norm is
  //*          used. If WORK(1) is much less than 1, then the stability
  //*          of the LU factorization of the (equilibrated) matrix A
  //*          could be poor. This also means that the solution X, condition
  //*          estimator RCOND, and forward error bound FERR could be
  //*          unreliable. If factorization fails with 0<INFO<=N, then
  //*          WORK(1) contains the reciprocal pivot growth factor for the
  //*          leading INFO columns of A.
 
  float *WORK = (float*) Malloc(4*N*sizeof(float));

  //*  IWORK   (workspace) INTEGER array, dimension (N)

  int *IWORK = (int*) Malloc(4*N*sizeof(int));

  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  //*          > 0:  if INFO = i, and i is
  //*                <= N:  U(i,i) is exactly zero.  The factorization has
  //*                       been completed, but the factor U is exactly
  //*                       singular, so the solution and error bounds
  //*                       could not be computed. RCOND = 0 is returned.
  //*                = N+1: U is nonsingular, but RCOND is less than machine
  //*                       precision, meaning that the matrix is singular
  //*                       to working precision.  Nevertheless, the
  //*                       solution and error bounds are computed because
  //*                       there are a number of situations where the
  //*                       computed solution can be more accurate than the
  //*                       value of RCOND would suggest.
  //*
  //*  =====================================================================

  int INFO;

  sgesvx_(&FACT, &TRANS, &N, &NRHS, A, &LDA, AF, &LDAF, IPIV, &EQUED, R, C, B,
	  &LDB, X, &LDX, &RCOND, FERR, BERR, WORK, IWORK, &INFO);
  if ((INFO == N) || (INFO == N+1) || (RCOND < getFloatEPS())) {
    snprintf(msgBuffer,MSGBUFLEN,"Matrix is singular to working (single) precision.  RCOND = %e\n",RCOND);
    eval->warningMessage(msgBuffer);
  }
  // Free the allocated arrays...
  Free(AF);
  Free(IPIV);
  Free(R);
  Free(C);
  Free(WORK);
  Free(IWORK);
}

// Solve A*C = B, where A is m x m, and B is m x n, all quantities are real.
void complexSolveLinEq(Interpreter* eval, int m, int n, float *c, float* a, float *b) {
  // Here are the comments from the LAPACK routine used:
  //SUBROUTINE CGESVX( FACT, TRANS, N, NRHS, A, LDA, AF, LDAF, IPIV,
  //$                   EQUED, R, C, B, LDB, X, LDX, RCOND, FERR, BERR,
  //$                   WORK, IWORK, INFO )
  //*
  //*  -- LAPACK driver routine (version 3.0) --
  //*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
  //*     Courant Institute, Argonne National Lab, and Rice University
  //*     June 30, 1999
  //*
  //*     .. Scalar Arguments ..
  //	CHARACTER          EQUED, FACT, TRANS
  //	INTEGER            INFO, LDA, LDAF, LDB, LDX, N, NRHS
  //	REAL   RCOND
  //*     ..
  //*     .. Array Arguments ..
  //      INTEGER            IPIV( * )
  //      REAL   BERR( * ), C( * ), FERR( * ), R( * ),
  //     $                   RWORK( * )
  //      COMPLEX         A( LDA, * ), AF( LDAF, * ), B( LDB, * ),
  //     $                   WORK( * ), X( LDX, * )
  //*     ..
  //*
  //*  Purpose
  //*  =======
  //*
  //*  ZGESVX uses the LU factorization to compute the solution to a real
  //*  system of linear equations
  //*     A * X = B,
  //*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices.
  //*
  //*  Error bounds on the solution and a condition estimate are also
  //*  provided.
  //*
  //*  Description
  //*  ===========
  //*
  //*  The following steps are performed:
  //*
  //*  1. If FACT = 'E', real scaling factors are computed to equilibrate
  //*     the system:
  //*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B
  //*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B
  //*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B
  //*     Whether or not the system will be equilibrated depends on the
  //*     scaling of the matrix A, but if equilibration is used, A is
  //*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N')
  //*     or diag(C)*B (if TRANS = 'T' or 'C').
  //*
  //*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the
  //*     matrix A (after equilibration if FACT = 'E') as
  //*        A = P * L * U,
  //*     where P is a permutation matrix, L is a unit lower triangular
  //*     matrix, and U is upper triangular.
  //*
  //*  3. If some U(i,i)=0, so that U is exactly singular, then the routine
  //*     returns with INFO = i. Otherwise, the factored form of A is used
  //*     to estimate the condition number of the matrix A.  If the
  //*     reciprocal of the condition number is less than machine precision,
  //*     INFO = N+1 is returned as a warning, but the routine still goes on
  //*     to solve for X and compute error bounds as described below.
  //*
  //*  4. The system of equations is solved for X using the factored form
  //*     of A.
  //*
  //*  5. Iterative refinement is applied to improve the computed solution
  //*     matrix and calculate error bounds and backward error estimates
  //*     for it.
  //*
  //*  6. If equilibration was used, the matrix X is premultiplied by
  //*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so
  //*     that it solves the original system before equilibration.
  //*
  //*  Arguments
  //*  =========
  //*
  //*  FACT    (input) CHARACTER*1
  //*          Specifies whether or not the factored form of the matrix A is
  //*          supplied on entry, and if not, whether the matrix A should be
  //*          equilibrated before it is factored.
  //*          = 'F':  On entry, AF and IPIV contain the factored form of A.
  //*                  If EQUED is not 'N', the matrix A has been
  //*                  equilibrated with scaling factors given by R and C.
  //*                  A, AF, and IPIV are not modified.
  //*          = 'N':  The matrix A will be copied to AF and factored.
  //*          = 'E':  The matrix A will be equilibrated if necessary, then
  //*                  copied to AF and factored.

  char FACT = 'E';

  //*
  //*  TRANS   (input) CHARACTER*1
  //*          Specifies the form of the system of equations:
  //*          = 'N':  A * X = B     (No transpose)
  //*          = 'T':  A**T * X = B  (Transpose)
  //*          = 'C':  A**H * X = B  (Transpose)

  char TRANS = 'N';

  //*
  //*  N       (input) INTEGER
  //*          The number of linear equations, i.e., the order of the
  //*          matrix A.  N >= 0.
  //*

  int N = m;

  //*  NRHS    (input) INTEGER
  //*          The number of right hand sides, i.e., the number of columns
  //*          of the matrices B and X.  NRHS >= 0.

  int NRHS = n;
  
  //*  A       (input/output) COMPLEX array, dimension (LDA,N)
  //*          On entry, the N-by-N matrix A.  If FACT = 'F' and EQUED is
  //*          not 'N', then A must have been equilibrated by the scaling
  //*          factors in R and/or C.  A is not modified if FACT = 'F' or
  //*          'N', or if FACT = 'E' and EQUED = 'N' on exit.
  //*
  //*          On exit, if EQUED .ne. 'N', A is scaled as follows:
  //*          EQUED = 'R':  A := diag(R) * A
  //*          EQUED = 'C':  A := A * diag(C)
  //*          EQUED = 'B':  A := diag(R) * A * diag(C).
  
  float* A = a;

  //*  LDA     (input) INTEGER
  //*          The leading dimension of the array A.  LDA >= max(1,N).
  //*

  int LDA = m;

  //*  AF      (input or output) COMPLEX array, dimension (LDAF,N)
  //*          If FACT = 'F', then AF is an input argument and on entry
  //*          contains the factors L and U from the factorization
  //*          A = P*L*U as computed by DGETRF.  If EQUED .ne. 'N', then
  //*          AF is the factored form of the equilibrated matrix A.
  //*
  //*          If FACT = 'N', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then AF is an output argument and on exit
  //*          returns the factors L and U from the factorization A = P*L*U
  //*          of the equilibrated matrix A (see the description of A for
  //*          the form of the equilibrated matrix).

  float *AF = (float*) Malloc(2*sizeof(float)*LDA*N);

  //*  LDAF    (input) INTEGER
  //*          The leading dimension of the array AF.  LDAF >= max(1,N).
  //*

  int LDAF = m;

  //*  IPIV    (input or output) INTEGER array, dimension (N)
  //*          If FACT = 'F', then IPIV is an input argument and on entry
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          as computed by DGETRF; row i of the matrix was interchanged
  //*          with row IPIV(i).
  //*
  //*          If FACT = 'N', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the original matrix A.
  //*
  //*          If FACT = 'E', then IPIV is an output argument and on exit
  //*          contains the pivot indices from the factorization A = P*L*U
  //*          of the equilibrated matrix A.

  int *IPIV = (int*) Malloc(sizeof(int)*N);

  //*  EQUED   (input or output) CHARACTER*1
  //*          Specifies the form of equilibration that was done.
  //*          = 'N':  No equilibration (always true if FACT = 'N').
  //*          = 'R':  Row equilibration, i.e., A has been premultiplied by
  //*                  diag(R).
  //*          = 'C':  Column equilibration, i.e., A has been postmultiplied
  //*                  by diag(C).
  //*          = 'B':  Both row and column equilibration, i.e., A has been
  //*                  replaced by diag(R) * A * diag(C).
  //*          EQUED is an input argument if FACT = 'F'; otherwise, it is an
  //*          output argument.
  
  char EQUED;

  //*  R       (input or output) REAL array, dimension (N)
  //*          The row scale factors for A.  If EQUED = 'R' or 'B', A is
  //*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R
  //*          is not accessed.  R is an input argument if FACT = 'F';
  //*          otherwise, R is an output argument.  If FACT = 'F' and
  //*          EQUED = 'R' or 'B', each element of R must be positive.

  float *R = (float*) Malloc(sizeof(float)*N);

  //*  C       (input or output) REAL array, dimension (N)
  //*          The column scale factors for A.  If EQUED = 'C' or 'B', A is
  //*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C
  //*          is not accessed.  C is an input argument if FACT = 'F';
  //*          otherwise, C is an output argument.  If FACT = 'F' and
  //*          EQUED = 'C' or 'B', each element of C must be positive.
  
  float *C = (float*) Malloc(sizeof(float)*N);

  //*  B       (input/output) COMPLEX array, dimension (LDB,NRHS)
  //*          On entry, the N-by-NRHS right hand side matrix B.
  //*          On exit,
  //*          if EQUED = 'N', B is not modified;
  //*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by
  //*          diag(R)*B;
  //*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is
  //*          overwritten by diag(C)*B.

  float *B = b;

  //*  LDB     (input) INTEGER
  //*          The leading dimension of the array B.  LDB >= max(1,N).

  int LDB = m;

  //*  X       (output) COMPLEX array, dimension (LDX,NRHS)
  //*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X
  //*          to the original system of equations.  Note that A and B are
  //*          modified on exit if EQUED .ne. 'N', and the solution to the
  //*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and
  //*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C'
  //*          and EQUED = 'R' or 'B'.
  
  float *X = c;

  //*  LDX     (input) INTEGER
  //*          The leading dimension of the array X.  LDX >= max(1,N).
  
  int LDX = m;

  //*  RCOND   (output) REAL
  //*          The estimate of the reciprocal condition number of the matrix
  //*          A after equilibration (if done).  If RCOND is less than the
  //*          machine precision (in particular, if RCOND = 0), the matrix
  //*          is singular to working precision.  This condition is
  //*          indicated by a return code of INFO > 0.

  float RCOND;
  
  //*  FERR    (output) REAL array, dimension (NRHS)
  //*          The estimated forward error bound for each solution vector
  //*          X(j) (the j-th column of the solution matrix X).
  //*          If XTRUE is the true solution corresponding to X(j), FERR(j)
  //*          is an estimated upper bound for the magnitude of the largest
  //*          element in (X(j) - XTRUE) divided by the magnitude of the
  //*          largest element in X(j).  The estimate is as reliable as
  //*          the estimate for RCOND, and is almost always a slight
  //*          overestimate of the true error.

  float *FERR = (float*) Malloc(n*sizeof(float));

  //*  BERR    (output) REAL array, dimension (NRHS)
  //*          The componentwise relative backward error of each solution
  //*          vector X(j) (i.e., the smallest relative change in
  //*          any element of A or B that makes X(j) an exact solution).

  float *BERR = (float*) Malloc(n*sizeof(float));

  //*  WORK    (workspace) COMPLEX array, dimension (2*N)

  float *WORK = (float*) Malloc(2*(2*N)*sizeof(float));
  
  //*  RWORK   (workspace/output) REAL array, dimension (2*N)
  //*          On exit, RWORK(1) contains the reciprocal pivot growth
  //*          factor norm(A)/norm(U). The "max absolute element" norm is
  //*          used. If RWORK(1) is much less than 1, then the stability
  //*          of the LU factorization of the (equilibrated) matrix A
  //*          could be poor. This also means that the solution X, condition
  //*          estimator RCOND, and forward error bound FERR could be
  //*          unreliable. If factorization fails with 0<INFO<=N, then
  //*          RWORK(1) contains the reciprocal pivot growth factor for the
  //*          leading INFO columns of A.

  float *RWORK = (float*) Malloc(2*N*sizeof(float));

  //*  INFO    (output) INTEGER
  //*          = 0:  successful exit
  //*          < 0:  if INFO = -i, the i-th argument had an illegal value
  //*          > 0:  if INFO = i, and i is
  //*                <= N:  U(i,i) is exactly zero.  The factorization has
  //*                       been completed, but the factor U is exactly
  //*                       singular, so the solution and error bounds
  //*                       could not be computed. RCOND = 0 is returned.
  //*                = N+1: U is nonsingular, but RCOND is less than machine
  //*                       precision, meaning that the matrix is singular
  //*                       to working precision.  Nevertheless, the
  //*                       solution and error bounds are computed because
  //*                       there are a number of situations where the
  //*                       computed solution can be more accurate than the
  //*                       value of RCOND would suggest.
  //*
  //*  =====================================================================

  int INFO;

  cgesvx_(&FACT, &TRANS, &N, &NRHS, A, &LDA, AF, &LDAF, IPIV, &EQUED, R, C, B,
	  &LDB, X, &LDX, &RCOND, FERR, BERR, WORK, RWORK, &INFO);
  if ((INFO == N) || (INFO == N+1) || (RCOND < getFloatEPS())) {
    snprintf(msgBuffer,MSGBUFLEN,"Matrix is singular to working (single) precision.  RCOND = %e\n",RCOND);
    eval->warningMessage(msgBuffer);
  }
  // Free the allocated arrays...
  Free(IPIV);
  Free(R);
  Free(C);
  Free(WORK);
  Free(RWORK);
  Free(FERR);
  Free(BERR);
}
