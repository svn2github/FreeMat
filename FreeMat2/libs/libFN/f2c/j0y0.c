/* ../j0y0.f -- translated by f2c (version 20031025).
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

/* Subroutine */ int caljy0_(real *arg, real *result, integer *jint)
{
    /* Initialized data */

    static real zero = 0.f;
    static real two56 = 256.f;
    static real cons = -.11593151565841244881f;
    static real pi2 = .63661977236758134308f;
    static real twopi = 6.2831853071795864769f;
    static real twopi1 = 6.28125f;
    static real twopi2 = .0019353071795864769253f;
    static real xmax = 8190.f;
    static real xsmall = 1.22e-9f;
    static real xinf = 1.7e38f;
    static real xj0 = 2.4048255576957727686f;
    static real one = 1.f;
    static real xj1 = 5.5200781102863106496f;
    static real xy0 = .89357696627916752158f;
    static real xy1 = 3.9576784193148578684f;
    static real xy2 = 7.0860510603017726976f;
    static real xj01 = 616.f;
    static real xj02 = -.0014244423042272313784f;
    static real xj11 = 1413.f;
    static real xj12 = 5.468602863106495966e-4f;
    static real xy01 = 228.f;
    static real xy02 = .0029519662791675215849f;
    static real three = 3.f;
    static real xy11 = 1013.f;
    static real xy12 = 6.4716931485786837568e-4f;
    static real xy21 = 1814.f;
    static real xy22 = 1.1356030177269762362e-4f;
    static real plg[4] = { -24.562334077563243311f,236.42701335621505212f,
	    -549.89956895857911039f,356.87548468071500413f };
    static real qlg[4] = { -35.553900764052419184f,194.00230218539473193f,
	    -334.42903192607538956f,178.43774234035750207f };
    static real pj0[7] = { 6630299.7904833794242f,-621407004.23540120665f,
	    27282507878.605942706f,-412986685009.90866786f,
	    -.12117036164593528341f,103.44222815443188943f,
	    -36629.814655107086448f };
    static real qj0[5] = { 456126.962242199382f,139850973.72263433271f,
	    26328198300.859648632f,2388378799633.2290397f,
	    936.14022392337710626f };
    static real pj1[8] = { 4417.6707025325087628f,11725.046279757103576f,
	    10341.910641583726701f,-7287.9702464464618998f,
	    -12254.078161378989535f,-1831.9397969392084011f,
	    48.591703355916499363f,743.21196680624245801f };
    static real qj1[7] = { 333.07310774649071172f,-2945.8766545509337327f,
	    18680.990008359188352f,-84055.062591169562211f,
	    245991.02262586308984f,-357834.78026152301072f,
	    -25.258076240801555057f };
    static real four = 4.f;
    static real py0[6] = { 10102.53294802090759f,-2128754.8474401797963f,
	    204222743.57376619816f,-8371625545.1260504098f,
	    107235387820.03176831f,-18.402381979244993524f };
    static real qy0[5] = { 664.75986689240190091f,238893.93209447253406f,
	    55662956.624278251596f,8161718777.7290363573f,
	    588738657389.97033405f };
    static real py1[7] = { -14566.86583266363592f,4690528.861167863151f,
	    -695904393.94619619534f,43600098638.603061642f,
	    -551074352067.22644429f,-22213976967566.192242f,
	    17.427031242901594547f };
    static real qy1[6] = { 830.30857612070288823f,406699.82352539552018f,
	    139602027.70986831075f,34015103849.971240096f,
	    5426682441941.234755f,433861465807072.64428f };
    static real py2[8] = { 21363.534169313901632f,-10085539.923498211426f,
	    2195882717.0518100757f,-193630512667.72083678f,
	    -128299123640.88687306f,670166418691732.37784f,
	    -8072872690515021.0443f,-17.439661319197499338f };
    static real qy2[7] = { 879.03362168128450017f,539247.3920976805703f,
	    247272194.75672302327f,86926121104.209825246f,
	    22598377924042.897629f,3927242556964030.9819f,
	    345637246288464575.19f };
    static real p0[6] = { 3480.6486443249270347f,21170.523380864944322f,
	    41345.386639580765797f,22779.090197304684302f,
	    .88961548424210455236f,153.76201909008354296f };
    static real q0[5] = { 3502.8735138235608207f,21215.35056188011573f,
	    41370.41249551041664f,22779.090197304684318f,
	    157.11159858080893649f };
    static real p1[6] = { -22.300261666214198472f,-111.83429920482737611f,
	    -185.919536443429938f,-89.226600200800094098f,
	    -.0088033303048680751817f,-1.2441026745835638459f };
    static real q1[5] = { 1488.7231232283756582f,7264.2780169211018836f,
	    11951.131543434613647f,5710.5024128512061905f,
	    90.593769594993125859f };
    static real eight = 8.f;
    static real five5 = 5.5f;
    static real sixty4 = 64.f;
    static real oneov8 = .125f;
    static real p17 = .1716f;

    /* Builtin functions */
    double log(doublereal), r_int(real *), cos(doublereal), sin(doublereal), 
	    sqrt(doublereal);

    /* Local variables */
    static integer i__;
    static real w, z__, r0, r1, ax, up, xy, wsq, zsq, xden, resj, prod, down, 
	    xnum;

/* --------------------------------------------------------------------- */

/* This packet computes zero-order Bessel functions of the first and */
/*   second kind (J0 and Y0), for real arguments X, where 0 < X <= XMAX */
/*   for Y0, and |X| <= XMAX for J0.  It contains two function-type */
/*   subprograms,  BESJ0  and  BESY0,  and one subroutine-type */
/*   subprogram,  CALJY0.  The calling statements for the primary */
/*   entries are: */

/*           Y = BESJ0(X) */
/*   and */
/*           Y = BESY0(X), */

/*   where the entry points correspond to the functions J0(X) and Y0(X), */
/*   respectively.  The routine  CALJY0  is intended for internal packet */
/*   use only, all computations within the packet being concentrated in */
/*   this one routine.  The function subprograms invoke  CALJY0  with */
/*   the statement */
/*           CALL CALJY0(ARG,RESULT,JINT), */
/*   where the parameter usage is as follows: */

/*      Function                  Parameters for CALJY0 */
/*       call              ARG             RESULT          JINT */

/*     BESJ0(ARG)     |ARG| .LE. XMAX       J0(ARG)          0 */
/*     BESY0(ARG)   0 .LT. ARG .LE. XMAX    Y0(ARG)          1 */

/*   The main computation uses unpublished minimax rational */
/*   approximations for X .LE. 8.0, and an approximation from the */
/*   book  Computer Approximations  by Hart, et. al., Wiley and Sons, */
/*   New York, 1968, for arguments larger than 8.0   Part of this */
/*   transportable packet is patterned after the machine-dependent */
/*   FUNPACK program BESJ0(X), but cannot match that version for */
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
/*   XSMALL = positive argument such that  1.0-(X/2)**2 = 1.0 */
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
/*    -XINF when BESLY0 is called with a negative or zero argument. */


/* Intrinsic functions required are: */

/*     ABS, AINT, COS, LOG, SIN, SQRT */


/*  Latest modification: June 2, 1989 */

/*  Author: W. J. Cody */
/*          Mathematics and Computer Science Division */
/*          Argonne National Laboratory */
/*          Argonne, IL 60439 */

/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Mathematical constants */
/*    CONS = ln(.5) + Euler's gamma */
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
/*  J0(X) / (X**2 - XJ0**2),  XSMALL  <  |X|  <=  4.0 */
/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*  J0(X) / (X**2 - XJ1**2),  4.0  <  |X|  <=  8.0 */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*    (Y0(X) - 2 LN(X/XY0) J0(X)) / (X**2 - XY0**2), */
/*        XSMALL  <  |X|  <=  3.0 */
/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*    (Y0(X) - 2 LN(X/XY1) J0(X)) / (X**2 - XY1**2), */
/*        3.0  <  |X|  <=  5.5 */
/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for rational approximation of */
/*    (Y0(X) - 2 LN(X/XY2) J0(X)) / (X**2 - XY2**2), */
/*        5.5  <  |X|  <=  8.0 */
/* -------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Coefficients for Hart,s approximation,  |X| > 8.0 */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*  Check for error conditions */
/* ------------------------------------------------------------------- */
    ax = dabs(*arg);
    if (*jint == 1 && *arg <= zero) {
	*result = -xinf;
	goto L2000;
    } else if (ax > xmax) {
	*result = zero;
	goto L2000;
    }
    if (ax > eight) {
	goto L800;
    }
    if (ax <= xsmall) {
	if (*jint == 0) {
	    *result = one;
	} else {
	    *result = pi2 * (log(ax) + cons);
	}
	goto L2000;
    }
/* ------------------------------------------------------------------- */
/*  Calculate J0 for appropriate interval, preserving */
/*     accuracy near the zero of J0 */
/* ------------------------------------------------------------------- */
    zsq = ax * ax;
    if (ax <= four) {
	xnum = (pj0[4] * zsq + pj0[5]) * zsq + pj0[6];
	xden = zsq + qj0[4];
	for (i__ = 1; i__ <= 4; ++i__) {
	    xnum = xnum * zsq + pj0[i__ - 1];
	    xden = xden * zsq + qj0[i__ - 1];
/* L50: */
	}
	prod = (ax - xj01 / two56 - xj02) * (ax + xj0);
    } else {
	wsq = one - zsq / sixty4;
	xnum = pj1[6] * wsq + pj1[7];
	xden = wsq + qj1[6];
	for (i__ = 1; i__ <= 6; ++i__) {
	    xnum = xnum * wsq + pj1[i__ - 1];
	    xden = xden * wsq + qj1[i__ - 1];
/* L220: */
	}
	prod = (ax + xj1) * (ax - xj11 / two56 - xj12);
    }
    *result = prod * xnum / xden;
    if (*jint == 0) {
	goto L2000;
    }
/* ------------------------------------------------------------------- */
/*  Calculate Y0.  First find  RESJ = pi/2 ln(x/xn) J0(x), */
/*    where xn is a zero of Y0 */
/* ------------------------------------------------------------------- */
    if (ax <= three) {
	up = ax - xy01 / two56 - xy02;
	xy = xy0;
    } else if (ax <= five5) {
	up = ax - xy11 / two56 - xy12;
	xy = xy1;
    } else {
	up = ax - xy21 / two56 - xy22;
	xy = xy2;
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
/*  Now calculate Y0 for appropriate interval, preserving */
/*     accuracy near the zero of Y0 */
/* ------------------------------------------------------------------- */
    if (ax <= three) {
	xnum = py0[5] * zsq + py0[0];
	xden = zsq + qy0[0];
	for (i__ = 2; i__ <= 5; ++i__) {
	    xnum = xnum * zsq + py0[i__ - 1];
	    xden = xden * zsq + qy0[i__ - 1];
/* L340: */
	}
    } else if (ax <= five5) {
	xnum = py1[6] * zsq + py1[0];
	xden = zsq + qy1[0];
	for (i__ = 2; i__ <= 6; ++i__) {
	    xnum = xnum * zsq + py1[i__ - 1];
	    xden = xden * zsq + qy1[i__ - 1];
/* L360: */
	}
    } else {
	xnum = py2[7] * zsq + py2[0];
	xden = zsq + qy2[0];
	for (i__ = 2; i__ <= 7; ++i__) {
	    xnum = xnum * zsq + py2[i__ - 1];
	    xden = xden * zsq + qy2[i__ - 1];
/* L380: */
	}
    }
    *result = resj + up * down * xnum / xden;
    goto L2000;
/* ------------------------------------------------------------------- */
/*  Calculate J0 or Y0 for |ARG|  >  8.0 */
/* ------------------------------------------------------------------- */
L800:
    z__ = eight / ax;
    w = ax / twopi;
    w = r_int(&w) + oneov8;
    w = ax - w * twopi1 - w * twopi2;
    zsq = z__ * z__;
    xnum = p0[4] * zsq + p0[5];
    xden = zsq + q0[4];
    up = p1[4] * zsq + p1[5];
    down = zsq + q1[4];
    for (i__ = 1; i__ <= 4; ++i__) {
	xnum = xnum * zsq + p0[i__ - 1];
	xden = xden * zsq + q0[i__ - 1];
	up = up * zsq + p1[i__ - 1];
	down = down * zsq + q1[i__ - 1];
/* L850: */
    }
    r0 = xnum / xden;
    r1 = up / down;
    if (*jint == 0) {
	*result = sqrt(pi2 / ax) * (r0 * cos(w) - z__ * r1 * sin(w));
    } else {
	*result = sqrt(pi2 / ax) * (r0 * sin(w) + z__ * r1 * cos(w));
    }
L2000:
    return 0;
/* ---------- Last line of CALJY0 ---------- */
} /* caljy0_ */

doublereal besj0_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer jint;
    extern /* Subroutine */ int caljy0_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This subprogram computes approximate values for Bessel functions */
/*   of the first kind of order zero for arguments  |X| <= XMAX */
/*   (see comments heading CALJY0). */

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
    jint = 0;
    caljy0_(x, &result, &jint);
    ret_val = result;
    return ret_val;
/* ---------- Last line of BESJ0 ---------- */
} /* besj0_ */

doublereal besy0_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer jint;
    extern /* Subroutine */ int caljy0_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This subprogram computes approximate values for Bessel functions */
/*   of the second kind of order zero for arguments 0 < X <= XMAX */
/*   (see comments heading CALJY0). */

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
    jint = 1;
    caljy0_(x, &result, &jint);
    ret_val = result;
    return ret_val;
/* ---------- Last line of BESY0 ---------- */
} /* besy0_ */

