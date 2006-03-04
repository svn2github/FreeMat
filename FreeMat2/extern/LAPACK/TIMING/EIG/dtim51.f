      SUBROUTINE DTIM51( LINE, NSIZES, NN, NTYPES, DOTYPE, NPARMS, NNB,
     $                   NSHFTS, NEISPS, MINNBS, MINBKS, LDAS, TIMMIN,
     $                   NOUT, ISEED, A, B, H, T, Q, Z, W, WORK, LWORK,
     $                   LLWORK, TIMES, LDT1, LDT2, LDT3, OPCNTS, LDO1,
     $                   LDO2, LDO3, INFO )
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     September 30, 1994
*
*     .. Scalar Arguments ..
      CHARACTER*80       LINE
      INTEGER            INFO, LDO1, LDO2, LDO3, LDT1, LDT2, LDT3,
     $                   LWORK, NOUT, NPARMS, NSIZES, NTYPES
      DOUBLE PRECISION   TIMMIN
*     ..
*     .. Array Arguments ..
      LOGICAL            DOTYPE( * ), LLWORK( * )
      INTEGER            ISEED( * ), LDAS( * ), MINBKS( * ),
     $                   MINNBS( * ), NEISPS( * ), NN( * ), NNB( * ),
     $                   NSHFTS( * )
      DOUBLE PRECISION   A( * ), B( * ), H( * ),
     $                   OPCNTS( LDO1, LDO2, LDO3, * ), Q( * ), T( * ),
     $                   TIMES( LDT1, LDT2, LDT3, * ), W( * ),
     $                   WORK( * ), Z( * )
*     ..
*
*  Purpose
*  =======
*
*  DTIM51 times the LAPACK routines for the real non-symmetric
*  generalized eigenvalue problem   A x = w B x.
*
*  For each N value in NN(1:NSIZES) and .TRUE. value in
*  DOTYPE(1:NTYPES), a pair of matrices will be generated and used to
*  test the selected routines.  Thus, NSIZES*(number of .TRUE. values
*  in DOTYPE) matrices will be generated.
*
*  Arguments
*  =========
*
*  LINE    (input) CHARACTER*80
*          The input line which requested this routine.  This line may
*          contain a subroutine name, such as DGGHRD, indicating that
*          only routine DGGHRD will be timed, or it may contain a
*          generic name, such as DHG.  In this case, the rest of the
*          line is scanned for the first 18 non-blank characters,
*          corresponding to the eighteen combinations of subroutine and
*          options:
*          LAPACK:                                     Table Heading:
*           1: DGGHRD(no Q, no Z) (+DGEQRF, etc.)      'SGGHRD(N)'
*           2: DGGHRD(Q only)     (+DGEQRF, etc.)      'SGGHRD(Q)'
*           3: DGGHRD(Z only)     (+DGEQRF, etc.)      'SGGHRD(Z)'
*           4: DGGHRD(Q and Z)    (+DGEQRF, etc.)      'SGGHRD(Q,Z)'
*           5: DHGEQZ(Eigenvalues only)                'SHGEQZ(E)'
*           6: DHGEQZ(Schur form only)                 'SHGEQZ(S)'
*           7: DHGEQZ(Schur form and Q)                'SHGEQZ(Q)'
*           8: DHGEQZ(Schur form and Z)                'SHGEQZ(Z)'
*           9: DHGEQZ(Schur form, Q and Z)             'SHGEQZ(Q,Z)'
*          10: DTGEVC(SIDE='L', HOWMNY='A')            'STGEVC(L,A)'
*          11: DTGEVC(SIDE='L', HOWMNY='B')            'STGEVC(L,B)'
*          12: DTGEVC(SIDE='R', HOWMNY='A')            'STGEVC(R,A)'
*          13: DTGEVC(SIDE='R', HOWMNY='B')            'STGEVC(R,B)'
*          EISPACK:                       Compare w/:  Table Heading:
*          14: QZHES w/ matz=.false.            1      'QZHES(F)'
*          15: QZHES w/ matz=.true.             3      'QZHES(T)'
*          16: QZIT and QZVAL w/ matz=.false.   5      'QZIT(F)'
*          17: QZIT and QZVAL w/ matz=.true.    8      'QZIT(T)'
*          18: QZVEC                           13      'QZVEC'
*          If a character is 'T' or 't', the corresponding routine in
*          this path is timed.  If the entire line is blank, all the
*          routines in the path are timed.
*
*          Note that since QZHES does more than DGGHRD, the
*          "DGGHRD" timing also includes the time for the calls
*          to DGEQRF, DORMQR, and (if Q is computed) DORGQR
*          which are necessary to get the same functionality
*          as QZHES.
*
*  NSIZES  (input) INTEGER
*          The number of values of N contained in the vector NN.
*
*  NN      (input) INTEGER array, dimension (NSIZES)
*          The values of the matrix size N to be tested.  For each
*          N value in the array NN, and each .TRUE. value in DOTYPE,
*          a matrix A will be generated and used to test the routines.
*
*  NTYPES  (input) INTEGER
*          The number of types in DOTYPE.  Only the first MAXTYP
*          elements will be examined.  Exception: if NSIZES=1 and
*          NTYPES=MAXTYP+1, and DOTYPE=MAXTYP*f,t, then the input
*          value of A will be used.
*
*  DOTYPE  (input) LOGICAL
*          If DOTYPE(j) is .TRUE., then a pair of matrices (A,B) of
*          type j will be generated.  A and B have the form  U T1 V
*          and  U T2 V , resp., where U and V are orthogonal, T1 is
*          block upper triangular (with 1x1 and 2x2 diagonal blocks),
*          and T2 is upper triangular.  T2 has random O(1) entries in
*          the strict upper triangle and ( 0, 1, 0, 1, 1, ..., 1, 0 )
*          on the diagonal, while T1 has random O(1) entries in the
*          strict (block) upper triangle, its block diagonal will have
*          the singular values:
*          (j=1)   0, 0, 1, 1, ULP,..., ULP, 0.
*          (j=2)   0, 0, 1, 1, 1-d, 1-2*d, ..., 1-(N-5)*d=ULP, 0.
*
*                                  2        N-5
*          (j=3)   0, 0, 1, 1, a, a , ..., a   =ULP, 0.
*          (j=4)   0, 0, 1, r1, r2, ..., r(N-4), 0, where r1, etc.
*                  are random numbers in (ULP,1).
*
*  NPARMS  (input) INTEGER
*          The number of values in each of the arrays NNB, NSHFTS,
*          NEISPS, and LDAS.  For each matrix A generated according to
*          NN and DOTYPE, tests will be run with (NB,NSHIFT,NEISP,LDA)=
*          (NNB(1), NSHFTS(1), NEISPS(1), LDAS(1)),...,
*          (NNB(NPARMS), NSHFTS(NPARMS), NEISPS(NPARMS), LDAS(NPARMS))
*
*  NNB     (input) INTEGER array, dimension (NPARMS)
*          The values of the blocksize ("NB") to be tested.  They must
*          be at least 1.  Currently, this is only used by DGEQRF,
*          etc., in the timing of DGGHRD.
*
*  NSHFTS  (input) INTEGER array, dimension (NPARMS)
*          The values of the number of shifts ("NSHIFT") to be tested.
*          (Currently not used.)
*
*  NEISPS  (input) INTEGER array, dimension (NPARMS)
*          The values of "NEISP", the size of largest submatrix to be
*          processed by DLAEQZ (EISPACK method), to be tested.
*          (Currently not used.)
*
*  MINNBS  (input) INTEGER array, dimension (NPARMS)
*          The values of "MINNB", the minimum size of a product of
*          transformations which may be applied as a blocked
*          transformation, to be tested.  (Currently not used.)
*
*  MINBKS  (input) INTEGER array, dimension (NPARMS)
*          The values of "MINBK", the minimum number of rows/columns
*          to be updated with a blocked transformation, to be tested.
*          (Currently not used.)
*
*  LDAS    (input) INTEGER array, dimension (NPARMS)
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
*  ISEED   (input/output) INTEGER array, dimension (4)
*          The random seed used by the random number generator, used
*          by the test matrix generator.  It is used and updated on
*          each call to DTIM51
*
*  A       (workspace) DOUBLE PRECISION array, dimension
*                      (max(NN)*max(LDAS))
*          (a) During the testing of DGGHRD, "A", the original
*              left-hand-side matrix to be tested.
*          (b) Later, "S", the Schur form of the original "A" matrix.
*
*  B       (workspace) DOUBLE PRECISION array, dimension
*                      (max(NN)*max(LDAS))
*          (a) During the testing of DGGHRD, "B", the original
*              right-hand-side matrix to be tested.
*          (b) Later, "P", the Schur form of the original "B" matrix.
*
*  H       (workspace) DOUBLE PRECISION array, dimension
*                      (max(NN)*max(LDAS))
*          (a) During the testing of DGGHRD and DHGEQZ, "H", the
*              Hessenberg form of the original "A" matrix.
*          (b) During the testing of DTGEVC, "L", the matrix of left
*              eigenvectors.
*
*  T       (workspace) DOUBLE PRECISION array, dimension
*                      (max(NN)*max(LDAS))
*          (a) During the testing of DGGHRD and DHGEQZ, "T", the
*              triangular form of the original "B" matrix.
*          (b) During the testing of DTGEVC, "R", the matrix of right
*              eigenvectors.
*
*  Q       (workspace) DOUBLE PRECISION array, dimension
*                      (max(NN)*max(LDAS))
*          The orthogonal matrix on the left generated by DGGHRD.  If
*          DHGEQZ computes only Q or Z, then that matrix is stored here.
*          If both Q and Z are computed, the Q matrix goes here.
*
*  Z       (workspace) DOUBLE PRECISION array, dimension
*                      (max(NN)*max(LDAS))
*          The orthogonal matrix on the right generated by DGGHRD.
*          If DHGEQZ computes both Q and Z, the Z matrix is stored here.
*          Also used as scratch space for timing the DLACPY calls.
*
*  W       (workspace) DOUBLE PRECISION array, dimension (3*max(LDAS))
*          Treated as an LDA x 3 matrix whose 1st and 2nd columns hold
*          ALPHAR and ALPHAI, the real and imaginary parts of the
*          diagonal entries of "S" that would result from reducing "S"
*          and "P" simultaneously to triangular form), and whose 3rd
*          column holds BETA, the diagonal entries of "P" that would so
*          result.
*
*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK)
*
*  LWORK   (input) INTEGER
*          Number of elements in WORK.  It must be at least
*          (a)  6*max(NN)
*          (b)  NSIZES*NTYPES*NPARMS
*
*  LLWORK  (workspace) LOGICAL array, dimension (max( max(NN), NPARMS ))
*
*  TIMES   (output) DOUBLE PRECISION array, dimension
*                   (LDT1,LDT2,LDT3,NSUBS)
*          TIMES(i,j,k,l) will be set to the run time (in seconds) for
*          subroutine l, with N=NN(k), matrix type j, and LDA=LDAS(i),
*          NEISP=NEISPS(i), NBLOCK=NNB(i), NSHIFT=NSHFTS(i),
*          MINNB=MINNBS(i), and MINBLK=MINBKS(i).
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
*  OPCNTS  (output) DOUBLE PRECISION array, dimension
*                   (LDO1,LDO2,LDO3,NSUBS)
*          OPCNTS(i,j,k,l) will be set to the number of floating-point
*          operations executed by subroutine l, with N=NN(k), matrix
*          type j, and LDA=LDAS(i), NEISP=NEISPS(i), NBLOCK=NNB(i),
*          NSHIFT=NSHFTS(i), MINNB=MINNBS(i), and MINBLK=MINBKS(i).
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
      PARAMETER          ( MAXTYP = 4, NSUBS = 18 )
      DOUBLE PRECISION   ZERO, ONE
      PARAMETER          ( ZERO = 0.0D0, ONE = 1.0D0 )
