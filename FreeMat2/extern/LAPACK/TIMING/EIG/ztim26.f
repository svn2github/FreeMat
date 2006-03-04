      SUBROUTINE ZTIM26( LINE, NSIZES, NN, MM, NTYPES, DOTYPE, NPARMS,
     $                   NNB, LDAS, TIMMIN, NOUT, ISEED, A, H, U, VT, D,
     $                   DC, E, EC, TAUP, TAUQ, WORK, LWORK, RWORK,
     $                   IWORK, LLWORK, TIMES, LDT1, LDT2, LDT3, OPCNTS,
     $                   LDO1, LDO2, LDO3, INFO )
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
      DOUBLE PRECISION   TIMMIN
*     ..
*     .. Array Arguments ..
      LOGICAL            DOTYPE( * ), LLWORK( * )
      INTEGER            ISEED( * ), IWORK( * ), LDAS( * ), MM( * ),
     $                   NN( * ), NNB( * )
      DOUBLE PRECISION   D( * ), E( * ), OPCNTS( LDO1, LDO2, LDO3, * ),
     $                   RWORK( * ), TIMES( LDT1, LDT2, LDT3, * )
      COMPLEX*16         A( * ), DC( * ), EC( * ), H( * ), TAUP( * ),
     $                   TAUQ( * ), U( * ), VT( * ), WORK( * )
