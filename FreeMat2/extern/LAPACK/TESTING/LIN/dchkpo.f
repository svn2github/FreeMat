      SUBROUTINE DCHKPO( DOTYPE, NN, NVAL, NNB, NBVAL, NNS, NSVAL,
     $                   THRESH, TSTERR, NMAX, A, AFAC, AINV, B, X,
     $                   XACT, WORK, RWORK, IWORK, NOUT )
*
*  -- LAPACK test routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     December 7, 1999
*
*     .. Scalar Arguments ..
      LOGICAL            TSTERR
      INTEGER            NMAX, NN, NNB, NNS, NOUT
      DOUBLE PRECISION   THRESH
*     ..
*     .. Array Arguments ..
      LOGICAL            DOTYPE( * )
      INTEGER            IWORK( * ), NBVAL( * ), NSVAL( * ), NVAL( * )
      DOUBLE PRECISION   A( * ), AFAC( * ), AINV( * ), B( * ),
     $                   RWORK( * ), WORK( * ), X( * ), XACT( * )
*     ..
*
*  Purpose
*  =======
*
*  DCHKPO tests DPOTRF, -TRI, -TRS, -RFS, and -CON
*
*  Arguments
*  =========
*
*  DOTYPE  (input) LOGICAL array, dimension (NTYPES)
*          The matrix types to be used for testing.  Matrices of type j
*          (for 1 <= j <= NTYPES) are used for testing if DOTYPE(j) =
*          .TRUE.; if DOTYPE(j) = .FALSE., then type j is not used.
*
*  NN      (input) INTEGER
*          The number of values of N contained in the vector NVAL.
*
*  NVAL    (input) INTEGER array, dimension (NN)
*          The values of the matrix dimension N.
*
*  NNB     (input) INTEGER
*          The number of values of NB contained in the vector NBVAL.
*
*  NBVAL   (input) INTEGER array, dimension (NBVAL)
*          The values of the blocksize NB.
*
*  NNS     (input) INTEGER
*          The number of values of NRHS contained in the vector NSVAL.
*
*  NSVAL   (input) INTEGER array, dimension (NNS)
*          The values of the number of right hand sides NRHS.
*
*  THRESH  (input) DOUBLE PRECISION
*          The threshold value for the test ratios.  A result is
*          included in the output file if RESULT >= THRESH.  To have
*          every test ratio printed, use THRESH = 0.
*
*  TSTERR  (input) LOGICAL
*          Flag that indicates whether error exits are to be tested.
*
*  NMAX    (input) INTEGER
*          The maximum value permitted for N, used in dimensioning the
*          work arrays.
*
*  A       (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX)
*
*  AFAC    (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX)
*
*  AINV    (workspace) DOUBLE PRECISION array, dimension (NMAX*NMAX)
*
*  B       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX)
*          where NSMAX is the largest entry in NSVAL.
*
*  X       (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX)
*
*  XACT    (workspace) DOUBLE PRECISION array, dimension (NMAX*NSMAX)
*
*  WORK    (workspace) DOUBLE PRECISION array, dimension
*                      (NMAX*max(3,NSMAX))
*
*  RWORK   (workspace) DOUBLE PRECISION array, dimension
*                      (max(NMAX,2*NSMAX))
*
*  IWORK   (workspace) INTEGER array, dimension (NMAX)
*
*  NOUT    (input) INTEGER
*          The unit number for output.
*
*  =====================================================================
*
*     .. Parameters ..
      DOUBLE PRECISION   ZERO
      PARAMETER          ( ZERO = 0.0D+0 )
      INTEGER            NTYPES
      PARAMETER          ( NTYPES = 9 )
      INTEGER            NTESTS
      PARAMETER          ( NTESTS = 8 )
*     ..
*     .. Local Scalars ..
      LOGICAL            ZEROT
      CHARACTER          DIST, TYPE, UPLO, XTYPE
      CHARACTER*3        PATH
      INTEGER            I, IMAT, IN, INB, INFO, IOFF, IRHS, IUPLO,
     $                   IZERO, K, KL, KU, LDA, MODE, N, NB, NERRS,
     $                   NFAIL, NIMAT, NRHS, NRUN
      DOUBLE PRECISION   ANORM, CNDNUM, RCOND, RCONDC
