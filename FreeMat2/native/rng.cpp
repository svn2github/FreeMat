#include <stdlib.h>
#include <stdio.h>
#include <math.h>


void do_single_sided_algo(float a, float b,float *pvec, int adder, int count) {
  double d;
  float p;
  float cplus;
  d = a;
  //   cplus = nextafter(c,c+b);
  //   if (b > 0) {
  //     while (d <= cplus) {
  //       p = (float) d;
  //       printf("%0.24f %x \n",p,*((int*)(&p)));
  //       d += (double) b;
  //     }
  //   } else {
  //     while (d >= cplus) {
  //       p = (float) d;
  //       printf("%0.24f %x \n",p,*((int*)(&p)));
  //       d += b;
  //     }
  //   }
  for (int i=0;i<count;i++) {
    pvec[i*adder] = (float) d;
    d += b;
  }
}

void do_double_sided_algo(float a, float b, float c, float *pvec, int adder, int count) {
  if (count%2) {
    do_single_sided_algo(a,b,pvec,adder,count/2);
    do_single_sided_algo(c,-b,pvec+(count-1)*adder,-adder,count/2+1);
  } else {
    do_single_sided_algo(a,b,pvec,adder,count/2);
    do_single_sided_algo(c,-b,pvec+(count-1)*adder,-adder,count/2);
  }
}

void fillarray(float a, float b, float c, float *pvec, int count, bool use_double) {

}

int main(int argc, char *argv[]) {
  float  a, b, c;
  float wb;
  int stepcount;

  a = atof(argv[1]);
  b = atof(argv[2]);
  c = atof(argv[3]);

  int adder = 1;

  //ideally, n = (c-a)/b
  // But this really defines an interval... we let
  // n_min = min(c-a)/max(b)
  // n_max = max(c-a)/min(b)
  // where min(x) = {y \in fp | |y| is max, |y| < |x|, sign(y) = sign(x)}
  //       max(x) = {y \in fp | |y| is min, |y| > |x|, sign(y) = sign(x)}

  float ntest_min = nextafterf(c-a,0)/nextafterf(b,b+b);
  float ntest_max = nextafterf(c-a,c-a+b)/nextafterf(b,0);

  printf("%0.24f %0.24f\n",ntest_min,ntest_max);
  
  int npts = (int) floor(ntest_max);

  bool use_double_sided = (ntest_min <= npts) && (npts <= ntest_max);

  //   float ntest = (c-a)/b;
  //   float ntest2 = (nextafterf(c,c+1)-a)/b;
  //   int npts = (int) floor(ntest2) + 1;
  //   printf("%0.24f %0.24f %0.24f %d\n",ntest,ntest2,nextafterf(ntest,ntest+1),npts);
  npts++;
  float *outvec = (float*) malloc(sizeof(float)*npts);
//   if (b < 0) {
//     adder = -1;
//     b = -b;
//   }
  if (use_double_sided) {
    printf("double sided\n");
    if (adder>0)
      do_double_sided_algo(a,b,c,outvec,1,npts);
    else
      do_double_sided_algo(a,b,c,outvec+npts-1,-1,npts);      
  }  else {
    if (adder>0)
      do_single_sided_algo(a,b,outvec,1,npts);
    else
      do_single_sided_algo(a,b,outvec+npts-1,-1,npts);
  }

  for (int i=0;i<npts;i++) {
    printf("[%d] = %0.24f %x \n",i,outvec[i],*((int*)(outvec+i)));
  }
  return 0;
}