*     ..
*
*  Purpose
*  =======
*
*     ZTIM26 times the LAPACK routines for the COMPLEX*16 singular value
*     decomposition.
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
*          such as ZGEBRD, indicating that only routine CGEBRD will
*          be timed, or it may contain a generic name, such as ZBD.
*          In this case, the rest of the line is scanned for the
*          first 11 non-blank characters, corresponding to the eleven
*          combinations of subroutine and options:
*          LAPACK:
*           1: ZGEBRD
*              (labeled ZGEBRD in the output)
*           2: ZBDSQR (singular values only)
*              (labeled ZBDSQR in the output)
*           3: ZBDSQR (singular values and left singular vectors;
*                      assume original matrix M by N)
*              (labeled ZBDSQR(L) in the output)
*           4: ZBDSQR (singular values and right singular vectors;
*                      assume original matrix M by N)
*              (labeled ZBDSQR(R) in the output)
*           5: ZBDSQR (singular values and left and right singular
*                      vectors; assume original matrix M by N)
*              (labeled ZBDSQR(B) in the output)
*           6: ZBDSQR (singular value and multiply square MIN(M,N)
*                      matrix by transpose of left singular vectors)
*              (labeled ZBDSQR(V) in the output)
*           7: ZGEBRD+ZBDSQR (singular values only)
*              (labeled LAPSVD in the output)
*           8: ZGEBRD+ZUNGBR+ZBDSQR(L) (singular values and min(M,N)
*                                       left singular vectors)
*              (labeled LAPSVD(l) in the output)
*           9: ZGEBRD+ZUNGBR+ZBDSQR(L) (singular values and M left
*                                       singular vectors)
*              (labeled LAPSVD(L) in the output)
*          10: ZGEBRD+ZUNGBR+ZBDSQR(R) (singular values and N right
*                                       singular vectors)
*              (labeled LAPSVD(R) in the output)
*          11: ZGEBRD+ZUNGBR+ZBDSQR(B) (singular values and min(M,N)
*                                       left singular vectors and N
*                                       right singular vectors)
*              (labeled LAPSVD(B) in the output)
*          12: ZGESDD (singular values and min(M,N) left singular
*                      vectors and N right singular vectors if M>=N,
*                      singular values and M left singular vectors
*                      and min(M,N) right singular vectors otherwise.)
*              (labeled ZGESDD(B) in the output)
*          LINPACK:
*          13: ZSVDC (singular values only) (comparable to 7 above)
*              (labeled LINSVD in the output)
*          14: ZSVDC (singular values and min(M,N) left singular
*                     vectors) (comparable to 8 above)
*              (labeled LINSVD(l) in the output)
*          15: ZSVDC (singular values and M left singular vectors)
*                     (comparable to 9 above)
*              (labeled LINSVD(L) in the output)
*          16: ZSVDC (singular values and N right singular vectors)
*                     (comparable to 10 above)
*              (labeled LINSVD(R) in the output)
*          17: ZSVDC (singular values and min(M,N) left singular
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
*           j=1: A = U*D*V where U and V are random unitary
*                matrices and D has evenly spaced entries 1,...,ULP
*                with random signs on the diagonal
*           j=2: A = U*D*V where U and V are random unitary
*                matrices and D has geometrically spaced entries
*                1,...,ULP with random signs on the diagonal
*           j=3: A = U*D*V where U and V are random unitary
*                matrices and D has "clustered" entries
*                1,ULP,...,ULP with random signs on the diagonal
*           j=4: A contains uniform complex random numbers with
*                components from [-1,1]
*           j=5: A is a special nearly bidiagonal matrix, where the
*                upper bidiagonal entries are exp(-2*r*log(ULP))
*                where r is a uniform random number from [0,1],
*                and the nonbidiagonal entries are r*ULP, where
*                r is a uniform complex random number with components
*                from [0,1]
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
*  TIMMIN  (input) DOUBLE PRECISION
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
*          each call to ZTIM26.
*
*  A       (workspace) COMPLEX*16 array,
*                      dimension( max(NN)*max(LDAS) )
*          During the testing of ZGEBRD, the original dense matrix.
*
*  H       (workspace) COMPLEX*16 array,
*                      dimension( max(NN)*max(LDAS) )
*          The packed unitary matrices reducing A to bidiagonal
*          form.
*
*  U       (workspace) COMPLEX*16 array,
*                      dimension( max(NN,MM)*max(LDAS) )
*          The left singular vectors of the original matrix.
*
*  VT      (workspace) COMPLEX*16 array,
*                      dimension( max(NN,MM)*max(LDAS) )
*          The right singular vectors of the original matrix.
*
*  D       (workspace) DOUBLE PRECISION array, dimension( max(NN,MM) )
*          Diagonal entries of bidiagonal matrix to which A
*          is reduced.
*
*  DC      (workspace) COMPLEX*16 array, dimension( max(NN,MM) )
*          Diagonal entries of bidiagonal matrix to which A
*          is reduced. May be equivalence with D in calling routine.
*
*  E       (workspace) DOUBLE PRECISION array, dimension( max(NN,MM) )
*          Offdiagonal entries of bidiagonal matrix to which A
*          is reduced.
*
*  EC      (workspace) COMPLEX*16 array, dimension( max(NN,MM) )
*          Offdiagonal entries of bidiagonal matrix to which A
*          is reduced. May be equivalence with E in calling routine.
*
*  TAUP    (workspace) COMPLEX*16 array, dimension( max(NN,MM) )
*          More information used with H.
*
*  TAUQ    (workspace) COMPLEX*16 array, dimension( max(NN,MM) )
*          More information used with H.
*
*  WORK    (workspace) COMPLEX*16 array, dimension( LWORK )
*
*  LWORK   (input) INTEGER
*          Number of elements in WORK and RWORK. Must be at least
*          MAX(6*MIN(M,N),3*MAX(M,N),NSIZES*NPARMS*NTYPES)
*
*  RWORK   (workspace) DOUBLE PRECISION array, dimension( LWORK )
*          May be equivalenced to WORK in calling routine.
*
*  IWORK   (workspace) INTEGER array, dimension at least 8*min(M,N).
*
*  LLWORK  (workspace) LOGICAL array, dimension( NPARMS ),
*
*  TIMES   (output) DOUBLE PRECISION array,
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
*  OPCNTS  (output) DOUBLE PRECISION array,
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
*          The third dimension of OPCNTS.   LDO3 >= min( 1, NSIZES ).
*
*  INFO    (output) INTEGER
*          Error flag.  It will be set to zero if no error occurred.
*
*  =====================================================================
*
*     .. Parameters ..
      INTEGER            MAXTYP, NSUBS
      PARAMETER          ( MAXTYP = 5, NSUBS = 17 )
      DOUBLE PRECISION   ZERO, ONE, TWO
      PARAMETER          ( ZERO = 0.0D0, ONE = 1.0D0, TWO = 2.0D0 )
      COMPLEX*16         CONE, CTWO
      PARAMETER          ( CONE = ( 1.0D0, 0.0D0 ),
     $                   CTWO = ( 2.0D0, 0.0D0 ) )
*     ..
*     .. Local Scalars ..
      LOGICAL            RUNBRD, TRNBRD
      CHARACTER          UPLO
      INTEGER            IC, IINFO, IMODE, IN, IPAR, ISUB, ITYPE, J, J1,
     $                   J2, J3, J4, KU, KVT, LASTNL, LDA, M, MINMN,
     $                   MTYPES, N, NB
      DOUBLE PRECISION   CONDS, S1, S2, TIME, ULP, ULPINV, UNTIME
*     ..
*     .. Local Arrays ..
      LOGICAL            TIMSUB( NSUBS )
      CHARACTER*4        PNAMES( 2 )
      CHARACTER*9        SUBNAM( NSUBS )
      INTEGER            INPARM( NSUBS ), IOLDSD( 4 ), JDUM( 1 ),
     $                   KMODE( 3 )
*     ..
*     .. External Functions ..
      DOUBLE PRECISION   DLAMCH, DLARND, DOPLA, DSECND, DOPLA2
      COMPLEX*16         ZLARND
      EXTERNAL           DLAMCH, DLARND, DOPLA, DSECND, DOPLA2, ZLARND
*     ..
*     .. External Subroutines ..
      EXTERNAL           ATIMIN, DCOPY, DPRTBV, XLAENV, ZBDSQR, ZGEBRD,
     $                   ZGESDD, ZLACPY, ZLASET, ZLATMR, ZLATMS, ZSVDC,
     $                   ZUNGBR
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          ABS, DBLE, EXP, LOG, MAX, MIN
*     ..
*     .. Common blocks ..
      COMMON             / LATIME / OPS, ITCNT
*     ..
*     .. Scalars in Common ..
      DOUBLE PRECISION   ITCNT, OPS
*     ..
*     .. Data statements ..
      DATA               SUBNAM / 'ZGEBRD', 'ZBDSQR', 'ZBDSQR(L)',
     $                   'ZBDSQR(R)', 'ZBDSQR(B)', 'ZBDSQR(V)',
     $                   'LAPSVD', 'LAPSVD(l)', 'LAPSVD(L)',
     $                   'LAPSVD(R)', 'LAPSVD(B)', 'ZGESDD(B)',
     $                   'LINSVD', 'LINSVD(l)', 'LINSVD(L)',
     $                   'LINSVD(R)', 'LINSVD(B)' /
      DATA               INPARM / 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1,
     $                   1, 1, 1, 1 /
      DATA               PNAMES / 'LDA', 'NB' /
      DATA               KMODE / 4, 3, 1 /
*     ..
*     .. Executable Statements ..
*
*
*     Extract the timing request from the input line.
*
      CALL ATIMIN( 'ZBD', LINE, NSUBS, SUBNAM, TIMSUB, NOUT, INFO )
      IF( INFO.NE.0 )
     $   RETURN
*
*     Check LWORK and
*     Check that N <= LDA and M <= LDA for the input values.
*
      DO 20 J2 = 1, NSIZES
         IF( LWORK.LT.MAX( 6*MIN( MM( J2 ), NN( J2 ) ), 3*MAX( MM( J2 ),
     $       NN( J2 ) ), NSIZES*NPARMS*NTYPES ) ) THEN
            INFO = -24
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
*     Check to see whether ZGEBRD must be run.
*
*     RUNBRD -- if ZGEBRD must be run without timing.
*     TRNBRD -- if ZGEBRD must be run with timing.
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
      ULP = DLAMCH( 'Epsilon' )*DLAMCH( 'Base' )
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
      DO 710 IN = 1, NSIZES
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
         DO 700 ITYPE = 1, MTYPES
            IF( .NOT.DOTYPE( ITYPE ) )
     $         GO TO 700
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
                  CALL ZLATMS( M, N, 'U', ISEED, 'N', D, IMODE, ULPINV,
     $                         ONE, M, N, 'N', A, M, WORK, INFO )
               ELSE IF( ITYPE.GE.4 .AND. ITYPE.LE.5 ) THEN
                  IF( ITYPE.EQ.4 )
     $               CONDS = -ONE
                  IF( ITYPE.EQ.5 )
     $               CONDS = ULP
                  CALL ZLATMR( M, N, 'S', ISEED, 'N', DC, 6, ZERO, CONE,
     $                         'T', 'N', DC, 0, ONE, DC, 0, ONE, 'N',
     $                         JDUM, M, N, ZERO, CONDS, 'N', A, M, JDUM,
     $                         INFO )
                  IF( ITYPE.EQ.5 ) THEN
                     CONDS = -TWO*LOG( ULP )
                     DO 80 J = 1, ( MINMN-1 )*M + MINMN, M + 1
                        A( J ) = ZLARND( 5, ISEED )*
     $                           EXP( CONDS*DLARND( 1, ISEED ) )
   80                CONTINUE
                     IF( M.GE.N ) THEN
                        DO 90 J = M + 1, ( MINMN-1 )*M + MINMN - 1,
     $                          M + 1
                           A( J ) = ZLARND( 5, ISEED )*
     $                              EXP( CONDS*DLARND( 1, ISEED ) )
   90                   CONTINUE
                     ELSE
                        DO 100 J = 2, ( MINMN-2 )*M + MINMN, M + 1
                           A( J ) = ZLARND( 5, ISEED )*
     $                              EXP( CONDS*DLARND( 1, ISEED ) )
  100                   CONTINUE
                     END IF
                  END IF
               END IF
            END IF
*
*           Time ZGEBRD for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 1 ) .OR. TRNBRD ) THEN
               DO 130 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time ZGEBRD
