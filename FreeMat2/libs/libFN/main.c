#include <stdio.h>
#include <stdlib.h>

#define dotestdouble(x,nm) {double x(double*g); t = x(&f); printf("%s(%.12f) = %.12f\n",nm,f,t);}
#define dotestfloat(x,nm) {float x(float*g); t2 = x(&f2); printf("%s(%.12f) = %.12f\n",nm,f2,t2);}

int main(int argc, char*argv[]) {
  double f, t;
  f = 0.3123323452;
  float f2, t2;
  f2 = 0.3123323452;
  dotestdouble(derf_,"derf");
  dotestfloat(erf_,"erf");
  dotestdouble(derfc_,"derfc");
  dotestfloat(erfc_,"erfc");
  dotestdouble(derfcx_,"derfcx");
  dotestfloat(erfcx_,"erfcx");
  dotestdouble(dgamma_,"dgamma");
  dotestfloat(gamma_,"gamma");
  dotestdouble(dlgama_,"dlgamma");
  dotestfloat(algama_,"algamma");
  dotestdouble(ddaw_,"ddaw");
  dotestfloat(daw_,"daw");
  dotestdouble(dei_,"dei");
  dotestfloat(ei_,"ei");
  dotestdouble(dbesi0_,"dbesi0");
  dotestfloat(besi0_,"besi0");
  dotestdouble(dbesi1_,"dbesi1");
  dotestfloat(besi1_,"besi1");
  dotestdouble(dbesei0_,"dbesei0");
  dotestfloat(besei0_,"besei0");
  dotestdouble(dbesei1_,"dbesei1");
  dotestfloat(besei1_,"besei1");
  dotestdouble(dbesj0_,"dbesj0");
  dotestfloat(besj0_,"besj0");
  dotestdouble(dbesy0_,"dbesy0");
  dotestfloat(besy0_,"besy0");
  dotestdouble(dbesj1_,"dbesj1");
  dotestfloat(besj1_,"besj1");
  dotestdouble(dbesy1_,"dbesy1");
  dotestfloat(besy1_,"besy1");
  dotestdouble(dbesk0_,"dbesk0");
  dotestfloat(besk0_,"besk0");
  dotestdouble(dbesek0_,"dbesek0");
  dotestfloat(besek0_,"besek0");
  dotestdouble(dbesek1_,"dbesek1");
  dotestfloat(besek1_,"besek1");
  dotestdouble(dpsi_,"dpsi");
  dotestfloat(psi_,"psi");
  return 0;
}
