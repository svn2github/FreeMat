#include "Sparse.hpp"
#include "Malloc.hpp"
#include <algorithm>

namespace FreeMat {

  template <class T>
  void RawPrint(T** src, int rows, int cols) {
    int i, j;
    for (i=0;i<cols;i++) {
      printf("%d <",(int)src[i][0]);
      for (j=0;j<(int)src[i][0];j++) {
	std::cout << " " << src[i][j+1];
      }
      std::cout << "\r\n";
    }
  }
  
  template <class T>
  void DeleteSparse(T** src, int rows, int cols) {
    int i;
    for (i=0;i<cols;i++)
      delete[] src[i];
    delete[] src;
  }
  template <class T>
  void DecompressComplexString(const T* src, T* dst, int count) {
    int i=0;
    int n=1;
    int j;
    while (i<2*count) {
      if ((src[n] != 0) || (src[n+1] != 0)) {
	dst[i] = src[n];
	i++;
	n++;
	dst[i] = src[n];
	i++;
	n++;
      } else {
	j = (int) src[n+2];
	memset(dst+i,0,2*sizeof(T)*j);
	i += j*2;
	n += 3;
      }
    }
  }

  template <class T>
  T* CompressComplexVector(const T* src, int count) {
    int i, j;
    int zlen;
    int outlen = 0;
    i = 0;
    while (i<count) {
      if ((src[2*i] != 0) || (src[2*i+1] != 0)) {
	i++;
	outlen+=2;
      } else {
	outlen+=3;
	while ((i<count) && (src[2*i] == 0) && (src[2*i+1] == 0)) i++;
      }
    }
    T* dp = new T[outlen+1];
    dp[0] = outlen;
    j = 0;
    i = 0;
    outlen = 1;
    while (i<count) {
      if ((src[2*i] != 0) || (src[2*i+1] != 0)) {
	dp[outlen] = src[2*i];
	outlen++;
	dp[outlen] = src[2*i+1];
	outlen++;
	i++;
      } else {
	zlen = 0;
	while ((i<count) && (src[2*i] == 0) && (src[2*i+1] == 0)) {
	  i++;
	  zlen++;
	}
	dp[outlen++] = 0;
	dp[outlen++] = 0;
	dp[outlen++] = zlen;
      }
    }
    return dp;
  }

  template <class T>
  void DecompressRealString(const T* src, T* dst, int count) {
    int i=0;
    int n=1;
    int j;
    while (i<count) {
      if (src[n] != 0) {
	dst[i] = src[n];
	i++;
	n++;
      } else {
	j = (int) src[n+1];
	memset(dst+i,0,sizeof(T)*j);
	i += j;
	n += 2;
      }
    }
  }

  template <class T>
  T* CompressRealVector(const T* src, int count) {
    int i, j;
    int zlen;
    int outlen = 0;
    i = 0;
    while (i<count) {
      if (src[i] != 0) {
	i++;
	outlen++;
      } else {
	outlen+=2;
	while ((i<count) && (src[i] == 0)) i++;
      }
    }
    T* dp = new T[outlen+1];
    dp[0] = outlen;
    j = 0;
    i = 0;
    outlen = 1;
    while (i<count) {
      if (src[i] != 0) {
	dp[outlen] = src[i];
	i++;
	outlen++;
      } else {
	zlen = 0;
	while ((i<count) && (src[i] == 0)) {
	  i++;
	  zlen++;
	}
	dp[outlen++] = 0;
	dp[outlen++] = zlen;
      }
    }
    return dp;
  }

  template <class T>
  T** ConvertDenseToSparseComplex(const T* src, int rows, int cols) {
    T** dp;
    dp = new T*[cols];
    for (int i=0;i<cols;i++)
      dp[i] = CompressComplexVector<T>(src+i*rows*2, rows);
    return dp;
  }
  
  template <class T>
  T** ConvertDenseToSparseReal(const T* src, int rows, int cols) {
    T** dp;
    dp = new T*[cols];
    for (int i=0;i<cols;i++)
      dp[i] = CompressRealVector<T>(src+i*rows, rows);
    return dp;
  }
  
  template <class T>
  T* ConvertSparseToDenseComplex(const T** src, int rows, int cols) {
    T* dst;
    dst = (T*) Malloc(sizeof(T)*rows*cols*2);
    for (int i=0;i<cols;i++)
      DecompressComplexString<T>(src[i],dst+i*2*rows,rows);
    return dst;
  }
  
  template <class T>
  T* ConvertSparseToDenseReal(const T** src, int rows, int cols) {
    T* dst;
    dst = (T*) Malloc(sizeof(T)*rows*cols);
    for (int i=0;i<cols;i++)
      DecompressRealString<T>(src[i],dst+i*rows,rows);
    return dst;
  }

  void* makeDenseArray(Class dclass, int rows, int cols, const void *cp) {
    switch(dclass) {
    case FM_INT32:
      return ConvertSparseToDenseReal<int32>((const int32 **)cp,rows,cols);
    case FM_FLOAT:
      return ConvertSparseToDenseReal<float>((const float **)cp,rows,cols);
    case FM_DOUBLE:
      return ConvertSparseToDenseReal<double>((const double **)cp,rows,cols);
    case FM_COMPLEX:
      return ConvertSparseToDenseComplex<float>((const float **)cp,rows,cols);
    case FM_DCOMPLEX:
      return ConvertSparseToDenseComplex<double>((const double **)cp,rows,cols);
    otherwise:
      throw Exception("Unsupported type in makeDenseArray");
    }
  }

  void* makeSparseArray(Class dclass, int rows, int cols, const void *cp) {
    switch(dclass) {
    case FM_INT32:
      return ConvertDenseToSparseReal<int32>((const int32 *)cp,rows,cols);
    case FM_FLOAT:
      return ConvertDenseToSparseReal<float>((const float *)cp,rows,cols);
    case FM_DOUBLE:
      return ConvertDenseToSparseReal<double>((const double *)cp,rows,cols);
    case FM_COMPLEX:
      return ConvertDenseToSparseComplex<float>((const float *)cp,rows,cols);
    case FM_DCOMPLEX:
      return ConvertDenseToSparseComplex<double>((const double *)cp,rows,cols);
    otherwise:
      throw Exception("Unsupported type in makeSparseArray");
    }
  }

  template <class T>
  class IJVEntry {
  public:
    uint32 I;
    uint32 J;
    T Vreal;
    T Vimag;
  };
  
  template <class T>
  bool operator<(const IJVEntry<T>& a, const IJVEntry<T>& b) {
    return ((a.J < b.J) || ((a.J == b.J) && (a.I <= b.I)));
  }

  template <class T>
  void printIJV(IJVEntry<T>* a, int len) {
    int i;
    for (i=0;i<len;i++) {
      std::cout << "ijv[" << i << "] I = " << a[i].I;
      std::cout << "  J = " << a[i].J << "  Vr = " << a[i].Vreal;
      std::cout << "  Vi = " << a[i].Vimag << "\r\n";
    }    
  }

  void tstop() {
    int i;
    i = 423;
  }

  template <class T>
  T* CompressRealIJV(IJVEntry<T> *mlist, int len, int &ptr, int col, int row) {
    if ((ptr >= len) || (mlist[ptr].J > col)) {
      T* buffer = new T[3];
      buffer[0] = 2;
      buffer[1] = 0;
      buffer[2] = row;
      return buffer;
    }
    T* buffer = new T[row];
    memset(buffer,0,row*sizeof(T));
    while ((ptr < len) && (mlist[ptr].J == col)) {
      buffer[mlist[ptr].I] += mlist[ptr].Vreal;
      ptr++;
    }
    T* ret = CompressRealVector<T>(buffer,row);
    delete buffer;
    return ret;
  }

