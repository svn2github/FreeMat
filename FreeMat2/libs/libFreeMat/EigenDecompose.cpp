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

#include "EigenDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

namespace FreeMat {

  void floatEigenDecomposeSymmetric(const int n, float *v, float *d, float *a,
				    bool eigenvectors) {
    //      SUBROUTINE SSYEV( JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  SSYEV computes all eigenvalues and, optionally, eigenvectors of a
    //*  real symmetric matrix A.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  JOBZ    (input) CHARACTER*1
    //*          = 'N':  Compute eigenvalues only;
    //*          = 'V':  Compute eigenvalues and eigenvectors.
    char JOBZ;
    if (eigenvectors)
      JOBZ = 'V';
    else
      JOBZ = 'N';

    //*
    //*  UPLO    (input) CHARACTER*1
    //*          = 'U':  Upper triangle of A is stored;
    //*          = 'L':  Lower triangle of A is stored.
    //*
    char UPLO = 'U';

    //*  N       (input) INTEGER
    //*          The order of the matrix A.  N >= 0.
    //*
    int N = n;

    //*  A       (input/output) REAL array, dimension (LDA, N)
    //*          On entry, the symmetric matrix A.  If UPLO = 'U', the
    //*          leading N-by-N upper triangular part of A contains the
    //*          upper triangular part of the matrix A.  If UPLO = 'L',
    //*          the leading N-by-N lower triangular part of A contains
    //*          the lower triangular part of the matrix A.
    //*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the
    //*          orthonormal eigenvectors of the matrix A.
    //*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')
    //*          or the upper triangle (if UPLO='U') of A, including the
    //*          diagonal, is destroyed.
    //*

    float *Ain = a;
    
    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).
    //*
    
    int LDA = n;
    
    //*  W       (output) REAL array, dimension (N)
    //*          If INFO = 0, the eigenvalues in ascending order.
    //*
    
    //*  WORK    (workspace/output) REAL array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
    //*
    
    //*  LWORK   (input) INTEGER
    //*          The length of the array WORK.  LWORK >= max(1,3*N-1).
    //*          For optimal efficiency, LWORK >= (NB+2)*N,
    //*          where NB is the blocksize for SSYTRD returned by ILAENV.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.
    //*
    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value
    //*          > 0:  if INFO = i, the algorithm failed to converge; i
    //*                off-diagonal elements of an intermediate tridiagonal
    //*                form did not converge to zero.
    //    
    int INFO;
    float WORKSZE;
    int LWORK;

