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

#include "EigenDecompose.hpp"
#include "LAPACK.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "Malloc.hpp"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

namespace FreeMat {

  void floatEigenDecomposeSymmetric(int n, float *v, float *d, float *a,
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

  void floatEigenDecompose(int n, float *v, float *d, float *a,
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

  void floatGenEigenDecompose(int n, float *v, float *d, float *a,
			      float *b, bool eigenvectors) {
    //    SUBROUTINE SGGEV( JOBVL, JOBVR, N, A, LDA, B, LDB, ALPHAR, ALPHAI,
    //     $                  BETA, VL, LDVL, VR, LDVR, WORK, LWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  SGGEV computes for a pair of N-by-N real nonsymmetric matrices (A,B)
    //*  the generalized eigenvalues, and optionally, the left and/or right
    //*  generalized eigenvectors.
    //*
    //*  A generalized eigenvalue for a pair of matrices (A,B) is a scalar
    //*  lambda or a ratio alpha/beta = lambda, such that A - lambda*B is
    //*  singular. It is usually represented as the pair (alpha,beta), as
    //*  there is a reasonable interpretation for beta=0, and even for both
    //*  being zero.
    //*
    //*  The right eigenvector v(j) corresponding to the eigenvalue lambda(j)
    //*  of (A,B) satisfies
    //*
    //*                   A * v(j) = lambda(j) * B * v(j).
    //*
    //*  The left eigenvector u(j) corresponding to the eigenvalue lambda(j)
    //*  of (A,B) satisfies
    //*
    //*                   u(j)**H * A  = lambda(j) * u(j)**H * B .
    //*
    //*  where u(j)**H is the conjugate-transpose of u(j).
    //*
    //*
    //*  Arguments
    //*  =========
    //*
    //*  JOBVL   (input) CHARACTER*1
    //*          = 'N':  do not compute the left generalized eigenvectors;
    //*          = 'V':  compute the left generalized eigenvectors.
    //*
    char JOBVL = 'N';
    //*  JOBVR   (input) CHARACTER*1
    //*          = 'N':  do not compute the right generalized eigenvectors;
    //*          = 'V':  compute the right generalized eigenvectors.
    //*
    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';
    //*  N       (input) INTEGER
    //*          The order of the matrices A, B, VL, and VR.  N >= 0.
    //*
    int N = n;
    //*  A       (input/output) REAL array, dimension (LDA, N)
    //*          On entry, the matrix A in the pair (A,B).
    //*          On exit, A has been overwritten.
    //*
    float *A = a;
    //*  LDA     (input) INTEGER
    //*          The leading dimension of A.  LDA >= max(1,N).
    //*
    int LDA = n;
    //*  B       (input/output) REAL array, dimension (LDB, N)
    //*          On entry, the matrix B in the pair (A,B).
    //*          On exit, B has been overwritten.
    //*
    float *B = b;
    //*  LDB     (input) INTEGER
    //*          The leading dimension of B.  LDB >= max(1,N).
    //*
    int LDB = n;
    //*  ALPHAR  (output) REAL array, dimension (N)
    //*  ALPHAI  (output) REAL array, dimension (N)
    //*  BETA    (output) REAL array, dimension (N)
    //*          On exit, (ALPHAR(j) + ALPHAI(j)*i)/BETA(j), j=1,...,N, will
    //*          be the generalized eigenvalues.  If ALPHAI(j) is zero, then
    //*          the j-th eigenvalue is real; if positive, then the j-th and
    //*          (j+1)-st eigenvalues are a complex conjugate pair, with
    //*          ALPHAI(j+1) negative.
    //*
    float *ALPHAR = (float*) Malloc(n*sizeof(float));
    float *ALPHAI = (float*) Malloc(n*sizeof(float));
    float *BETA = (float*) Malloc(n*sizeof(float));
    //*          Note: the quotients ALPHAR(j)/BETA(j) and ALPHAI(j)/BETA(j)
    //*          may easily over- or underflow, and BETA(j) may even be zero.
    //*          Thus, the user should avoid naively computing the ratio
    //*          alpha/beta.  However, ALPHAR and ALPHAI will be always less
    //*          than and usually comparable with norm(A) in magnitude, and
    //*          BETA always less than and usually comparable with norm(B).
    //*
    //*  VL      (output) REAL array, dimension (LDVL,N)
    //*          If JOBVL = 'V', the left eigenvectors u(j) are stored one
    //*          after another in the columns of VL, in the same order as
    //*          their eigenvalues. If the j-th eigenvalue is real, then
    //*          u(j) = VL(:,j), the j-th column of VL. If the j-th and
    //*          (j+1)-th eigenvalues form a complex conjugate pair, then
    //*          u(j) = VL(:,j)+i*VL(:,j+1) and u(j+1) = VL(:,j)-i*VL(:,j+1).
    //*          Each eigenvector will be scaled so the largest component have
    //*          abs(real part)+abs(imag. part)=1.
    //*          Not referenced if JOBVL = 'N'.
    //*
    float *VL = NULL;
    //*  LDVL    (input) INTEGER
    //*          The leading dimension of the matrix VL. LDVL >= 1, and
    //*          if JOBVL = 'V', LDVL >= N.
    //*
    int LDVL = 1;
    //*  VR      (output) REAL array, dimension (LDVR,N)
    //*          If JOBVR = 'V', the right eigenvectors v(j) are stored one
    //*          after another in the columns of VR, in the same order as
    //*          their eigenvalues. If the j-th eigenvalue is real, then
    //*          v(j) = VR(:,j), the j-th column of VR. If the j-th and
    //*          (j+1)-th eigenvalues form a complex conjugate pair, then
    //*          v(j) = VR(:,j)+i*VR(:,j+1) and v(j+1) = VR(:,j)-i*VR(:,j+1).
    //*          Each eigenvector will be scaled so the largest component have
    //*          abs(real part)+abs(imag. part)=1.
    //*          Not referenced if JOBVR = 'N'.
    //*
    float *VR = v;
    //*  LDVR    (input) INTEGER
    //*          The leading dimension of the matrix VR. LDVR >= 1, and
    //*          if JOBVR = 'V', LDVR >= N.
    //*
    int LDVR = n;
    //*  WORK    (workspace/output) REAL array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
    //*
    //*  LWORK   (input) INTEGER
    //*          The dimension of the array WORK.  LWORK >= max(1,8*N).
    //*          For good performance, LWORK must generally be larger.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.
    //*
    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
    //*          = 1,...,N:
    //*                The QZ iteration failed.  No eigenvectors have been
    //*                calculated, but ALPHAR(j), ALPHAI(j), and BETA(j)
    //*                should be correct for j=INFO+1,...,N.
    //*          > N:  =N+1: other than QZ iteration failed in SHGEQZ.
    //*                =N+2: error return from STGEVC.
    //  
    float WORKSZE;
    int LWORK = -1;
    int INFO;
    sggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHAR, ALPHAI,
	    BETA, VL, &LDVL, VR, &LDVR, &WORKSZE, &LWORK, &INFO );
    LWORK = (int) WORKSZE;
    float *WORK = (float*) Malloc(LWORK*sizeof(float));
    sggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHAR, ALPHAI,
	    BETA, VL, &LDVL, VR, &LDVR, WORK, &LWORK, &INFO );
    int i;
    for (i=0;i<n;i++) {
      d[2*i] = ALPHAR[i]/BETA[i];
      d[2*i+1] = ALPHAI[i]/BETA[i];
    }
    Free(ALPHAR);
    Free(BETA);
    Free(ALPHAI);
    Free(WORK);
  }
  
