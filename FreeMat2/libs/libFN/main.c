#include <stdio.h>
#include <stdlib.h>

double derf_(double* x);
float erf_(float* x);
double derfc_(double* x);
float erfc_(float* x);
int main(int argc, char*argv[]) {
  double f, t;
  f = 0.3123323452;
  t = derf_(&f);
  printf("erf(%.12f) = %.12f\n",f,t);
  t = derfc_(&f);
  printf("erfc(%.12f) = %.12f\n",f,t);
  float f2, t2;
  f2 = 0.3123323452;
  t2 = erf_(&f2);
  printf("erf(%.12f) = %.12f\n",f2,t2);
  t2 = erfc_(&f2);
  printf("erfc(%.12f) = %.12f\n",f2,t2);
  return 0;
}
