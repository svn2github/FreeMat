      PROGRAM DTIMEE
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     June 30, 1999
*
*  Purpose
*  =======
*
*  DTIMEE is the main timing program for the DOUBLE PRECISION matrix
*  eigenvalue routines in LAPACK.
*
*  There are four sets of routines that can be timed:
*
*  NEP (Nonsymmetric Eigenvalue Problem):
*      Includes DGEHRD, DHSEQR, DTREVC, and DHSEIN
*
*  SEP (Symmetric Eigenvalue Problem):
*      Includes DSYTRD, DORGTR, DORMTR, DSTEQR, DSTERF, DPTEQR, DSTEBZ,
*      DSTEIN, and DSTEDC
*
*  SVD (Singular Value Decomposition):
*      Includes DGEBRD, DBDSQR, DORGBR, DBDSDC and DGESDD
*
*  GEP (Generalized nonsymmetric Eigenvalue Problem):
*      Includes DGGHRD, DHGEQZ, and DTGEVC
*
*  Each test path has a different input file.  The first line of the
*  input file should contain the characters NEP, SEP, SVD, or GEP in
*  columns 1-3.  The number of remaining lines depends on what is found
*  on the first line.
*
*-----------------------------------------------------------------------
*
*  NEP input file:
*
*  line 2:  NN, INTEGER
*           Number of values of N.
*
*  line 3:  NVAL, INTEGER array, dimension (NN)
*           The values for the matrix dimension N.
*
*  line 4:  NPARM, INTEGER
*           Number of values of the parameters NB, NS, MAXB, and LDA.
*
*  line 5:  NBVAL, INTEGER array, dimension (NPARM)
*           The values for the blocksize NB.
*
*  line 6:  NSVAL, INTEGER array, dimension (NPARM)
*           The values for the number of shifts.
*
*  line 7:  MXBVAL, INTEGER array, dimension (NPARM)
*           The values for MAXB, used in determining whether multishift
*           will be used.
*
*  line 8:  LDAVAL, INTEGER array, dimension (NPARM)
*           The values for the leading dimension LDA.
*
*  line 9:  TIMMIN, DOUBLE PRECISION
*           The minimum time (in seconds) that a subroutine will be
*           timed.  If TIMMIN is zero, each routine should be timed only
*           once.
*
*  line 10: NTYPES, INTEGER
*           The number of matrix types to be used in the timing run.
*           If NTYPES >= MAXTYP, all the types are used.
*
*  If 0 < NTYPES < MAXTYP, then line 11 specifies NTYPES integer
*  values, which are the numbers of the matrix types to be used.
*
*  The remaining lines specify a path name and the specific routines to
*  be timed.  For the nonsymmetric eigenvalue problem, the path name is
*  'DHS'.  A line to request all the routines in this path has the form
*     DHS   T T T T T T T T T T T T
*  where the first 3 characters specify the path name, and up to MAXTYP
*  nonblank characters may appear in columns 4-80.  If the k-th such
*  character is 'T' or 't', the k-th routine will be timed.  If at least
*  one but fewer than 12 nonblank characters are specified, the
*  remaining routines will not be timed.  If columns 4-80 are blank, all
*  the routines will be timed, so the input line
*     DHS
*  is equivalent to the line above.
*
*-----------------------------------------------------------------------
*
*  SEP input file:
*
*  line 2:  NN, INTEGER
*           Number of values of N.
*
*  line 3:  NVAL, INTEGER array, dimension (NN)
*           The values for the matrix dimension N.
*
*  line 4:  NPARM, INTEGER
*           Number of values of the parameters NB and LDA.
*
*  line 5:  NBVAL, INTEGER array, dimension (NPARM)
*           The values for the blocksize NB.
*
*  line 6:  LDAVAL, INTEGER array, dimension (NPARM)
*           The values for the leading dimension LDA.
*
*  line 7:  TIMMIN, DOUBLE PRECISION
*           The minimum time (in seconds) that a subroutine will be
*           timed.  If TIMMIN is zero, each routine should be timed only
*           once.
*
*  line 8:  NTYPES, INTEGER
*           The number of matrix types to be used in the timing run.
*           If NTYPES >= MAXTYP, all the types are used.
*
*  If 0 < NTYPES < MAXTYP, then line 9 specifies NTYPES integer
*  values, which are the numbers of the matrix types to be used.
*
*  The remaining lines specify a path name and the specific routines to
*  be timed as for the NEP input file.  For the symmetric eigenvalue
*  problem, the path name is 'DST' and up to 8 routines may be timed.
*
*-----------------------------------------------------------------------
*
*  SVD input file:
*
*  line 2:  NN, INTEGER
*           Number of values of M and N.
*
*  line 3:  MVAL, INTEGER array, dimension (NN)
*           The values for the matrix dimension M.
*
*  line 4:  NVAL, INTEGER array, dimension (NN)
*           The values for the matrix dimension N.
*
*  line 5:  NPARM, INTEGER
*           Number of values of the parameters NB and LDA.
*
*  line 6:  NBVAL, INTEGER array, dimension (NPARM)
*           The values for the blocksize NB.
*
*  line 7:  LDAVAL, INTEGER array, dimension (NPARM)
*           The values for the leading dimension LDA.
*
*  line 8:  TIMMIN, DOUBLE PRECISION
*           The minimum time (in seconds) that a subroutine will be
*           timed.  If TIMMIN is zero, each routine should be timed only
*           once.
*
*  line 9:  NTYPES, INTEGER
*           The number of matrix types to be used in the timing run.
*           If NTYPES >= MAXTYP, all the types are used.
*
*  If 0 < NTYPES < MAXTYP, then line 10 specifies NTYPES integer
*  values, which are the numbers of the matrix types to be used.
*
*  The remaining lines specify a path name and the specific routines to
*  be timed as for the NEP input file.  For the singular value
*  decomposition the path name is 'DBD' and up to 16 routines may be
*  timed.
*
*-----------------------------------------------------------------------
*
*  GEP input file:
*
*  line 2:  NN, INTEGER
*           Number of values of N.
*
*  line 3:  NVAL, INTEGER array, dimension (NN)
*           The values for the matrix dimension N.
*
*  line 4:  NPARM, INTEGER
*           Number of values of the parameters NB, NS, MAXB, and LDA.
*
*  line 5:  NBVAL, INTEGER array, dimension (NPARM)
*           The values for the blocksize NB.
*
*  line 6:  NSVAL, INTEGER array, dimension (NPARM)
*           The values for the number of shifts.
*
*  line 7:  NEIVAL, INTEGER array, dimension (NPARM)
*           The values for NEISP, used in determining whether multishift
*           will be used.
*
*  line 8:  NBMVAL, INTEGER array, dimension (NPARM)
*           The values for MINNB, used in determining minimum blocksize.
*
*  line 9:  NBKVAL, INTEGER array, dimension (NPARM)
*           The values for MINBLK, also used in determining minimum
*           blocksize.
*
*  line 10: LDAVAL, INTEGER array, dimension (NPARM)
*           The values for the leading dimension LDA.
*
*  line 11: TIMMIN, DOUBLE PRECISION
*           The minimum time (in seconds) that a subroutine will be
*           timed.  If TIMMIN is zero, each routine should be timed only
*           once.
*
*  line 12: NTYPES, INTEGER
*           The number of matrix types to be used in the timing run.
*           If NTYPES >= MAXTYP, all the types are used.
*
*  If 0 < NTYPES < MAXTYP, then line 13 specifies NTYPES integer
*  values, which are the numbers of the matrix types to be used.
*
*  The remaining lines specify a path name and the specific routines to
*  be timed.  For the nonsymmetric eigenvalue problem, the path name is
*  'DHG'.  A line to request all the routines in this path has the form
*     DHG   T T T T T T T T T T T T T T T T T T
*  where the first 3 characters specify the path name, and up to MAXTYP
*  nonblank characters may appear in columns 4-80.  If the k-th such
*  character is 'T' or 't', the k-th routine will be timed.  If at least
*  one but fewer than 18 nonblank characters are specified, the
*  remaining routines will not be timed.  If columns 4-80 are blank, all
*  the routines will be timed, so the input line
*     DHG
*  is equivalent to the line above.
*
*=======================================================================
*
*  The workspace requirements in terms of square matrices for the
*  different test paths are as follows:
*
*  NEP:   3 N**2 + N*(3*NB+2)
*  SEP:   2 N**2 + N*(2*N) + N
*  SVD:   4 N**2 + MAX( 6*N, MAXIN*MAXPRM*MAXT )
*  GEP:   6 N**2 + 3*N
*
*  MAXN is currently set to 400,
*  LG2MXN = ceiling of log-base-2 of MAXN = 9, and LDAMAX = 420.
*  The real work space needed is LWORK = MAX( MAXN*(4*MAXN+2),
*       2*LDAMAX+1+3*MAXN+2*MAXN*LG2MXN+3*MAXN**2 ),  and the integer
*  workspace needed is  LIWRK2 = 6 + 6*MAXN + 5*MAXN*LG2MXN.
*  For SVD, we assume NRHS may be as big
*  as N.  The parameter NEED is set to 4 to allow for 4 NxN matrices
*  for SVD.
*
*     .. Parameters ..
      INTEGER            MAXN, LDAMAX, LG2MXN
      PARAMETER          ( MAXN = 400, LDAMAX = 420, LG2MXN = 9 )
      INTEGER            NEED
      PARAMETER          ( NEED = 6 )
      INTEGER            LIWRK2
      PARAMETER          ( LIWRK2 = 6+6*MAXN+5*MAXN*LG2MXN )
      INTEGER            LWORK
      PARAMETER          ( LWORK = 2*LDAMAX+1+3*MAXN+2*MAXN*LG2MXN+
     $                   4*MAXN**2 )
      INTEGER            MAXIN, MAXPRM, MAXT, MAXSUB
      PARAMETER          ( MAXIN = 12, MAXPRM = 10, MAXT = 10,
     $                   MAXSUB = 25 )
      INTEGER            NIN, NOUT
      PARAMETER          ( NIN = 5, NOUT = 6 )