*     ..
*     .. Local Arrays ..
      CHARACTER          UPLOS( 2 )
      INTEGER            ISEED( 4 ), ISEEDY( 4 )
      DOUBLE PRECISION   RESULT( NTESTS )
*     ..
*     .. External Functions ..
      DOUBLE PRECISION   DGET06, DLANSY
      EXTERNAL           DGET06, DLANSY
*     ..
*     .. External Subroutines ..
      EXTERNAL           ALAERH, ALAHD, ALASUM, DERRPO, DGET04, DLACPY,
     $                   DLARHS, DLATB4, DLATMS, DPOCON, DPORFS, DPOT01,
     $                   DPOT02, DPOT03, DPOT05, DPOTRF, DPOTRI, DPOTRS,
     $                   XLAENV
*     ..
*     .. Scalars in Common ..
      LOGICAL            LERR, OK
      CHARACTER*6        SRNAMT
      INTEGER            INFOT, NUNIT
*     ..
*     .. Common blocks ..
      COMMON             / INFOC / INFOT, NUNIT, OK, LERR
      COMMON             / SRNAMC / SRNAMT
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          MAX
*     ..
*     .. Data statements ..
      DATA               ISEEDY / 1988, 1989, 1990, 1991 /
      DATA               UPLOS / 'U', 'L' /
*     ..
*     .. Executable Statements ..
*
*     Initialize constants and the random number seed.
*
      PATH( 1: 1 ) = 'Double precision'
      PATH( 2: 3 ) = 'PO'
      NRUN = 0
      NFAIL = 0
      NERRS = 0
      DO 10 I = 1, 4
         ISEED( I ) = ISEEDY( I )
   10 CONTINUE
*
*     Test the error exits
*
      IF( TSTERR )
     $   CALL DERRPO( PATH, NOUT )
      INFOT = 0
      CALL XLAENV( 2, 2 )
*
*     Do for each value of N in NVAL
*
      DO 120 IN = 1, NN
         N = NVAL( IN )
         LDA = MAX( N, 1 )
         XTYPE = 'N'
         NIMAT = NTYPES
         IF( N.LE.0 )
     $      NIMAT = 1
*
         IZERO = 0
         DO 110 IMAT = 1, NIMAT
*
*           Do the tests only if DOTYPE( IMAT ) is true.
*
            IF( .NOT.DOTYPE( IMAT ) )
     $         GO TO 110
*
*           Skip types 3, 4, or 5 if the matrix size is too small.
*
            ZEROT = IMAT.GE.3 .AND. IMAT.LE.5
            IF( ZEROT .AND. N.LT.IMAT-2 )
     $         GO TO 110
*
*           Do first for UPLO = 'U', then for UPLO = 'L'
*
            DO 100 IUPLO = 1, 2
               UPLO = UPLOS( IUPLO )
*
*              Set up parameters with DLATB4 and generate a test matrix
*              with DLATMS.
*
               CALL DLATB4( PATH, IMAT, N, N, TYPE, KL, KU, ANORM, MODE,
     $                      CNDNUM, DIST )
*
               SRNAMT = 'DLATMS'
               CALL DLATMS( N, N, DIST, ISEED, TYPE, RWORK, MODE,
     $                      CNDNUM, ANORM, KL, KU, UPLO, A, LDA, WORK,
     $                      INFO )
*
*              Check error code from DLATMS.
*
               IF( INFO.NE.0 ) THEN
                  CALL ALAERH( PATH, 'DLATMS', INFO, 0, UPLO, N, N, -1,
     $                         -1, -1, IMAT, NFAIL, NERRS, NOUT )
                  GO TO 100
               END IF
*
*              For types 3-5, zero one row and column of the matrix to
*              test that INFO is returned correctly.
*
               IF( ZEROT ) THEN
                  IF( IMAT.EQ.3 ) THEN
                     IZERO = 1
                  ELSE IF( IMAT.EQ.4 ) THEN
                     IZERO = N
                  ELSE
                     IZERO = N / 2 + 1
                  END IF
                  IOFF = ( IZERO-1 )*LDA