*
                  IC = 0
                  OPS = ZERO
                  S1 = DSECND( )
  110             CONTINUE
                  CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
                  CALL ZGEBRD( M, N, H, LDA, D, E, TAUQ, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 1 ), IINFO, M, N,
     $                  ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
*
                  S2 = DSECND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 110
*
*                 Subtract the time used in ZLACPY.
*
                  S1 = DSECND( )
                  DO 120 J = 1, IC
                     CALL ZLACPY( 'Full', M, N, A, M, U, LDA )
  120             CONTINUE
                  S2 = DSECND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 1 ) = MAX( TIME-UNTIME,
     $               ZERO ) / DBLE( IC )
                  OPCNTS( IPAR, ITYPE, IN, 1 ) = DOPLA( 'ZGEBRD', M, N,
     $               0, 0, NB )
  130          CONTINUE
            ELSE
               IF( RUNBRD ) THEN
                  CALL ZLACPY( 'Full', M, N, A, M, H, M )
                  CALL ZGEBRD( M, N, H, M, D, E, TAUQ, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 1 ), IINFO, M, N,
     $                  ITYPE, 0, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
               END IF
            END IF
*
*           Time ZBDSQR (singular values only) for each pair
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
*                    Time ZBDSQR (singular values only)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  150                CONTINUE
                     CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                     CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
                     CALL ZBDSQR( UPLO, MINMN, 0, 0, 0, RWORK,
     $                            RWORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, RWORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 2 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 150
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 160 J = 1, IC
                        CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                        CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
  160                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 2 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 2 ) = OPS / DBLE( IC )
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
*           Time ZBDSQR (singular values and left singular vectors,
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
*                    Time ZBDSQR (singular values and left singular
*                    vectors, assume original matrix square)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  190                CONTINUE
                     CALL ZLASET( 'Full', M, MINMN, CONE, CTWO, U, LDA )
                     CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                     CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
                     CALL ZBDSQR( UPLO, MINMN, 0, M, 0, RWORK,
     $                            RWORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, RWORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 3 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 190
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 200 J = 1, IC
                        CALL ZLASET( 'Full', M, MINMN, CONE, CTWO, U,
     $                               LDA )
                        CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                        CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
  200                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 3 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 3 ) = OPS / DBLE( IC )
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
*           Time ZBDSQR (singular values and right singular vectors,
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
*                    Time ZBDSQR (singular values and right singular
*                    vectors, assume original matrix square)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  230                CONTINUE
                     CALL ZLASET( 'Full', MINMN, N, CONE, CTWO, VT,
     $                            LDA )
                     CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                     CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
                     CALL ZBDSQR( UPLO, MINMN, N, 0, 0, RWORK,
     $                            RWORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, RWORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 4 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 230
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 240 J = 1, IC
                        CALL ZLASET( 'Full', MINMN, N, CONE, CTWO, VT,
     $                               LDA )
                        CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                        CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
  240                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 4 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 4 ) = OPS / DBLE( IC )
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
*           Time ZBDSQR (singular values and left and right singular
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
*                    Time ZBDSQR (singular values and left and right
*                    singular vectors, assume original matrix square)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  270                CONTINUE
                     CALL ZLASET( 'Full', MINMN, N, CONE, CTWO, VT,
     $                            LDA )
                     CALL ZLASET( 'Full', M, MINMN, CONE, CTWO, U, LDA )
                     CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                     CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
                     CALL ZBDSQR( UPLO, MINMN, N, M, 0, RWORK,
     $                            RWORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, RWORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 5 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 270
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 280 J = 1, IC
                        CALL ZLASET( 'Full', MINMN, N, CONE, CTWO, VT,
     $                               LDA )
                        CALL ZLASET( 'Full', M, MINMN, CONE, CTWO, U,
     $                               LDA )
                        CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                        CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
  280                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 5 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 5 ) = OPS / DBLE( IC )
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
*           Time ZBDSQR (singular values and multiply square matrix
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
*                    Time ZBDSQR (singular values and multiply square
*                    matrix by transpose of left singular vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  310                CONTINUE
                     CALL ZLASET( 'Full', MINMN, MINMN, CONE, CTWO, U,
     $                            LDA )
                     CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                     CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
                     CALL ZBDSQR( UPLO, MINMN, 0, 0, MINMN, RWORK,
     $                            RWORK( MINMN+1 ), VT, LDA, U, LDA, U,
     $                            LDA, RWORK( 2*MINMN+1 ), IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 6 ), IINFO, M,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 310
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 320 J = 1, IC
                        CALL ZLASET( 'Full', MINMN, MINMN, CONE, CTWO,
     $                               U, LDA )
                        CALL DCOPY( MINMN, D, 1, RWORK, 1 )
                        CALL DCOPY( MINMN-1, E, 1, RWORK( MINMN+1 ), 1 )
  320                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 6 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 6 ) = OPS / DBLE( IC )
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
*           Time ZGEBRD+ZBDSQR (singular values only) for each pair
*           NNB(j), LDAS(j)
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
*           Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and min(M,N)
*           left singular vectors) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 8 ) ) THEN
               DO 370 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and
*                 min(M,N) left singular vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = DSECND( )
  350             CONTINUE
                  CALL ZLACPY( 'L', M, MINMN, H, LDA, U, LDA )
                  CALL ZUNGBR( 'Q', M, MINMN, KU, U, LDA, TAUQ, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 8 ), IINFO, M, N,
     $                  ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
                  S2 = DSECND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 350
*
*                 Subtract the time used in ZLACPY.
*
                  S1 = DSECND( )
                  DO 360 J = 1, IC
                     CALL ZLACPY( 'L', M, MINMN, H, LDA, U, LDA )
  360             CONTINUE
                  S2 = DSECND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 8 ) = MAX( TIME-UNTIME,
     $               ZERO ) / DBLE( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 3 )
                  OPCNTS( IPAR, ITYPE, IN, 8 ) = DOPLA2( 'ZUNGBR', 'Q',
     $               M, MINMN, KU, 0, NB ) + OPCNTS( IPAR, ITYPE, IN,
     $               1 ) + OPCNTS( IPAR, ITYPE, IN, 3 )
  370          CONTINUE
            END IF
