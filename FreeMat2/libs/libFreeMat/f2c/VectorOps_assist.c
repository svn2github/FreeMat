/* VectorOps_assist.f -- translated by f2c (version 20030211).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Copyright (c) 2002, 2003 Samit Basu */

/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */

/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS */
/* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/* ************************************************************************** */
/* Vector Add functions */
/* ************************************************************************** */
/* Add two int32 vectors */
/* Subroutine */ int int32add_(integer *n, integer *c__, integer *a, integer *
	stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] + b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32add_ */

/* Add two float vectors */
/* Subroutine */ int floatadd_(integer *n, real *c__, real *a, integer *
	stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] + b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatadd_ */

/* Add two double vectors */
/* Subroutine */ int doubleadd_(integer *n, doublereal *c__, doublereal *a, 
	integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] + b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doubleadd_ */

/* Add two complex vectors */
/* Subroutine */ int complexadd_(integer *n, complex *c__, complex *a, 
	integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    complex q__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	i__4 = p;
	q__1.r = a[i__3].r + b[i__4].r, q__1.i = a[i__3].i + b[i__4].i;
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexadd_ */

/* Add two complex vectors */
/* Subroutine */ int dcomplexadd_(integer *n, doublecomplex *c__, 
	doublecomplex *a, integer *stride1, doublecomplex *b, integer *
	stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublecomplex z__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	i__4 = p;
	z__1.r = a[i__3].r + b[i__4].r, z__1.i = a[i__3].i + b[i__4].i;
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexadd_ */

/* ************************************************************************** */
/* Vector Subtract functions */
/* ************************************************************************** */
/* Subtract two int32 vectors */
/* Subroutine */ int int32subtract_(integer *n, integer *c__, integer *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] - b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32subtract_ */

/* Subtract two float vectors */
/* Subroutine */ int floatsubtract_(integer *n, real *c__, real *a, integer *
	stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] - b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatsubtract_ */

/* Subtract two double vectors */
/* Subroutine */ int doublesubtract_(integer *n, doublereal *c__, doublereal *
	a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] - b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doublesubtract_ */

/* Subtract two complex vectors */
/* Subroutine */ int complexsubtract_(integer *n, complex *c__, complex *a, 
	integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    complex q__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	i__4 = p;
	q__1.r = a[i__3].r - b[i__4].r, q__1.i = a[i__3].i - b[i__4].i;
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexsubtract_ */

/* Subtract two complex vectors */
/* Subroutine */ int dcomplexsubtract_(integer *n, doublecomplex *c__, 
	doublecomplex *a, integer *stride1, doublecomplex *b, integer *
	stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublecomplex z__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	i__4 = p;
	z__1.r = a[i__3].r - b[i__4].r, z__1.i = a[i__3].i - b[i__4].i;
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexsubtract_ */

/* ************************************************************************** */
/* Vector DotMultiply functions */
/* ************************************************************************** */
/* DotMultiply two int32 vectors */
/* Subroutine */ int int32dotmultiply_(integer *n, integer *c__, integer *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] * b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32dotmultiply_ */

/* DotMultiply two float vectors */
/* Subroutine */ int floatdotmultiply_(integer *n, real *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] * b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatdotmultiply_ */

/* DotMultiply two double vectors */
/* Subroutine */ int doubledotmultiply_(integer *n, doublereal *c__, 
	doublereal *a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] * b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doubledotmultiply_ */

/* DotMultiply two complex vectors */
/* Subroutine */ int complexdotmultiply_(integer *n, complex *c__, complex *a,
	 integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    complex q__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	i__4 = p;
	q__1.r = a[i__3].r * b[i__4].r - a[i__3].i * b[i__4].i, q__1.i = a[
		i__3].r * b[i__4].i + a[i__3].i * b[i__4].r;
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexdotmultiply_ */

/* DotMultiply two complex vectors */
/* Subroutine */ int dcomplexdotmultiply_(integer *n, doublecomplex *c__, 
	doublecomplex *a, integer *stride1, doublecomplex *b, integer *
	stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublecomplex z__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	i__4 = p;
	z__1.r = a[i__3].r * b[i__4].r - a[i__3].i * b[i__4].i, z__1.i = a[
		i__3].r * b[i__4].i + a[i__3].i * b[i__4].r;
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexdotmultiply_ */

/* ************************************************************************** */
/* Vector Dotleftdivide functions */
/* ************************************************************************** */
/* Dotleftdivide two int32 vectors */
/* Subroutine */ int int32dotleftdivide_(integer *n, integer *c__, integer *a,
	 integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = b[p] / a[m];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32dotleftdivide_ */

/* Dotleftdivide two float vectors */
/* Subroutine */ int floatdotleftdivide_(integer *n, real *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = b[p] / a[m];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatdotleftdivide_ */

/* Dotleftdivide two double vectors */
/* Subroutine */ int doubledotleftdivide_(integer *n, doublereal *c__, 
	doublereal *a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = b[p] / a[m];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doubledotleftdivide_ */

/* Dotleftdivide two complex vectors */
/* Subroutine */ int complexdotleftdivide_(integer *n, complex *c__, complex *
	a, integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2;
    complex q__1;

    /* Builtin functions */
    void c_div(complex *, complex *, complex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	c_div(&q__1, &b[p], &a[m]);
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexdotleftdivide_ */

/* Dotleftdivide two complex vectors */
/* Subroutine */ int dcomplexdotleftdivide_(integer *n, doublecomplex *c__, 
	doublecomplex *a, integer *stride1, doublecomplex *b, integer *
	stride2)
{
    /* System generated locals */
    integer i__1, i__2;
    doublecomplex z__1;

    /* Builtin functions */
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	z_div(&z__1, &b[p], &a[m]);
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexdotleftdivide_ */

/* ************************************************************************** */
/* Vector Dotrightdivide functions */
/* ************************************************************************** */
/* Dotrightdivide two int32 vectors */
/* Subroutine */ int int32dotrightdivide_(integer *n, integer *c__, integer *
	a, integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] / b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32dotrightdivide_ */

/* Dotrightdivide two float vectors */
/* Subroutine */ int floatdotrightdivide_(integer *n, real *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] / b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatdotrightdivide_ */

/* Dotrightdivide two double vectors */
/* Subroutine */ int doubledotrightdivide_(integer *n, doublereal *c__, 
	doublereal *a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = a[m] / b[p];
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doubledotrightdivide_ */

/* Dotrightdivide two complex vectors */
/* Subroutine */ int complexdotrightdivide_(integer *n, complex *c__, complex 
	*a, integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2;
    complex q__1;

    /* Builtin functions */
    void c_div(complex *, complex *, complex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	c_div(&q__1, &a[m], &b[p]);
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexdotrightdivide_ */

/* Dotrightdivide two complex vectors */
/* Subroutine */ int dcomplexdotrightdivide_(integer *n, doublecomplex *c__, 
	doublecomplex *a, integer *stride1, doublecomplex *b, integer *
	stride2)
{
    /* System generated locals */
    integer i__1, i__2;
    doublecomplex z__1;

    /* Builtin functions */
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	z_div(&z__1, &a[m], &b[p]);
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexdotrightdivide_ */

/* ************************************************************************** */
/* Vector Dotpower functions */
/* ************************************************************************** */
/* Subroutine */ int cicpower_(integer *n, complex *c__, complex *a, integer *
	stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2;
    complex q__1;

    /* Builtin functions */
    void pow_ci(complex *, complex *, integer *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	pow_ci(&q__1, &a[m], &b[p]);
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* cicpower_ */

/* Subroutine */ int cfcpower_(integer *n, complex *c__, complex *a, integer *
	stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    complex q__1;
    doublecomplex z__1, z__2, z__3;

    /* Builtin functions */
    void pow_zz(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	z__2.r = a[i__3].r, z__2.i = a[i__3].i;
	i__4 = p;
	z__3.r = b[i__4], z__3.i = 0.;
	pow_zz(&z__1, &z__2, &z__3);
	q__1.r = z__1.r, q__1.i = z__1.i;
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* cfcpower_ */

/* Subroutine */ int zdzpower_(integer *n, doublecomplex *c__, doublecomplex *
	a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublecomplex z__1, z__2;

    /* Builtin functions */
    void pow_zz(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = p;
	z__2.r = b[i__3], z__2.i = 0.;
	pow_zz(&z__1, &a[m], &z__2);
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* zdzpower_ */

/* Subroutine */ int cccpower_(integer *n, complex *c__, complex *a, integer *
	stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    complex q__1;
    doublecomplex z__1, z__2, z__3;

    /* Builtin functions */
    void pow_zz(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = m;
	z__2.r = a[i__3].r, z__2.i = a[i__3].i;
	i__4 = p;
	z__3.r = b[i__4].r, z__3.i = b[i__4].i;
	pow_zz(&z__1, &z__2, &z__3);
	q__1.r = z__1.r, q__1.i = z__1.i;
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* cccpower_ */

/* Subroutine */ int zzzpower_(integer *n, doublecomplex *c__, doublecomplex *
	a, integer *stride1, doublecomplex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2;
    doublecomplex z__1;

    /* Builtin functions */
    void pow_zz(doublecomplex *, doublecomplex *, doublecomplex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	pow_zz(&z__1, &a[m], &b[p]);
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* zzzpower_ */

/* Subroutine */ int zizpower_(integer *n, doublecomplex *c__, doublecomplex *
	a, integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2;
    doublecomplex z__1;

    /* Builtin functions */
    void pow_zi(doublecomplex *, doublecomplex *, integer *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	pow_zi(&z__1, &a[m], &b[p]);
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* zizpower_ */

/* Subroutine */ int didpower_(integer *n, doublereal *c__, doublereal *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double pow_di(doublereal *, integer *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = pow_di(&a[m], &b[p]);
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* didpower_ */

/* Subroutine */ int dddpower_(integer *n, doublereal *c__, doublereal *a, 
	integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = pow_dd(&a[m], &b[p]);
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dddpower_ */

/* Subroutine */ int fifpower_(integer *n, real *c__, real *a, integer *
	stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double pow_ri(real *, integer *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = pow_ri(&a[m], &b[p]);
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* fifpower_ */

/* Subroutine */ int fffpower_(integer *n, real *c__, real *a, integer *
	stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = (doublereal) a[m];
	d__2 = (doublereal) b[p];
	c__[i__] = pow_dd(&d__1, &d__2);
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* fffpower_ */

/* ************************************************************************** */
/* Vector Negate functions */
/* ************************************************************************** */
/* Negate a int32 vector */
/* Subroutine */ int int32negate_(integer *n, integer *c__, integer *a)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --a;
    --c__;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = -a[i__];
/* L10: */
    }
    return 0;
} /* int32negate_ */

/* Negate a float vector */
/* Subroutine */ int floatnegate_(integer *n, real *c__, real *a)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --a;
    --c__;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = -a[i__];
/* L10: */
    }
    return 0;
} /* floatnegate_ */

/* Negate a double vector */
/* Subroutine */ int doublenegate_(integer *n, doublereal *c__, doublereal *a)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --a;
    --c__;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	c__[i__] = -a[i__];
/* L10: */
    }
    return 0;
} /* doublenegate_ */

/* Negate a complex vector */
/* Subroutine */ int complexnegate_(integer *n, complex *c__, complex *a)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    complex q__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --a;
    --c__;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = i__;
	q__1.r = -a[i__3].r, q__1.i = -a[i__3].i;
	c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
/* L10: */
    }
    return 0;
} /* complexnegate_ */

/* Negate a dcomplex vector */
/* Subroutine */ int dcomplexnegate_(integer *n, doublecomplex *c__, 
	doublecomplex *a)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublecomplex z__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --a;
    --c__;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	i__3 = i__;
	z__1.r = -a[i__3].r, z__1.i = -a[i__3].i;
	c__[i__2].r = z__1.r, c__[i__2].i = z__1.i;
/* L10: */
    }
    return 0;
} /* dcomplexnegate_ */

/* ************************************************************************** */
/* Vector LessThan functions */
/* ************************************************************************** */
/* Compare two int32 vectors */
/* Subroutine */ int int32lessthan_(integer *n, integer1 *c__, integer *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] < b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32lessthan_ */

/* Compare two float vectors */
/* Subroutine */ int floatlessthan_(integer *n, integer1 *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] < b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatlessthan_ */

/* Compare two double vectors */
/* Subroutine */ int doublelessthan_(integer *n, integer1 *c__, doublereal *a,
	 integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] < b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doublelessthan_ */

/* Compare two complex vectors */
/* Subroutine */ int complexlessthan_(integer *n, integer1 *c__, complex *a, 
	integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double c_abs(complex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (c_abs(&a[m]) < c_abs(&b[p])) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexlessthan_ */

/* Compare two dcomplex vectors */
/* Subroutine */ int dcomplexlessthan_(integer *n, integer1 *c__, doublereal *
	a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = a[m], abs(d__1)) < (d__2 = b[p], abs(d__2))) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexlessthan_ */

/* ************************************************************************** */
/* Vector Lessequals functions */
/* ************************************************************************** */
/* Compare two int32 vectors */
/* Subroutine */ int int32lessequals_(integer *n, integer1 *c__, integer *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] <= b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32lessequals_ */

/* Compare two float vectors */
/* Subroutine */ int floatlessequals_(integer *n, integer1 *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] <= b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatlessequals_ */

/* Compare two double vectors */
/* Subroutine */ int doublelessequals_(integer *n, integer1 *c__, doublereal *
	a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] <= b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doublelessequals_ */

/* Compare two complex vectors */
/* Subroutine */ int complexlessequals_(integer *n, integer1 *c__, complex *a,
	 integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double c_abs(complex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (c_abs(&a[m]) <= c_abs(&b[p])) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexlessequals_ */

/* Compare two dcomplex vectors */
/* Subroutine */ int dcomplexlessequals_(integer *n, integer1 *c__, 
	doublereal *a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = a[m], abs(d__1)) <= (d__2 = b[p], abs(d__2))) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexlessequals_ */

/* ************************************************************************** */
/* Vector GreaterThan functions */
/* ************************************************************************** */
/* Compare two int32 vectors */
/* Subroutine */ int int32greaterthan_(integer *n, integer1 *c__, integer *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] > b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32greaterthan_ */

/* Compare two float vectors */
/* Subroutine */ int floatgreaterthan_(integer *n, integer1 *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] > b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatgreaterthan_ */

/* Compare two double vectors */
/* Subroutine */ int doublegreaterthan_(integer *n, integer1 *c__, doublereal 
	*a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] > b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doublegreaterthan_ */

/* Compare two complex vectors */
/* Subroutine */ int complexgreaterthan_(integer *n, integer1 *c__, complex *
	a, integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double c_abs(complex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (c_abs(&a[m]) > c_abs(&b[p])) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexgreaterthan_ */

/* Compare two dcomplex vectors */
/* Subroutine */ int dcomplexgreaterthan_(integer *n, integer1 *c__, 
	doublereal *a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = a[m], abs(d__1)) > (d__2 = b[p], abs(d__2))) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexgreaterthan_ */

/* ************************************************************************** */
/* Vector Greaterequals functions */
/* ************************************************************************** */
/* Compare two int32 vectors */
/* Subroutine */ int int32greaterequals_(integer *n, integer1 *c__, integer *
	a, integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] >= b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32greaterequals_ */

/* Compare two float vectors */
/* Subroutine */ int floatgreaterequals_(integer *n, integer1 *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] >= b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatgreaterequals_ */

/* Compare two double vectors */
/* Subroutine */ int doublegreaterequals_(integer *n, integer1 *c__, 
	doublereal *a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] >= b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doublegreaterequals_ */

/* Compare two complex vectors */
/* Subroutine */ int complexgreaterequals_(integer *n, integer1 *c__, complex 
	*a, integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double c_abs(complex *);

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (c_abs(&a[m]) >= c_abs(&b[p])) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexgreaterequals_ */

/* Compare two dcomplex vectors */
/* Subroutine */ int dcomplexgreaterequals_(integer *n, integer1 *c__, 
	doublereal *a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = a[m], abs(d__1)) >= (d__2 = b[p], abs(d__2))) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexgreaterequals_ */

/* ************************************************************************** */
/* Vector Notequals functions */
/* ************************************************************************** */
/* Compare two int32 vectors */
/* Subroutine */ int int32notequals_(integer *n, integer1 *c__, integer *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] != b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32notequals_ */

/* Compare two float vectors */
/* Subroutine */ int floatnotequals_(integer *n, integer1 *c__, real *a, 
	integer *stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] != b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatnotequals_ */

/* Compare two double vectors */
/* Subroutine */ int doublenotequals_(integer *n, integer1 *c__, doublereal *
	a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] != b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doublenotequals_ */

/* Compare two complex vectors */
/* Subroutine */ int complexnotequals_(integer *n, integer1 *c__, complex *a, 
	integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = m;
	i__3 = p;
	if (a[i__2].r != b[i__3].r || a[i__2].i != b[i__3].i) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexnotequals_ */

/* Compare two dcomplex vectors */
/* Subroutine */ int dcomplexnotequals_(integer *n, integer1 *c__, doublereal 
	*a, integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] != b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexnotequals_ */

/* ************************************************************************** */
/* Vector Equals functions */
/* ************************************************************************** */
/* Compare two int32 vectors */
/* Subroutine */ int int32equals_(integer *n, integer1 *c__, integer *a, 
	integer *stride1, integer *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] == b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* int32equals_ */

/* Compare two float vectors */
/* Subroutine */ int floatequals_(integer *n, integer1 *c__, real *a, integer 
	*stride1, real *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] == b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* floatequals_ */

/* Compare two double vectors */
/* Subroutine */ int doubleequals_(integer *n, integer1 *c__, doublereal *a, 
	integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] == b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* doubleequals_ */

/* Compare two complex vectors */
/* Subroutine */ int complexequals_(integer *n, integer1 *c__, complex *a, 
	integer *stride1, complex *b, integer *stride2)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = m;
	i__3 = p;
	if (a[i__2].r == b[i__3].r && a[i__2].i == b[i__3].i) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* complexequals_ */

/* Compare two dcomplex vectors */
/* Subroutine */ int dcomplexequals_(integer *n, integer1 *c__, doublereal *a,
	 integer *stride1, doublereal *b, integer *stride2)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, m, p;

    /* Parameter adjustments */
    --b;
    --a;
    --c__;

    /* Function Body */
    m = 1;
    p = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[m] == b[p]) {
	    c__[i__] = 1;
	} else {
	    c__[i__] = 0;
	}
	m += *stride1;
	p += *stride2;
/* L10: */
    }
    return 0;
} /* dcomplexequals_ */

