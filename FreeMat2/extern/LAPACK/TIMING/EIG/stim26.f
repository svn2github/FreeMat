      SUBROUTINE STIM26( LINE, NSIZES, NN, MM, NTYPES, DOTYPE, NPARMS,
     $                   NNB, LDAS, TIMMIN, NOUT, ISEED, A, H, U, VT, D,
     $                   E, TAUP, TAUQ, WORK, LWORK, IWORK, LLWORK,
     $                   TIMES, LDT1, LDT2, LDT3, OPCNTS, LDO1, LDO2,
     $                   LDO3, INFO )
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     June 30, 1999
*
*     .. Scalar Arguments ..
      CHARACTER*80       LINE
      INTEGER            INFO, LDO1, LDO2, LDO3, LDT1, LDT2, LDT3,
     $                   LWORK, NOUT, NPARMS, NSIZES, NTYPES
      REAL               TIMMIN
*     ..
*     .. Array Arguments ..
      LOGICAL            DOTYPE( * ), LLWORK( * )
      INTEGER            ISEED( * ), IWORK( * ), LDAS( * ), MM( * ),
     $                   NN( * ), NNB( * )
      REAL               A( * ), D( * ), E( * ), H( * ),
     $                   OPCNTS( LDO1, LDO2, LDO3, * ), TAUP( * ),
     $                   TAUQ( * ), TIMES( LDT1, LDT2, LDT3, * ),
     $                   U( * ), VT( * ), WORK( * )