*     ..
*     .. Local Scalars ..
      LOGICAL            RUNEQ, RUNES, RUNHES, RUNHRD, RUNQZ
      INTEGER            IC, IINFO, IN, IPAR, ISUB, ITEMP, ITYPE, J, J1,
     $                   J2, J3, J4, JC, JR, LASTL, LDA, LDAMIN, LDH,
     $                   LDQ, LDS, LDW, MINBLK, MINNB, MTYPES, N, N1,
     $                   NB, NBSMAX, NEISP, NMAX, NSHIFT
      DOUBLE PRECISION   S1, S2, TIME, ULP, UNTIME
*     ..
*     .. Local Arrays ..
      LOGICAL            TIMSUB( NSUBS )
      CHARACTER*6        PNAMES( 6 )
      CHARACTER*11       SUBNAM( NSUBS )
      INTEGER            INPARM( NSUBS ), IOLDSD( 4 ), KATYPE( MAXTYP )
*     ..
*     .. External Functions ..
      DOUBLE PRECISION   DLAMCH, DLARND, DOPLA, DSECND
      EXTERNAL           DLAMCH, DLARND, DOPLA, DSECND
*     ..
*     .. External Subroutines ..
      EXTERNAL           ATIMIN, DHGEQZ, DLACPY, DLAQZH, DLARFG, DLASET,
     $                   DLATM4, DORM2R, DPRTBG, DTGEVC, QZHES, QZIT,
     $                   QZVAL, QZVEC, XLAENV
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          ABS, DBLE, MAX, MIN, SIGN
*     ..
*     .. Common blocks ..
      COMMON             / LATIME / OPS, ITCNT
*     ..
*     .. Scalars in Common ..
      DOUBLE PRECISION   ITCNT, OPS
*     ..
*     .. Data statements ..
      DATA               SUBNAM / 'DGGHRD(N)', 'DGGHRD(Q)', 'DGGHRD(Z)',
     $                   'DGGHRD(Q,Z)', 'DHGEQZ(E)', 'DHGEQZ(S)',
     $                   'DHGEQZ(Q)', 'DHGEQZ(Z)', 'DHGEQZ(Q,Z)',
     $                   'DTGEVC(L,A)', 'DTGEVC(L,B)', 'DTGEVC(R,A)',
     $                   'DTGEVC(R,B)', 'QZHES(F)', 'QZHES(T)',
     $                   'QZIT(F)', 'QZIT(T)', 'QZVEC' /
      DATA               INPARM / 4*2, 5*1, 4*1, 5*1 /
      DATA               PNAMES / '   LDA', '    NB', '    NS',
     $                   ' NEISP', ' MINNB', 'MINBLK' /
      DATA               KATYPE / 5, 8, 7, 9 /
*     ..
*     .. Executable Statements ..
*
*     Quick Return
*
      INFO = 0
      IF( NSIZES.LE.0 .OR. NTYPES.LE.0 .OR. NPARMS.LE.0 )
     $   RETURN
*
*     Extract the timing request from the input line.
*
      CALL ATIMIN( 'DHG', LINE, NSUBS, SUBNAM, TIMSUB, NOUT, INFO )
      IF( INFO.NE.0 )
     $   RETURN
*
*     Compute Maximum Values
*
      NMAX = 0
      DO 10 J1 = 1, NSIZES
         NMAX = MAX( NMAX, NN( J1 ) )
   10 CONTINUE
*
      LDAMIN = 2*MAX( 1, NMAX )
      NBSMAX = 0
      DO 20 J1 = 1, NPARMS
         LDAMIN = MIN( LDAMIN, LDAS( J1 ) )
         NBSMAX = MAX( NBSMAX, NNB( J1 )+NSHFTS( J1 ) )
   20 CONTINUE
