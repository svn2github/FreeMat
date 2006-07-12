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

#include "Utils.hpp"
#include "Exception.hpp"
#include <math.h>

float complex_abs(float real, float imag) {
  float temp;
  
  if(real < 0)
    real = -real;
  if(imag < 0)
    imag = -imag;
  if(imag > real){
    temp = real;
    real = imag;
    imag = temp;
  }
  if((real+imag) == real)
    return(real);
  
  temp = imag/real;
  temp = real*sqrt(1.0 + temp*temp);  /*overflow!!*/
  return(temp);
}

double complex_abs(double real, double imag) {
  double temp;
  
  if(real < 0)
    real = -real;
  if(imag < 0)
    imag = -imag;
  if(imag > real){
    temp = real;
    real = imag;
    imag = temp;
  }
  if((real+imag) == real)
    return(real);
  
  temp = imag/real;
  temp = real*sqrt(1.0 + temp*temp);  /*overflow!!*/
  return(temp);
}

void c_log(float real, float imag, float *re, float *im) {
  *re = log(complex_abs(real,imag));
  *im = atan2(imag,real);
}

void c_exp(float real, float imag, float *re, float *im) {
  float t;
  t = exp(real);
  *re = t*cos(imag);
  *im = t*sin(imag);
}

// Complex square root is defined as exp(0.5*log(a))
void c_sqrt(float real, float imag, float *re, float *im) {
  float tr, ti;
  c_log(real,imag,&tr,&ti);
  tr /= 2.0;
  ti /= 2.0;
  c_exp(tr,ti,re,im);
}

// Square a complex value: (a+i*b)*(a+i*b) = (a^2-b^2) + 2*i*a*b
void c_sqr(float real, float imag, float *re, float *im) {
  *re = real*real - imag*imag;
  *im = 2.0*real*imag;
}

void z_log(double real, double imag, double *re, double *im) {
  *re = log(complex_abs(real,imag));
  *im = atan2(imag,real);
}

void z_exp(double real, double imag, double *re, double *im) {
  double t;
  t = exp(real);
  *re = t*cos(imag);
  *im = t*sin(imag);
}

// Complex square root is defined as exp(0.5*log(a))
void z_sqrt(double real, double imag, double *re, double *im) {
  double tr, ti;
  z_log(real,imag,&tr,&ti);
  tr /= 2.0;
  ti /= 2.0;
  z_exp(tr,ti,re,im);
}

// Square a complex value: (a+i*b)*(a+i*b) = (a^2-b^2) + 2*i*a*b
void z_sqr(double real, double imag, double *re, double *im) {
  *re = real*real - imag*imag;
  *im = 2.0*real*imag;
}