*     ..
*     .. Local Scalars ..
      LOGICAL            FATAL, GEP, NEP, SEP, SVD
      CHARACTER*3        C3, PATH
      CHARACTER*6        VNAME
      CHARACTER*80       LINE
      INTEGER            I, INFO, MAXTYP, NN, NPARMS, NTYPES
      DOUBLE PRECISION   S1, S2, TIMMIN
*     ..
*     .. Local Arrays ..
      LOGICAL            DOTYPE( MAXT ), LOGWRK( MAXN )
      INTEGER            ISEED( 4 ), IWORK( MAXT ), IWORK2( LIWRK2 ),
     $                   LDAVAL( MAXPRM ), MVAL( MAXIN ),
     $                   MXBVAL( MAXPRM ), MXTYPE( 4 ),
     $                   NBKVAL( MAXPRM ), NBMVAL( MAXPRM ),
     $                   NBVAL( MAXPRM ), NSVAL( MAXPRM ), NVAL( MAXIN )
      DOUBLE PRECISION   A( LDAMAX*MAXN, NEED ), D( MAXN, 4 ),
     $                   OPCNTS( MAXPRM, MAXT, MAXIN, MAXSUB ),
     $                   RESULT( MAXPRM, MAXT, MAXIN, MAXSUB ),
     $                   WORK( LWORK )