  bool floatGenEigenDecomposeSymmetric(int n, float *v, float *d,
				       float *a, float *b, bool eigenvectors) {
    //          SUBROUTINE SSYGV( ITYPE, JOBZ, UPLO, N, A, LDA, B, LDB, W, WORK,
    //     $                  LWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  SSYGV computes all the eigenvalues, and optionally, the eigenvectors
    //*  of a real generalized symmetric-definite eigenproblem, of the form
    //*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x.
    //*  Here A and B are assumed to be symmetric and B is also
    //*  positive definite.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  ITYPE   (input) INTEGER
    //*          Specifies the problem type to be solved:
    //*          = 1:  A*x = (lambda)*B*x
    //*          = 2:  A*B*x = (lambda)*x
    //*          = 3:  B*A*x = (lambda)*x
    //*
    int ITYPE = 1;
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
    //*          = 'U':  Upper triangles of A and B are stored;
    //*          = 'L':  Lower triangles of A and B are stored.
    //*
    char UPLO = 'U';
    //*  N       (input) INTEGER
    //*          The order of the matrices A and B.  N >= 0.
    //*
    int N = n;
    //*  A       (input/output) REAL array, dimension (LDA, N)
    //*          On entry, the symmetric matrix A.  If UPLO = 'U', the
    //*          leading N-by-N upper triangular part of A contains the
    //*          upper triangular part of the matrix A.  If UPLO = 'L',
    //*          the leading N-by-N lower triangular part of A contains
    //*          the lower triangular part of the matrix A.
    //*
    //*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the
    //*          matrix Z of eigenvectors.  The eigenvectors are normalized
    //*          as follows:
    //*          if ITYPE = 1 or 2, Z**T*B*Z = I;
    //*          if ITYPE = 3, Z**T*inv(B)*Z = I.
    //*          If JOBZ = 'N', then on exit the upper triangle (if UPLO='U')
    //*          or the lower triangle (if UPLO='L') of A, including the
    //*          diagonal, is destroyed.
    //*
    float *A = a;
    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).
    //*
    int LDA = n;
    //*  B       (input/output) REAL array, dimension (LDB, N)
    //*          On entry, the symmetric positive definite matrix B.
    //*          If UPLO = 'U', the leading N-by-N upper triangular part of B
    //*          contains the upper triangular part of the matrix B.
    //*          If UPLO = 'L', the leading N-by-N lower triangular part of B
    //*          contains the lower triangular part of the matrix B.
    //*
    //*          On exit, if INFO <= N, the part of B containing the matrix is
    //*          overwritten by the triangular factor U or L from the Cholesky
    //*          factorization B = U**T*U or B = L*L**T.
    //*
    float *B = b;
    //*  LDB     (input) INTEGER
    //*          The leading dimension of the array B.  LDB >= max(1,N).
    //*
    int LDB = n;
    //*  W       (output) REAL array, dimension (N)
    //*          If INFO = 0, the eigenvalues in ascending order.
    //*
    float *W = d;
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
    //*          > 0:  SPOTRF or SSYEV returned an error code:
    //*             <= N:  if INFO = i, SSYEV failed to converge;
    //*                    i off-diagonal elements of an intermediate
    //*                    tridiagonal form did not converge to zero;
    //*             > N:   if INFO = N + i, for 1 <= i <= N, then the leading
    //*                    minor of order i of B is not positive definite.
    //*                    The factorization of B could not be completed and
    //*                    no eigenvalues or eigenvectors were computed.
    float WORKSIZE;
    int LWORK = -1;
    int INFO;
    ssygv_( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORKSIZE, 
	    &LWORK, &INFO );
    LWORK = (int) WORKSIZE;
    float *WORK = (float*) Malloc(LWORK*sizeof(float));
    ssygv_( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, WORK, 
	    &LWORK, &INFO );
    Free(WORK);
    if (INFO>N) return false;
    if (eigenvectors)
      memcpy(v,a,n*n*sizeof(float));
    return true;
  }

  void doubleEigenDecomposeSymmetric(int n, double *v, double *d, 
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

  void doubleEigenDecompose(int n, double *v, double *d, double *a,
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
  
    int maxN = (N < 6) ? 6 : N;
    int LWORK = maxN*maxN*2;

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

    //     LWORK = -1;
    //     dgeevx_( &BALANC, &JOBVL, &JOBVR, &SENSE, &N, Ain, &LDA, WR, WI,
    // 	     VL, &LDVL, VR, &LDVR, &ILO, &IHI, SCALE, &ABNRM,
    // 	     RCONDE, RCONDV, &WORKSZE, &LWORK, IWORK, &INFO );
    
    //     LWORK = (int) WORKSZE;
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

  void doubleGenEigenDecompose(int n, double *v, double *d, double *a,
			      double *b, bool eigenvectors) {
    char JOBVL = 'N';
    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';
    int N = n;
    double *A = a;
    int LDA = n;
    double *B = b;
    int LDB = n;
    double *ALPHAR = (double*) Malloc(n*sizeof(double));
    double *ALPHAI = (double*) Malloc(n*sizeof(double));
    double *BETA = (double*) Malloc(n*sizeof(double));
    double *VL = NULL;
    int LDVL = 1;
    double *VR = v;
    int LDVR = n;
    double WORKSZE;
    int LWORK = -1;
    int INFO;
    dggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHAR, ALPHAI,
    	    BETA, VL, &LDVL, VR, &LDVR, &WORKSZE, &LWORK, &INFO );
    LWORK = (int) WORKSZE;
    double *WORK = (double*) Malloc(LWORK*sizeof(double));
    dggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHAR, ALPHAI,
    	    BETA, VL, &LDVL, VR, &LDVR, WORK, &LWORK, &INFO );
    int i;
    for (i=0;i<n;i++) {
      d[2*i] = ALPHAR[i]/BETA[i];
      d[2*i+1] = ALPHAI[i]/BETA[i];
    }
    Free(ALPHAR);
    Free(BETA);
    Free(ALPHAI);
    Free(WORK);
  }
  
  bool doubleGenEigenDecomposeSymmetric(int n, double *v, double *d,
				       double *a, double *b, bool eigenvectors) {
    int ITYPE = 1;
    char JOBZ;
    if (eigenvectors)
      JOBZ = 'V';
    else
      JOBZ = 'N';
    char UPLO = 'U';
    int N = n;
    double *A = a;
    int LDA = n;
    double *B = b;
    int LDB = n;
    double *W = d;
    double WORKSIZE;
    int LWORK = -1;
    int INFO;
    dsygv_( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, &WORKSIZE, 
	    &LWORK, &INFO );
    LWORK = (int) WORKSIZE;
    double *WORK = (double*) Malloc(LWORK*sizeof(double));
    dsygv_( &ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, W, WORK, 
	    &LWORK, &INFO );
    Free(WORK);
    if (INFO>N) return false;
    if (eigenvectors)
      memcpy(v,a,n*n*sizeof(double));
    return true;
  }

  void complexEigenDecomposeSymmetric(int n, float *v, float *d,
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
    Free(RWORK);
    if (eigenvectors)
      memcpy(v,a,2*n*n*sizeof(float));
  }

  void complexEigenDecompose(int n, float *v, float *d, float *a,
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

  void local_c_div(float *c, float *a, float *b) {
    double ratio, den;
    double abr, abi, cr;

    if( (abr = b[0]) < 0.)
      abr = - abr;
    if( (abi = b[1]) < 0.)
      abi = - abi;
    if( abr <= abi ) {
      if(abi == 0) {
	float af, bf;
	af = bf = abr;
	if (a[1] != 0 || a[0] != 0)
	  af = 1.;
	c[1] = c[0] = af / bf;
	return;
      }
      ratio = (double)b[0] / b[1] ;
      den = b[1] * (1 + ratio*ratio);
      cr = (a[0]*ratio + a[1]) / den;
      c[1] = (a[1]*ratio - a[0]) / den;
    } else {
      ratio = (double)b[1] / b[0] ;
      den = b[0] * (1 + ratio*ratio);
      cr = (a[0] + a[1]*ratio) / den;
      c[1] = (a[1] - a[0]*ratio) / den;
    }
    c[0] = cr;
  }

  void complexGenEigenDecompose(int n, float *v, float *d, float *a,
				float *b, bool eigenvectors) {
    //      SUBROUTINE CGGEV( JOBVL, JOBVR, N, A, LDA, B, LDB, ALPHA, BETA,
    //     $                  VL, LDVL, VR, LDVR, WORK, LWORK, RWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  CGGEV computes for a pair of N-by-N complex nonsymmetric matrices
    //*  (A,B), the generalized eigenvalues, and optionally, the left and/or
    //*  right generalized eigenvectors.
    //*
    //*  A generalized eigenvalue for a pair of matrices (A,B) is a scalar
    //*  lambda or a ratio alpha/beta = lambda, such that A - lambda*B is
    //*  singular. It is usually represented as the pair (alpha,beta), as
    //*  there is a reasonable interpretation for beta=0, and even for both
    //*  being zero.
    //*
    //*  The right generalized eigenvector v(j) corresponding to the
    //*  generalized eigenvalue lambda(j) of (A,B) satisfies
    //*
    //*               A * v(j) = lambda(j) * B * v(j).
    //*
    //*  The left generalized eigenvector u(j) corresponding to the
    //*  generalized eigenvalues lambda(j) of (A,B) satisfies
    //*
    //*               u(j)**H * A = lambda(j) * u(j)**H * B
    //*
    //*  where u(j)**H is the conjugate-transpose of u(j).
    //*
    //*  Arguments
    //*  =========
    //*
    //*  JOBVL   (input) CHARACTER*1
    //*          = 'N':  do not compute the left generalized eigenvectors;
    //*          = 'V':  compute the left generalized eigenvectors.
    char JOBVL = 'N';
    //*
    //*  JOBVR   (input) CHARACTER*1
    //*          = 'N':  do not compute the right generalized eigenvectors;
    //*          = 'V':  compute the right generalized eigenvectors.
    //*
    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';
    //*  N       (input) INTEGER
    //*          The order of the matrices A, B, VL, and VR.  N >= 0.
    //*
    int N = n;
    //*  A       (input/output) COMPLEX array, dimension (LDA, N)
    //*          On entry, the matrix A in the pair (A,B).
    //*          On exit, A has been overwritten.
    //*
    float *A = a;
    //*  LDA     (input) INTEGER
    //*          The leading dimension of A.  LDA >= max(1,N).
    //*
    int LDA = n;
    //*  B       (input/output) COMPLEX array, dimension (LDB, N)
    //*          On entry, the matrix B in the pair (A,B).
    //*          On exit, B has been overwritten.
    //*
    float *B = b;
    //*  LDB     (input) INTEGER
    //*          The leading dimension of B.  LDB >= max(1,N).
    //*
    int LDB = N;
    //*  ALPHA   (output) COMPLEX array, dimension (N)
    //*  BETA    (output) COMPLEX array, dimension (N)
    //*          On exit, ALPHA(j)/BETA(j), j=1,...,N, will be the
    //*          generalized eigenvalues.
    //*
    //*          Note: the quotients ALPHA(j)/BETA(j) may easily over- or
    //*          underflow, and BETA(j) may even be zero.  Thus, the user
    //*          should avoid naively computing the ratio alpha/beta.
    //*          However, ALPHA will be always less than and usually
    //*          comparable with norm(A) in magnitude, and BETA always less
    //*          than and usually comparable with norm(B).
    //*
    float *ALPHA = (float*) Malloc(2*n*sizeof(float));
    float *BETA = (float*) Malloc(2*n*sizeof(float));
    //*  VL      (output) COMPLEX array, dimension (LDVL,N)
    //*          If JOBVL = 'V', the left generalized eigenvectors u(j) are
    //*          stored one after another in the columns of VL, in the same
    //*          order as their eigenvalues.
    //*          Each eigenvector will be scaled so the largest component
    //*          will have abs(real part) + abs(imag. part) = 1.
    //*          Not referenced if JOBVL = 'N'.
    //*
    float *VL = NULL;
    //*  LDVL    (input) INTEGER
    //*          The leading dimension of the matrix VL. LDVL >= 1, and
    //*          if JOBVL = 'V', LDVL >= N.
    //*
    int LDVL = n;
    //*  VR      (output) COMPLEX array, dimension (LDVR,N)
    //*          If JOBVR = 'V', the right generalized eigenvectors v(j) are
    //*          stored one after another in the columns of VR, in the same
    //*          order as their eigenvalues.
    //*          Each eigenvector will be scaled so the largest component
    //*          will have abs(real part) + abs(imag. part) = 1.
    //*          Not referenced if JOBVR = 'N'.
    //*
    float *VR = v;
    //*  LDVR    (input) INTEGER
    //*          The leading dimension of the matrix VR. LDVR >= 1, and
    //*          if JOBVR = 'V', LDVR >= N.
    //*
    int LDVR = n;
    //*  WORK    (workspace/output) COMPLEX array, dimension (LWORK)
    //*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK.
    //*
    //*  LWORK   (input) INTEGER
    //*          The dimension of the array WORK.  LWORK >= max(1,2*N).
    //*          For good performance, LWORK must generally be larger.
    //*
    //*          If LWORK = -1, then a workspace query is assumed; the routine
    //*          only calculates the optimal size of the WORK array, returns
    //*          this value as the first entry of the WORK array, and no error
    //*          message related to LWORK is issued by XERBLA.
    //*
    //*  RWORK   (workspace/output) REAL array, dimension (8*N)
    //*
    float *RWORK = (float*) Malloc(8*n*sizeof(float));
    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value.
    //*          =1,...,N:
    //*                The QZ iteration failed.  No eigenvectors have been
    //*                calculated, but ALPHA(j) and BETA(j) should be
    //*                correct for j=INFO+1,...,N.
    //*          > N:  =N+1: other then QZ iteration failed in SHGEQZ,
    //*                =N+2: error return from STGEVC.
    //
    float WORKSIZE[2];
    int LWORK = -1;
    int INFO;
    cggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHA, BETA,
	    VL, &LDVL, VR, &LDVR, &WORKSIZE[0], &LWORK, RWORK, &INFO );
    LWORK = (int) WORKSIZE[0];
    float *WORK = (float*) Malloc(LWORK*2*sizeof(float));
    cggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHA, BETA,
	    VL, &LDVL, VR, &LDVR, WORK, &LWORK, RWORK, &INFO );
    int i;
    for (i=0;i<n;i++)
      local_c_div(d+2*i,ALPHA+2*i,BETA+2*i);
    Free(ALPHA);
    Free(BETA);
    Free(RWORK);
    Free(WORK);
  }

  bool complexGenEigenDecomposeSymmetric(int n, float *v, float *d,
					 float *a, float *b, 
					 bool eigenvectors) {
    //      SUBROUTINE CHEGV( ITYPE, JOBZ, UPLO, N, A, LDA, B, LDB, W, WORK,
    //     $                  LWORK, RWORK, INFO )
    //*  Purpose
    //*  =======
    //*
    //*  CHEGV computes all the eigenvalues, and optionally, the eigenvectors
    //*  of a complex generalized Hermitian-definite eigenproblem, of the form
    //*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x.
    //*  Here A and B are assumed to be Hermitian and B is also
    //*  positive definite.
    //*
    //*  Arguments
    //*  =========
    //*
    //*  ITYPE   (input) INTEGER
    //*          Specifies the problem type to be solved:
    //*          = 1:  A*x = (lambda)*B*x
    //*          = 2:  A*B*x = (lambda)*x
    //*          = 3:  B*A*x = (lambda)*x
    //*
    int ITYPE = 1;
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
    //*          = 'U':  Upper triangles of A and B are stored;
    //*          = 'L':  Lower triangles of A and B are stored.
    //*
    char UPLO = 'U';
    //*  N       (input) INTEGER
    //*          The order of the matrices A and B.  N >= 0.
    //*
    int N = n;
    //*  A       (input/output) COMPLEX array, dimension (LDA, N)
    //*          On entry, the Hermitian matrix A.  If UPLO = 'U', the
    //*          leading N-by-N upper triangular part of A contains the
    //*          upper triangular part of the matrix A.  If UPLO = 'L',
    //*          the leading N-by-N lower triangular part of A contains
    //*          the lower triangular part of the matrix A.
    //*
    //*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the
    //*          matrix Z of eigenvectors.  The eigenvectors are normalized
    //*          as follows:
    //*          if ITYPE = 1 or 2, Z**H*B*Z = I;
    //*          if ITYPE = 3, Z**H*inv(B)*Z = I.
    //*          If JOBZ = 'N', then on exit the upper triangle (if UPLO='U')
    //*          or the lower triangle (if UPLO='L') of A, including the
    //*          diagonal, is destroyed.
    //*
    float *A = a;
    //*  LDA     (input) INTEGER
    //*          The leading dimension of the array A.  LDA >= max(1,N).
    //*
    int LDA = n;
    //*  B       (input/output) COMPLEX array, dimension (LDB, N)
    //*          On entry, the Hermitian positive definite matrix B.
    //*          If UPLO = 'U', the leading N-by-N upper triangular part of B
    //*          contains the upper triangular part of the matrix B.
    //*          If UPLO = 'L', the leading N-by-N lower triangular part of B
    //*          contains the lower triangular part of the matrix B.
    //*
    //*          On exit, if INFO <= N, the part of B containing the matrix is
    //*          overwritten by the triangular factor U or L from the Cholesky
    //*          factorization B = U**H*U or B = L*L**H.
    //*
    float *B = b;
    //*  LDB     (input) INTEGER
    //*          The leading dimension of the array B.  LDB >= max(1,N).
    //*
    int LDB = n;
    //*  W       (output) REAL array, dimension (N)
    //*          If INFO = 0, the eigenvalues in ascending order.
    //*
    float *W = d;
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
    float *RWORK = (float*) Malloc(MAX(1,3*N-2)*sizeof(float));
    //*
    //*  INFO    (output) INTEGER
    //*          = 0:  successful exit
    //*          < 0:  if INFO = -i, the i-th argument had an illegal value
    //*          > 0:  CPOTRF or CHEEV returned an error code:
    //*             <= N:  if INFO = i, CHEEV failed to converge;
    //*                    i off-diagonal elements of an intermediate
    //*                    tridiagonal form did not converge to zero;
    //*             > N:   if INFO = N + i, for 1 <= i <= N, then the leading
    //*                    minor of order i of B is not positive definite.
    //*                    The factorization of B could not be completed and
    //*                    no eigenvalues or eigenvectors were computed.
    int INFO;
    int LWORK;
    LWORK = MAX(1,2*N-1);
    float *WORK = (float*) Malloc(2*LWORK*sizeof(float));
    chegv_(&ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, d, WORK,
	   &LWORK, RWORK, &INFO );    
    if (INFO>N) return false;
    if (eigenvectors)
      memcpy(v,a,2*n*n*sizeof(float));
    return true;
  }

  void dcomplexEigenDecomposeSymmetric(int n, double *v, double *d,
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
    Free(RWORK);
    if (eigenvectors)
      memcpy(v,a,2*n*n*sizeof(double));
  }

  void dcomplexEigenDecompose(int n, double *v, double *d, 
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

  void local_z_div(double *c, double *a, double *b) {
    double ratio, den;
    double abr, abi, cr;

    if( (abr = b[0]) < 0.)
      abr = - abr;
    if( (abi = b[1]) < 0.)
      abi = - abi;
    if( abr <= abi ) {
      if(abi == 0) {
	if (a[1] != 0 || a[0] != 0)
	  abi = 1.;
	c[1] = c[0] = abi / abr;
	return;
      }
      ratio = b[0] / b[1] ;
      den = b[1] * (1 + ratio*ratio);
      cr = (a[0]*ratio + a[1]) / den;
      c[1] = (a[1]*ratio - a[0]) / den;
    } else {
      ratio = b[1] / b[0] ;
      den = b[0] * (1 + ratio*ratio);
      cr = (a[0] + a[1]*ratio) / den;
      c[1] = (a[1] - a[0]*ratio) / den;
    }
    c[0] = cr;
  }

  bool dcomplexGenEigenDecomposeSymmetric(int n, double *v, double *d,
					  double *a, double *b, 
					  bool eigenvectors) {
    int ITYPE = 1;
    char JOBZ;
    if (eigenvectors)
      JOBZ = 'V';
    else
      JOBZ = 'N';
    char UPLO = 'U';
    int N = n;
    double *A = a;
    int LDA = n;
    double *B = b;
    int LDB = n;
    double *W = d;
    double *RWORK = (double*) Malloc(MAX(1,3*N-2)*sizeof(double));
    int INFO;
    int LWORK;
    double WORKSZE[2];
    LWORK = MAX(1,2*N-1);
    double *WORK = (double*) Malloc(LWORK*sizeof(double)*2);
    zhegv_(&ITYPE, &JOBZ, &UPLO, &N, A, &LDA, B, &LDB, d, WORK,
	   &LWORK, RWORK, &INFO );    
    Free(WORK);
    Free(RWORK);
    if (INFO>N) return false;
    if (eigenvectors)
      memcpy(v,a,2*n*n*sizeof(double));
    return true;
  }

  void dcomplexGenEigenDecompose(int n, double *v, double *d,
				 double *a, double *b, 
				 bool eigenvectors) {
    char JOBVL = 'N';
    char JOBVR;
    if (eigenvectors)
      JOBVR = 'V';
    else
      JOBVR = 'N';
    int N = n;
    double *A = a;
    int LDA = n;
    double *B = b;
    int LDB = N;
    double *ALPHA = (double*) Malloc(2*n*sizeof(double));
    double *BETA = (double*) Malloc(2*n*sizeof(double));
    double *VL = NULL;
    int LDVL = n;
    double *VR = v;
    int LDVR = n;
    double *RWORK = (double*) Malloc(8*n*sizeof(double));
    double WORKSIZE[2];
    int LWORK = -1;
    int INFO;
    zggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHA, BETA,
	    VL, &LDVL, VR, &LDVR, &WORKSIZE[0], &LWORK, RWORK, &INFO );
    LWORK = (int) WORKSIZE[0];
    double *WORK = (double*) Malloc(LWORK*2*sizeof(double));
    zggev_( &JOBVL, &JOBVR, &N, A, &LDA, B, &LDB, ALPHA, BETA,
	    VL, &LDVL, VR, &LDVR, WORK, &LWORK, RWORK, &INFO );
    int i;
    for (i=0;i<n;i++)
      local_z_div(d+2*i,ALPHA+2*i,BETA+2*i);
    Free(ALPHA);
    Free(BETA);
    Free(RWORK);
    Free(WORK);
  }
}