*
*           Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and M
*           left singular vectors) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 9 ) ) THEN
               DO 400 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and
*                 M left singular vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = DSECND( )
  380             CONTINUE
                  CALL ZLACPY( 'L', M, MINMN, H, LDA, U, LDA )
                  CALL ZUNGBR( 'Q', M, M, KU, U, LDA, TAUQ, WORK, LWORK,
     $                         IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 9 ), IINFO, M, N,
     $                  ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
                  S2 = DSECND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 380
*
*                 Subtract the time used in ZLACPY.
*
                  S1 = DSECND( )
                  DO 390 J = 1, IC
                     CALL ZLACPY( 'L', M, MINMN, H, LDA, U, LDA )
  390             CONTINUE
                  S2 = DSECND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 9 ) = MAX( TIME-UNTIME,
     $               ZERO ) / DBLE( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 3 )
                  OPCNTS( IPAR, ITYPE, IN, 9 ) = DOPLA2( 'ZUNGBR', 'Q',
     $               M, M, KU, 0, NB ) + OPCNTS( IPAR, ITYPE, IN, 1 ) +
     $               OPCNTS( IPAR, ITYPE, IN, 3 )
  400          CONTINUE
            END IF
*
*           Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and N
*           right singular vectors) for each pair NNB(j), LDAS(j)
*
            IF( TIMSUB( 10 ) ) THEN
               DO 430 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and