*     ..
*     .. External Functions ..
      LOGICAL            LSAMEN
      DOUBLE PRECISION   DSECND
      EXTERNAL           LSAMEN, DSECND
*     ..
*     .. External Subroutines ..
      EXTERNAL           DTIM21, DTIM22, DTIM26, DTIM51
*     ..
*     .. Scalars in Common ..
      DOUBLE PRECISION   ITCNT, OPS
*     ..
*     .. Arrays in Common ..
      INTEGER            IPARMS( 100 )
*     ..
*     .. Common blocks ..
      COMMON             / LATIME / OPS, ITCNT
      COMMON             / CLAENV / IPARMS
*     ..
*     .. Save statement ..
      SAVE               / CLAENV /
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC          MAX
*     ..
*     .. Data statements ..
      DATA               ISEED / 0, 0, 0, 1 /
      DATA               MXTYPE / 8, 4, 5, 4 /
*     ..
*     .. Executable Statements ..
*
      S1 = DSECND( )
      FATAL = .FALSE.
      NEP = .FALSE.
      SEP = .FALSE.
      SVD = .FALSE.
      GEP = .FALSE.
*
*     Read the 3-character test path
*
      READ( NIN, FMT = '(A3)', END = 160 )PATH
      NEP = LSAMEN( 3, PATH, 'NEP' ) .OR. LSAMEN( 3, PATH, 'DHS' )
      SEP = LSAMEN( 3, PATH, 'SEP' ) .OR. LSAMEN( 3, PATH, 'DST' )
      SVD = LSAMEN( 3, PATH, 'SVD' ) .OR. LSAMEN( 3, PATH, 'DBD' )
      GEP = LSAMEN( 3, PATH, 'GEP' ) .OR. LSAMEN( 3, PATH, 'DHG' )
