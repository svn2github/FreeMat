#include <vector>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

long timer() {
  struct timeval tp;
  struct timezone tzp;
  gettimeofday(&tp,&tzp);
  return tp.tv_sec;
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

// Multiply a sparse matrix by a sparse matrix (result is sparse)
// Here we use the following order for the loops:
// A normal muliply is
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
    m=0;
    while (k<A_cols) {
      if (B[j][m] != 0) {
	T Bval = B[j][m];
	T* Acol = A[k];
	i=0;
	n=0;
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

template <class T>
void DenseDenseRealMultiply(T* A, int A_rows, int A_cols,
			    T* B, int B_cols, T* C) {
  int i, j, k;
  for (i=0;i<A_rows;i++)
    for (j=0;j<B_cols;j++) {
      T accum = 0;
      for (k=0;k<A_cols;k++)
	accum += A[i+k*A_rows]*B[k+j*A_cols];
      C[i+j*A_rows] = accum;
    }
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
	n=0;
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
    k = 0;
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

template <class T>
void DenseDenseRealAdd(T* A, T* B, int rows, int cols, T* C) {
  int i;
  for (i=0;i<rows*cols;i++)
    C[i] = A[i] + B[i];
}

template <class T>
void DenseSparseRealAdd(T* A, T** B, int rows, int cols, T* C) {
  int i, j;
  T* Bcol = new T[rows];
  for (i=0;i<cols;i++) {
    DecompressRealString<T>(B[i],Bcol,rows);
    for (j=0;j<rows;j++)
      C[i*rows+j] = A[i*rows+j] + Bcol[j];
  }
  delete Bcol;
}

template <class T>
void SparseSparseRealAdd(T** A, T** B, int rows, int cols, T** C) {
  int i, j;
  T* Bcol = new T[rows];
  T* Acol = new T[rows];
  T* Ccol = new T[rows];
  for (i=0;i<cols;i++) {
    DecompressRealString<T>(B[i],Bcol,rows);
    DecompressRealString<T>(A[i],Acol,rows);
    for (j=0;j<rows;j++)
      Ccol[j] = Acol[j] + Bcol[j]; 
   C[i] = CompressRealVector<T>(Ccol, rows);
  }
  delete Bcol;
  delete Acol;
  delete Ccol;
}


template <class T>
T** ConvertDenseToSparseReal(const T* src, int rows, int cols) {
  T** dp;
  dp = new T*[cols];
  int i;
  for (i=0;i<cols;i++)
    dp[i] = CompressRealVector<T>(src+i*rows, rows);
  return dp;
}

template <class T>
void ConvertSparseToDenseReal(T** src, int rows, int cols, T* dst) {
  int i;
  for (i=0;i<cols;i++)
    DecompressRealString<T>(src[i],dst+i*rows,rows);
}

template <class T>
void DeleteSparse(T** src, int rows, int cols) {
  int i;
  for (i=0;i<cols;i++)
    delete[] src[i];
  delete[] src;
}

template <class T>
void PrintSparse(T** src, int rows, int cols) {
  T* full = new T[rows*cols];
  int i, j;

  ConvertSparseToDenseReal(src, rows, cols, full);
  for (i=0;i<rows;i++) {
    for (j=0;j<cols;j++) {
      std::cout << full[i+j*rows] << "\t";
    }
    std::cout << "\n";
  }
  delete full;
  std::cout << "--------------------\n";
}

template <class T>
void PrintDense(T* full, int rows, int cols) {
  int i, j;
  for (i=0;i<rows;i++) {
    for (j=0;j<cols;j++) {
      std::cout << full[i+j*rows] << "\t";
    }
    std::cout << "\n";
  }
  std::cout << "--------------------\n";
}

float *RandSparse(float frac, int rows, int cols) {
  float *N = new float[rows*cols];
  int i;
  for (i=0;i<rows*cols;i++) {
    if (drand48() < frac)
      N[i] = drand48()*10 - 5;
  }
  return N;
}

clock_t tsve;

void tic() {
  tsve = clock();
}

void toc(char *str) {
  clock_t tdif;
  tdif = clock() - tsve;
  printf("Elapsed time %f sec for run %s\n",((float)tdif)/CLOCKS_PER_SEC,str);
}

#define TSIZE 5000
int main(int argc, char *argv[]) {
  // N = [0, 3, 4, 0, 2]
  //     [1, 0, 3, 0, 0]
  //     [2, 7, 2, 0, 0]
  float N[15] = {0,1,2,3,0,7,4,3,2,0,0,0,2,0,0};
  float **Nsparse = ConvertDenseToSparseReal<float>(N,3,5);
  // M = [5, 0, 2]
  //     [1, 0, 0]
  //     [0, 0, 1]
  //     [0, 0, 0]
  float M[12] = {5,1,0,0,0,0,0,0,2,0,1,0};
  float **Msparse = ConvertDenseToSparseReal<float>(M,4,3);

  PrintDense<float>(N,3,5);
  PrintDense<float>(M,4,3);
  PrintSparse<float>(Nsparse,3,5);
  PrintSparse<float>(Msparse,4,3);
  
  float CDense[20];
  memset(CDense,0,20*sizeof(float));
  
  DenseDenseRealMultiply<float>(M,4,3,N,5,CDense);
  PrintDense<float>(CDense,4,5);
  DenseSparseRealMultiply<float>(M,4,3,Nsparse,5,CDense);
  PrintDense<float>(CDense,4,5);
  SparseDenseRealMultiply<float>(Msparse,4,3,N,5,CDense);
  PrintDense<float>(CDense,4,5);

  float *CSparse[5];
  SparseSparseRealMultiply<float>(Msparse,4,3,Nsparse,5,CSparse);
  PrintSparse<float>(CSparse,4,5);

  // N = [0, 3, 4, 0, 2]
  //     [1, 0, 3, 0, 0]
  //     [2, 7, 2, 0, 0]
  // P = [0, 0, -4, 0, 0]
  //     [-1, 2, 0, 0, 0]
  //     [-2, 0, 0, 1, 1]
  float P[15] = {0,-1,-2,0,2,0,-4,0,0,0,0,1,0,0,1};
  float **Psparse = ConvertDenseToSparseReal<float>(P,3,5);

  float S[15];
  DenseDenseRealAdd<float>(N,P,3,5,S);
  PrintDense<float>(S,3,5);
  DenseSparseRealAdd<float>(N,Psparse,3,5,S);
  PrintDense<float>(S,3,5);
  DenseSparseRealAdd<float>(P,Nsparse,3,5,S);
  PrintDense<float>(S,3,5);
  float *Ssparse[5];
  SparseSparseRealAdd<float>(Nsparse,Psparse,3,5,Ssparse);
  PrintSparse<float>(Ssparse,3,5);

  float *N2 = RandSparse(0.0015,4000,1000);
  float **N2sparse = ConvertDenseToSparseReal<float>(N2,4000,1000);
  float *M2 = RandSparse(0.0015,1200,4000);
  float **M2sparse = ConvertDenseToSparseReal<float>(M2,1200,4000);

  float *C2Dense = new float[1200*1000];

  tic();
  //  DenseDenseRealMultiply<float>(M2,1200,4000,N2,1000,C2Dense);
  // Takes way too long...
  toc("dd");

  tic();
  DenseSparseRealMultiply<float>(M2,1200,4000,N2sparse,1000,C2Dense);
  toc("ds");

  tic();
  SparseDenseRealMultiply<float>(M2sparse,1200,4000,N2,1000,C2Dense);
  toc("sd");

  float **C2Sparse = new float*[1000];
  tic();
  SparseSparseRealMultiply<float>(M2sparse,1200,4000,N2sparse,1000,C2Sparse);
  toc("ss");

  float *X1 = RandSparse(0.0015,4000,4000);
  float **X1sparse = ConvertDenseToSparseReal<float>(X1,4000,4000);
  float *X2 = RandSparse(0.0015,4000,4000);
  float **X2sparse = ConvertDenseToSparseReal<float>(X2,4000,4000);
  float *X3 = new float[4000*4000];
  float *X3sparse[4000];
  
  tic();
  DenseDenseRealAdd<float>(X1,X2,4000,4000,X3);
  toc("dd");

  tic();
  DenseSparseRealAdd<float>(X1,X2sparse,4000,4000,X3);
  toc("ds");

  tic();
  SparseSparseRealAdd<float>(X1sparse,X2sparse,4000,4000,X3sparse);
  toc("ss");

    
}


