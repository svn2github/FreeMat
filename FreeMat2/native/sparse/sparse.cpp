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

// Multiply a dense matrix by a sparse matrix (result is dense)
template <class T>
void DenseSparseRealMultiply(T* A, int A_rows, int A_cols,
			     T** B, int B_cols, T* C) {
  int i, j, k, n;
  int B_rows;
  T* str;
  T accum;
  B_rows = A_cols;
  for (i=0;i<A_rows;i++) {
    for (j=0;j<B_cols;j++) {
      str = B[j];
      k = 0;
      n = 0;
      accum = 0;
      while (n<B_rows) {
	if (str[k] != 0) {
	  accum += A[i+n*A_rows]*str[k];
	  n++;
	  k++;
	} else {
	  n += (int) (str[k+1]);
	  k += 2;
	}
      }
      C[i+j*A_rows] = accum;
    }
  }
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
      while ((src[i] == 0) && (i<count)) i++;
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
      while ((src[i] == 0) && (i<count)) {
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

#define TSIZE 5000
int main(int argc, char *argv[]) {
  float P[15] = {0,0,1,2,0,0,4,0,0,0,0,5,2,0,0};
  float *C;
  int Clen;
  C = CompressRealVector<float>(P,15);
  int j;
  for (j=0;j<12;j++) {
    printf("C[%d] = %f\n",j,C[j]);
  }
  float Q[15];
  DecompressRealString<float>(C,Q,15);
  for (j=0;j<15;j++) {
    printf("Q[%d] = %f\n",j,Q[j]);
  }
  // N = [0, 3, 4, 0, 2]
  //     [1, 0, 3, 0, 0]
  //     [2, 7, 2, 1, 0]
  float N[15] = {0,1,2,3,0,7,4,3,2,0,0,1,2,0,0};

  float **Nsparse = ConvertDenseToSparseReal<float>(N,3,5);
  float NN[15];
  ConvertSparseToDenseReal<float>(Nsparse,3,5,NN);
  for (j=0;j<15;j++) {
    printf("NN[%d] = %f\n",j,NN[j]);
  }

  // Z = [1 5 3]
  //     [3 2 3]
  float Z[6] = {1,3,5,2,3,3};

  
  float ZP[10];
  DenseDenseRealMultiply<float>(Z,2,3,N,5,ZP);
  for (int k=0;k<10;k++)
    printf("ZP[%d] = %f\n",k,ZP[k]);
  
  printf("Sparse version...\n");
  DenseSparseRealMultiply<float>(Z,2,3,Nsparse,5,ZP);
  for (int k=0;k<10;k++)
    printf("ZP[%d] = %f\n",k,ZP[k]);
  


  float *M;
  std::cout << "Allocating array\n";
  M = new float[TSIZE*TSIZE];
  int i;
  std::cout << "Filling array...\n";
  int t1, t2;
  t1 = timer();
  for (i=0;i<TSIZE;i++)
    for (j=0;j<TSIZE;j++) {
      double useelt = ((double) random())/RAND_MAX;
      if (useelt < 0.01)
	M[i+TSIZE*j] = useelt;
    }
  t2 = timer() - t1;
  std::cout << "time for fill = " << t2 << " sec\n";
  std::cout << "Converting array...\n";
  float** L;
  t1 = timer();
  L = ConvertDenseToSparseReal(M,TSIZE,TSIZE);
  t2 = timer() - t1;
  std::cout << "time for convert2 = " << t2 << " sec\n";
  DeleteSparse(L,TSIZE,TSIZE);
  std::cout << "Done.\n";
  return 0;

}