*
*     Report values of parameters as they are read.
*
      IF( NEP ) THEN
         WRITE( NOUT, FMT = 9993 )
      ELSE IF( SEP ) THEN
         WRITE( NOUT, FMT = 9992 )
      ELSE IF( SVD ) THEN
         WRITE( NOUT, FMT = 9991 )
      ELSE IF( GEP ) THEN
         WRITE( NOUT, FMT = 9990 )
      ELSE
         WRITE( NOUT, FMT = 9996 )PATH
         STOP
      END IF
      WRITE( NOUT, FMT = 9985 )
      WRITE( NOUT, FMT = 9989 )
*
*     Read the number of values of M and N.
*
      READ( NIN, FMT = * )NN
      IF( NN.LT.1 ) THEN
         WRITE( NOUT, FMT = 9995 )'NN  ', NN, 1
         NN = 0
         FATAL = .TRUE.
      ELSE IF( NN.GT.MAXIN ) THEN
         WRITE( NOUT, FMT = 9994 )'NN  ', NN, MAXIN
         NN = 0
         FATAL = .TRUE.
      END IF
*
*     Read the values of M
*
      READ( NIN, FMT = * )( MVAL( I ), I = 1, NN )
      IF( SVD ) THEN
         VNAME = '  M'
      ELSE
         VNAME = '  N'
      END IF
      DO 10 I = 1, NN
         IF( MVAL( I ).LT.0 ) THEN
            WRITE( NOUT, FMT = 9995 )VNAME, MVAL( I ), 0
            FATAL = .TRUE.
         ELSE IF( MVAL( I ).GT.MAXN ) THEN
            WRITE( NOUT, FMT = 9994 )VNAME, MVAL( I ), MAXN
            FATAL = .TRUE.
         END IF
   10 CONTINUE
*
*     Read the values of N
*
      IF( SVD ) THEN
         WRITE( NOUT, FMT = 9988 )'M   ', ( MVAL( I ), I = 1, NN )
         READ( NIN, FMT = * )( NVAL( I ), I = 1, NN )
         DO 20 I = 1, NN
            IF( NVAL( I ).LT.0 ) THEN
               WRITE( NOUT, FMT = 9995 )'N   ', NVAL( I ), 0
               FATAL = .TRUE.
            ELSE IF( NVAL( I ).GT.MAXN ) THEN
               WRITE( NOUT, FMT = 9994 )'N   ', NVAL( I ), MAXN
               FATAL = .TRUE.
            END IF
   20    CONTINUE
      ELSE
         DO 30 I = 1, NN
            NVAL( I ) = MVAL( I )
   30    CONTINUE
      END IF
      WRITE( NOUT, FMT = 9988 )'N   ', ( NVAL( I ), I = 1, NN )
*
*     Read the number of parameter values.
*
      READ( NIN, FMT = * )NPARMS
      IF( NPARMS.LT.1 ) THEN
         WRITE( NOUT, FMT = 9995 )'NPARMS', NPARMS, 1
         NPARMS = 0
         FATAL = .TRUE.
      ELSE IF( NPARMS.GT.MAXIN ) THEN
         WRITE( NOUT, FMT = 9994 )'NPARMS', NPARMS, MAXIN
         NPARMS = 0
         FATAL = .TRUE.
      END IF
