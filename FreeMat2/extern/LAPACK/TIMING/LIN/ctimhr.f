      SUBROUTINE CTIMHR( LINE, NM, MVAL, NN, NVAL, NNB, NBVAL, NXVAL,
     $                   NLDA, LDAVAL, TIMMIN, A, TAU, B, WORK, RWORK,
     $                   RESLTS, LDR1, LDR2, LDR3, NOUT )
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     March 31, 1993
*
*     .. Scalar Arguments ..
      CHARACTER*80       LINE
      INTEGER            LDR1, LDR2, LDR3, NLDA, NM, NN, NNB, NOUT
      REAL               TIMMIN
*     ..
*     .. Array Arguments ..
      INTEGER            LDAVAL( * ), MVAL( * ), NBVAL( * ), NVAL( * ),
     $                   NXVAL( * )
      REAL               RESLTS( LDR1, LDR2, LDR3, * ), RWORK( * )
      COMPLEX            A( * ), B( * ), TAU( * ), WORK( * )
*     ..
*
*  Purpose
*  =======
*
*  CTIMHR times the LAPACK routines CGEHRD, CUNGHR, and CUNMHR.
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
*  NN      (input) INTEGER
*          The number of values of N contained in the vector NVAL.
*
*  NVAL    (input) INTEGER array, dimension (NN)
*          The values of the matrix column dimension N.
*
*  NNB     (input) INTEGER
*          The number of values of NB and NX contained in the
*          vectors NBVAL and NXVAL.  The blocking parameters are used
*          in pairs (NB,NX).
*
*  NBVAL   (input) INTEGER array, dimension (NNB)
*          The values of the blocksize NB.
*
*  NXVAL   (input) INTEGER array, dimension (NNB)
*          The values of the crossover point NX.
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
*  A       (workspace) COMPLEX array, dimension (LDAMAX*NMAX)
*          where LDAMAX and NMAX are the maximum values of LDA and N.
*
*  TAU     (workspace) COMPLEX array, dimension (min(M,N))
*
*  B       (workspace) COMPLEX array, dimension (LDAMAX*NMAX)
*
*  WORK    (workspace) COMPLEX array, dimension (LDAMAX*NBMAX)
*          where NBMAX is the maximum value of NB.
*
*  RWORK   (workspace) REAL array, dimension
*                      (min(MMAX,NMAX))
*
*  RESLTS  (workspace) REAL array, dimension
*                      (LDR1,LDR2,LDR3,4*NN+3)
*          The timing results for each subroutine over the relevant
*          values of M, (NB,NX), LDA, and N.
*
*  LDR1    (input) INTEGER
*          The first dimension of RESLTS.  LDR1 >= max(1,NNB).
*
*  LDR2    (input) INTEGER
*          The second dimension of RESLTS.  LDR2 >= max(1,NM).
*
*  LDR3    (input) INTEGER
*          The third dimension of RESLTS.  LDR3 >= max(1,NLDA).
*
*  NOUT    (input) INTEGER
*          The unit number for output.
*
*  Internal Parameters
*  ===================
*
*  MODE    INTEGER
*          The matrix type.  MODE = 3 is a geometric distribution of
*          eigenvalues.  See CLATMS for further details.
*
*  COND    REAL
*          The condition number of the matrix.  The singular values are
*          set to values from DMAX to DMAX/COND.
*
*  DMAX    REAL
*          The magnitude of the largest singular value.
*
*  =====================================================================
*
*     .. Parameters ..
      INTEGER            NSUBS
      PARAMETER          ( NSUBS = 3 )
      INTEGER            MODE
      REAL               COND, DMAX
      PARAMETER          ( MODE = 3, COND = 100.0E0, DMAX = 1.0E0 )
*     ..
*     .. Local Scalars ..
      CHARACTER          LAB1, LAB2, SIDE, TRANS
      CHARACTER*3        PATH
      CHARACTER*6        CNAME
      INTEGER            I, I4, IC, ICL, IHI, ILDA, ILO, IM, IN, INB,
     $                   INFO, ISIDE, ISUB, ITOFF, ITRAN, LDA, LW, M,
     $                   M1, N, N1, NB, NX
      REAL               OPS, S1, S2, TIME, UNTIME
