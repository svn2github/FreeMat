      SUBROUTINE CTIMTP( LINE, NN, NVAL, NNS, NSVAL, LA, TIMMIN, A, B,
     $                   RESLTS, LDR1, LDR2, LDR3, NOUT )
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     March 31, 1993
*
*     .. Scalar Arguments ..
      CHARACTER*80       LINE
      INTEGER            LA, LDR1, LDR2, LDR3, NN, NNS, NOUT
      REAL               TIMMIN
*     ..
*     .. Array Arguments ..
      INTEGER            NSVAL( * ), NVAL( * )
      REAL               RESLTS( LDR1, LDR2, LDR3, * )
      COMPLEX            A( * ), B( * )
*     ..
*
*  Purpose
*  =======
*
*  CTIMTP times CTPTRI and -TRS.
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
*  NNS     (input) INTEGER
*          The number of values of NRHS contained in the vector NSVAL.
*
*  NSVAL   (input) INTEGER array, dimension (NNS)
*          The values of the number of right hand sides NRHS.
*
*  LA      (input) INTEGER
*          The size of the arrays A and B.
*
*  TIMMIN  (input) REAL
*          The minimum time a subroutine will be timed.
*
*  A       (workspace) COMPLEX array, dimension (LA)
*
*  B       (workspace) COMPLEX array, dimension (NMAX*NMAX)
*          where NMAX is the maximum value of N in NVAL.
*
*  RESLTS  (output) REAL array, dimension
*                   (LDR1,LDR2,LDR3,NSUBS)
*          The timing results for each subroutine over the relevant
*          values of N.
*
*  LDR1    (input) INTEGER
*          The first dimension of RESLTS.  LDR1 >= 1.
*
*  LDR2    (input) INTEGER
*          The second dimension of RESLTS.  LDR2 >= max(1,NN).
*
*  LDR3    (input) INTEGER
*          The third dimension of RESLTS.  LDR3 >= 2.
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
      CHARACTER          UPLO
      CHARACTER*3        PATH
      CHARACTER*6        CNAME
      INTEGER            I, IC, ICL, IN, INFO, ISUB, IUPLO, LDA, LDB,
     $                   MAT, N, NRHS
      REAL               OPS, S1, S2, TIME, UNTIME
*     ..
*     .. Local Arrays ..
      LOGICAL            TIMSUB( NSUBS )
      CHARACTER          UPLOS( 2 )
      CHARACTER*6        SUBNAM( NSUBS )
      INTEGER            IDUMMY( 1 ), LAVAL( 1 )
*     ..
*     .. External Functions ..
      LOGICAL            LSAME
      REAL               SECOND, SMFLOP, SOPLA
      EXTERNAL           LSAME, SECOND, SMFLOP, SOPLA
*     ..
*     .. External Subroutines ..
      EXTERNAL           ATIMCK, ATIMIN, CTIMMG, CTPTRI, CTPTRS, SPRTBL
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          MOD, REAL
*     ..
*     .. Data statements ..
      DATA               SUBNAM / 'CTPTRI', 'CTPTRS' /
      DATA               UPLOS / 'U', 'L' /
*     ..
*     .. Executable Statements ..
*
*     Extract the timing request from the input line.
*
      PATH( 1: 1 ) = 'Complex precision'
      PATH( 2: 3 ) = 'TP'
      CALL ATIMIN( PATH, LINE, NSUBS, SUBNAM, TIMSUB, NOUT, INFO )
      IF( INFO.NE.0 )
     $   GO TO 100
*
*     Check that N*(N+1)/2 <= LA for the input values.
*
      CNAME = LINE( 1: 6 )
      LAVAL( 1 ) = LA
      CALL ATIMCK( 4, CNAME, NN, NVAL, 1, LAVAL, NOUT, INFO )
      IF( INFO.GT.0 ) THEN
         WRITE( NOUT, FMT = 9999 )CNAME
         GO TO 100
      END IF
*
*     Do first for UPLO = 'U', then for UPLO = 'L'
*
      DO 70 IUPLO = 1, 2
         UPLO = UPLOS( IUPLO )
         IF( LSAME( UPLO, 'U' ) ) THEN
            MAT = 12
         ELSE
            MAT = -12
         END IF
