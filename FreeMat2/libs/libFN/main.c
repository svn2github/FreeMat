#include <stdio.h>
#include <stdlib.h>

int main(int argc, char*argv[]) {
  double f, t;
  f = 0.3123323452;
  double derf_(double* x);
  t = derf_(&f);
  printf("erf(%.12f) = %.12f\n",f,t);
  float f2, t2;
  f2 = 0.3123323452;
  float erf_(float* x);
  t2 = erf_(&f2);
  printf("erf(%.12f) = %.12f\n",f2,t2);
  return 0;
}
