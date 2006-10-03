#include "array.hpp"
#include <string.h>

int main(int argc, char *argv[]) {
  if (strcmp(argv[1],"array") == 0) {
    int *dims = new int[2];
    dims[0] = 500; dims[1] = 500;
    array A = array(2,dims,malloc(sizeof(double)*500*500),sizeof(double)*500*500);
    array B = array(2,dims,malloc(sizeof(double)*500*500),sizeof(double)*500*500);
    for (int k=0;k<1000;k++) {
      for (int i=0;i<500;i++) {
	array I = array(i);
	for (int j=0;j<500;j++) {
	  array J = array(j);
	  B.set(I,J,A.get(J,I));
	}
      }
    }
  } else if (strcmp(argv[1],"c") == 0) {
    double *g = new double[500*500];
    double *h = new double[500*500];
    for (int i=0;i<500;i++)
      for (int j=0;j<500;j++)
	g[i+j*500] = h[j+i*500];
  } else {
    array a(0);
    for (int i=0;i<10000000;i++) {
      a = array(i);
    }
  }
  return 0;
}