*
*     Check that N <= LDA for the input values.
*
      IF( NMAX.GT.LDAMIN ) THEN
         INFO = -12
         WRITE( NOUT, FMT = 9999 )LINE( 1: 6 )
 9999    FORMAT( 1X, A, ' timing run not attempted -- N > LDA', / )
         RETURN
      END IF
*
*     Check LWORK
*
      IF( LWORK.LT.MAX( ( NBSMAX+1 )*( 2*NBSMAX+NMAX+1 ), 6*NMAX,
     $    NSIZES*NTYPES*NPARMS ) ) THEN
         INFO = -24
         WRITE( NOUT, FMT = 9998 )LINE( 1: 6 )
 9998    FORMAT( 1X, A, ' timing run not attempted -- LWORK too small.',
     $         / )
         RETURN
      END IF
*
*     Check to see whether DGGHRD or DHGEQZ must be run.
*        RUNHRD -- if DGGHRD must be run.
*        RUNES  -- if DHGEQZ must be run to get Schur form.
*        RUNEQ  -- if DHGEQZ must be run to get Schur form and Q.
*
      RUNHRD = .FALSE.
      RUNES = .FALSE.
      RUNEQ = .FALSE.
*
      IF( TIMSUB( 10 ) .OR. TIMSUB( 12 ) )
     $   RUNES = .TRUE.
      IF( TIMSUB( 11 ) .OR. TIMSUB( 13 ) )
     $   RUNEQ = .TRUE.
      IF( TIMSUB( 5 ) .OR. TIMSUB( 6 ) .OR. TIMSUB( 7 ) .OR.
     $    TIMSUB( 8 ) .OR. TIMSUB( 9 ) .OR. RUNES .OR. RUNEQ )
     $    RUNHRD = .TRUE.
*
      IF( TIMSUB( 6 ) .OR. TIMSUB( 7 ) .OR. TIMSUB( 8 ) .OR.
     $    TIMSUB( 9 ) .OR. RUNEQ )RUNES = .FALSE.
      IF( TIMSUB( 7 ) .OR. TIMSUB( 8 ) .OR. TIMSUB( 9 ) )
     $   RUNEQ = .FALSE.
      IF( TIMSUB( 1 ) .OR. TIMSUB( 2 ) .OR. TIMSUB( 3 ) .OR.
     $    TIMSUB( 4 ) )RUNHRD = .FALSE.
*
*     Check to see whether QZHES or QZIT must be run.
*
*     RUNHES -- if QZHES must be run.
*     RUNQZ  -- if QZIT and QZVAL must be run (w/ MATZ=.TRUE.).
*
      RUNHES = .FALSE.
      RUNQZ = .FALSE.
*
      IF( TIMSUB( 18 ) )
     $   RUNQZ = .TRUE.
      IF( TIMSUB( 16 ) .OR. TIMSUB( 17 ) .OR. RUNQZ )
     $   RUNHES = .TRUE.
      IF( TIMSUB( 17 ) )
     $   RUNQZ = .FALSE.
      IF( TIMSUB( 14 ) .OR. TIMSUB( 15 ) )
     $   RUNHES = .FALSE.
*
*     Various Constants
*
      ULP = DLAMCH( 'Epsilon' )*DLAMCH( 'Base' )
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
      DO 930 IN = 1, NSIZES
*
         N = NN( IN )
         N1 = MAX( 1, N )
*
*        Do for each .TRUE. value in DOTYPE:
*
         MTYPES = MIN( MAXTYP, NTYPES )
         IF( NTYPES.EQ.MAXTYP+1 .AND. NSIZES.EQ.1 )
     $      MTYPES = NTYPES
         DO 920 ITYPE = 1, MTYPES
            IF( .NOT.DOTYPE( ITYPE ) )
     $         GO TO 920
*
*           Save random number seed for error messages
*
            DO 70 J = 1, 4
               IOLDSD( J ) = ISEED( J )
   70       CONTINUE
*
*           Time the LAPACK Routines
*
*           Generate A and B
*
            IF( ITYPE.LE.MAXTYP ) THEN
*
*              Generate A (w/o rotation)
*
               CALL DLATM4( KATYPE( ITYPE ), N, 3, 1, 2, ONE, ULP, ONE,
     $                      2, ISEED, A, N1 )
               IF( 3.LE.N )
     $            A( 3+2*N1 ) = ONE
*
*              Generate B (w/o rotation)
*
               CALL DLATM4( 8, N, 3, 1, 0, ONE, ONE, ONE, 2, ISEED, B,
     $                      N1 )
               IF( 2.LE.N )
     $            B( 2+N1 ) = ONE
*
               IF( N.GT.0 ) THEN
*
*                 Include rotations
*
*                 Generate U, V as Householder transformations times
*                 a diagonal matrix.
*
                  DO 90 JC = 1, N - 1
                     IC = ( JC-1 )*N1
                     DO 80 JR = JC, N
                        Q( JR+IC ) = DLARND( 3, ISEED )
                        Z( JR+IC ) = DLARND( 3, ISEED )
   80                CONTINUE
                     CALL DLARFG( N+1-JC, Q( JC+IC ), Q( JC+1+IC ), 1,
     $                            WORK( JC ) )
                     WORK( 2*N+JC ) = SIGN( ONE, Q( JC+IC ) )
                     Q( JC+IC ) = ONE
                     CALL DLARFG( N+1-JC, Z( JC+IC ), Z( JC+1+IC ), 1,
     $                            WORK( N+JC ) )
                     WORK( 3*N+JC ) = SIGN( ONE, Z( JC+IC ) )
                     Z( JC+IC ) = ONE
   90             CONTINUE
                  IC = ( N-1 )*N1
                  Q( N+IC ) = ONE
                  WORK( N ) = ZERO
                  WORK( 3*N ) = SIGN( ONE, DLARND( 2, ISEED ) )
                  Z( N+IC ) = ONE
                  WORK( 2*N ) = ZERO
                  WORK( 4*N ) = SIGN( ONE, DLARND( 2, ISEED ) )