*
*                 Set row and column IZERO of A to 0.
*
                  IF( IUPLO.EQ.1 ) THEN
                     DO 20 I = 1, IZERO - 1
                        A( IOFF+I ) = ZERO
   20                CONTINUE
                     IOFF = IOFF + IZERO
                     DO 30 I = IZERO, N
                        A( IOFF ) = ZERO
                        IOFF = IOFF + LDA
   30                CONTINUE
                  ELSE
                     IOFF = IZERO
                     DO 40 I = 1, IZERO - 1
                        A( IOFF ) = ZERO
                        IOFF = IOFF + LDA
   40                CONTINUE
                     IOFF = IOFF - IZERO
                     DO 50 I = IZERO, N
                        A( IOFF+I ) = ZERO
   50                CONTINUE
                  END IF
               ELSE
                  IZERO = 0
               END IF
*
*              Do for each value of NB in NBVAL
*
               DO 90 INB = 1, NNB
                  NB = NBVAL( INB )
                  CALL XLAENV( 1, NB )
*
*                 Compute the L*L' or U'*U factorization of the matrix.
*
                  CALL DLACPY( UPLO, N, N, A, LDA, AFAC, LDA )
                  SRNAMT = 'DPOTRF'
                  CALL DPOTRF( UPLO, N, AFAC, LDA, INFO )
*
*                 Check error code from DPOTRF.
*
                  IF( INFO.NE.IZERO ) THEN
                     CALL ALAERH( PATH, 'DPOTRF', INFO, IZERO, UPLO, N,
     $                            N, -1, -1, NB, IMAT, NFAIL, NERRS,
     $                            NOUT )
                     GO TO 90
                  END IF
*
*                 Skip the tests if INFO is not 0.
*
                  IF( INFO.NE.0 )
     $               GO TO 90
*
*+    TEST 1
*                 Reconstruct matrix from factors and compute residual.
*
                  CALL DLACPY( UPLO, N, N, AFAC, LDA, AINV, LDA )
                  CALL DPOT01( UPLO, N, A, LDA, AINV, LDA, RWORK,
     $                         RESULT( 1 ) )
*
*+    TEST 2
*                 Form the inverse and compute the residual.
*
                  CALL DLACPY( UPLO, N, N, AFAC, LDA, AINV, LDA )
                  SRNAMT = 'DPOTRI'
                  CALL DPOTRI( UPLO, N, AINV, LDA, INFO )
*
*                 Check error code from DPOTRI.
*
                  IF( INFO.NE.0 )
     $               CALL ALAERH( PATH, 'DPOTRI', INFO, 0, UPLO, N, N,
     $                            -1, -1, -1, IMAT, NFAIL, NERRS, NOUT )
*
                  CALL DPOT03( UPLO, N, A, LDA, AINV, LDA, WORK, LDA,
     $                         RWORK, RCONDC, RESULT( 2 ) )
*
*                 Print information about the tests that did not pass
*                 the threshold.
*
                  DO 60 K = 1, 2
                     IF( RESULT( K ).GE.THRESH ) THEN
                        IF( NFAIL.EQ.0 .AND. NERRS.EQ.0 )
     $                     CALL ALAHD( NOUT, PATH )
                        WRITE( NOUT, FMT = 9999 )UPLO, N, NB, IMAT, K,
     $                     RESULT( K )
                        NFAIL = NFAIL + 1
                     END IF
   60             CONTINUE
                  NRUN = NRUN + 2
*
*                 Skip the rest of the tests unless this is the first
*                 blocksize.
*
                  IF( INB.NE.1 )
     $               GO TO 90
*
                  DO 80 IRHS = 1, NNS
                     NRHS = NSVAL( IRHS )
*
*+    TEST 3
*                 Solve and compute residual for A * X = B .
*
                     SRNAMT = 'DLARHS'
                     CALL DLARHS( PATH, XTYPE, UPLO, ' ', N, N, KL, KU,
     $                            NRHS, A, LDA, XACT, LDA, B, LDA,
     $                            ISEED, INFO )
                     CALL DLACPY( 'Full', N, NRHS, B, LDA, X, LDA )
*
                     SRNAMT = 'DPOTRS'
                     CALL DPOTRS( UPLO, N, NRHS, AFAC, LDA, X, LDA,
     $                            INFO )
*
*                 Check error code from DPOTRS.
*
                     IF( INFO.NE.0 )
     $                  CALL ALAERH( PATH, 'DPOTRS', INFO, 0, UPLO, N,
     $                               N, -1, -1, NRHS, IMAT, NFAIL,
     $                               NERRS, NOUT )