*
*        Do for each value of N:
*
         DO 60 IN = 1, NN
            N = NVAL( IN )
            LDA = N*( N+1 ) / 2
            LDB = N
            IF( MOD( N, 2 ).EQ.0 )
     $         LDB = LDB + 1
*
*           Time CTPTRI
*
            IF( TIMSUB( 1 ) ) THEN
               CALL CTIMMG( MAT, N, N, A, LDA, 0, 0 )
               IC = 0
               S1 = SECOND( )
   10          CONTINUE
               CALL CTPTRI( UPLO, 'Non-unit', N, A, INFO )
               S2 = SECOND( )
               TIME = S2 - S1
               IC = IC + 1
               IF( TIME.LT.TIMMIN ) THEN
                  CALL CTIMMG( MAT, N, N, A, LDA, 0, 0 )
                  GO TO 10
               END IF
*
*              Subtract the time used in CTIMMG.
*
               ICL = 1
               S1 = SECOND( )
   20          CONTINUE
               S2 = SECOND( )
               UNTIME = S2 - S1
               ICL = ICL + 1
               IF( ICL.LE.IC ) THEN
                  CALL CTIMMG( MAT, N, N, A, LDA, 0, 0 )
                  GO TO 20
               END IF
*
               TIME = ( TIME-UNTIME ) / REAL( IC )
               OPS = SOPLA( 'CTPTRI', N, N, 0, 0, 0 )
               RESLTS( 1, IN, IUPLO, 1 ) = SMFLOP( OPS, TIME, INFO )
            ELSE
*
*              Generate a triangular matrix A.
*
               CALL CTIMMG( MAT, N, N, A, LDA, 0, 0 )
            END IF
*
*           Time CTPTRS
*
            IF( TIMSUB( 2 ) ) THEN
               DO 50 I = 1, NNS
                  NRHS = NSVAL( I )
                  CALL CTIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                  IC = 0
                  S1 = SECOND( )
   30             CONTINUE
                  CALL CTPTRS( UPLO, 'No transpose', 'Non-unit', N,
     $                         NRHS, A, B, LDB, INFO )
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN ) THEN
                     CALL CTIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                     GO TO 30
                  END IF
*
*                 Subtract the time used in CTIMMG.
*
                  ICL = 1
                  S1 = SECOND( )
   40             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
                  ICL = ICL + 1
                  IF( ICL.LE.IC ) THEN
                     CALL CTIMMG( 0, N, NRHS, B, LDB, 0, 0 )
                     GO TO 40
                  END IF
*
                  TIME = ( TIME-UNTIME ) / REAL( IC )
                  OPS = SOPLA( 'CTPTRS', N, NRHS, 0, 0, 0 )
                  RESLTS( I, IN, IUPLO, 2 ) = SMFLOP( OPS, TIME, INFO )
   50          CONTINUE
            END IF
   60    CONTINUE
   70 CONTINUE
*
*     Print a table of results.
*
      DO 90 ISUB = 1, NSUBS
         IF( .NOT.TIMSUB( ISUB ) )
     $      GO TO 90
         WRITE( NOUT, FMT = 9998 )SUBNAM( ISUB )
         DO 80 IUPLO = 1, 2
            WRITE( NOUT, FMT = 9997 )SUBNAM( ISUB ), UPLOS( IUPLO )
            IF( ISUB.EQ.1 ) THEN
               CALL SPRTBL( ' ', 'N', 1, IDUMMY, NN, NVAL, 1,
     $                      RESLTS( 1, 1, IUPLO, 1 ), LDR1, LDR2, NOUT )
            ELSE IF( ISUB.EQ.2 ) THEN
               CALL SPRTBL( 'NRHS', 'N', NNS, NSVAL, NN, NVAL, 1,
     $                      RESLTS( 1, 1, IUPLO, 2 ), LDR1, LDR2, NOUT )
            END IF
   80    CONTINUE
   90 CONTINUE
*
  100 CONTINUE
 9999 FORMAT( 1X, A6, ' timing run not attempted', / )
 9998 FORMAT( / ' *** Speed of ', A6, ' in megaflops ***', / )
 9997 FORMAT( 5X, A6, ' with UPLO = ''', A1, '''', / )
      RETURN
*
*     End of CTIMTP
*
      END