*     ..
*
*  Purpose
*  =======
*
*     STIM26 times the LAPACK routines for the REAL
*     singular value decomposition.
*
*     For each N value in NN(1:NSIZES), M value in MM(1:NSIZES),
*     and .TRUE. value in DOTYPE(1:NTYPES), a matrix will be generated
*     and used to test the selected routines.  Thus, NSIZES*(number of
*     .TRUE. values in DOTYPE) matrices will be generated.
*
*  Arguments
*  =========
*
*  LINE    (input) CHARACTER*80
*          On entry, LINE contains the input line which requested
*          this routine.  This line may contain a subroutine name,
*          such as SGEBRD, indicating that only routine SGEBRD will
*          be timed, or it may contain a generic name, such as SBD.
*          In this case, the rest of the line is scanned for the
*          first 11 non-blank characters, corresponding to the eleven
*          combinations of subroutine and options:
*          LAPACK:
*           1: SGEBRD
*              (labeled SGEBRD in the output)
*           2: SBDSQR (singular values only)
*              (labeled SBDSQR in the output)
*           3: SBDSQR (singular values and left singular vectors;
*                      assume original matrix M by N)
*              (labeled SBDSQR(L) in the output)
*           4: SBDSQR (singular values and right singular vectors;
*                      assume original matrix M by N)
*              (labeled SBDSQR(R) in the output)
*           5: SBDSQR (singular values and left and right singular
*                      vectors; assume original matrix M by N)
*              (labeled SBDSQR(B) in the output)
*           6: SBDSQR (singular value and multiply square MIN(M,N)
*                      matrix by transpose of left singular vectors)
*              (labeled SBDSQR(V) in the output)
*           7: SGEBRD+SBDSQR (singular values only)
*              (labeled LAPSVD in the output)
*           8: SGEBRD+SORGBR+SBDSQR(L) (singular values and min(M,N)
*                                       left singular vectors)
*              (labeled LAPSVD(l) in the output)
*           9: SGEBRD+SORGBR+SBDSQR(L) (singular values and M left
*                                       singular vectors)
*              (labeled LAPSVD(L) in the output)
*          10: SGEBRD+SORGBR+SBDSQR(R) (singular values and N right
*                                       singular vectors)
*              (labeled LAPSVD(R) in the output)
*          11: SGEBRD+SORGBR+SBDSQR(B) (singular values and min(M,N)
*                                       left singular vectors and N
*                                       right singular vectors)
*              (labeled LAPSVD(B) in the output)
*          12: SBDSDC (singular values and left and right singular
*                      vectors; assume original matrix min(M,N) by
*                      min(M,N))
*              (labeled SBDSDC(B) in the output)
*          13: SGESDD (singular values and min(M,N) left singular
*                      vectors and N right singular vectors if M>=N,
*                      singular values and M left singular vectors
*                      and min(M,N) right singular vectors otherwise.)
*              (labeled SGESDD(B) in the output)
*          LINPACK:
*          14: SSVDC (singular values only) (comparable to 7 above)
*              (labeled LINSVD in the output)
*          15: SSVDC (singular values and min(M,N) left singular
*                     vectors) (comparable to 8 above)
*              (labeled LINSVD(l) in the output)
*          16: SSVDC (singular values and M left singular vectors)
*                     (comparable to 9 above)
*              (labeled LINSVD(L) in the output)
*          17: SSVDC (singular values and N right singular vectors)
*                     (comparable to 10 above)
*              (labeled LINSVD(R) in the output)
*          18: SSVDC (singular values and min(M,N) left singular
*                     vectors and N right singular vectors)
*                     (comparable to 11 above)
*              (labeled LINSVD(B) in the output)
*
*          If a character is 'T' or 't', the corresponding routine in
*          this path is timed.  If the entire line is blank, all the
*          routines in the path are timed.
*
*  NSIZES  (input) INTEGER
*          The number of values of N contained in the vector NN.
*
*  NN      (input) INTEGER array, dimension( NSIZES )
*          The numbers of columns of the matrices to be tested.  For
*          each N value in the array NN, and each .TRUE. value in
*          DOTYPE, a matrix A will be generated and used to test the
*          routines.
*
*  MM      (input) INTEGER array, dimension( NSIZES )
*          The numbers of rows of the matrices to be tested.  For
*          each M value in the array MM, and each .TRUE. value in
*          DOTYPE, a matrix A will be generated and used to test the
*          routines.
*
*  NTYPES  (input) INTEGER
*          The number of types in DOTYPE.  Only the first MAXTYP
*          elements will be examined.  Exception: if NSIZES=1 and
*          NTYPES=MAXTYP+1, and DOTYPE=MAXTYP*f,t, then the input
*          value of A will be used.
*
*  DOTYPE  (input) LOGICAL
*          If DOTYPE(j) is .TRUE., then a matrix of type j will be
*          generated as follows:
*           j=1: A = U*D*V where U and V are random orthogonal
*                matrices and D has evenly spaced entries 1,...,ULP
*                with random signs on the diagonal
*           j=2: A = U*D*V where U and V are random orthogonal
*                matrices and D has geometrically spaced entries
*                1,...,ULP with random signs on the diagonal
*           j=3: A = U*D*V where U and V are random orthogonal
*                matrices and D has "clustered" entries
*                 1,ULP,...,ULP with random signs on the diagonal
*           j=4: A contains uniform random numbers from [-1,1]
*           j=5: A is a special nearly bidiagonal matrix, where the
*                upper bidiagonal entries are exp(-2*r*log(ULP))
*                and the nonbidiagonal entries are r*ULP, where r
*                is a uniform random number from [0,1]
*
*  NPARMS  (input) INTEGER
*          The number of values in each of the arrays NNB and LDAS.
*          For each matrix A generated according to NN, MM and DOTYPE,
*          tests will be run with (NB,,LDA)= (NNB(1), LDAS(1)),...,
*          (NNB(NPARMS), LDAS(NPARMS)).
*
*  NNB     (input) INTEGER array, dimension( NPARMS )
*          The values of the blocksize ("NB") to be tested.
*
*  LDAS    (input) INTEGER array, dimension( NPARMS )
*          The values of LDA, the leading dimension of all matrices,
*          to be tested.
*
*  TIMMIN  (input) REAL
*          The minimum time a subroutine will be timed.
*
*  NOUT    (input) INTEGER
*          If NOUT > 0 then NOUT specifies the unit number
*          on which the output will be printed.  If NOUT <= 0, no
*          output is printed.
*
*  ISEED   (input/output) INTEGER array, dimension( 4 )
*          The random seed used by the random number generator, used
*          by the test matrix generator.  It is used and updated on
*          each call to STIM26.
*
*  A       (workspace) REAL array,
*                      dimension( max(NN)*max(LDAS))
*          During the testing of SGEBRD, the original dense matrix.
*
*  H       (workspace) REAL array,
*                      dimension( max(NN)*max(LDAS))
*          The Householder vectors used to reduce A to bidiagonal
*          form (as returned by SGEBD2.)
*
*  U       (workspace) REAL array,
*                      dimension( max(NN,MM)*max(LDAS) )
*          The left singular vectors of the original matrix.
*
*  VT      (workspace) REAL array,
*                      dimension( max(NN,MM)*max(LDAS) )
*          The right singular vectors of the original matrix.
*
*  D       (workspace) REAL array, dimension( max(NN,MM) )
*          Diagonal entries of bidiagonal matrix to which A
*          is reduced.
*
*  E       (workspace) REAL array, dimension( max(NN,MM) )
*          Offdiagonal entries of bidiagonal matrix to which A
*          is reduced.
*
*  TAUP    (workspace) REAL array, dimension( max(NN,MM) )
*          The coefficients for the Householder transformations
*          applied on the right to reduce A to bidiagonal form.
*
*  TAUQ    (workspace) REAL array, dimension( max(NN,MM) )
*          The coefficients for the Householder transformations
*          applied on the left to reduce A to bidiagonal form.
*
*  WORK    (workspace) REAL array, dimension( LWORK )
*
*  LWORK   (input) INTEGER
*          Number of elements in WORK. Must be at least
*          MAX(6*MIN(M,N),3*MAX(M,N),NSIZES*NPARMS*NTYPES)
*
*  IWORK   (workspace) INTEGER array, dimension at least 8*min(M,N).
*
*  LLWORK  (workspace) LOGICAL array, dimension( NPARMS ),
*
*  TIMES   (output) REAL array,
*                   dimension (LDT1,LDT2,LDT3,NSUBS)
*          TIMES(i,j,k,l) will be set to the run time (in seconds) for
*          subroutine/path l, with N=NN(k), M=MM(k), matrix type j,
*          LDA=LDAS(i), and NBLOCK=NNB(i).
*
*  LDT1    (input) INTEGER
*          The first dimension of TIMES.  LDT1 >= min( 1, NPARMS ).
*
*  LDT2    (input) INTEGER
*          The second dimension of TIMES.  LDT2 >= min( 1, NTYPES ).
*
*  LDT3    (input) INTEGER
*          The third dimension of TIMES.  LDT3 >= min( 1, NSIZES ).
*
*  OPCNTS  (output) REAL array,
*                   dimension (LDO1,LDO2,LDO3,NSUBS)
*          OPCNTS(i,j,k,l) will be set to the number of floating-point
*          operations executed by subroutine/path l, with N=NN(k),
*          M=MM(k), matrix type j, LDA=LDAS(i), and NBLOCK=NNB(i).
*
*  LDO1    (input) INTEGER
*          The first dimension of OPCNTS.  LDO1 >= min( 1, NPARMS ).
*
*  LDO2    (input) INTEGER
*          The second dimension of OPCNTS.  LDO2 >= min( 1, NTYPES ).
*
*  LDO3    (input) INTEGER
*          The third dimension of OPCNTS.  LDO3 >= min( 1, NSIZES ).
*
*  INFO    (output) INTEGER
*          Error flag.  It will be set to zero if no error occurred.
*
*  =====================================================================
*
*     .. Parameters ..
      INTEGER            MAXTYP, NSUBS
      PARAMETER          ( MAXTYP = 5, NSUBS = 18 )
      REAL               ZERO, ONE, TWO
      PARAMETER          ( ZERO = 0.0E0, ONE = 1.0E0, TWO = 2.0E0 )
