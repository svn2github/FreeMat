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
  void* SetSparseVectorSubsets(Class dclass, int &rows, int &cols, 
			       const void* src, const indexType* indx, 
			       int irows, int icols, const void* data, 
			       int advance);
  void* SetSparseNDimSubsets(Class dclass, int &rows, int &cols, 
			     const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols,
			     const void* data, int advance);
  void* GetSparseScalarElement(Class dclass, int rows, int cols, 
			       const void* src,  indexType rindx, 
			       indexType cindx);
  void* SparseToIJV(Class dclass, int rows, int cols, const void* cp,
		    uint32* &I, uint32* &J, int &nnz);
  void* SparseFloatZeros(int rows, int cols);
  void* makeSparseFromIJV(Class dclass, int rows, int cols, int nnz, 
			  uint32* I, int istride, uint32 *J, int jstride,
			  const void* cp, int cpstride);
  void* DeleteSparseMatrixCols(Class dclass, int rows, int cols, const void* cp,
			       bool *dmap);
  void* DeleteSparseMatrixRows(Class dclass, int rows, int cols, const void* cp,
			       bool *dmap);
  void* DeleteSparseMatrixVectorSubset(Class dclass, int &rows, int &cols, 
				       const void *cp,
				       const indexType *todel, int delete_len);
  void* GetSparseDiagonal(Class dclass, int rows, int cols, const void* cp, int diag_order);
  bool SparseAnyNotFinite(Class dclass, int rows, int cols, const void* cp);
  void* SparseArrayHermitian(Class dclass, int rows, int cols, const void* cp);
  void* SparseArrayTranspose(Class dclass, int rows, int cols, const void* cp);
  void* SparseSparseAdd(Class dclass, const void *ap, int rows, int cols, 
			const void *bp);
  void* SparseSparseSubtract(Class dclass, const void *ap, int rows, int cols, 
			     const void *bp);
  void* SparseSparseMultiply(Class dclass, const void *ap, int rows, int cols, 
			     const void *bp);
  void* SparseScalarMultiply(Class dclass, const void *ap, int rows, int cols, 
			     const void *bp);
  void* SparseSolveLinEq(Class dclass, int Arows, int Acols, const void *Ap,
						 int Brows, int Bcols, const void *Bp);
  ArrayVector SparseLUDecompose(int nargout, Array A);
  ArrayVector SparseEigDecompose(int nargout, Array A, int k);
  ArrayVector SparseEigDecomposeShifted(int nargout, Array A, int k, double shift[2]);
}
#endif
