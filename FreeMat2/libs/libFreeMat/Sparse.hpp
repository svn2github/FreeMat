#ifndef __Sparse_hpp__
#define __Sparse_hpp__

#include "Types.hpp"
#include "Array.hpp"

namespace FreeMat {
  void DeleteSparseMatrix(Class dclass, int rows, int cols, void * cp);
  void* makeDenseArray(Class dclass, int rows, int cols, const void* cp);
  void* makeSparseArray(Class dclass, int rows, int cols, const void* cp);
  void* TypeConvertSparse(Class dclass, int rows, int cols, 
			  const void *cp, Class oclass);
  void* CopySparseMatrix(Class dclass, int rows, int cols, const void* cp);
}

#endif