*     ..
*     .. Local Scalars ..
      LOGICAL            RUNBRD, TRNBRD
      CHARACTER          UPLO
      INTEGER            IC, IINFO, IMODE, IN, IPAR, ISUB, ITYPE,
     $                   J, J1, J2, J3, J4, KU, KVT, LASTNL, LDA,
     $                   LDH, M, MINMN, MTYPES, N, NB
      REAL               CONDS, ESUM, S1, S2, TIME, ULP, ULPINV, UNTIME
*     ..
*     .. Local Arrays ..
      LOGICAL            TIMSUB( NSUBS )
      CHARACTER*4        PNAMES( 2 )
      CHARACTER*9        SUBNAM( NSUBS )
      INTEGER            INPARM( NSUBS ), IOLDSD( 4 ), JDUM( 1 ),
     $                   KMODE( 3 )
      REAL               DUM( 1 )
*     ..
*     .. External Functions ..
      REAL               SECOND, SASUM, SLAMCH, SLARND,
     $                   SOPLA, SOPLA2
      EXTERNAL           SECOND, SASUM, SLAMCH, SLARND,
     $                   SOPLA, SOPLA2
*     ..
*     .. External Subroutines ..
      EXTERNAL           SBDSDC, SBDSQR, SCOPY, SGEBRD,
     $                   SGESDD, SLACPY, SLASET, SLATMR,
     $                   SLATMS, SORGBR, SPRTBV, SSVDC, 
     $                   ATIMIN, XLAENV
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          REAL, ABS, EXP, LOG, MAX, MIN
*     ..
*     .. Common blocks ..
      COMMON             / LATIME / OPS, ITCNT
*     ..
*     .. Scalars in Common ..
      REAL               ITCNT, OPS
*     ..
*     .. Data statements ..
      DATA               SUBNAM / 'SGEBRD', 'SBDSQR', 'SBDSQR(L)',
     $                   'SBDSQR(R)', 'SBDSQR(B)', 'SBDSQR(V)',
     $                   'LAPSVD', 'LAPSVD(l)', 'LAPSVD(L)',
     $                   'LAPSVD(R)', 'LAPSVD(B)', 'SBDSDC(B)',
     $                   'SGESDD(B)', 'LINSVD', 'LINSVD(l)',
     $                   'LINSVD(L)', 'LINSVD(R)', 'LINSVD(B)' /
      DATA               INPARM / 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 2,
     $                   1, 1, 1, 1, 1 /
      DATA               PNAMES / 'LDA', 'NB' /
      DATA               KMODE / 4, 3, 1 /
*     ..
*     .. Executable Statements ..
*
*
*     Extract the timing request from the input line.
*
      CALL ATIMIN( 'SBD', LINE, NSUBS, SUBNAM, TIMSUB, NOUT, INFO )
      IF( INFO.NE.0 )
     $   RETURN
*
*     Check LWORK and
*     Check that N <= LDA and M <= LDA for the input values.
*
      DO 20 J2 = 1, NSIZES
         IF( LWORK.LT.MAX( 6*MIN( MM( J2 ), NN( J2 ) ), 3*MAX( MM( J2 ),
     $       NN( J2 ) ), NSIZES*NPARMS*NTYPES ) ) THEN
            INFO = -22
            WRITE( NOUT, FMT = 9999 )LINE( 1: 6 )
            RETURN
         END IF
         DO 10 J1 = 1, NPARMS
            IF( MAX( NN( J2 ), MM( J2 ) ).GT.LDAS( J1 ) ) THEN
               INFO = -9
               WRITE( NOUT, FMT = 9999 )LINE( 1: 6 )
 9999          FORMAT( 1X, A, ' timing run not attempted', / )
               RETURN
            END IF
   10    CONTINUE
   20 CONTINUE
