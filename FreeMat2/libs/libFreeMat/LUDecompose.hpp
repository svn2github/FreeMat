#ifndef __LUDecompose_hpp__
#define __LUDecompose_hpp__

#include "Array.hpp"

namespace FreeMat {
  void floatLU(int nrows, int ncols, float *l, float *u,
	       int *p, float *a);
  void complexLU(int nrows, int ncols, float *l, float *u,
	       int *p, float *a);
  void doubleLU(int nrows, int ncols, double *l, double *u,
		int *p, double *a);
  void dcomplexLU(int nrows, int ncols, double *l, double *u,
		  int *p, double *a);
  ArrayVector LUDecompose(int nargout, Array A);
}

#endif
