#ifndef __FN_hpp__
#define __FN_hpp__

#include "Array.hpp"
#include "WalkTree.hpp"

namespace FreeMat {
  ArrayVector FitFunFunction(int nargout, const ArrayVector& arg, WalkTree* eval);
}

#endif