*
*     Check to see whether SGEBRD must be run.
*
*     RUNBRD -- if SGEBRD must be run without timing.
*     TRNBRD -- if SGEBRD must be run with timing.
*
      RUNBRD = .FALSE.
      TRNBRD = .FALSE.
      IF( TIMSUB( 2 ) .OR. TIMSUB( 3 ) .OR. TIMSUB( 4 ) .OR.
     $    TIMSUB( 5 ) .OR. TIMSUB( 6 ) )RUNBRD = .TRUE.
      IF( TIMSUB( 1 ) )
     $   RUNBRD = .FALSE.
      IF( TIMSUB( 7 ) .OR. TIMSUB( 8 ) .OR. TIMSUB( 9 ) .OR.
     $    TIMSUB( 10 ) .OR. TIMSUB( 11 ) )TRNBRD = .TRUE.
*
*     Various Constants
*
      ULP = SLAMCH( 'Epsilon' )*SLAMCH( 'Base' )
      ULPINV = ONE / ULP
      CALL XLAENV( 9, 25 )
*
*     Zero out OPCNTS, TIMES
*
      DO 60 J4 = 1, NSUBS
         DO 50 J3 = 1, NSIZES
            DO 40 J2 = 1, NTYPES
               DO 30 J1 = 1, NPARMS
                  OPCNTS( J1, J2, J3, J4 ) = ZERO
                  TIMES( J1, J2, J3, J4 ) = ZERO
   30          CONTINUE
   40       CONTINUE
   50    CONTINUE
   60 CONTINUE
*
*     Do for each value of N:
*
      DO 750 IN = 1, NSIZES
*
         N = NN( IN )
         M = MM( IN )
         MINMN = MIN( M, N )
         IF( M.GE.N ) THEN
            UPLO = 'U'
            KU = MINMN
            KVT = MAX( MINMN-1, 0 )
         ELSE
            UPLO = 'L'
            KU = MAX( MINMN-1, 0 )
            KVT = MINMN
         END IF
*
*        Do for each .TRUE. value in DOTYPE:
*
         MTYPES = MIN( MAXTYP, NTYPES )
         IF( NTYPES.EQ.MAXTYP+1 .AND. NSIZES.EQ.1 )
     $      MTYPES = NTYPES
         DO 740 ITYPE = 1, MTYPES
            IF( .NOT.DOTYPE( ITYPE ) )
     $         GO TO 740
*
*           Save random number seed for error messages
*
            DO 70 J = 1, 4
               IOLDSD( J ) = ISEED( J )
   70       CONTINUE
*
*-----------------------------------------------------------------------
*
*           Time the LAPACK Routines
*
*           Generate A
*
            IF( ITYPE.LE.MAXTYP ) THEN
               IF( ITYPE.GE.1 .AND. ITYPE.LE.3 ) THEN
                  IMODE = KMODE( ITYPE )
                  CALL SLATMS( M, N, 'U', ISEED, 'N', D, IMODE, ULPINV,
     $                         ONE, M, N, 'N', A, M, WORK, INFO )
               ELSE IF( ITYPE.GE.4 .AND. ITYPE.LE.5 ) THEN
                  IF( ITYPE.EQ.4 )
     $               CONDS = -ONE
                  IF( ITYPE.EQ.5 )
     $               CONDS = ULP
                  CALL SLATMR( M, N, 'S', ISEED, 'N', D, 6, ZERO, ONE,
     $                         'T', 'N', D, 0, ONE, D, 0, ONE, 'N',
     $                         JDUM, M, N, ZERO, CONDS, 'N', A, M, JDUM,
     $                         INFO )
                  IF( ITYPE.EQ.5 ) THEN
                     CONDS = -TWO*LOG( ULP )
                     DO 80 J = 1, ( MINMN-1 )*M + MINMN, M + 1
                        A( J ) = EXP( CONDS*SLARND( 1, ISEED ) )
   80                CONTINUE
                     IF( M.GE.N ) THEN
                        DO 90 J = M + 1, ( MINMN-1 )*M + MINMN - 1,
     $                          M + 1
                           A( J ) = EXP( CONDS*SLARND( 1, ISEED ) )
   90                   CONTINUE
                     ELSE
                        DO 100 J = 2, ( MINMN-2 )*M + MINMN, M + 1
                           A( J ) = EXP( CONDS*SLARND( 1, ISEED ) )
  100                   CONTINUE
                     END IF
                  END IF
               END IF
            END IF
*
*           Time SGEBRD for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 1 ) .OR. TRNBRD ) THEN
               DO 130 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time SGEBRD
*
                  IC = 0
                  OPS = ZERO
                  S1 = SECOND( )
  110             CONTINUE
                  CALL SLACPY( 'Full', M, N, A, M, H, LDA )
                  CALL SGEBRD( M, N, H, LDA, D, E, TAUQ, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 1 ), IINFO, M, N,
     $                  ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