*
*                 Apply the diagonal matrices
*
                  DO 110 JC = 1, N
                     DO 100 JR = 1, N
                        A( JR+IC ) = WORK( 2*N+JR )*WORK( 3*N+JC )*
     $                               A( JR+IC )
                        B( JR+IC ) = WORK( 2*N+JR )*WORK( 3*N+JC )*
     $                               B( JR+IC )
  100                CONTINUE
  110             CONTINUE
                  CALL DORM2R( 'L', 'N', N, N, N-1, Q, N1, WORK, A, N1,
     $                         WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 120
                  CALL DORM2R( 'R', 'T', N, N, N-1, Z, N1, WORK( N+1 ),
     $                         A, N1, WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 120
                  CALL DORM2R( 'L', 'N', N, N, N-1, Q, N1, WORK, B, N1,
     $                         WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 120
                  CALL DORM2R( 'R', 'T', N, N, N-1, Z, N1, WORK( N+1 ),
     $                         B, N1, WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 120
               END IF
  120          CONTINUE
            END IF
*
* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
*
*           Time DGGHRD
*
*           Time DGEQRF+DGGHRD('N','N',...) for each pair
*           (LDAS(j),NNB(j))
*
            IF( TIMSUB( 1 ) ) THEN
               DO 160 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = NNB( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 1 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 1 ) = ZERO
                     GO TO 160
                  END IF
*
*                 If this value of (NB,LDA) has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 130 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) .AND. NB.EQ.NNB( J ) )
     $                  LASTL = J
  130             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time DGGHRD, computing neither Q nor Z
*                    (Actually, time DGEQRF + DORMQR + DGGHRD.)
*
                     CALL XLAENV( 1, NB )
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  140                CONTINUE
                     CALL DLACPY( 'Full', N, N, A, N1, H, LDA )
                     CALL DLACPY( 'Full', N, N, B, N1, T, LDA )
                     CALL DLAQZH( .FALSE., .FALSE., N, 1, N, H, LDA, T,
     $                            LDA, Q, LDA, Z, LDA, WORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 1 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
*
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 140
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 150 J = 1, IC
                        CALL DLACPY( 'Full', N, N, A, N1, Z, LDA )
                        CALL DLACPY( 'Full', N, N, B, N1, Z, LDA )
  150                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 1 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 1 ) = OPS / DBLE( IC ) +
     $                  DOPLA( 'DGEQRF', N, N, 0, 0, NB ) +
     $                  DOPLA( 'DORMQR', N, N, 0, 0, NB )
                     LDH = LDA
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 1 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 1 )
                     TIMES( IPAR, ITYPE, IN, 1 ) = TIMES( LASTL, ITYPE,
     $                  IN, 1 )
                  END IF
  160          CONTINUE
            ELSE IF( RUNHRD ) THEN
               CALL DLACPY( 'Full', N, N, A, N1, H, N1 )
               CALL DLACPY( 'Full', N, N, B, N1, T, N1 )
               CALL DLAQZH( .FALSE., .FALSE., N, 1, N, H, N1, T, N1, Q,
     $                      N1, Z, N1, WORK, IINFO )
               IF( IINFO.NE.0 ) THEN
                  WRITE( NOUT, FMT = 9997 )SUBNAM( 1 ), IINFO, N,
     $               ITYPE, 0, IOLDSD
                  INFO = ABS( IINFO )
                  GO TO 920
               END IF
               LDH = N
            END IF
*
*           Time DGGHRD('I','N',...) for each pair (LDAS(j),NNB(j))
*
            IF( TIMSUB( 2 ) ) THEN
               DO 200 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = NNB( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 2 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 2 ) = ZERO
                     GO TO 200
                  END IF
*
*                 If this value of (NB,LDA) has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 170 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) .AND. NB.EQ.NNB( J ) )
     $                  LASTL = J
  170             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time DGGHRD, computing Q but not Z
*                    (Actually, DGEQRF + DORMQR + DORGQR + DGGHRD.)
*
                     CALL XLAENV( 1, NB )
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  180                CONTINUE
                     CALL DLACPY( 'Full', N, N, A, N1, H, LDA )
                     CALL DLACPY( 'Full', N, N, B, N1, T, LDA )
                     CALL DLAQZH( .TRUE., .FALSE., N, 1, N, H, LDA, T,
     $                            LDA, Q, LDA, Z, LDA, WORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 2 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
*
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 180
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 190 J = 1, IC
                        CALL DLACPY( 'Full', N, N, A, N1, Z, LDA )
                        CALL DLACPY( 'Full', N, N, B, N1, Z, LDA )
  190                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 2 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 2 ) = OPS / DBLE( IC ) +
     $                  DOPLA( 'DGEQRF', N, N, 0, 0, NB ) +
     $                  DOPLA( 'DORMQR', N, N, 0, 0, NB ) +
     $                  DOPLA( 'DORGQR', N, N, 0, 0, NB )
                     LDH = LDA
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 2 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 2 )
                     TIMES( IPAR, ITYPE, IN, 2 ) = TIMES( LASTL, ITYPE,
     $                  IN, 2 )
                  END IF
  200          CONTINUE
            END IF
*
*           Time DGGHRD('N','I',...) for each pair (LDAS(j),NNB(j))
*
            IF( TIMSUB( 3 ) ) THEN
               DO 240 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = NNB( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 3 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 3 ) = ZERO
                     GO TO 240
                  END IF
*
*                 If this value of (NB,LDA) has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 210 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) .AND. NB.EQ.NNB( J ) )
     $                  LASTL = J
  210             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time DGGHRD, computing Z but not Q
*                    (Actually, DGEQRF + DORMQR + DGGHRD.)
*
                     CALL XLAENV( 1, NB )
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  220                CONTINUE
                     CALL DLACPY( 'Full', N, N, A, N1, H, LDA )
                     CALL DLACPY( 'Full', N, N, B, N1, T, LDA )
                     CALL DLAQZH( .FALSE., .TRUE., N, 1, N, H, LDA, T,
     $                            LDA, Q, LDA, Z, LDA, WORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 3 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
*
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 220
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 230 J = 1, IC
                        CALL DLACPY( 'Full', N, N, A, N1, Z, LDA )
                        CALL DLACPY( 'Full', N, N, B, N1, Z, LDA )
  230                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 3 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 3 ) = OPS / DBLE( IC ) +
     $                  DOPLA( 'DGEQRF', N, N, 0, 0, NB ) +
     $                  DOPLA( 'DORMQR', N, N, 0, 0, NB )
                     LDH = LDA
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 3 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 3 )
                     TIMES( IPAR, ITYPE, IN, 3 ) = TIMES( LASTL, ITYPE,
     $                  IN, 3 )
                  END IF
  240          CONTINUE
            END IF
*
*           Time DGGHRD('I','I',...) for each pair (LDAS(j),NNB(j))
*
            IF( TIMSUB( 4 ) ) THEN
               DO 280 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  NB = NNB( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 4 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 4 ) = ZERO
                     GO TO 280
                  END IF
*
*                 If this value of (NB,LDA) has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 250 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) .AND. NB.EQ.NNB( J ) )
     $                  LASTL = J
  250             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time DGGHRD, computing Q and Z
*                    (Actually, DGEQRF + DORMQR + DORGQR + DGGHRD.)
*
                     CALL XLAENV( 1, NB )
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  260                CONTINUE
                     CALL DLACPY( 'Full', N, N, A, N1, H, LDA )
                     CALL DLACPY( 'Full', N, N, B, N1, T, LDA )
                     CALL DLAQZH( .TRUE., .TRUE., N, 1, N, H, LDA, T,
     $                            LDA, Q, LDA, Z, LDA, WORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 4 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
*
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 260
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 270 J = 1, IC
                        CALL DLACPY( 'Full', N, N, A, N1, Z, LDA )
                        CALL DLACPY( 'Full', N, N, B, N1, Z, LDA )
  270                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 4 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 4 ) = OPS / DBLE( IC ) +
     $                  DOPLA( 'DGEQRF', N, N, 0, 0, NB ) +
     $                  DOPLA( 'DORMQR', N, N, 0, 0, NB ) +
     $                  DOPLA( 'DORGQR', N, N, 0, 0, NB )
                     LDH = LDA
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 4 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 4 )
                     TIMES( IPAR, ITYPE, IN, 4 ) = TIMES( LASTL, ITYPE,
     $                  IN, 4 )
                  END IF
  280          CONTINUE
            END IF
*
* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
*
*           Time DHGEQZ
*
*           Time DHGEQZ with JOB='E' for each value of LDAS(j)
*
            IF( TIMSUB( 5 ) ) THEN
               DO 320 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 5 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 5 ) = ZERO
                     GO TO 320
                  END IF
*
*                 If this value of LDA has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 290 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  290             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time DHGEQZ with JOB='E'
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  300                CONTINUE
                     CALL DLACPY( 'Full', N, N, H, LDH, A, LDA )
                     CALL DLACPY( 'Full', N, N, T, LDH, B, LDA )
                     CALL DHGEQZ( 'E', 'N', 'N', N, 1, N, A, LDA, B,
     $                            LDA, W, W( LDA+1 ), W( 2*LDA+1 ), Q,
     $                            LDA, Z, LDA, WORK, LWORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 5 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 300
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 310 J = 1, IC
                        CALL DLACPY( 'Full', N, N, H, LDH, Z, LDA )
                        CALL DLACPY( 'Full', N, N, T, LDH, Z, LDA )
  310                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 5 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 5 ) = OPS / DBLE( IC )
                     LDS = 0
                     LDQ = 0
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 5 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 5 )
                     TIMES( IPAR, ITYPE, IN, 5 ) = TIMES( LASTL, ITYPE,
     $                  IN, 5 )
                  END IF
  320          CONTINUE
            END IF
