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
