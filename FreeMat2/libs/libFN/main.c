#include <stdio.h>
#include <stdlib.h>

double derf_(double* x);
float erf_(float* x);
double derfc_(double* x);
float erfc_(float* x);
double derfcx_(double* x);
float erfcx_(float* x);
double dgamma_(double* x);
float gamma_(float* x);
double dlgama_(double* x);
float algama_(float* x);
double ddaw_(double* x);
float daw_(float* x);
int main(int argc, char*argv[]) {
  double f, t;
  f = 0.3123323452;
  t = derf_(&f);
  printf("erf(%.12f) = %.12f\n",f,t);
  t = derfc_(&f);
  printf("erfc(%.12f) = %.12f\n",f,t);
  t = derfcx_(&f);
  printf("erfcx(%.12f) = %.12f\n",f,t);
  t = dgamma_(&f);
  printf("gamma(%.12f) = %.12f\n",f,t);
  t = dlgama_(&f);
  printf("dlgama(%.12f) = %.12f\n",f,t);
  t = ddaw_(&f);
  printf("ddaw(%.12f) = %.12f\n",f,t);
  float f2, t2;
  f2 = 0.3123323452;
  t2 = erf_(&f2);
  printf("erf(%.12f) = %.12f\n",f2,t2);
  t2 = erfc_(&f2);
  printf("erfc(%.12f) = %.12f\n",f2,t2);
  t2 = erfcx_(&f2);
  printf("erfcx(%.12f) = %.12f\n",f2,t2);
  t2 = gamma_(&f2);
  printf("gamma(%.12f) = %.12f\n",f2,t2);
  t2 = algama_(&f2);
  t2 = daw_(&f2);
  printf("daw(%.12f) = %.12f\n",f2,t2);
  return 0;
}