*
*           Time DHGEQZ with JOB='S', COMPQ=COMPZ='N' for each value
*           of LDAS(j)
*
            IF( TIMSUB( 6 ) ) THEN
               DO 360 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 6 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 6 ) = ZERO
                     GO TO 360
                  END IF
*
*                 If this value of LDA has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 330 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  330             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                 Time DHGEQZ with JOB='S', COMPQ=COMPZ='N'
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  340                CONTINUE
                     CALL DLACPY( 'Full', N, N, H, LDH, A, LDA )
                     CALL DLACPY( 'Full', N, N, T, LDH, B, LDA )
                     CALL DHGEQZ( 'S', 'N', 'N', N, 1, N, A, LDA, B,
     $                            LDA, W, W( LDA+1 ), W( 2*LDA+1 ), Q,
     $                            LDA, Z, LDA, WORK, LWORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 6 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 340
*
*                 Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 350 J = 1, IC
                        CALL DLACPY( 'Full', N, N, H, LDH, Z, LDA )
                        CALL DLACPY( 'Full', N, N, T, LDH, Z, LDA )
  350                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 6 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 6 ) = OPS / DBLE( IC )
                     LDS = LDA
                     LDQ = 0
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 6 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 6 )
                     TIMES( IPAR, ITYPE, IN, 6 ) = TIMES( LASTL, ITYPE,
     $                  IN, 6 )
                  END IF
  360          CONTINUE
            ELSE IF( RUNES ) THEN
               CALL DLACPY( 'Full', N, N, H, LDH, A, N1 )
               CALL DLACPY( 'Full', N, N, T, LDH, B, N1 )
               CALL DHGEQZ( 'S', 'N', 'N', N, 1, N, A, N1, B, N1, W,
     $                      W( N1+1 ), W( 2*N1+1 ), Q, N1, Z, N1, WORK,
     $                      LWORK, IINFO )
               IF( IINFO.NE.0 ) THEN
                  WRITE( NOUT, FMT = 9997 )SUBNAM( 6 ), IINFO, N,
     $               ITYPE, 0, IOLDSD
                  INFO = ABS( IINFO )
                  GO TO 920
               END IF
               LDS = N1
               LDQ = 0
            END IF
*
*           Time DHGEQZ with JOB='S', COMPQ='I', COMPZ='N' for each
*           value of LDAS(j)
*
            IF( TIMSUB( 7 ) ) THEN
               DO 400 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 7 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 7 ) = ZERO
                     GO TO 400
                  END IF
*
*                 If this value of LDA has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 370 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  370             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                 Time DHGEQZ with JOB='S', COMPQ='I', COMPZ='N'
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  380                CONTINUE
                     CALL DLACPY( 'Full', N, N, H, LDH, A, LDA )
                     CALL DLACPY( 'Full', N, N, T, LDH, B, LDA )
                     CALL DHGEQZ( 'S', 'I', 'N', N, 1, N, A, LDA, B,
     $                            LDA, W, W( LDA+1 ), W( 2*LDA+1 ), Q,
     $                            LDA, Z, LDA, WORK, LWORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 7 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 380
*
*                 Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 390 J = 1, IC
                        CALL DLACPY( 'Full', N, N, H, LDH, Z, LDA )
                        CALL DLACPY( 'Full', N, N, T, LDH, Z, LDA )
  390                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 7 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 7 ) = OPS / DBLE( IC )
                     LDS = LDA
                     LDQ = LDA
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 7 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 7 )
                     TIMES( IPAR, ITYPE, IN, 7 ) = TIMES( LASTL, ITYPE,
     $                  IN, 7 )
                  END IF
  400          CONTINUE
            ELSE IF( RUNEQ ) THEN
               CALL DLACPY( 'Full', N, N, H, LDH, A, N1 )
               CALL DLACPY( 'Full', N, N, T, LDH, B, N1 )
               CALL DHGEQZ( 'S', 'I', 'N', N, 1, N, A, N1, B, N1, W,
     $                      W( N1+1 ), W( 2*N1+1 ), Q, N1, Z, N1, WORK,
     $                      LWORK, IINFO )
               IF( IINFO.NE.0 ) THEN
                  WRITE( NOUT, FMT = 9997 )SUBNAM( 7 ), IINFO, N,
     $               ITYPE, 0, IOLDSD
                  INFO = ABS( IINFO )
                  GO TO 920
               END IF
               LDS = N1
               LDQ = N1
            END IF
*
*           Time DHGEQZ with JOB='S', COMPQ='N', COMPZ='I' for each
*           value of LDAS(j)
*
            IF( TIMSUB( 8 ) ) THEN
               DO 440 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 8 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 8 ) = ZERO
                     GO TO 440
                  END IF
*
*                 If this value of LDA has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 410 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  410             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
                     NB = MIN( N, NNB( IPAR ) )
                     NSHIFT = NSHFTS( IPAR )
                     NEISP = NEISPS( IPAR )
                     MINNB = MINNBS( IPAR )
                     MINBLK = MINBKS( IPAR )
*
*                 Time DHGEQZ with JOB='S', COMPQ='N', COMPZ='I'
*                 (Note that the "Z" matrix is stored in the array Q)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  420                CONTINUE
                     CALL DLACPY( 'Full', N, N, H, LDH, A, LDA )
                     CALL DLACPY( 'Full', N, N, T, LDH, B, LDA )
                     CALL DHGEQZ( 'S', 'N', 'I', N, 1, N, A, LDA, B,
     $                            LDA, W, W( LDA+1 ), W( 2*LDA+1 ), Z,
     $                            LDA, Q, LDA, WORK, LWORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 8 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 420
*
*                 Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 430 J = 1, IC
                        CALL DLACPY( 'Full', N, N, H, LDH, Z, LDA )
                        CALL DLACPY( 'Full', N, N, T, LDH, Z, LDA )
  430                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 8 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 8 ) = OPS / DBLE( IC )
                     LDS = LDA
                     LDQ = LDA
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 8 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 8 )
                     TIMES( IPAR, ITYPE, IN, 8 ) = TIMES( LASTL, ITYPE,
     $                  IN, 8 )
                  END IF
  440          CONTINUE
            END IF
*
*           Time DHGEQZ with JOB='S', COMPQ='I', COMPZ='I' for each
*           value of LDAS(j)
*
            IF( TIMSUB( 9 ) ) THEN
               DO 480 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 9 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 9 ) = ZERO
                     GO TO 480
                  END IF
*
*                 If this value of LDA has occurred before,
*                 just use that value.
*
                  LASTL = 0
                  DO 450 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  450             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                 Time DHGEQZ with JOB='S', COMPQ='I', COMPZ='I'
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  460                CONTINUE
                     CALL DLACPY( 'Full', N, N, H, LDH, A, LDA )
                     CALL DLACPY( 'Full', N, N, T, LDH, B, LDA )
                     CALL DHGEQZ( 'S', 'I', 'I', N, 1, N, A, LDA, B,
     $                            LDA, W, W( LDA+1 ), W( 2*LDA+1 ), Q,
     $                            LDA, Z, LDA, WORK, LWORK, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 9 ), IINFO, N,
     $                     ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 460
*
*                 Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 470 J = 1, IC
                        CALL DLACPY( 'Full', N, N, H, LDH, Z, LDA )
                        CALL DLACPY( 'Full', N, N, T, LDH, Z, LDA )
  470                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 9 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 9 ) = OPS / DBLE( IC )
                     LDS = LDA
                     LDQ = LDA
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 9 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 9 )
                     TIMES( IPAR, ITYPE, IN, 9 ) = TIMES( LASTL, ITYPE,
     $                  IN, 9 )
                  END IF
  480          CONTINUE
            END IF
