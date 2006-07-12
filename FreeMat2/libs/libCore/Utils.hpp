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
void c_log(float real, float imag, float *re, float *im);
void c_exp(float real, float imag, float *re, float *im);
void c_sqrt(float real, float imag, float *re, float *im);
void c_sqr(float real, float imag, float *re, float *im);
void z_log(double real, double imag, double *re, double *im);
void z_exp(double real, double imag, double *re, double *im);
void z_sqrt(double real, double imag, double *re, double *im);
void z_sqr(double real, double imag, double *re, double *im);
#endif
