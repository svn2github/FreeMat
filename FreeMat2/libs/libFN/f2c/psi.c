/* ../psi.f -- translated by f2c (version 20031025).
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

doublereal psi_(real *xx)
{
    /* Initialized data */

    static real zero = 0.f;
    static real xmax1 = 8.38e6f;
    static real xsmall = 1.35e-4f;
    static real xlarge = 1.2e6f;
    static real x01 = 187.f;
    static real x01d = 128.f;
    static real x02 = 6.9464496836234126266e-4f;
    static real p1[9] = { .004510468124576293416f,5.4932855833000385356f,
	    376.46693175929276856f,7952.5490849151998065f,
	    71451.59581895193321f,306559.76301987365674f,
	    636069.97788964458797f,580413.12783537569993f,
	    165856.95029761022321f };
    static real q1[8] = { 96.141654774222358525f,2628.771579058119333f,
	    29862.49702225027792f,162065.66091533671639f,
	    434878.80712768329037f,542563.84537269993733f,
	    242421.85002017985252f,6.4155223783576225996e-8f };
    static real p2[7] = { -2.7103228277757834192f,-15.166271776896121383f,
	    -19.784554148719218667f,-8.8100958828312219821f,
	    -1.4479614616899842986f,-.073689600332394549911f,
	    -6.5135387732718171306e-21f };
    static real q2[6] = { 44.992760373789365846f,202.40955312679931159f,
	    247.36979003315290057f,107.42543875702278326f,
	    17.463965060678569906f,.88427520398873480342f };
    static real fourth = .25f;
    static real half = .5f;
    static real one = 1.f;
    static real three = 3.f;
    static real four = 4.f;
    static real piov4 = .78539816339744830962f;
    static real xinf = 1.7e38f;
    static real xmin1 = 5.89e-39f;

    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double r_int(real *), tan(doublereal), log(doublereal);

    /* Local variables */
    static integer i__, n;
    static real w, x, z__;
    static integer nq;
    static real den, aug, sgn, upper;

/* ---------------------------------------------------------------------- */

/* This function program evaluates the logarithmic derivative of the */
/*   gamma function, */

/*      psi(x) = d/dx (gamma(x)) / gamma(x) = d/dx (ln gamma(x)) */

/*   for real x, where either */

/*          -xmax1 < x < -xmin (x not a negative integer), or */
/*            xmin < x. */

/*   The calling sequence for this function is */

/*                  Y = PSI(X) */

/*   The main computation uses rational Chebyshev approximations */
/*   published in Math. Comp. 27, 123-127 (1973) by Cody, Strecok and */
/*   Thacher.  This transportable program is patterned after the */
/*   machine-dependent FUNPACK program PSI(X), but cannot match that */
/*   version for efficiency or accuracy.  This version uses rational */
/*   approximations that are theoretically accurate to 20 significant */
/*   decimal digits.  The accuracy achieved depends on the arithmetic */
/*   system, the compiler, the intrinsic functions, and proper selection */
/*   of the machine-dependent constants. */

/* ******************************************************************* */
/* ******************************************************************* */

/* Explanation of machine-dependent constants */

/*   XINF   = largest positive machine number */
/*   XMAX1  = beta ** (p-1), where beta is the radix for the */
/*            floating-point system, and p is the number of base-beta */
/*            digits in the floating-point significand.  This is an */
/*            upper bound on non-integral floating-point numbers, and */
/*            the negative of the lower bound on acceptable negative */
/*            arguments for PSI.  If rounding is necessary, round this */
/*            value down. */
/*   XMIN1  = the smallest in magnitude acceptable argument.  We */
/*            recommend XMIN1 = MAX(1/XINF,xmin) rounded up, where */
/*            xmin is the smallest positive floating-point number. */
/*   XSMALL = absolute argument below which  PI*COTAN(PI*X)  may be */
/*            represented by 1/X.  We recommend XSMALL < sqrt(3 eps)/pi, */
/*            where eps is the smallest positive number such that */
/*            1+eps > 1. */
/*   XLARGE = argument beyond which PSI(X) may be represented by */
/*            LOG(X).  The solution to the equation */
/*               x*ln(x) = beta ** p */
/*            is a safe value. */

/*     Approximate values for some important machines are */

/*                        beta  p     eps     xmin       XINF */

/*  CDC 7600      (S.P.)    2  48  7.11E-15  3.13E-294  1.26E+322 */
/*  CRAY-1        (S.P.)    2  48  7.11E-15  4.58E-2467 5.45E+2465 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)    2  24  1.19E-07  1.18E-38   3.40E+38 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)    2  53  1.11D-16  2.23E-308  1.79D+308 */
/*  IBM 3033      (D.P.)   16  14  1.11D-16  5.40D-79   7.23D+75 */
/*  SUN 3/160     (D.P.)    2  53  1.11D-16  2.23D-308  1.79D+308 */
/*  VAX 11/780    (S.P.)    2  24  5.96E-08  2.94E-39   1.70E+38 */
/*                (D.P.)    2  56  1.39D-17  2.94D-39   1.70D+38 */
/*   (G Format)   (D.P.)    2  53  1.11D-16  5.57D-309  8.98D+307 */

/*                         XMIN1      XMAX1     XSMALL    XLARGE */

