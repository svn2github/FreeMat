/* daw.f -- translated by f2c (version 20031025).
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

doublereal daw_(real *xx)
{
    /* Initialized data */

    static real zero = 0.f;
    static real p1[10] = { -2.6902039878870478241e-12f,
	    4.18572065374337710778e-10f,-1.34848304455939419963e-8f,
	    9.28264872583444852976e-7f,-1.23877783329049120592e-5f,
	    4.07205792429155826266e-4f,-.00284388121441008500446f,
	    .0470139022887204722217f,-.138868086253931995101f,
	    1.00000000000000000004f };
    static real q1[10] = { 1.71257170854690554214e-10f,
	    1.19266846372297253797e-8f,4.32287827678631772231e-7f,
	    1.03867633767414421898e-5f,1.7891096528424624934e-4f,
	    .00226061077235076703171f,.0207422774641447644725f,
	    .132212955897210128811f,.527798580412734677256f,1.f };
    static real p2[10] = { -1.7095380470085549493f,-37.9258977271042880786f,
	    26.1935631268825992835f,12.5808703738951251885f,
	    -22.7571829525075891337f,4.56604250725163310122f,
	    -7.3308008989640287075f,46.5842087940015295573f,
	    -17.3717177843672791149f,.500260183622027967838f };
    static real q2[9] = { 1.82180093313514478378f,1100.67081034515532891f,
	    -7.08465686676573000364f,453.642111102577727153f,
	    40.6209742218935689922f,302.890110610122663923f,
	    170.641269745236227356f,951.190923960381458747f,
	    .206522691539642105009f };
    static real p3[10] = { -4.55169503255094815112f,-18.6647123338493852582f,
	    -7.36315669126830526754f,-66.8407240337696756838f,
	    48.450726508149145213f,26.9790586735467649969f,
	    -33.5044149820592449072f,7.50964459838919612289f,
	    -1.48432341823343965307f,.499999810924858824981f };
    static real q3[9] = { 44.7820908025971749852f,99.8607198039452081913f,
	    14.0238373126149385228f,3488.17758822286353588f,
	    -9.18871385293215873406f,1240.18500009917163023f,
	    -68.8024952504512254535f,-2.3125157538514514307f,
	    .250041492369922381761f };
    static real p4[10] = { -8.11753647558432685797f,-38.404388247745445343f,
	    -22.3787669028751886675f,-28.8301992467056105854f,
	    -5.99085540418222002197f,-11.3867365736066102577f,
	    -6.5282872752698074159f,-4.50002293000355585708f,
	    -2.50000000088955834952f,.5000000000000004884f };
    static real q4[9] = { 269.382300417238816428f,50.4198958742465752861f,
	    61.1539671480115846173f,208.210246935564547889f,
	    19.7325365692316183531f,-12.2097010558934838708f,
	    -6.99732735041547247161f,-2.49999970104184464568f,
	    .749999999999027092188f };
    static real half = .5f;
    static real one = 1.f;
    static real six25 = 6.25f;
    static real one225 = 12.25f;
    static real two5 = 25.f;
    static real xsmall = 2.44e-4f;
    static real xlarge = 4100.f;
    static real xmax = 4.25e37f;

    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer i__;
    static real x, y, w2, frac, sump, sumq;

/* ---------------------------------------------------------------------- */

/* This function program evaluates Dawson's integral, */

/*                       2  / x   2 */
/*                     -x   |    t */
/*             F(x) = e     |   e    dt */
/*                          | */
/*                          / 0 */

/*   for a real argument x. */

/*   The calling sequence for this function is */

/*                   Y=DAW(X) */

/*   The main computation uses rational Chebyshev approximations */
/*   published in Math. Comp. 24, 171-178 (1970) by Cody, Paciorek */
/*   and Thacher.  This transportable program is patterned after the */
/*   machine-dependent FUNPACK program DDAW(X), but cannot match that */
/*   version for efficiency or accuracy.  This version uses rational */
/*   approximations that are theoretically accurate to about 19 */
/*   significant decimal digits.  The accuracy achieved depends on the */
/*   arithmetic system, the compiler, the intrinsic functions, and */
/*   proper selection of the machine-dependent constants. */

/* ******************************************************************* */
/* ******************************************************************* */

/* Explanation of machine-dependent constants */

/*   XINF   = largest positive machine number */
/*   XMIN   = the smallest positive machine number. */
/*   EPS    = smallest positive number such that 1+eps > 1. */
/*            Approximately  beta**(-p), where beta is the machine */
/*            radix and p is the number of significant base-beta */
/*            digits in a floating-point number. */
/*   XMAX   = absolute argument beyond which DAW(X) underflows. */
/*            XMAX = min(0.5/xmin, xinf). */
/*   XSMALL = absolute argument below DAW(X)  may be represented */
/*            by X.  We recommend XSMALL = sqrt(eps). */
/*   XLARGE = argument beyond which DAW(X) may be represented by */
/*            1/(2x).  We recommend XLARGE = 1/sqrt(eps). */