*
* . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
*
*           Time DTGEVC
*
            IF( TIMSUB( 10 ) .OR. TIMSUB( 11 ) .OR. TIMSUB( 12 ) .OR.
     $          TIMSUB( 13 ) ) THEN
               DO 610 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     DO 490 J = 10, 13
                        IF( TIMSUB( J ) ) THEN
                           TIMES( IPAR, ITYPE, IN, J ) = ZERO
                           OPCNTS( IPAR, ITYPE, IN, J ) = ZERO
                        END IF
  490                CONTINUE
                     GO TO 610
                  END IF
*
*                 If this value of LDA has come up before, just use
*                 the value previously computed.
*
                  LASTL = 0
                  DO 500 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  500             CONTINUE
*
*                 Time DTGEVC if this is a new value of LDA
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Copy S (which is in A) and P (which is in B)
*                    if necessary to get right LDA.
*
                     IF( LDA.GT.LDS ) THEN
                        DO 520 JC = N, 1, -1
                           DO 510 JR = N, 1, -1
                              A( JR+( JC-1 )*LDA ) = A( JR+( JC-1 )*
     $                           LDS )
                              B( JR+( JC-1 )*LDA ) = B( JR+( JC-1 )*
     $                           LDS )
  510                      CONTINUE
  520                   CONTINUE
                     ELSE IF( LDA.LT.LDS ) THEN
                        DO 540 JC = 1, N
                           DO 530 JR = 1, N
                              A( JR+( JC-1 )*LDA ) = A( JR+( JC-1 )*
     $                           LDS )
                              B( JR+( JC-1 )*LDA ) = B( JR+( JC-1 )*
     $                           LDS )
  530                      CONTINUE
  540                   CONTINUE
                     END IF
                     LDS = LDA
*
*                    Time DTGEVC for Left Eigenvectors only,
*                    without back transforming
*
                     IF( TIMSUB( 10 ) ) THEN
                        IC = 0
                        OPS = ZERO
                        S1 = DSECND( )
  550                   CONTINUE
                        CALL DTGEVC( 'L', 'A', LLWORK, N, A, LDA, B,
     $                               LDA, H, LDA, T, LDA, N, ITEMP,
     $                               WORK, IINFO )
                        IF( IINFO.NE.0 ) THEN
                           WRITE( NOUT, FMT = 9997 )SUBNAM( 10 ),
     $                        IINFO, N, ITYPE, IPAR, IOLDSD
                           INFO = ABS( IINFO )
                           GO TO 920
                        END IF
                        S2 = DSECND( )
                        TIME = S2 - S1
                        IC = IC + 1
                        IF( TIME.LT.TIMMIN )
     $                     GO TO 550
*
                        TIMES( IPAR, ITYPE, IN, 10 ) = TIME / DBLE( IC )
                        OPCNTS( IPAR, ITYPE, IN, 10 ) = OPS / DBLE( IC )
                     END IF
*
*                    Time DTGEVC for Left Eigenvectors only,
*                    with back transforming
*
                     IF( TIMSUB( 11 ) ) THEN
                        IC = 0
                        OPS = ZERO
                        S1 = DSECND( )
  560                   CONTINUE
                        CALL DLACPY( 'Full', N, N, Q, LDQ, H, LDA )
                        CALL DTGEVC( 'L', 'B', LLWORK, N, A, LDA, B,
     $                               LDA, H, LDA, T, LDA, N, ITEMP,
     $                               WORK, IINFO )
                        IF( IINFO.NE.0 ) THEN
                           WRITE( NOUT, FMT = 9997 )SUBNAM( 11 ),
     $                        IINFO, N, ITYPE, IPAR, IOLDSD
                           INFO = ABS( IINFO )
                           GO TO 920
                        END IF
                        S2 = DSECND( )
                        TIME = S2 - S1
                        IC = IC + 1
                        IF( TIME.LT.TIMMIN )
     $                     GO TO 560
*
*                       Subtract the time used in DLACPY.
*
                        S1 = DSECND( )
                        DO 570 J = 1, IC
                           CALL DLACPY( 'Full', N, N, Q, LDQ, H, LDA )
  570                   CONTINUE
                        S2 = DSECND( )
                        UNTIME = S2 - S1
*
                        TIMES( IPAR, ITYPE, IN, 11 ) = MAX( TIME-UNTIME,
     $                     ZERO ) / DBLE( IC )
                        OPCNTS( IPAR, ITYPE, IN, 11 ) = OPS / DBLE( IC )
                     END IF
*
*                    Time DTGEVC for Right Eigenvectors only,
*                    without back transforming
*
                     IF( TIMSUB( 12 ) ) THEN
                        IC = 0
                        OPS = ZERO
                        S1 = DSECND( )
  580                   CONTINUE
                        CALL DTGEVC( 'R', 'A', LLWORK, N, A, LDA, B,
     $                               LDA, H, LDA, T, LDA, N, ITEMP,
     $                               WORK, IINFO )
                        IF( IINFO.NE.0 ) THEN
                           WRITE( NOUT, FMT = 9997 )SUBNAM( 12 ),
     $                        IINFO, N, ITYPE, IPAR, IOLDSD
                           INFO = ABS( IINFO )
                           GO TO 920
                        END IF
                        S2 = DSECND( )
                        TIME = S2 - S1
                        IC = IC + 1
                        IF( TIME.LT.TIMMIN )
     $                     GO TO 580
*
                        TIMES( IPAR, ITYPE, IN, 12 ) = TIME / DBLE( IC )
                        OPCNTS( IPAR, ITYPE, IN, 12 ) = OPS / DBLE( IC )
                     END IF
*
*                    Time DTGEVC for Right Eigenvectors only,
*                    with back transforming
*
                     IF( TIMSUB( 13 ) ) THEN
                        IC = 0
                        OPS = ZERO
                        S1 = DSECND( )
  590                   CONTINUE
                        CALL DLACPY( 'Full', N, N, Q, LDQ, T, LDA )
                        CALL DTGEVC( 'R', 'B', LLWORK, N, A, LDA, B,
     $                               LDA, H, LDA, T, LDA, N, ITEMP,
     $                               WORK, IINFO )
                        IF( IINFO.NE.0 ) THEN
                           WRITE( NOUT, FMT = 9997 )SUBNAM( 13 ),
     $                        IINFO, N, ITYPE, IPAR, IOLDSD
                           INFO = ABS( IINFO )
                           GO TO 920
                        END IF
                        S2 = DSECND( )
                        TIME = S2 - S1
                        IC = IC + 1
                        IF( TIME.LT.TIMMIN )
     $                     GO TO 590
*
*                       Subtract the time used in DLACPY.
*
                        S1 = DSECND( )
                        DO 600 J = 1, IC
                           CALL DLACPY( 'Full', N, N, Q, LDQ, T, LDA )
  600                   CONTINUE
                        S2 = DSECND( )
                        UNTIME = S2 - S1
*
                        TIMES( IPAR, ITYPE, IN, 13 ) = MAX( TIME-UNTIME,
     $                     ZERO ) / DBLE( IC )
                        OPCNTS( IPAR, ITYPE, IN, 13 ) = OPS / DBLE( IC )
                     END IF
*
                  ELSE