*     ..
*     .. Local Arrays ..
      LOGICAL            TIMSUB( NSUBS )
      CHARACTER          SIDES( 2 ), TRANSS( 2 )
      CHARACTER*6        SUBNAM( NSUBS )
      INTEGER            ISEED( 4 ), RESEED( 4 )
*     ..
*     .. External Functions ..
      REAL               SECOND, SMFLOP, SOPLA
      EXTERNAL           SECOND, SMFLOP, SOPLA
*     ..
*     .. External Subroutines ..
      EXTERNAL           ATIMCK, ATIMIN, CGEHRD, CLACPY, CLATMS, CTIMMG,
     $                   CUNGHR, CUNMHR, ICOPY, SPRTB3, SPRTBL, XLAENV
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          MAX, REAL
*     ..
*     .. Data statements ..
      DATA               SUBNAM / 'CGEHRD', 'CUNGHR', 'CUNMHR' /
      DATA               SIDES / 'L', 'R' / , TRANSS / 'N', 'C' /
      DATA               ISEED / 0, 0, 0, 1 /
*     ..
*     .. Executable Statements ..
*
*     Extract the timing request from the input line.
*
      PATH( 1: 1 ) = 'Complex precision'
      PATH( 2: 3 ) = 'HR'
      CALL ATIMIN( PATH, LINE, NSUBS, SUBNAM, TIMSUB, NOUT, INFO )
      IF( INFO.NE.0 )
     $   GO TO 190
*
*     Check that N <= LDA for the input values.
*
      CNAME = LINE( 1: 6 )
      CALL ATIMCK( 2, CNAME, NM, MVAL, NLDA, LDAVAL, NOUT, INFO )
      IF( INFO.GT.0 ) THEN
         WRITE( NOUT, FMT = 9999 )CNAME
         GO TO 190
      END IF
*
*     Check that K <= LDA for CUNMHR
*
      IF( TIMSUB( 3 ) ) THEN
         CALL ATIMCK( 3, CNAME, NN, NVAL, NLDA, LDAVAL, NOUT, INFO )
         IF( INFO.GT.0 ) THEN
            WRITE( NOUT, FMT = 9999 )SUBNAM( 3 )
            TIMSUB( 3 ) = .FALSE.
         END IF
      END IF
*
*     Do for each value of M:
*
      DO 120 IM = 1, NM
         M = MVAL( IM )
         ILO = 1
         IHI = M
         CALL ICOPY( 4, ISEED, 1, RESEED, 1 )
*
*        Do for each value of LDA:
*
         DO 110 ILDA = 1, NLDA
            LDA = LDAVAL( ILDA )
*
*           Do for each pair of values (NB, NX) in NBVAL and NXVAL.
*
            DO 100 INB = 1, NNB
               NB = NBVAL( INB )
               CALL XLAENV( 1, NB )
               NX = NXVAL( INB )
               CALL XLAENV( 3, NX )
               LW = MAX( 1, M*MAX( 1, NB ) )
*
*              Generate a test matrix of size M by M.
*
               CALL ICOPY( 4, RESEED, 1, ISEED, 1 )
               CALL CLATMS( M, M, 'Uniform', ISEED, 'Nonsymm', RWORK,
     $                      MODE, COND, DMAX, M, M, 'No packing', B,
     $                      LDA, WORK, INFO )
*
               IF( TIMSUB( 1 ) ) THEN
*
*                 CGEHRD:  Reduction to Hesenberg form
*
                  CALL CLACPY( 'Full', M, M, B, LDA, A, LDA )
                  IC = 0
                  S1 = SECOND( )
   10             CONTINUE
                  CALL CGEHRD( M, ILO, IHI, A, LDA, TAU, WORK, LW,
     $                         INFO )
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN ) THEN
                     CALL CLACPY( 'Full', M, M, B, LDA, A, LDA )
                     GO TO 10
                  END IF
*
*                 Subtract the time used in CLACPY.
*
                  ICL = 1
                  S1 = SECOND( )
   20             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
                  ICL = ICL + 1
                  IF( ICL.LE.IC ) THEN
                     CALL CLACPY( 'Full', M, M, A, LDA, B, LDA )
                     GO TO 20
                  END IF