/*  CDC 7600      (S.P.)  3.13E-294  1.40E+14  4.64E-08  9.42E+12 */
/*  CRAY-1        (S.P.)  1.84E-2466 1.40E+14  4.64E-08  9.42E+12 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)  1.18E-38   8.38E+06  1.90E-04  1.20E+06 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)  2.23D-308  4.50D+15  5.80D-09  2.71D+14 */
/*  IBM 3033      (D.P.)  1.39D-76   4.50D+15  5.80D-09  2.05D+15 */
/*  SUN 3/160     (D.P.)  2.23D-308  4.50D+15  5.80D-09  2.71D+14 */
/*  VAX 11/780    (S.P.)  5.89E-39   8.38E+06  1.35E-04  1.20E+06 */
/*                (D.P.)  5.89D-39   3.60D+16  2.05D-09  2.05D+15 */
/*   (G Format)   (D.P.)  1.12D-308  4.50D+15  5.80D-09  2.71D+14 */

/* ******************************************************************* */
/* ******************************************************************* */

/* Error Returns */

/*  The program returns XINF for  X < -XMAX1, for X zero or a negative */
/*    integer, or when X lies in (-XMIN1, 0), and returns -XINF */
/*    when X lies in (0, XMIN1). */

/* Intrinsic functions required are: */

/*     ABS, AINT, DBLE, INT, LOG, REAL, TAN */


/*  Author: W. J. Cody */
/*          Mathematics and Computer Science Division */
/*          Argonne National Laboratory */
/*          Argonne, IL 60439 */

/*  Latest modification: June 8, 1988 */

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Mathematical constants.  PIOV4 = pi / 4 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Machine-dependent constants */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Zero of psi(x) */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for approximation to  psi(x)/(x-x0)  over [0.5, 3.0] */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for approximation to  psi(x) - ln(x) + 1/(2x) */
/*     for  x > 3.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
    x = *xx;
    w = dabs(x);
    aug = zero;
/* ---------------------------------------------------------------------- */
/*  Check for valid arguments, then branch to appropriate algorithm */
/* ---------------------------------------------------------------------- */
    if (-x >= xmax1 || w < xmin1) {
	goto L410;
    } else if (x >= half) {
	goto L200;
/* ---------------------------------------------------------------------- */
/*  X < 0.5, use reflection formula: psi(1-x) = psi(x) + pi * cot(pi*x) */
/*     Use 1/X for PI*COTAN(PI*X)  when  XMIN1 < |X| <= XSMALL. */
/* ---------------------------------------------------------------------- */
    } else if (w <= xsmall) {
	aug = -one / x;
	goto L150;
    }
/* ---------------------------------------------------------------------- */
/*  Argument reduction for cot */
/* ---------------------------------------------------------------------- */
/* L100: */
    if (x < zero) {
	sgn = piov4;
    } else {
	sgn = -piov4;
    }
    w -= r_int(&w);
    nq = (integer) (w * four);
    w = four * (w - (real) nq * fourth);
/* ---------------------------------------------------------------------- */
/*  W is now related to the fractional part of  4.0 * X. */
/*     Adjust argument to correspond to values in the first */
/*     quadrant and determine the sign. */
/* ---------------------------------------------------------------------- */
    n = nq / 2;
    if (n + n != nq) {
	w = one - w;
    }
    z__ = piov4 * w;
    if (n % 2 != 0) {
	sgn = -sgn;
    }
/* ---------------------------------------------------------------------- */
/*  determine the final value for  -pi * cotan(pi*x) */
/* ---------------------------------------------------------------------- */
    n = (nq + 1) / 2;
    if (n % 2 == 0) {
/* ---------------------------------------------------------------------- */
/*  Check for singularity */
/* ---------------------------------------------------------------------- */
	if (z__ == zero) {
	    goto L410;
	}
	aug = sgn * (four / tan(z__));
    } else {
	aug = sgn * (four * tan(z__));
    }
L150:
    x = one - x;
L200:
    if (x > three) {
	goto L300;
    }
/* ---------------------------------------------------------------------- */
/*  0.5 <= X <= 3.0 */
/* ---------------------------------------------------------------------- */
    den = x;
    upper = p1[0] * x;
    for (i__ = 1; i__ <= 7; ++i__) {
	den = (den + q1[i__ - 1]) * x;
	upper = (upper + p1[i__]) * x;
/* L210: */
    }
    den = (upper + p1[8]) / (den + q1[7]);
    x = x - x01 / x01d - x02;
    ret_val = den * x + aug;
    goto L500;
/* ---------------------------------------------------------------------- */
/*  3.0 < X */
/* ---------------------------------------------------------------------- */
L300:
    if (x < xlarge) {
	w = one / (x * x);
	den = w;
	upper = p2[0] * w;
	for (i__ = 1; i__ <= 5; ++i__) {
	    den = (den + q2[i__ - 1]) * w;
	    upper = (upper + p2[i__]) * w;
/* L310: */
	}
	aug = (upper + p2[6]) / (den + q2[5]) - half / x + aug;
    }
    ret_val = aug + log(x);
    goto L500;
/* ---------------------------------------------------------------------- */
/*  Error return */
/* ---------------------------------------------------------------------- */
L410:
    ret_val = xinf;
    if (x > zero) {
	ret_val = -xinf;
    }
L500:
    return ret_val;
/* ---------- Last card of PSI ---------- */
} /* psi_ */

