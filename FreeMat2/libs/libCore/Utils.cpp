// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Utils.hpp"
#include "Exception.hpp"
#include <math.h>

namespace FreeMat {
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

  void clog(float real, float imag, float *re, float *im) {
    *re = log(complex_abs(real,imag));
    *im = atan2(imag,real);
  }

  void cexp(float real, float imag, float *re, float *im) {
    float t;
    t = exp(real);
    *re = t*cos(imag);
    *im = t*sin(imag);
  }

  // Complex square root is defined as exp(0.5*log(a))
  void csqrt(float real, float imag, float *re, float *im) {
    float tr, ti;
    clog(real,imag,&tr,&ti);
    tr /= 2.0;
    ti /= 2.0;
    cexp(tr,ti,re,im);
  }

  // Square a complex value: (a+i*b)*(a+i*b) = (a^2-b^2) + 2*i*a*b
  void csqr(float real, float imag, float *re, float *im) {
    *re = real*real - imag*imag;
    *im = 2.0*real*imag;
  }

  void zlog(double real, double imag, double *re, double *im) {
    *re = log(complex_abs(real,imag));
    *im = atan2(imag,real);
  }

  void zexp(double real, double imag, double *re, double *im) {
    double t;
    t = exp(real);
    *re = t*cos(imag);
    *im = t*sin(imag);
  }

  // Complex square root is defined as exp(0.5*log(a))
  void zsqrt(double real, double imag, double *re, double *im) {
    double tr, ti;
    zlog(real,imag,&tr,&ti);
    tr /= 2.0;
    ti /= 2.0;
    zexp(tr,ti,re,im);
  }

  // Square a complex value: (a+i*b)*(a+i*b) = (a^2-b^2) + 2*i*a*b
  void zsqr(double real, double imag, double *re, double *im) {
    *re = real*real - imag*imag;
    *im = 2.0*real*imag;
  }

}