*
                  TIME = ( TIME-UNTIME ) / REAL( IC )
                  OPS = SOPLA( 'CGEHRD', M, ILO, IHI, 0, NB )
                  RESLTS( INB, IM, ILDA, 1 ) = SMFLOP( OPS, TIME, INFO )
               ELSE
*
*                 If CGEHRD was not timed, generate a matrix and factor
*                 it using CGEHRD anyway so that the factored form of
*                 the matrix can be used in timing the other routines.
*
                  CALL CLACPY( 'Full', M, M, B, LDA, A, LDA )
                  CALL CGEHRD( M, ILO, IHI, A, LDA, TAU, WORK, LW,
     $                         INFO )
               END IF
*
               IF( TIMSUB( 2 ) ) THEN
*
*                 CUNGHR:  Generate the orthogonal matrix Q from the
*                 reduction to Hessenberg form A = Q*H*Q'
*
                  CALL CLACPY( 'Full', M, M, A, LDA, B, LDA )
                  IC = 0
                  S1 = SECOND( )
   30             CONTINUE
                  CALL CUNGHR( M, ILO, IHI, B, LDA, TAU, WORK, LW,
     $                         INFO )
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN ) THEN
                     CALL CLACPY( 'Full', M, M, A, LDA, B, LDA )
                     GO TO 30
                  END IF
*
*                 Subtract the time used in CLACPY.
*
                  ICL = 1
                  S1 = SECOND( )
   40             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
                  ICL = ICL + 1
                  IF( ICL.LE.IC ) THEN
                     CALL CLACPY( 'Full', M, M, A, LDA, B, LDA )
                     GO TO 40
                  END IF
*
                  TIME = ( TIME-UNTIME ) / REAL( IC )
*
*                 Op count for CUNGHR:  same as
*                    CUNGQR( IHI-ILO, IHI-ILO, IHI-ILO, ... )
*
                  OPS = SOPLA( 'CUNGQR', IHI-ILO, IHI-ILO, IHI-ILO, 0,
     $                  NB )
                  RESLTS( INB, IM, ILDA, 2 ) = SMFLOP( OPS, TIME, INFO )
               END IF
*
               IF( TIMSUB( 3 ) ) THEN
*
*                 CUNMHR:  Multiply by Q stored as a product of
*                 elementary transformations
*
                  I4 = 2
                  DO 90 ISIDE = 1, 2
                     SIDE = SIDES( ISIDE )
                     DO 80 IN = 1, NN
                        N = NVAL( IN )
                        LW = MAX( 1, MAX( 1, NB )*N )
                        IF( ISIDE.EQ.1 ) THEN
                           M1 = M
                           N1 = N
                        ELSE
                           M1 = N
                           N1 = M
                        END IF
                        ITOFF = 0
                        DO 70 ITRAN = 1, 2
                           TRANS = TRANSS( ITRAN )
                           CALL CTIMMG( 0, M1, N1, B, LDA, 0, 0 )
                           IC = 0
                           S1 = SECOND( )
   50                      CONTINUE
                           CALL CUNMHR( SIDE, TRANS, M1, N1, ILO, IHI,
     $                                  A, LDA, TAU, B, LDA, WORK, LW,
     $                                  INFO )
                           S2 = SECOND( )
                           TIME = S2 - S1
                           IC = IC + 1
                           IF( TIME.LT.TIMMIN ) THEN
                              CALL CTIMMG( 0, M1, N1, B, LDA, 0, 0 )
                              GO TO 50
                           END IF
*
*                          Subtract the time used in CTIMMG.
*
                           ICL = 1
                           S1 = SECOND( )
   60                      CONTINUE
                           S2 = SECOND( )
                           UNTIME = S2 - S1
                           ICL = ICL + 1
                           IF( ICL.LE.IC ) THEN
                              CALL CTIMMG( 0, M1, N1, B, LDA, 0, 0 )
                              GO TO 60
                           END IF
*
                           TIME = ( TIME-UNTIME ) / REAL( IC )
