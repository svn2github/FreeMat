#ifndef __LUDecompose_hpp__
#define __LUDecompose_hpp__

#include "Array.hpp"

namespace FreeMat {
  void floatLU(const int nrows, const int ncols, float *l, float *u,
	       int *p, float *a);
  void complexLU(const int nrows, const int ncols, float *l, float *u,
	       int *p, float *a);
  void doubleLU(const int nrows, const int ncols, double *l, double *u,
		int *p, double *a);
  void dcomplexLU(const int nrows, const int ncols, double *l, double *u,
		  int *p, double *a);
  ArrayVector LUDecompose(int nargout, Array A);
}

#endif