*
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 110
*
*                 Subtract the time used in SLACPY.
*
                  S1 = SECOND( )
                  DO 120 J = 1, IC
                     CALL SLACPY( 'Full', M, N, A, M, U, LDA )
  120             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 1 ) = MAX( TIME-UNTIME,
     $               ZERO ) / REAL( IC )
                  OPCNTS( IPAR, ITYPE, IN, 1 ) = SOPLA( 'SGEBRD', M, N,
     $               0, 0, NB )
  130          CONTINUE
               LDH = LDA
            ELSE
               IF( RUNBRD ) THEN
                  CALL SLACPY( 'Full', M, N, A, M, H, M )
                  CALL SGEBRD( M, N, H, M, D, E, TAUQ, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 1 ), IINFO, M, N,
     $                  ITYPE, 0, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
                  LDH = M
               END IF
            END IF
*
*           Time SBDSQR (singular values only) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 2 ) .OR. TIMSUB( 7 ) ) THEN
               DO 170 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 140 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  140             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SBDSQR (singular values only)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  150                CONTINUE
                     CALL SCOPY( MINMN, D, 1, WORK, 1 )
                     CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
                     CALL SBDSQR( UPLO, MINMN, 0, 0, 0, WORK,
     $                            WORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, WORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 2 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 150
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 160 J = 1, IC
                        CALL SCOPY( MINMN, D, 1, WORK, 1 )
                        CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
  160                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 2 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 2 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 2 ) = TIMES( LASTNL, ITYPE,
     $                  IN, 2 )
                     OPCNTS( IPAR, ITYPE, IN, 2 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 2 )
                  END IF
  170          CONTINUE
            END IF
*
*           Time SBDSQR (singular values and left singular vectors,
*           assume original matrix square) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 3 ) .OR. TIMSUB( 8 ) .OR. TIMSUB( 9 ) ) THEN
               DO 210 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 180 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  180             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SBDSQR (singular values and left singular
*                    vectors, assume original matrix square)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  190                CONTINUE
                     CALL SLASET( 'Full', M, MINMN, ONE, TWO, U, LDA )
                     CALL SCOPY( MINMN, D, 1, WORK, 1 )
                     CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
                     CALL SBDSQR( UPLO, MINMN, 0, M, 0, WORK,
     $                            WORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, WORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 3 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 190
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 200 J = 1, IC
                        CALL SLASET( 'Full', M, MINMN, ONE, TWO, U,
     $                               LDA )
                        CALL SCOPY( MINMN, D, 1, WORK, 1 )
                        CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
  200                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 3 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 3 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 3 ) = TIMES( LASTNL, ITYPE,
     $                  IN, 3 )
                     OPCNTS( IPAR, ITYPE, IN, 3 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 3 )
                  END IF
  210          CONTINUE
            END IF
*
*           Time SBDSQR (singular values and right singular vectors,
*           assume original matrix square) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 4 ) .OR. TIMSUB( 10 ) ) THEN
               DO 250 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 220 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  220             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SBDSQR (singular values and right singular
*                    vectors, assume original matrix square)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  230                CONTINUE
                     CALL SLASET( 'Full', MINMN, N, ONE, TWO, VT, LDA )
                     CALL SCOPY( MINMN, D, 1, WORK, 1 )
                     CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
                     CALL SBDSQR( UPLO, MINMN, N, 0, 0, WORK,
     $                            WORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, WORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 4 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 230
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 240 J = 1, IC
                        CALL SLASET( 'Full', MINMN, N, ONE, TWO, VT,
     $                               LDA )
                        CALL SCOPY( MINMN, D, 1, WORK, 1 )
                        CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
  240                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 4 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 4 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 4 ) = TIMES( LASTNL, ITYPE,
     $                  IN, 4 )
                     OPCNTS( IPAR, ITYPE, IN, 4 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 4 )
                  END IF
  250          CONTINUE
            END IF
*
*           Time SBDSQR (singular values and left and right singular
*           vectors,assume original matrix square) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 5 ) .OR. TIMSUB( 11 ) ) THEN
               DO 290 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 260 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  260             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SBDSQR (singular values and left and right
*                    singular vectors, assume original matrix square)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  270                CONTINUE
                     CALL SLASET( 'Full', MINMN, N, ONE, TWO, VT, LDA )
                     CALL SLASET( 'Full', M, MINMN, ONE, TWO, U, LDA )
                     CALL SCOPY( MINMN, D, 1, WORK, 1 )
                     CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
                     CALL SBDSQR( UPLO, MINMN, N, M, 0, WORK,
     $                            WORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, WORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 5 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 270
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 280 J = 1, IC
                        CALL SLASET( 'Full', MINMN, N, ONE, TWO, VT,
     $                               LDA )
                        CALL SLASET( 'Full', M, MINMN, ONE, TWO, U,
     $                               LDA )
                        CALL SCOPY( MINMN, D, 1, WORK, 1 )
                        CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
  280                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 5 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 5 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 5 ) = TIMES( LASTNL, ITYPE,
     $                  IN, 5 )
                     OPCNTS( IPAR, ITYPE, IN, 5 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 5 )
                  END IF
  290          CONTINUE
            END IF