*                 N right singular vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = DSECND( )
  410             CONTINUE
                  CALL ZLACPY( 'U', MINMN, N, H, LDA, VT, LDA )
                  CALL ZUNGBR( 'P', N, N, KVT, VT, LDA, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 10 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
                  S2 = DSECND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 410
*
*                 Subtract the time used in ZLACPY.
*
                  S1 = DSECND( )
                  DO 420 J = 1, IC
                     CALL ZLACPY( 'U', MINMN, N, H, LDA, VT, LDA )
  420             CONTINUE
                  S2 = DSECND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 10 ) = MAX( TIME-UNTIME,
     $               ZERO ) / DBLE( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 4 )
                  OPCNTS( IPAR, ITYPE, IN, 10 ) = DOPLA2( 'ZUNGBR', 'P',
     $               N, N, KVT, 0, NB ) + OPCNTS( IPAR, ITYPE, IN, 1 ) +
     $               OPCNTS( IPAR, ITYPE, IN, 4 )
  430          CONTINUE
            END IF
*
*           Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and min(M,N) left
*           singular vectors and N right singular vectors) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 11 ) ) THEN
               DO 460 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time ZGEBRD+ZUNGBR+ZBDSQR (singular values and
*                 min(M,N) left singular vectors and N right singular
*                 vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = DSECND( )
  440             CONTINUE
                  CALL ZLACPY( 'L', M, MINMN, H, LDA, U, LDA )
                  CALL ZUNGBR( 'Q', M, MINMN, KU, U, LDA, TAUQ, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 11 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
                  CALL ZLACPY( 'U', MINMN, N, H, LDA, VT, LDA )
                  CALL ZUNGBR( 'P', N, N, KVT, VT, LDA, TAUP, WORK,
     $                         LWORK, IINFO )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 11 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
                  S2 = DSECND( )
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 440
*
*                 Subtract the time used in ZLACPY.
*
                  S1 = DSECND( )
                  DO 450 J = 1, IC
                     CALL ZLACPY( 'L', MINMN, MINMN, H, LDA, VT, LDA )
  450             CONTINUE
                  S2 = DSECND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 11 ) = MAX( TIME-UNTIME,
     $               ZERO ) / DBLE( IC ) + TIMES( IPAR, ITYPE, IN, 1 ) +
     $               TIMES( IPAR, ITYPE, IN, 5 )
                  OPCNTS( IPAR, ITYPE, IN, 11 ) = DOPLA2( 'ZUNGBR', 'Q',
     $               M, MINMN, KU, 0, NB ) + DOPLA2( 'ZUNGBR', 'P', N,
     $               N, KVT, 0, NB ) + OPCNTS( IPAR, ITYPE, IN, 1 ) +
     $               OPCNTS( IPAR, ITYPE, IN, 5 )
  460          CONTINUE
            END IF
*
*           Time ZGESDD( singular values and min(M,N) left singular
*           vectors and N right singular vectors when M>=N,
*           singular values and M left singular vectors and min(M,N)
*           right singular vectors otherwise) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 12 ) ) THEN
               DO 490 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = MIN( N, NNB( IPAR ) )
                  CALL XLAENV( 1, NB )
                  CALL XLAENV( 2, 2 )
                  CALL XLAENV( 3, NB )
