      DOUBLE PRECISION FUNCTION DERF(X)
C--------------------------------------------------------------------
C
C This subprogram computes approximate values for erf(x).
C   (see comments heading CALERF).
C
C   Author/date: W. J. Cody, January 8, 1985
C
C--------------------------------------------------------------------
      INTEGER JINT
      DOUBLE PRECISION X, RESULT
C------------------------------------------------------------------
      JINT = 0
      CALL DCALERF(X,RESULT,JINT)
      DERF = RESULT
      RETURN
C---------- Last card of DERF ----------
      END
      DOUBLE PRECISION FUNCTION DERFC(X)
C--------------------------------------------------------------------
C
C This subprogram computes approximate values for erfc(x).
C   (see comments heading CALERF).
C
C   Author/date: W. J. Cody, January 8, 1985
C
C--------------------------------------------------------------------
      INTEGER JINT
      DOUBLE PRECISION X, RESULT
C------------------------------------------------------------------
      JINT = 1
      CALL DCALERF(X,RESULT,JINT)
      DERFC = RESULT
      RETURN
      END
      DOUBLE PRECISION FUNCTION DERFCX(X)
C--------------------------------------------------------------------
C
C This subprogram computes approximate values for erfc(x).
C   (see comments heading CALERF).
C
C   Author/date: W. J. Cody, January 8, 1985
C
C--------------------------------------------------------------------
      INTEGER JINT
      DOUBLE PRECISION X, RESULT
C------------------------------------------------------------------
      JINT = 2
      CALL DCALERF(X,RESULT,JINT)
      DERFCX = RESULT
      RETURN
      END