*
*           Time SBDSQR (singular values and multiply square matrix
*           by transpose of left singular vectors) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 6 ) ) THEN
               DO 330 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 300 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  300             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SBDSQR (singular values and multiply square
*                    matrix by transpose of left singular vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  310                CONTINUE
                     CALL SLASET( 'Full', MINMN, MINMN, ONE, TWO, U,
     $                            LDA )
                     CALL SCOPY( MINMN, D, 1, WORK, 1 )
                     CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
                     CALL SBDSQR( UPLO, MINMN, 0, 0, MINMN, WORK,
     $                            WORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, WORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 6 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 310
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 320 J = 1, IC
                        CALL SLASET( 'Full', MINMN, MINMN, ONE, TWO, U,
     $                               LDA )
                        CALL SCOPY( MINMN, D, 1, WORK, 1 )
                        CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
  320                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 6 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 6 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 6 ) = TIMES( LASTNL, ITYPE,
     $                  IN, 6 )
                     OPCNTS( IPAR, ITYPE, IN, 6 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 6 )
                  END IF
  330          CONTINUE
            END IF
*
*           Time SGEBRD+SBDSQR (singular values only) for each pair
*           NNB(j), LDAS(j)
*           Use previously computed timings for SGEBRD & SBDSQR
*
            IF( TIMSUB( 7 ) ) THEN
               DO 340 IPAR = 1, NPARMS
                  TIMES( IPAR, ITYPE, IN, 7 ) = TIMES( IPAR, ITYPE, IN,
     $               1 ) + TIMES( IPAR, ITYPE, IN, 2 )
                  OPCNTS( IPAR, ITYPE, IN, 7 ) = OPCNTS( IPAR, ITYPE,
     $               IN, 1 ) + OPCNTS( IPAR, ITYPE, IN, 2 )
  340          CONTINUE
            END IF
*
*           Time SGEBRD+SORGBR+SBDSQR (singular values and min(M,N)
*           left singular vectors) for each pair NNB(j), LDAS(j)
*
*           Use previously computed timings for SGEBRD & SBDSQR
*
            IF( TIMSUB( 8 ) ) THEN
               DO 370 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time SGEBRD+SORGBR+SBDSQR (singular values and
*                 min(M,N) left singular vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = SECOND( )
  350             CONTINUE
                  CALL SLACPY( 'L', M, MINMN, H, LDH, U, LDA )
                  CALL SORGBR( 'Q', M, MINMN, KU, U, LDA, TAUQ, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 8 ), IINFO, M, N,
     $                  ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 350
*
*                 Subtract the time used in SLACPY.
*
                  S1 = SECOND( )
                  DO 360 J = 1, IC
                     CALL SLACPY( 'L', M, MINMN, H, LDH, U, LDA )
  360             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 8 ) = MAX( TIME-UNTIME,
     $               ZERO ) / REAL( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 3 )
                  OPCNTS( IPAR, ITYPE, IN, 8 ) = SOPLA2( 'SORGBR', 'Q',
     $               M, MINMN, KU, 0, NB ) + OPCNTS( IPAR, ITYPE, IN,
     $               1 ) + OPCNTS( IPAR, ITYPE, IN, 3 )
  370          CONTINUE
            END IF
*
*           Time SGEBRD+SORGBR+SBDSQR (singular values and M
*           left singular vectors) for each pair NNB(j), LDAS(j)
*
*           Use previously computed timings for SGEBRD & SBDSQR
*
            IF( TIMSUB( 9 ) ) THEN
               DO 400 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time SGEBRD+SORGBR+SBDSQR (singular values and
*                 M left singular vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = SECOND( )
  380             CONTINUE
                  CALL SLACPY( 'L', M, MINMN, H, LDH, U, LDA )
                  CALL SORGBR( 'Q', M, M, KU, U, LDA, TAUQ, WORK, LWORK,
     $                         IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 9 ), IINFO, M, N,
     $                  ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 380
*
*                 Subtract the time used in SLACPY.
*
                  S1 = SECOND( )
                  DO 390 J = 1, IC
                     CALL SLACPY( 'L', M, MINMN, H, LDH, U, LDA )
  390             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 9 ) = MAX( TIME-UNTIME,
     $               ZERO ) / REAL( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 3 )
                  OPCNTS( IPAR, ITYPE, IN, 9 ) = SOPLA2( 'SORGBR', 'Q',
     $               M, M, KU, 0, NB ) + OPCNTS( IPAR, ITYPE, IN, 1 ) +
     $               OPCNTS( IPAR, ITYPE, IN, 3 )
  400          CONTINUE
            END IF
*
*           Time SGEBRD+SORGBR+SBDSQR (singular values and N
*           right singular vectors) for each pair NNB(j), LDAS(j)
*
*           Use previously computed timings for SGEBRD & SBDSQR
*
            IF( TIMSUB( 10 ) ) THEN
               DO 430 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time SGEBRD+SORGBR+SBDSQR (singular values and
*                 N right singular vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = SECOND( )
  410             CONTINUE
                  CALL SLACPY( 'U', MINMN, N, H, LDH, VT, LDA )
                  CALL SORGBR( 'P', N, N, KVT, VT, LDA, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 10 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 410
*
*                 Subtract the time used in SLACPY.
*
                  S1 = SECOND( )
                  DO 420 J = 1, IC
                     CALL SLACPY( 'U', MINMN, N, H, LDH, VT, LDA )
  420             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 10 ) = MAX( TIME-UNTIME,
     $               ZERO ) / REAL( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 4 )
                  OPCNTS( IPAR, ITYPE, IN, 10 ) = SOPLA2( 'SORGBR', 'P',
     $               N, N, KVT, 0, NB ) + OPCNTS( IPAR, ITYPE, IN, 1 ) +
     $               OPCNTS( IPAR, ITYPE, IN, 4 )
  430          CONTINUE
            END IF
*
*           Time SGEBRD+SORGBR+SBDSQR (singular values and min(M,N) left
*           singular vectors and N right singular vectors) for each pair
*           NNB(j), LDAS(j)
*
*           Use previously computed timings for SGEBRD & SBDSQR
*
            IF( TIMSUB( 11 ) ) THEN
               DO 460 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time SGEBRD+SORGBR+SBDSQR (singular values and
*                 min(M,N) left singular vectors and N right singular
*                 vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = SECOND( )
  440             CONTINUE
                  CALL SLACPY( 'L', M, MINMN, H, LDH, U, LDA )
                  CALL SORGBR( 'Q', M, MINMN, KU, U, LDA, TAUQ, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 11 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
                  CALL SLACPY( 'U', MINMN, N, H, LDH, VT, LDA )
                  CALL SORGBR( 'P', N, N, KVT, VT, LDA, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 11 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
                  S2 = SECOND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 440
*
*                 Subtract the time used in SLACPY.
*
                  S1 = SECOND( )
                  DO 450 J = 1, IC
                     CALL SLACPY( 'L', MINMN, MINMN, H, LDH, VT, LDA )
  450             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 11 ) = MAX( TIME-UNTIME,
     $               ZERO ) / REAL( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 5 )
                  OPCNTS( IPAR, ITYPE, IN, 11 ) = SOPLA2( 'SORGBR', 'Q',
     $               M, MINMN, KU, 0, NB ) + SOPLA2( 'SORGBR', 'P', N,
     $               N, KVT, 0, NB ) + OPCNTS( IPAR, ITYPE, IN, 1 ) +
     $               OPCNTS( IPAR, ITYPE, IN, 5 )
  460          CONTINUE
            END IF
*
*           Time SBDSDC (singular values and left and right singular
*           vectors,assume original matrix square) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 12 ) ) THEN
               ESUM = SASUM( MINMN-1, E, 1 )
               IF( ESUM.EQ.ZERO ) THEN
                  CALL SLACPY( 'Full', M, N, A, M, H, M )
                  CALL SGEBRD( M, N, H, M, D, E, TAUQ, TAUP, WORK,
     $                         LWORK, IINFO )
               END IF
               DO 500 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 470 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  470             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SBDSDC (singular values and left and right
*                    singular vectors, assume original matrix square).
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  480                CONTINUE
                     CALL SCOPY( MINMN, D, 1, WORK, 1 )
                     CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
                     CALL SBDSDC( UPLO, 'I', MINMN, WORK,
     $                            WORK( MINMN+1 ), U, LDA, VT, LDA, DUM,
     $                            JDUM, WORK( 2*MINMN+1 ), IWORK,
     $                            IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 12 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 480
*
*                    Subtract the time used in SCOPY.
*
                     S1 = SECOND( )
                     DO 490 J = 1, IC
                        CALL SCOPY( MINMN, D, 1, WORK, 1 )
                        CALL SCOPY( MINMN-1, E, 1, WORK( MINMN+1 ), 1 )
  490                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 12 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 12 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 12 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 12 )
                     OPCNTS( IPAR, ITYPE, IN, 12 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 12 )
                  END IF
  500          CONTINUE
            END IF
*
*           Time SGESDD( singular values and min(M,N) left singular
*           vectors and N right singular vectors when M>=N,
*           singular values and M left singular vectors and min(M,N)
*           right singular vectors otherwise) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 13 ) ) THEN
               DO 530 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time SGESDD(singular values and min(M,N) left singular
*                 vectors and N right singular vectors when M>=N;
*                 singular values and M left singular vectors and
*                 min(M,N) right singular vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = SECOND( )
  510             CONTINUE
                  CALL SLACPY( 'Full', M, N, A, M, H, LDA )
                  CALL SGESDD( 'S', M, N, H, LDA, WORK, U, LDA, VT, LDA,
     $                         WORK( MINMN+1 ), LWORK-MINMN, IWORK,
     $                         IINFO )
                  S2 = SECOND( )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 13 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 740
                  END IF
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 510
*
*                 Subtract the time used in SLACPY.
*
                  S1 = SECOND( )
                  DO 520 J = 1, IC
                     CALL SLACPY( 'Full', M, N, A, M, H, LDA )
  520             CONTINUE
                  S2 = SECOND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 13 ) = MAX( TIME-UNTIME,
     $               ZERO ) / REAL( IC )
                  OPCNTS( IPAR, ITYPE, IN, 13 ) = OPS / REAL( IC )
  530          CONTINUE
            END IF
