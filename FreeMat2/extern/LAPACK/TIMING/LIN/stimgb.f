      SUBROUTINE STIMGB( LINE, NM, MVAL, NK, KVAL, NNS, NSVAL, NNB,
     $                   NBVAL, NLDA, LDAVAL, TIMMIN, A, B, IWORK,
     $                   RESLTS, LDR1, LDR2, LDR3, NOUT )
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     March 31, 1993
*
*     .. Scalar Arguments ..
      CHARACTER*80       LINE
      INTEGER            LDR1, LDR2, LDR3, NK, NLDA, NM, NNB, NNS, NOUT
      REAL               TIMMIN
*     ..
*     .. Array Arguments ..
      INTEGER            IWORK( * ), KVAL( * ), LDAVAL( * ), MVAL( * ),
     $                   NBVAL( * ), NSVAL( * )
      REAL               A( * ), B( * ), RESLTS( LDR1, LDR2, LDR3, * )
*     ..
*
*  Purpose
*  =======
*
*  STIMGB times SGBTRF and -TRS.
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
*  NM      (input) INTEGER
*          The number of values of M contained in the vector MVAL.
*
*  MVAL    (input) INTEGER array, dimension (NM)
*          The values of the matrix size M.
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
*  NNB     (input) INTEGER
*          The number of values of NB contained in the vector NBVAL.
*
*  NBVAL   (input) INTEGER array, dimension (NNB)
*          The values of the blocksize NB.
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
*  IWORK   (workspace) INTEGER array, dimension (NMAX)
*
*  RESLTS  (output) REAL array, dimension
*                   (LDR1,LDR2,LDR3,NSUBS)
*          The timing results for each subroutine over the relevant
*          values of N, K, NB, and LDA.
*
*  LDR1    (input) INTEGER
*          The first dimension of RESLTS.  LDR1 >= max(4,NNB).
*
*  LDR2    (input) INTEGER
*          The second dimension of RESLTS.  LDR2 >= max(1,NK).
*
*  LDR3    (input) INTEGER
*          The third dimension of RESLTS.  LDR3 >= max(1,NLDA).
*
*  NOUT    (input) INTEGER
*          The unit number for output.
*
*  =====================================================================
*
*     .. Parameters ..
      INTEGER            NSUBS
      PARAMETER          ( NSUBS = 2 )
*     ..
*     .. Local Scalars ..
      CHARACTER*3        PATH
      CHARACTER*6        CNAME
      INTEGER            I, IC, ICL, IK, ILDA, IM, INB, INFO, ISUB, K,
     $                   KL, KU, LDA, LDB, M, N, NB, NRHS
      REAL               OPS, S1, S2, TIME, UNTIME
*     ..
*     .. Local Arrays ..
      LOGICAL            TIMSUB( NSUBS )
      CHARACTER*6        SUBNAM( NSUBS )
*     ..
*     .. External Functions ..
      REAL               SECOND, SMFLOP, SOPGB, SOPLA
      EXTERNAL           SECOND, SMFLOP, SOPGB, SOPLA
*     ..
*     .. External Subroutines ..
      EXTERNAL           ATIMCK, ATIMIN, SGBTRF, SGBTRS, SPRTBL, STIMMG,
     $                   XLAENV
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          MAX, MIN, REAL
*     ..
*     .. Data statements ..
      DATA               SUBNAM / 'SGBTRF', 'SGBTRS' /
*     ..
*     .. Executable Statements ..
*
*     Extract the timing request from the input line.
*
      PATH( 1: 1 ) = 'Single precision'
      PATH( 2: 3 ) = 'GB'
      CALL ATIMIN( PATH, LINE, NSUBS, SUBNAM, TIMSUB, NOUT, INFO )
      IF( INFO.NE.0 )
     $   GO TO 120
*
*     Check that 3*K+1 <= LDA for the input values.
*
      CNAME = LINE( 1: 6 )
      CALL ATIMCK( 0, CNAME, NK, KVAL, NLDA, LDAVAL, NOUT, INFO )
      IF( INFO.GT.0 ) THEN
         WRITE( NOUT, FMT = 9999 )CNAME
         GO TO 120
      END IF
*
*     Do for each value of the matrix size M:
*
      DO 110 IM = 1, NM
         M = MVAL( IM )
         N = M
*
*        Do for each value of LDA:
*
         DO 80 ILDA = 1, NLDA
            LDA = LDAVAL( ILDA )
*
*           Do for each value of the band width K:
*
            DO 70 IK = 1, NK
               K = KVAL( IK )
               KL = MAX( 0, MIN( K, M-1 ) )
               KU = MAX( 0, MIN( K, N-1 ) )
*
*              Time SGBTRF
*
               IF( TIMSUB( 1 ) ) THEN
