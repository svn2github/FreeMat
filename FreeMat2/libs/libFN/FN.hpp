#ifndef __FN_hpp__
#define __FN_hpp__

#include "Array.hpp"
#include "WalkTree.hpp"

namespace FreeMat {
  ArrayVector FitFunFunction(int nargout, const ArrayVector& arg, WalkTree* eval);
  ArrayVector ExpeiFunction(int nargout, const ArrayVector& arg);
  ArrayVector EoneFunction(int nargout, const ArrayVector& arg);
  ArrayVector EiFunction(int nargout, const ArrayVector& arg);
  ArrayVector ErfcxFunction(int nargout, const ArrayVector& arg);
  ArrayVector ErfcFunction(int nargout, const ArrayVector& arg);
  ArrayVector ErfFunction(int nargout, const ArrayVector& arg);
  ArrayVector DawsonFunction(int nargout, const ArrayVector& arg);
  ArrayVector PsiFunction(int nargout, const ArrayVector& arg);
  ArrayVector GammaFunction(int nargout, const ArrayVector& arg);
  ArrayVector GammaLnFunction(int nargout, const ArrayVector& arg);
}

#endif