*
*                    If this LDA has previously appeared, use the
*                    previously computed value(s).
*
                     IF( TIMSUB( 10 ) ) THEN
                        OPCNTS( IPAR, ITYPE, IN, 10 ) = OPCNTS( LASTL,
     $                     ITYPE, IN, 10 )
                        TIMES( IPAR, ITYPE, IN, 10 ) = TIMES( LASTL,
     $                     ITYPE, IN, 10 )
                     END IF
                     IF( TIMSUB( 11 ) ) THEN
                        OPCNTS( IPAR, ITYPE, IN, 11 ) = OPCNTS( LASTL,
     $                     ITYPE, IN, 11 )
                        TIMES( IPAR, ITYPE, IN, 11 ) = TIMES( LASTL,
     $                     ITYPE, IN, 11 )
                     END IF
                     IF( TIMSUB( 12 ) ) THEN
                        OPCNTS( IPAR, ITYPE, IN, 12 ) = OPCNTS( LASTL,
     $                     ITYPE, IN, 12 )
                        TIMES( IPAR, ITYPE, IN, 12 ) = TIMES( LASTL,
     $                     ITYPE, IN, 12 )
                     END IF
                     IF( TIMSUB( 13 ) ) THEN
                        OPCNTS( IPAR, ITYPE, IN, 13 ) = OPCNTS( LASTL,
     $                     ITYPE, IN, 13 )
                        TIMES( IPAR, ITYPE, IN, 13 ) = TIMES( LASTL,
     $                     ITYPE, IN, 13 )
                     END IF
                  END IF
  610          CONTINUE
            END IF
*
*           Time the EISPACK Routines
*
*           Restore random number seed
*
            DO 620 J = 1, 4
               ISEED( J ) = IOLDSD( J )
  620       CONTINUE
*
*           Re-generate A
*
            IF( ITYPE.LE.MAXTYP ) THEN
*
*              Generate A (w/o rotation)
*
               CALL DLATM4( KATYPE( ITYPE ), N, 3, 1, 2, ONE, ULP, ONE,
     $                      2, ISEED, A, N1 )
               IF( 3.LE.N )
     $            A( 3+2*N1 ) = ONE
*
*              Generate B (w/o rotation)
*
               CALL DLATM4( 8, N, 3, 1, 0, ONE, ONE, ONE, 2, ISEED, B,
     $                      N1 )
               IF( 2.LE.N )
     $            B( 2+N1 ) = ONE
*
               IF( N.GT.0 ) THEN
*
*                 Include rotations
*
*                 Generate U, V as Householder transformations times
*                 a diagonal matrix.
*
                  DO 640 JC = 1, N - 1
                     IC = ( JC-1 )*N1
                     DO 630 JR = JC, N
                        Q( JR+IC ) = DLARND( 3, ISEED )
                        Z( JR+IC ) = DLARND( 3, ISEED )
  630                CONTINUE
                     CALL DLARFG( N+1-JC, Q( JC+IC ), Q( JC+1+IC ), 1,
     $                            WORK( JC ) )
                     WORK( 2*N+JC ) = SIGN( ONE, Q( JC+IC ) )
                     Q( JC+IC ) = ONE
                     CALL DLARFG( N+1-JC, Z( JC+IC ), Z( JC+1+IC ), 1,
     $                            WORK( N+JC ) )
                     WORK( 3*N+JC ) = SIGN( ONE, Z( JC+IC ) )
                     Z( JC+IC ) = ONE
  640             CONTINUE
                  IC = ( N-1 )*N1
                  Q( N+IC ) = ONE
                  WORK( N ) = ZERO
                  WORK( 3*N ) = SIGN( ONE, DLARND( 2, ISEED ) )
                  Z( N+IC ) = ONE
                  WORK( 2*N ) = ZERO
                  WORK( 4*N ) = SIGN( ONE, DLARND( 2, ISEED ) )
*
*                 Apply the diagonal matrices
*
                  DO 660 JC = 1, N
                     DO 650 JR = 1, N
                        A( JR+IC ) = WORK( 2*N+JR )*WORK( 3*N+JC )*
     $                               A( JR+IC )
                        B( JR+IC ) = WORK( 2*N+JR )*WORK( 3*N+JC )*
     $                               B( JR+IC )
  650                CONTINUE
  660             CONTINUE
                  CALL DORM2R( 'L', 'N', N, N, N-1, Q, N1, WORK, A, N1,
     $                         WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 670
                  CALL DORM2R( 'R', 'T', N, N, N-1, Z, N1, WORK( N+1 ),
     $                         A, N1, WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 670
                  CALL DORM2R( 'L', 'N', N, N, N-1, Q, N1, WORK, B, N1,
     $                         WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 670
                  CALL DORM2R( 'R', 'T', N, N, N-1, Z, N1, WORK( N+1 ),
     $                         B, N1, WORK( 2*N+1 ), IINFO )
                  IF( IINFO.NE.0 )
     $               GO TO 670
               END IF
  670          CONTINUE
            END IF
*
*           Time QZHES w/ MATZ=.FALSE. for each LDAS(j)
*
            IF( TIMSUB( 14 ) ) THEN
               DO 710 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 14 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 14 ) = ZERO
                     GO TO 710
                  END IF
*
*                 If this value of LDA has come up before, just use
*                 the value previously computed.
*
                  LASTL = 0
                  DO 680 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  680             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time QZHES( ...,.FALSE.,..)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  690                CONTINUE
                     CALL DLACPY( 'Full', N, N, A, N1, H, LDA )
                     CALL DLACPY( 'Full', N, N, B, N1, T, LDA )
                     CALL QZHES( LDA, N, H, T, .FALSE., Q )
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 690
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 700 J = 1, IC
                        CALL DLACPY( 'Full', N, N, A, N1, Z, LDA )
                        CALL DLACPY( 'Full', N, N, B, N1, Z, LDA )
  700                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 14 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 14 ) = OPS / DBLE( IC )
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 14 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 14 )
                     TIMES( IPAR, ITYPE, IN, 14 ) = TIMES( LASTL, ITYPE,
     $                  IN, 14 )
                  END IF
                  LDH = LDA
  710          CONTINUE
            ELSE IF( RUNHES ) THEN
               CALL DLACPY( 'Full', N, N, A, N1, H, N1 )
               CALL DLACPY( 'Full', N, N, B, N1, T, N1 )
               CALL QZHES( N1, N, H, T, .FALSE., Q )
               LDH = N1
            END IF
*
*           Time QZHES w/ MATZ=.TRUE. for each LDAS(j)
*
            IF( TIMSUB( 15 ) ) THEN
               DO 750 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 15 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 15 ) = ZERO
                     GO TO 750
                  END IF
*
*                 If this value of LDA has come up before, just use
*                 the value previously computed.
*
                  LASTL = 0
                  DO 720 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  720             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time QZHES( ...,.TRUE.,..)
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  730                CONTINUE
                     CALL DLACPY( 'Full', N, N, A, N1, H, LDA )
                     CALL DLACPY( 'Full', N, N, B, N1, T, LDA )
                     CALL QZHES( LDA, N, H, T, .TRUE., Q )
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 730
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 740 J = 1, IC
                        CALL DLACPY( 'Full', N, N, A, N1, Z, LDA )
                        CALL DLACPY( 'Full', N, N, B, N1, Z, LDA )
  740                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 15 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 15 ) = OPS / DBLE( IC )
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 15 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 15 )
                     TIMES( IPAR, ITYPE, IN, 15 ) = TIMES( LASTL, ITYPE,
     $                  IN, 15 )
                  END IF
                  LDH = LDA
  750          CONTINUE
            END IF
*
*           Time QZIT and QZVAL w/ MATZ=.FALSE. for each LDAS(j)
*
            IF( TIMSUB( 16 ) ) THEN
               DO 790 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 16 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = ZERO
                     GO TO 790
                  END IF
*
*                 If this value of LDA has come up before, just use
*                 the value previously computed.
*
                  LASTL = 0
                  DO 760 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  760             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time QZIT and QZVAL with MATZ=.FALSE.
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  770                CONTINUE
                     CALL DLACPY( 'Full', N, N, H, LDH, A, LDA )
                     CALL DLACPY( 'Full', N, N, T, LDH, B, LDA )
                     CALL QZIT( LDA, N, A, B, ZERO, .FALSE., Q, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 16 ), IINFO,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