  template <class T>
  T* CompressComplexIJV(IJVEntry<T> *mlist, int len, int &ptr, int col, int row) {
    if ((ptr >= len) || (mlist[ptr].J > col)) {
      T* buffer = new T[4];
      buffer[0] = 3;
      buffer[1] = 0;
      buffer[2] = 0;
      buffer[3] = row;
      return buffer;
    }
    T* buffer = new T[2*row];
    memset(buffer,0,2*row*sizeof(T));
    while ((ptr < len) && (mlist[ptr].J == col)) {
      buffer[2*mlist[ptr].I] += mlist[ptr].Vreal;
      buffer[2*mlist[ptr].I+1] += mlist[ptr].Vimag;
      ptr++;
    }
    T* ret = CompressComplexVector<T>(buffer,row);
    delete buffer;
    return ret;
  }


  template <class T>
  void* makeSparseFromIJVReal(int rows, int cols, int nnz,
			      uint32* I, int istride, uint32 *J, int jstride,
			      T* cp, int cpstride) {
    // build an IJV master list
    IJVEntry<T> *mlist = new IJVEntry<T>[nnz];
    int i;
    for (i=0;i<nnz;i++) {
      mlist[i].I = I[istride*i]-1;
      if ((mlist[i].I < 0) || (mlist[i].I >= rows))
	throw Exception("an element of the I vector in the I,J,V exceeds the size of the output matrix");
      mlist[i].J = J[jstride*i]-1;
      if ((mlist[i].J < 0) || (mlist[i].J >= cols))
	throw Exception("an element of the J vector in the I,J,V exceeds the size of the output matrix");
      mlist[i].Vreal = cp[cpstride*i];
    }
    std::sort(mlist,mlist+nnz);
    T** op;
    // Allocate the output (sparse) array
    op = new T*[cols];
    // Get an integer pointer into the IJV list
    int ptr = 0;
    // Stringify...
    for (int col=0;col<cols;col++)
      op[col] = CompressRealIJV<T>(mlist,nnz,ptr,col,rows);
    // Free the space
    delete[] mlist;
    // Return the array
    return op;
  }

  template <class T>
  void* makeSparseFromIJVComplex(int rows, int cols, int nnz,
			      uint32* I, int istride, uint32 *J, int jstride,
			      T* cp, int cpstride) {
    // build an IJV master list
    IJVEntry<T> *mlist = new IJVEntry<T>[nnz];
    int i;
    for (i=0;i<nnz;i++) {
      mlist[i].I = I[istride*i]-1;
      if ((mlist[i].I < 0) || (mlist[i].I >= rows))
	throw Exception("an element of the I vector in the I,J,V exceeds the size of the output matrix");
      mlist[i].J = J[jstride*i]-1;
      if ((mlist[i].J < 0) || (mlist[i].J >= cols))
	throw Exception("an element of the J vector in the I,J,V exceeds the size of the output matrix");
      mlist[i].Vreal = cp[2*cpstride*i];
      mlist[i].Vimag = cp[2*cpstride*i+1];
    }
    std::sort(mlist,mlist+nnz);
    T** op;
    // Allocate the output (sparse) array
    op = new T*[cols];
    // Get an integer pointer into the IJV list
    int ptr = 0;
    // Stringify...
    for (int col=0;col<cols;col++)
      op[col] = CompressComplexIJV<T>(mlist,nnz,ptr,col,rows);
    // Free the space
    delete[] mlist;
    // Return the array
    return op;
  }


  void* makeSparseFromIJV(Class dclass, int rows, int cols, int nnz, 
			  uint32* I, int istride, uint32 *J, int jstride,
			  const void* cp, int cpstride) {
    switch(dclass) {
    case FM_INT32:
      return makeSparseFromIJVReal<int32>(rows,cols,nnz,I,istride,
					  J,jstride,(int32*)cp,cpstride);
    case FM_FLOAT:
      return makeSparseFromIJVReal<float>(rows,cols,nnz,I,istride,
					  J,jstride,(float*)cp,cpstride);
    case FM_DOUBLE:
      return makeSparseFromIJVReal<double>(rows,cols,nnz,I,istride,
					   J,jstride,(double*)cp,cpstride);
    case FM_COMPLEX:
      return makeSparseFromIJVComplex<float>(rows,cols,nnz,I,istride,
					     J,jstride,(float*)cp,cpstride);
    case FM_DCOMPLEX:
      return makeSparseFromIJVComplex<double>(rows,cols,nnz,I,istride,
					      J,jstride,(double*)cp,cpstride);
    }
  }
			  

  void DeleteSparseMatrix(Class dclass, int rows, int cols, void *cp) {
    switch(dclass) {
    case FM_INT32:
      DeleteSparse<int32>((int32**)cp,rows,cols);
      return;
    case FM_FLOAT:
      DeleteSparse<float>((float**)cp,rows,cols);
      return;
    case FM_DOUBLE:
      DeleteSparse<double>((double**)cp,rows,cols);
      return;
    case FM_COMPLEX:
      DeleteSparse<float>((float**)cp,rows,cols);
      return;
    case FM_DCOMPLEX:
      DeleteSparse<double>((double**)cp,rows,cols);
      return;
    }
  }
  

  template <class T, class S>
  S** TypeConvertSparseRealReal(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    T* src_col;
    src_col = new T[rows];
    S* dst_col;
    dst_col = new S[rows];
    int i, j;
    for (i=0;i<cols;i++) {
      DecompressRealString<T>(src[i],src_col,rows);
      for (j=0;j<rows;j++)
	dst_col[j] = (S) src_col[j];
      dp[i] = CompressRealVector<S>(dst_col,rows);
    }
    delete[] src_col;
    delete[] dst_col;
    return dp;
  }

  template <class T, class S>
  S** TypeConvertSparseComplexComplex(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    T* src_col;
    src_col = new T[2*rows];
    S* dst_col;
    dst_col = new S[2*rows];
    int i, j;
    for (i=0;i<cols;i++) {
      DecompressComplexString<T>(src[i],src_col,rows);
      for (j=0;j<2*rows;j++)
	dst_col[j] = (S) src_col[j];
      dp[i] = CompressComplexVector<S>(dst_col,rows);
    }
    delete[] src_col;
    delete[] dst_col;
    return dp;
  }

  template <class T, class S>
  S** TypeConvertSparseRealComplex(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    T* src_col;
    src_col = new T[rows];
    S* dst_col;
    dst_col = new S[rows*2];
    int i, j;
    for (i=0;i<cols;i++) {
      DecompressRealString<T>(src[i],src_col,rows);
      for (j=0;j<rows;j++) {
	dst_col[2*j] = (S) src_col[j];
	dst_col[2*j+1] = 0;
      }
      dp[i] = CompressComplexVector<S>(dst_col,rows);
    }
    delete[] src_col;
    delete[] dst_col;
    return dp;
  }

  template <class T, class S>
  S** TypeConvertSparseComplexReal(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    T* src_col;
    src_col = new T[rows*2];
    S* dst_col;
    dst_col = new S[rows];
    int i, j;
    for (i=0;i<cols;i++) {
      DecompressComplexString<T>(src[i],src_col,rows);
      for (j=0;j<rows;j++)
	dst_col[j] = (S) src_col[2*j];
      dp[i] = CompressRealVector<S>(dst_col,rows);
    }
    delete[] src_col;
    delete[] dst_col;
    return dp;
  }

