#include "Sparse.hpp"
#include "Malloc.hpp"
namespace FreeMat {

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
    int n=0;
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
    T* dp = new T[outlen];
    j = 0;
    i = 0;
    outlen = 0;
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
    int n=0;
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
    T* dp = new T[outlen];
    j = 0;
    i = 0;
    outlen = 0;
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
      for (j=0;j<rows;j++)
	dst_col[2*j] = (S) src_col[j];
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

  void* CopySparseMatrix(Class dclass, int rows, int cols, const void* cp) { 
    switch (dclass) {
    case FM_INT32:
      return CopySparseMatrixReal<int32>((int32**)cp,rows,cols);
    case FM_FLOAT:
      return CopySparseMatrixReal<float>((float**)cp,rows,cols);
    case FM_DOUBLE:
      return CopySparseMatrixReal<double>((double**)cp,rows,cols);
    case FM_COMPLEX:
      return CopySparseMatrixComplex<float>((float**)cp,rows,cols);
    case FM_DCOMPLEX:
      return CopySparseMatrixComplex<double>((double**)cp,rows,cols);
    }
  }

  /* Type convert the given sparse array and delete it */
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
}