*
*                          Op count for CUNMHR, SIDE='L':  same as
*                          CUNMQR( 'L', TRANS, IHI-ILO, N, IHI-ILO, ...)
*
*                          Op count for CUNMHR, SIDE='R':  same as
*                          CUNMQR( 'R', TRANS, M, IHI-ILO, IHI-ILO, ...)
*
                           IF( ISIDE.EQ.1 ) THEN
                              OPS = SOPLA( 'CUNMQR', IHI-ILO, N1,
     $                              IHI-ILO, -1, NB )
                           ELSE
                              OPS = SOPLA( 'CUNMQR', M1, IHI-ILO,
     $                              IHI-ILO, 1, NB )
                           END IF
*
                           RESLTS( INB, IM, ILDA,
     $                        I4+ITOFF+IN ) = SMFLOP( OPS, TIME, INFO )
                           ITOFF = NN
   70                   CONTINUE
   80                CONTINUE
                     I4 = I4 + 2*NN
   90             CONTINUE
               END IF
*
  100       CONTINUE
  110    CONTINUE
  120 CONTINUE
*
*     Print tables of results for CGEHRD and CUNGHR
*
      DO 140 ISUB = 1, NSUBS - 1
         IF( .NOT.TIMSUB( ISUB ) )
     $      GO TO 140
         WRITE( NOUT, FMT = 9998 )SUBNAM( ISUB )
         IF( NLDA.GT.1 ) THEN
            DO 130 I = 1, NLDA
               WRITE( NOUT, FMT = 9997 )I, LDAVAL( I )
  130       CONTINUE
         END IF
         WRITE( NOUT, FMT = 9995 )
         CALL SPRTB3( '(  NB,  NX)', 'N', NNB, NBVAL, NXVAL, NM, MVAL,
     $                NLDA, RESLTS( 1, 1, 1, ISUB ), LDR1, LDR2, NOUT )
  140 CONTINUE
*
*     Print tables of results for CUNMHR
*
      ISUB = 3
      IF( TIMSUB( ISUB ) ) THEN
         I4 = 2
         DO 180 ISIDE = 1, 2
            IF( ISIDE.EQ.1 ) THEN
               LAB1 = 'M'
               LAB2 = 'N'
               IF( NLDA.GT.1 ) THEN
                  WRITE( NOUT, FMT = 9998 )SUBNAM( ISUB )
                  DO 150 I = 1, NLDA
                     WRITE( NOUT, FMT = 9997 )I, LDAVAL( I )
  150             CONTINUE
                  WRITE( NOUT, FMT = 9994 )
               END IF
            ELSE
               LAB1 = 'N'
               LAB2 = 'M'
            END IF
            DO 170 ITRAN = 1, 2
               DO 160 IN = 1, NN
                  WRITE( NOUT, FMT = 9996 )SUBNAM( ISUB ),
     $               SIDES( ISIDE ), TRANSS( ITRAN ), LAB2, NVAL( IN )
                  CALL SPRTBL( 'NB', LAB1, NNB, NBVAL, NM, MVAL, NLDA,
     $                         RESLTS( 1, 1, 1, I4+IN ), LDR1, LDR2,
     $                         NOUT )
  160          CONTINUE
               I4 = I4 + NN
  170       CONTINUE
  180    CONTINUE
      END IF
  190 CONTINUE
*
*     Print a table of results for each timed routine.
*
      RETURN
 9999 FORMAT( 1X, A6, ' timing run not attempted', / )
 9998 FORMAT( / ' *** Speed of ', A6, ' in megaflops *** ' )
 9997 FORMAT( 5X, 'line ', I2, ' with LDA = ', I5 )
 9996 FORMAT( / 5X, A6, ' with SIDE = ''', A1, ''', TRANS = ''', A1,
     $      ''', ', A1, ' =', I6, / )
 9995 FORMAT( / 5X, 'ILO = 1, IHI = N', / )
 9994 FORMAT( / 5X, 'ILO = 1, IHI = M if SIDE = ''L''', / 5X,
     $      '             = N if SIDE = ''R''' )
*
*     End of CTIMHR
*
      END