*
*     Read the values of NB
*
      READ( NIN, FMT = * )( NBVAL( I ), I = 1, NPARMS )
      DO 40 I = 1, NPARMS
         IF( NBVAL( I ).LT.0 ) THEN
            WRITE( NOUT, FMT = 9995 )'NB  ', NBVAL( I ), 0
            FATAL = .TRUE.
         END IF
   40 CONTINUE
      WRITE( NOUT, FMT = 9988 )'NB  ', ( NBVAL( I ), I = 1, NPARMS )
*
      IF( NEP .OR. GEP ) THEN
*
*        Read the values of NSHIFT
*
         READ( NIN, FMT = * )( NSVAL( I ), I = 1, NPARMS )
         DO 50 I = 1, NPARMS
            IF( NSVAL( I ).LT.0 ) THEN
               WRITE( NOUT, FMT = 9995 )'NS  ', NSVAL( I ), 0
               FATAL = .TRUE.
            END IF
   50    CONTINUE
         WRITE( NOUT, FMT = 9988 )'NS  ', ( NSVAL( I ), I = 1, NPARMS )
*
*        Read the values of MAXB
*
         READ( NIN, FMT = * )( MXBVAL( I ), I = 1, NPARMS )
         DO 60 I = 1, NPARMS
            IF( MXBVAL( I ).LT.0 ) THEN
               WRITE( NOUT, FMT = 9995 )'MAXB', MXBVAL( I ), 0
               FATAL = .TRUE.
            END IF
   60    CONTINUE
         WRITE( NOUT, FMT = 9988 )'MAXB',
     $      ( MXBVAL( I ), I = 1, NPARMS )
      ELSE
         DO 70 I = 1, NPARMS
            NSVAL( I ) = 1
            MXBVAL( I ) = 1
   70    CONTINUE
      END IF
*
      IF( GEP ) THEN
*
*        Read the values of NBMIN
*
         READ( NIN, FMT = * )( NBMVAL( I ), I = 1, NPARMS )
         DO 80 I = 1, NPARMS
            IF( NBMVAL( I ).LT.0 ) THEN
               WRITE( NOUT, FMT = 9995 )'NBMIN', NBMVAL( I ), 0
               FATAL = .TRUE.
            END IF
   80    CONTINUE
         WRITE( NOUT, FMT = 9988 )'NBMIN',
     $      ( NBMVAL( I ), I = 1, NPARMS )
*
*        Read the values of MINBLK
*
         READ( NIN, FMT = * )( NBKVAL( I ), I = 1, NPARMS )
         DO 90 I = 1, NPARMS
            IF( NBKVAL( I ).LT.0 ) THEN
               WRITE( NOUT, FMT = 9995 )'MINBLK', NBKVAL( I ), 0
               FATAL = .TRUE.
            END IF
   90    CONTINUE
         WRITE( NOUT, FMT = 9988 )'MINBLK',
     $      ( NBKVAL( I ), I = 1, NPARMS )
      ELSE
         DO 100 I = 1, NPARMS
            NBMVAL( I ) = MAXN + 1
            NBKVAL( I ) = MAXN + 1
  100    CONTINUE
      END IF
*
*     Read the values of LDA
*
      READ( NIN, FMT = * )( LDAVAL( I ), I = 1, NPARMS )
      DO 110 I = 1, NPARMS
         IF( LDAVAL( I ).LT.0 ) THEN
            WRITE( NOUT, FMT = 9995 )'LDA ', LDAVAL( I ), 0
            FATAL = .TRUE.
         ELSE IF( LDAVAL( I ).GT.LDAMAX ) THEN
            WRITE( NOUT, FMT = 9994 )'LDA ', LDAVAL( I ), LDAMAX
            FATAL = .TRUE.
         END IF
  110 CONTINUE
      WRITE( NOUT, FMT = 9988 )'LDA ', ( LDAVAL( I ), I = 1, NPARMS )