*
*           Time SSVDC (singular values only) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 14 ) ) THEN
               DO 570 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 540 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  540             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SSVDC (singular values only)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  550                CONTINUE
                     CALL SLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL SSVDC( H, LDA, M, N, D, E, U, LDA, VT, LDA,
     $                           WORK, 0, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 14 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 550
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 560 J = 1, IC
                        CALL SLACPY( 'Full', M, N, A, M, H, LDA )
  560                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 14 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 14 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 14 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 14 )
                     OPCNTS( IPAR, ITYPE, IN, 14 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 14 )
                  END IF
  570          CONTINUE
            END IF
*
*           Time SSVDC (singular values and min(M,N) left singular
*           vectors) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 15 ) ) THEN
               DO 610 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 580 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  580             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SSVDC (singular values and min(M,N) left
*                    singular vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  590                CONTINUE
                     CALL SLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL SSVDC( H, LDA, M, N, D, E, U, LDA, VT, LDA,
     $                           WORK, 20, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 15 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 590
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 600 J = 1, IC
                        CALL SLACPY( 'Full', M, N, A, M, H, LDA )
  600                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 15 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 15 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 15 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 15 )
                     OPCNTS( IPAR, ITYPE, IN, 15 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 15 )
                  END IF
  610          CONTINUE
            END IF
*
*           Time SSVDC (singular values and M left singular
*           vectors) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 16 ) ) THEN
               DO 650 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 620 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  620             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SSVDC (singular values and M left singular
*                    vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  630                CONTINUE
                     CALL SLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL SSVDC( H, LDA, M, N, D, E, U, LDA, VT, LDA,
     $                           WORK, 10, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 16 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 630
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 640 J = 1, IC
                        CALL SLACPY( 'Full', M, N, A, M, H, LDA )
  640                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 16 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 16 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 16 )
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 16 )
                  END IF
  650          CONTINUE
            END IF