    LWORK = -1;
    ssyev_(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORKSZE, &LWORK, &INFO);
    LWORK = (int) WORKSZE;
    float *WORK = (float*) Malloc(LWORK*sizeof(float));
    ssyev_(&JOBZ, &UPLO, &N, Ain, &LDA, d, WORK, &LWORK, &INFO);
    Free(WORK);
    if (eigenvectors)
      memcpy(v,a,n*n*sizeof(float));
  }

  void floatEigenDecompose(const int n, float *v, float *d, float *a,
			   bool eigenvectors, bool balance) {
    //	SUBROUTINE SGEEVX( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, WR, WI,
    //     $                   VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
    //     $                   RCONDE, RCONDV, WORK, LWORK, IWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  SGEEVX computes for an N-by-N real nonsymmetric matrix A, the
    //*  eigenvalues and, optionally, the left and/or right eigenvectors.
    //*
    //*  Optionally also, it computes a balancing transformation to improve
    //*  the conditioning of the eigenvalues and eigenvectors (ILO, IHI,
    //*  SCALE, and ABNRM), reciprocal condition numbers for the eigenvalues
    //*  (RCONDE), and reciprocal condition numbers for the right
    //*  eigenvectors (RCONDV).
    //*
    //*  The right eigenvector v(j) of A satisfies
    //*                   A * v(j) = lambda(j) * v(j)
    //*  where lambda(j) is its eigenvalue.
    //*  The left eigenvector u(j) of A satisfies
    //*                u(j)**H * A = lambda(j) * u(j)**H
    //*  where u(j)**H denotes the conjugate transpose of u(j).
    //*
    //*  The computed eigenvectors are normalized to have Euclidean norm
    //*  equal to 1 and largest component real.
    //*
    //*  Balancing a matrix means permuting the rows and columns to make it
    //*  more nearly upper triangular, and applying a diagonal similarity
    //*  transformation D * A * D**(-1), where D is a diagonal matrix, to
    //*  make its rows and columns closer in norm and the condition numbers
    //*  of its eigenvalues and eigenvectors smaller.  The computed
    //*  reciprocal condition numbers correspond to the balanced matrix.
    //*  Permuting rows and columns will not change the condition numbers
    //*  (in exact arithmetic) but diagonal scaling will.  For further
    //*  explanation of balancing, see section 4.10.2 of the LAPACK
    //*  Users' Guide.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  BALANC  (input) CHARACTER*1
    //*          Indicates how the input matrix should be diagonally scaled
    //*          and/or permuted to improve the conditioning of its
    //*          eigenvalues.
    //*          = 'N': Do not diagonally scale or permute;
    //*          = 'P': Perform permutations to make the matrix more nearly
    //*                 upper triangular. Do not diagonally scale;
    //*          = 'S': Diagonally scale the matrix, i.e. replace A by
    //*                 D*A*D**(-1), where D is a diagonal matrix chosen
    //*                 to make the rows and columns of A more equal in
    //*                 norm. Do not permute;
    //*          = 'B': Both diagonally scale and permute A.
    //*
    //*          Computed reciprocal condition numbers will be for the matrix
    //*          after balancing and/or permuting. Permuting does not change
    //*          condition numbers (in exact arithmetic), but balancing does.
    //*

    char BALANC;
    if (balance)
      BALANC = 'B';
    else
      BALANC = 'N';

    //*  JOBVL   (input) CHARACTER*1
    //*          = 'N': left eigenvectors of A are not computed;
    //*          = 'V': left eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVL must = 'V'.
  
    char JOBVL = 'N';
  
    //*  JOBVR   (input) CHARACTER*1
    //*          = 'N': right eigenvectors of A are not computed;
    //*          = 'V': right eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVR must = 'V'.

    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';

    //*  SENSE   (input) CHARACTER*1
    //*          Determines which reciprocal condition numbers are computed.
    //*          = 'N': None are computed;
    //*          = 'E': Computed for eigenvalues only;
    //*          = 'V': Computed for right eigenvectors only;
    //*          = 'B': Computed for eigenvalues and right eigenvectors.
    //*
    //*          If SENSE = 'E' or 'B', both left and right eigenvectors
    //*          must also be computed (JOBVL = 'V' and JOBVR = 'V').

    char SENSE = 'N';

    //*  N       (input) INTEGER
    //*          The order of the matrix A. N >= 0.

    int N = n;
  
    //*  A       (input/output) REAL array, dimension (LDA,N)
    //*          On entry, the N-by-N matrix A.
    //*          On exit, A has been overwritten.  If JOBVL = 'V' or
    //*          JOBVR = 'V', A contains the real Schur form of the balanced
    //*          version of the input matrix A.

    float *Ain = a;

    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).

    int LDA = n;

    //*  WR      (output) REAL array, dimension (N)
    //*  WI      (output) REAL array, dimension (N)
    //*          WR and WI contain the real and imaginary parts,
    //*          respectively, of the computed eigenvalues.  Complex
    //*          conjugate pairs of eigenvalues will appear consecutively
    //*          with the eigenvalue having the positive imaginary part
    //*          first.
  
    float *WR = (float*) Malloc(n*sizeof(float));
    float *WI = (float*) Malloc(n*sizeof(float));

    //*  VL      (output) REAL array, dimension (LDVL,N)
    //*          If JOBVL = 'V', the left eigenvectors u(j) are stored one
    //*          after another in the columns of VL, in the same order
    //*          as their eigenvalues.
    //*          If JOBVL = 'N', VL is not referenced.
    //*          If the j-th eigenvalue is real, then u(j) = VL(:,j),
    //*          the j-th column of VL.
    //*          If the j-th and (j+1)-st eigenvalues form a complex
    //*          conjugate pair, then u(j) = VL(:,j) + i*VL(:,j+1) and
    //*          u(j+1) = VL(:,j) - i*VL(:,j+1).

    float *VL = NULL;

    //*  LDVL    (input) INTEGER
    //*          The leading dimension of the array VL.  LDVL >= 1; if
    //*          JOBVL = 'V', LDVL >= N.

    int LDVL = 1;

    //*  VR      (output) REAL array, dimension (LDVR,N)
    //*          If JOBVR = 'V', the right eigenvectors v(j) are stored one
    //*          after another in the columns of VR, in the same order
    //*          as their eigenvalues.
    //*          If JOBVR = 'N', VR is not referenced.
    //*          If the j-th eigenvalue is real, then v(j) = VR(:,j),
    //*          the j-th column of VR.
    //*          If the j-th and (j+1)-st eigenvalues form a complex
    //*          conjugate pair, then v(j) = VR(:,j) + i*VR(:,j+1) and
    //*          v(j+1) = VR(:,j) - i*VR(:,j+1).

    float *VR = v;

    //*  LDVR    (input) INTEGER
    //*          The leading dimension of the array VR.  LDVR >= 1, and if
    //*          JOBVR = 'V', LDVR >= N.

    int LDVR = n;

    //*  ILO,IHI (output) INTEGER
    //*          ILO and IHI are integer values determined when A was
    //*          balanced.  The balanced A(i,j) = 0 if I > J and
    //*          J = 1,...,ILO-1 or I = IHI+1,...,N.

    int ILO;
    int IHI;

    //*  SCALE   (output) REAL array, dimension (N)
    //*          Details of the permutations and scaling factors applied
    //*          when balancing A.  If P(j) is the index of the row and column
    //*          interchanged with row and column j, and D(j) is the scaling
    //*          factor applied to row and column j, then
    //*          SCALE(J) = P(J),    for J = 1,...,ILO-1
    //*                   = D(J),    for J = ILO,...,IHI
    //*                   = P(J)     for J = IHI+1,...,N.
    //*          The order in which the interchanges are made is N to IHI+1,
    //*          then 1 to ILO-1.

    float *SCALE = (float*) Malloc(n*sizeof(float));

    //*  ABNRM   (output) REAL
    //*          The one-norm of the balanced matrix (the maximum
    //*          of the sum of absolute values of elements of any column).

    float ABNRM;

    //*  RCONDE  (output) REAL array, dimension (N)
    //*          RCONDE(j) is the reciprocal condition number of the j-th
    //*          eigenvalue.

    float *RCONDE = (float*) Malloc(n*sizeof(float));

    //*  RCONDV  (output) REAL array, dimension (N)
    //*          RCONDV(j) is the reciprocal condition number of the j-th
    //*          right eigenvector.

    float *RCONDV = (float*) Malloc(n*sizeof(float));

    //*  WORK    (workspace/output) REAL array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    //    double *WORK = (double*) Malloc(250*sizeof(double));

    //*
    //*  LWORK   (input) INTEGER
    //*          The dimension of the array WORK.   If SENSE = 'N' or 'E',
    //*          LWORK >= max(1,2*N), and if JOBVL = 'V' or JOBVR = 'V',
    //*          LWORK >= 3*N.  If SENSE = 'V' or 'B', LWORK >= N*(N+6).
    //*          For good performance, LWORK must generally be larger.
    //*
    //*          If LWORK = -1, a workspace query is assumed.  The optimal
    //*          size for the WORK array is calculated and stored in WORK(1),
    //*          and no other work except argument checking is performed.
  
    int LWORK;

    //*  IWORK   (workspace) INTEGER array, dimension (2*N-2)
    //*          If SENSE = 'N' or 'E', not referenced.

    int *IWORK = (int*) Malloc((2*n-2)*sizeof(int));

    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
    //*          > 0:  if INFO = i, the QR algorithm failed to compute all the
    //*                eigenvalues, and no eigenvectors or condition numbers
    //*                have been computed; elements 1:ILO-1 and i+1:N of WR
    //*                and WI contain eigenvalues which have converged.

    int INFO;
    float WORKSZE;

    LWORK = -1;
    sgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, WR, WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, &WORKSZE, &LWORK, IWORK, &INFO );

    LWORK = (int) WORKSZE;
    float *WORK = (float*) Malloc(LWORK*sizeof(float));

    sgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, WR, WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, WORK, &LWORK, IWORK, &INFO );

    for (int i=0;i<N;i++) {
      d[2*i] = WR[i];
      d[2*i+1] = WI[i];
    }

    Free(WORK);
    Free(WR);
    Free(WI);
    Free(SCALE);
    Free(RCONDE);
    Free(RCONDV);
    Free(IWORK);
  }

  void doubleEigenDecomposeSymmetric(const int n, double *v, double *d, 
				     double *a, bool eigenvectors) {
    //      SUBROUTINE DSYEV( JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  DSYEV computes all eigenvalues and, optionally, eigenvectors of a
    //*  real symmetric matrix A.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  JOBZ    (input) CHARACTER*1
    //*          = 'N':  Compute eigenvalues only;
    //*          = 'V':  Compute eigenvalues and eigenvectors.
    char JOBZ;
    if (eigenvectors)
      JOBZ = 'V';
    else
      JOBZ = 'N';

    //*
    //*  UPLO    (input) CHARACTER*1
    //*          = 'U':  Upper triangle of A is stored;
    //*          = 'L':  Lower triangle of A is stored.
    //*
    char UPLO = 'U';

    //*  N       (input) INTEGER
    //*          The order of the matrix A.  N >= 0.
    //*
    int N = n;

    //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N)
    //*          On entry, the symmetric matrix A.  If UPLO = 'U', the
    //*          leading N-by-N upper triangular part of A contains the
    //*          upper triangular part of the matrix A.  If UPLO = 'L',
    //*          the leading N-by-N lower triangular part of A contains
    //*          the lower triangular part of the matrix A.
    //*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the
    //*          orthonormal eigenvectors of the matrix A.
    //*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')
    //*          or the upper triangle (if UPLO='U') of A, including the
    //*          diagonal, is destroyed.
    //*

    double *Ain = a;
    
    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).
    //*
    
    int LDA = n;
    
    //*  W       (output) DOUBLE PRECISION array, dimension (N)
    //*          If INFO = 0, the eigenvalues in ascending order.
    //*
    
    //*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
    //*
    
    //*  LWORK   (input) INTEGER
    //*          The length of the array WORK.  LWORK >= max(1,3*N-1).
    //*          For optimal efficiency, LWORK >= (NB+2)*N,
    //*          where NB is the blocksize for SSYTRD returned by ILAENV.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.
    //*
    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value
    //*          > 0:  if INFO = i, the algorithm failed to converge; i
    //*                off-diagonal elements of an intermediate tridiagonal
    //*                form did not converge to zero.
    //    
    int INFO;
    double WORKSZE;
    int LWORK;

    LWORK = -1;
    dsyev_(&JOBZ, &UPLO, &N, Ain, &LDA, d, &WORKSZE, &LWORK, &INFO);
    LWORK = (int) WORKSZE;
    double *WORK = (double*) Malloc(LWORK*sizeof(double));
    dsyev_(&JOBZ, &UPLO, &N, Ain, &LDA, d, WORK, &LWORK, &INFO);
    Free(WORK);
    if (eigenvectors)
      memcpy(v,a,n*n*sizeof(double));
  }

  void doubleEigenDecompose(const int n, double *v, double *d, double *a,
			    bool eigenvectors, bool balance) {
    //	SUBROUTINE DGEEVX( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, WR, WI,
    //     $                   VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
    //     $                   RCONDE, RCONDV, WORK, LWORK, IWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  DGEEVX computes for an N-by-N real nonsymmetric matrix A, the
    //*  eigenvalues and, optionally, the left and/or right eigenvectors.
    //*
    //*  Optionally also, it computes a balancing transformation to improve
    //*  the conditioning of the eigenvalues and eigenvectors (ILO, IHI,
    //*  SCALE, and ABNRM), reciprocal condition numbers for the eigenvalues
    //*  (RCONDE), and reciprocal condition numbers for the right
    //*  eigenvectors (RCONDV).
    //*
    //*  The right eigenvector v(j) of A satisfies
    //*                   A * v(j) = lambda(j) * v(j)
    //*  where lambda(j) is its eigenvalue.
    //*  The left eigenvector u(j) of A satisfies
    //*                u(j)**H * A = lambda(j) * u(j)**H
    //*  where u(j)**H denotes the conjugate transpose of u(j).
    //*
    //*  The computed eigenvectors are normalized to have Euclidean norm
    //*  equal to 1 and largest component real.
    //*
    //*  Balancing a matrix means permuting the rows and columns to make it
    //*  more nearly upper triangular, and applying a diagonal similarity
    //*  transformation D * A * D**(-1), where D is a diagonal matrix, to
    //*  make its rows and columns closer in norm and the condition numbers
    //*  of its eigenvalues and eigenvectors smaller.  The computed
    //*  reciprocal condition numbers correspond to the balanced matrix.
    //*  Permuting rows and columns will not change the condition numbers
    //*  (in exact arithmetic) but diagonal scaling will.  For further
    //*  explanation of balancing, see section 4.10.2 of the LAPACK
    //*  Users' Guide.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  BALANC  (input) CHARACTER*1
    //*          Indicates how the input matrix should be diagonally scaled
    //*          and/or permuted to improve the conditioning of its
    //*          eigenvalues.
    //*          = 'N': Do not diagonally scale or permute;
    //*          = 'P': Perform permutations to make the matrix more nearly
    //*                 upper triangular. Do not diagonally scale;
    //*          = 'S': Diagonally scale the matrix, i.e. replace A by
    //*                 D*A*D**(-1), where D is a diagonal matrix chosen
    //*                 to make the rows and columns of A more equal in
    //*                 norm. Do not permute;
    //*          = 'B': Both diagonally scale and permute A.
    //*
    //*          Computed reciprocal condition numbers will be for the matrix
    //*          after balancing and/or permuting. Permuting does not change
    //*          condition numbers (in exact arithmetic), but balancing does.
    //*

    char BALANC;
    if (balance)
      BALANC = 'B';
    else
      BALANC = 'N';

    //*  JOBVL   (input) CHARACTER*1
    //*          = 'N': left eigenvectors of A are not computed;
    //*          = 'V': left eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVL must = 'V'.
  
    char JOBVL = 'N';
  
    //*  JOBVR   (input) CHARACTER*1
    //*          = 'N': right eigenvectors of A are not computed;
    //*          = 'V': right eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVR must = 'V'.

    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';

    //*  SENSE   (input) CHARACTER*1
    //*          Determines which reciprocal condition numbers are computed.
    //*          = 'N': None are computed;
    //*          = 'E': Computed for eigenvalues only;
    //*          = 'V': Computed for right eigenvectors only;
    //*          = 'B': Computed for eigenvalues and right eigenvectors.
    //*
    //*          If SENSE = 'E' or 'B', both left and right eigenvectors
    //*          must also be computed (JOBVL = 'V' and JOBVR = 'V').

    char SENSE = 'N';

    //*  N       (input) INTEGER
    //*          The order of the matrix A. N >= 0.

    int N = n;
  
    //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
    //*          On entry, the N-by-N matrix A.
    //*          On exit, A has been overwritten.  If JOBVL = 'V' or
    //*          JOBVR = 'V', A contains the real Schur form of the balanced
    //*          version of the input matrix A.

    double *Ain = a;

    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).

    int LDA = n;

    //*  WR      (output) DOUBLE PRECISION array, dimension (N)
    //*  WI      (output) DOUBLE PRECISION array, dimension (N)
    //*          WR and WI contain the real and imaginary parts,
    //*          respectively, of the computed eigenvalues.  Complex
    //*          conjugate pairs of eigenvalues will appear consecutively
    //*          with the eigenvalue having the positive imaginary part
    //*          first.
  
    double *WR = (double*) Malloc(n*sizeof(double));
    double *WI = (double*) Malloc(n*sizeof(double));

    //*  VL      (output) DOUBLE PRECISION array, dimension (LDVL,N)
    //*          If JOBVL = 'V', the left eigenvectors u(j) are stored one
    //*          after another in the columns of VL, in the same order
    //*          as their eigenvalues.
    //*          If JOBVL = 'N', VL is not referenced.
    //*          If the j-th eigenvalue is real, then u(j) = VL(:,j),
    //*          the j-th column of VL.
    //*          If the j-th and (j+1)-st eigenvalues form a complex
    //*          conjugate pair, then u(j) = VL(:,j) + i*VL(:,j+1) and
    //*          u(j+1) = VL(:,j) - i*VL(:,j+1).

    double *VL = NULL;

    //*  LDVL    (input) INTEGER
    //*          The leading dimension of the array VL.  LDVL >= 1; if
    //*          JOBVL = 'V', LDVL >= N.

    int LDVL = 1;

    //*  VR      (output) DOUBLE PRECISION array, dimension (LDVR,N)
    //*          If JOBVR = 'V', the right eigenvectors v(j) are stored one
    //*          after another in the columns of VR, in the same order
    //*          as their eigenvalues.
    //*          If JOBVR = 'N', VR is not referenced.
    //*          If the j-th eigenvalue is real, then v(j) = VR(:,j),
    //*          the j-th column of VR.
    //*          If the j-th and (j+1)-st eigenvalues form a complex
    //*          conjugate pair, then v(j) = VR(:,j) + i*VR(:,j+1) and
    //*          v(j+1) = VR(:,j) - i*VR(:,j+1).

    double *VR = v;

    //*  LDVR    (input) INTEGER
    //*          The leading dimension of the array VR.  LDVR >= 1, and if
    //*          JOBVR = 'V', LDVR >= N.

    int LDVR = n;

    //*  ILO,IHI (output) INTEGER
    //*          ILO and IHI are integer values determined when A was
    //*          balanced.  The balanced A(i,j) = 0 if I > J and
    //*          J = 1,...,ILO-1 or I = IHI+1,...,N.

    int ILO;
    int IHI;

    //*  SCALE   (output) DOUBLE PRECISION array, dimension (N)
    //*          Details of the permutations and scaling factors applied
    //*          when balancing A.  If P(j) is the index of the row and column
    //*          interchanged with row and column j, and D(j) is the scaling
    //*          factor applied to row and column j, then
    //*          SCALE(J) = P(J),    for J = 1,...,ILO-1
    //*                   = D(J),    for J = ILO,...,IHI
    //*                   = P(J)     for J = IHI+1,...,N.
    //*          The order in which the interchanges are made is N to IHI+1,
    //*          then 1 to ILO-1.

    double *SCALE = (double*) Malloc(n*sizeof(double));

    //*  ABNRM   (output) DOUBLE PRECISION
    //*          The one-norm of the balanced matrix (the maximum
    //*          of the sum of absolute values of elements of any column).

    double ABNRM;

    //*  RCONDE  (output) DOUBLE PRECISION array, dimension (N)
    //*          RCONDE(j) is the reciprocal condition number of the j-th
    //*          eigenvalue.

    double *RCONDE = (double*) Malloc(n*sizeof(double));

    //*  RCONDV  (output) DOUBLE PRECISION array, dimension (N)
    //*          RCONDV(j) is the reciprocal condition number of the j-th
    //*          right eigenvector.

    double *RCONDV = (double*) Malloc(n*sizeof(double));

    //*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    //    double *WORK = (double*) Malloc(250*sizeof(double));

    //*
    //*  LWORK   (input) INTEGER
    //*          The dimension of the array WORK.   If SENSE = 'N' or 'E',
    //*          LWORK >= max(1,2*N), and if JOBVL = 'V' or JOBVR = 'V',
    //*          LWORK >= 3*N.  If SENSE = 'V' or 'B', LWORK >= N*(N+6).
    //*          For good performance, LWORK must generally be larger.
    //*
    //*          If LWORK = -1, a workspace query is assumed.  The optimal
    //*          size for the WORK array is calculated and stored in WORK(1),
    //*          and no other work except argument checking is performed.
  
    int LWORK;

    //*  IWORK   (workspace) INTEGER array, dimension (2*N-2)
    //*          If SENSE = 'N' or 'E', not referenced.

    int *IWORK = (int*) Malloc((2*n-2)*sizeof(int));

    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
    //*          > 0:  if INFO = i, the QR algorithm failed to compute all the
    //*                eigenvalues, and no eigenvectors or condition numbers
    //*                have been computed; elements 1:ILO-1 and i+1:N of WR
    //*                and WI contain eigenvalues which have converged.

    int INFO;
    double WORKSZE;

    LWORK = -1;
    dgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, WR, WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, &WORKSZE, &LWORK, IWORK, &INFO );

    LWORK = (int) WORKSZE;
    double *WORK = (double*) Malloc(LWORK*sizeof(double));

    dgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, WR, WI,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, WORK, &LWORK, IWORK, &INFO );

    for (int i=0;i<N;i++) {
      d[2*i] = WR[i];
      d[2*i+1] = WI[i];
    }

    Free(WORK);
    Free(WR);
    Free(WI);
    Free(SCALE);
    Free(RCONDE);
    Free(RCONDV);
    Free(IWORK);
  }

  void complexEigenDecomposeSymmetric(const int n, float *v, float *d,
				      float *a, bool eigenvectors) {
    //        SUBROUTINE CHEEV( JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, RWORK,
    //     $                  INFO )
    //*  Purpose
    //*  =======
    //*
    //*  CHEEV computes all eigenvalues and, optionally, eigenvectors of a
    //*  complex Hermitian matrix A.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  JOBZ    (input) CHARACTER*1
    //*          = 'N':  Compute eigenvalues only;
    //*          = 'V':  Compute eigenvalues and eigenvectors.
    //*

    char JOBZ;
    if (eigenvectors)
      JOBZ = 'V';
    else
      JOBZ = 'N';

    //*  UPLO    (input) CHARACTER*1
    //*          = 'U':  Upper triangle of A is stored;
    //*          = 'L':  Lower triangle of A is stored.
    //*
    char UPLO = 'U';

    //*  N       (input) INTEGER
    //*          The order of the matrix A.  N >= 0.
    //*
    int N = n;

    //*  A       (input/output) COMPLEX array, dimension (LDA, N)
    //*          On entry, the Hermitian matrix A.  If UPLO = 'U', the
    //*          leading N-by-N upper triangular part of A contains the
    //*          upper triangular part of the matrix A.  If UPLO = 'L',
    //*          the leading N-by-N lower triangular part of A contains
    //*          the lower triangular part of the matrix A.
    //*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the
    //*          orthonormal eigenvectors of the matrix A.
    //*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')
    //*          or the upper triangle (if UPLO='U') of A, including the
    //*          diagonal, is destroyed.
    //*
    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).
    //*
    
    int LDA = n;
    
    //*  W       (output) REAL array, dimension (N)
    //*          If INFO = 0, the eigenvalues in ascending order.
    //*

    //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
    //*

    //*  LWORK   (input) INTEGER
    //*          The length of the array WORK.  LWORK >= max(1,2*N-1).
    //*          For optimal efficiency, LWORK >= (NB+1)*N,
    //*          where NB is the blocksize for CHETRD returned by ILAENV.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.
    //*

    //*  RWORK   (workspace) REAL array, dimension (max(1, 3*N-2))
    //*
    float *RWORK = (float*) Malloc(MAX(1,3*N-2)*sizeof(float));

    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value
    //*          > 0:  if INFO = i, the algorithm failed to converge; i
    //*                off-diagonal elements of an intermediate tridiagonal
    //*                form did not converge to zero.
    //*
    int LWORK;
    int INFO;
    float WORKSZE[2];
    LWORK = -1;
    cheev_(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,RWORK,&INFO);
    LWORK = (int) WORKSZE[0];
    float *WORK = (float*) Malloc(2*LWORK*sizeof(float));
    cheev_(&JOBZ,&UPLO,&N,a,&LDA,d,WORK,&LWORK,RWORK,&INFO);
    Free(WORK);
    if (eigenvectors)
      memcpy(v,a,2*n*n*sizeof(float));
  }

  void complexEigenDecompose(const int n, float *v, float *d, float *a,
			     bool eigenvectors, bool balance) {
    //	SUBROUTINE CGEEVX( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W, VL,
    //     $                   LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM, RCONDE,
    //     $                   RCONDV, WORK, LWORK, RWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  ZGEEVX computes for an N-by-N complex nonsymmetric matrix A, the
    //*  eigenvalues and, optionally, the left and/or right eigenvectors.
    //*
    //*  Optionally also, it computes a balancing transformation to improve
    //*  the conditioning of the eigenvalues and eigenvectors (ILO, IHI,
    //*  SCALE, and ABNRM), reciprocal condition numbers for the eigenvalues
    //*  (RCONDE), and reciprocal condition numbers for the right
    //*  eigenvectors (RCONDV).
    //*
    //*  The right eigenvector v(j) of A satisfies
    //*                   A * v(j) = lambda(j) * v(j)
    //*  where lambda(j) is its eigenvalue.
    //*  The left eigenvector u(j) of A satisfies
    //*                u(j)**H * A = lambda(j) * u(j)**H
    //*  where u(j)**H denotes the conjugate transpose of u(j).
    //*
    //*  The computed eigenvectors are normalized to have Euclidean norm
    //*  equal to 1 and largest component real.
    //*
    //*  Balancing a matrix means permuting the rows and columns to make it
    //*  more nearly upper triangular, and applying a diagonal similarity
    //*  transformation D * A * D**(-1), where D is a diagonal matrix, to
    //*  make its rows and columns closer in norm and the condition numbers
    //*  of its eigenvalues and eigenvectors smaller.  The computed
    //*  reciprocal condition numbers correspond to the balanced matrix.
    //*  Permuting rows and columns will not change the condition numbers
    //*  (in exact arithmetic) but diagonal scaling will.  For further
    //*  explanation of balancing, see section 4.10.2 of the LAPACK
    //*  Users' Guide.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  BALANC  (input) CHARACTER*1
    //*          Indicates how the input matrix should be diagonally scaled
    //*          and/or permuted to improve the conditioning of its
    //*          eigenvalues.
    //*          = 'N': Do not diagonally scale or permute;
    //*          = 'P': Perform permutations to make the matrix more nearly
    //*                 upper triangular. Do not diagonally scale;
    //*          = 'S': Diagonally scale the matrix, ie. replace A by
    //*                 D*A*D**(-1), where D is a diagonal matrix chosen
    //*                 to make the rows and columns of A more equal in
    //*                 norm. Do not permute;
    //*          = 'B': Both diagonally scale and permute A.
    //*
    //*          Computed reciprocal condition numbers will be for the matrix
    //*          after balancing and/or permuting. Permuting does not change
    //*          condition numbers (in exact arithmetic), but balancing does.
  
    char BALANC;
    if (balance)
      BALANC = 'B';
    else
      BALANC = 'N';

    //*  JOBVL   (input) CHARACTER*1
    //*          = 'N': left eigenvectors of A are not computed;
    //*          = 'V': left eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVL must = 'V'.
  
    char JOBVL = 'N';
  
    //*  JOBVR   (input) CHARACTER*1
    //*          = 'N': right eigenvectors of A are not computed;
    //*          = 'V': right eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVR must = 'V'.

    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';

    //*  SENSE   (input) CHARACTER*1
    //*          Determines which reciprocal condition numbers are computed.
    //*          = 'N': None are computed;
    //*          = 'E': Computed for eigenvalues only;
    //*          = 'V': Computed for right eigenvectors only;
    //*          = 'B': Computed for eigenvalues and right eigenvectors.
    //*
    //*          If SENSE = 'E' or 'B', both left and right eigenvectors
    //*          must also be computed (JOBVL = 'V' and JOBVR = 'V').

    char SENSE = 'N';

    //*  N       (input) INTEGER
    //*          The order of the matrix A. N >= 0.

    int N = n;
  
    //*  A       (input/output) REAL array, dimension (LDA,N)
    //*          On entry, the N-by-N matrix A.
    //*          On exit, A has been overwritten.  If JOBVL = 'V' or
    //*          JOBVR = 'V', A contains the real Schur form of the balanced
    //*          version of the input matrix A.

    float *Ain = a;

    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).

    int LDA = n;

    //*  W       (output) COMPLEX array, dimension (N)
    //*          W contains the computed eigenvalues.

    float *W = d;

    //*  VL      (output) COMPLEX array, dimension (LDVL,N)
    //*          If JOBVL = 'V', the left eigenvectors u(j) are stored one
    //*          after another in the columns of VL, in the same order
    //*          as their eigenvalues.
    //*          If JOBVL = 'N', VL is not referenced.
    //*          u(j) = VL(:,j), the j-th column of VL.
    //
    float *VL = NULL;

    //*  LDVL    (input) INTEGER
    //*          The leading dimension of the array VL.  LDVL >= 1; if
    //*          JOBVL = 'V', LDVL >= N.

    int LDVL = 1;

    //*  VR      (output) COMPLEX array, dimension (LDVR,N)
    //*          If JOBVR = 'V', the right eigenvectors v(j) are stored one
    //*          after another in the columns of VR, in the same order
    //*          as their eigenvalues.
    //*          If JOBVR = 'N', VR is not referenced.
    //*          v(j) = VR(:,j), the j-th column of VR.

    float *VR = v;

    //*  LDVR    (input) INTEGER
    //*          The leading dimension of the array VR.  LDVR >= 1, and if
    //*          JOBVR = 'V', LDVR >= N.

    int LDVR = n;

    //*  ILO,IHI (output) INTEGER
    //*          ILO and IHI are integer values determined when A was
    //*          balanced.  The balanced A(i,j) = 0 if I > J and
    //*          J = 1,...,ILO-1 or I = IHI+1,...,N.

    int ILO;
    int IHI;

    //*  SCALE   (output) REAL array, dimension (N)
    //*          Details of the permutations and scaling factors applied
    //*          when balancing A.  If P(j) is the index of the row and column
    //*          interchanged with row and column j, and D(j) is the scaling
    //*          factor applied to row and column j, then
    //*          SCALE(J) = P(J),    for J = 1,...,ILO-1
    //*                   = D(J),    for J = ILO,...,IHI
    //*                   = P(J)     for J = IHI+1,...,N.
    //*          The order in which the interchanges are made is N to IHI+1,
    //*          then 1 to ILO-1.

    float *SCALE = (float*) Malloc(n*sizeof(float));

    //*  ABNRM   (output) REAL
    //*          The one-norm of the balanced matrix (the maximum
    //*          of the sum of absolute values of elements of any column).

    float ABNRM;

    //*  RCONDE  (output) REAL array, dimension (N)
    //*          RCONDE(j) is the reciprocal condition number of the j-th
    //*          eigenvalue.

    float *RCONDE = (float*) Malloc(n*sizeof(float));

    //*  RCONDV  (output) REAL array, dimension (N)
    //*          RCONDV(j) is the reciprocal condition number of the j-th
    //*          right eigenvector.

    float *RCONDV = (float*) Malloc(n*sizeof(float));

    //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    //*  LWORK   (input) INTEGER
    //*          The dimension of the array WORK.  If SENSE = 'N' or 'E',
    //*          LWORK >= max(1,2*N), and if SENSE = 'V' or 'B',
    //*          LWORK >= N*N+2*N.
    //*          For good performance, LWORK must generally be larger.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.

    int LWORK;

    //*  RWORK   (workspace) REAL array, dimension (2*N)
    float *RWORK = (float*) Malloc(2*n*sizeof(float));

    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
    //*          > 0:  if INFO = i, the QR algorithm failed to compute all the
    //*                eigenvalues, and no eigenvectors or condition numbers
    //*                have been computed; elements 1:ILO-1 and i+1:N of W
    //*                contain eigenvalues which have converged.

    int INFO;

    float WORKSZE[2];

    LWORK = -1;
    cgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, WORKSZE, &LWORK, RWORK, &INFO );

    LWORK = (int) WORKSZE[0];
    float *WORK = (float*) Malloc(2*LWORK*sizeof(float));

    cgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, WORK, &LWORK, RWORK, &INFO );

    Free(WORK);
    Free(SCALE);
    Free(RCONDE);
    Free(RCONDV);
    Free(RWORK);
  }

  void dcomplexEigenDecomposeSymmetric(const int n, double *v, double *d,
				       double *a, bool eigenvectors) {
    //        SUBROUTINE ZHEEV( JOBZ, UPLO, N, A, LDA, W, WORK, LWORK, RWORK,
    //     $                  INFO )
    //*  Purpose
    //*  =======
    //*
    //*  ZHEEV computes all eigenvalues and, optionally, eigenvectors of a
    //*  complex Hermitian matrix A.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  JOBZ    (input) CHARACTER*1
    //*          = 'N':  Compute eigenvalues only;
    //*          = 'V':  Compute eigenvalues and eigenvectors.
    //*

    char JOBZ;
    if (eigenvectors)
      JOBZ = 'V';
    else
      JOBZ = 'N';

    //*  UPLO    (input) CHARACTER*1
    //*          = 'U':  Upper triangle of A is stored;
    //*          = 'L':  Lower triangle of A is stored.
    //*
    char UPLO = 'U';

    //*  N       (input) INTEGER
    //*          The order of the matrix A.  N >= 0.
    //*
    int N = n;

    //*  A       (input/output) COMPLEX array, dimension (LDA, N)
    //*          On entry, the Hermitian matrix A.  If UPLO = 'U', the
    //*          leading N-by-N upper triangular part of A contains the
    //*          upper triangular part of the matrix A.  If UPLO = 'L',
    //*          the leading N-by-N lower triangular part of A contains
    //*          the lower triangular part of the matrix A.
    //*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the
    //*          orthonormal eigenvectors of the matrix A.
    //*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')
    //*          or the upper triangle (if UPLO='U') of A, including the
    //*          diagonal, is destroyed.
    //*
    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).
    //*
    
    int LDA = n;
    
    //*  W       (output) REAL array, dimension (N)
    //*          If INFO = 0, the eigenvalues in ascending order.
    //*

    //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
    //*

    //*  LWORK   (input) INTEGER
    //*          The length of the array WORK.  LWORK >= max(1,2*N-1).
    //*          For optimal efficiency, LWORK >= (NB+1)*N,
    //*          where NB is the blocksize for CHETRD returned by ILAENV.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.
    //*

    //*  RWORK   (workspace) REAL array, dimension (max(1, 3*N-2))
    //*
    double *RWORK = (double*) Malloc(MAX(1,3*N-2)*sizeof(double));

    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value
    //*          > 0:  if INFO = i, the algorithm failed to converge; i
    //*                off-diagonal elements of an intermediate tridiagonal
    //*                form did not converge to zero.
    //*
    int LWORK;
    int INFO;
    double WORKSZE[2];
    LWORK = -1;
    zheev_(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,RWORK,&INFO);
    LWORK = (int) WORKSZE[0];
    double *WORK = (double*) Malloc(2*LWORK*sizeof(double));
    zheev_(&JOBZ,&UPLO,&N,a,&LDA,d,WORK,&LWORK,RWORK,&INFO);
    Free(WORK);
    if (eigenvectors)
      memcpy(v,a,2*n*n*sizeof(double));
  }

  void dcomplexEigenDecompose(const int n, double *v, double *d, 
			      double *a, bool eigenvectors, bool balance) {
    //	SUBROUTINE ZGEEVX( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, W, VL,
    //     $                   LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM, RCONDE,
    //     $                   RCONDV, WORK, LWORK, RWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  ZGEEVX computes for an N-by-N complex nonsymmetric matrix A, the
    //*  eigenvalues and, optionally, the left and/or right eigenvectors.
    //*
    //*  Optionally also, it computes a balancing transformation to improve
    //*  the conditioning of the eigenvalues and eigenvectors (ILO, IHI,
    //*  SCALE, and ABNRM), reciprocal condition numbers for the eigenvalues
    //*  (RCONDE), and reciprocal condition numbers for the right
    //*  eigenvectors (RCONDV).
    //*
    //*  The right eigenvector v(j) of A satisfies
    //*                   A * v(j) = lambda(j) * v(j)
    //*  where lambda(j) is its eigenvalue.
    //*  The left eigenvector u(j) of A satisfies
    //*                u(j)**H * A = lambda(j) * u(j)**H
    //*  where u(j)**H denotes the conjugate transpose of u(j).
    //*
    //*  The computed eigenvectors are normalized to have Euclidean norm
    //*  equal to 1 and largest component real.
    //*
    //*  Balancing a matrix means permuting the rows and columns to make it
    //*  more nearly upper triangular, and applying a diagonal similarity
    //*  transformation D * A * D**(-1), where D is a diagonal matrix, to
    //*  make its rows and columns closer in norm and the condition numbers
    //*  of its eigenvalues and eigenvectors smaller.  The computed
    //*  reciprocal condition numbers correspond to the balanced matrix.
    //*  Permuting rows and columns will not change the condition numbers
    //*  (in exact arithmetic) but diagonal scaling will.  For further
    //*  explanation of balancing, see section 4.10.2 of the LAPACK
    //*  Users' Guide.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  BALANC  (input) CHARACTER*1
    //*          Indicates how the input matrix should be diagonally scaled
    //*          and/or permuted to improve the conditioning of its
    //*          eigenvalues.
    //*          = 'N': Do not diagonally scale or permute;
    //*          = 'P': Perform permutations to make the matrix more nearly
    //*                 upper triangular. Do not diagonally scale;
    //*          = 'S': Diagonally scale the matrix, ie. replace A by
    //*                 D*A*D**(-1), where D is a diagonal matrix chosen
    //*                 to make the rows and columns of A more equal in
    //*                 norm. Do not permute;
    //*          = 'B': Both diagonally scale and permute A.
    //*
    //*          Computed reciprocal condition numbers will be for the matrix
    //*          after balancing and/or permuting. Permuting does not change
    //*          condition numbers (in exact arithmetic), but balancing does.
  
    char BALANC;
    if (balance)
      BALANC = 'B';
    else
      BALANC = 'N';

    //*  JOBVL   (input) CHARACTER*1
    //*          = 'N': left eigenvectors of A are not computed;
    //*          = 'V': left eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVL must = 'V'.
  
    char JOBVL = 'N';
  
    //*  JOBVR   (input) CHARACTER*1
    //*          = 'N': right eigenvectors of A are not computed;
    //*          = 'V': right eigenvectors of A are computed.
    //*          If SENSE = 'E' or 'B', JOBVR must = 'V'.

    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';

    //*  SENSE   (input) CHARACTER*1
    //*          Determines which reciprocal condition numbers are computed.
    //*          = 'N': None are computed;
    //*          = 'E': Computed for eigenvalues only;
    //*          = 'V': Computed for right eigenvectors only;
    //*          = 'B': Computed for eigenvalues and right eigenvectors.
    //*
    //*          If SENSE = 'E' or 'B', both left and right eigenvectors
    //*          must also be computed (JOBVL = 'V' and JOBVR = 'V').

    char SENSE = 'N';

    //*  N       (input) INTEGER
    //*          The order of the matrix A. N >= 0.

    int N = n;
  
    //*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
    //*          On entry, the N-by-N matrix A.
    //*          On exit, A has been overwritten.  If JOBVL = 'V' or
    //*          JOBVR = 'V', A contains the real Schur form of the balanced
    //*          version of the input matrix A.

    double *Ain = a;

    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).

    int LDA = n;

    //*  W       (output) COMPLEX*16 array, dimension (N)
    //*          W contains the computed eigenvalues.

    double *W = d;

    //*  VL      (output) COMPLEX*16 array, dimension (LDVL,N)
    //*          If JOBVL = 'V', the left eigenvectors u(j) are stored one
    //*          after another in the columns of VL, in the same order
    //*          as their eigenvalues.
    //*          If JOBVL = 'N', VL is not referenced.
    //*          u(j) = VL(:,j), the j-th column of VL.
    //
    double *VL = NULL;

    //*  LDVL    (input) INTEGER
    //*          The leading dimension of the array VL.  LDVL >= 1; if
    //*          JOBVL = 'V', LDVL >= N.

    int LDVL = 1;

    //*  VR      (output) COMPLEX*16 array, dimension (LDVR,N)
    //*          If JOBVR = 'V', the right eigenvectors v(j) are stored one
    //*          after another in the columns of VR, in the same order
    //*          as their eigenvalues.
    //*          If JOBVR = 'N', VR is not referenced.
    //*          v(j) = VR(:,j), the j-th column of VR.

    double *VR = v;

    //*  LDVR    (input) INTEGER
    //*          The leading dimension of the array VR.  LDVR >= 1, and if
    //*          JOBVR = 'V', LDVR >= N.

    int LDVR = n;

    //*  ILO,IHI (output) INTEGER
    //*          ILO and IHI are integer values determined when A was
    //*          balanced.  The balanced A(i,j) = 0 if I > J and
    //*          J = 1,...,ILO-1 or I = IHI+1,...,N.

    int ILO;
    int IHI;

    //*  SCALE   (output) DOUBLE PRECISION array, dimension (N)
    //*          Details of the permutations and scaling factors applied
    //*          when balancing A.  If P(j) is the index of the row and column
    //*          interchanged with row and column j, and D(j) is the scaling
    //*          factor applied to row and column j, then
    //*          SCALE(J) = P(J),    for J = 1,...,ILO-1
    //*                   = D(J),    for J = ILO,...,IHI
    //*                   = P(J)     for J = IHI+1,...,N.
    //*          The order in which the interchanges are made is N to IHI+1,
    //*          then 1 to ILO-1.

    double *SCALE = (double*) Malloc(n*sizeof(double));

    //*  ABNRM   (output) DOUBLE PRECISION
    //*          The one-norm of the balanced matrix (the maximum
    //*          of the sum of absolute values of elements of any column).

    double ABNRM;

    //*  RCONDE  (output) DOUBLE PRECISION array, dimension (N)
    //*          RCONDE(j) is the reciprocal condition number of the j-th
    //*          eigenvalue.

    double *RCONDE = (double*) Malloc(n*sizeof(double));

    //*  RCONDV  (output) DOUBLE PRECISION array, dimension (N)
    //*          RCONDV(j) is the reciprocal condition number of the j-th
    //*          right eigenvector.

    double *RCONDV = (double*) Malloc(n*sizeof(double));

    //*  WORK    (workspace/output) COMPLEX*16 array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    //*  LWORK   (input) INTEGER
    //*          The dimension of the array WORK.  If SENSE = 'N' or 'E',
    //*          LWORK >= max(1,2*N), and if SENSE = 'V' or 'B',
    //*          LWORK >= N*N+2*N.
    //*          For good performance, LWORK must generally be larger.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.

    int LWORK;

    //*  RWORK   (workspace) DOUBLE PRECISION array, dimension (2*N)
    double *RWORK = (double*) Malloc(2*n*sizeof(double));

    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
    //*          > 0:  if INFO = i, the QR algorithm failed to compute all the
    //*                eigenvalues, and no eigenvectors or condition numbers
    //*                have been computed; elements 1:ILO-1 and i+1:N of W
    //*                contain eigenvalues which have converged.

    int INFO;

    double WORKSZE[2];

    LWORK = -1;
    zgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, WORKSZE, &LWORK, RWORK, &INFO );

    LWORK = (int) WORKSZE[0];
    double *WORK = (double*) Malloc(2*LWORK*sizeof(double));

    zgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, W,
	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
	     RCONDE, RCONDV, WORK, &LWORK, RWORK, &INFO );

    Free(WORK);
    Free(SCALE);
    Free(RCONDE);
    Free(RCONDV);
    Free(RWORK);
  }
}