*
*     Read the minimum time a subroutine will be timed.
*
      READ( NIN, FMT = * )TIMMIN
      WRITE( NOUT, FMT = 9987 )TIMMIN
*
*     Read the number of matrix types to use in timing.
*
      READ( NIN, FMT = * )NTYPES
      IF( NTYPES.LT.0 ) THEN
         WRITE( NOUT, FMT = 9995 )'NTYPES', NTYPES, 0
         FATAL = .TRUE.
         NTYPES = 0
      END IF
*
*     Read the matrix types.
*
      IF( NEP ) THEN
         MAXTYP = MXTYPE( 1 )
      ELSE IF( SEP ) THEN
         MAXTYP = MXTYPE( 2 )
      ELSE IF( SVD ) THEN
         MAXTYP = MXTYPE( 3 )
      ELSE
         MAXTYP = MXTYPE( 4 )
      END IF
      IF( NTYPES.LT.MAXTYP ) THEN
         READ( NIN, FMT = * )( IWORK( I ), I = 1, NTYPES )
         DO 120 I = 1, MAXTYP
            DOTYPE( I ) = .FALSE.
  120    CONTINUE
         DO 130 I = 1, NTYPES
            IF( IWORK( I ).LT.0 ) THEN
               WRITE( NOUT, FMT = 9995 )'TYPE', IWORK( I ), 0
               FATAL = .TRUE.
            ELSE IF( IWORK( I ).GT.MAXTYP ) THEN
               WRITE( NOUT, FMT = 9994 )'TYPE', IWORK( I ), MAXTYP
               FATAL = .TRUE.
            ELSE
               DOTYPE( IWORK( I ) ) = .TRUE.
            END IF
  130    CONTINUE
      ELSE
         NTYPES = MAXTYP
         DO 140 I = 1, MAXT
            DOTYPE( I ) = .TRUE.
  140    CONTINUE
      END IF
*
      IF( FATAL ) THEN
         WRITE( NOUT, FMT = 9999 )
 9999    FORMAT( / ' Execution not attempted due to input errors' )
         STOP
      END IF
*
*     Read the input lines indicating the test path and the routines
*     to be timed.  The first three characters indicate the test path.
*
  150 CONTINUE
      READ( NIN, FMT = '(A80)', END = 160 )LINE
      C3 = LINE( 1: 3 )
*
*     -------------------------------------
*     NEP:  Nonsymmetric Eigenvalue Problem
*     -------------------------------------
*
      IF( LSAMEN( 3, C3, 'DHS' ) .OR. LSAMEN( 3, C3, 'NEP' ) ) THEN
         CALL DTIM21( LINE, NN, NVAL, MAXTYP, DOTYPE, NPARMS, NBVAL,
     $                NSVAL, MXBVAL, LDAVAL, TIMMIN, NOUT, ISEED,
     $                A( 1, 1 ), A( 1, 2 ), A( 1, 3 ), D( 1, 1 ), WORK,
     $                LWORK, LOGWRK, IWORK2, RESULT, MAXPRM, MAXT,
     $                MAXIN, OPCNTS, MAXPRM, MAXT, MAXIN, INFO )
         IF( INFO.NE.0 )
     $      WRITE( NOUT, FMT = 9986 )'DTIM21', INFO
*
*     ----------------------------------
*     SEP:  Symmetric Eigenvalue Problem
*     ----------------------------------
*
      ELSE IF( LSAMEN( 3, C3, 'DST' ) .OR. LSAMEN( 3, C3, 'SEP' ) ) THEN
         CALL DTIM22( LINE, NN, NVAL, MAXTYP, DOTYPE, NPARMS, NBVAL,
     $                LDAVAL, TIMMIN, NOUT, ISEED, A( 1, 1 ), D( 1, 1 ),
     $                D( 1, 2 ), D( 1, 3 ), A( 1, 2 ), A( 1, 3 ), WORK,
     $                LWORK, LOGWRK, IWORK2, RESULT, MAXPRM, MAXT,
     $                MAXIN, OPCNTS, MAXPRM, MAXT, MAXIN, INFO )
         IF( INFO.NE.0 )
     $      WRITE( NOUT, FMT = 9986 )'DTIM22', INFO