*
*                 Time ZGESDD (singular values and
*                 min(M,N) left singular vectors and N right singular
*                 vectors)
*
                  IC = 0
                  OPS = ZERO
                  S1 = DSECND( )
  470             CONTINUE
                  CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
                  CALL ZGESDD( 'S', M, N, H, LDA, RWORK, U, LDA, VT,
     $                         LDA, WORK, LWORK, RWORK( 2*MINMN+1 ),
     $                         IWORK, IINFO )
                  S2 = DSECND( )
                  IF( IINFO.NE.0 ) THEN
                     WRITE( NOUT, FMT = 9998 )SUBNAM( 12 ), IINFO, M,
     $                  N, ITYPE, IPAR, IOLDSD
                     INFO = ABS( IINFO )
                     GO TO 700
                  END IF
                  TIME = S2 - S1
                  IC = IC + 1
                  IF( TIME.LT.TIMMIN )
     $               GO TO 470
*
*                 Subtract the time used in ZLACPY.
*
                  S1 = DSECND( )
                  DO 480 J = 1, IC
                     CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
  480             CONTINUE
                  S2 = DSECND( )
                  UNTIME = S2 - S1
*
                  TIMES( IPAR, ITYPE, IN, 12 ) = MAX( TIME-UNTIME,
     $               ZERO ) / DBLE( IC )
                  OPCNTS( IPAR, ITYPE, IN, 12 ) = OPS / DBLE( IC )
  490          CONTINUE
            END IF
