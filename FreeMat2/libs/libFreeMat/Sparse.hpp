/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __Sparse_hpp__
#define __Sparse_hpp__

#include "Types.hpp"
#include "Array.hpp"

typedef enum {
  SLO_LT,
  SLO_GT,
  SLO_LE,
  SLO_GE,
  SLO_NE,
  SLO_EQ,
  SLO_AND,
  SLO_NOT,
  SLO_OR
} SparseLogOpID;  

// Test 56
void DeleteSparseMatrix(Class dclass, int cols, void * cp);
// Test 57
void* makeDenseArray(Class dclass, int rows, int cols, const void* cp);
// Test 57
void* makeSparseArray(Class dclass, int rows, int cols, const void* cp);
// Test 58
void* TypeConvertSparse(Class dclass, int rows, int cols, 
			const void *cp, Class oclass);
// Test 57 & 58
void* CopySparseMatrix(Class dclass, int cols, const void* cp);
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
void* SparseMatrixConstructor(Class dclass, int cols, ArrayMatrix m);
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
void SetSparseNDimSubsets(Class dclass, int rows, 
			  void* src,
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
void* SparseToIJV2(Class dclass, int rows, int cols, const void* cp,
		   uint32* &I, uint32* &J, int &nnz);
// Test 69
void* SparseFloatZeros(int rows, int cols);
// Test 70
void* makeSparseFromIJV(Class dclass, int rows, int cols, int nnz, 
			const uint32* I, int istride, 
			const uint32 *J, int jstride,
			const void* cp, int cpstride);
// Test 71
void* DeleteSparseMatrixCols(Class dclass, int cols, const void* cp,
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
bool SparseAnyNotFinite(Class dclass, int cols, const void* cp);
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
// Test 81
void* SparseOnesFunc(Class dclass, int Arows, int Acols, const void *Ap);
Array SparsePowerFunc(Array A, Array B);
bool SparseIsPositive(Class dclass, int Arows, int Acols, const void *Ap);
void* SparseMatrixSumRows(Class dclass, int Arows, int Acols, const void *Ap);  
void* SparseMatrixSumColumns(Class dclass, int Arows, int Acols, const void *Ap);  
uint32* SparseLogicalToOrdinal(int rows, int cols, const void *Ap, int& nnz);
void* SparseSparseLogicalOp(Class dclass, int rows, int cols, const void *Ap, const void *Bp, SparseLogOpID opselect);
void* SparseScalarLogicalOp(Class dclass, int rows, int cols, const void *Ap, const void *Bp, SparseLogOpID opselect);
void* ReshapeSparseMatrix(Class dclass, int rows, int cols, const void *Ap, int newrows, int newcols);
void* SparseAbsFunction(Class dclass, int rows, int cols, const void *Ap);
void* CopyResizeSparseMatrix(Class dclass, int rows, int cols, 
			     const void *Ap, int newrows, int newcols);
#endif
