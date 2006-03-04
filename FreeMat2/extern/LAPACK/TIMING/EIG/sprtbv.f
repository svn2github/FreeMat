      SUBROUTINE SPRTBV( SUBNAM, NTYPES, DOTYPE, NSIZES, MM, NN, INPARM,
     $                   PNAMES, NPARMS, NP1, NP2, OPS, LDO1, LDO2,
     $                   TIMES, LDT1, LDT2, RWORK, LLWORK, NOUT )
*
*  -- LAPACK timing routine (version 3.0) --
*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
*     Courant Institute, Argonne National Lab, and Rice University
*     February 29, 1992 
*
*     .. Scalar Arguments ..
      CHARACTER*( * )    SUBNAM
      INTEGER            INPARM, LDO1, LDO2, LDT1, LDT2, NOUT, NPARMS,
     $                   NSIZES, NTYPES
*     ..
*     .. Array Arguments ..
      LOGICAL            DOTYPE( NTYPES ), LLWORK( NPARMS )
      CHARACTER*( * )    PNAMES( * )
      INTEGER            MM( NSIZES ), NN( NSIZES ), NP1( * ), NP2( * )
      REAL               OPS( LDO1, LDO2, * ), RWORK( * ),
     $                   TIMES( LDT1, LDT2, * )
*     ..
*
*  Purpose
*  =======
*
*     SPRTBV prints out timing information for the eigenvalue routines.
*     The table has NTYPES block rows and NSIZES columns, with NPARMS
*     individual rows in each block row.  There are INPARM quantities
*     which depend on rows (currently, INPARM <= 4).
*
*  Arguments (none are modified)
*  =========
*
*  SUBNAM - CHARACTER*(*)
*           The label for the output.
*
*  NTYPES - INTEGER
*           The number of values of DOTYPE, and also the
*           number of sets of rows of the table.
*
*  DOTYPE - LOGICAL array of dimension( NTYPES )
*           If DOTYPE(j) is .TRUE., then block row j (which includes
*           data from RESLTS( i, j, k ), for all i and k) will be
*           printed.  If DOTYPE(j) is .FALSE., then block row j will
*           not be printed.
*
*  NSIZES - INTEGER
*           The number of values of NN, and also the
*           number of columns of the table.
*
*  MM   -   INTEGER array of dimension( NSIZES )
*           The values of M used to label each column.
*
*  NN   -   INTEGER array of dimension( NSIZES )
*           The values of N used to label each column.
*
*  INPARM - INTEGER
*           The number of different parameters which are functions of
*           the row number.  At the moment, INPARM <= 4.
*
*  PNAMES - CHARACTER*(*) array of dimension( INPARM )
*           The label for the columns.
*
*  NPARMS - INTEGER
*           The number of values for each "parameter", i.e., the
*           number of rows for each value of DOTYPE.
*
*  NP1    - INTEGER array of dimension( NPARMS )
*           The first quantity which depends on row number.
*
*  NP2    - INTEGER array of dimension( NPARMS )
*           The second quantity which depends on row number.
*
*  OPS    - REAL array of dimension( LDT1, LDT2, NSIZES )
*           The operation counts.  The first index indicates the row,
*           the second index indicates the block row, and the last
*           indicates the column.
*
*  LDO1   - INTEGER
*           The first dimension of OPS.  It must be at least
*           min( 1, NPARMS ).
*
*  LDO2   - INTEGER
*           The second dimension of OPS.  It must be at least
*           min( 1, NTYPES ).
*
*  TIMES  - REAL array of dimension( LDT1, LDT2, NSIZES )
*           The times (in seconds).  The first index indicates the row,
*           the second index indicates the block row, and the last
*           indicates the column.
*
*  LDT1   - INTEGER
*           The first dimension of RESLTS.  It must be at least
*           min( 1, NPARMS ).
*
*  LDT2   - INTEGER
*           The second dimension of RESLTS.  It must be at least
*           min( 1, NTYPES ).
*
*  RWORK  - REAL array of dimension( NSIZES*NTYPES*NPARMS )
*           Real workspace.
*           Modified.
*
*  LLWORK - LOGICAL array of dimension( NPARMS )
*           Logical workspace.  It is used to turn on or off specific
*           lines in the output.  If LLWORK(i) is .TRUE., then row i
*           (which includes data from OPS(i,j,k) or TIMES(i,j,k) for
*           all j and k) will be printed.  If LLWORK(i) is
*           .FALSE., then row i will not be printed.
*           Modified.
*
*  NOUT   - INTEGER
*           The output unit number on which the table
*           is to be printed.  If NOUT <= 0, no output is printed.
*
*  =====================================================================
*
*     .. Local Scalars ..
      LOGICAL            LTEMP
      INTEGER            I, IINFO, ILINE, ILINES, IPAR, J, JP, JS, JT
