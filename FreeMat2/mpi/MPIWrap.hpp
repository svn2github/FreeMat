#ifndef __MPIWRAP_HPP__
#define __MPIWRAP_HPP__

#include "Array.hpp"

namespace FreeMat {
  ArrayVector MPISend(int nargout, const ArrayVector& args);
  ArrayVector MPIRecv(int nargout, const ArrayVector& args);
  ArrayVector MPICommRank(int nargout, const ArrayVector& args);
  ArrayVector MPICommSize(int nargout, const ArrayVector& args);
  ArrayVector MPIEval(int nargout, const ArrayVector& args);
  ArrayVector MPIBcast(int nargout, const ArrayVector& args);
  ArrayVector MPIBarrier(int nargout, const ArrayVector& args);
  ArrayVector MPIReduce(int nargout, const ArrayVector& args);
  void LoadMPIFunctions(Context*ctxt);
  void InitializeMPIWrap();
}

#endif