*
                     CALL QZVAL( LDA, N, A, B, W, W( LDA+1 ),
     $                           W( 2*LDA+1 ), .FALSE., Q )
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 770
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 780 J = 1, IC
                        CALL DLACPY( 'Full', N, N, H, LDH, Z, LDA )
                        CALL DLACPY( 'Full', N, N, T, LDH, Z, LDA )
  780                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 16 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = OPS / DBLE( IC )
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 16 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 16 )
                     TIMES( IPAR, ITYPE, IN, 16 ) = TIMES( LASTL, ITYPE,
     $                  IN, 16 )
                  END IF
                  LDS = 0
  790          CONTINUE
            END IF
*
*           Time QZIT and QZVAL w/ MATZ=.TRUE. for each LDAS(j)
*
            IF( TIMSUB( 17 ) ) THEN
               DO 830 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 17 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 17 ) = ZERO
                     GO TO 830
                  END IF
*
*                 If this value of LDA has come up before, just use
*                 the value previously computed.
*
                  LASTL = 0
                  DO 800 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  800             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Time QZIT and QZVAL with MATZ=.TRUE.
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  810                CONTINUE
                     CALL DLACPY( 'Full', N, N, H, LDH, A, LDA )
                     CALL DLACPY( 'Full', N, N, T, LDH, B, LDA )
                     CALL DLASET( 'Full', N, N, ZERO, ONE, Q, LDA )
                     CALL QZIT( LDA, N, A, B, ZERO, .TRUE., Q, IINFO )
                     IF( IINFO.NE.0 ) THEN
                        WRITE( NOUT, FMT = 9997 )SUBNAM( 17 ), IINFO,
     $                     N, ITYPE, IPAR, IOLDSD
                        INFO = ABS( IINFO )
                        GO TO 920
                     END IF
*
                     CALL QZVAL( LDA, N, A, B, W, W( LDA+1 ),
     $                           W( 2*LDA+1 ), .TRUE., Q )
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 810
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 820 J = 1, IC
                        CALL DLACPY( 'Full', N, N, H, LDH, Z, LDA )
                        CALL DLACPY( 'Full', N, N, T, LDH, Z, LDA )
                        CALL DLASET( 'Full', N, N, ZERO, ONE, Z, LDA )
  820                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 17 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 17 ) = OPS / DBLE( IC )
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 17 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 17 )
                     TIMES( IPAR, ITYPE, IN, 17 ) = TIMES( LASTL, ITYPE,
     $                  IN, 17 )
                  END IF
                  LDS = LDA
                  LDW = LDA
  830          CONTINUE
            ELSE IF( RUNQZ ) THEN
               CALL DLACPY( 'Full', N, N, H, LDH, A, N1 )
               CALL DLACPY( 'Full', N, N, T, LDH, B, N1 )
               CALL DLASET( 'Full', N, N, ZERO, ONE, Q, N1 )
               CALL QZIT( N1, N, A, B, ZERO, .TRUE., Q, IINFO )
               IF( IINFO.NE.0 ) THEN
                  WRITE( NOUT, FMT = 9997 )SUBNAM( 17 ), IINFO, N,
     $               ITYPE, IPAR, IOLDSD
                  INFO = ABS( IINFO )
                  GO TO 920
               END IF
*
               CALL QZVAL( N1, N, A, B, W, W( N1+1 ), W( 2*N1+1 ),
     $                     .TRUE., Q )
               LDS = N1
               LDW = N1
            END IF
*
*           Time QZVEC for each LDAS(j)
*
            IF( TIMSUB( 18 ) ) THEN
               DO 910 IPAR = 1, NPARMS
                  LDA = LDAS( IPAR )
                  IF( LDA.LT.N1 ) THEN
                     TIMES( IPAR, ITYPE, IN, 18 ) = ZERO
                     OPCNTS( IPAR, ITYPE, IN, 18 ) = ZERO
                     GO TO 910
                  END IF
*
*                 If this value of LDA has come up before, just use
*                 the value previously computed.
*
                  LASTL = 0
                  DO 840 J = 1, IPAR - 1
                     IF( LDA.EQ.LDAS( J ) )
     $                  LASTL = J
  840             CONTINUE
*
                  IF( LASTL.EQ.0 ) THEN
*
*                    Copy W if necessary to get right LDA.
*
                     IF( LDA.GT.LDW ) THEN
                        DO 860 JC = 3, 1, -1
                           DO 850 JR = N, 1, -1
                              W( JR+( JC-1 )*LDA ) = W( JR+( JC-1 )*
     $                           LDW )
  850                      CONTINUE
  860                   CONTINUE
                     ELSE IF( LDA.LT.LDW ) THEN
                        DO 880 JC = 1, 3
                           DO 870 JR = 1, N
                              W( JR+( JC-1 )*LDA ) = W( JR+( JC-1 )*
     $                           LDW )
  870                      CONTINUE
  880                   CONTINUE
                     END IF
                     LDW = LDA
*
*                    Time QZVEC
*
                     IC = 0
                     OPS = ZERO
                     S1 = DSECND( )
  890                CONTINUE
                     CALL DLACPY( 'Full', N, N, A, LDS, H, LDA )
                     CALL DLACPY( 'Full', N, N, B, LDS, T, LDA )
                     CALL DLACPY( 'Full', N, N, Q, LDS, Z, LDA )
                     CALL QZVEC( LDA, N, H, T, W, W( LDA+1 ),
     $                           W( 2*LDA+1 ), Z )
                     S2 = DSECND( )
                     TIME = S2 - S1
                     IC = IC + 1
                     IF( TIME.LT.TIMMIN )
     $                  GO TO 890
*
*                    Subtract the time used in DLACPY.
*
                     S1 = DSECND( )
                     DO 900 J = 1, IC
                        CALL DLACPY( 'Full', N, N, A, LDS, Z, LDA )
                        CALL DLACPY( 'Full', N, N, B, LDS, Z, LDA )
                        CALL DLACPY( 'Full', N, N, Q, LDS, Z, LDA )
  900                CONTINUE
                     S2 = DSECND( )
                     UNTIME = S2 - S1
*
                     TIMES( IPAR, ITYPE, IN, 18 ) = MAX( TIME-UNTIME,
     $                  ZERO ) / DBLE( IC )
                     OPCNTS( IPAR, ITYPE, IN, 18 ) = OPS / DBLE( IC )
                  ELSE
                     OPCNTS( IPAR, ITYPE, IN, 18 ) = OPCNTS( LASTL,
     $                  ITYPE, IN, 18 )
                     TIMES( IPAR, ITYPE, IN, 18 ) = TIMES( LASTL, ITYPE,
     $                  IN, 18 )
                  END IF
  910          CONTINUE
            END IF
*
  920    CONTINUE
  930 CONTINUE
*
*     Print a table of results for each timed routine.
*
      DO 940 ISUB = 1, NSUBS
         IF( TIMSUB( ISUB ) ) THEN
            CALL DPRTBG( SUBNAM( ISUB ), MTYPES, DOTYPE, NSIZES, NN,
     $                   INPARM( ISUB ), PNAMES, NPARMS, LDAS, NNB,
     $                   NSHFTS, NEISPS, MINNBS, MINBKS,
     $                   OPCNTS( 1, 1, 1, ISUB ), LDO1, LDO2,
     $                   TIMES( 1, 1, 1, ISUB ), LDT1, LDT2, WORK,
     $                   LLWORK, NOUT )
         END IF
  940 CONTINUE
*
      RETURN
*
*     End of DTIM51
*
 9997 FORMAT( ' DTIM51: ', A, ' returned INFO=', I6, '.', / 9X, 'N=',
     $      I6, ', ITYPE=', I6, ', IPAR=', I6, ', ISEED=(',
     $      3( I5, ',' ), I5, ')' )
*
      END