*     ..
*     .. External Functions ..
      REAL               SMFLOP
      EXTERNAL           SMFLOP
*     ..
*     .. External Subroutines ..
      EXTERNAL           SPRTBR
*     ..
*     .. Executable Statements ..
*
*
*     First line
*
      WRITE( NOUT, FMT = 9999 )SUBNAM
*
*     Set up which lines are to be printed.
*
      LLWORK( 1 ) = .TRUE.
      ILINES = 1
      DO 20 IPAR = 2, NPARMS
         LLWORK( IPAR ) = .TRUE.
         DO 10 J = 1, IPAR - 1
            LTEMP = .FALSE.
            IF( INPARM.GE.1 .AND. NP1( J ).NE.NP1( IPAR ) )
     $         LTEMP = .TRUE.
            IF( INPARM.GE.2 .AND. NP2( J ).NE.NP2( IPAR ) )
     $         LTEMP = .TRUE.
            IF( .NOT.LTEMP )
     $         LLWORK( IPAR ) = .FALSE.
   10    CONTINUE
         IF( LLWORK( IPAR ) )
     $      ILINES = ILINES + 1
   20 CONTINUE
      IF( ILINES.EQ.1 ) THEN
         IF( INPARM.EQ.1 ) THEN
            WRITE( NOUT, FMT = 9995 )PNAMES( 1 ), NP1( 1 )
         ELSE IF( INPARM.EQ.2 ) THEN
            WRITE( NOUT, FMT = 9995 )PNAMES( 1 ), NP1( 1 ),
     $         PNAMES( 2 ), NP2( 1 )
         END IF
      ELSE
         ILINE = 0
         DO 30 J = 1, NPARMS
            IF( LLWORK( J ) ) THEN
               ILINE = ILINE + 1
               IF( INPARM.EQ.1 ) THEN
                  WRITE( NOUT, FMT = 9994 )ILINE, PNAMES( 1 ), NP1( J )
               ELSE IF( INPARM.EQ.2 ) THEN
                  WRITE( NOUT, FMT = 9994 )ILINE, PNAMES( 1 ),
     $               NP1( J ), PNAMES( 2 ), NP2( J )
               END IF
            END IF
   30    CONTINUE
      END IF
*
*     Execution Times
*
      WRITE( NOUT, FMT = 9996 )
      CALL SPRTBR( 'Type', 'M,N ', NTYPES, DOTYPE, NSIZES, MM, NN,
     $             NPARMS, LLWORK, TIMES, LDT1, LDT2, NOUT )
*
*     Operation Counts
*
      WRITE( NOUT, FMT = 9997 )
      CALL SPRTBR( 'Type', 'M,N ', NTYPES, DOTYPE, NSIZES, MM, NN,
     $             NPARMS, LLWORK, OPS, LDO1, LDO2, NOUT )
*
*     Megaflop Rates
*
      IINFO = 0
      DO 60 JS = 1, NSIZES
         DO 50 JT = 1, NTYPES
            IF( DOTYPE( JT ) ) THEN
               DO 40 JP = 1, NPARMS
                  I = JP + NPARMS*( JT-1+NTYPES*( JS-1 ) )
                  RWORK( I ) = SMFLOP( OPS( JP, JT, JS ),
     $                         TIMES( JP, JT, JS ), IINFO )
   40          CONTINUE
            END IF
   50    CONTINUE
   60 CONTINUE
*
      WRITE( NOUT, FMT = 9998 )
      CALL SPRTBR( 'Type', 'M,N ', NTYPES, DOTYPE, NSIZES, MM, NN,
     $             NPARMS, LLWORK, RWORK, NPARMS, NTYPES, NOUT )
*
 9999 FORMAT( / / / ' ****** Results for ', A, ' ******' )
 9998 FORMAT( / ' *** Speed in megaflops ***' )
 9997 FORMAT( / ' *** Number of floating-point operations ***' )
 9996 FORMAT( / ' *** Time in seconds ***' )
 9995 FORMAT( 5X, : 'with ', A, '=', I5, 3( : ', ', A, '=', I5 ) )
 9994 FORMAT( 5X, : 'line ', I2, ' with ', A, '=', I5,
     $      3( : ', ', A, '=', I5 ) )
      RETURN
*
*     End of SPRTBV
*
      END
