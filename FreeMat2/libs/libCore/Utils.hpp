/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __Utils_hpp__
#define __Utils_hpp__

float complex_abs(float, float);
double complex_abs(double, double);
void clog(float real, float imag, float *re, float *im);
void cexp(float real, float imag, float *re, float *im);
void csqrt(float real, float imag, float *re, float *im);
void csqr(float real, float imag, float *re, float *im);
void zlog(double real, double imag, double *re, double *im);
void zexp(double real, double imag, double *re, double *im);
void zsqrt(double real, double imag, double *re, double *im);
void zsqr(double real, double imag, double *re, double *im);
#endif
