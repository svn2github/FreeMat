/* ../gamma.f -- translated by f2c (version 20031025).
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

doublereal gamma_(real *x)
{
    /* Initialized data */

    static real one = 1.f;
    static real eps = 1.19e-7f;
    static real xinf = 3.4e38f;
    static real p[8] = { -1.71618513886549492533811f,
	    24.7656508055759199108314f,-379.804256470945635097577f,
	    629.331155312818442661052f,866.966202790413211295064f,
	    -31451.2729688483675254357f,-36144.4134186911729807069f,
	    66456.1438202405440627855f };
    static real q[8] = { -30.8402300119738975254353f,
	    315.350626979604161529144f,-1015.15636749021914166146f,
	    -3107.77167157231109440444f,22538.1184209801510330112f,
	    4755.84627752788110767815f,-134659.959864969306392456f,
	    -115132.259675553483497211f };
    static real c__[7] = { -.001910444077728f,8.4171387781295e-4f,
	    -5.952379913043012e-4f,7.93650793500350248e-4f,
	    -.002777777777777681622553f,.08333333333333333331554247f,
	    .0057083835261f };
    static real half = .5f;
    static real twelve = 12.f;
    static real two = 2.f;
    static real zero = 0.f;
    static real sqrtpi = .9189385332046727417803297f;
    static real pi = 3.1415926535897932384626434f;
    static real xbig = 35.04f;
    static real xminin = 1.18e-38f;

    /* System generated locals */
    integer i__1;
    real ret_val, r__1;

    /* Builtin functions */
    double r_int(real *), sin(doublereal), log(doublereal), exp(doublereal);

    /* Local variables */
    static integer i__, n;
    static real y, z__, y1, res, sum, ysq, fact, xden, xnum;
    static logical parity;

/* ---------------------------------------------------------------------- */

/* This routine calculates the GAMMA function for a real argument X. */
/*   Computation is based on an algorithm outlined in reference 1. */
/*   The program uses rational functions that approximate the GAMMA */
/*   function to at least 20 significant decimal digits.  Coefficients */
/*   for the approximation over the interval (1,2) are unpublished. */
/*   Those for the approximation for X .GE. 12 are from reference 2. */
/*   The accuracy achieved depends on the arithmetic system, the */
/*   compiler, the intrinsic functions, and proper selection of the */
/*   machine-dependent constants. */


/* ******************************************************************* */
/* ******************************************************************* */

/* Explanation of machine-dependent constants */

/* beta   - radix for the floating-point representation */
/* maxexp - the smallest positive power of beta that overflows */
/* XBIG   - the largest argument for which GAMMA(X) is representable */
/*          in the machine, i.e., the solution to the equation */
/*                  GAMMA(XBIG) = beta**maxexp */
/* XINF   - the largest machine representable floating-point number; */
/*          approximately beta**maxexp */
/* EPS    - the smallest positive floating-point number such that */
/*          1.0+EPS .GT. 1.0 */
/* XMININ - the smallest positive floating-point number such that */
/*          1/XMININ is machine representable */

/*     Approximate values for some important machines are: */

/*                            beta       maxexp        XBIG */

/* CRAY-1         (S.P.)        2         8191        966.961 */
/* Cyber 180/855 */
/*   under NOS    (S.P.)        2         1070        177.803 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)   (S.P.)        2          128        35.040 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)   (D.P.)        2         1024        171.624 */
/* IBM 3033       (D.P.)       16           63        57.574 */
/* VAX D-Format   (D.P.)        2          127        34.844 */
/* VAX G-Format   (D.P.)        2         1023        171.489 */

/*                            XINF         EPS        XMININ */

/* CRAY-1         (S.P.)   5.45E+2465   7.11E-15    1.84E-2466 */
/* Cyber 180/855 */
/*   under NOS    (S.P.)   1.26E+322    3.55E-15    3.14E-294 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)   (S.P.)   3.40E+38     1.19E-7     1.18E-38 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)   (D.P.)   1.79D+308    2.22D-16    2.23D-308 */
/* IBM 3033       (D.P.)   7.23D+75     2.22D-16    1.39D-76 */
/* VAX D-Format   (D.P.)   1.70D+38     1.39D-17    5.88D-39 */
/* VAX G-Format   (D.P.)   8.98D+307    1.11D-16    1.12D-308 */

/* ******************************************************************* */
/* ******************************************************************* */

/* Error returns */

/*  The program returns the value XINF for singularities or */
/*     when overflow would occur.  The computation is believed */
/*     to be free of underflow and overflow. */