*
*           Time ZSVDC (singular values only) for each pair
*           NNB(j), LDAS(j)
*
            IF( TIMSUB( 13 ) ) THEN
               DO 530 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
*
*                 If this value of LDA has been used before, just
*                 use that value
*
                  LASTNL = 0
                  DO 500 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTNL = J
  500             CONTINUE
*
                  IF( LASTNL.EQ.0 ) THEN
*
*                    Time ZSVDC (singular values only)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  510                CONTINUE
                     CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL ZSVDC( H, LDA, M, N, DC, EC, U, LDA, VT, LDA,
     $                           WORK, 0, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 13 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 510
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 520 J = 1, IC
                        CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
  520                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 13 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 13 ) = OPS / DBLE( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 13 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 13 )
                     OPCNTS( IPAR, ITYPE, IN, 13 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 13 )
                  END IF
  530          CONTINUE
            END IF
*
*           Time ZSVDC (singular values and min(M,N) left singular
*           vectors) for each pair NNB(j), LDAS(j)
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
*                    Time ZSVDC (singular values and min(M,N) left
*                    singular vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  550                CONTINUE
                     CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL ZSVDC( H, LDA, M, N, DC, EC, U, LDA, VT, LDA,
     $                           WORK, 20, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 14 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 550
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 560 J = 1, IC
                        CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
  560                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 14 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 14 ) = OPS / DBLE( IC )
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
*           Time ZSVDC (singular values and M left singular
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
*                    Time ZSVDC (singular values and M left singular
*                    vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  590                CONTINUE
                     CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL ZSVDC( H, LDA, M, N, DC, EC, U, LDA, VT, LDA,
     $                           WORK, 10, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 14 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 590
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 600 J = 1, IC
                        CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
  600                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 15 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 15 ) = OPS / DBLE( IC )
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
*           Time ZSVDC (singular values and N right singular
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
*                    Time ZSVDC (singular values and N right singular
*                    vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  630                CONTINUE
                     CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL ZSVDC( H, LDA, M, N, DC, EC, U, LDA, VT, LDA,
     $                           WORK, 1, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 15 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 630
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 640 J = 1, IC
                        CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
  640                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 16 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = OPS / DBLE( IC )
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
*           Time ZSVDC (singular values and min(M,N) left singular
*           vectors and N right singular vectors) for each pair
*           NNB(j), LDAS(j)
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
*                    Time ZSVDC (singular values and min(M,N) left
*                    singular vectors and N right singular vectors)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  670                CONTINUE
                     CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
                     CALL ZSVDC( H, LDA, M, N, DC, EC, U, LDA, VT, LDA,
     $                           WORK, 21, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9998 )SUBNAM( 16 ), IINFO,
     $                     M, N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 700
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 670
*
*                    Subtract the time used in ZLACPY.
*
                     S1 = DSECND( )
                     DO 680 J = 1, IC
                        CALL ZLACPY( 'Full', M, N, A, M, H, LDA )
  680                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 16 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = OPS / DBLE( IC )
*
                  ELSE
*
                     TIMES( IPAR, ITYPE, IN, 16 ) = TIMES( LASTNL,
     $                  ITYPE, IN, 16 )
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = OPCNTS( LASTNL,
     $                  ITYPE, IN, 16 )
                  END IF
  690          CONTINUE
            END IF
*
  700    CONTINUE
  710 CONTINUE
*
*-----------------------------------------------------------------------
*
*     Print a table of results for each timed routine.
*
      DO 720 ISUB = 1, NSUBS
         IF( TIMSUB( ISUB ) ) THEN
            CALL DPRTBV( SUBNAM( ISUB ), NTYPES, DOTYPE, NSIZES, MM, NN,
     $                   INPARM( ISUB ), PNAMES, NPARMS, LDAS, NNB,
     $                   OPCNTS( 1, 1, 1, ISUB ), LDO1, LDO2,
     $                   TIMES( 1, 1, 1, ISUB ), LDT1, LDT2, RWORK,
     $                   LLWORK, NOUT )
         END IF
  720 CONTINUE
*
      RETURN
*
*     End of ZTIM26
*
 9998 FORMAT( ' ZTIM26: ', A, ' returned INFO=', I6, '.', / 9X, 'M=',
     $      I6, ', N=', I6, ', ITYPE=', I6, ', IPAR=', I6, ',         ',
     $      '        ISEED=(', 4( I5, ',' ), I5, ')' )
*
      END