*
                     CALL DLACPY( 'Full', N, NRHS, B, LDA, WORK, LDA )
                     CALL DPOT02( UPLO, N, NRHS, A, LDA, X, LDA, WORK,
     $                            LDA, RWORK, RESULT( 3 ) )
*
*+    TEST 4
*                 Check solution from generated exact solution.
*
                     CALL DGET04( N, NRHS, X, LDA, XACT, LDA, RCONDC,
     $                            RESULT( 4 ) )
*
*+    TESTS 5, 6, and 7
*                 Use iterative refinement to improve the solution.
*
                     SRNAMT = 'DPORFS'
                     CALL DPORFS( UPLO, N, NRHS, A, LDA, AFAC, LDA, B,
     $                            LDA, X, LDA, RWORK, RWORK( NRHS+1 ),
     $                            WORK, IWORK, INFO )
*
*                 Check error code from DPORFS.
*
                     IF( INFO.NE.0 )
     $                  CALL ALAERH( PATH, 'DPORFS', INFO, 0, UPLO, N,
     $                               N, -1, -1, NRHS, IMAT, NFAIL,
     $                               NERRS, NOUT )
*
                     CALL DGET04( N, NRHS, X, LDA, XACT, LDA, RCONDC,
     $                            RESULT( 5 ) )
                     CALL DPOT05( UPLO, N, NRHS, A, LDA, B, LDA, X, LDA,
     $                            XACT, LDA, RWORK, RWORK( NRHS+1 ),
     $                            RESULT( 6 ) )
*
*                    Print information about the tests that did not pass
*                    the threshold.
*
                     DO 70 K = 3, 7
                        IF( RESULT( K ).GE.THRESH ) THEN
                           IF( NFAIL.EQ.0 .AND. NERRS.EQ.0 )
     $                        CALL ALAHD( NOUT, PATH )
                           WRITE( NOUT, FMT = 9998 )UPLO, N, NRHS,
     $                        IMAT, K, RESULT( K )
                           NFAIL = NFAIL + 1
                        END IF
   70                CONTINUE
                     NRUN = NRUN + 5
   80             CONTINUE
*
*+    TEST 8
*                 Get an estimate of RCOND = 1/CNDNUM.
*
                  ANORM = DLANSY( '1', UPLO, N, A, LDA, RWORK )
                  SRNAMT = 'DPOCON'
                  CALL DPOCON( UPLO, N, AFAC, LDA, ANORM, RCOND, WORK,
     $                         IWORK, INFO )
*
*                 Check error code from DPOCON.
*
                  IF( INFO.NE.0 )
     $               CALL ALAERH( PATH, 'DPOCON', INFO, 0, UPLO, N, N,
     $                            -1, -1, -1, IMAT, NFAIL, NERRS, NOUT )
*
                  RESULT( 8 ) = DGET06( RCOND, RCONDC )
*
*                 Print the test ratio if it is .GE. THRESH.
*
                  IF( RESULT( 8 ).GE.THRESH ) THEN
                     IF( NFAIL.EQ.0 .AND. NERRS.EQ.0 )
     $                  CALL ALAHD( NOUT, PATH )
                     WRITE( NOUT, FMT = 9997 )UPLO, N, IMAT, 8,
     $                  RESULT( 8 )
                     NFAIL = NFAIL + 1
                  END IF
                  NRUN = NRUN + 1
   90          CONTINUE
  100       CONTINUE
  110    CONTINUE
  120 CONTINUE
*
*     Print a summary of the results.
*
      CALL ALASUM( PATH, NOUT, NFAIL, NRUN, NERRS )
*
 9999 FORMAT( ' UPLO = ''', A1, ''', N =', I5, ', NB =', I4, ', type ',
     $      I2, ', test ', I2, ', ratio =', G12.5 )
 9998 FORMAT( ' UPLO = ''', A1, ''', N =', I5, ', NRHS=', I3, ', type ',
     $      I2, ', test(', I2, ') =', G12.5 )
 9997 FORMAT( ' UPLO = ''', A1, ''', N =', I5, ',', 10X, ' type ', I2,
     $      ', test(', I2, ') =', G12.5 )
      RETURN
*
*     End of DCHKPO
*
      END