*
*     ----------------------------------
*     SVD:  Singular Value Decomposition
*     ----------------------------------
*
      ELSE IF( LSAMEN( 3, C3, 'DBD' ) .OR. LSAMEN( 3, C3, 'SVD' ) ) THEN
         CALL DTIM26( LINE, NN, NVAL, MVAL, MAXTYP, DOTYPE, NPARMS,
     $                NBVAL, LDAVAL, TIMMIN, NOUT, ISEED, A( 1, 1 ),
     $                A( 1, 2 ), A( 1, 3 ), A( 1, 4 ), D( 1, 1 ),
     $                D( 1, 2 ), D( 1, 3 ), D( 1, 4 ), WORK, LWORK,
     $                IWORK2, LOGWRK, RESULT, MAXPRM, MAXT, MAXIN,
     $                OPCNTS, MAXPRM, MAXT, MAXIN, INFO )
         IF( INFO.NE.0 )
     $      WRITE( NOUT, FMT = 9986 )'DTIM26', INFO
*
*     -------------------------------------------------
*     GEP:  Generalized Nonsymmetric Eigenvalue Problem
*     -------------------------------------------------
*
      ELSE IF( LSAMEN( 3, C3, 'DHG' ) .OR. LSAMEN( 3, C3, 'GEP' ) ) THEN
         CALL DTIM51( LINE, NN, NVAL, MAXTYP, DOTYPE, NPARMS, NBVAL,
     $                NSVAL, MXBVAL, NBMVAL, NBKVAL, LDAVAL, TIMMIN,
     $                NOUT, ISEED, A( 1, 1 ), A( 1, 2 ), A( 1, 3 ),
     $                A( 1, 4 ), A( 1, 5 ), A( 1, 6 ), D( 1, 1 ), WORK,
     $                LWORK, LOGWRK, RESULT, MAXPRM, MAXT, MAXIN,
     $                OPCNTS, MAXPRM, MAXT, MAXIN, INFO )
         IF( INFO.NE.0 )
     $      WRITE( NOUT, FMT = 9986 )'DTIM51', INFO
      ELSE
         WRITE( NOUT, FMT = * )
         WRITE( NOUT, FMT = * )
         WRITE( NOUT, FMT = 9996 )C3
      END IF
      GO TO 150
  160 CONTINUE
      WRITE( NOUT, FMT = 9998 )
 9998 FORMAT( / / ' End of timing run' )
      S2 = DSECND( )
      WRITE( NOUT, FMT = 9997 )S2 - S1
*
 9997 FORMAT( ' Total time used = ', F12.2, ' seconds', / )
 9996 FORMAT( 1X, A3, ':  Unrecognized path name' )
 9995 FORMAT( ' *** Invalid input value: ', A6, '=', I6, '; must be >=',
     $      I6 )
 9994 FORMAT( ' *** Invalid input value: ', A6, '=', I6, '; must be <=',
     $      I6 )
 9993 FORMAT( ' Timing the Nonsymmetric Eigenvalue Problem routines',
     $      / '    DGEHRD, DHSEQR, DTREVC, and DHSEIN' )
 9992 FORMAT( ' Timing the Symmetric Eigenvalue Problem routines',
     $      / '    DSYTRD, DSTEQR, and DSTERF' )
 9991 FORMAT( ' Timing the Singular Value Decomposition routines',
     $      / '    DGEBRD, DBDSQR, DORGBR, DBDSDC and DGESDD' )
 9990 FORMAT( ' Timing the Generalized Eigenvalue Problem routines',
     $      / '    DGGHRD, DHGEQZ, and DTGEVC ' )
 9989 FORMAT( / ' The following parameter values will be used:' )
 9988 FORMAT( '    Values of ', A5, ':  ', 10I6, / 19X, 10I6 )
 9987 FORMAT( / ' Minimum time a subroutine will be timed = ', F8.2,
     $      ' seconds', / )
 9986 FORMAT( ' *** Error code from ', A6, ' = ', I4 )
 9985 FORMAT( / ' LAPACK VERSION 3.0, released June 30, 1999 ' )
*
*     End of DTIMEE
*
      END