*
*                 Do for each value of NB in NBVAL.  Only SGBTRF is
*                 timed in this loop since the other routines are
*                 independent of NB.
*
                  DO 30 INB = 1, NNB
                     NB = NBVAL( INB )
                     CALL XLAENV( 1, NB )
                     IC = 0
                     CALL STIMMG( 2, M, N, A, LDA, KL, KU )
                     S1 = SECOND( )
   10                CONTINUE
                     CALL SGBTRF( M, N, KL, KU, A, LDA, IWORK, INFO )
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN ) THEN
                        CALL STIMMG( 2, M, N, A, LDA, KL, KU )
                        GO TO 10
                     END IF
*
*                    Subtract the time used in STIMMG.
*
                     ICL = 1
                     S1 = SECOND( )
   20                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
                     ICL = ICL + 1
                     IF( ICL.LE.IC ) THEN
                        CALL STIMMG( 2, M, N, A, LDA, KL, KU )
                        GO TO 20
                     END IF
*
                     TIME = ( TIME-UNTIME ) / REAL( IC )
                     OPS = SOPGB( 'SGBTRF', M, N, KL, KU, IWORK )
                     RESLTS( INB, IK, ILDA, 1 ) = SMFLOP( OPS, TIME,
     $                  INFO )
   30             CONTINUE
               ELSE
                  IC = 0
                  CALL STIMMG( 2, M, N, A, LDA, KL, KU )
               END IF
*
*              Generate another matrix and factor it using SGBTRF so
*              that the factored form can be used in timing the other
*              routines.
*
               NB = 1
               CALL XLAENV( 1, NB )
               IF( IC.NE.1 )
     $            CALL SGBTRF( M, N, KL, KU, A, LDA, IWORK, INFO )
*
*              Time SGBTRS
*
               IF( TIMSUB( 2 ) ) THEN
                  DO 60 I = 1, NNS
                     NRHS = NSVAL( I )
                     LDB = N
                     IC = 0
                     CALL STIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                     S1 = SECOND( )
   40                CONTINUE
                     CALL SGBTRS( 'No transpose', N, KL, KU, NRHS, A,
     $                            LDA, IWORK, B, LDB, INFO )
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN ) THEN
                        CALL STIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                        GO TO 40
                     END IF
*
*                    Subtract the time used in STIMMG.
*
                     ICL = 1
                     S1 = SECOND( )
   50                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
                     ICL = ICL + 1
                     IF( ICL.LE.IC ) THEN
                        CALL STIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                        GO TO 50
                     END IF
*
                     TIME = ( TIME-UNTIME ) / REAL( IC )
                     OPS = SOPLA( 'SGBTRS', N, NRHS, KL, KU, 0 )
                     RESLTS( I, IK, ILDA, 2 ) = SMFLOP( OPS, TIME,
     $                  INFO )
   60             CONTINUE
               END IF
   70       CONTINUE
   80    CONTINUE
*
*        Print a table of results for each routine
*
         DO 100 ISUB = 1, NSUBS
            IF( .NOT.TIMSUB( ISUB ) )
     $         GO TO 100
*
*           Print header for routine names.
*
            IF( IM.EQ.1 .OR. CNAME.EQ.'SGB   ' ) THEN
               WRITE( NOUT, FMT = 9998 )SUBNAM( ISUB )
               IF( NLDA.EQ.1 ) THEN
                  WRITE( NOUT, FMT = 9997 )LDAVAL( 1 )
               ELSE
                  DO 90 I = 1, NLDA
                     WRITE( NOUT, FMT = 9996 )I, LDAVAL( I )
   90             CONTINUE
               END IF
            END IF
*
            WRITE( NOUT, FMT = 9995 )SUBNAM( ISUB ), N
            IF( ISUB.EQ.1 ) THEN
               CALL SPRTBL( 'NB', 'K', NNB, NBVAL, NK, KVAL, NLDA,
     $                      RESLTS( 1, 1, 1, 1 ), LDR1, LDR2, NOUT )
            ELSE IF( ISUB.EQ.2 ) THEN
               CALL SPRTBL( 'NRHS', 'K', NNS, NSVAL, NK, KVAL, NLDA,
     $                      RESLTS( 1, 1, 1, 2 ), LDR1, LDR2, NOUT )
            END IF
  100    CONTINUE
  110 CONTINUE
  120 CONTINUE
*
 9999 FORMAT( 1X, A6, ' timing run not attempted', / )
 9998 FORMAT( / ' *** Speed of ', A6, ' in megaflops ***' )
 9997 FORMAT( 5X, 'with LDA = ', I5 )
 9996 FORMAT( 5X, 'line ', I2, ' with LDA = ', I5 )
 9995 FORMAT( / 5X, A6, ' with M =', I6, / )
*
      RETURN
*
*     End of STIMGB
*
      END
