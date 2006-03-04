      SUBROUTINE STIMTB( LINE, NN, NVAL, NK, KVAL, NNS, NSVAL, NLDA,
     $                   LDAVAL, TIMMIN, A, B, RESLTS, LDR1, LDR2, LDR3,
     $                   NOUT )
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     March 31, 1993
*
*     .. Scalar Arguments ..
      CHARACTER*80       LINE
      INTEGER            LDR1, LDR2, LDR3, NK, NLDA, NN, NNS, NOUT
      REAL               TIMMIN
*     ..
*     .. Array Arguments ..
      INTEGER            KVAL( * ), LDAVAL( * ), NSVAL( * ), NVAL( * )
      REAL               A( * ), B( * ), RESLTS( LDR1, LDR2, LDR3, * )
*     ..
*
*  Purpose
*  =======
*
*  STIMTB times STBTRS.
*
*  Arguments
*  =========
*
*  LINE    (input) CHARACTER*80
*          The input line that requested this routine.  The first six
*          characters contain either the name of a subroutine or a
*          generic path name.  The remaining characters may be used to
*          specify the individual routines to be timed.  See ATIMIN for
*          a full description of the format of the input line.
*
*  NN      (input) INTEGER
*          The number of values of N contained in the vector NVAL.
*
*  NVAL    (input) INTEGER array, dimension (NN)
*          The values of the matrix size N.
*
*  NK      (input) INTEGER
*          The number of values of K contained in the vector KVAL.
*
*  KVAL    (input) INTEGER array, dimension (NK)
*          The values of the band width K.
*
*  NNS     (input) INTEGER
*          The number of values of NRHS contained in the vector NSVAL.
*
*  NSVAL   (input) INTEGER array, dimension (NNS)
*          The values of the number of right hand sides NRHS.
*
*  NLDA    (input) INTEGER
*          The number of values of LDA contained in the vector LDAVAL.
*
*  LDAVAL  (input) INTEGER array, dimension (NLDA)
*          The values of the leading dimension of the array A.
*
*  TIMMIN  (input) REAL
*          The minimum time a subroutine will be timed.
*
*  A       (workspace) REAL array, dimension (LDAMAX*NMAX)
*          where LDAMAX and NMAX are the maximum values permitted
*          for LDA and N.
*
*  B       (workspace) REAL array, dimension (LDAMAX*NMAX)
*
*  RESLTS  (output) REAL array, dimension
*                   (LDR1,LDR2,LDR3,NSUBS)
*          The timing results for each subroutine over the relevant
*          values of N, NB, and LDA.
*
*  LDR1    (input) INTEGER
*          The first dimension of RESLTS.  LDR1 >= max(1,NNB).
*
*  LDR2    (input) INTEGER
*          The second dimension of RESLTS.  LDR2 >= max(1,NN).
*
*  LDR3    (input) INTEGER
*          The third dimension of RESLTS.  LDR3 >= max(1,2*NLDA).
*
*  NOUT    (input) INTEGER
*          The unit number for output.
*
*  =====================================================================
*
*     .. Parameters ..
      INTEGER            NSUBS
      PARAMETER          ( NSUBS = 1 )
*     ..
*     .. Local Scalars ..
      CHARACTER          UPLO
      CHARACTER*3        PATH
      CHARACTER*6        CNAME
      INTEGER            I, I3, IC, ICL, IK, ILDA, IN, INFO, ISUB,
     $                   IUPLO, K, LDA, LDB, MAT, N, NRHS
      REAL               OPS, S1, S2, TIME, UNTIME
*     ..
*     .. Local Arrays ..
      LOGICAL            TIMSUB( NSUBS )
      CHARACTER          UPLOS( 2 )
      CHARACTER*6        SUBNAM( NSUBS )
*     ..
*     .. External Functions ..
      LOGICAL            LSAME
      REAL               SECOND, SMFLOP, SOPLA
      EXTERNAL           LSAME, SECOND, SMFLOP, SOPLA
*     ..
*     .. External Subroutines ..
      EXTERNAL           ATIMCK, ATIMIN, SPRTBL, STBTRS, STIMMG
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          MAX, MIN, REAL
*     ..
*     .. Data statements ..
      DATA               SUBNAM / 'STBTRS' /
      DATA               UPLOS / 'U', 'L' /
*     ..
*     .. Executable Statements ..
*
*     Extract the timing request from the input line.
*
      PATH( 1: 1 ) = 'Single precision'
      PATH( 2: 3 ) = 'TB'
      CALL ATIMIN( PATH, LINE, NSUBS, SUBNAM, TIMSUB, NOUT, INFO )
      IF( INFO.NE.0 )
     $   GO TO 110