*
*           Time SSVDC (singular values and N right singular
*           vectors) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 17 ) ) THEN
               DO 690 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 660 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  660             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SSVDC (singular values and N right singular
*                    vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  670                CONTINUE
                     CALL SLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL SSVDC( H, LDA, M, N, D, E, U, LDA, VT, LDA,
     $                           WORK, 1, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 17 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 670
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 680 J = 1, IC
                        CALL SLACPY( 'Full', M, N, A, M, H, LDA )
  680                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 17 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 17 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 17 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 17 )
                     OPCNTS( IPAR, ITYPE, IN, 17 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 17 )
                  END IF
  690          CONTINUE
            END IF
*
*           Time SSVDC (singular values and min(M,N) left singular
*           vectors and N right singular vectors) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 18 ) ) THEN
               DO 730 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 700 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  700             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time SSVDC (singular values and min(M,N) left
*                    singular vectors and N right singular vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = SECOND( )
  710                CONTINUE
                     CALL SLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL SSVDC( H, LDA, M, N, D, E, U, LDA, VT, LDA,
     $                           WORK, 21, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 18 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 740
                     END IF
                     S2 = SECOND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 710
*
*                    Subtract the time used in SLACPY.
*
                     S1 = SECOND( )
                     DO 720 J = 1, IC
                        CALL SLACPY( 'Full', M, N, A, M, H, LDA )
  720                CONTINUE
                     S2 = SECOND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 18 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / REAL( IC )
                     OPCNTS( IPAR, ITYPE, IN, 18 ) = OPS / REAL( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 18 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 18 )
                     OPCNTS( IPAR, ITYPE, IN, 18 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 18 )
                  END IF
  730          CONTINUE
            END IF
*
  740    CONTINUE
  750 CONTINUE
*
*-----------------------------------------------------------------------
*
*     Print a table of results for each timed routine.
*
      DO 760 ISUB = 1, NSUBS
         IF( TIMSUB( ISUB ) ) THEN
            CALL SPRTBV( SUBNAM( ISUB ), NTYPES, DOTYPE, NSIZES, MM, NN,
     $                   INPARM( ISUB ), PNAMES, NPARMS, LDAS, NNB,
     $                   OPCNTS( 1, 1, 1, ISUB ), LDO1, LDO2,
     $                   TIMES( 1, 1, 1, ISUB ), LDT1, LDT2, WORK,
     $                   LLWORK, NOUT )
         END IF
  760 CONTINUE
*
      RETURN
*
*     End of STIM26
*
 9998 FORMAT( ' STIM26: ', A, ' returned INFO=', I6, '.', / 9X, 'M=',
     $      I6, ', N=', I6, ', ITYPE=', I6, ', IPAR=', I6, ',         ',
     $      '        ISEED=(', 4( I5, ',' ), I5, ')' )
*
      END
