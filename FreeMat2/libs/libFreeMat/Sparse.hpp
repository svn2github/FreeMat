#ifndef __Sparse_hpp__
#define __Sparse_hpp__

#include "Types.hpp"
#include "Array.hpp"

namespace FreeMat {
  // Test 56
  void DeleteSparseMatrix(Class dclass, int rows, int cols, void * cp);
  // Test 57
  void* makeDenseArray(Class dclass, int rows, int cols, const void* cp);
  // Test 57
  void* makeSparseArray(Class dclass, int rows, int cols, const void* cp);
  // Test 58
  void* TypeConvertSparse(Class dclass, int rows, int cols, 
			  const void *cp, Class oclass);
  // Test 57 & 58
  void* CopySparseMatrix(Class dclass, int rows, int cols, const void* cp);
  int CountNonzeros(Class dclass, int rows, int cols, const void *cp);
  // Test 59
  void* SparseDenseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				  const void* ap, const void* bp);
  // Test 60
  void* DenseSparseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				  const void* ap, const void* bp);
  // Test 61
  void* SparseSparseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				   const void* ap, const void* bp);
  // Test 62
  void* SparseMatrixConstructor(Class dclass, int rows, int cols,
				ArrayMatrix m);
  // Test 63
  void* GetSparseVectorSubsets(Class dclass, int rows, int cols, 
			       const void* src, const indexType* indx, 
			       int irows, int icols);
  // Test 64
  void* GetSparseNDimSubsets(Class dclass, int rows, int cols, const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols);
  // Test 65
  void* SetSparseVectorSubsets(Class dclass, int &rows, int &cols, 
			       const void* src, const indexType* indx, 
			       int irows, int icols, const void* data, 
			       int advance);
  // Test 66
  void* SetSparseNDimSubsets(Class dclass, int &rows, int &cols, 
			     const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols,
			     const void* data, int advance);
  // Test 67
  void* GetSparseScalarElement(Class dclass, int rows, int cols, 
			       const void* src,  indexType rindx, 
			       indexType cindx);
  // Test 68
  void* SparseToIJV(Class dclass, int rows, int cols, const void* cp,
		    uint32* &I, uint32* &J, int &nnz);
  // Test 69
  void* SparseFloatZeros(int rows, int cols);
  // Test 70
  void* makeSparseFromIJV(Class dclass, int rows, int cols, int nnz, 
			  uint32* I, int istride, uint32 *J, int jstride,
			  const void* cp, int cpstride);
  // Test 71
  void* DeleteSparseMatrixCols(Class dclass, int rows, int cols, const void* cp,
			       bool *dmap);
  // Test 72
  void* DeleteSparseMatrixRows(Class dclass, int rows, int cols, const void* cp,
			       bool *dmap);
  // Test 73
  void* DeleteSparseMatrixVectorSubset(Class dclass, int &rows, int &cols, 
				       const void *cp,
				       const indexType *todel, int delete_len);
  // Test 74
  void* GetSparseDiagonal(Class dclass, int rows, int cols, const void* cp, int diag_order);
  // Test 75
  bool SparseAnyNotFinite(Class dclass, int rows, int cols, const void* cp);
  // Test 76
  void* SparseArrayHermitian(Class dclass, int rows, int cols, const void* cp);
  // Test 77
  void* SparseArrayTranspose(Class dclass, int rows, int cols, const void* cp);
  // Test 78
  void* SparseSparseAdd(Class dclass, const void *ap, int rows, int cols, 
			const void *bp);
  // Test 79
  void* SparseSparseSubtract(Class dclass, const void *ap, int rows, int cols, 
			     const void *bp);
  // Test 80
  void* SparseSparseMultiply(Class dclass, const void *ap, int rows, int cols, 
			     const void *bp);

  void* SparseScalarMultiply(Class dclass, const void *ap, int rows, int cols, 
			     const void *bp);
  // Test 82
  void* SparseSolveLinEq(Class dclass, int Arows, int Acols, const void *Ap,
						 int Brows, int Bcols, const void *Bp);
  ArrayVector SparseLUDecompose(int nargout, Array A);
  ArrayVector SparseEigDecompose(int nargout, Array A, int k, char* whichFlag);
  ArrayVector SparseEigDecomposeShifted(int nargout, Array A, int k, double shift[2]);
}
#endif