*
*     Check that K+1 <= LDA for the input values.
*
      CNAME = LINE( 1: 6 )
      CALL ATIMCK( 0, CNAME, NK, KVAL, NLDA, LDAVAL, NOUT, INFO )
      IF( INFO.GT.0 ) THEN
         WRITE( NOUT, FMT = 9999 )CNAME
         GO TO 110
      END IF
*
*     Do for each value of N:
*
      DO 100 IN = 1, NN
         N = NVAL( IN )
         LDB = N
*
*        Do first for UPLO = 'U', then for UPLO = 'L'
*
         DO 60 IUPLO = 1, 2
            UPLO = UPLOS( IUPLO )
            IF( LSAME( UPLO, 'U' ) ) THEN
               MAT = 11
            ELSE
               MAT = -11
            END IF
*
*           Do for each value of LDA:
*
            DO 50 ILDA = 1, NLDA
               LDA = LDAVAL( ILDA )
               I3 = ( IUPLO-1 )*NLDA + ILDA
*
*              Do for each value of the band width K:
*
               DO 40 IK = 1, NK
                  K = KVAL( IK )
                  K = MAX( 0, MIN( K, N-1 ) )
*
*                 Time STBTRS
*
                  IF( TIMSUB( 1 ) ) THEN
                     CALL STIMMG( MAT, N, N, A, LDA, K, K )
                     DO 30 I = 1, NNS
                        NRHS = NSVAL( I )
                        CALL STIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                        IC = 0
                        S1 = SECOND( )
   10                   CONTINUE
                        CALL STBTRS( UPLO, 'No transpose', 'Non-unit',
     $                               N, K, NRHS, A, LDA, B, LDB, INFO )
                        S2 = SECOND( )
                        TIME = S2 - S1
                        IC = IC + 1
                        IF( TIME.LT.TIMMIN ) THEN
                           CALL STIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                           GO TO 10
                        END IF
*
*                       Subtract the time used in STIMMG.
*
                        ICL = 1
                        S1 = SECOND( )
   20                   CONTINUE
                        S2 = SECOND( )
                        UNTIME = S2 - S1
                        ICL = ICL + 1
                        IF( ICL.LE.IC ) THEN
                           CALL STIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                           GO TO 20
                        END IF
*
                        TIME = ( TIME-UNTIME ) / REAL( IC )
                        OPS = SOPLA( 'STBTRS', N, NRHS, K, K, 0 )
                        RESLTS( I, IK, I3, 1 ) = SMFLOP( OPS, TIME,
     $                     INFO )
   30                CONTINUE
                  END IF
   40          CONTINUE
   50       CONTINUE
   60    CONTINUE
*
*        Print a table of results.
*
         DO 90 ISUB = 1, NSUBS
            IF( .NOT.TIMSUB( ISUB ) )
     $         GO TO 90
*
*           Print header for routine names.
*
            IF( IN.EQ.1 .OR. CNAME.EQ.'STB   ' ) THEN
               WRITE( NOUT, FMT = 9998 )SUBNAM( ISUB )
               IF( NLDA.EQ.1 ) THEN
                  WRITE( NOUT, FMT = 9997 )LDAVAL( 1 )
               ELSE
                  DO 70 I = 1, NLDA
                     WRITE( NOUT, FMT = 9996 )I, LDAVAL( I )
   70             CONTINUE
               END IF
            END IF
*
            DO 80 IUPLO = 1, 2
               WRITE( NOUT, FMT = 9995 )SUBNAM( ISUB ), N,
     $            UPLOS( IUPLO )
               I3 = ( IUPLO-1 )*NLDA + 1
               IF( ISUB.EQ.1 ) THEN
                  CALL SPRTBL( 'NRHS', 'K', NNS, NSVAL, NK, KVAL, NLDA,
     $                         RESLTS( 1, 1, I3, 1 ), LDR1, LDR2, NOUT )
               END IF
   80       CONTINUE
   90    CONTINUE
  100 CONTINUE
*
  110 CONTINUE
 9999 FORMAT( 1X, A6, ' timing run not attempted', / )
 9998 FORMAT( / ' *** Speed of ', A6, ' in megaflops ***' )
 9997 FORMAT( 5X, 'with LDA = ', I5 )
 9996 FORMAT( 5X, 'line ', I2, ' with LDA = ', I5 )
 9995 FORMAT( / 5X, A6, ' with M =', I6, ', UPLO = ''', A1, '''', / )
      RETURN
*
*     End of STIMTB
*
      END
