/* ei.f -- translated by f2c (version 20031025).
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

/* Subroutine */ int calcei_(real *arg, real *result, integer *int__)
{
    /* Initialized data */

    static real zero = 0.f;
    static real two4 = 24.f;
    static real fourty = 40.f;
    static real exp40 = 235385266837019985.41f;
    static real x01 = 381.5f;
    static real x11 = 1024.f;
    static real x02 = -5.1182968633365538008e-5f;
    static real x0 = .37250741078136663466f;
    static real xinf = 3.4e38f;
    static real xmax = 93.246f;
    static real xbig = 82.93f;
    static real p037 = .037f;
    static real a[7] = { 116.69552669734461083368f,2150.0672908092918123209f,
	    15924.175980637303639884f,89904.972007457256553251f,
	    150260.59476436982420737f,-148151.02102575750838086f,
	    5.019678518543984379102f };
    static real b[6] = { 40.205465640027706061433f,750.43163907103936624165f,
	    8125.8035174768735759855f,52440.529172056355429883f,
	    184340.70063353677359298f,256664.93484897117319268f };
    static real c__[9] = { .3828573121022477169108f,11.07326627786831743809f,
	    72.46689782858597021199f,170.0632978311516129328f,
	    169.8106763764238382705f,76.33628843705946890896f,
	    14.87967702840464066613f,.9999989642347613068437f,
	    1.737331760720576030932e-8f };
    static real d__[9] = { .08258160008564488034698f,4.34483633550928208336f,
	    46.62179610356861756812f,177.5728186717289799677f,
	    295.3136335677908517423f,234.2573504717625153053f,
	    90.21658450529372642314f,15.87964570758947927903f,1.f };
    static real e[10] = { 132.76881505637444622987f,35846.198743996904308695f,
	    172833.75773777593926828f,261814.54937205639647381f,
	    175032.73087497081314708f,59346.841538837119172356f,
	    10816.852399095915622498f,1061.1777263550331766871f,
	    52.199632588522572481039f,.99999999999999999087819f };
    static real f[10] = { 39147.856245556345627078f,259897.62083608489777411f,
	    559037.5621002286400338f,546168.42050691155735758f,
	    278581.34710520842139357f,79231.787945279043698718f,
	    12842.808586627297365998f,1163.5769915320848035459f,
	    54.199632588522559414924f,1.f };
    static real plg[4] = { -24.562334077563243311f,236.42701335621505212f,
	    -549.89956895857911039f,356.87548468071500413f };
    static real qlg[4] = { -35.553900764052419184f,194.00230218539473193f,
	    -334.42903192607538956f,178.43774234035750207f };
    static real p[10] = { -12.96370260247483002859f,
	    -1283.1220659262000678155f,-14287.072500197005777376f,
	    -1429984.1572091610380064f,-313986.6086424726586205f,
	    -353778096.944311334848f,319843542.35237738511048f,
	    -25301823984.599019348858f,12177698136.19959467758f,
	    -208290406668.0249712094f };
    static real q[10] = { 76.88671875f,-5564.8470543369082846819f,
	    194184.69440759880361415f,-4264843.4812177161405483f,
	    64698830.956576428587653f,-701085687.74215954065376f,
	    5422961798.4472955011862f,-28986272696.554495342658f,
	    98900934262.481749439886f,-89673749185.755048616855f };
    static real half = .5f;
    static real r__[10] = { -2.645677793077147237806f,
	    -2.378372882815725244124f,-24.2110695698065351155f,
	    10.52976392459015155422f,19.45603779539281810439f,
	    -30.15761863840593359165f,11.20011024227297451523f,
	    -3.988850730390541057912f,9.565134591978630774217f,
	    .9981193787537396413219f };
    static real s[9] = { 1.598517957704779356479e-4f,4.64418593258328694265f,
	    369.7412299772985940785f,-8.791401054875438925029f,
	    760.8194509086645763123f,28.52397548119248700147f,
	    473.1097187816050252967f,-236.9210235636181001661f,
	    1.24988482271244789144f };
    static real p1[10] = { -1.647721172463463140042f,
	    -18.60092121726437582253f,-10.00641913989284829961f,
	    -21.05740799548040450394f,-.9134835699998742552432f,
	    -33.23612579343962284333f,24.95487730402059440626f,
	    26.52575818452799819855f,-1.845086232391278674524f,
	    .9999933106160568739091f };
    static real q1[9] = { 97.9240359921729029684f,64.03800405352415551324f,
	    59.94932325667407355255f,253.8819315630708031713f,
	    44.29413178337928401161f,1192.832423968601006985f,
	    199.1004470817742470726f,-10.93556195391091143924f,
	    1.001533852045342697818f };
    static real p2[10] = { 175.33880126546597239f,-223.12767077763240955f,
	    -18.1949664929868906455f,-27.979852862430538934f,
	    -7.63147701620253630855f,-15.2856623636929636839f,
	    -7.06810977895029358836f,-5.00006640413131002475f,
	    -3.00000000320981265753f,1.00000000000000485503f };
    static real q2[9] = { 39784.597716741472084f,3.97277109100414518365f,
	    137.790390235747998793f,117.179220502086455287f,
	    70.4831847180424675988f,-12.0187763547154743238f,
	    -7.99243595776339741065f,-2.99999894040324959612f,
	    1.99999999999048104167f };
    static real one = 1.f;
    static real two = 2.f;
    static real three = 3.f;
    static real four = 4.f;
    static real six = 6.f;
    static real twelve = 12.f;

    /* Builtin functions */
    double log(doublereal), exp(doublereal);

    /* Local variables */
    static integer i__;
    static real t, w, x, y, ei, px[10], qx[10], ysq, xmx0, frac, sump, sumq;

/* ---------------------------------------------------------------------- */

/* This Fortran 77 packet computes the exponential integrals Ei(x), */
/*  E1(x), and  exp(-x)*Ei(x)  for real arguments  x  where */

/*           integral (from t=-infinity to t=x) (exp(t)/t),  x > 0, */
/*  Ei(x) = */
/*          -integral (from t=-x to t=infinity) (exp(t)/t),  x < 0, */

/*  and where the first integral is a principal value integral. */
/*  The packet contains three function type subprograms: EI, EONE, */
/*  and EXPEI;  and one subroutine type subprogram: CALCEI.  The */
/*  calling statements for the primary entries are */

/*                 Y = EI(X),            where  X .NE. 0, */

/*                 Y = EONE(X),          where  X .GT. 0, */
/*  and */
/*                 Y = EXPEI(X),         where  X .NE. 0, */

/*  and where the entry points correspond to the functions Ei(x), */
/*  E1(x), and exp(-x)*Ei(x), respectively.  The routine CALCEI */
/*  is intended for internal packet use only, all computations within */
/*  the packet being concentrated in this routine.  The function */
/*  subprograms invoke CALCEI with the Fortran statement */
/*         CALL CALCEI(ARG,RESULT,INT) */
/*  where the parameter usage is as follows */

/*     Function                  Parameters for CALCEI */
/*       Call                 ARG             RESULT         INT */

/*      EI(X)              X .NE. 0          Ei(X)            1 */
/*      EONE(X)            X .GT. 0         -Ei(-X)           2 */
/*      EXPEI(X)           X .NE. 0          exp(-X)*Ei(X)    3 */

/*  The main computation involves evaluation of rational Chebyshev */
/*  approximations published in Math. Comp. 22, 641-649 (1968), and */
/*  Math. Comp. 23, 289-303 (1969) by Cody and Thacher.  This */
/*  transportable program is patterned after the machine-dependent */
/*  FUNPACK packet  NATSEI,  but cannot match that version for */
/*  efficiency or accuracy.  This version uses rational functions */
/*  that theoretically approximate the exponential integrals to */
/*  at least 18 significant decimal digits.  The accuracy achieved */
/*  depends on the arithmetic system, the compiler, the intrinsic */
/*  functions, and proper selection of the machine-dependent */
/*  constants. */


/* ******************************************************************* */
/* ******************************************************************* */

/* Explanation of machine-dependent constants */

/*   beta = radix for the floating-point system. */
/*   minexp = smallest representable power of beta. */
/*   maxexp = smallest power of beta that overflows. */
/*   XBIG = largest argument acceptable to EONE; solution to */
/*          equation: */
/*                     exp(-x)/x * (1 + 1/x) = beta ** minexp. */
/*   XINF = largest positive machine number; approximately */
/*                     beta ** maxexp */
/*   XMAX = largest argument acceptable to EI; solution to */
/*          equation:  exp(x)/x * (1 + 1/x) = beta ** maxexp. */

/*     Approximate values for some important machines are: */

/*                           beta      minexp      maxexp */

/*  CRAY-1        (S.P.)       2       -8193        8191 */
/*  Cyber 180/185 */
/*    under NOS   (S.P.)       2        -975        1070 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)       2        -126         128 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)       2       -1022        1024 */
/*  IBM 3033      (D.P.)      16         -65          63 */
/*  VAX D-Format  (D.P.)       2        -128         127 */
/*  VAX G-Format  (D.P.)       2       -1024        1023 */

/*                           XBIG       XINF       XMAX */

/*  CRAY-1        (S.P.)    5670.31  5.45E+2465   5686.21 */
/*  Cyber 180/185 */
/*    under NOS   (S.P.)     669.31  1.26E+322     748.28 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (S.P.)      82.93  3.40E+38       93.24 */
/*  IEEE (IBM/XT, */
/*    SUN, etc.)  (D.P.)     701.84  1.79D+308     716.35 */
/*  IBM 3033      (D.P.)     175.05  7.23D+75      179.85 */
/*  VAX D-Format  (D.P.)      84.30  1.70D+38       92.54 */
/*  VAX G-Format  (D.P.)     703.22  8.98D+307     715.66 */

/* ******************************************************************* */
/* ******************************************************************* */

/* Error returns */

/*  The following table shows the types of error that may be */
/*  encountered in this routine and the function value supplied */
/*  in each case. */

/*       Error       Argument         Function values for */
/*                    Range         EI      EXPEI     EONE */

/*     UNDERFLOW  (-)X .GT. XBIG     0        -         0 */
/*     OVERFLOW      X .GE. XMAX    XINF      -         - */
/*     ILLEGAL X       X = 0       -XINF    -XINF     XINF */
/*     ILLEGAL X      X .LT. 0       -        -     USE ABS(X) */

/* Intrinsic functions required are: */

/*     ABS, SQRT, EXP */


/*  Author: W. J. Cody */
/*          Mathematics abd Computer Science Division */
/*          Argonne National Laboratory */
/*          Argonne, IL 60439 */

/*  Latest modification: September 9, 1988 */

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Mathematical constants */
/*   EXP40 = exp(40) */
/*   X0 = zero of Ei */
/*   X01/X11 + X02 = zero of Ei to extra precision */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* Machine-dependent constants */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* Coefficients  for -1.0 <= X < 0.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* Coefficients for -4.0 <= X < -1.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* Coefficients for X < -4.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*  Coefficients for rational approximation to ln(x/a), |1-x/a| < .1 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* Coefficients for  0.0 < X < 6.0, */
/*  ratio of Chebyshev polynomials */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* J-fraction coefficients for 6.0 <= X < 12.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* J-fraction coefficients for 12.0 <= X < 24.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* J-fraction coefficients for  X .GE. 24.0 */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
    x = *arg;
    if (x == zero) {
	ei = -xinf;
	if (*int__ == 2) {
	    ei = -ei;
	}
    } else if (x < zero || *int__ == 2) {
/* ---------------------------------------------------------------------- */
/* Calculate EI for negative argument or for E1. */
/* ---------------------------------------------------------------------- */
	y = dabs(x);
	if (y <= one) {
	    sump = a[6] * y + a[0];
	    sumq = y + b[0];
	    for (i__ = 2; i__ <= 6; ++i__) {
		sump = sump * y + a[i__ - 1];
		sumq = sumq * y + b[i__ - 1];
/* L110: */
	    }
	    ei = log(y) - sump / sumq;
	    if (*int__ == 3) {
		ei *= exp(y);
	    }
	} else if (y <= four) {
	    w = one / y;
	    sump = c__[0];
	    sumq = d__[0];
	    for (i__ = 2; i__ <= 9; ++i__) {
		sump = sump * w + c__[i__ - 1];
		sumq = sumq * w + d__[i__ - 1];
/* L130: */
	    }
	    ei = -sump / sumq;
	    if (*int__ != 3) {
		ei *= exp(-y);
	    }
	} else {
	    if (y > xbig && *int__ < 3) {
		ei = zero;
	    } else {
		w = one / y;
		sump = e[0];
		sumq = f[0];
		for (i__ = 2; i__ <= 10; ++i__) {
		    sump = sump * w + e[i__ - 1];
		    sumq = sumq * w + f[i__ - 1];
/* L150: */
		}
		ei = -w * (one - w * sump / sumq);
		if (*int__ != 3) {
		    ei *= exp(-y);
		}
	    }
	}
	if (*int__ == 2) {
	    ei = -ei;
	}
    } else if (x < six) {
/* ---------------------------------------------------------------------- */
/*  To improve conditioning, rational approximations are expressed */
/*    in terms of Chebyshev polynomials for 0 <= X < 6, and in */
/*    continued fraction form for larger X. */
/* ---------------------------------------------------------------------- */
	t = x + x;
	t = t / three - two;
	px[0] = zero;
	qx[0] = zero;
	px[1] = p[0];
	qx[1] = q[0];
	for (i__ = 2; i__ <= 9; ++i__) {
	    px[i__] = t * px[i__ - 1] - px[i__ - 2] + p[i__ - 1];
	    qx[i__] = t * qx[i__ - 1] - qx[i__ - 2] + q[i__ - 1];
/* L210: */
	}
	sump = half * t * px[9] - px[8] + p[9];
	sumq = half * t * qx[9] - qx[8] + q[9];
	frac = sump / sumq;
	xmx0 = x - x01 / x11 - x02;
	if (dabs(xmx0) >= p037) {
	    ei = log(x / x0) + xmx0 * frac;
	    if (*int__ == 3) {
		ei = exp(-x) * ei;
	    }
	} else {
/* ---------------------------------------------------------------------- */
/* Special approximation to  ln(X/X0)  for X close to X0 */
/* ---------------------------------------------------------------------- */
	    y = xmx0 / (x + x0);
	    ysq = y * y;
	    sump = plg[0];
	    sumq = ysq + qlg[0];
	    for (i__ = 2; i__ <= 4; ++i__) {
		sump = sump * ysq + plg[i__ - 1];
		sumq = sumq * ysq + qlg[i__ - 1];
/* L220: */
	    }
	    ei = (sump / (sumq * (x + x0)) + frac) * xmx0;
	    if (*int__ == 3) {
		ei = exp(-x) * ei;
	    }
	}
    } else if (x < twelve) {
	frac = zero;
	for (i__ = 1; i__ <= 9; ++i__) {
	    frac = s[i__ - 1] / (r__[i__ - 1] + x + frac);
/* L230: */
	}
	ei = (r__[9] + frac) / x;
	if (*int__ != 3) {
	    ei *= exp(x);
	}
    } else if (x <= two4) {
	frac = zero;
	for (i__ = 1; i__ <= 9; ++i__) {
	    frac = q1[i__ - 1] / (p1[i__ - 1] + x + frac);
/* L240: */
	}
	ei = (p1[9] + frac) / x;
	if (*int__ != 3) {
	    ei *= exp(x);
	}
    } else {
	if (x >= xmax && *int__ < 3) {
	    ei = xinf;
	} else {
	    y = one / x;
	    frac = zero;
	    for (i__ = 1; i__ <= 9; ++i__) {
		frac = q2[i__ - 1] / (p2[i__ - 1] + x + frac);
/* L250: */
	    }
	    frac = p2[9] + frac;
	    ei = y + y * y * frac;
	    if (*int__ != 3) {
		if (x <= xmax - two4) {
		    ei *= exp(x);
		} else {
/* ---------------------------------------------------------------------- */
/* Calculation reformulated to avoid premature overflow */
/* ---------------------------------------------------------------------- */
		    ei = ei * exp(x - fourty) * exp40;
		}
	    }
	}
    }
    *result = ei;
    return 0;
/* ---------- Last line of CALCEI ---------- */
} /* calcei_ */