  template <class T>
  void* CopySparseMatrix(const T** src, int rows, int cols) {
    T** dp;
    dp = new T*[cols];
    int i;
    for (i=0;i<cols;i++) {
      int blen = (int)(src[i][0]+1);
      dp[i] = new T[blen];
      memcpy(dp[i],src[i],sizeof(T)*blen);
    }
    return dp;
  }

  template <class T>
  int CountNonzerosComplex(const T** src, int rows, int cols) {
    int nnz = 0;
    int i, j, n;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if ((src[i][n] != 0) || (src[i][n+1] != 0)){
	  nnz++;
	  j++;
	  n+=2;
	} else {
	  j += (int) src[i][n+2];
	  n += 3;
	}
      }
    }
    return nnz;
  }

  template <class T>
  int CountNonzerosReal(const T** src, int rows, int cols) {
    int nnz = 0;
    int i, j, n;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if (src[i][n] != 0) {
	  nnz++;
	  j++;
	  n++;
	} else {
	  j += (int) src[i][n+1];
	  n += 2;
	}
      }
    }
    return nnz;
  }

  template <class T>
  T* SparseToIJVComplex(const T**src, int rows, int cols, uint32* &I, uint32* &J, int &nnz) {
    nnz = CountNonzerosComplex<T>(src,rows,cols);
    I = (uint32*) Malloc(sizeof(uint32)*nnz);
    J = (uint32*) Malloc(sizeof(uint32)*nnz);
    T* V = (T*) Malloc(sizeof(T)*nnz*2);
    int i, j, n;
    int ptr = 0;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if ((src[i][n] != 0) || (src[i][n+1] != 0)) {
	  I[ptr] = j+1;
	  J[ptr] = i+1;
	  V[2*ptr] = src[i][n];
	  V[2*ptr+1] = src[i][n+1];
	  ptr++;
	  j++;
	  n+=2;
	} else {
	  j += (int) src[i][n+2];
	  n += 3;
	}
      }
    }
    return V;
  }

  template <class T>
  T* SparseToIJVReal(const T**src, int rows, int cols, uint32* &I, uint32* &J, int &nnz) {
    nnz = CountNonzerosReal<T>(src,rows,cols);
    I = (uint32*) Malloc(sizeof(uint32)*nnz);
    J = (uint32*) Malloc(sizeof(uint32)*nnz);
    T* V = (T*) Malloc(sizeof(T)*nnz);
    int i, j, n;
    int ptr = 0;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if (src[i][n] != 0) {
	  I[ptr] = j+1;
	  J[ptr] = i+1;
	  V[ptr] = src[i][n];
	  ptr++;
	  j++;
	  n++;
	} else {
	  j += (int) src[i][n+1];
	  n += 2;
	}
      }
    }
    return V;
  }

  // Convert a sparse matrix to IJV
  void* SparseToIJV(Class dclass, int rows, int cols, const void* cp,
		    uint32* &I, uint32* &J, int &nnz) {
    switch (dclass) {
    case FM_INT32:
      return SparseToIJVReal<int32>((const int32**)cp,rows,cols,I,J,nnz);
    case FM_FLOAT:
      return SparseToIJVReal<float>((const float**)cp,rows,cols,I,J,nnz);
    case FM_DOUBLE:
      return SparseToIJVReal<double>((const double**)cp,rows,cols,I,J,nnz);
    case FM_COMPLEX:
      return SparseToIJVComplex<float>((const float**)cp,rows,cols,I,J,nnz);
    case FM_DCOMPLEX:
      return SparseToIJVComplex<double>((const double**)cp,rows,cols,I,J,nnz);
    }
  }

  void* CopySparseMatrix(Class dclass, int rows, int cols, const void* cp) { 
    switch (dclass) {
    case FM_INT32:
      return CopySparseMatrix<int32>((const int32**)cp,rows,cols);
    case FM_FLOAT:
      return CopySparseMatrix<float>((const float**)cp,rows,cols);
    case FM_DOUBLE:
      return CopySparseMatrix<double>((const double**)cp,rows,cols);
    case FM_COMPLEX:
      return CopySparseMatrix<float>((const float**)cp,rows,cols);
    case FM_DCOMPLEX:
      return CopySparseMatrix<double>((const double**)cp,rows,cols);
    }
  }

  int CountNonzeros(Class dclass, int rows, int cols, const void *cp) {
    switch (dclass) {
    case FM_INT32:
      return CountNonzerosReal<int32>((const int32**)cp,rows,cols);
    case FM_FLOAT:
      return CountNonzerosReal<float>((const float**)cp,rows,cols);
    case FM_DOUBLE:
      return CountNonzerosReal<double>((const double**)cp,rows,cols);
    case FM_COMPLEX:
      return CountNonzerosComplex<float>((const float**)cp,rows,cols);
    case FM_DCOMPLEX:
      return CountNonzerosComplex<double>((const double**)cp,rows,cols);
    }
  }

  // Multiply a sparse matrix by a sparse matrix (result is sparse)
  // Here we use the following order for the loops:
  // A normal multiply is
  // for j=1:Ccols
  //   for k=1:Acols
  //     for i=1:Arows
  //       c[i][j] += a[i][k]*b[k][j]
  template <class T>
  void SparseSparseRealMultiply(T** A, int A_rows, int A_cols,
				T** B, int B_cols,
				T** C) {
    T* CColumn;
    int i, j, k, m, n;
    
    CColumn = new T[A_rows];
    for (j=0;j<B_cols;j++) {
      memset(CColumn,0,A_rows*sizeof(T));
      k=0;
      m=1;
      while (k<A_cols) {
	if (B[j][m] != 0) {
	  T Bval = B[j][m];
	  T* Acol = A[k];
	  i=0;
	  n=1;
	  while (i<A_rows) {
	    if (Acol[n] != 0) {
	      CColumn[i] += Acol[n]*Bval;
	      n++;
	      i++;
	    } else {
	      i += (int) Acol[n+1];
	      n += 2;
	    }
	  }
	  k++;
	  m++;
	} else {
	  k += (int) B[j][m+1];
	  m += 2;
	}
      }
      C[j] = CompressRealVector(CColumn, A_rows);
    }
    delete CColumn;
  }

  // Multiply a sparse matrix by a dense matrix (result is dense)
  // A normal muliply is
  // for i=1:Arows
  //   for j=1:Ccols
  //     c[i][j] = 0;
  //     for k = 1:Acols
  //       c[i][j] += a[i][k]*b[k][j]
  // If we assume c is initialized to zero, we can write:
  // for i=1:Arows
  //   for j=1:Ccols
  //     for k = 1:Acols
  //       c[i][j] += a[i][k]*b[k][j]
  // And change the order of the loops:
  // for k=1:Acols
  //   for j=1:Bcols
  //      for i=1:Arows
  //       c[i][j] += a[i][k]*b[k][j]
  // The inner loop is now striding over the compressed
  // dimension of A, which is the best case.  Also, we can
  // apply a simple zero-check to b[k][j] and skip loops
  // over i for which it is zero.
  template <class T>
  void SparseDenseRealMultiply(T**A, int A_rows, int A_cols,
			       T*B, int B_cols,
			       T*C) {
    int i, j, k, n;
    T* Acol;
    T Bval;
    memset(C,0,A_rows*B_cols*sizeof(T));
    for (k=0;k<A_cols;k++) {
      Acol = A[k];
      for (j=0;j<B_cols;j++) {
	Bval = B[k+j*A_cols];
	if (Bval != 0) {
	  i=0;
	  n=1;
	  while (i<A_rows) {
	    if (Acol[n] != 0) {
	      C[i+j*A_rows] += Acol[n]*Bval;
	      n++;
	      i++;
	    } else {
	      i += (int) Acol[n+1];
	      n += 2;
	    }
	  }
	}
      }
    }
  }
  
  // for i=1:Arows
  //   for j=1:Ccols
  //     for k = 1:Acols
  //       c[i][j] += a[i][k]*b[k][j]
  // Multiply a dense matrix by a sparse matrix (result is dense)
  // If we move the I loop inside, we can use the sparsity of B
  // to our advantage
  // for j=1:Ccols
  //   for k = 1:Acols
  //     for i=1:Arows
  //       c[i][j] += a[i][k]*b[k][j]
  template <class T>
  void DenseSparseRealMultiply(T* A, int A_rows, int A_cols,
			       T** B, int B_cols, T* C) {
    int i, j, k, n;
    int B_rows;
    T* str;
    T accum;
    memset(C,0,sizeof(T)*A_rows*B_cols);
    B_rows = A_cols;
    for (j=0;j<B_cols;j++) {
      str = B[j];
      k = 1;
      n = 0;
      while (n<B_rows) {
	if (str[k] != 0) {
	  for (i=0;i<A_rows;i++)
	    C[i+j*A_rows] += A[i+n*A_rows]*str[k];
	  n++;
	  k++;
	} else {
	  n += (int) (str[k+1]);
	  k += 2;
	}
      }
    }
  }
  

  void* SparseDenseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				  const void* ap, const void* bp) {
    switch (dclass) {
    case FM_FLOAT: 
      {
	float *C = (float*) Malloc(rows*bcols*sizeof(float));
	SparseDenseRealMultiply<float>((float**)ap,rows,cols,
				       (float*)bp,bcols,C);
	return C;
      }
    case FM_DOUBLE: 
      {
	double *C = (double*) Malloc(rows*bcols*sizeof(double));
	SparseDenseRealMultiply<double>((double**)ap,rows,cols,
					(double*)bp,bcols,C);
	return C;
      }
    otherwise:
      throw Exception("Complex multiplies with sparse matrices not handled.");
    }
  }
  
  void* DenseSparseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				  const void* ap, const void* bp) {
    switch (dclass) {
    case FM_FLOAT: 
      {
	float *C = (float*) Malloc(rows*bcols*sizeof(float));
	DenseSparseRealMultiply<float>((float*)ap,rows,cols,
				       (float**)bp,bcols,C);
	return C;
      }
    case FM_DOUBLE: 
      {
	double *C = (double*) Malloc(rows*bcols*sizeof(double));
	DenseSparseRealMultiply<double>((double*)ap,rows,cols,
					(double**)bp,bcols,C);
	return C;
      }
    otherwise:
      throw Exception("Complex multiplies with sparse matrices not handled.");
    }
  }

  void* SparseSparseMatrixMultiply(Class dclass, int rows, int cols, int bcols,
				   const void* ap, const void* bp) {
    switch (dclass) {
    case FM_FLOAT: 
      {
	float **C = new float*[bcols];
	SparseSparseRealMultiply<float>((float**)ap,rows,cols,
					(float**)bp,bcols,C);
	return C;
      }
    case FM_DOUBLE: 
      {
	double **C = new double*[bcols];
	SparseSparseRealMultiply<double>((double**)ap,rows,cols,
					 (double**)bp,bcols,C);
	return C;
      }
    otherwise:
      throw Exception("Complex multiplies with sparse matrices not handled.");
    }
  }

  /* Type convert the given sparse array and delete it */
  /* dclass is the source type, oclass is the destination class*/
  void* TypeConvertSparse(Class dclass, int rows, int cols, 
			  const void *cp, Class oclass) {
    if (dclass == FM_INT32) {
      switch(oclass) {
      case FM_FLOAT:
	return TypeConvertSparseRealReal<int32,float>((int32**)cp,rows,cols);
      case FM_DOUBLE:
	return TypeConvertSparseRealReal<int32,double>((int32**)cp,rows,cols);
      case FM_COMPLEX:
	return TypeConvertSparseRealComplex<int32,float>((int32**)cp,rows,cols);
      case FM_DCOMPLEX:
	return TypeConvertSparseRealComplex<int32,double>((int32**)cp,rows,cols);
      }
    } else if (dclass == FM_FLOAT) {
      switch(oclass) {
      case FM_INT32:
	return TypeConvertSparseRealReal<float,int32>((float**)cp,rows,cols);
      case FM_DOUBLE:
	return TypeConvertSparseRealReal<float,double>((float**)cp,rows,cols);
      case FM_COMPLEX:
	return TypeConvertSparseRealComplex<float,float>((float**)cp,rows,cols);
      case FM_DCOMPLEX:
	return TypeConvertSparseRealComplex<float,double>((float**)cp,rows,cols);
      }
    } else if (dclass == FM_DOUBLE) {
      switch(oclass) {
      case FM_INT32:
	return TypeConvertSparseRealReal<double,int32>((double**)cp,rows,cols);
      case FM_FLOAT:
	return TypeConvertSparseRealReal<double,float>((double**)cp,rows,cols);
      case FM_COMPLEX:
	return TypeConvertSparseRealComplex<double,float>((double**)cp,rows,cols);
      case FM_DCOMPLEX:
	return TypeConvertSparseRealComplex<double,double>((double**)cp,rows,cols);
      }
    } else if (dclass == FM_COMPLEX) {
      switch(oclass) {
      case FM_INT32:
	return TypeConvertSparseComplexReal<float,int32>((float**)cp,rows,cols);
      case FM_FLOAT:
	return TypeConvertSparseComplexReal<float,float>((float**)cp,rows,cols);
      case FM_DOUBLE:
	return TypeConvertSparseComplexReal<float,double>((float**)cp,rows,cols);
      case FM_DCOMPLEX:
	return TypeConvertSparseComplexComplex<float,double>((float**)cp,rows,cols);
      }
    } else if (dclass == FM_DCOMPLEX) {
      switch(oclass) {
      case FM_INT32:
	return TypeConvertSparseComplexReal<double,int32>((double**)cp,rows,cols);
      case FM_FLOAT:
	return TypeConvertSparseComplexReal<double,float>((double**)cp,rows,cols);
      case FM_DOUBLE:
	return TypeConvertSparseComplexReal<double,double>((double**)cp,rows,cols);
      case FM_COMPLEX:
	return TypeConvertSparseComplexComplex<double,float>((double**)cp,rows,cols);
      }
    } 
  }

  // The strategy is straightforward, we loop over the output columns
  // We need one pointer for each row of the array matrix
  template <class T>
  void* SparseMatrixConst(int rows, int cols, ArrayMatrix m) {
    T** dst;
    dst = new T*[cols];
    // The blockindx array tracks which "block" within each 
    // row is active
    int *blockindx = new int[m.size()];
    // The colindx array tracks which column within each block
    // is active
    int *colindx = new int[m.size()];
    int j;
    for (j=0;j<m.size();j++) {
      blockindx[j] = 0;
      colindx[j] = 0;
    }
    for (int i=0;i<cols;i++) {
      // Working on column i
      // For the current column, we loop over all rows of m, and
      // concatenate their strings together.
      int outstringlength = 0;
      for (j=0;j<m.size();j++) {
	T** src;
	src = (T**) m[j][blockindx[j]].getSparseDataPointer();
	// Get the string length
	outstringlength += (int) src[colindx[j]][0];
      }
      // Allocate space for the output string
      dst[i] = new T[outstringlength+1];
      dst[i][0] = (T) outstringlength;
      // Loop over the rows again, copying them into the new string
      // buffer
      outstringlength = 0;
      for (j=0;j<m.size();j++) {
	T** src;
	src = (T**) m[j][blockindx[j]].getSparseDataPointer();
	memcpy(dst[i]+outstringlength+1,src[colindx[j]]+1,
	       ((int) src[colindx[j]][0])*sizeof(T));
	outstringlength += (int) src[colindx[j]][0];
      }
      // Now we have to update the column pointers
      for (j=0;j<m.size();j++) {
	colindx[j]++;
	if (colindx[j] >= m[j][blockindx[j]].getDimensionLength(1)) {
	  blockindx[j]++;
	  colindx[j] = 0;
	}
      }
    }
    delete blockindx;
    delete colindx;
    return dst;
  } 

  void* SparseMatrixConstructor(Class dclass, int rows, int cols,
				ArrayMatrix m) {
    // Precondition the arrays by converting to sparse and to
    // the output type
    for (ArrayMatrix::iterator i=m.begin();i != m.end();i++) {
      for (ArrayVector::iterator j= i->begin(); j != i->end();j++) {
	j->promoteType(dclass);
	j->makeSparse();
      }
    }
    // Now, we can construct the output array
    switch (dclass) {
    case FM_INT32:
      return SparseMatrixConst<int32>(rows, cols, m);
    case FM_FLOAT:
      return SparseMatrixConst<float>(rows, cols, m);
    case FM_DOUBLE:
      return SparseMatrixConst<double>(rows, cols, m);
    case FM_COMPLEX:
      return SparseMatrixConst<float>(rows, cols, m);
    case FM_DCOMPLEX:
      return SparseMatrixConst<double>(rows, cols, m);
    }    
  }

  void *SparseFloatZeros(int rows, int cols) {
    float **src;
    src = new float*[cols];
    int i;
    for (i=0;i<cols;i++) {
      src[i] = new float[3];
      src[i][0] = 2;
      src[i][1] = 0;
      src[i][2] = rows;
    }
    return src;
  }


  template <class T>
  void RealStringExtract(const T* src, int ndx, T* dst) {
    int i=0;
    int n=1;
    int j;
    while (i<ndx) {
      if (src[n] != 0) {
	i++;
	n++;
      } else {
	j = (int) src[n+1];
	i += j;
	n += 2;
      }
    }
    if (i==ndx)
      *dst = src[n];
    else
      *dst = 0;
  }

  template <class T>
  void ComplexStringExtract(const T* src, int ndx, T* dst) {
    int i=0;
    int n=1;
    int j;
    while (i<2*ndx) {
      if ((src[n] != 0) || (src[n+1] != 0)) {
	i+=2;
	n+=2;
      } else {
	j = (int) src[n+2];
	i += j*2;
	n += 3;
      }
    }
    if (i==2*ndx) {
      dst[0] = src[n];
      dst[1] = src[n+1];
    } else {
      dst[0] = 0;
      dst[1] = 0;
    }
  }
  
  template <class T>
  IJVEntry<T>* ConvertSparseToIJVListReal(const T** src, int rows, int cols, int &nnz) {
    nnz = CountNonzerosReal<T>(src,rows,cols);
    IJVEntry<T>* mlist = new IJVEntry<T>[nnz];
    int ptr = 0;
    int i, j, n;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if (src[i][n] != 0) {
	  mlist[ptr].I = j;
	  mlist[ptr].J = i;
	  mlist[ptr].Vreal = src[i][n];
	  ptr++;
	  j++;
	  n++;
	} else {
	  j += (int) src[i][n+1];
	  n+=2;
	}
      }
    }    
    return mlist;
  }

  template <class T>
  IJVEntry<T>* ConvertSparseToIJVListComplex(const T** src, int rows, int cols, int &nnz) { 
    nnz = CountNonzerosComplex<T>(src,rows,cols);
    IJVEntry<T>* mlist = new IJVEntry<T>[nnz];
    int ptr = 0;
    int i, j, n;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if ((src[i][n] != 0) || (src[i][n+1] != 0)) {
	  mlist[ptr].I = j;
	  mlist[ptr].J = i;
	  mlist[ptr].Vreal = src[i][n];
	  mlist[ptr].Vimag = src[i][n+1];
	  ptr++;
	  j++;
	  n+=2;
	} else {
	  j += (int) src[i][n+2];
	  n+=3;
	}
      }
    }
    return mlist;
  }

  // The original implementation is poor because for a linear access of the elements
  // of A, it requires significant time.  A better approach is to build a sorted list
  // of the type:
  //              ord indx_row indx_col
  // and then sort by the indx_col (and subsort by indx_row).  We can then loop over
  // the indx_col's and populate the output with the corresponding element ord.  The
  // reason this approach is best is because of the assignment version (which is equally
  // complicated).
  //
  // The problem is that the output needs to be resorted by ord to build the 
  // sparse output matrix.  I think the IJV notation is in order here...
  //
  // So the process is:
  //  1. Expand the sparse matrix into IJV format
  //  2. Sort the get array by linear index
  //  3. Do a sorted list compare/extract --> IJV format
  //  4. Recode IJV --> sparse output
  // Now the get array has 4 pieces of information - source row & col,
  // and dest row & col.  All of this information can be packed into
  // an IJVEntry...
  template <class T>
  void* GetSparseVectorSubsetsReal(int rows, int cols, const T** A,
				   const indexType* indx, int irows, int icols) {
    // Convert the sparse matrix into IJV format
    IJVEntry<T>* mlist;
    int nnz;
    mlist = ConvertSparseToIJVListReal<T>(A,rows,cols,nnz);
    // Unpack the indexing array into IJV format
    IJVEntry<T>* ilist;
    int icount;
    icount = irows*icols;
    ilist = new IJVEntry<T>[icount];
    int i, j;
    for (i=0;i<icount;i++) {
      ilist[i].I = (indx[i]-1) % rows;
      ilist[i].J = (indx[i]-1) / rows;
      ilist[i].Vreal = (int) (i % irows);
      ilist[i].Vimag = (int) (i / irows);
    }
    // Sort the indexing array.
    std::sort(ilist,ilist+icount);
    // Now do a merge operation between the
    // indexing array and the source array.
    int iptr;
    int jptr;
    jptr = 0;
    for (iptr=0;iptr<icount;iptr++) {
      while ((jptr < nnz) && ((mlist[jptr].J < ilist[iptr].J) ||
			      ((mlist[jptr].J == ilist[iptr].J) &&
			       (mlist[jptr].I < ilist[iptr].I))))
	jptr++;
      if ((jptr < nnz) && (mlist[jptr].I == ilist[iptr].I)
	  && (mlist[jptr].J == ilist[iptr].J)) {
	ilist[iptr].I = (int) ilist[iptr].Vreal;
	ilist[iptr].J = (int) ilist[iptr].Vimag;
	ilist[iptr].Vreal = mlist[jptr].Vreal;
      } else {
	ilist[iptr].I = (int) ilist[iptr].Vreal;
	ilist[iptr].J = (int) ilist[iptr].Vimag;
	ilist[iptr].Vreal = 0;
      }
    }
    // Now resort the list
    std::sort(ilist,ilist+icount);
    // Build a sparse matrix out of this
    T** B;
    B = new T*[icols];
    int ptr = 0;
    for (int col=0;col<icols;col++)
      B[col] = CompressRealIJV<T>(ilist,icount,ptr,col,irows);
    delete[] mlist;
    delete[] ilist;
    return B;
  }

  template <class T>
  void* SetSparseVectorSubsetsReal(int &rows, int &cols, const T** A,
				   const indexType* indx, int irows, int icols,
				   const T* data, int advance) {
    // Convert the sparse matrix into IJV format
    IJVEntry<T>* mlist;
    int nnz;
    mlist = ConvertSparseToIJVListReal<T>(A,rows,cols,nnz);
    // Unpack the indexing array into IJV format
    IJVEntry<T>* ilist;
    int icount;
    icount = irows*icols;
    // check to see if a resize is necessary...
    int maxindx = indx[0];
    int i, j;
    for (i=0;i<icount;i++)
      maxindx = (maxindx > indx[i]) ? maxindx : indx[i];
    // if maxindx is larger than rows*cols, we have to
    // vector resize
    if (maxindx > rows*cols) {
      // To vector resize, we set rows = maxindx, cols = 1, but
      // we also adjust the row & column index of each IJVentry
      // first
      for (i=0;i<nnz;i++) {
	mlist[i].I += rows*mlist[i].J;
	mlist[i].J = 0;
      }
      rows = maxindx; 
      cols = 1;
    }
    ilist = new IJVEntry<T>[icount];
    for (i=0;i<icount;i++) {
      ilist[i].I = (indx[i]-1) % rows;
      ilist[i].J = (indx[i]-1) / rows;
      ilist[i].Vreal = data[advance*i];
    }
    
    // Sort the indexing array.
    std::sort(ilist,ilist+icount);
    // Create the destination array
    IJVEntry<T>* dlist;
    dlist = new IJVEntry<T>[icount+nnz];
    // Now do a merge operation between the
    // indexing array and the source array.
    int iptr;
    int jptr;
    int optr;
    jptr = 0;
    optr = 0;
    for (iptr=0;iptr<icount;iptr++) {
      while ((jptr < nnz) && ((mlist[jptr].J < ilist[iptr].J) ||
			      ((mlist[jptr].J == ilist[iptr].J) &&
			       (mlist[jptr].I < ilist[iptr].I)))) {
	dlist[optr++] = mlist[jptr];
	jptr++;
      }
      dlist[optr++] = ilist[iptr];
      if ((jptr < nnz) && (mlist[jptr].I == ilist[iptr].I)
	  && (mlist[jptr].J == ilist[iptr].J)) 
	jptr++;
    }
    while (jptr < nnz)
      dlist[optr++] = mlist[jptr++];
    // Now resort the list
    std::sort(dlist,dlist+optr);
    printIJV(dlist,optr);
    // Build a sparse matrix out of this
    T** B;
    B = new T*[cols];
    int ptr = 0;
    for (int col=0;col<cols;col++)
      B[col] = CompressRealIJV<T>(dlist,optr,ptr,col,rows);
    delete[] mlist;
    delete[] ilist;
    delete[] dlist;
    return B;
  }

  template <class T>
  void* SetSparseVectorSubsetsComplex(int &rows, int &cols, const T** A,
				      const indexType* indx, int irows, 
				      int icols, const T* data, int advance) {
    // Convert the sparse matrix into IJV format
    IJVEntry<T>* mlist;
    int nnz;
    mlist = ConvertSparseToIJVListComplex<T>(A,rows,cols,nnz);
    // Unpack the indexing array into IJV format
    IJVEntry<T>* ilist;
    int icount;
    icount = irows*icols;
    // check to see if a resize is necessary...
    int maxindx = indx[0];
    int i, j;
    for (i=0;i<icount;i++)
      maxindx = (maxindx > indx[i]) ? maxindx : indx[i];
    // if maxindx is larger than rows*cols, we have to
    // vector resize
    if (maxindx > rows*cols) {
      // To vector resize, we set rows = maxindx, cols = 1, but
      // we also adjust the row & column index of each IJVentry
      // first
      for (i=0;i<nnz;i++) {
	mlist[i].I += rows*mlist[i].J;
	mlist[i].J = 0;
      }
      rows = maxindx; 
      cols = 1;
    }
    ilist = new IJVEntry<T>[icount];
    for (i=0;i<icount;i++) {
      ilist[i].I = (indx[i]-1) % rows;
      ilist[i].J = (indx[i]-1) / rows;
      ilist[i].Vreal = data[2*advance*i];
      ilist[i].Vimag = data[2*advance*i+1];
    }
    
    // Sort the indexing array.
    std::sort(ilist,ilist+icount);
    // Create the destination array
    IJVEntry<T>* dlist;
    dlist = new IJVEntry<T>[icount+nnz];
    // Now do a merge operation between the
    // indexing array and the source array.
    int iptr;
    int jptr;
    int optr;
    jptr = 0;
    optr = 0;
    for (iptr=0;iptr<icount;iptr++) {
      while ((jptr < nnz) && ((mlist[jptr].J < ilist[iptr].J) ||
			      ((mlist[jptr].J == ilist[iptr].J) &&
			       (mlist[jptr].I < ilist[iptr].I)))) {
	dlist[optr++] = mlist[jptr];
	jptr++;
      }
      dlist[optr++] = ilist[iptr];
      if ((jptr < nnz) && (mlist[jptr].I == ilist[iptr].I)
	  && (mlist[jptr].J == ilist[iptr].J)) 
	jptr++;
    }
    while (jptr < nnz)
      dlist[optr++] = mlist[jptr++];
    // Now resort the list
    std::sort(dlist,dlist+optr);
    printIJV(dlist,optr);
    // Build a sparse matrix out of this
    T** B;
    B = new T*[cols];
    int ptr = 0;
    for (int col=0;col<cols;col++)
      B[col] = CompressComplexIJV<T>(dlist,optr,ptr,col,rows);
    delete[] mlist;
    delete[] ilist;
    delete[] dlist;
    return B;
  }

  // This implementation is poor because for a linear access of the elements
  // of A, it requires significant time.
  template <class T>
  void* GetSparseVectorSubsetsComplex(int &rows, int &cols, const T** A,
				      const indexType* indx, int irows, 
				      int icols) {
    // Convert the sparse matrix into IJV format
    IJVEntry<T>* mlist;
    int nnz;
    mlist = ConvertSparseToIJVListComplex<T>(A,rows,cols,nnz);
    // Unpack the indexing array into IJV format
    IJVEntry<T>* ilist;
    int icount;
    icount = irows*icols;
    ilist = new IJVEntry<T>[icount];
    int i, j;
    for (i=0;i<icount;i++) {
      ilist[i].I = (indx[i]-1) % rows;
      ilist[i].J = (indx[i]-1) / rows;
      ilist[i].Vreal = (int) (i % irows);
      ilist[i].Vimag = (int) (i / irows);
    }
    // Sort the indexing array.
    std::sort(ilist,ilist+icount);
    // Now do a merge operation between the
    // indexing array and the source array.
    int iptr;
    int jptr;
    jptr = 0;
    for (iptr=0;iptr<icount;iptr++) {
      while ((jptr < nnz) && ((mlist[jptr].J < ilist[iptr].J) ||
			      ((mlist[jptr].J == ilist[iptr].J) &&
			       (mlist[jptr].I < ilist[iptr].I))))
	jptr++;
      if ((jptr < nnz) && (mlist[jptr].I == ilist[iptr].I)
	  && (mlist[jptr].J == ilist[iptr].J)) {
	ilist[iptr].I = (int) ilist[iptr].Vreal;
	ilist[iptr].J = (int) ilist[iptr].Vimag;
	ilist[iptr].Vreal = mlist[jptr].Vreal;
	ilist[iptr].Vimag = mlist[jptr].Vimag;
      } else {
	ilist[iptr].I = (int) ilist[iptr].Vreal;
	ilist[iptr].J = (int) ilist[iptr].Vimag;
	ilist[iptr].Vreal = 0;
	ilist[iptr].Vimag = 0;
      }
    }
    // Now resort the list
    std::sort(ilist,ilist+icount);
    // Build a sparse matrix out of this
    T** B;
    B = new T*[icols];
    int ptr = 0;
    for (int col=0;col<icols;col++)
      B[col] = CompressComplexIJV<T>(ilist,icount,ptr,col,irows);
    delete[] mlist;
    delete[] ilist;
    return B;
  }

  // GetSparseVectorSubsets - This one is a bit difficult to do efficiently.
  // For each column in the output, we have to extract potentially random
  // elements from the source array.
  void* GetSparseVectorSubsets(Class dclass, int rows, int cols, const void* src,
			       const indexType* indx, int irows, int icols) {
    switch (dclass) {
    case FM_INT32:
      return GetSparseVectorSubsetsReal<int32>(rows, cols, (const int32**) src,
					       indx, irows, icols);
    case FM_FLOAT:
      return GetSparseVectorSubsetsReal<float>(rows, cols, (const float**) src,
					       indx, irows, icols);
    case FM_DOUBLE:
      return GetSparseVectorSubsetsReal<double>(rows, cols, (const double**) src,
						indx, irows, icols);
    case FM_COMPLEX:
      return GetSparseVectorSubsetsComplex<float>(rows, cols, (const float**) src,
      						  indx, irows, icols);
    case FM_DCOMPLEX:
      return GetSparseVectorSubsetsComplex<double>(rows, cols, (const double**) src,
      						   indx, irows, icols);
    }
  }

  // SetSparseVectorSubsets - This one is a bit difficult to do efficiently.
  // For each column in the output, we have to extract potentially random
  // elements from the source array.
  void* SetSparseVectorSubsets(Class dclass, int &rows, int &cols, 
			       const void* src,
			       const indexType* indx, int irows, int icols,
			       const void* data, int advance) {
    switch (dclass) {
    case FM_INT32:
      return SetSparseVectorSubsetsReal<int32>(rows, cols, (const int32**) src,
					       indx, irows, icols, 
					       (const int32*) data, advance);
    case FM_FLOAT:
      return SetSparseVectorSubsetsReal<float>(rows, cols, (const float**) src,
					       indx, irows, icols, 
					       (const float*) data, advance);
    case FM_DOUBLE:
      return SetSparseVectorSubsetsReal<double>(rows, cols, 
						(const double**) src,
						indx, irows, icols, 
						(const double*) data, 
						advance);
    case FM_COMPLEX:
      return SetSparseVectorSubsetsComplex<float>(rows, cols, 
 						  (const float**) src,
       						  indx, irows, icols, 
 						  (const float*) data, 
 						  advance);
    case FM_DCOMPLEX:
      return SetSparseVectorSubsetsComplex<double>(rows, cols, 
 						   (const double**) src,
       						   indx, irows, icols, 
						   (const double*) data, 
 						   advance);
    }
  }

  template <class T>
  void* GetSparseNDimSubsetsReal(int rows, int cols, const T** A,
				 const indexType* rindx, int irows, 
				 const indexType* cindx, int icols) {
    int i, j, n, m;
    int nrow, ncol;
    T* Acol = new T[rows];
    T* Bcol = new T[irows];
    T** dp = new T*[icols];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if ((m<0) || (m>=cols))
	throw Exception("column index exceeds variable bounds in expression of type A(n,m)");
      DecompressRealString<T>(A[m],Acol,rows);
      memset(Bcol,0,irows*sizeof(T));
      for (j=0;j<irows;j++) {
	// Get the row index
	n = rindx[j] - 1;
	if ((n<0) || (n>=rows))
	  throw Exception("row index exceeds variable bounds in expression of type A(n,m)");
	Bcol[j] = Acol[n];
      }
      dp[i] = CompressRealVector<T>(Bcol,irows);
    }
    delete Acol;
    delete Bcol;
    return dp;
  }


  template <class T>
  void* GetSparseNDimSubsetsComplex(int rows, int cols, const T** A,
				    const indexType* rindx, int irows, 
				    const indexType* cindx, int icols) {
    int i, j, n, m;
    int nrow, ncol;
    T* Acol = new T[2*rows];
    T* Bcol = new T[2*irows];
    T** dp = new T*[icols];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if ((m<0) || (m>=cols))
	throw Exception("column index exceeds variable bounds in expression of type A(n,m)");
      DecompressComplexString<T>(A[m],Acol,rows);
      memset(Bcol,0,2*irows*sizeof(T));
      for (j=0;j<irows;j++) {
	// Get the row index
	n = rindx[j] - 1;
	if ((n<0) || (n>=rows))
	  throw Exception("row index exceeds variable bounds in expression of type A(n,m)");
	Bcol[2*j] = Acol[2*n];
	Bcol[2*j+1] = Acol[2*n+1];
      }
      dp[i] = CompressComplexVector<T>(Bcol,irows);
    }
    delete Acol;
    delete Bcol;
    return dp;
  }

  // GetSparseNDimSubsets
  void* GetSparseNDimSubsets(Class dclass, int rows, int cols, const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols) {
    switch(dclass) {
    case FM_INT32:
      return GetSparseNDimSubsetsReal<int32>(rows, cols, (const int32**) src,
					     rindx, irows, cindx, icols);
    case FM_FLOAT:
      return GetSparseNDimSubsetsReal<float>(rows, cols, (const float**) src,
					     rindx, irows, cindx, icols);
    case FM_DOUBLE:
      return GetSparseNDimSubsetsReal<double>(rows, cols, (const double**) src,
					      rindx, irows, cindx, icols);
    case FM_COMPLEX:
      return GetSparseNDimSubsetsComplex<float>(rows, cols, (const float**) src,
						rindx, irows, cindx, icols);
    case FM_DCOMPLEX:
      return GetSparseNDimSubsetsComplex<double>(rows, cols, (const double**) src,
						 rindx, irows, cindx, icols);
    }
  }

  template <class T>
  void* GetSparseScalarReal(int rows, int cols, const T** src,
			    int rindx, int cindx) {
    T* ret = (T*) Malloc(sizeof(T)*1);
    rindx -= 1;
    cindx -= 1;
    if ((rindx<0) || (rindx >= rows))
      throw Exception("out of range row index in sparse scalar indexing expression A(m,n)");
    if ((cindx<0) || (cindx >= cols))
      throw Exception("out of range col index in sparse scalar indexing expression A(m,n)");
    RealStringExtract(src[cindx],rindx,ret);
    return ret;
  }

  template <class T>
  void* GetSparseScalarComplex(int rows, int cols, const T** src,
			       int rindx, int cindx) {
    T* ret = (T*) Malloc(sizeof(T)*2);
    rindx -= 1;
    cindx -= 1;
    if ((rindx<0) || (rindx >= rows))
      throw Exception("out of range row index in sparse scalar indexing expression A(m,n)");
    if ((cindx<0) || (cindx >= cols))
      throw Exception("out of range col index in sparse scalar indexing expression A(m,n)");
    ComplexStringExtract(src[cindx],rindx,ret);
    return ret;
  }

  // GetSparseNDimSubsets
  void* GetSparseScalarElement(Class dclass, int rows, int cols, 
			       const void* src, indexType rindx, 
			       indexType cindx) {
    switch(dclass) {
    case FM_INT32:
      return GetSparseScalarReal<int32>(rows, cols, (const int32**) src,
					rindx, cindx);
    case FM_FLOAT:
      return GetSparseScalarReal<float>(rows, cols, (const float**) src,
					rindx, cindx);
    case FM_DOUBLE:
      return GetSparseScalarReal<double>(rows, cols, (const double**) src,
					 rindx, cindx);
    case FM_COMPLEX:
      return GetSparseScalarComplex<float>(rows, cols, (const float**) src,
					   rindx, cindx);
    case FM_DCOMPLEX:
      return GetSparseScalarComplex<double>(rows, cols, (const double**) src,
					    rindx, cindx);
    }
  }

  template <class T>
  void* CopyResizeSparseMatrixComplex(const T** src, int rows, int cols,
				      int maxrow, int maxcol) {
    T** dp;
    dp = new T*[maxcol];
    int i;
    for (i=0;i<cols;i++) {
      int blen = (int)(src[i][0]+1);
      if (maxrow == rows) {
	dp[i] = new T[blen];
	memcpy(dp[i],src[i],sizeof(T)*blen);
      } else {
	dp[i] = new T[blen+3];
	memcpy(dp[i],src[i],sizeof(T)*blen);
	dp[i][0] = blen+3;
	dp[i][blen] = 0;
	dp[i][blen+1] = 0;
	dp[i][blen+2] = (maxrow-rows);	
      }
    }
    for (i=cols;i<maxcol;i++) {
      dp[i] = new T[4];
      dp[i][0] = 3;
      dp[i][1] = 0;
      dp[i][2] = 0;
      dp[i][3] = maxrow;
    }
    return dp;
  }

  template <class T>
  void* CopyResizeSparseMatrixReal(const T** src, int rows, int cols,
				   int maxrow, int maxcol) {
    T** dp;
    dp = new T*[maxcol];
    int i;
    for (i=0;i<cols;i++) {
      int blen = (int)(src[i][0]+1);
      if (maxrow == rows) {
	dp[i] = new T[blen];
	memcpy(dp[i],src[i],sizeof(T)*blen);
      } else {
	dp[i] = new T[blen+2];
	memcpy(dp[i],src[i],sizeof(T)*blen);
	dp[i][0] = blen+2;
	dp[i][blen] = 0;
	dp[i][blen+1] = (maxrow-rows);
      }
    }
    for (i=cols;i<maxcol;i++) {
      dp[i] = new T[3];
      dp[i][0] = 2;
      dp[i][1] = 0;
      dp[i][2] = maxrow;
    }
    return dp;
  }

  template <class T>
  void* SetSparseNDimSubsetsReal(int &rows, int &cols, const T** A,
				 const indexType* rindx, int irows, 
				 const indexType* cindx, int icols,
				 const T* data, int advance) {
    int i, j, n, m;
    int nrow, ncol;

    T**dp;
    // Check for a resize...
    int maxrow, maxcol;
    maxrow = rindx[0];
    for (i=0;i<irows;i++)
      maxrow = (maxrow > rindx[i]) ? maxrow : rindx[i];
    maxcol = cindx[0];
    for (i=0;i<icols;i++)
      maxcol = (maxcol > cindx[i]) ? maxcol : cindx[i];
    if ((maxrow > rows) || (maxcol > cols)) {
      maxrow = maxrow > rows ? maxrow : rows;
      maxcol = maxcol > cols ? maxcol : cols;
      dp = (T**) CopyResizeSparseMatrixReal<T>(A,rows,cols,maxrow,maxcol);
      rows = maxrow;
      cols = maxcol;
    } else 
      dp = (T**) CopySparseMatrix<T>(A,rows,cols);

    T* Acol = new T[rows];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if (m<0)
	throw Exception("negative column index encountered in sparse matrix assigment of type A(n,m) = B");
      DecompressRealString<T>(dp[m],Acol,rows);
      for (j=0;j<irows;j++) {
	// Get the row index
	n = rindx[j] - 1;
	if (m<0)
	  throw Exception("negative row index encountered in sparse matrix assigment of type A(n,m) = B");
	Acol[n] = *data;
	data += advance;
      }
      delete dp[m];
      dp[m] = CompressRealVector<T>(Acol,rows);
    }
    delete Acol;
    return dp;
  }


  template <class T>
  void* SetSparseNDimSubsetsComplex(int &rows, int &cols, const T** A,
				    const indexType* rindx, int irows, 
				    const indexType* cindx, int icols,
				    const T* data, int advance) {
    int i, j, n, m;
    int nrow, ncol;

    T**dp;
    // Check for a resize...
    int maxrow, maxcol;
    maxrow = rindx[0];
    for (i=0;i<irows;i++)
      maxrow = (maxrow > rindx[i]) ? maxrow : rindx[i];
    maxcol = cindx[0];
    for (i=0;i<icols;i++)
      maxcol = (maxcol > cindx[i]) ? maxcol : cindx[i];
    if ((maxrow > rows) || (maxcol > cols)) {
      maxrow = maxrow > rows ? maxrow : rows;
      maxcol = maxcol > cols ? maxcol : cols;
      dp = (T**) CopyResizeSparseMatrixComplex<T>(A,rows,cols,maxrow,maxcol);
      rows = maxrow;
      cols = maxcol;
    } else 
      dp = (T**) CopySparseMatrix<T>(A,rows,cols);

    T* Acol = new T[2*rows];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if (m<0)
	throw Exception("negative column index encountered in sparse matrix assigment of type A(n,m) = B");
      DecompressComplexString<T>(dp[m],Acol,rows);
      for (j=0;j<irows;j++) {
	// Get the row index
	n = rindx[j] - 1;
	if (m<0)
	  throw Exception("negative row index encountered in sparse matrix assigment of type A(n,m) = B");
	Acol[2*n] = *data;
	Acol[2*n+1] = *(data+1);
	data += 2*advance;
      }
      delete dp[m];
      dp[m] = CompressComplexVector<T>(Acol,rows);
    }
    delete Acol;
    return dp;
  }

  void* SetSparseNDimSubsets(Class dclass, int &rows, int &cols, 
			     const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols,
			     const void *data, int advance) {
    switch(dclass) {
    case FM_INT32:
      return SetSparseNDimSubsetsReal<int32>(rows, cols, (const int32**) src,
					     rindx, irows, cindx, icols,
					     (const int32*) data, advance);
    case FM_FLOAT:
      return SetSparseNDimSubsetsReal<float>(rows, cols, (const float**) src,
					     rindx, irows, cindx, icols,
					     (const float*) data, advance);
    case FM_DOUBLE:
      return SetSparseNDimSubsetsReal<double>(rows, cols, (const double**) src,
					      rindx, irows, cindx, icols,
					      (const double*) data, advance);
    case FM_COMPLEX:
      return SetSparseNDimSubsetsComplex<float>(rows, cols, 
						(const float**) src,
						rindx, irows, cindx, icols,
						(const float*) data, advance);
    case FM_DCOMPLEX:
      return SetSparseNDimSubsetsComplex<double>(rows, cols, 
						 (const double**) src,
						 rindx, irows, cindx, icols,
						 (const double*) data, 
						 advance);
    }
  }
}
