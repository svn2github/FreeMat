/* ../erf.f -- translated by f2c (version 20031025).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Subroutine */ int calerf_(real *arg, real *result, integer *jint)
{
    /* Initialized data */

    static real four = 4.f;
    static real xneg = -9.382f;
    static real xsmall = 5.96e-8f;
    static real xbig = 9.194f;
    static real xhuge = 2900.f;
    static real xmax = 4.79e37f;
    static real a[5] = { 3.1611237438705656f,113.864154151050156f,
	    377.485237685302021f,3209.37758913846947f,.185777706184603153f };
    static real b[4] = { 23.6012909523441209f,244.024637934444173f,
	    1282.61652607737228f,2844.23683343917062f };
    static real c__[9] = { .564188496988670089f,8.88314979438837594f,
	    66.1191906371416295f,298.635138197400131f,881.95222124176909f,
	    1712.04761263407058f,2051.07837782607147f,1230.33935479799725f,
	    2.15311535474403846e-8f };
    static real d__[8] = { 15.7449261107098347f,117.693950891312499f,
	    537.181101862009858f,1621.38957456669019f,3290.79923573345963f,
	    4362.61909014324716f,3439.36767414372164f,1230.33935480374942f };
    static real p[6] = { .305326634961232344f,.360344899949804439f,
	    .125781726111229246f,.0160837851487422766f,
	    6.58749161529837803e-4f,.0163153871373020978f };
    static real one = 1.f;
    static real q[5] = { 2.56852019228982242f,1.87295284992346047f,
	    .527905102951428412f,.0605183413124413191f,.00233520497626869185f 
	    };
    static real half = .5f;
    static real two = 2.f;
    static real zero = 0.f;
    static real sqrpi = .56418958354775628695f;
    static real thresh = .46875f;
    static real sixten = 16.f;
    static real xinf = 3.4e38f;

    /* System generated locals */
    real r__1;

    /* Builtin functions */
    double exp(doublereal), r_int(real *);

    /* Local variables */
    static integer i__;
    static real x, y, del, ysq, xden, xnum;

/* ------------------------------------------------------------------ */

/* This packet evaluates  erf(x),  erfc(x),  and  exp(x*x)*erfc(x) */
/*   for a real argument  x.  It contains three FUNCTION type */
/*   subprograms: ERF, ERFC, and ERFCX (or DERF, DERFC, and DERFCX), */
/*   and one SUBROUTINE type subprogram, CALERF.  The calling */
/*   statements for the primary entries are: */

/*                   Y=ERF(X)     (or   Y=DERF(X)), */

/*                   Y=ERFC(X)    (or   Y=DERFC(X)), */
/*   and */
/*                   Y=ERFCX(X)   (or   Y=DERFCX(X)). */

/*   The routine  CALERF  is intended for internal packet use only, */
/*   all computations within the packet being concentrated in this */
/*   routine.  The function subprograms invoke  CALERF  with the */
/*   statement */

/*          CALL CALERF(ARG,RESULT,JINT) */

/*   where the parameter usage is as follows */

/*      Function                     Parameters for CALERF */
/*       call              ARG                  Result          JINT */

/*     ERF(ARG)      ANY REAL ARGUMENT         ERF(ARG)          0 */
/*     ERFC(ARG)     ABS(ARG) .LT. XBIG        ERFC(ARG)         1 */
/*     ERFCX(ARG)    XNEG .LT. ARG .LT. XMAX   ERFCX(ARG)        2 */

/*   The main computation evaluates near-minimax approximations */
/*   from "Rational Chebyshev approximations for the error function" */
/*   by W. J. Cody, Math. Comp., 1969, PP. 631-638.  This */
/*   transportable program uses rational functions that theoretically */
/*   approximate  erf(x)  and  erfc(x)  to at least 18 significant */
/*   decimal digits.  The accuracy achieved depends on the arithmetic */
/*   system, the compiler, the intrinsic functions, and proper */
/*   selection of the machine-dependent constants. */

/* ******************************************************************* */
/* ******************************************************************* */

/* Explanation of machine-dependent constants */

/*   XMIN   = the smallest positive floating-point number. */
/*   XINF   = the largest positive finite floating-point number. */
/*   XNEG   = the largest negative argument acceptable to ERFCX; */
/*            the negative of the solution to the equation */
/*            2*exp(x*x) = XINF. */
/*   XSMALL = argument below which erf(x) may be represented by */
/*            2*x/sqrt(pi)  and above which  x*x  will not underflow. */
/*            A conservative value is the largest machine number X */
/*            such that   1.0 + X = 1.0   to machine precision. */
/*   XBIG   = largest argument acceptable to ERFC;  solution to */
/*            the equation:  W(x) * (1-0.5/x**2) = XMIN,  where */
/*            W(x) = exp(-x*x)/[x*sqrt(pi)]. */
/*   XHUGE  = argument above which  1.0 - 1/(2*x*x) = 1.0  to */
/*            machine precision.  A conservative value is */
/*            1/[2*sqrt(XSMALL)] */
/*   XMAX   = largest acceptable argument to ERFCX; the minimum */
/*            of XINF and 1/[sqrt(pi)*XMIN]. */

/*   Approximate values for some important machines are: */

/*                          XMIN       XINF        XNEG     XSMALL */

/*  CDC 7600      (S.P.)  3.13E-294   1.26E+322   -27.220  7.11E-15 */
/*  CRAY-1        (S.P.)  4.58E-2467  5.45E+2465  -75.345  7.11E-15 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)  1.18E-38    3.40E+38     -9.382  5.96E-8 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)  2.23D-308   1.79D+308   -26.628  1.11D-16 */
/*  IBM 195       (D.P.)  5.40D-79    7.23E+75    -13.190  1.39D-17 */
/*  UNIVAC 1108   (D.P.)  2.78D-309   8.98D+307   -26.615  1.73D-18 */
/*  VAX D-Format  (D.P.)  2.94D-39    1.70D+38     -9.345  1.39D-17 */
/*  VAX G-Format  (D.P.)  5.56D-309   8.98D+307   -26.615  1.11D-16 */


/*                          XBIG       XHUGE       XMAX */

/*  CDC 7600      (S.P.)  25.922      8.39E+6     1.80X+293 */
/*  CRAY-1        (S.P.)  75.326      8.39E+6     5.45E+2465 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)   9.194      2.90E+3     4.79E+37 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)  26.543      6.71D+7     2.53D+307 */
/*  IBM 195       (D.P.)  13.306      1.90D+8     7.23E+75 */
/*  UNIVAC 1108   (D.P.)  26.582      5.37D+8     8.98D+307 */
/*  VAX D-Format  (D.P.)   9.269      1.90D+8     1.70D+38 */
/*  VAX G-Format  (D.P.)  26.569      6.71D+7     8.98D+307 */

/* ******************************************************************* */
/* ******************************************************************* */

/* Error returns */

/*  The program returns  ERFC = 0      for  ARG .GE. XBIG; */

/*                       ERFCX = XINF  for  ARG .LT. XNEG; */
/*      and */
/*                       ERFCX = 0     for  ARG .GE. XMAX. */


/* Intrinsic functions required are: */

/*     ABS, AINT, EXP */


/*  Author: W. J. Cody */
/*          Mathematics and Computer Science Division */
/*          Argonne National Laboratory */
/*          Argonne, IL 60439 */

/*  Latest modification: March 19, 1990 */

/* ------------------------------------------------------------------ */
/* ------------------------------------------------------------------ */
/*  Mathematical constants */
/* ------------------------------------------------------------------ */
/* ------------------------------------------------------------------ */
/*  Machine-dependent constants */
/* ------------------------------------------------------------------ */
/* ------------------------------------------------------------------ */
/*  Coefficients for approximation to  erf  in first interval */
/* ------------------------------------------------------------------ */
/* ------------------------------------------------------------------ */
/*  Coefficients for approximation to  erfc  in second interval */
/* ------------------------------------------------------------------ */
/* ------------------------------------------------------------------ */
/*  Coefficients for approximation to  erfc  in third interval */
/* ------------------------------------------------------------------ */
/* ------------------------------------------------------------------ */
    x = *arg;
    y = dabs(x);
    if (y <= thresh) {
/* ------------------------------------------------------------------ */
/*  Evaluate  erf  for  |X| <= 0.46875 */
/* ------------------------------------------------------------------ */
	ysq = zero;
	if (y > xsmall) {
	    ysq = y * y;
	}
	xnum = a[4] * ysq;
	xden = ysq;
	for (i__ = 1; i__ <= 3; ++i__) {
	    xnum = (xnum + a[i__ - 1]) * ysq;
	    xden = (xden + b[i__ - 1]) * ysq;
/* L20: */
	}
	*result = x * (xnum + a[3]) / (xden + b[3]);
	if (*jint != 0) {
	    *result = one - *result;
	}
	if (*jint == 2) {
	    *result = exp(ysq) * *result;
	}
	goto L800;
/* ------------------------------------------------------------------ */
/*  Evaluate  erfc  for 0.46875 <= |X| <= 4.0 */
/* ------------------------------------------------------------------ */
    } else if (y <= four) {
	xnum = c__[8] * y;
	xden = y;
	for (i__ = 1; i__ <= 7; ++i__) {
	    xnum = (xnum + c__[i__ - 1]) * y;
	    xden = (xden + d__[i__ - 1]) * y;
/* L120: */
	}
	*result = (xnum + c__[7]) / (xden + d__[7]);
	if (*jint != 2) {
	    r__1 = y * sixten;
	    ysq = r_int(&r__1) / sixten;
	    del = (y - ysq) * (y + ysq);
	    *result = exp(-ysq * ysq) * exp(-del) * *result;
	}
/* ------------------------------------------------------------------ */
/*  Evaluate  erfc  for |X| > 4.0 */
/* ------------------------------------------------------------------ */
    } else {
	*result = zero;
	if (y >= xbig) {
	    if (*jint != 2 || y >= xmax) {
		goto L300;
	    }
	    if (y >= xhuge) {
		*result = sqrpi / y;
		goto L300;
	    }
	}
	ysq = one / (y * y);
	xnum = p[5] * ysq;
	xden = ysq;
	for (i__ = 1; i__ <= 4; ++i__) {
	    xnum = (xnum + p[i__ - 1]) * ysq;
	    xden = (xden + q[i__ - 1]) * ysq;
/* L240: */
	}
	*result = ysq * (xnum + p[4]) / (xden + q[4]);
	*result = (sqrpi - *result) / y;
	if (*jint != 2) {
	    r__1 = y * sixten;
	    ysq = r_int(&r__1) / sixten;
	    del = (y - ysq) * (y + ysq);
	    *result = exp(-ysq * ysq) * exp(-del) * *result;
	}
    }
/* ------------------------------------------------------------------ */
/*  Fix up for negative argument, erf, etc. */
/* ------------------------------------------------------------------ */
L300:
    if (*jint == 0) {
	*result = half - *result + half;
	if (x < zero) {
	    *result = -(*result);
	}
    } else if (*jint == 1) {
	if (x < zero) {
	    *result = two - *result;
	}
    } else {
	if (x < zero) {
	    if (x < xneg) {
		*result = xinf;
	    } else {
		r__1 = x * sixten;
		ysq = r_int(&r__1) / sixten;
		del = (x - ysq) * (x + ysq);
		y = exp(ysq * ysq) * exp(del);
		*result = y + y - *result;
	    }
	}
    }
L800:
    return 0;
/* ---------- Last card of CALERF ---------- */
} /* calerf_ */

doublereal erf_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer jint;
    extern /* Subroutine */ int calerf_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This subprogram computes approximate values for erf(x). */
/*   (see comments heading CALERF). */

/*   Author/date: W. J. Cody, January 8, 1985 */

/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------ */
    jint = 0;
    calerf_(x, &result, &jint);
    ret_val = result;
    return ret_val;
/* ---------- Last card of DERF ---------- */
} /* erf_ */

doublereal erfc_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer jint;
    extern /* Subroutine */ int calerf_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This subprogram computes approximate values for erfc(x). */
/*   (see comments heading CALERF). */

/*   Author/date: W. J. Cody, January 8, 1985 */

/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------ */
    jint = 1;
    calerf_(x, &result, &jint);
    ret_val = result;
    return ret_val;
} /* erfc_ */

doublereal erfcx_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer jint;
    extern /* Subroutine */ int calerf_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This subprogram computes approximate values for erfcx(x). */
/*   (see comments heading CALERF). */

/*   Author/date: W. J. Cody, January 8, 1985 */

/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------ */
    jint = 2;
    calerf_(x, &result, &jint);
    ret_val = result;
    return ret_val;
} /* erfcx_ */