doublereal ei_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer int__;
    extern /* Subroutine */ int calcei_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This function program computes approximate values for the */
/*   exponential integral  Ei(x), where  x  is real. */

/*  Author: W. J. Cody */

/*  Latest modification: January 12, 1988 */

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
    int__ = 1;
    calcei_(x, &result, &int__);
    ret_val = result;
    return ret_val;
/* ---------- Last line of EI ---------- */
} /* ei_ */

doublereal expei_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer int__;
    extern /* Subroutine */ int calcei_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This function program computes approximate values for the */
/*   function  exp(-x) * Ei(x), where  Ei(x)  is the exponential */
/*   integral, and  x  is real. */

/*  Author: W. J. Cody */

/*  Latest modification: January 12, 1988 */

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
    int__ = 3;
    calcei_(x, &result, &int__);
    ret_val = result;
    return ret_val;
/* ---------- Last line of EXPEI ---------- */
} /* expei_ */

doublereal eone_(real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static integer int__;
    extern /* Subroutine */ int calcei_(real *, real *, integer *);
    static real result;

/* -------------------------------------------------------------------- */

/* This function program computes approximate values for the */
/*   exponential integral E1(x), where  x  is real. */

/*  Author: W. J. Cody */

/*  Latest modification: January 12, 1988 */

/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
    int__ = 2;
    calcei_(x, &result, &int__);
    ret_val = result;
    return ret_val;
/* ---------- Last line of EONE ---------- */
} /* eone_ */

