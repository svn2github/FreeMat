      REAL FUNCTION ERFCX(X)
C--------------------------------------------------------------------
C
C This subprogram computes approximate values for erfcx(x).
C   (see comments heading CALERF).
C
C   Author/date: W. J. Cody, January 8, 1985
C
C--------------------------------------------------------------------
      INTEGER JINT
      REAL X, RESULT
C------------------------------------------------------------------
      JINT = 2
      CALL CALERF(X,RESULT,JINT)
      ERFCX = RESULT
      RETURN
      END
