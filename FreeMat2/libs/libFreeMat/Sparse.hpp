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
  int CountNonzeros(Class dclass, int rows, int cols, const void *cp);
  void* SparseDenseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				  const void* ap, const void* bp);
  void* DenseSparseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				  const void* ap, const void* bp);
  void* SparseSparseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				   const void* ap, const void* bp);
  void* SparseMatrixConstructor(Class dclass, int rows, int cols,
				ArrayMatrix m);
  void* GetSparseVectorSubsets(Class dclass, int rows, int cols, 
			       const void* src, const indexType* indx, 
			       int irows, int icols);
  void* GetSparseNDimSubsets(Class dclass, int rows, int cols, const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols);
  void* GetSparseScalarElement(Class dclass, int rows, int cols, 
			       const void* src,  indexType rindx, 
			       indexType cindx);
}

#endif
