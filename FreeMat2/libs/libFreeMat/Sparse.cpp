#include "Sparse.hpp"
#include "Malloc.hpp"
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
  void* CopySparseMatrix(T** src, int rows, int cols) {
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
  int CountNonzerosComplex(T** src, int rows, int cols) {
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
  int CountNonzerosReal(T** src, int rows, int cols) {
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
  void* SparseToIJVComplex(T**A, int rows, int cols) {
    int nnz = CountNonzerosComplex<T>(A,rows,cols);
    T* ret;
    ret = (T*) Malloc(sizeof(T)*nnz*4);
    int i, j, n;
    int ptr = 0;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if ((src[i][n] != 0) || (src[i][n+1] != 0)) {
	  T[ptr] = j+1;
	  T[ptr+nnz] = i+1;
	  T[ptr+2*nnz] = src[i][n];
	  T[ptr+3*nnz] = src[i][n+1];
	  ptr++;
	  j++;
	  n+=2;
	} else {
	  j += (int) src[i][n+2];
	  n += 3;
	}
      }
    }
    return T;
  }

  template <class T>
  void* SparseToIJVReal(T**A, int rows, int cols) {
    int nnz = CountNonzerosReal<T>(A,rows,cols);
    T* ret;
    ret = (T*) Malloc(sizeof(T)*nnz*3);
    int i, j, n;
    int ptr = 0;
    for (i=0;i<cols;i++) {
      n = 1;
      j = 0;
      while (j<rows) {
	if (src[i][n] != 0) {
	  T[ptr] = j+1;
	  T[ptr+nnz] = i+1;
	  T[ptr+2*nnz] = src[i][n];
	  ptr++;
	  j++;
	  n++;
	} else {
	  j += (int) src[i][n+1];
	  n += 2;
	}
      }
    }
    return T;
  }

  // Convert a sparse matrix to IJV
  void* SparseToIJV(Class dclass, int rows, int cols, const void* cp) {
    switch (dclass) {
    case FM_INT32:
      return SparseToIJVReal<int32>((int32**)cp,rows,cols);
    case FM_FLOAT:
      return SparseToIJVReal<float>((float**)cp,rows,cols);
    case FM_DOUBLE:
      return SparseToIJVReal<double>((double**)cp,rows,cols);
    case FM_COMPLEX:
      return SparseToIJVComplex<float>((float**)cp,rows,cols);
    case FM_DCOMPLEX:
      return SparseToIJVComplex<double>((double**)cp,rows,cols);
    }
  }

  void* CopySparseMatrix(Class dclass, int rows, int cols, const void* cp) { 
    switch (dclass) {
    case FM_INT32:
      return CopySparseMatrix<int32>((int32**)cp,rows,cols);
    case FM_FLOAT:
      return CopySparseMatrix<float>((float**)cp,rows,cols);
    case FM_DOUBLE:
      return CopySparseMatrix<double>((double**)cp,rows,cols);
    case FM_COMPLEX:
      return CopySparseMatrix<float>((float**)cp,rows,cols);
    case FM_DCOMPLEX:
      return CopySparseMatrix<double>((double**)cp,rows,cols);
    }
  }

  int CountNonzeros(Class dclass, int rows, int cols, const void *cp) {
    switch (dclass) {
    case FM_INT32:
      return CountNonzerosReal<int32>((int32**)cp,rows,cols);
    case FM_FLOAT:
      return CountNonzerosReal<float>((float**)cp,rows,cols);
    case FM_DOUBLE:
      return CountNonzerosReal<double>((double**)cp,rows,cols);
    case FM_COMPLEX:
      return CountNonzerosComplex<float>((float**)cp,rows,cols);
    case FM_DCOMPLEX:
      return CountNonzerosComplex<double>((double**)cp,rows,cols);
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
  

  // This implementation is poor because for a linear access of the elements
  // of A, it requires significant time.
  template <class T>
  void* GetSparseVectorSubsetsComplex(int rows, int cols, const T** A,
				      const indexType* indx, int irows, int icols) {
    int i, j, n;
    int nrow, ncol;
    T* Acol = new T[2*irows];
    T** dp = new T*[icols];
    for (i=0;i<icols;i++) {
      for (j=0;j<irows;j++) {
	// Get the vector index
	n = indx[i*irows+j] - 1;
	// Decompose this into a row and column
	ncol = n / rows;
	if (ncol > cols)
	  throw Exception("Index exceeds variable dimensions");
	nrow = n % rows;
	// Extract this value from the source matrix
	ComplexStringExtract<T>(A[ncol],nrow,Acol+2*j);
      }
      dp[i] = CompressComplexVector(Acol,irows);
    }
    delete Acol;
    return dp;
  }

  // This implementation is poor because for a linear access of the elements
  // of A, it requires significant time.
  template <class T>
  void* GetSparseVectorSubsetsReal(int rows, int cols, const T** A,
				   const indexType* indx, int irows, int icols) {
    int i, j, n;
    int nrow, ncol;
    T* Acol = new T[irows];
    T** dp = new T*[icols];
    for (i=0;i<icols;i++) {
      for (j=0;j<irows;j++) {
	// Get the vector index
	n = indx[i*irows+j] - 1;
	// Decompose this into a row and column
	ncol = n / rows;
	if (ncol > cols)
	  throw Exception("Index exceeds variable dimensions");
	nrow = n % rows;
	// Extract this value from the source matrix
	RealStringExtract<T>(A[ncol],nrow,Acol+j);
      }
      dp[i] = CompressRealVector(Acol,irows);
    }
    delete Acol;
    return dp;
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

}