/*  Intrinsic functions required are: */

/*     INT, DBLE, EXP, LOG, REAL, SIN */


/* References: "An Overview of Software Development for Special */
/*              Functions", W. J. Cody, Lecture Notes in Mathematics, */
/*              506, Numerical Analysis Dundee, 1975, G. A. Watson */
/*              (ed.), Springer Verlag, Berlin, 1976. */

/*              Computer Approximations, Hart, Et. Al., Wiley and */
/*              sons, New York, 1968. */

/*  Latest modification: October 12, 1989 */

/*  Authors: W. J. Cody and L. Stoltz */
/*           Applied Mathematics Division */
/*           Argonne National Laboratory */
/*           Argonne, IL 60439 */

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Mathematical constants */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Machine dependent parameters */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Numerator and denominator coefficients for rational minimax */
/*     approximation over (1,2). */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for minimax approximation over (12, INF). */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Statement functions for conversion between integer and float */
/* ---------------------------------------------------------------------- */
    parity = FALSE_;
    fact = one;
    n = 0;
    y = *x;
    if (y <= zero) {
/* ---------------------------------------------------------------------- */
/*  Argument is negative */
/* ---------------------------------------------------------------------- */
	y = -(*x);
	y1 = r_int(&y);
	res = y - y1;
	if (res != zero) {
	    r__1 = y1 * half;
	    if (y1 != r_int(&r__1) * two) {
		parity = TRUE_;
	    }
	    fact = -pi / sin(pi * res);
	    y += one;
	} else {
	    res = xinf;
	    goto L900;
	}
    }
/* ---------------------------------------------------------------------- */
/*  Argument is positive */
/* ---------------------------------------------------------------------- */
    if (y < eps) {
/* ---------------------------------------------------------------------- */
/*  Argument .LT. EPS */
/* ---------------------------------------------------------------------- */
	if (y >= xminin) {
	    res = one / y;
	} else {
	    res = xinf;
	    goto L900;
	}
    } else if (y < twelve) {
	y1 = y;
	if (y < one) {
/* ---------------------------------------------------------------------- */
/*  0.0 .LT. argument .LT. 1.0 */
/* ---------------------------------------------------------------------- */
	    z__ = y;
	    y += one;
	} else {
/* ---------------------------------------------------------------------- */
/*  1.0 .LT. argument .LT. 12.0, reduce argument if necessary */
/* ---------------------------------------------------------------------- */
	    n = (integer) y - 1;
	    y -= (real) n;
	    z__ = y - one;
	}
/* ---------------------------------------------------------------------- */
/*  Evaluate approximation for 1.0 .LT. argument .LT. 2.0 */
/* ---------------------------------------------------------------------- */
	xnum = zero;
	xden = one;
	for (i__ = 1; i__ <= 8; ++i__) {
	    xnum = (xnum + p[i__ - 1]) * z__;
	    xden = xden * z__ + q[i__ - 1];
/* L260: */
	}
	res = xnum / xden + one;
	if (y1 < y) {
/* ---------------------------------------------------------------------- */
/*  Adjust result for case  0.0 .LT. argument .LT. 1.0 */
/* ---------------------------------------------------------------------- */
	    res /= y1;
	} else if (y1 > y) {
/* ---------------------------------------------------------------------- */
/*  Adjust result for case  2.0 .LT. argument .LT. 12.0 */
/* ---------------------------------------------------------------------- */
	    i__1 = n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		res *= y;
		y += one;
/* L290: */
	    }
	}
    } else {
/* ---------------------------------------------------------------------- */
/*  Evaluate for argument .GE. 12.0, */
/* ---------------------------------------------------------------------- */
	if (y <= xbig) {
	    ysq = y * y;
	    sum = c__[6];
	    for (i__ = 1; i__ <= 6; ++i__) {
		sum = sum / ysq + c__[i__ - 1];
/* L350: */
	    }
	    sum = sum / y - y + sqrtpi;
	    sum += (y - half) * log(y);
	    res = exp(sum);
	} else {
	    res = xinf;
	    goto L900;
	}
    }
/* ---------------------------------------------------------------------- */
/*  Final adjustments and return */
/* ---------------------------------------------------------------------- */
    if (parity) {
	res = -res;
    }
    if (fact != one) {
	res = fact / res;
    }
L900:
    ret_val = res;
    return ret_val;
/* ---------- Last line of GAMMA ---------- */
} /* gamma_ */

