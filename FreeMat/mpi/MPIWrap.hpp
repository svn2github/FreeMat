#ifndef __MPIWRAP_HPP__
#define __MPIWRAP_HPP__

#include "Array.hpp"

namespace FreeMat {
  ArrayVector MPISend(int nargout, const ArrayVector& args);
  ArrayVector MPIRecv(int nargout, const ArrayVector& args);
  ArrayVector MPICommRank(int nargout, const ArrayVector& args);
  ArrayVector MPICommSize(int nargout, const ArrayVector& args);
  ArrayVector MPIEval(int nargout, const ArrayVector& args);
  void InitializeMPIWrap();
}

#endif
