/* ../j1y1.f -- translated by f2c (version 20031025).
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

/* Subroutine */ int caljy1_(real *arg, real *result, integer *jint)
{
    /* Initialized data */

    static real eight = 8.f;
    static real twopi2 = .0019353071795864769253f;
    static real two56 = 256.f;
    static real rtpi2 = .79788456080286535588f;
    static real xmax = 8190.f;
    static real xsmall = 1.22e-9f;
    static real xinf = 1.7e38f;
    static real xj0 = 3.8317059702075123156f;
    static real xj1 = 7.0155866698156187535f;
    static real xy0 = 2.1971413260310170351f;
    static real xy1 = 5.4296810407941351328f;
    static real four = 4.f;
    static real xj01 = 981.f;
    static real xj02 = -3.2527979248768438556e-4f;
    static real xj11 = 1796.f;
    static real xj12 = -3.833018438124646295e-5f;
    static real xy01 = 562.f;
    static real xy02 = .001828826031017035149f;
    static real xy11 = 1390.f;
    static real xy12 = -6.4592058648672279948e-6f;
    static real plg[4] = { -24.562334077563243311f,236.42701335621505212f,
	    -549.89956895857911039f,356.87548468071500413f };
    static real qlg[4] = { -35.553900764052419184f,194.00230218539473193f,
	    -334.42903192607538956f,178.43774234035750207f };
    static real half = .5f;
    static real pj0[7] = { 980629.04098958257677f,-115486967.64841276794f,
	    6678104126.1492395835f,-142585098013.66645672f,
	    -4461.579298277507613f,10.650724020080236441f,
	    -.010767857011487300348f };
    static real qj0[5] = { 591176.14494174794095f,202283751.40097033958f,
	    42091902282.580133541f,4186860446082.017529f,
	    1074.2272239517380498f };
    static real pj1[8] = { 4.617919185275825228f,-7132.9006872560947377f,
	    4503965.8105749078904f,-1443771771.8363239107f,
	    235692853972.17157313f,-16324168293282.543629f,
	    113570227199794.68624f,1005189971711528.5432f };
    static real qj1[7] = { 1126712.506502913805f,648725028.99596389593f,
	    276227772862.44082666f,84899346165481.429307f,
	    17128800897135812.012f,1725390588844768119.4f,
	    1388.6978985861357615f };
    static real py0[7] = { 221579.5322228026082f,-59157479.997408395984f,
	    7214454821.4502560419f,-375959744978.19597599f,
	    5470861171652.5426053f,40535726612579.544093f,
	    -317.14424660046133456f };
    static real qy0[6] = { 820.79908168393867438f,381364.70753052572164f,
	    122504351.2218296322f,27800352738.690585613f,
	    4127228620040.6461981f,307378739210792.86084f };
    static real py1[9] = { 1915380.6858264202986f,-1195796191.2070617006f,
	    374536739624.38488783f,-59530713129741.981618f,
	    4068627528980474.4814f,-23638408497043134.724f,
	    -5680809457472420457.7f,11514276357909013326.f,
	    -1233.7180442012953128f };
    static real qy1[8] = { 1285.5164849321609336f,1045374.8201934079734f,
	    635503180.87088919566f,302217668529.60403645f,
	    111870100658569.71027f,30837179548112881.95f,
	    5696819882285717891.1f,533218443133161856970.f };
    static real p0[6] = { -109824.05543459346727f,-1523529.3511811373833f,
	    -6603373.2483649391093f,-9942246.5050776411957f,
	    -4435757.8167941278571f,-1611.6166443246101165f };
    static real q0[6] = { -107263.85991103820119f,-1511809.5066341608816f,
	    -6585339.4797230870728f,-9934124.389934585659f,
	    -4435757.8167941278568f,-1455.0094401904961825f };
    static real throv8 = .375f;
    static real p1[6] = { 1706.3754290207680021f,18494.262873223866797f,
	    66178.836581270835179f,85145.160675335701966f,
	    33220.913409857223519f,35.265133846636032186f };
    static real q1[6] = { 37890.229745772202641f,400294.43582266975117f,
	    1419460.6696037208929f,1819458.0422439972989f,
	    708712.81941028743574f,863.83677696049909675f };
    static real pi2 = .63661977236758134308f;
    static real p17 = .1716f;
    static real twopi = 6.2831853071795864769f;
    static real zero = 0.f;
    static real twopi1 = 6.28125f;

    /* System generated locals */
    real r__1;

    /* Builtin functions */
    double log(doublereal), r_int(real *), sqrt(doublereal), cos(doublereal), 
	    sin(doublereal);

    /* Local variables */
    static integer i__;
    static real w, z__, r0, r1, ax, up, xy, wsq, zsq, xden, resj, prod, down, 
	    xnum;

/* --------------------------------------------------------------------- */

/* This packet computes first-order Bessel functions of the first and */
/*   second kind (J1 and Y1), for real arguments X, where 0 < X <= XMAX */
/*   for Y1, and |X| <= XMAX for J1.  It contains two function-type */
/*   subprograms,  BESJ1  and  BESY1,  and one subroutine-type */
/*   subprogram,  CALJY1.  The calling statements for the primary */
/*   entries are: */

/*           Y = BESJ1(X) */
/*   and */
/*           Y = BESY1(X), */

/*   where the entry points correspond to the functions J1(X) and Y1(X), */
/*   respectively.  The routine  CALJY1  is intended for internal packet */
/*   use only, all computations within the packet being concentrated in */
/*   this one routine.  The function subprograms invoke  CALJY1  with */
/*   the statement */
/*           CALL CALJY1(ARG,RESULT,JINT), */
/*   where the parameter usage is as follows: */

/*      Function                  Parameters for CALJY1 */
/*       call              ARG             RESULT          JINT */

/*     BESJ1(ARG)     |ARG| .LE. XMAX       J1(ARG)          0 */
/*     BESY1(ARG)   0 .LT. ARG .LE. XMAX    Y1(ARG)          1 */

/*   The main computation uses unpublished minimax rational */
/*   approximations for X .LE. 8.0, and an approximation from the */
/*   book  Computer Approximations  by Hart, et. al., Wiley and Sons, */
/*   New York, 1968, for arguments larger than 8.0   Part of this */
/*   transportable packet is patterned after the machine-dependent */
/*   FUNPACK program BESJ1(X), but cannot match that version for */
/*   efficiency or accuracy.  This version uses rational functions */
/*   that are theoretically accurate to at least 18 significant decimal */
/*   digits for X <= 8, and at least 18 decimal places for X > 8.  The */
/*   accuracy achieved depends on the arithmetic system, the compiler, */
/*   the intrinsic functions, and proper selection of the machine- */
/*   dependent constants. */

/* ******************************************************************* */

/* Explanation of machine-dependent constants */

/*   XINF   = largest positive machine number */
/*   XMAX   = largest acceptable argument.  The functions AINT, SIN */
/*            and COS must perform properly for  ABS(X) .LE. XMAX. */
/*            We recommend that XMAX be a small integer multiple of */
/*            sqrt(1/eps), where eps is the smallest positive number */
/*            such that  1+eps > 1. */
/*   XSMALL = positive argument such that  1.0-(1/2)(X/2)**2 = 1.0 */
/*            to machine precision for all  ABS(X) .LE. XSMALL. */
/*            We recommend that  XSMALL < sqrt(eps)/beta, where beta */
/*            is the floating-point radix (usually 2 or 16). */

/*     Approximate values for some important machines are */

/*                          eps      XMAX     XSMALL      XINF */

/*  CDC 7600      (S.P.)  7.11E-15  1.34E+08  2.98E-08  1.26E+322 */
/*  CRAY-1        (S.P.)  7.11E-15  1.34E+08  2.98E-08  5.45E+2465 */
/*  IBM PC (8087) (S.P.)  5.96E-08  8.19E+03  1.22E-04  3.40E+38 */
/*  IBM PC (8087) (D.P.)  1.11D-16  2.68D+08  3.72D-09  1.79D+308 */
/*  IBM 195       (D.P.)  2.22D-16  6.87D+09  9.09D-13  7.23D+75 */
/*  UNIVAC 1108   (D.P.)  1.73D-18  4.30D+09  2.33D-10  8.98D+307 */
/*  VAX 11/780    (D.P.)  1.39D-17  1.07D+09  9.31D-10  1.70D+38 */

/* ******************************************************************* */
/* ******************************************************************* */

/* Error Returns */

/*  The program returns the value zero for  X .GT. XMAX, and returns */
/*    -XINF when BESLY1 is called with a negative or zero argument. */


/* Intrinsic functions required are: */

/*     ABS, AINT, COS, LOG, SIN, SQRT */


/*  Author: W. J. Cody */
/*          Mathematics and Computer Science Division */
/*          Argonne National Laboratory */
/*          Argonne, IL 60439 */

/*  Latest modification: November 10, 1987 */

/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Mathematical constants */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Machine-dependent constants */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Zeroes of Bessel functions */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation to ln(x/a) */
/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*  J1(X) / (X * (X**2 - XJ0**2)),  XSMALL  <  |X|  <=  4.0 */
/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*  J1(X) / (X * (X**2 - XJ1**2)),  4.0  <  |X|  <=  8.0 */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*    (Y1(X) - 2 LN(X/XY0) J1(X)) / (X**2 - XY0**2), */
/*        XSMALL  <  |X|  <=  4.0 */
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*    (Y1(X) - 2 LN(X/XY1) J1(X)) / (X**2 - XY1**2), */
/*        4.0  <  |X|  <=  8.0 */
/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for Hart,s approximation,  |X| > 8.0 */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Check for error conditions */
/* ------------------------------------------------------------------- */
    ax = dabs(*arg);
    if (*jint == 1 && (*arg <= zero || *arg < half && ax * xinf < pi2)) {
	*result = -xinf;
	goto L2000;
    } else if (ax > xmax) {
	*result = zero;
	goto L2000;
    }
    if (ax > eight) {
	goto L800;
    } else if (ax <= xsmall) {
	if (*jint == 0) {
	    *result = *arg * half;
	} else {
	    *result = -pi2 / ax;
	}
	goto L2000;
    }
/* ------------------------------------------------------------------- */
/*  Calculate J1 for appropriate interval, preserving */
/*     accuracy near the zero of J1 */
/* ------------------------------------------------------------------- */
    zsq = ax * ax;
    if (ax <= four) {
	xnum = (pj0[6] * zsq + pj0[5]) * zsq + pj0[4];
	xden = zsq + qj0[4];
	for (i__ = 1; i__ <= 4; ++i__) {
	    xnum = xnum * zsq + pj0[i__ - 1];
	    xden = xden * zsq + qj0[i__ - 1];
/* L50: */
	}
	prod = *arg * (ax - xj01 / two56 - xj02) * (ax + xj0);
    } else {
	xnum = pj1[0];
	xden = (zsq + qj1[6]) * zsq + qj1[0];
	for (i__ = 2; i__ <= 6; ++i__) {
	    xnum = xnum * zsq + pj1[i__ - 1];
	    xden = xden * zsq + qj1[i__ - 1];
/* L220: */
	}
	xnum = xnum * (ax - eight) * (ax + eight) + pj1[6];
	xnum = xnum * (ax - four) * (ax + four) + pj1[7];
	prod = *arg * (ax - xj11 / two56 - xj12) * (ax + xj1);
    }
    *result = prod * (xnum / xden);
    if (*jint == 0) {
	goto L2000;
    }
/* ------------------------------------------------------------------- */
/*  Calculate Y1.  First find  RESJ = pi/2 ln(x/xn) J1(x), */
/*    where xn is a zero of Y1 */
/* ------------------------------------------------------------------- */
    if (ax <= four) {
	up = ax - xy01 / two56 - xy02;
	xy = xy0;
    } else {
	up = ax - xy11 / two56 - xy12;
	xy = xy1;
    }
    down = ax + xy;
    if (dabs(up) < p17 * down) {
	w = up / down;
	wsq = w * w;
	xnum = plg[0];
	xden = wsq + qlg[0];
	for (i__ = 2; i__ <= 4; ++i__) {
	    xnum = xnum * wsq + plg[i__ - 1];
	    xden = xden * wsq + qlg[i__ - 1];
/* L320: */
	}
	resj = pi2 * *result * w * xnum / xden;
    } else {
	resj = pi2 * *result * log(ax / xy);
    }
/* ------------------------------------------------------------------- */
/*  Now calculate Y1 for appropriate interval, preserving */
/*     accuracy near the zero of Y1 */
/* ------------------------------------------------------------------- */
    if (ax <= four) {
	xnum = py0[6] * zsq + py0[0];
	xden = zsq + qy0[0];
	for (i__ = 2; i__ <= 6; ++i__) {
	    xnum = xnum * zsq + py0[i__ - 1];
	    xden = xden * zsq + qy0[i__ - 1];
/* L340: */
	}
    } else {
	xnum = py1[8] * zsq + py1[0];
	xden = zsq + qy1[0];
	for (i__ = 2; i__ <= 8; ++i__) {
	    xnum = xnum * zsq + py1[i__ - 1];
	    xden = xden * zsq + qy1[i__ - 1];
/* L360: */
	}
    }
    *result = resj + up * down / ax * xnum / xden;
    goto L2000;
/* ------------------------------------------------------------------- */
/*  Calculate J1 or Y1 for |ARG|  >  8.0 */
/* ------------------------------------------------------------------- */
L800:
    z__ = eight / ax;
    r__1 = ax / twopi;
    w = r_int(&r__1) + throv8;
    w = ax - w * twopi1 - w * twopi2;
    zsq = z__ * z__;
    xnum = p0[5];
    xden = zsq + q0[5];
    up = p1[5];
    down = zsq + q1[5];
    for (i__ = 1; i__ <= 5; ++i__) {
	xnum = xnum * zsq + p0[i__ - 1];
	xden = xden * zsq + q0[i__ - 1];
	up = up * zsq + p1[i__ - 1];
	down = down * zsq + q1[i__ - 1];
/* L850: */
    }
    r0 = xnum / xden;
    r1 = up / down;
    if (*jint == 0) {
	*result = rtpi2 / sqrt(ax) * (r0 * cos(w) - z__ * r1 * sin(w));
    } else {
	*result = rtpi2 / sqrt(ax) * (r0 * sin(w) + z__ * r1 * cos(w));
    }
    if (*jint == 0 && *arg < zero) {
	*result = -(*result);
    }
L2000:
    return 0;
/* ---------- Last card of CALJY1 ---------- */
} /* caljy1_ */

doublereal besj1_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer jint;
    extern /* Subroutine */ int caljy1_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This subprogram computes approximate values for Bessel functions */
/*   of the first kind of order zero for arguments  |X| <= XMAX */
/*   (see comments heading CALJY1). */

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
    jint = 0;
    caljy1_(x, &result, &jint);
    ret_val = result;
    return ret_val;
/* ---------- Last card of BESJ1 ---------- */
} /* besj1_ */

doublereal besy1_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer jint;
    extern /* Subroutine */ int caljy1_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This subprogram computes approximate values for Bessel functions */
/*   of the second kind of order zero for arguments 0 < X <= XMAX */
/*   (see comments heading CALJY1). */

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
    jint = 1;
    caljy1_(x, &result, &jint);
    ret_val = result;
    return ret_val;
/* ---------- Last card of BESY1 ---------- */
} /* besy1_ */