/*     Approximate values for some important machines are */

/*                        beta  p     eps     xmin       xinf */

/*  CDC 7600      (S.P.)    2  48  7.11E-15  3.14E-294  1.26E+322 */
/*  CRAY-1        (S.P.)    2  48  7.11E-15  4.58E-2467 5.45E+2465 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)    2  24  1.19E-07  1.18E-38   3.40E+38 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)    2  53  1.11D-16  2.23E-308  1.79D+308 */
/*  IBM 3033      (D.P.)   16  14  1.11D-16  5.40D-79   7.23D+75 */
/*  VAX 11/780    (S.P.)    2  24  5.96E-08  2.94E-39   1.70E+38 */
/*                (D.P.)    2  56  1.39D-17  2.94D-39   1.70D+38 */
/*   (G Format)   (D.P.)    2  53  1.11D-16  5.57D-309  8.98D+307 */

/*                         XSMALL     XLARGE     XMAX */

/*  CDC 7600      (S.P.)  5.96E-08   1.68E+07  1.59E+293 */
/*  CRAY-1        (S.P.)  5.96E-08   1.68E+07  5.65E+2465 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)  2.44E-04   4.10E+03  4.25E+37 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)  1.05E-08   9.49E+07  2.24E+307 */
/*  IBM 3033      (D.P.)  3.73D-09   2.68E+08  7.23E+75 */
/*  VAX 11/780    (S.P.)  2.44E-04   4.10E+03  1.70E+38 */
/*                (D.P.)  3.73E-09   2.68E+08  1.70E+38 */
/*   (G Format)   (D.P.)  1.05E-08   9.49E+07  8.98E+307 */

/* ******************************************************************* */
/* ******************************************************************* */

/* Error Returns */

/*  The program returns 0.0 for |X| > XMAX. */

/* Intrinsic functions required are: */

/*     ABS */


/*  Author: W. J. Cody */
/*          Mathematics and Computer Science Division */
/*          Argonne National Laboratory */
/*          Argonne, IL 60439 */

/*  Latest modification: June 15, 1988 */

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Mathematical constants. */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Machine-dependent constants */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for R(9,9) approximation for  |x| < 2.5 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for R(9,9) approximation in J-fraction form */
/*     for  x in [2.5, 3.5) */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for R(9,9) approximation in J-fraction form */
/*     for  x in [3.5, 5.0] */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for R(9,9) approximation in J-fraction form */
/*     for  |x| > 5.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
    x = *xx;
    if (dabs(x) > xlarge) {
	if (dabs(x) <= xmax) {
	    ret_val = half / x;
	} else {
	    ret_val = zero;
	}
    } else if (dabs(x) < xsmall) {
	ret_val = x;
    } else {
	y = x * x;
	if (y < six25) {
/* ---------------------------------------------------------------------- */
/*  ABS(X) .LT. 2.5 */
/* ---------------------------------------------------------------------- */
	    sump = p1[0];
	    sumq = q1[0];
	    for (i__ = 2; i__ <= 10; ++i__) {
		sump = sump * y + p1[i__ - 1];
		sumq = sumq * y + q1[i__ - 1];
/* L100: */
	    }
	    ret_val = x * sump / sumq;
	} else if (y < one225) {
/* ---------------------------------------------------------------------- */
/*  2.5 .LE. ABS(X) .LT. 3.5 */
/* ---------------------------------------------------------------------- */
	    frac = zero;
	    for (i__ = 1; i__ <= 9; ++i__) {
/* L200: */
		frac = q2[i__ - 1] / (p2[i__ - 1] + y + frac);
	    }
	    ret_val = (p2[9] + frac) / x;
	} else if (y < two5) {
/* ---------------------------------------------------------------------- */
/*  3.5 .LE. ABS(X) .LT. 5.0 */
/* --------------------------------------------------------------------- */
	    frac = zero;
	    for (i__ = 1; i__ <= 9; ++i__) {
/* L300: */
		frac = q3[i__ - 1] / (p3[i__ - 1] + y + frac);
	    }
	    ret_val = (p3[9] + frac) / x;
	} else {
/* ---------------------------------------------------------------------- */
/*  5.0 .LE. ABS(X) .LE. XLARGE */
/* ------------------------------------------------------------------ */
	    w2 = one / x / x;
	    frac = zero;
	    for (i__ = 1; i__ <= 9; ++i__) {
/* L400: */
		frac = q4[i__ - 1] / (p4[i__ - 1] + y + frac);
	    }
	    frac = p4[9] + frac;
	    ret_val = (half + half * w2 * frac) / x;
	}
    }
    return ret_val;
/* ---------- Last line of DAW ---------- */
} /* daw_ */

