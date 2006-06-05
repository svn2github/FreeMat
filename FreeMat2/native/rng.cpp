#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]) {
  float  a, b, c;
  int stepcount;

  a = atof(argv[1]);
  b = atof(argv[2]);
  c = atof(argv[3]);
  
  double d = a;
  if (b > 0) {
    while (d <= nextafterf(c,c*2)) {
      printf("%0.18f \n",(float)d);
      d += b;
    }
  } else {
    while (d >= nextafterf(c,c*2)) {
      printf("%0.18f \n",(float)d);
      d += b;
    }
  }
  printf("------\n");
  return 0;
}
