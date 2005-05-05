#include "Sparse.hpp"
#include "Malloc.hpp"
#include <algorithm>
#include "IEEEFP.hpp"
extern "C" {
#include "umfpack.h"
}
#include "LAPACK.hpp"
#include "Math.hpp"
#include <math.h>

extern "C" {
  int znaupd_(int *ido, char *bmat, int *n, char*
	      which, int *nev, double *tol, double *resid, int *ncv,
	      double *v, int *ldv, int *iparam, int *ipntr, 
	      double *workd, double *workl, int *lworkl, double *rwork, 
	      int *info);
  int zneupd_(int *rvec, char *howmny, int *select, 
	      double *d, double *z, int *ldz, 
	      double *sigma, double *workev, char *bmat, 
	      int *n, char *which, int *nev, double *tol, 
	      double *resid, int *ncv, double *v, int *ldv, int 
	      *iparam, int *ipntr, double *workd, double *workl, 
	      int *lworkl, double *rwork, int *info);
  int dnaupd_(int *ido, char *bmat, int *n, char*
	      which, int *nev, double *tol, double *resid, int *ncv,
	      double *v, int *ldv, int *iparam, int *ipntr, 
	      double *workd, double *workl, int *lworkl, int *info);
  int dneupd_(int *rvec, char *howmny, int *select, 
	double *dr, double *di, double *z__, int *ldz, 
	double *sigmar, double *sigmai, double *workev, char *
	bmat, int *n, char *which, int *nev, double *tol, 
	double *resid, int *ncv, double *v, int *ldv, int 
	*iparam, int *ipntr, double *workd, double *workl, 
	int *lworkl, int *info);
  int dsaupd_(int *ido, char *bmat, int *n, char*
	      which, int *nev, double *tol, double *resid, int *ncv,
	      double *v, int *ldv, int *iparam, int *ipntr, 
	      double *workd, double *workl, int *lworkl, int *info);
  int dseupd_(int *rvec, char *howmny, int *select, 
	double *d, double *z__, int *ldz, 
	double *sigma, char *bmat, int *n, char *which, int *nev, double *tol, 
	double *resid, int *ncv, double *v, int *ldv, int 
	*iparam, int *ipntr, double *workd, double *workl, 
	int *lworkl, int *info);
}

template <class T>
T* RLEDuplicate(const T*src) {
  int blen = (int) (src[0]+1);
  T* ret = new T[blen];
  memcpy(ret,src,sizeof(T)*blen);
  return ret;
}

template <class T>
class RLEEncoder {
  T* buffer;
  int m;
  int n;
  int len;
  int zlen;
  int state;
public:
  RLEEncoder(T* buf, int alen) {
    m = 0;
    n = 0;
    buffer = buf;
    len = alen;
    state = 0;
    zlen = 0;
  }
  int row() {return m;}
  void set(int p) {
    if (p <= m) return;
    p -= m;
    if (state == 0) {
      zlen = p;
      state = 1;
    } else
      zlen += p;
    m += p;  
  }
  void push(T val) {
    if (state == 0) {
      if (val != 0) {
	buffer[n] = val;
	n++;
      } else {
	state = 1;
	zlen = 1;
      }
    } else {
      if (val == 0) {
	zlen++;
      } else {
	if (zlen) {
	  buffer[n++] = 0;
	  buffer[n++] = zlen;
	}
	buffer[n++] = val;
	state = 0;
	zlen = 0;
      }
    }
    m++;
  }
  void end() {
    set(len);
    if (zlen) {
      buffer[n++] = 0;
      buffer[n++] = zlen;
    }
    state = 0;
  }
  T* copyout() {
    T* ret;
    ret = new T[n+1];
    ret[0] = n;
    memcpy(ret+1,buffer,n*sizeof(T));
    return ret;
  }
};


template <class T>
class RLEDecoder {
  const T* data;
  int m;
  int n;
  int len;
public:
  RLEDecoder(const T* str, int alen) {
    data = str;
    m = 0;
    n = 1;
    len = alen;
  }
  int row() {
    return m;
  }
  void update() {
    while ((m < len) && (data[n] == 0)) {
      m += (int) data[n+1];
      n += 2;
      if ((m < len) && (n>data[0])) {
	throw FreeMat::Exception("Invalid data string!\n");
      }
    }
  }
  void advance() {
    if (m < len) {
      m++;
      n++;
      update();
    }
  }
  T value() {
    if (m >= len)
      throw FreeMat::Exception("RLE Decoder overflow - corrupted sparse matrix string encountered");
    return data[n];
  }
  bool more() {
    return (m < len);
  }
  int nnzs() {
    int nnz = 0;
    int t = 0;
    int p = 1;
    while (t < len) {
      if (data[p] != 0) {
	nnz++;
	p++;
	t++;
      } else {
	t += (int) data[p+1];
	p += 2;
      }
    }
    return nnz;
  }
};


template <class T>
class RLEEncoderComplex {
  T* buffer;
  int m;
  int n;
  int len;
  int zlen;
  int state;
public:
  RLEEncoderComplex(T* buf, int alen) {
    m = 0;
    n = 0;
    buffer = buf;
    len = alen;
    state = 0;
    zlen = 0;
  }
  int row() {
    return m;
  }
  void set(int p) {
    if (p <= m) return;
    p -= m;
    if (state == 0) {
      zlen = p;
      state = 1;
    } else
      zlen += p;
    m += p;  
  }
  void push(T valr, T vali) {
    if (state == 0) {
      if ((valr != 0) || (vali != 0)) {
	buffer[n++] = valr;
	buffer[n++] = vali;
      } else {
	state = 1;
	zlen = 1;
      }
    } else {
      if ((valr == 0) && (vali == 0)) {
	zlen++;
      } else {
	if (zlen) {
	  buffer[n++] = 0;
	  buffer[n++] = 0;
	  buffer[n++] = zlen;
	}
	buffer[n++] = valr;
	buffer[n++] = vali;
	state = 0;
	zlen = 0;
      }
    }
    m++;
  }
  void end() {
    set(len);
    if (zlen>0) {
      buffer[n++] = 0;
      buffer[n++] = 0;
      buffer[n++] = zlen;
    }
    state = 0;
  }
  T* copyout() {
    T* ret;
    ret = new T[n+1];
    ret[0] = n;
    memcpy(ret+1,buffer,n*sizeof(T));
    return ret;
  }
};

template <class T>
class RLEDecoderComplex {
  const T* data;
  int m;
  int n;
  int len;
public:
  RLEDecoderComplex(const T* str, int alen) {
    data = str;
    m = 0;
    n = 1;
    len = alen;
  }
  int row() {
    return m;
  }
  void update() {
    while ((m < len) && (data[n] == 0) && (data[n+1] == 0)) {
      m += (int) data[n+2];
      n += 3;
    }
  }
  void advance() {
    if (m < len) {
      m++;
      n+=2;
      update();
    }
  }
  T value_real() {
    if (m >= len)
      throw FreeMat::Exception("RLE DecoderComplex overflow - corrupted sparse matrix string encountered");
    return data[n];
  }
  T value_imag() {
    if (m >= len)
      throw FreeMat::Exception("RLE DecoderComplex overflow - corrupted sparse matrix string encountered");
    return data[n+1];
  }
  bool more() {
    return (m < len);
  }
  int nnzs() {
    int nnz = 0;
    int t = 0;
    int p = 1;
    while (t < len) {
      if ((data[p] != 0) || (data[p+1] != 0)) {
	nnz++;
	p+=2;
	t++;
      } else {
	t += (int) data[p+2];
	p += 3;
      }
    }
    return nnz;
  }
};


// The following ops are O(N^2) instead of O(nnz^2):
//
//  GetSparseNDimSubsets - If the rowindex is sorted into an IJV type list, it can be done without the Decompress step.  Although it is really not too bad, since the vector being recompressed is of the subset size.
//  SetSparseNDimSubsets - same story, only this time the vector being recompressed is of size (N) instead of size (M).  So the cost is higher.  The right way to do this is with a list merge.
//  DeleteSparseMatrixRowsReal


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
    RLEDecoderComplex<T> A(src,count);
    A.update();
    while (A.more()) {
      dst[2*A.row()] = A.value_real();
      dst[2*A.row()+1] = A.value_imag();
      A.advance();
    }
  }

  template <class T>
  T* CompressComplexVector(T* buffer, const T* src, int count) {
    RLEEncoderComplex<T> A(buffer,count);
    for (int i=0;i<count;i++)
      A.push(src[2*i],src[2*i+1]);
    A.end();
    return A.copyout();
  }

  template <class T>
  void DecompressRealString(const T* src, T* dst, int count) {
    RLEDecoder<T> A(src,count);
    A.update();
    while (A.more()) {
      dst[A.row()] = A.value();
      A.advance();
    }
  }

  template <class T>
  T* CompressRealVector(T* buffer, const T* src, int count) {
    RLEEncoder<T> A(buffer,count);
    for (int i=0;i<count;i++)
      A.push(src[i]);
    A.end();
    return A.copyout();
  }

  template <class T>
  T** ConvertDenseToSparseComplex(const T* src, int rows, int cols) {
    T** dp = new T*[cols];
    T* buffer = new T[rows*4];
    for (int i=0;i<cols;i++)
      dp[i] = CompressComplexVector<T>(buffer,src+i*rows*2, rows);
    delete buffer;
    return dp;
  }
  
  template <class T>
  T** ConvertDenseToSparseReal(const T* src, int rows, int cols) {
    T** dp = new T*[cols];
    T* buffer = new T[rows*2];
    for (int i=0;i<cols;i++) 
      dp[i] = CompressRealVector<T>(buffer,src+i*rows, rows);
    delete buffer;
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

  logical* ConvertSparseToDenseLogical(const uint32** src, int rows, int cols) {
    logical* dst;
    uint32* tbuf;
    dst = (logical*) Malloc(sizeof(logical)*rows*cols);
    tbuf = (uint32*) Malloc(sizeof(uint32)*rows);
    for (int i=0;i<cols;i++) {
      memset(tbuf,0,sizeof(uint32)*rows);
      DecompressRealString<uint32>(src[i],tbuf,rows);
      for (int j=0;j<rows;j++)
	dst[j+i*rows] = tbuf[j];
    }
    Free(tbuf);
    return dst;
  }
  

  void* makeDenseArray(Class dclass, int rows, int cols, const void *cp) {
    switch(dclass) {
    case FM_LOGICAL:
      return ConvertSparseToDenseLogical((const uint32 **)cp,rows,cols);
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
    case FM_LOGICAL: 
      return ConvertDenseToSparseReal<logical>((const logical *)cp,rows,cols);
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
  bool operator==(const IJVEntry<T>& a, const IJVEntry<T>& b) {
    return ((a.I == b.I) && (a.J == b.J) && (a.Vreal == b.Vreal) && (a.Vimag == b.Vimag));
  }

  template <class T>
  T* CompressRealIJV(T* buffer, IJVEntry<T> *mlist, int len, int &ptr, int col, int row) {
    // Special case an all zeros column
    if ((ptr >= len) || (mlist[ptr].J > col)) {
      T* buf = new T[3];
      buf[0] = 2;
      buf[1] = 0;
      buf[2] = row;
      return buf;
    }
    RLEEncoder<T> A(buffer, row);
    while (ptr < len && mlist[ptr].J == col) {
      int n;
      n = mlist[ptr].I;
      T accum = 0;
      while ((mlist[ptr].I == n) && (mlist[ptr].J == col)) {
	accum += mlist[ptr].Vreal;
	ptr++;
      }
      A.set(n);
      A.push(accum);
    }
    A.end();
    return A.copyout();
  }

  template <class T>
  T* CompressComplexIJV(T* buffer, IJVEntry<T> *mlist, int len, int &ptr, int col, int row) {
    if ((ptr >= len) || (mlist[ptr].J > col)) {
      T* buf = new T[4];
      buf[0] = 3;
      buf[1] = 0;
      buf[2] = 0;
      buf[3] = row;
      return buf;
    }
    RLEEncoderComplex<T> A(buffer, row);
    while (ptr < len && mlist[ptr].J == col) {
      int n;
      n = mlist[ptr].I;
      T accum_real = 0;
      T accum_imag = 0;
      while ((mlist[ptr].I == n) && (mlist[ptr].J == col)) {
	accum_real += mlist[ptr].Vreal;
	accum_imag += mlist[ptr].Vimag;
	ptr++;
      }
      A.set(n);
      A.push(accum_real,accum_imag);
    }
    A.end();
    return A.copyout();
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
    T* buffer = new T[rows*2];
    for (int col=0;col<cols;col++)
      op[col] = CompressRealIJV<T>(buffer,mlist,nnz,ptr,col,rows);
    delete[] buffer;
    // Free the space
    delete[] mlist;
    // Return the array
    return op;
  }

  template <class T>
  void *SparseZerosReal(int rows, int cols) {
    T **src;
    src = new T*[cols];
    int i;
    for (i=0;i<cols;i++) {
      src[i] = new T[3];
      src[i][0] = 2;
      src[i][1] = 0;
      src[i][2] = rows;
    }
    return src;
  }

  void* SparseFloatZeros(int rows, int cols) {
    return SparseZerosReal<float>(rows,cols);
  }

  template <class T>
  void *SparseZerosComplex(int rows, int cols) {
    T **src;
    src = new T*[cols];
    int i;
    for (i=0;i<cols;i++) {
      src[i] = new T[4];
      src[i][0] = 3;
      src[i][1] = 0;
      src[i][2] = 0;
      src[i][3] = rows;
    }
    return src;
  }

  template <class T>
  void* MakeSparseFromDiagReal(T* cp, int len) {
    T** B;
    B = new T*[len];
    // Each column is in 5,0,N,k,0,M
    for (int i=0;i<len;i++) {
      if (cp[i] == 0) {
	B[i] = new T[3];
	B[i][0] = 2;
	B[i][1] = 0;
	B[i][2] = len;
      } else {
	B[i] = new T[6];
	B[i][0] = 5;
	B[i][1] = 0;
	B[i][2] = i;
	B[i][3] = cp[i];
	B[i][4] = 0;
	B[i][5] = len-i-1;
      }
    }
    return B;
  }

  template <class T>
  void* MakeSparseFromDiagComplex(T* cp, int len) {
    T** B;
    B = new T*[len];
    // Each column is in 8,0,0,N,kr,ki,0,0,M
    for (int i=0;i<len;i++) {
      if ((cp[2*i] == 0) && (cp[2*i+1] == 0)) {
	B[i] = new T[4];
	B[i][0] = 3;
	B[i][1] = 0;
	B[i][2] = 0;
	B[i][3] = len;
      } else {
	B[i] = new T[9];
	B[i][0] = 8;
	B[i][1] = 0;
	B[i][2] = 0;
	B[i][3] = i;
	B[i][4] = cp[2*i];
	B[i][5] = cp[2*i+1];
	B[i][6] = 0;
	B[i][7] = 0;
	B[i][8] = len-i-1;
      }
    }
    return B;
  }

  template <class T>
  void* MakeSparseScaledIdentityReal(T cp, int len) {
    T** B;
    if (cp == 0)
      return SparseZerosReal<T>(len, len);
    B = new T*[len];
    // Each column is in 5,0,N,k,0,M
    for (int i=0;i<len;i++) {
      B[i] = new T[6];
      B[i][0] = 5;
      B[i][1] = 0;
      B[i][2] = i;
      B[i][3] = cp;
      B[i][4] = 0;
      B[i][5] = len-i-1;
    }
    return B;
  }

  template <class T>
  void* MakeSparseScaledIdentityComplex(T cp_r, T cp_i, int len) {
    T** B;
    B = new T*[len];
    if ((cp_r == 0) && (cp_i == 0))
      return SparseZerosComplex<T>(len, len);
    // Each column is in 8,0,0,N,kr,ki,0,0,M
    for (int i=0;i<len;i++) {
      B[i] = new T[9];
      B[i][0] = 8;
      B[i][1] = 0;
      B[i][2] = 0;
      B[i][3] = i;
      B[i][4] = cp_r;
      B[i][5] = cp_i;
      B[i][6] = 0;
      B[i][7] = 0;
      B[i][8] = len-i-1;
    }
    return B;
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
    T* buffer = new T[rows*4];
    for (int col=0;col<cols;col++)
      op[col] = CompressComplexIJV<T>(buffer,mlist,nnz,ptr,col,rows);
    delete[] buffer;
    // Free the space
    delete[] mlist;
    // Return the array
    return op;
  }


  void* makeSparseFromIJV(Class dclass, int rows, int cols, int nnz, 
			  uint32* I, int istride, uint32 *J, int jstride,
			  const void* cp, int cpstride) {
    switch(dclass) {
    case FM_LOGICAL:
      return makeSparseFromIJVReal<uint32>(rows,cols,nnz,I,istride,
					    J,jstride,(uint32*)cp,cpstride);
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
    throw Exception("unsupported type for makeSparseFromIJV");
  }
			  
  void DeleteSparseMatrix(Class dclass, int rows, int cols, void *cp) {
    switch(dclass) {
    case FM_LOGICAL:
      DeleteSparse<uint32>((uint32**)cp,rows,cols);
      return;
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
    throw Exception("unsupported type for DeleteSparseMatrix");
  }
  

  template <class T, class S>
  S** TypeConvertSparseRealReal(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    S* buffer = new S[rows*2];
    for (int p=0;p<cols;p++) {
      RLEEncoder<S> B(buffer,rows);
      RLEDecoder<T> A(src[p],rows);
      A.update(); 
      while (A.more()) {
	B.set(A.row());
	B.push((S) A.value());
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  template <class T, class S>
  S** TypeConvertSparseComplexComplex(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    S* buffer = new S[rows*4];
    for (int p=0;p<cols;p++) {
      RLEEncoderComplex<S> B(buffer,rows);
      RLEDecoderComplex<T> A(src[p],rows);
      A.update(); 
      while (A.more()) {
	B.set(A.row());
	B.push((S) A.value_real(),(S) A.value_imag());
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  template <class T, class S>
  S** TypeConvertSparseRealComplex(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    S* buffer = new S[rows*4];
    for (int p=0;p<cols;p++) {
      RLEEncoderComplex<S> B(buffer,rows);
      RLEDecoder<T> A(src[p],rows);
      A.update();
      while (A.more()) {
	B.set(A.row());
	B.push(A.value(),0);
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  template <class T, class S>
  S** TypeConvertSparseComplexReal(T** src, int rows, int cols) {
    S** dp;
    dp = new S*[cols];
    S* buffer = new S[rows*2];
    for (int p=0;p<cols;p++) {
      RLEEncoder<S> B(buffer,rows);
      RLEDecoderComplex<T> A(src[p],rows);
      A.update();
      while (A.more()) {
	B.set(A.row());
	B.push((S) A.value_real());
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  template <class T>
  uint32** TypeConvertSparseRealLogical(T** src, int rows, int cols) {
    uint32** dp;
    dp = new uint32*[cols];
    uint32* buffer = new uint32[rows*2];
    for (int p=0;p<cols;p++) {
      RLEEncoder<uint32> B(buffer,rows);
      RLEDecoder<T> A(src[p],rows);
      A.update(); 
      while (A.more()) {
	B.set(A.row());
	if (A.value() == 0)
	  B.push(0);
	else
	  B.push(1);
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  template <class T>
  uint32** TypeConvertSparseComplexLogical(T** src, int rows, int cols) {
    uint32** dp;
    dp = new uint32*[cols];
    uint32* buffer = new uint32[rows*2];
    for (int p=0;p<cols;p++) {
      RLEEncoder<uint32> B(buffer,rows);
      RLEDecoderComplex<T> A(src[p],rows);
      A.update();
      while (A.more()) {
	B.set(A.row());
	if ((A.value_real() == 0) && (A.value_imag() == 0))
	  B.push(0);
	else
	  B.push(1);
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  template <class T>
  void* CopySparseMatrix(const T** src, int rows, int cols) {
    T** dp;
    dp = new T*[cols];
    int i;
    for (i=0;i<cols;i++) 
      dp[i] = RLEDuplicate(src[i]);
    return dp;
  }

  template <class T>
  int CountNonzerosComplex(const T** src, int rows, int cols) {
    int nnz = 0;
    for (int i=0;i<cols;i++) {
      RLEDecoderComplex<T> A(src[i],rows);
      nnz += A.nnzs();
    }
    return nnz;
  }

  template <class T>
  int CountNonzerosReal(const T** src, int rows, int cols) {
    int nnz = 0;
    for (int i=0;i<cols;i++) {
      RLEDecoder<T> A(src[i],rows);
      nnz += A.nnzs();
    }
    return nnz;
  }

  template <class T>
  T* SparseToIJVComplex(const T**src, int rows, int cols, uint32* &I, uint32* &J, int &nnz) {
    nnz = CountNonzerosComplex<T>(src,rows,cols);
    I = (uint32*) Malloc(sizeof(uint32)*nnz);
    J = (uint32*) Malloc(sizeof(uint32)*nnz);
    T* V = (T*) Malloc(sizeof(T)*nnz*2);
    int ptr = 0;
    for (int i=0;i<cols;i++) {
      RLEDecoderComplex<T> A(src[i],rows);
      A.update();
      while (A.more()) {
	I[ptr] = A.row()+1;
	J[ptr] = i+1;
	V[2*ptr] = A.value_real();
	V[2*ptr+1] = A.value_imag();
	ptr++;
	A.advance();
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
    int ptr = 0;
    for (int i=0;i<cols;i++) {
      RLEDecoder<T> A(src[i],rows);
      A.update();
      while (A.more()) {
	I[ptr] = A.row()+1;
	J[ptr] = i+1;
	V[ptr] = A.value();
	ptr++;
	A.advance();
      }
    }
    return V;
  }

  // Convert a sparse matrix to IJV
  void* SparseToIJV(Class dclass, int rows, int cols, const void* cp,
		    uint32* &I, uint32* &J, int &nnz) {
    switch (dclass) {
    case FM_LOGICAL:
      return SparseToIJVReal<uint32>((const uint32**)cp,rows,cols,I,J,nnz);
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
    throw Exception("unsupported type for SparseToIJV");
  }

  void* CopySparseMatrix(Class dclass, int rows, int cols, const void* cp) { 
    switch (dclass) {
    case FM_LOGICAL:
      return CopySparseMatrix<uint32>((const uint32**)cp,rows,cols);
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
    throw Exception("unsupported type for CopySparseMatrix");
  }

  int CountNonzeros(Class dclass, int rows, int cols, const void *cp) {
    switch (dclass) {
    case FM_LOGICAL:
      return CountNonzerosReal<uint32>((const uint32**)cp,rows,cols);
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
    throw Exception("unsupported type for CountNonzeros");
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
    // This new version of the sparse-sparse matrix multiply works
    // as follows.  The outer loop is over the columns of B
    T* abuff = new T[2*A_rows];
    for (int j=0;j<B_cols;j++) {
      T* cbuff = new T[3];
      cbuff[0] = 2; cbuff[1] = 0; cbuff[2] = A_rows;
      // Put a decoder on this column of B
      RLEDecoder<T> Bcol(B[j],A_cols);
      Bcol.update();
      while (Bcol.more()) {
	T scale = Bcol.value();
	RLEDecoder<T> Acol(A[Bcol.row()],A_rows);
	RLEDecoder<T> Ccol(cbuff,A_rows);
	RLEEncoder<T> Dcol(abuff,A_rows);
	// Add Acol*scale + Ccol --> Dcol
	Acol.update();
	Ccol.update();
	while (Acol.more() || Ccol.more()) {
	  if (Acol.row() == Ccol.row()) {
	    Dcol.set(Acol.row());
	    Dcol.push(Acol.value()*scale+Ccol.value());
	    Acol.advance();
	    Ccol.advance();
	  } else if (Acol.row() < Ccol.row()) {
	    Dcol.set(Acol.row());
	    Dcol.push(Acol.value()*scale);
	    Acol.advance();
	  } else {
	    Dcol.set(Ccol.row());
	    Dcol.push(Ccol.value());
	    Ccol.advance();
	  }
	}
	Dcol.end();
	delete cbuff;
	cbuff = Dcol.copyout();
	Bcol.advance();
      }
      C[j] = cbuff;
    }
    delete abuff;
  }
  
  // Multiply a sparse matrix by a sparse matrix (result is sparse)
  // Here we use the following order for the loops:
  // A normal multiply is
  // for j=1:Ccols
  //   for k=1:Acols
  //     for i=1:Arows
  //       c[i][j] += a[i][k]*b[k][j]
  template <class T>
  void SparseSparseComplexMultiply(T** A, int A_rows, int A_cols,
				   T** B, int B_cols,
				   T** C) {
    T* abuff = new T[4*A_rows];
    for (int j=0;j<B_cols;j++) {
      T* cbuff = new T[4];
      cbuff[0] = 3; cbuff[1] = 0; cbuff[2] = 0; cbuff[3] = A_rows;
      // Put a decoder on this column of B
      RLEDecoderComplex<T> Bcol(B[j],A_cols);
      Bcol.update();
      while (Bcol.more()) {
	T scale_real = Bcol.value_real();
	T scale_imag = Bcol.value_imag();
	RLEDecoderComplex<T> Acol(A[Bcol.row()],A_rows);
	RLEDecoderComplex<T> Ccol(cbuff,A_rows);
	RLEEncoderComplex<T> Dcol(abuff,A_rows);
	// Add Acol*scale + Ccol --> Dcol
	Acol.update();
	Ccol.update();
	while (Acol.more() || Ccol.more()) {
	  if (Acol.row() == Ccol.row()) {
	    Dcol.set(Acol.row());
	    Dcol.push(Acol.value_real()*scale_real - 
		      Acol.value_imag()*scale_imag + Ccol.value_real(),
		      Acol.value_imag()*scale_real + 
		      Acol.value_real()*scale_imag + Ccol.value_imag());
	    Acol.advance();
	    Ccol.advance();
	  } else if (Acol.row() < Ccol.row()) {
	    Dcol.set(Acol.row());
	    Dcol.push(Acol.value_real()*scale_real - 
		      Acol.value_imag()*scale_imag,
		      Acol.value_imag()*scale_real + 
		      Acol.value_real()*scale_imag);
	    Acol.advance();
	  } else {
	    Dcol.set(Ccol.row());
	    Dcol.push(Ccol.value_real(),Ccol.value_imag());
	    Ccol.advance();
	  }
	}
	Dcol.end();
	delete cbuff;
	cbuff = Dcol.copyout();
	Bcol.advance();
      }
      C[j] = cbuff;
    }
    delete abuff;
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
    T Bval;
    memset(C,0,A_rows*B_cols*sizeof(T));
    for (int k=0;k<A_cols;k++) 
      for (int j=0;j<B_cols;j++) {
	Bval = B[k+j*A_cols];
	if (Bval != 0) {
	  RLEDecoder<T> Acol(A[k],A_rows);
	  Acol.update();
	  while (Acol.more()) {
	    C[Acol.row()+j*A_rows] += Acol.value()*Bval;
	    Acol.advance();
	  }
	}
      }
  }
  
  template <class T>
  void SparseDenseComplexMultiply(T**A, int A_rows, int A_cols,
				  T*B, int B_cols,
				  T*C) {
    T Bval_real;
    T Bval_imag;
    memset(C,0,2*A_rows*B_cols*sizeof(T));
    for (int k=0;k<A_cols;k++) 
      for (int j=0;j<B_cols;j++) {
	Bval_real = B[2*(k+j*A_cols)];
	Bval_imag = B[2*(k+j*A_cols)+1];
	if ((Bval_real != 0) || (Bval_imag != 0)) {
	  RLEDecoderComplex<T> Acol(A[k],A_rows);
	  Acol.update();
	  while (Acol.more()) {
	    int i = Acol.row();
	    C[2*(i+j*A_rows)] += Acol.value_real()*Bval_real - 
	      Acol.value_imag()*Bval_imag;
	    C[2*(i+j*A_rows)+1] += Acol.value_real()*Bval_imag + 
	      Acol.value_imag()*Bval_real;
	    Acol.advance();
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
    int B_rows;
    memset(C,0,sizeof(T)*A_rows*B_cols);
    B_rows = A_cols;
    for (int j=0;j<B_cols;j++) {
      RLEDecoder<T> Bcol(B[j],B_rows);
      Bcol.update();
      while (Bcol.more()) {
	T Bval = Bcol.value();
	int n = Bcol.row();
	for (int i=0;i<A_rows;i++)
	  C[i+j*A_rows] += A[i+n*A_rows]*Bval;
	Bcol.advance();
      }
    }
  }

  template <class T>
  void DenseSparseComplexMultiply(T* A, int A_rows, int A_cols,
				  T** B, int B_cols, T* C) {
    int B_rows;
    memset(C,0,2*sizeof(T)*A_rows*B_cols);
    B_rows = A_cols;
    for (int j=0;j<B_cols;j++) {
      RLEDecoderComplex<T> Bcol(B[j],B_rows);
      Bcol.update();
      while (Bcol.more()) {
	T Bval_real = Bcol.value_real();
	T Bval_imag = Bcol.value_imag();
	int n = Bcol.row();
	for (int i=0;i<A_rows;i++) {
	  C[2*(i+j*A_rows)] += A[2*(i+n*A_rows)]*Bval_real - 
	    A[2*(i+n*A_rows)+1]*Bval_imag;
	  C[2*(i+j*A_rows)+1] += A[2*(i+n*A_rows)+1]*Bval_real + 
	    A[2*(i+n*A_rows)]*Bval_imag;
	}
	Bcol.advance();
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
    case FM_COMPLEX: 
      {
	float *C = (float*) Malloc(2*rows*bcols*sizeof(float));
	SparseDenseComplexMultiply<float>((float**)ap,rows,cols,
					  (float*)bp,bcols,C);
	return C;
      }
    case FM_DCOMPLEX: 
      {
	double *C = (double*) Malloc(2*rows*bcols*sizeof(double));
	SparseDenseComplexMultiply<double>((double**)ap,rows,cols,
					   (double*)bp,bcols,C);
	return C;
      }
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
    case FM_COMPLEX: 
      {
	float *C = (float*) Malloc(2*rows*bcols*sizeof(float));
	DenseSparseComplexMultiply<float>((float*)ap,rows,cols,
				       (float**)bp,bcols,C);
	return C;
      }
    case FM_DCOMPLEX: 
      {
	double *C = (double*) Malloc(2*rows*bcols*sizeof(double));
	DenseSparseComplexMultiply<double>((double*)ap,rows,cols,
					(double**)bp,bcols,C);
	return C;
      }
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
    case FM_COMPLEX: 
      {
	float **C = new float*[bcols];
	SparseSparseComplexMultiply<float>((float**)ap,rows,cols,
					   (float**)bp,bcols,C);
	return C;
      }
    case FM_DCOMPLEX: 
      {
	double **C = new double*[bcols];
	SparseSparseComplexMultiply<double>((double**)ap,rows,cols,
					    (double**)bp,bcols,C);
	return C;
      }
    }
  }

  /* Type convert the given sparse array and delete it */
  /* dclass is the source type, oclass is the destination class*/
  void* TypeConvertSparse(Class dclass, int rows, int cols, 
			  const void *cp, Class oclass) {
    if (dclass == FM_LOGICAL) {
      switch(oclass) {
      case FM_INT32:
	return TypeConvertSparseRealReal<uint32,int32>((uint32**)cp,rows,cols);
      case FM_FLOAT:
	return TypeConvertSparseRealReal<uint32,float>((uint32**)cp,rows,cols);
      case FM_DOUBLE:
	return TypeConvertSparseRealReal<uint32,double>((uint32**)cp,rows,cols);
      case FM_COMPLEX:
	return TypeConvertSparseRealComplex<uint32,float>((uint32**)cp,rows,cols);
      case FM_DCOMPLEX:
	return TypeConvertSparseRealComplex<uint32,double>((uint32**)cp,rows,cols);
      }
    } else if (dclass == FM_INT32) {
      switch(oclass) {
      case FM_LOGICAL:
	return TypeConvertSparseRealLogical<int32>((int32**)cp,rows,cols);
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
      case FM_LOGICAL:
	return TypeConvertSparseRealLogical<float>((float**)cp,rows,cols);
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
      case FM_LOGICAL:
	return TypeConvertSparseRealLogical<double>((double**)cp,rows,cols);
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
      case FM_LOGICAL:
	return TypeConvertSparseComplexLogical<float>((float**)cp,rows,cols);
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
      case FM_LOGICAL:
	return TypeConvertSparseComplexLogical<double>((double**)cp,rows,cols);
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
    throw Exception("unsupported type for TypeConvertSparse");
  }

  // There _has_ to be a better way to do this.. =P
  void TrimEmpties(ArrayMatrix &m) {
    for (ArrayMatrix::iterator i=m.begin();i != m.end();i++) {
      // Scan for empties
      bool emptiesFound = true;
      while (emptiesFound) {
	emptiesFound = false;
	for (ArrayVector::iterator j = i->begin(); j != i->end(); j++)
	  if (j->isEmpty()) {
	    i->erase(j);
	    emptiesFound = true;
	    break;
	  }
      }
    }
    bool emptiesFound = true;
    while (emptiesFound) {
      emptiesFound = false;
      for (ArrayMatrix::iterator i=m.begin();i != m.end();i++) {
	if (i->empty()) {
	  m.erase(i);
	  emptiesFound = true;
	  break;
	}
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
    TrimEmpties(m);
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
    case FM_LOGICAL:
      return SparseMatrixConst<uint32>(rows, cols, m);
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
    throw Exception("unsupported type for SparseMatrixConstructor");
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

  template <class T>
  void* ConvertIJVtoRLEReal(IJVEntry<T>* mlist, int nnz, int rows, int cols) {
    T** B;
    B = new T*[cols];
    T* buffer = new T[rows*2];
    int ptr = 0;
    for (int col=0;col<cols;col++)
      B[col] = CompressRealIJV<T>(buffer,mlist,nnz,ptr,col,rows);
    delete buffer;
    return B;
  }

  template <class T>
  void* ConvertIJVtoRLEComplex(IJVEntry<T>* mlist, int nnz, int rows, int cols) {
    T** B;
    B = new T*[cols];
    T* buffer = new T[rows*4];
    int ptr = 0;
    for (int col=0;col<cols;col++)
      B[col] = CompressComplexIJV<T>(buffer,mlist,nnz,ptr,col,rows);
    delete buffer;
    return B;
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
    T** B = (T**) ConvertIJVtoRLEReal<T>(ilist,icount,irows,cols);
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
    // Build a sparse matrix out of this
    T** B = (T**) ConvertIJVtoRLEReal<T>(dlist,optr,rows,cols);
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
    // Build a sparse matrix out of this
    T** B = (T**) ConvertIJVtoRLEComplex<T>(dlist,optr,rows,cols);
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
    T** B = (T**) ConvertIJVtoRLEComplex<T>(ilist,icount,irows,cols);
    return B;
  }

  // GetSparseVectorSubsets - This one is a bit difficult to do efficiently.
  // For each column in the output, we have to extract potentially random
  // elements from the source array.
  void* GetSparseVectorSubsets(Class dclass, int rows, int cols, const void* src,
			       const indexType* indx, int irows, int icols) {
    switch (dclass) {
    case FM_LOGICAL:
      return GetSparseVectorSubsetsReal<uint32>(rows, cols, (const uint32**) src,
					       indx, irows, icols);
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
    throw Exception("unsupported type for GetSparseVctorSubsets");
  }

  // SetSparseVectorSubsets - This one is a bit difficult to do efficiently.
  // For each column in the output, we have to extract potentially random
  // elements from the source array.
  void* SetSparseVectorSubsets(Class dclass, int &rows, int &cols, 
			       const void* src,
			       const indexType* indx, int irows, int icols,
			       const void* data, int advance) {
    switch (dclass) {
    case FM_LOGICAL:
      return SetSparseVectorSubsetsReal<uint32>(rows, cols, (const uint32**) src,
					       indx, irows, icols, 
					       (const uint32*) data, advance);
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
    throw Exception("unsupported type for SetSparseVectorSubsets");
  }


  template <class T>
  void* GetSparseColumnSubsetAssist(int rows, int cols, const T** A, const indexType*cindx, int icols) {
    for (int i=0;i<icols;i++) 
      if ((cindx[i] < 1) || (cindx[i] > cols))
	throw Exception("out of range column index in sparse matrix expression of type A(:,n)");
    T** dest;
    dest = new T*[icols];
    for (int i=0;i<icols;i++) 
      dest[i] = RLEDuplicate<T>(A[cindx[i]-1]);
    return dest;
  }

  // GetSparseNDimSubsets
  void* GetSparseColumnSubset(Class dclass, int rows, int cols, const void* src,
			      const indexType* cindx, int icols) {
    switch(dclass) {
    case FM_LOGICAL:
      return GetSparseColumnSubsetAssist<uint32>(rows, cols, (const uint32**) src, cindx, icols);
    case FM_INT32:
      return GetSparseColumnSubsetAssist<int32>(rows, cols, (const int32**) src, cindx, icols);
    case FM_FLOAT:
      return GetSparseColumnSubsetAssist<float>(rows, cols, (const float**) src, cindx, icols);
    case FM_DOUBLE:
      return GetSparseColumnSubsetAssist<double>(rows, cols, (const double**) src, cindx, icols);
    case FM_COMPLEX:
      return GetSparseColumnSubsetAssist<float>(rows, cols, (const float**) src, cindx, icols);
    case FM_DCOMPLEX:
      return GetSparseColumnSubsetAssist<double>(rows, cols, (const double**) src, cindx, icols);
    }
    throw Exception("unsupported type for GetSparseColumnSubset");
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
    T* buffer = new T[irows*2];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if ((m<0) || (m>=cols))
	throw Exception("column index exceeds variable bounds in expression of type A(n,m)");
      memset(Acol,0,rows*sizeof(T));
      DecompressRealString<T>(A[m],Acol,rows);
      memset(Bcol,0,irows*sizeof(T));
      for (j=0;j<irows;j++) {
	// Get the row index
	n = rindx[j] - 1;
	if ((n<0) || (n>=rows))
	  throw Exception("row index exceeds variable bounds in expression of type A(n,m)");
	Bcol[j] = Acol[n];
      }
      dp[i] = CompressRealVector<T>(buffer,Bcol,irows);
    }
    delete buffer;
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
    T* buffer = new T[irows*4];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if ((m<0) || (m>=cols))
	throw Exception("column index exceeds variable bounds in expression of type A(n,m)");
      memset(Acol,0,2*rows*sizeof(T));
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
      dp[i] = CompressComplexVector<T>(buffer,Bcol,irows);
    }
    delete buffer;
    delete Acol;
    delete Bcol;
    return dp;
  }


  bool CheckAllRowsReference(const indexType* rindx, int rows) {
    for (int i=0;i<rows;i++)
      if (rindx[i] != (i+1)) return false;
    return true;
  }

  // GetSparseNDimSubsets
  void* GetSparseNDimSubsets(Class dclass, int rows, int cols, const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols) {
    if ((irows == rows) && CheckAllRowsReference(rindx,rows))
      return GetSparseColumnSubset(dclass,rows,cols,src,cindx,icols);
    switch(dclass) {
    case FM_LOGICAL:
      return GetSparseNDimSubsetsReal<uint32>(rows, cols, (const uint32**) src,
					     rindx, irows, cindx, icols);
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
    throw Exception("unsupported type for GetSparseNDimSubsets");
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
    RealStringExtract<T>(src[cindx],rindx,ret);
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
    case FM_LOGICAL:
      return GetSparseScalarReal<uint32>(rows, cols, (const uint32**) src,
					rindx, cindx);
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
    throw Exception("unsupported type for GetSparseScalarElement");
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
    T* buffer = new T[rows*2];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if (m<0)
	throw Exception("negative column index encountered in sparse matrix assigment of type A(n,m) = B");
      memset(Acol,0,rows*sizeof(T));
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
      dp[m] = CompressRealVector<T>(buffer,Acol,rows);
    }
    delete Acol;
    delete buffer;
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
    T* buffer = new T[4*rows];
    for (i=0;i<icols;i++) {
      // Get the column index
      m = cindx[i] - 1;
      if (m<0)
	throw Exception("negative column index encountered in sparse matrix assigment of type A(n,m) = B");
      memset(Acol,0,2*rows*sizeof(T));
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
      dp[m] = CompressComplexVector<T>(buffer,Acol,rows);
    }
    delete Acol;
    delete buffer;
    return dp;
  }

  void* SetSparseNDimSubsets(Class dclass, int &rows, int &cols, 
			     const void* src,
			     const indexType* rindx, int irows,
			     const indexType* cindx, int icols,
			     const void *data, int advance) {
    switch(dclass) {
    case FM_LOGICAL:
      return SetSparseNDimSubsetsReal<uint32>(rows, cols, (const uint32**) src,
					     rindx, irows, cindx, icols,
					     (const uint32*) data, advance);
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
    throw Exception("unsupported type for SetSparseNDimSubsets");
  }


  template <class T>
  void* DeleteSparseMatrixRowsComplex(int rows, int cols, const T** src, bool *dmap) {
    // Count the number of undeleted rows
    int newrow;
    int i;
    newrow = 0;
    for (i=0;i<rows;i++) if (!dmap[i]) newrow++;
    // Allocate an output array 
    T** dest;
    dest = new T*[cols];
    // Allocate a buffer array
    T* NBuf = new T[newrow*2];
    T* OBuf = new T[rows*2];
    T* buffer = new T[newrow*4];
    // Allocate the output array
    int ptr = 0;
    for (i=0;i<cols;i++) {
      // Decompress this column
      memset(OBuf,0,2*rows*sizeof(T));
      DecompressComplexString<T>(src[i],OBuf,rows);
      // Copy it
      int ptr = 0;
      for (int j=0;j<rows;j++)
	if (!dmap[j]) {
	  NBuf[ptr++] = OBuf[2*j];
	  NBuf[ptr++] = OBuf[2*j+1];
	}
      // Recompress it
      dest[i] = CompressComplexVector<T>(buffer,NBuf,newrow);
    }
    delete buffer;
    return dest;
  }

  template <class T>
  void* DeleteSparseMatrixRowsReal(int rows, int cols, const T** src, bool *dmap) {
    // Count the number of undeleted columns
    int newrow;
    int i;
    newrow = 0;
    for (i=0;i<rows;i++) if (!dmap[i]) newrow++;
    // Allocate an output array 
    T** dest;
    dest = new T*[cols];
    // Allocate a buffer array
    T* NBuf = new T[newrow];
    T* OBuf = new T[rows];
    T* buffer = new T[newrow*2];
    // Allocate the output array
    int ptr = 0;
    for (i=0;i<cols;i++) {
      // Decompress this column
      memset(OBuf,0,rows*sizeof(T));
      DecompressRealString<T>(src[i],OBuf,rows);
      // Copy it
      int ptr = 0;
      for (int j=0;j<rows;j++)
	if (!dmap[j]) NBuf[ptr++] = OBuf[j];
      // Recompress it
      dest[i] = CompressRealVector<T>(buffer,NBuf,newrow);
    }
    delete buffer;
    delete NBuf;
    delete OBuf;
    return dest;
  }

  template <class T>
  void* DeleteSparseMatrixCols(int rows, int cols, const T** src, bool *dmap) {
    // Count the number of undeleted columns
    int newcol;
    int i;
    newcol = 0;
    for (i=0;i<cols;i++) if (!dmap[i]) newcol++;
    // Allocate an output array 
    T** dest;
    dest = new T*[newcol];
    int ptr = 0;
    for (i=0;i<cols;i++) {
      if (!dmap[i]) {
	dest[ptr] = RLEDuplicate(src[i]);
	ptr++;
      }
    }
    return dest;
  }

  void* DeleteSparseMatrixCols(Class dclass, int rows, int cols, const void* cp,
			       bool *dmap) {
    switch(dclass) {
    case FM_LOGICAL:
      return DeleteSparseMatrixCols<uint32>(rows, cols, (const uint32**) cp,
					     dmap);
    case FM_INT32:
      return DeleteSparseMatrixCols<int32>(rows, cols, (const int32**) cp,
					   dmap);
    case FM_FLOAT:
      return DeleteSparseMatrixCols<float>(rows, cols, (const float**) cp,
					   dmap);
    case FM_DOUBLE:
      return DeleteSparseMatrixCols<double>(rows, cols, (const double**) cp,
					    dmap);
    case FM_COMPLEX:
      return DeleteSparseMatrixCols<float>(rows, cols, (const float**) cp,
					   dmap);
    case FM_DCOMPLEX:
      return DeleteSparseMatrixCols<double>(rows, cols, (const double**) cp,
					    dmap);
    }    
    throw Exception("unsupported type for DeleteSparseMatrixCols");
  }

  void* DeleteSparseMatrixRows(Class dclass, int rows, int cols, const void* cp,
			       bool *dmap) {
    switch(dclass) {
    case FM_LOGICAL:
      return DeleteSparseMatrixRowsReal<uint32>(rows, cols, (const uint32**) cp,
					       dmap);
    case FM_INT32:
      return DeleteSparseMatrixRowsReal<int32>(rows, cols, (const int32**) cp,
					       dmap);
    case FM_FLOAT:
      return DeleteSparseMatrixRowsReal<float>(rows, cols, (const float**) cp,
					       dmap);
    case FM_DOUBLE:
      return DeleteSparseMatrixRowsReal<double>(rows, cols, (const double**) cp,
						dmap);
    case FM_COMPLEX:
      return DeleteSparseMatrixRowsComplex<float>(rows, cols, (const float**) cp,
						  dmap);
    case FM_DCOMPLEX:
      return DeleteSparseMatrixRowsComplex<double>(rows, cols, (const double**) cp,
						   dmap);
    }
    throw Exception("unsupported type for DeleteSparseMatrixRows");
  }

  template <class T>
  void* DeleteSparseMatrixVectorReal(int &rows, int &cols, const T** src,
				     const indexType* ndx, int delete_len) {
    // Get the source matrix as an IJV array
    int nnz;
    IJVEntry<T>* mlist = ConvertSparseToIJVListReal<T>(src,rows,cols,nnz);
    // reshape the IJV list into a vector
    int i;
    for (i=0;i<nnz;i++) {
      mlist[i].I += rows*mlist[i].J;
      mlist[i].J = 0;
    }
    rows = rows*cols;
    cols = 1;
    int* ilist = new int[delete_len];
    // Copy the index info in
    for (i=0;i<delete_len;i++)
      ilist[i] = ndx[i]-1;
    // Sort the list
    std::sort(ilist,ilist+delete_len);
    // eliminate duplicates
    i=0;
    int j=0;
    while (i<delete_len) {
      j = i+1;
      while ((j<delete_len) && 
	     (ilist[j] == ilist[i])) j++;
      if (j>i+1) {
	for (int k=j;k<delete_len;k++)
	  ilist[i+1+k-j] = ilist[k];
	delete_len -= (j-i-1);
      }
      i++;
    }
    // Now, we have to determine how many elements are present in the
    // output 
    int newnnz;
    // to do this, we loop through the IJV list
    int dptr;
    dptr = 0;
    int iptr;
    iptr = 0;
    while (dptr < nnz) {
      // Check the index pointer.  If it is smaller than our 
      // current index, skip forward.
      while ((iptr < delete_len) && 
	     (ilist[iptr] < mlist[dptr].I)) iptr++;
      // Is this element deleted?
      if ((iptr < delete_len) && (ilist[iptr] == mlist[dptr].I)) {
	// yes, so copy and adjust
	for (int j=dptr+1;j<nnz;j++)
	  mlist[j-1] = mlist[j];
	nnz--;
      } else {
	mlist[dptr].I -= iptr;
	dptr++;
      }
    }
    rows -= delete_len;
    T** dst = (T**) ConvertIJVtoRLEReal<T>(mlist,nnz,rows,1);
    delete mlist;
    delete ilist;
    return dst;
  }

  template <class T>
  void* DeleteSparseMatrixVectorComplex(int &rows, int &cols, const T** src,
					const indexType* ndx, int delete_len) {
    // Get the source matrix as an IJV array
    int nnz;
    IJVEntry<T>* mlist = ConvertSparseToIJVListComplex<T>(src,rows,cols,nnz);
    // reshape the IJV list into a vector
    int i;
    for (i=0;i<nnz;i++) {
      mlist[i].I += rows*mlist[i].J;
      mlist[i].J = 0;
    }
    rows = rows*cols;
    cols = 1;
    int* ilist = new int[delete_len];
    // Copy the index info in
    for (i=0;i<delete_len;i++)
      ilist[i] = ndx[i]-1;
    // Sort the list
    std::sort(ilist,ilist+delete_len);
    // eliminate duplicates
    i=0;
    int j=0;
    while (i<delete_len) {
      j = i+1;
      while ((j<delete_len) && 
	     (ilist[j] == ilist[i])) j++;
      if (j>i+1) {
	for (int k=j;k<delete_len;k++)
	  ilist[i+1+k-j] = ilist[k];
	delete_len -= (j-i-1);
      }
      i++;
    }
    // Now, we have to determine how many elements are present in the
    // output 
    int newnnz;
    // to do this, we loop through the IJV list
    int dptr;
    dptr = 0;
    int iptr;
    iptr = 0;
    while (dptr < nnz) {
      // Check the index pointer.  If it is smaller than our 
      // current index, skip forward.
      while ((iptr < delete_len) && 
	     (ilist[iptr] < mlist[dptr].I)) iptr++;
      // Is this element deleted?
      if ((iptr < delete_len) && (ilist[iptr] == mlist[dptr].I)) {
	// yes, so copy and adjust
	for (int j=dptr+1;j<nnz;j++)
	  mlist[j-1] = mlist[j];
	nnz--;
      } else {
	mlist[dptr].I -= iptr;
	dptr++;
      }
    }
    int qptr = 0;
    rows -= delete_len;
    T** dst = (T**) ConvertIJVtoRLEComplex<T>(mlist,nnz,rows,1);
    delete mlist;
    delete ilist;
    return dst;
  }

  void* DeleteSparseMatrixVectorSubset(Class dclass, int &rows, int &cols,
				       const void* cp, const indexType* ndx,
				       int delete_len) {
    switch(dclass) {
    case FM_LOGICAL:
      return DeleteSparseMatrixVectorReal<uint32>(rows, cols, (const uint32**) cp,
						   ndx, delete_len);
    case FM_INT32:
      return DeleteSparseMatrixVectorReal<int32>(rows, cols, (const int32**) cp,
						 ndx, delete_len);
    case FM_FLOAT:
      return DeleteSparseMatrixVectorReal<float>(rows, cols, (const float**) cp,
						 ndx, delete_len);
    case FM_DOUBLE:
      return DeleteSparseMatrixVectorReal<double>(rows, cols, 
						  (const double**) cp,
						  ndx, delete_len);
    case FM_COMPLEX:
      return DeleteSparseMatrixVectorComplex<float>(rows, cols, 
						    (const float**) cp,
						    ndx, delete_len);
    case FM_DCOMPLEX:
      return DeleteSparseMatrixVectorComplex<double>(rows, cols, 
						     (const double**) cp,
						     ndx, delete_len);
    }
    throw Exception("unsupported type for DeleteSparseMatrixVectorSubset");
  }

  template <class T>
  void* GetSparseDiagonalReal(int rows, int cols, const T** src, int diagonalOrder) {
    T** dst;
    dst = new T*[1];
    int outLen;
    if (diagonalOrder < 0) {
      outLen = (rows+diagonalOrder) < cols ? (rows+diagonalOrder) : cols;
      T* buffer = new T[outLen];
      T* abuf = new T[outLen*2];
      for (int j=0;j<outLen;j++) 
	RealStringExtract<T>(src[j],j-diagonalOrder,buffer+j);
      dst[0] = CompressRealVector<T>(abuf,buffer,outLen);
      delete abuf;      
    } else {
      outLen = rows < (cols-diagonalOrder) ? rows : (cols-diagonalOrder);
      T* buffer = new T[outLen];
      T* abuf = new T[outLen*2];
      for (int j=0;j<outLen;j++) 
	RealStringExtract<T>(src[diagonalOrder+j],j,buffer+j);
      dst[0] = CompressRealVector<T>(abuf,buffer,outLen);
      delete abuf;
    }      
    return dst;
  }

  template <class T>
  void* GetSparseDiagonalComplex(int rows, int cols, const T** src, int diagonalOrder) {
    T** dst;
    dst = new T*[1];
    int outLen;
    if (diagonalOrder < 0) {
      outLen = (rows+diagonalOrder) < cols ? (rows+diagonalOrder) : cols;
      T* buffer = new T[2*outLen];
      T* abuf = new T[4*outLen];
      for (int j=0;j<outLen;j++) 
	ComplexStringExtract(src[j],j-diagonalOrder,buffer+2*j);
      dst[0] = CompressComplexVector(abuf,buffer,outLen);
      delete abuf;
    } else {
      outLen = rows < (cols-diagonalOrder) ? rows : (cols-diagonalOrder);
      T* buffer = new T[2*outLen];
      T* abuf = new T[4*outLen];
      for (int j=0;j<outLen;j++) 
	ComplexStringExtract(src[diagonalOrder+j],j,buffer+2*j);
      dst[0] = CompressComplexVector(abuf,buffer,outLen);
      delete abuf;
    }      
    return dst;    
  }

  void* GetSparseDiagonal(Class dclass, int rows, int cols, const void* cp, int diag_order) {
    switch(dclass) {
    case FM_LOGICAL:
      return GetSparseDiagonalReal<uint32>(rows, cols, (const uint32**) cp, diag_order);
    case FM_INT32:
      return GetSparseDiagonalReal<int32>(rows, cols, (const int32**) cp, diag_order);
    case FM_FLOAT:
      return GetSparseDiagonalReal<float>(rows, cols, (const float**) cp, diag_order);
    case FM_DOUBLE:
      return GetSparseDiagonalReal<double>(rows, cols, (const double**) cp, diag_order);
    case FM_COMPLEX:
      return GetSparseDiagonalComplex<float>(rows, cols, (const float**) cp, diag_order);
    case FM_DCOMPLEX:
      return GetSparseDiagonalComplex<double>(rows, cols, (const double**) cp, diag_order);
    }    
    throw Exception("unsupported type for GetSparseDiagonal");
  }

  template <class T>
  bool TestSparseNotFinite(int rows, int cols, const T** src) {
    for (int i=0;i<cols;i++)
      for (int j=0;j<(int)src[i][0];j++) 
	if (!IsFinite(src[i][j])) return true;
    return false;
  }
  
  bool SparseAnyNotFinite(Class dclass, int rows, int cols, const void* cp) {
    switch(dclass) {
    case FM_LOGICAL:
      return false;
    case FM_INT32:
      return false;
    case FM_FLOAT:
      return TestSparseNotFinite<float>(rows, cols, (const float**) cp);
    case FM_DOUBLE:
      return TestSparseNotFinite<double>(rows, cols, (const double**) cp);
    case FM_COMPLEX:
      return TestSparseNotFinite<float>(rows, cols, (const float**) cp);
    case FM_DCOMPLEX:
      return TestSparseNotFinite<double>(rows, cols, (const double**) cp);
    }        
     throw Exception("unsupported type for SparseAnyNotFinite");
 }

  template <class T>
  void* SparseArrayTransposeReal(int rows, int cols, const T** src) {
    int nnz;
    IJVEntry<T>* mlist = ConvertSparseToIJVListReal<T>(src, rows, cols, nnz);
    for (int i=0;i<nnz;i++) {
      int tmp = mlist[i].I;
      mlist[i].I = mlist[i].J;
      mlist[i].J = tmp;
    }
    std::sort(mlist,mlist+nnz);
    T** B = (T**) ConvertIJVtoRLEReal<T>(mlist,nnz,cols,rows);
    delete mlist;
    return B;
  }

  template <class T>
  void* SparseArrayTransposeComplex(int rows, int cols, const T** src) {
    int nnz;
    IJVEntry<T>* mlist = ConvertSparseToIJVListComplex<T>(src, rows, cols, nnz);
    for (int i=0;i<nnz;i++) {
      int tmp = mlist[i].I;
      mlist[i].I = mlist[i].J;
      mlist[i].J = tmp;
    }
    std::sort(mlist,mlist+nnz);
    T** B = (T**) ConvertIJVtoRLEComplex<T>(mlist,nnz,cols,rows);
    delete mlist;
    return B;
  }

  template <class T>
  void* SparseArrayHermitianComplex(int rows, int cols, const T** src) {
    int nnz;
    IJVEntry<T>* mlist = ConvertSparseToIJVListComplex<T>(src, rows, cols, nnz);
    for (int i=0;i<nnz;i++) {
      int tmp = mlist[i].I;
      mlist[i].I = mlist[i].J;
      mlist[i].J = tmp;
      mlist[i].Vimag = -mlist[i].Vimag;
    }
    std::sort(mlist,mlist+nnz);
    T** B = (T**) ConvertIJVtoRLEComplex<T>(mlist,nnz,cols,rows);
    delete mlist;
    return B;
  }

  void* SparseArrayTranspose(Class dclass, int rows, int cols, const void* cp) {
    switch(dclass) {
    case FM_LOGICAL:
      return SparseArrayTransposeReal<uint32>(rows, cols, (const uint32**) cp);
    case FM_INT32:
      return SparseArrayTransposeReal<int32>(rows, cols, (const int32**) cp);
    case FM_FLOAT:
      return SparseArrayTransposeReal<float>(rows, cols, (const float**) cp);
    case FM_DOUBLE:
      return SparseArrayTransposeReal<double>(rows, cols, (const double**) cp);
    case FM_COMPLEX:
      return SparseArrayTransposeComplex<float>(rows, cols, (const float**) cp);
    case FM_DCOMPLEX:
      return SparseArrayTransposeComplex<double>(rows, cols, (const double**) cp);
    }        
    throw Exception("unsupported type for SparseArrayTranspose");
  }

  void* SparseArrayHermitian(Class dclass, int rows, int cols, const void* cp) {
    switch(dclass) {
    case FM_COMPLEX:
      return SparseArrayHermitianComplex<float>(rows, cols, (const float**) cp);
    case FM_DCOMPLEX:
      return SparseArrayHermitianComplex<double>(rows, cols, (const double**) cp);
    }
    throw Exception("unsupported type for SparseArrayHermitian");
  }

  template <class T>
  void* SparseAddReal(int rows, int cols,const T** Amat,const T** Bmat) {
    int i;
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*2];
    for (i=0;i<cols;i++) {
      RLEDecoder<T> A(Amat[i],rows);
      RLEDecoder<T> B(Bmat[i],rows);
      RLEEncoder<T> C(buffer,rows); 
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(A.value() + B.value());
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  C.set(A.row());
	  C.push(A.value());
	  A.advance();
	} else {
	  C.set(B.row());
	  C.push(B.value());
	  B.advance();
	}
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  template <class T>
  void* SparseAddComplex(int rows, int cols,const T** Amat,const T** Bmat) {
    int i;
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*4];
    for (i=0;i<cols;i++) {
      RLEDecoderComplex<T> A(Amat[i],rows);
      RLEDecoderComplex<T> B(Bmat[i],rows);
      RLEEncoderComplex<T> C(buffer,rows); 
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(A.value_real() + B.value_real(),
		 A.value_imag() + B.value_imag());
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  C.set(A.row());
	  C.push(A.value_real(),A.value_imag());
	  A.advance();
	} else {
	  C.set(B.row());
	  C.push(B.value_real(),B.value_imag());
	  B.advance();
	}
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  void* SparseSparseAdd(Class dclass, const void *ap, int rows, int cols,
			const void *bp) {
    switch(dclass) {
    case FM_INT32:
      return SparseAddReal<int32>(rows, cols, (const int32**) ap,
				  (const int32**) bp);
    case FM_FLOAT:
      return SparseAddReal<float>(rows, cols, (const float**) ap,
				  (const float**) bp);
    case FM_DOUBLE:
      return SparseAddReal<double>(rows, cols, (const double**) ap,
				   (const double**) bp);
    case FM_COMPLEX:
      return SparseAddComplex<float>(rows, cols, (const float**) ap,
				     (const float**) bp);
    case FM_DCOMPLEX:
      return SparseAddComplex<double>(rows, cols, (const double**) ap,
				      (const double**) bp);
    }
    throw Exception("unsupported type for SparseSparseAdd");
  }


  template <class T>
  void* SparseSubtractReal(int rows, int cols,const T** Amat,const T** Bmat) {
    int i;
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*2];
    for (i=0;i<cols;i++) {
      RLEDecoder<T> A(Amat[i],rows);
      RLEDecoder<T> B(Bmat[i],rows);
      RLEEncoder<T> C(buffer,rows); 
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(A.value() - B.value());
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  C.set(A.row());
	  C.push(A.value());
	  A.advance();
	} else {
	  C.set(B.row());
	  C.push(-B.value());
	  B.advance();
	}
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  template <class T>
  void* SparseSubtractComplex(int rows, int cols,const T** Amat,const T** Bmat) {
    int i;
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*4];
    for (i=0;i<cols;i++) {
      RLEDecoderComplex<T> A(Amat[i],rows);
      RLEDecoderComplex<T> B(Bmat[i],rows);
      RLEEncoderComplex<T> C(buffer,rows); 
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(A.value_real() - B.value_real(),
		 A.value_imag() - B.value_imag());
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  C.set(A.row());
	  C.push(A.value_real(),A.value_imag());
	  A.advance();
	} else {
	  C.set(B.row());
	  C.push(-B.value_real(),-B.value_imag());
	  B.advance();
	}
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  void* SparseSparseSubtract(Class dclass, const void *ap, int rows, int cols,
			     const void *bp) {
    switch(dclass) {
    case FM_INT32:
      return SparseSubtractReal<int32>(rows, cols, (const int32**) ap,
				       (const int32**) bp);
    case FM_FLOAT:
      return SparseSubtractReal<float>(rows, cols, (const float**) ap,
				       (const float**) bp);
    case FM_DOUBLE:
      return SparseSubtractReal<double>(rows, cols, (const double**) ap,
					(const double**) bp);
    case FM_COMPLEX:
      return SparseSubtractComplex<float>(rows, cols, (const float**) ap,
					  (const float**) bp);
    case FM_DCOMPLEX:
      return SparseSubtractComplex<double>(rows, cols, (const double**) ap,
					   (const double**) bp);
    }
    throw Exception("unsupported type for SparseSparseSubtract");
  }

  template <class T>
  void* SparseMultiplyReal(int rows, int cols,const T** Amat,const T** Bmat) {
    int i;
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*2];
    for (i=0;i<cols;i++) {
      RLEDecoder<T> A(Amat[i],rows);
      RLEDecoder<T> B(Bmat[i],rows);
      RLEEncoder<T> C(buffer,rows); 
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(A.value()*B.value());
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  A.advance();
	} else {
	  B.advance();
	}
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  template <class T>
  void* SparseMultiplyComplex(int rows, int cols,const T** Amat,const T** Bmat) {
    int i;
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*4];
    for (i=0;i<cols;i++) {
      RLEDecoderComplex<T> A(Amat[i],rows);
      RLEDecoderComplex<T> B(Bmat[i],rows);
      RLEEncoderComplex<T> C(buffer,rows); 
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(A.value_real()*B.value_real()-
		 A.value_imag()*B.value_imag(),
		 A.value_real()*B.value_imag()+
		 A.value_imag()*B.value_real());
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  A.advance();
	} else {
	  B.advance();
	}
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  void* SparseSparseMultiply(Class dclass, const void *ap, int rows, int cols,
			     const void *bp) {
    switch(dclass) {
    case FM_INT32:
      return SparseMultiplyReal<int32>(rows, cols, (const int32**) ap,
				       (const int32**) bp);
    case FM_FLOAT:
      return SparseMultiplyReal<float>(rows, cols, (const float**) ap,
				       (const float**) bp);
    case FM_DOUBLE:
      return SparseMultiplyReal<double>(rows, cols, (const double**) ap,
					(const double**) bp);
    case FM_COMPLEX:
      return SparseMultiplyComplex<float>(rows, cols, (const float**) ap,
					  (const float**) bp);
    case FM_DCOMPLEX:
      return SparseMultiplyComplex<double>(rows, cols, (const double**) ap,
					   (const double**) bp);
    }
    throw Exception("unsupported type for SparseSparseMultiply");
  }
  
  template <class T>
  void* SparseScalarMultiplyReal(int rows, int cols, const T** Amat, 
				 const T* Bval) {
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*2];
    for (int i=0;i<cols;i++) {
      RLEDecoder<T> A(Amat[i],rows);
      RLEEncoder<T> C(buffer,rows);
      A.update();
      while (A.more()) {
	C.set(A.row());
	C.push(A.value()*Bval[0]);
	A.advance();
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  template <class T>
  void* SparseScalarMultiplyComplex(int rows, int cols, const T** Amat, 
				    const T* Bval) {
    T** Cmat;
    Cmat = new T*[cols];
    T* buffer = new T[rows*4];
    for (int i=0;i<cols;i++) {
      RLEDecoderComplex<T> A(Amat[i],rows);
      RLEEncoderComplex<T> C(buffer,rows);
      A.update();
      while (A.more()) {
	C.set(A.row());
	C.push(A.value_real()*Bval[0]-A.value_imag()*Bval[1],
	       A.value_real()*Bval[1]+A.value_imag()*Bval[0]);
	A.advance();
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    return Cmat;
  }

  template <class T>
  float** SparseOnesFuncReal(int rows, int cols, const T** Amat) {
    float** Cmat;
    Cmat = new float*[cols];
    float* buffer = new float[rows*2];
    for (int i=0;i<cols;i++) {
      RLEDecoder<T> A(Amat[i],rows);
      RLEEncoder<float> C(buffer,rows);
      A.update();
      while (A.more()) {
	C.set(A.row());
	C.push(1.0f);
	A.advance();
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    return Cmat;
  }

  template <class T>
  float** SparseOnesFuncComplex(int rows, int cols, const T** Amat) {
    float** Cmat;
    Cmat = new float*[cols];
    float* buffer = new float[rows*2];
    for (int i=0;i<cols;i++) {
      RLEDecoderComplex<T> A(Amat[i],rows);
      RLEEncoder<float> C(buffer,rows);
      A.update();
      while (A.more()) {
	C.set(A.row());
	C.push(1.0f);
	A.advance();
      }
      C.end();
      Cmat[i] = C.copyout();
    }
    return Cmat;
  }


  void* SparseOnesFunc(Class dclass, int Arows, int Acols, const void *Ap) {
    switch(dclass) {
    case FM_LOGICAL:
      return SparseOnesFuncReal<uint32>(Arows,Acols,(const uint32**)Ap);
    case FM_INT32:
      return SparseOnesFuncReal<int32>(Arows,Acols,(const int32**)Ap);
    case FM_FLOAT:
      return SparseOnesFuncReal<float>(Arows,Acols,(const float**)Ap);
    case FM_DOUBLE:
      return SparseOnesFuncReal<double>(Arows,Acols,(const double**)Ap);
    case FM_COMPLEX:
      return SparseOnesFuncComplex<float>(Arows,Acols,(const float**)Ap);
    case FM_DCOMPLEX:
      return SparseOnesFuncComplex<double>(Arows,Acols,(const double**)Ap);
    }
    throw Exception("unsupported type for SparseOnesFunc");
  }

  void* SparseScalarMultiply(Class dclass, const void *ap, int rows, int cols,
			     const void *bp) {
    switch(dclass) {
    case FM_INT32:
      return SparseScalarMultiplyReal<int32>(rows, cols, (const int32**) ap,
					     (const int32*) bp);
    case FM_FLOAT:
      return SparseScalarMultiplyReal<float>(rows, cols, (const float**) ap,
					     (const float*) bp);
    case FM_DOUBLE:
      return SparseScalarMultiplyReal<double>(rows, cols, (const double**) ap,
					      (const double*) bp);
    case FM_COMPLEX:
      return SparseScalarMultiplyComplex<float>(rows, cols, (const float**) ap,
						(const float*) bp);
    case FM_DCOMPLEX:
      return SparseScalarMultiplyComplex<double>(rows, cols, 
						 (const double**) ap,
						 (const double*) bp);
    }
    throw Exception("unsupported type for SparseScalarMultiply");
  }

  int ConvertSparseCCSReal(int rows, int cols, const double **Ap, int* &Acolstart,
			   int* &Arowindx, double* &Adata) {
    // Get number of nonzeros
    int nnz = CountNonzerosReal<double>(Ap,rows,cols);
    // Set up the output arrays
    Acolstart = new int[cols+1];
    Arowindx = new int[nnz];
    Adata = new double[nnz];
    // We have to unpack the matrix into these arrays... we do this
    // by traversing the columns
    int m = 0;
    Acolstart[0] = 0;
    for (int i=0;i<cols;i++) {
      RLEDecoder<double> A(Ap[i],rows);
      A.update();
      while (A.more()) {
	Arowindx[m] = A.row();
	Adata[m++] = A.value();
	A.advance();
      }
      Acolstart[i+1] = m;
    }
    return nnz;
  }

  int ConvertSparseCCSComplex(int rows, int cols, const double **Ap, 
			      int* &Acolstart,
			      int* &Arowindx, double* &Adata,
			      double* &Aimag) {
    // Get number of nonzeros
    int nnz = CountNonzerosComplex<double>(Ap,rows,cols);
    // Set up the output arrays
    Acolstart = new int[cols+1];
    Arowindx = new int[nnz];
    Adata = new double[nnz];
    Aimag = new double[nnz];
    // We have to unpack the matrix into these arrays... we do this
    // by traversing the columns
    int m = 0;
    Acolstart[0] = 0;
    for (int i=0;i<cols;i++) {
      RLEDecoderComplex<double> A(Ap[i],rows);
      A.update();
      while (A.more()) {
	Arowindx[m] = A.row();
	Adata[m] = A.value_real();
	Aimag[m++] = A.value_imag();
	A.advance();
      }
      Acolstart[i+1] = m;
    }
    return nnz;
  }

  void* SparseSolveLinEqReal(int Arows, int Acols, const void *Ap, 
			     int Brows, int Bcols, const void *Bp) {
    // Convert A into CCS form
    int *Acolstart;
    int *Arowindx;
    double *Adata;
    int nnz;
    nnz = ConvertSparseCCSReal(Arows, Acols, (const double**) Ap, 
			       Acolstart, Arowindx, Adata);
    double *null = (double *) NULL ;
    void *Symbolic, *Numeric ;
    (void) umfpack_di_symbolic (Acols, Acols, Acolstart, Arowindx, Adata, 
				&Symbolic, null, null);
    (void) umfpack_di_numeric (Acolstart, Arowindx, Adata, Symbolic, 
			       &Numeric, null, null) ;
    umfpack_di_free_symbolic (&Symbolic) ;
    double *x = (double*) Malloc(sizeof(double)*Arows*Bcols);
    const double *b = (const double*) Bp;
    for (int i=0;i<Bcols;i++)
      (void) umfpack_di_solve (UMFPACK_A, Acolstart, Arowindx, 
			       Adata, x+i*Arows, b+i*Brows, Numeric, 
			       null, null) ;
    umfpack_di_free_numeric (&Numeric);
    delete Acolstart;
    delete Arowindx;
    delete Adata;
    return x;
  }
  
  void* SparseSolveLinEqComplex(int Arows, int Acols, const void *Ap, 
				int Brows, int Bcols, const void *Bp) {
    // Convert A into CCS form
    int *Acolstart;
    int *Arowindx;
    double *Adata;
    double *Aimag;
    int nnz;
    nnz = ConvertSparseCCSComplex(Arows, Acols, (const double**) Ap, 
				  Acolstart, Arowindx, Adata, Aimag);
    double *null = (double *) NULL ;
    void *Symbolic, *Numeric ;
    (void) umfpack_zi_symbolic (Acols, Acols, Acolstart, Arowindx, Adata, 
				Aimag, &Symbolic, null, null);
    (void) umfpack_zi_numeric (Acolstart, Arowindx, Adata, Aimag, Symbolic, 
			       &Numeric, null, null) ;
    umfpack_zi_free_symbolic (&Symbolic) ;
    double *x = (double*) Malloc(sizeof(double)*2*Arows*Bcols);
    double *xr = (double*) Malloc(sizeof(double)*Arows);
    double *xi = (double*) Malloc(sizeof(double)*Arows);
    double *br = (double*) Malloc(sizeof(double)*Brows);
    double *bi = (double*) Malloc(sizeof(double)*Brows);
    const double * bp = (const double *) Bp;
    for (int i=0;i<Bcols;i++) {
      for (int j=0;j<Brows;j++) {
	br[j] = bp[2*Brows*i+2*j];
	bi[j] = bp[2*Brows*i+2*j+1];
      }
      memset(xr,0,sizeof(double)*Arows);
      memset(xi,0,sizeof(double)*Arows);
      (void) umfpack_zi_solve (UMFPACK_A, Acolstart, Arowindx, 
			       Adata, Aimag, xr, xi, br, bi, Numeric, 
			       null, null);
      for (int j=0;j<Arows;j++) {
	x[2*Arows*i+2*j] = xr[j];
	x[2*Arows*i+2*j+1] = xi[j];
      }
    }
    umfpack_zi_free_numeric (&Numeric) ;
    delete Acolstart;
    delete Arowindx;
    delete Adata;
    delete Aimag;
    Free(xr);
    Free(xi);
    Free(br);
    Free(bi);
    return x;
  }

  IJVEntry<double>* ConvertCCSToIJVListReal(int *Ap, int *Ai, double *Ax, 
					    int Acols, int Anz) {
    IJVEntry<double>* T = new IJVEntry<double>[Anz];
    int i, j, p, q;
    p = 0;
    for (i=0;i<Acols;i++) {
      for (j=0;j<(Ap[i+1] - Ap[i]);j++) {
	T[p].I = Ai[p];
	T[p].J = i;
	T[p].Vreal = Ax[p];
	p++;
      }
    }
    return T;
  }

  IJVEntry<double>* ConvertCCSToIJVListComplex(int *Ap, int *Ai, double *Ax, double *Ay,
					       int Acols, int Anz) {
    IJVEntry<double>* T = new IJVEntry<double>[Anz];
    int i, j, p, q;
    p = 0;
    for (i=0;i<Acols;i++) {
      for (j=0;j<(Ap[i+1] - Ap[i]);j++) {
	T[p].I = Ai[p];
	T[p].J = i;
	T[p].Vreal = Ax[p];
	T[p].Vimag = Ay[p];
	p++;
      }
    }
    return T;
  }

  void* SparseSolveLinEq(Class dclass, int Arows, int Acols, const void *Ap,
			 int Brows, int Bcols, const void *Bp) {
    if (dclass == FM_DOUBLE)
      return SparseSolveLinEqReal(Arows, Acols, Ap, Brows, Bcols, Bp);
    else
      return SparseSolveLinEqComplex(Arows, Acols, Ap, Brows, Bcols, Bp);
  }

  ArrayVector SparseLUDecomposeReal(int Arows, int Acols, const void *Ap) {
    // Convert A into CCS form
    int *Acolstart;
    int *Arowindx;
    double *Adata;
    int nnz;
    nnz = ConvertSparseCCSReal(Arows, Acols, (const double**) Ap, 
			       Acolstart, Arowindx, Adata);
    double *null = (double *) NULL ;
    void *Symbolic, *Numeric ;
    (void) umfpack_di_symbolic (Acols, Acols, Acolstart, Arowindx, Adata, 
				&Symbolic, null, null);
    (void) umfpack_di_numeric (Acolstart, Arowindx, Adata, Symbolic, 
			       &Numeric, null, null);
    // Set up the output arrays for the LU Decomposition.
    // The first matrix is L, which is stored in comprssed row form.
    int lnz;
    int unz;
    int n_row;
    int n_col;
    int nz_udiag;

    (void) umfpack_di_get_lunz(&lnz,&unz,&n_row,&n_col,&nz_udiag,Numeric);

    int *Lp = new int[Arows+1];
    int *Lj = new int[lnz];
    double *Lx = new double[lnz];

    int *Up = new int[Acols+1];
    int *Ui = new int[unz];
    double *Ux = new double[unz];

    int32 *P = (int32*) Malloc(sizeof(int32)*Arows);
    int32 *Q = (int32*) Malloc(sizeof(int32)*Acols);
    double *Rs = new double[Arows];
    
    int do_recip;
    umfpack_di_get_numeric(Lp, Lj, Lx, Up, Ui, Ux, P, Q, NULL, &do_recip, Rs, Numeric);

    for (int i=0;i<Arows;i++)
      P[i]++;

    for (int i=0;i<Acols;i++)
      Q[i]++;

    IJVEntry<double>* llist = ConvertCCSToIJVListReal(Lp,Lj,Lx,Arows,lnz);
    for (int j=0;j<lnz;j++) {
      int tmp;
      tmp = llist[j].I;
      llist[j].I = llist[j].J;
      llist[j].J = tmp;
    }
    IJVEntry<double>* ulist = ConvertCCSToIJVListReal(Up,Ui,Ux,Acols,unz);
    IJVEntry<double>* rlist = new IJVEntry<double>[Arows];
    std::sort(llist,llist+lnz);
    for (int i=0;i<Arows;i++) {
      rlist[i].I = i;
      rlist[i].J = i;
      if (do_recip)
	rlist[i].Vreal = Rs[i];
      else
	rlist[i].Vreal = 1.0/Rs[i];
    }
    ArrayVector retval;
    // Push L, U, P, Q, R
    int Amid;
    Amid = (Arows < Acols) ? Arows : Acols;
    retval.push_back(Array(FM_DOUBLE,Dimensions(Arows,Amid),
			   ConvertIJVtoRLEReal<double>(llist,lnz,Arows,Amid),true));
    retval.push_back(Array(FM_DOUBLE,Dimensions(Amid,Acols),
			   ConvertIJVtoRLEReal<double>(ulist,unz,Amid,Acols),true));
    retval.push_back(Array(FM_INT32,Dimensions(1,Arows),P,false));
    retval.push_back(Array(FM_INT32,Dimensions(1,Acols),Q,false));
    retval.push_back(Array(FM_DOUBLE,Dimensions(Arows,Arows),
			   ConvertIJVtoRLEReal<double>(rlist,Arows,Arows,Arows),true));
    umfpack_di_free_symbolic(&Symbolic);
    umfpack_di_free_numeric(&Numeric);
    delete[] rlist;
    delete[] ulist;
    delete[] llist;
    delete[] Rs;
    delete[] Ux;
    delete[] Ui;
    delete[] Up;
    delete[] Lx;
    delete[] Lj;
    delete[] Lp;
    delete[] Acolstart;
    delete[] Arowindx;
    delete[] Adata;
    return retval;
  }

  ArrayVector SparseLUDecomposeComplex(int Arows, int Acols, const void *Ap) {
    // Convert A into CCS form
    int *Acolstart;
    int *Arowindx;
    double *Adata;
    double *Aimag;
    int nnz;
    nnz = ConvertSparseCCSComplex(Arows, Acols, (const double**) Ap, 
				  Acolstart, Arowindx, Adata, Aimag);
    double *null = (double *) NULL ;
    void *Symbolic, *Numeric ;
    (void) umfpack_zi_symbolic (Acols, Acols, Acolstart, Arowindx, Adata, 
				Aimag, &Symbolic, null, null);
    (void) umfpack_zi_numeric (Acolstart, Arowindx, Adata, Aimag, Symbolic, 
			       &Numeric, null, null);
    // Set up the output arrays for the LU Decomposition.
    // The first matrix is L, which is stored in comprssed row form.
    int lnz;
    int unz;
    int n_row;
    int n_col;
    int nz_udiag;

    (void) umfpack_zi_get_lunz(&lnz,&unz,&n_row,&n_col,&nz_udiag,Numeric);

    int *Lp = new int[Arows+1];
    int *Lj = new int[lnz];
    double *Lx = new double[lnz];
    double *Ly = new double[lnz];

    int *Up = new int[Acols+1];
    int *Ui = new int[unz];
    double *Ux = new double[unz];
    double *Uy = new double[unz];

    int32 *P = (int32*) Malloc(sizeof(int32)*Arows);
    int32 *Q = (int32*) Malloc(sizeof(int32)*Acols);

    double *Rs = new double[Arows];
    
    int do_recip;
    umfpack_zi_get_numeric(Lp, Lj, Lx, Ly, Up, Ui, Ux, Uy,
			   P, Q, NULL, NULL, &do_recip, Rs, Numeric);

    for (int i=0;i<Arows;i++)
      P[i]++;

    for (int i=0;i<Acols;i++)
      Q[i]++;

    IJVEntry<double>* llist = ConvertCCSToIJVListComplex(Lp,Lj,Lx,Ly,Arows,lnz);
    for (int j=0;j<lnz;j++) {
      int tmp;
      tmp = llist[j].I;
      llist[j].I = llist[j].J;
      llist[j].J = tmp;
    }
    IJVEntry<double>* ulist = ConvertCCSToIJVListComplex(Up,Ui,Ux,Uy,Acols,unz);
    IJVEntry<double>* rlist = new IJVEntry<double>[Arows];
    std::sort(llist,llist+lnz);
    for (int i=0;i<Arows;i++) {
      rlist[i].I = i;
      rlist[i].J = i;
      if (do_recip)
	rlist[i].Vreal = Rs[i];
      else
	rlist[i].Vreal = 1.0/Rs[i];
    }
    ArrayVector retval;
    // Push L, U, P, Q, R
    int Amid;
    Amid = (Arows < Acols) ? Arows : Acols;
    retval.push_back(Array(FM_DCOMPLEX,Dimensions(Arows,Amid),
			   ConvertIJVtoRLEComplex<double>(llist,lnz,Arows,Amid),true));
    retval.push_back(Array(FM_DCOMPLEX,Dimensions(Amid,Acols),
			   ConvertIJVtoRLEComplex<double>(ulist,unz,Amid,Acols),true));
    retval.push_back(Array(FM_INT32,Dimensions(1,Arows),P,false));
    retval.push_back(Array(FM_INT32,Dimensions(1,Acols),Q,false));
    retval.push_back(Array(FM_DOUBLE,Dimensions(Arows,Arows),
			   ConvertIJVtoRLEReal<double>(rlist,Arows,Arows,Arows),true));
    umfpack_di_free_symbolic(&Symbolic);
    umfpack_di_free_numeric(&Numeric);
    delete[] rlist;
    delete[] ulist;
    delete[] llist;
    delete[] Rs;
    delete[] Uy;
    delete[] Ux;
    delete[] Ui;
    delete[] Up;
    delete[] Ly;
    delete[] Lx;
    delete[] Lj;
    delete[] Lp;
    delete[] Acolstart;
    delete[] Arowindx;
    delete[] Adata;
    return retval;
  }

  
  ArrayVector SparseLUDecompose(int nargout, Array A) {
    if ((A.getDataClass() == FM_FLOAT) || (A.getDataClass() == FM_COMPLEX))
      throw Exception("FreeMat currently only supports the LU decomposition for double and dcomplex matrices");
    if (A.getDataClass() < FM_FLOAT)
      A.promoteType(FM_DOUBLE);
    int Arows;
    int Acols;
    Arows = A.getDimensionLength(0);
    Acols = A.getDimensionLength(1);
    if (Arows != Acols)
      throw Exception("FreeMat currently only supports LU decompositions for square matrices");
    if (A.getDataClass() == FM_DOUBLE)
      return SparseLUDecomposeReal(Arows, Acols, A.getSparseDataPointer());
    else
      return SparseLUDecomposeComplex(Arows, Acols, A.getSparseDataPointer());
  }

  template <class T>
  bool SparseIsSymmetricReal(int rows, int cols, const T** src) {
    int nnz;
    IJVEntry<T>* mlist = ConvertSparseToIJVListReal<T>(src, rows, cols, nnz);
    IJVEntry<T>* plist = new IJVEntry<T>[nnz];
    // Transpose the array
    for (int i=0;i<nnz;i++) {
      plist[i].I = mlist[i].J;
      plist[i].J = mlist[i].I;
      plist[i].Vreal = mlist[i].Vreal;
      plist[i].Vimag = 0;
      mlist[i].Vimag = 0;
    }
    // Sort the transposed array
    std::sort(plist,plist+nnz);
    // These should be equal 
    bool symmetric = true;
    for (int i=0;(i<nnz) && symmetric; i++) 
      symmetric = (mlist[i] == plist[i]);
    delete[] mlist;
    delete[] plist;
    return symmetric;
  }

  void DNEUPARPACKError(int info) {
    if (info == 1) 
      throw Exception("ARPACK Error: The Schur form computed by LAPACK routine dlahqr could not be reordered by LAPACK routine dtrsen.  Please file a bug report with the matrix and arguments that caused this error.");
    if (info == -1)
      throw Exception("N must be positive.");
    if (info == -2)
      throw Exception("NEV must be positive.");
    if (info == -3)
      throw Exception("Too many eigenvalues/eigenvectors requested.");
    if (info == -5)
      throw Exception("WHICH must be one of 'LM', 'SM', 'LR', 'SR', 'LI', 'SI'");
    if (info == -6)
      throw Exception("BMAT must be one of 'I' or 'G'.");
    if (info == -7)
      throw Exception("Length of private work WORKL array is not sufficient.");
    if (info == -8)
      throw Exception("Error return from calculation of a real Schur form.  Informational error from LAPACK routine dlahqr.");
    if (info == -9)
      throw Exception("Error return from calculation of eigenvectors. Onformational error from LAPACK routine dtrevc.");
    if (info == -10)
      throw Exception("IPARAM(7) must be 1,2,3,4.");
    if (info == -11)
      throw Exception("IPARAM(7) = 1 and BMAT = 'G' are incompatible.");
    if (info == -12)
      throw Exception("HOWMNY = 'S' not yet implemented");
    if (info == -13)
      throw Exception("HOWMNY must be one of 'A' or 'P' if RVEC = .true.");
    if (info == -14)
      throw Exception("DNAUPD did not find any eigenvalues to sufficient accuracy.");
    throw Exception("Generic ARPACK error occured in call to dneupd.");
  }

  void DNAUPARPACKError(int info) {
    if (info == 1)
      throw Exception("Maximum number of iterations taken.  All possible eigenvalues of OP has been found.");
    if (info == 3)
      throw Exception("No shifts could be applied during a cycle of the implicitly restarted Arnoldi iteration. One possibility is to increase the size of NCV relative to NEV.");
    if (info == -1)
      throw Exception("Problem size must be positive.");
    if (info == -2)
      throw Exception("Number of requested eigenvalues must be positive.");
    if (info == -3)
      throw Exception("Illegal value for number of spanning vectors (ncv) - ARPACK cannot solve for all of the eigenvalues of a matrix (use eig for that).");
    if (info == -4)
      throw Exception("The maximum number of Arnoldi update iteration must be greater than zero.");
    if (info == -5)
      throw Exception("WHICH must be one of 'LM', 'SM', 'LR', 'SR', 'LI', 'SI'");
    if (info == -6)
      throw Exception("BMAT must be one of 'I' or 'G'");
    if (info == -7)
      throw Exception("Length of private work array is not sufficient.");
    if (info == -8)
      throw Exception("Error return from LAPACK eigenvalue calculation;");
    if (info == -9)
      throw Exception("Starting vector is zero.");
    if (info == -10)
      throw Exception("Illegal mode selection for ARPACK dnaup");
    if (info == -11)
      throw Exception("IPARAM(7) = 1 and BMAT = 'G' are incompatible.");
    if (info == -12)
      throw Exception("IPARAM(1) must be equal to 0 or 1.");
    throw Exception("Could not build an Arnoldi factorization.");
  }

#define min(a,b) ((a) < (b) ? (a) : (b))
#define swap(a,b) {double tmp; tmp = a; a = b; b = tmp;}

  ArrayVector SparseEigDecomposeNonsymmetricReal(double **ap, int rows, int cols, 
						 int nev, int nargout, char* which) {
    // Initialization call
    int ido = 0;
    char bmat = 'I';
    int n = rows;
    // How many eigenvalues to compute
    char cmach = 'E';
    double tol = dlamch_(&cmach);
    double *resid = new double[n];
    int ncv = 2*nev+1;
    if (ncv > n) ncv = n;
    double *v = new double[n*ncv];
    int ldv = n;
    int iparam[11];
    iparam[0] = 1;
    iparam[1] = 0;
    iparam[2] = 300;
    iparam[3] = 1;
    iparam[4] = 0;
    iparam[5] = 0;
    iparam[6] = 1;
    iparam[7] = 0;
    iparam[8] = 0;
    iparam[9] = 0;
    iparam[10] = 0;
    double *workd = new double[3*n];
    int lworkl = 3*ncv*ncv+6*ncv;
    double *workl = new double[lworkl];
    int info = 0;
    int ipntr[14];
    while (1) {
      dnaupd_(&ido, &bmat, &n , which, &nev, &tol, resid, 
	      &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	      &info);
      if ((ido == -1) || (ido == 1)) 
	SparseDenseRealMultiply<double>((double**)ap,rows,cols,
					workd+ipntr[0]-1, 1,
					workd+ipntr[1]-1);
      else
	break;
    }
    if (info < 0)
      DNAUPARPACKError(info);
    // Compute vectors and values
    int rvec;
    if (nargout <= 1)
      rvec = 0;
    else
      rvec = 1;
    char howmny = 'A';
    int *select = new int[ncv];
    double *dr = (double*) Malloc(sizeof(double)*(nev+1));
    double *di = (double*) Malloc(sizeof(double)*(nev+1));
    double *z;
    if (nargout <= 1)
      z = NULL;
    else
      z = (double*) Malloc(sizeof(double)*(n*(nev+1)));
    int ldz = n;
    double sigmar;
    double sigmai;
    double *workev = new double[3*ncv];
    int ierr;
    dneupd_(&rvec, &howmny, select, dr, di, z, &ldv, 
	    &sigmar, &sigmai, workev, &bmat, &n, which, &nev, &tol, 
	    resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	    &lworkl, &ierr);
    int nconv = iparam[4];
    if (ierr != 0)
      DNEUPARPACKError(ierr);
    // Reverse the vectors dr and di
    if (rvec == 0) {
      for (int i=0;i<(nconv)/2;i++) {
	swap(dr[i],dr[nconv-1-i]);
	swap(di[i],di[nconv-1-i]);
      }
    }
//     if (rvec == 1) {
//       // Reverse the columns of z...
//       for (int i=0;i<(nconv)/2;i++) 
// 	for (int j=0;j<n;j++)
// 	  swap(z[i*n+j],z[(nconv-1-i)*n+j]);
//     }
    delete[] resid;
    delete[] v;
    delete[] workd;
    delete[] workl;
    delete[] select;
    delete[] workev;
    // Check for complex eigenvalues
    bool anycomplex = false;
    for (int i=0;(!anycomplex) && (i<nconv);i++,anycomplex = (di[i] != 0));
    if (anycomplex) {
      double *eigvals = (double*) Malloc(nev*sizeof(double)*2);
      for (int i=0;i<min(nev,nconv);i++) {
	eigvals[2*i] = dr[i];
	eigvals[2*i+1] = di[i];
      }
      double *eigvecs;
      if (nargout > 1) {
	eigvecs = (double*) Malloc(nev*n*sizeof(double)*2);
	// if eigenvalue i is complex, then the corresponding eigenvector
	// should be constructed from columns i and i+1 of z if i is even
	// and columns i-1 and i of z if i is odd
	int vcol = 0;
	while (vcol < min(nconv,nev)) {
	  if (di[vcol] != 0) {
	    for (int j=0;j<n;j++) {
	      eigvecs[vcol*n*2+2*j] = z[vcol*n+j];
	      eigvecs[vcol*n*2+2*j+1] = z[(vcol+1)*n+j];
	      if ((vcol+1) < nev) {
		eigvecs[(vcol+1)*n*2+2*j] = z[vcol*n+j];
		eigvecs[(vcol+1)*n*2+2*j+1] = -z[(vcol+1)*n+j];
	      }
	    }
	    vcol += 2;
	  } else {
	    for (int j=0;j<n;j++)
	      eigvecs[vcol*n*2+2*j] = z[vcol*n+j];
	    vcol++;
	  }
	}
      }
      ArrayVector retval;
      if (nargout <= 1)
	retval.push_back(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals));
      else {
	retval.push_back(Array(FM_DCOMPLEX,Dimensions(n,nev),eigvecs));
	retval.push_back(Array::diagonalConstructor(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals),0));
      }
      Free(z);
      return retval;
    } else {
      ArrayVector retval;
      if (nargout <= 1)
	retval.push_back(Array(FM_DOUBLE,Dimensions(nev,1),dr));
      else {
	// I know that technically this is a bad thing... dr and z are larger than
	// they need to be, but I don't think this will cause any problems.
	retval.push_back(Array(FM_DOUBLE,Dimensions(n,nev),z));
	retval.push_back(Array::diagonalConstructor(Array(FM_DOUBLE,Dimensions(nev,1),dr),0));
      }
      Free(di);
      return retval;
    }
  } 

  ArrayVector SparseEigDecomposeSymmetricReal(double **ap, int rows, int cols, 
					      int nev, int nargout, char *which) {
    // Initialization call
    int ido = 0;
    char bmat = 'I';
    int n = rows;
    // How many eigenvalues to compute
    char cmach = 'E';
    double tol = dlamch_(&cmach);
    double *resid = new double[n];
    int ncv = 2*nev+1;
    if (ncv > n) ncv = n;
    double *v = new double[n*ncv];
    int ldv = n;
    int iparam[11];
    iparam[0] = 1;
    iparam[1] = 0;
    iparam[2] = 300;
    iparam[3] = 1;
    iparam[4] = 0;
    iparam[5] = 0;
    iparam[6] = 1;
    iparam[7] = 0;
    iparam[8] = 0;
    iparam[9] = 0;
    iparam[10] = 0;
    double *workd = new double[3*n];
    int lworkl = ncv*ncv+8*ncv;
    double *workl = new double[lworkl];
    int info = 0;
    int ipntr[11];
    while (1) {
      dsaupd_(&ido, &bmat, &n , which, &nev, &tol, resid, 
	      &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	      &info);
      if ((ido == -1) || (ido == 1)) 
	SparseDenseRealMultiply<double>((double**)ap,rows,cols,
					workd+ipntr[0]-1, 1,
					workd+ipntr[1]-1);
      else
	break;
    }
    if (info < 0)
      DNAUPARPACKError(info);
    // Compute vectors and values
    int rvec;
    if (nargout <= 1)
      rvec = 0;
    else
      rvec = 1;
    char howmny = 'A';
    int *select = new int[ncv];
    double *d = (double*) Malloc(sizeof(double)*nev);
    double *z;
    if (nargout <= 1)
      z = NULL;
    else
      z = (double*) Malloc(sizeof(double)*(n*nev));
    int ldz = n;
    double sigma;
    int ierr;
    dseupd_(&rvec, &howmny, select, d,z, &ldv, 
	    &sigma,&bmat, &n, which, &nev, &tol, 
	    resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	    &lworkl, &ierr);
    int nconv = iparam[4];
    if (ierr != 0)
      DNEUPARPACKError(ierr);
    // Reverse the vectors dr and di
    for (int i=0;i<(nconv)/2;i++) 
      swap(d[i],d[nconv-1-i]);
    if (rvec == 1) {
      for (int i=0;i<(nconv)/2;i++)
 	for (int j=0;j<n;j++)
 	  swap(z[i*n+j],z[(nconv-1-i)*n+j]);
    }
    delete[] resid;
    delete[] v;
    delete[] workd;
    delete[] workl;
    delete[] select;
    ArrayVector retval;
    // I know that technically this is a bad thing... dr and z are larger than
    // they need to be, but I don't think this will cause any problems.
    if (nargout <= 1) {
      retval.push_back(Array(FM_DOUBLE,Dimensions(nev,1),d));
      Free(z);
    } else {
      retval.push_back(Array(FM_DOUBLE,Dimensions(n,nev),z));
      retval.push_back(Array::diagonalConstructor(Array(FM_DOUBLE,Dimensions(nev,1),d),0));
    }
    return retval;
  } 

  ArrayVector SparseEigDecomposeNonsymmetricComplex(double **ap, int rows, int cols, 
						   int nev, int nargout, char *which) {
    // Initialization call
    int ido = 0;
    char bmat = 'I';
    int n = rows;
    // How many eigenvalues to compute
    char cmach = 'E';
    double tol = dlamch_(&cmach);
    double *resid = new double[2*n];
    int ncv = 2*nev+1;
    if (ncv > n) ncv = n;
    double *v = new double[2*n*ncv];
    int ldv = n;
    int iparam[11];
    iparam[0] = 1;
    iparam[1] = 0;
    iparam[2] = 300;
    iparam[3] = 1;
    iparam[4] = 0;
    iparam[5] = 0;
    iparam[6] = 1;
    iparam[7] = 0;
    iparam[8] = 0;
    iparam[9] = 0;
    iparam[10] = 0;
    double *workd = new double[2*3*n];
    int lworkl = 3*ncv*ncv+5*ncv;
    double *workl = new double[2*lworkl];
    double *rwork = new double[ncv];
    int info = 0;
    int ipntr[14];
    while (1) {
      znaupd_(&ido, &bmat, &n , which, &nev, &tol, resid, 
	      &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	      rwork, &info);
      if ((ido == -1) || (ido == 1)) 
	SparseDenseComplexMultiply<double>((double**)ap,rows,cols,
					   workd+2*(ipntr[0]-1), 1,
					   workd+2*(ipntr[1]-1));
      else
	break;
    }
    if (info < 0)
      DNAUPARPACKError(info);
    // Compute vectors and values
    int rvec;
    if (nargout <= 1)
      rvec = 0;
    else
      rvec = 1;
    char howmny = 'A';
    int *select = new int[ncv];
    double *d = (double*) Malloc(2*sizeof(double)*(nev+1));
    double *z;
    if (nargout <= 1)
      z = NULL;
    else
      z = (double*) Malloc(2*sizeof(double)*(n*(nev+1)));
    int ldz = n;
    double sigma[2];
    double *workev = new double[2*2*ncv];
    int ierr;
    zneupd_(&rvec, &howmny, select, d, z, &ldv, 
	    sigma, workev, &bmat, &n, which, &nev, &tol, 
	    resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	    &lworkl, rwork, &ierr);
    int nconv = iparam[4];
    if (ierr != 0)
      DNEUPARPACKError(ierr);
    // Reverse the vectors dr and di
    if (rvec == 0) {
      for (int i=0;i<(nconv)/2;i++) {
	swap(d[2*i],d[2*(nconv-1-i)]);
	swap(d[2*i+1],d[2*(nconv-1-i)+1]);
      }
    }
    delete[] resid;
    delete[] v;
    delete[] workd;
    delete[] workl;
    delete[] select;
    delete[] workev;
    double *eigvals = (double*) Malloc(nev*sizeof(double)*2);
    for (int i=0;i<min(nev,nconv);i++) {
      eigvals[2*i] = d[2*i];
      eigvals[2*i+1] = d[2*i+1];
    }
    double *eigvecs;
    if (nargout > 1) {
      eigvecs = (double*) Malloc(nev*n*sizeof(double)*2);
      for (int i=0;i<min(nev,nconv);i++) {
	for (int j=0;j<2*n;j++) 
	  eigvecs[i*2*n+j] = d[i*2*n+j];
      }
    }
    ArrayVector retval;
    if (nargout <= 1)
      retval.push_back(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals));
    else {
      retval.push_back(Array(FM_DCOMPLEX,Dimensions(n,nev),eigvecs));
      retval.push_back(Array::diagonalConstructor(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals),0));
    }
    Free(z);
    return retval;
  } 

  // For shifted eigendecomposition problems, we have to change the behavior of the
  // reverse communication interface.  This is done by changing the operation mode
  // to 3, and by solving (A-shift*I)x = b.  Because this equation has to be solved
  // multiple times, we calculate a C matrix as A-shift*I, decompose it using the
  // UMFPack routines, and then use the result in repeated solutions.
  ArrayVector SparseEigDecomposeNonsymmetricRealShifted(double **ap, int rows, int cols, 
							int nev, int nargout, double shift) {
    // Set up the scaled identity matrix
    double** scI = (double**) MakeSparseScaledIdentityReal<double>(shift, rows);
    // Compute A - scI
    double** C = (double**) SparseSubtractReal(rows,cols,(const double**) ap,(const double**) scI);
    // Factor it...
    // Convert C into CCS form
    int *Ccolstart;
    int *Crowindx;
    double *Cdata;
    int nnz;
    nnz = ConvertSparseCCSReal(rows, cols, (const double**) C, Ccolstart, Crowindx, Cdata);
    double *null = (double *) NULL ;
    void *Symbolic, *Numeric ;
    int res;
    res = umfpack_di_symbolic (cols, cols, Ccolstart, Crowindx, Cdata, &Symbolic, null, null);
    res = umfpack_di_numeric (Ccolstart, Crowindx, Cdata, Symbolic, &Numeric, null, null);
    umfpack_di_free_symbolic (&Symbolic);    
    // Initialization call
    int ido = 0;
    char bmat = 'I';
    int n = rows;
    char *which = "LM";
    // How many eigenvalues to compute
    char cmach = 'E';
    double tol = dlamch_(&cmach);
    double *resid = new double[n];
    int ncv = 2*nev+1;
    if (ncv > n) ncv = n;
    double *v = new double[n*ncv];
    int ldv = n;
    int iparam[11];
    iparam[0] = 1;
    iparam[1] = 0;
    iparam[2] = 300;
    iparam[3] = 1;
    iparam[4] = 0;
    iparam[5] = 0;
    iparam[6] = 3;
    iparam[7] = 0;
    iparam[8] = 0;
    iparam[9] = 0;
    iparam[10] = 0;
    double *workd = new double[3*n];
    int lworkl = 3*ncv*ncv+6*ncv;
    double *workl = new double[lworkl];
    int info = 0;
    int ipntr[14];
    while (1) {
      dnaupd_(&ido, &bmat, &n , which, &nev, &tol, resid, 
	      &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	      &info);
      if ((ido == -1) || (ido == 1)) 
	res = umfpack_di_solve(UMFPACK_A, Ccolstart, Crowindx, Cdata, workd+ipntr[1]-1,workd+ipntr[0]-1,Numeric, null, null);
      else if (ido == 2)
	memcpy( workd+ipntr[1]-1, workd+ipntr[0]-1, sizeof(double)*rows);
      else
	break;
    }

    // Free the numeric component
    umfpack_di_free_numeric(&Numeric);
    if (info < 0)
      DNAUPARPACKError(info);
    // Compute vectors and values
    int rvec;
    if (nargout <= 1)
      rvec = 0;
    else
      rvec = 1;
    char howmny = 'A';
    int *select = new int[ncv];
	//lambda_a = 1/lambda_c + sigma
    double *dr = (double*) Malloc(sizeof(double)*(nev+1));
    double *di = (double*) Malloc(sizeof(double)*(nev+1));
    double *z;
    if (nargout <= 1)
      z = NULL;
    else
      z = (double*) Malloc(sizeof(double)*(n*(nev+1)));
    int ldz = n;
    double sigmar;
    double sigmai;
    sigmar = shift;
    sigmai = 0.0;
    double *workev = new double[3*ncv];
    int ierr;
    dneupd_(&rvec, &howmny, select, dr, di, z, &ldv, 
	    &sigmar, &sigmai, workev, &bmat, &n, which, &nev, &tol, 
	    resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	    &lworkl, &ierr);
    int nconv = iparam[4];
    if (ierr != 0)
      DNEUPARPACKError(ierr);
    // Reverse the vectors dr and di
    if (rvec == 0) {
      for (int i=0;i<(nconv)/2;i++) {
	swap(dr[i],dr[nconv-1-i]);
	swap(di[i],di[nconv-1-i]);
      }
    }
    delete[] resid;
    delete[] v;
    delete[] workd;
    delete[] workl;
    delete[] select;
    delete[] workev;
    // Check for complex eigenvalues
    bool anycomplex = false;
    for (int i=0;(!anycomplex) && (i<nconv);i++,anycomplex = (di[i] != 0));
    if (anycomplex) {
      double *eigvals = (double*) Malloc(nev*sizeof(double)*2);
      for (int i=0;i<min(nev,nconv);i++) {
	eigvals[2*i] = dr[i];
	eigvals[2*i+1] = di[i];
      }
      double *eigvecs;
      if (nargout > 1) {
	eigvecs = (double*) Malloc(nev*n*sizeof(double)*2);
	// if eigenvalue i is complex, then the corresponding eigenvector
	// should be constructed from columns i and i+1 of z if i is even
	// and columns i-1 and i of z if i is odd
	int vcol = 0;
	while (vcol < min(nconv,nev)) {
	  if (di[vcol] != 0) {
	    for (int j=0;j<n;j++) {
	      eigvecs[vcol*n*2+2*j] = z[vcol*n+j];
	      eigvecs[vcol*n*2+2*j+1] = z[(vcol+1)*n+j];
	      if ((vcol+1) < nev) {
		eigvecs[(vcol+1)*n*2+2*j] = z[vcol*n+j];
		eigvecs[(vcol+1)*n*2+2*j+1] = -z[(vcol+1)*n+j];
	      }
	    }
	    vcol += 2;
	  } else {
	    for (int j=0;j<n;j++)
	      eigvecs[vcol*n*2+2*j] = z[vcol*n+j];
	    vcol++;
	  }
	}
      }
      ArrayVector retval;
      if (nargout <= 1)
	retval.push_back(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals));
      else {
	retval.push_back(Array(FM_DCOMPLEX,Dimensions(n,nev),eigvecs));
	retval.push_back(Array::diagonalConstructor(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals),0));
      }
      Free(z);
      return retval;
    } else {
      ArrayVector retval;
      if (nargout <= 1)
	retval.push_back(Array(FM_DOUBLE,Dimensions(nev,1),dr));
      else {
	// I know that technically this is a bad thing... dr and z are larger than
	// they need to be, but I don't think this will cause any problems.
	retval.push_back(Array(FM_DOUBLE,Dimensions(n,nev),z));
	retval.push_back(Array::diagonalConstructor(Array(FM_DOUBLE,Dimensions(nev,1),dr),0));
      }
      Free(di);
      return retval;
    }
  } 

  // For shifted eigendecomposition problems, we have to change the behavior of the
  // reverse communication interface.  This is done by changing the operation mode
  // to 3, and by solving (A-shift*I)x = b.  Because this equation has to be solved
  // multiple times, we calculate a C matrix as A-shift*I, decompose it using the
  // UMFPack routines, and then use the result in repeated solutions.
  ArrayVector SparseEigDecomposeNonsymmetricComplexShifted(double **ap, int rows, int cols, 
							int nev, int nargout, double *shift) {
    // Set up the scaled identity matrix
    double** scI = (double**) MakeSparseScaledIdentityComplex<double>(shift[0], shift[1], rows);
    // Compute A - scI
    double** C = (double**) SparseSubtractComplex(rows,cols,(const double**) ap,(const double**) scI);
    // Factor it...
    // Convert C into CCS form
    int *Ccolstart;
    int *Crowindx;
    double *Cdata;
    double *Cimag;
    int nnz;
    nnz = ConvertSparseCCSComplex(rows, cols, (const double**) C, Ccolstart, Crowindx, Cdata, Cimag);
    double *null = (double *) NULL ;
    void *Symbolic, *Numeric ;
    int res;
    res = umfpack_zi_symbolic (cols, cols, Ccolstart, Crowindx, Cdata, Cimag, &Symbolic, null, null);
    res = umfpack_zi_numeric (Ccolstart, Crowindx, Cdata, Cimag, Symbolic, &Numeric, null, null);
    umfpack_zi_free_symbolic (&Symbolic);    
    // Initialization call
    int ido = 0;
    char bmat = 'I';
    int n = rows;
    char *which = "LM";
    // How many eigenvalues to compute
    char cmach = 'E';
    double tol = dlamch_(&cmach);
    double *resid = new double[2*n];
    int ncv = 2*nev+1;
    if (ncv > n) ncv = n;
    double *v = new double[2*n*ncv];
    int ldv = n;
    int iparam[11];
    iparam[0] = 1;
    iparam[1] = 0;
    iparam[2] = 300;
    iparam[3] = 1;
    iparam[4] = 0;
    iparam[5] = 0;
    iparam[6] = 3;
    iparam[7] = 0;
    iparam[8] = 0;
    iparam[9] = 0;
    iparam[10] = 0;
    double *workd = new double[2*3*n];
    int lworkl = 3*ncv*ncv+5*ncv;
    double *workl = new double[2*lworkl];
    double *rwork = new double[ncv];
    double *xr = new double[rows];
    double *xi = new double[rows];
    double *yr = new double[rows];
    double *yi = new double[rows];
    int info = 0;
    int ipntr[14];
    while (1) {
      znaupd_(&ido, &bmat, &n , which, &nev, &tol, resid, 
	      &ncv, v, &ldv, iparam, ipntr, workd, workl, &lworkl, 
	      rwork, &info);
      if ((ido == -1) || (ido == 1)) {
	for (int i=0;i<rows;i++) {
	  yr[i] = workd[2*(ipntr[0]-1)+2*i];
	  yi[i] = workd[2*(ipntr[0]-1)+2*i+1];
	}
	memset(xr,0,sizeof(double)*rows);
	memset(xi,0,sizeof(double)*rows);
	res = umfpack_zi_solve(UMFPACK_A, Ccolstart, Crowindx, Cdata, Cimag, xr, xi, yr, yi,Numeric, null, null);
	for (int i=0;i<rows;i++) {
	  workd[2*(ipntr[1]-1)+2*i] = xr[i];
	  workd[2*(ipntr[1]-1)+2*i+1] = xi[i];
	}	
      }
      else if (ido == 2)
	memcpy( workd+2*(ipntr[1]-1), workd+2*(ipntr[0]-1), sizeof(double)*rows*2);
      else
	break;
    }
    // Free the numeric component
    umfpack_zi_free_numeric(&Numeric);
    if (info < 0)
      DNAUPARPACKError(info);
    // Compute vectors and values
    int rvec;
    if (nargout <= 1)
      rvec = 0;
    else
      rvec = 1;
    char howmny = 'A';
    int *select = new int[ncv];
	//lambda_a = 1/lambda_c + sigma
    double *d = (double*) Malloc(2*sizeof(double)*(nev+1));
    double *z;
    if (nargout <= 1)
      z = NULL;
    else
      z = (double*) Malloc(2*sizeof(double)*(n*(nev+1)));
    int ldz = n;
    double sigma[2];
    sigma[0] = shift[0];
    sigma[1] = shift[1];
    double *workev = new double[2*2*ncv];
    int ierr;
    zneupd_(&rvec, &howmny, select, d, z, &ldv, 
	    sigma, workev, &bmat, &n, which, &nev, &tol, 
	    resid, &ncv, v, &ldv, iparam, ipntr, workd, workl,
	    &lworkl, rwork, &ierr);
    int nconv = iparam[4];
    if (ierr != 0)
      DNEUPARPACKError(ierr);
    // Reverse the vectors dr and di
    if (rvec == 0) {
      for (int i=0;i<(nconv)/2;i++) {
	swap(d[2*i],d[2*(nconv-1-i)]);
	swap(d[2*i+1],d[2*(nconv-1-i)+1]);
      }
    }
    delete[] resid;
    delete[] v;
    delete[] workd;
    delete[] workl;
    delete[] select;
    delete[] workev;
    double *eigvals = (double*) Malloc(nev*sizeof(double)*2);
    for (int i=0;i<min(nev,nconv);i++) {
      eigvals[2*i] = d[2*i];
      eigvals[2*i+1] = d[2*i+1];
    }
    double *eigvecs;
    if (nargout > 1) {
      eigvecs = (double*) Malloc(nev*n*sizeof(double)*2);
      for (int i=0;i<min(nev,nconv);i++) {
	for (int j=0;j<2*n;j++) 
	  eigvecs[i*2*n+j] = d[i*2*n+j];
      }
    }
    ArrayVector retval;
    if (nargout <= 1)
      retval.push_back(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals));
    else {
      retval.push_back(Array(FM_DCOMPLEX,Dimensions(n,nev),eigvecs));
      retval.push_back(Array::diagonalConstructor(Array(FM_DCOMPLEX,Dimensions(nev,1),eigvals),0));
    }
    Free(z);
    return retval;
  } 

  ArrayVector SparseEigDecompose(int nargout, Array A, int k, char* whichFlag) {
    if (A.isComplex()) {
      return SparseEigDecomposeNonsymmetricComplex((double**) A.getSparseDataPointer(),
						   A.getDimensionLength(0),
						   A.getDimensionLength(1),
						   k, nargout, whichFlag);
    } else {
      bool symDetect = SparseIsSymmetricReal<double>(A.getDimensionLength(0),
						     A.getDimensionLength(1),
						     (const double**) A.getSparseDataPointer());
      if (symDetect)
	return SparseEigDecomposeSymmetricReal((double**) A.getSparseDataPointer(),
					       A.getDimensionLength(0),
					       A.getDimensionLength(1),
					       k, nargout, whichFlag);
      else
	return SparseEigDecomposeNonsymmetricReal((double**) A.getSparseDataPointer(),
						  A.getDimensionLength(0),
						  A.getDimensionLength(1),
						  k, nargout, whichFlag);
    }
  }

  ArrayVector SparseEigDecomposeShifted(int nargout, Array A, int k, double shift[2]) {
    if (shift[1] != 0)
      A.promoteType(FM_DCOMPLEX);
    if (A.isComplex()) {
      return SparseEigDecomposeNonsymmetricComplexShifted((double**) A.getSparseDataPointer(),
							  A.getDimensionLength(0),
							  A.getDimensionLength(1),
							  k, nargout, shift);
    } else {
      return SparseEigDecomposeNonsymmetricRealShifted((double**) A.getSparseDataPointer(),
						       A.getDimensionLength(0),
						       A.getDimensionLength(1),
						       k, nargout, shift[0]);
    }
  }
  
  Array SparsePowerFuncComplexComplex(Array A, Array B) {
    A.promoteType(FM_DCOMPLEX);
    B.promoteType(FM_DCOMPLEX);
    B.makeDense();
    int rows, cols;
    rows = A.getDimensionLength(0);
    cols = A.getDimensionLength(1);
    const double *bval = (const double *) B.getDataPointer();
    double **dp = new double*[cols];
    double *buffer = new double[4*rows];
    const double **ap = (const double **) A.getSparseDataPointer();
    for (int p=0;p<cols;p++) {
      RLEDecoderComplex<double> Adecoder(ap[p],rows);
      RLEEncoderComplex<double> Cencoder(buffer,rows);
      Adecoder.update();
      while (Adecoder.more()) {
	Cencoder.set(Adecoder.row());
	double aval[2];
	double cval[2];
	aval[0] = Adecoder.value_real();
	aval[1] = Adecoder.value_imag();
	power_zz(cval,aval,bval);
	Cencoder.push(cval[0],cval[1]);
	Adecoder.advance();
      }
      Cencoder.end();
      dp[p] = Cencoder.copyout();
    }
    delete [] buffer;
    return Array(FM_DCOMPLEX,Dimensions(rows,cols),dp,true);
  }
  
  Array SparsePowerFuncComplexInteger(Array A, Array B) {
    A.promoteType(FM_DCOMPLEX);
    B.promoteType(FM_INT32);
    B.makeDense();
    int rows, cols;
    rows = A.getDimensionLength(0);
    cols = A.getDimensionLength(1);
    int32 bval = *((const int32 *) B.getDataPointer());
    double **dp = new double*[cols];
    double *buffer = new double[4*rows];
    const double **ap = (const double **) A.getSparseDataPointer();
    for (int p=0;p<cols;p++) {
      RLEDecoderComplex<double> Adecoder(ap[p],rows);
      RLEEncoderComplex<double> Cencoder(buffer,rows);
      Adecoder.update();
      while (Adecoder.more()) {
	Cencoder.set(Adecoder.row());
	double aval[2];
	double cval[2];
	aval[0] = Adecoder.value_real();
	aval[1] = Adecoder.value_imag();
	power_zi(cval,aval,bval);
	Cencoder.push(cval[0],cval[1]);
	Adecoder.advance();
      }
      Cencoder.end();
      dp[p] = Cencoder.copyout();
    }
    delete [] buffer;
    return Array(FM_DCOMPLEX,Dimensions(rows,cols),dp,true);
  }

  Array SparsePowerFuncRealReal(Array A, Array B) {
    A.promoteType(FM_DOUBLE);
    B.promoteType(FM_DOUBLE);
    B.makeDense();
    int rows, cols;
    rows = A.getDimensionLength(0);
    cols = A.getDimensionLength(1);
    double bval = *((const double *) B.getDataPointer());
    double **dp = new double*[cols];
    double *buffer = new double[2*rows];
    const double **ap = (const double **) A.getSparseDataPointer();
    for (int p=0;p<cols;p++) {
      RLEDecoder<double> Adecoder(ap[p],rows);
      RLEEncoder<double> Cencoder(buffer,rows);
      Adecoder.update();
      while (Adecoder.more()) {
	Cencoder.set(Adecoder.row());
	Cencoder.push(power_dd(Adecoder.value(),bval));
	Adecoder.advance();
      }
      Cencoder.end();
      dp[p] = Cencoder.copyout();
    }
    delete [] buffer;
    return Array(FM_DOUBLE,Dimensions(rows,cols),dp,true);
  }
  
  Array SparsePowerFuncRealInteger(Array A, Array B) {
    A.promoteType(FM_DOUBLE);
    B.promoteType(FM_INT32);
    B.makeDense();
    int rows, cols;
    rows = A.getDimensionLength(0);
    cols = A.getDimensionLength(1);
    int32 bval = *((const int32 *) B.getDataPointer());
    double **dp = new double*[cols];
    double *buffer = new double[2*rows];
    const double **ap = (const double **) A.getSparseDataPointer();
    for (int p=0;p<cols;p++) {
      RLEDecoder<double> Adecoder(ap[p],rows);
      RLEEncoder<double> Cencoder(buffer,rows);
      Adecoder.update();
      while (Adecoder.more()) {
	Cencoder.set(Adecoder.row());
	Cencoder.push(power_di(Adecoder.value(),bval));
	Adecoder.advance();
      }
      Cencoder.end();
      dp[p] = Cencoder.copyout();
    }
    delete [] buffer;
    return Array(FM_DOUBLE,Dimensions(rows,cols),dp,true);
  }

  // Compute A^B where B is a scalar, A is sparse and B is a scalar
  // We do this the lazy way - convert both to dcomplex or complex, and
  // then check the result to see if its real (or integer)
  //
  // These are the four case to consider:
  //
  // power_zi
  // power_zz
  // power_di
  // power_dd
  //
  //
  Array SparsePowerFunc(Array A, Array B) {
    Class Aclass, Bclass;
    Aclass = A.getDataClass();
    Bclass = B.getDataClass();
    Array C;
    if (A.isReal())
      if (Bclass == FM_INT32)
	C = SparsePowerFuncRealInteger(A,B);
      else
	C = SparsePowerFuncRealReal(A,B);
    else
      if (Bclass == FM_INT32)
	C = SparsePowerFuncComplexInteger(A,B);
      else
	C = SparsePowerFuncComplexComplex(A,B);
    C.promoteType(A.getDataClass());
    return C;
  }

  template <class T>
  bool IsPositiveSparse(int rows, int cols, const T** src) {
    bool allPositive = true;
    for (int p=0;p<cols;p++) {
      RLEDecoder<T> A(src[p],rows);
      A.update();
      while (A.more()) {
	allPositive = allPositive & (A.value() >= 0);
	if (!allPositive) return false;
	A.advance();
      }
    }
    return allPositive;
  }

  // Test for positive entries (technically non-negative entries) - only applicable to 
  // real valued matrices
  bool SparseIsPositive(Class dclass, int rows, int cols, const void *ap) {
    switch(dclass) {
    case FM_INT32:
      return IsPositiveSparse<int32>(rows, cols, (const int32**) ap);
    case FM_FLOAT:
      return IsPositiveSparse<float>(rows, cols, (const float**) ap);
    case FM_DOUBLE:
      return IsPositiveSparse<double>(rows, cols, (const double**) ap);
    }
    throw Exception("unsupported type for ispositive");
  }


  template <class T>
  void* SparseMatrixSumColumnsReal(int rows, int cols, const T** A) {
    T **out = (T**) new T*[cols];
    T* buffer = new T[3];
    for (int col=0;col<cols;col++) {
      T accum = 0;
      RLEDecoder<T> Acmp(A[col], rows);
      Acmp.update();
      while (Acmp.more()) {
	accum += Acmp.value();
	Acmp.advance();
      }
      RLEEncoder<T> Bcmp(buffer,1);
      Bcmp.push(accum);
      Bcmp.end();
      out[col] = Bcmp.copyout();
    }
    delete[] buffer;
    return out;
  }

  template <class T>
  void* SparseMatrixSumColumnsComplex(int rows, int cols, const T** A) {
    T **out = (T**) new T*[cols];
    T* buffer = new T[5];
    for (int col=0;col<cols;col++) {
      T accum_real = 0;
      T accum_imag = 0;
      RLEDecoderComplex<T> Acmp(A[col], rows);
      Acmp.update();
      while (Acmp.more()) {
	accum_real += Acmp.value_real();
	accum_imag += Acmp.value_imag();
	Acmp.advance();
      }
      RLEEncoderComplex<T> Bcmp(buffer,1);
      Bcmp.push(accum_real,accum_imag);
      Bcmp.end();
      out[col] = Bcmp.copyout();
    }
    delete[] buffer;
    return out;
  }

  template <class T>
  void* SparseMatrixSumRowsReal(int rows, int cols, const T** A) {
    T** out = (T**) new T*[1];
    T* buffer = new T[rows];
    memset(buffer,0,sizeof(T)*rows);
    for (int col=0;col<cols;col++) {
      RLEDecoder<T> Acmp(A[col], rows);
      Acmp.update();
      while (Acmp.more()) {
	buffer[Acmp.row()] += Acmp.value();
	Acmp.advance();
      }
    }
    T* cbuf = new T[rows*2];
    out[0] = CompressRealVector<T>(cbuf, buffer, rows);
    delete[] cbuf;
    delete[] buffer;
    return out;
  }

  template <class T>
  void* SparseMatrixSumRowsComplex(int rows, int cols, const T** A) {
    T** out = (T**) new T*[1];
    T* buffer = new T[2*rows];
    memset(buffer,0,sizeof(T)*rows*2);
    for (int col=0;col<cols;col++) {
      RLEDecoderComplex<T> Acmp(A[col], rows);
      Acmp.update();
      while (Acmp.more()) {
	buffer[2*Acmp.row()] += Acmp.value_real();
	buffer[2*Acmp.row()+1] += Acmp.value_imag();
	Acmp.advance();
      }
    }
    T* cbuf = new T[rows*4];
    out[0] = CompressComplexVector(cbuf, buffer, rows);
    delete[] cbuf;
    delete[] buffer;
    return out;
  }

  void* SparseMatrixSumRows(Class dclass, int Arows, int Acols, const void *Ap) {
    switch(dclass) {
    case FM_INT32:
      return SparseMatrixSumRowsReal<int32>(Arows, Acols, (const int32**) Ap);
    case FM_FLOAT:
      return SparseMatrixSumRowsReal<float>(Arows, Acols, (const float**) Ap);
    case FM_DOUBLE:
      return SparseMatrixSumRowsReal<double>(Arows, Acols, (const double**) Ap);
    case FM_COMPLEX:
      return SparseMatrixSumRowsComplex<float>(Arows, Acols, (const float**) Ap);
    case FM_DCOMPLEX:
      return SparseMatrixSumRowsComplex<double>(Arows, Acols, (const double**) Ap);
    }
  }

  void* SparseMatrixSumColumns(Class dclass, int Arows, int Acols, const void *Ap) {
    switch(dclass) {
    case FM_INT32:
      return SparseMatrixSumColumnsReal<int32>(Arows, Acols, (const int32**) Ap);
    case FM_FLOAT:
      return SparseMatrixSumColumnsReal<float>(Arows, Acols, (const float**) Ap);
    case FM_DOUBLE:
      return SparseMatrixSumColumnsReal<double>(Arows, Acols, (const double**) Ap);
    case FM_COMPLEX:
      return SparseMatrixSumColumnsComplex<float>(Arows, Acols, (const float**) Ap);
    case FM_DCOMPLEX:
      return SparseMatrixSumColumnsComplex<double>(Arows, Acols, (const double**) Ap);
    }
  }
  
  template <class T>
  void* ReshapeSparseMatrixComplex(int rows, int cols, const T** A, int newrows, int newcols) {
    // Convert the sparse matrix into IJV format
    IJVEntry<T>* mlist;
    int nnz;
    mlist = ConvertSparseToIJVListComplex<T>(A,rows,cols,nnz);
    for (int i=0;i<nnz;i++) {
      int vind = mlist[i].I + mlist[i].J*rows;
      mlist[i].I = vind % newrows;
      mlist[i].J = vind / newrows;
    }
    std::sort(mlist,mlist+nnz);
    return ConvertIJVtoRLEComplex<T>(mlist,nnz,newrows,newcols);
  }


  template <class T>
  void* ReshapeSparseMatrixReal(int rows, int cols, const T** A, int newrows, int newcols) {
    // Convert the sparse matrix into IJV format
    IJVEntry<T>* mlist;
    int nnz;
    mlist = ConvertSparseToIJVListReal<T>(A,rows,cols,nnz);
    for (int i=0;i<nnz;i++) {
      int vind = mlist[i].I + mlist[i].J*rows;
      mlist[i].I = vind % newrows;
      mlist[i].J = vind / newrows;
    }
    std::sort(mlist,mlist+nnz);
    return ConvertIJVtoRLEReal<T>(mlist,nnz,newrows,newcols);
  }

  void* ReshapeSparseMatrix(Class dclass, int rows, int cols, const void *Ap,
			    int newrows, int newcols) {
    switch(dclass) {
    case FM_LOGICAL:
      return ReshapeSparseMatrixReal<uint32>(rows,cols,(const uint32**) Ap,newrows,newcols);
    case FM_INT32:
      return ReshapeSparseMatrixReal<int32>(rows,cols,(const int32**) Ap,newrows,newcols);
    case FM_FLOAT:
      return ReshapeSparseMatrixReal<float>(rows,cols,(const float**) Ap,newrows,newcols);
    case FM_DOUBLE:
      return ReshapeSparseMatrixReal<double>(rows,cols,(const double**) Ap,newrows,newcols);
    case FM_COMPLEX:
      return ReshapeSparseMatrixComplex<float>(rows,cols,(const float**) Ap,newrows,newcols);
    case FM_DCOMPLEX:
      return ReshapeSparseMatrixReal<double>(rows,cols,(const double**) Ap,newrows,newcols);
    }    
  }

  uint32* SparseLogicalToOrdinal(int rows, int cols, const void *Ap, int &nnz) {
    nnz = CountNonzerosReal((const uint32 **) Ap,rows,cols);
    uint32* retptr = (uint32*) Malloc(sizeof(uint32)*nnz);
    const uint32 **src = (const uint32**) Ap;
    int k = 0;
    for (int i=0;i<cols;i++) {
      RLEDecoder<uint32> A(src[i],rows);
      A.update();
      while (A.more()) {
	retptr[k++] = A.row()+i*rows+1;
	A.advance();
      }
    }
    return retptr;
  }

  template <class T>
  uint32 slo_and_real(T a, T b) {
    return (a && b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_or_real(T a, T b) {
    return (a || b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_lt_real(T a, T b) {
    return (a < b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_gt_real(T a, T b) {
    return (a > b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_ne_real(T a, T b) {
    return (a != b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_le_real(T a, T b) {
    return (a <= b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_ge_real(T a, T b) {
    return (a >= b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_eq_real(T a, T b) {
    return (a == b) ? 1 : 0;
  }

  template <class T>
  uint32 slo_lt_complex(T ar, T ai, T br, T bi) {
    return (complex_abs<T>(ar,ai) < complex_abs<T>(br,bi)) ? 1 : 0;
  }

  template <class T>
  uint32 slo_gt_complex(T ar, T ai, T br, T bi) {
    return (complex_abs<T>(ar,ai) > complex_abs<T>(br,bi)) ? 1 : 0;
  }

  template <class T>
  uint32 slo_ne_complex(T ar, T ai, T br, T bi) {
    return ((ar != br) || (ai != bi)) ? 1 : 0;
  }

  template <class T>
  uint32 slo_le_complex(T ar, T ai, T br, T bi) {
    return (complex_abs<T>(ar,ai) <= complex_abs<T>(br,bi)) ? 1 : 0;
  }

  template <class T>
  uint32 slo_ge_complex(T ar, T ai, T br, T bi) {
    return (complex_abs<T>(ar,ai) >= complex_abs<T>(br,bi)) ? 1 : 0;
  }

  template <class T>
  uint32 slo_eq_complex(T ar, T ai, T br, T bi) {
    return ((ar == br) && (ai == bi)) ? 1 : 0;
  }

  template <class T>
  uint32** ApplyLogicalOpComplexZP(int rows, int cols, 
				    const T** Asrc, const T** Bsrc, 
				    uint32 (*fnop)(T,T,T,T)) {
    uint32** Cmat = new uint32*[cols];
    uint32* buffer = new uint32[rows*2];
    memset(buffer,0,sizeof(uint32)*rows*2);
    for (int col=0;col<cols;col++) {
      RLEDecoderComplex<T> A(Asrc[col],rows);
      RLEDecoderComplex<T> B(Bsrc[col],rows);
      RLEEncoder<uint32> C(buffer,rows);
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(fnop(A.value_real(),A.value_imag(),
		      B.value_real(),B.value_imag()));
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  C.set(A.row());
	  C.push(fnop(A.value_real(),A.value_imag(),0,0));
	  A.advance();
	} else {
	  C.set(B.row());
	  C.push(fnop(0,0,B.value_real(),B.value_imag()));
	  B.advance();
	}
      }
      C.end();
      Cmat[col] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  template <class T>
  uint32** ApplyLogicalOpRealZP(int rows, int cols, 
				 const T** Asrc, const T** Bsrc, 
				 uint32 (*fnop)(T,T)) {
    uint32** Cmat = new uint32*[cols];
    uint32* buffer = new uint32[rows*2];
    memset(buffer,0,sizeof(uint32)*rows*2);
    for (int col=0;col<cols;col++) {
      RLEDecoder<T> A(Asrc[col],rows);
      RLEDecoder<T> B(Bsrc[col],rows);
      RLEEncoder<uint32> C(buffer,rows);
      A.update();
      B.update();
      while (A.more() || B.more()) {
	if (A.row() == B.row()) {
	  C.set(A.row());
	  C.push(fnop(A.value(),B.value()));
	  A.advance();
	  B.advance();
	} else if (A.row() < B.row()) {
	  C.set(A.row());
	  C.push(fnop(A.value(),0));
	  A.advance();
	} else {
	  C.set(B.row());
	  C.push(fnop(0,B.value()));
	  B.advance();
	}
      }
      C.end();
      Cmat[col] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  // Non-zero preserving
  template <class T>
  uint32** ApplyLogicalOpRealNZ(int rows, int cols, 
				 const T** Asrc, const T** Bsrc, 
				 uint32 (*fnop)(T,T)) {
    uint32** Cmat = new uint32*[cols];
    T* Abuf = new T[rows];
    T* Bbuf = new T[rows];
    uint32* Cbuf = new uint32[rows];
    uint32* buffer = new uint32[rows*2];
    for (int col=0;col<cols;col++) {
      memset(Abuf,0,rows*sizeof(T));
      memset(Bbuf,0,rows*sizeof(T));
      DecompressRealString<T>(Asrc[col],Abuf,rows);
      DecompressRealString<T>(Bsrc[col],Bbuf,rows);
      for (int row=0;row<rows;row++)
	Cbuf[row] = fnop(Abuf[row],Bbuf[row]);
      Cmat[col] = CompressRealVector<uint32>(buffer,Cbuf,rows);
    }
    delete buffer;
    delete Abuf;
    delete Bbuf;
    return Cmat;
  }

  // Non-zero preserving
  template <class T>
  uint32** ApplyLogicalOpComplexNZ(int rows, int cols, 
				   const T** Asrc, const T** Bsrc, 
				   uint32 (*fnop)(T,T,T,T)) {
    uint32** Cmat = new uint32*[cols];
    T* Abuf = new T[2*rows];
    T* Bbuf = new T[2*rows];
    uint32* Cbuf = new uint32[rows];
    uint32* buffer = new uint32[rows*2];
    for (int col=0;col<cols;col++) {
      memset(Abuf,0,2*rows*sizeof(T));
      memset(Bbuf,0,2*rows*sizeof(T));
      DecompressComplexString<T>(Asrc[col],Abuf,rows);
      DecompressComplexString<T>(Bsrc[col],Bbuf,rows);
      for (int row=0;row<rows;row++)
	Cbuf[row] = fnop(Abuf[2*row],
			 Abuf[2*row+1],
			 Bbuf[2*row],
			 Bbuf[2*row+1]);
      Cmat[col] = CompressRealVector<uint32>(buffer,Cbuf,rows);
    }
    delete buffer;
    delete Abuf;
    delete Bbuf;
    return Cmat;
  }
  template <class T>
  uint32** ApplyLogicalOpComplexScalarZP(int rows, int cols, 
					 const T** Asrc, const T* Bsrc, 
					 uint32 (*fnop)(T,T,T,T)) {
    uint32** Cmat = new uint32*[cols];
    uint32* buffer = new uint32[rows*2];
    memset(buffer,0,sizeof(uint32)*rows*2);
    for (int col=0;col<cols;col++) {
      RLEDecoderComplex<T> A(Asrc[col],rows);
      RLEEncoder<uint32> C(buffer,rows);
      A.update();
      while (A.more()) {
	C.set(A.row());
	C.push(fnop(A.value_real(),A.value_imag(),
		    Bsrc[0],Bsrc[1]));
	A.advance();
      }
      C.end();
      Cmat[col] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  template <class T>
  uint32** ApplyLogicalOpRealScalarZP(int rows, int cols, 
				      const T** Asrc, const T* Bsrc, 
				      uint32 (*fnop)(T,T)) {
    uint32** Cmat = new uint32*[cols];
    uint32* buffer = new uint32[rows*2];
    memset(buffer,0,sizeof(uint32)*rows*2);
    for (int col=0;col<cols;col++) {
      RLEDecoder<T> A(Asrc[col],rows);
      RLEEncoder<uint32> C(buffer,rows);
      A.update();
      while (A.more()) {
	C.set(A.row());
	C.push(fnop(A.value(),Bsrc[0]));
	A.advance();
      }
      C.end();
      Cmat[col] = C.copyout();
    }
    delete buffer;
    return Cmat;
  }

  // Non-zero preserving
  template <class T>
  uint32** ApplyLogicalOpRealScalarNZ(int rows, int cols, 
				      const T** Asrc, const T* Bsrc, 
				      uint32 (*fnop)(T,T)) {
    uint32** Cmat = new uint32*[cols];
    T* Abuf = new T[rows];
    uint32* Cbuf = new uint32[rows];
    uint32* buffer = new uint32[rows*2];
    for (int col=0;col<cols;col++) {
      memset(Abuf,0,rows*sizeof(T));
      DecompressRealString<T>(Asrc[col],Abuf,rows);
      for (int row=0;row<rows;row++)
	Cbuf[row] = fnop(Abuf[row],Bsrc[0]);
      Cmat[col] = CompressRealVector<uint32>(buffer,Cbuf,rows);
    }
    delete buffer;
    delete Abuf;
    return Cmat;
  }
  
  // Non-zero preserving
  template <class T>
  uint32** ApplyLogicalOpComplexScalarNZ(int rows, int cols, 
					 const T** Asrc, const T* Bsrc, 
					 uint32 (*fnop)(T,T,T,T)) {
    uint32** Cmat = new uint32*[cols];
    T* Abuf = new T[2*rows];
    uint32* Cbuf = new uint32[rows];
    uint32* buffer = new uint32[rows*2];
    for (int col=0;col<cols;col++) {
      memset(Abuf,0,2*rows*sizeof(T));
      DecompressComplexString<T>(Asrc[col],Abuf,rows);
      for (int row=0;row<rows;row++)
	Cbuf[row] = fnop(Abuf[2*row],
			 Abuf[2*row+1],
			 Bsrc[0],
			 Bsrc[1]);
      Cmat[col] = CompressRealVector<uint32>(buffer,Cbuf,rows);
    }
    delete buffer;
    delete Abuf;
    return Cmat;
  }
  
  template <class T>
  uint32** ApplyLogicalOpRealScalar(int rows, int cols,
				  const T** Asrc,
				  const T* Bsrc,
				  uint32 (*fnop)(T,T)) {
    // Is fnop zero preserving?
    if (!fnop(0,Bsrc[0])) 
      return ApplyLogicalOpRealScalarZP<T>(rows,cols,Asrc,Bsrc,fnop);
    else 
      return ApplyLogicalOpRealScalarNZ<T>(rows,cols,Asrc,Bsrc,fnop);
  }

  template <class T>
  uint32** ApplyLogicalOpComplexScalar(int rows, int cols,
				       const T** Asrc,
				       const T* Bsrc,
				       uint32 (*fnop)(T,T,T,T)) {
    // Is fnop zero preserving?
    if (!fnop(0,0,Bsrc[0],Bsrc[1])) 
      return ApplyLogicalOpComplexScalarZP<T>(rows,cols,Asrc,Bsrc,fnop);
    else 
      return ApplyLogicalOpComplexScalarNZ<T>(rows,cols,Asrc,Bsrc,fnop);
  }

  // Apply a logical op to two sparse matrices of the same size.  
  // The algo to handle the operation depends on wheter or not
  // the op is zero preserving.  For sparse-sparse interactions,
  // the ops stack up like this:
  //  SLO_LT,  0 < 0 = 0   -> Zero Preserving
  //  SLO_GT,  0 > 0 = 0   -> Zero Preserving
  //  SLO_NE,  0 ~= 0 = 0  -> Zero Preserving
  //  SLO_AND, 0 && 0 = 0  -> Zero Preserving
  //  SLO_OR   0 || 0 = 0  -> Zero Preserving
  //  SLO_LE,  0 <= 0 = 1  -> not zero preserving
  //  SLO_GE,  0 >= 0 = 1  -> not zero preserving
  //  SLO_EQ,  0 == 0 = 1  -> not zero preserving
  //  SLO_NOT, ~0 = 1      -> not zero preserving
  // For an action that is _not_ zero preserving, we shouldn't be here - just
  // convert both arguments to full matrices and use the regular routines
  void* SparseSparseLogicalOp(Class dclass, int rows, int cols, 
			      const void *Ap, const void *Bp, 
			      SparseLogOpID opselect) {
    switch (dclass) {
    case FM_LOGICAL:
      switch (opselect) {
      case SLO_AND: return ApplyLogicalOpRealZP<uint32>(rows,cols,(const uint32**)Ap,(const uint32**)Bp,slo_and_real<uint32>);
      case SLO_OR: return ApplyLogicalOpRealZP<uint32>(rows,cols,(const uint32**)Ap,(const uint32**)Bp,slo_or_real<uint32>);
	throw Exception("unsupported sparse type/op combination");
      }
    case FM_INT32:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpRealZP<int32>(rows,cols,(const int32**)Ap,(const int32**)Bp,slo_lt_real<int32>);
      case SLO_GT: return ApplyLogicalOpRealZP<int32>(rows,cols,(const int32**)Ap,(const int32**)Bp,slo_gt_real<int32>);
      case SLO_NE: return ApplyLogicalOpRealZP<int32>(rows,cols,(const int32**)Ap,(const int32**)Bp,slo_ne_real<int32>);
      case SLO_LE: return ApplyLogicalOpRealNZ<int32>(rows,cols,(const int32**)Ap,(const int32**)Bp,slo_le_real<int32>);
      case SLO_GE: return ApplyLogicalOpRealNZ<int32>(rows,cols,(const int32**)Ap,(const int32**)Bp,slo_ge_real<int32>);
      case SLO_EQ: return ApplyLogicalOpRealNZ<int32>(rows,cols,(const int32**)Ap,(const int32**)Bp,slo_eq_real<int32>);
	throw Exception("unsupported sparse type/op combination");
      }
    case FM_FLOAT:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpRealZP<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_lt_real<float>);
      case SLO_GT: return ApplyLogicalOpRealZP<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_gt_real<float>);
      case SLO_NE: return ApplyLogicalOpRealZP<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_ne_real<float>);
      case SLO_LE: return ApplyLogicalOpRealNZ<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_le_real<float>);
      case SLO_GE: return ApplyLogicalOpRealNZ<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_ge_real<float>);
      case SLO_EQ: return ApplyLogicalOpRealNZ<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_eq_real<float>);
	throw Exception("unsupported sparse type/op combination");
      }	
    case FM_DOUBLE:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpRealZP<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_lt_real<double>);
      case SLO_GT: return ApplyLogicalOpRealZP<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_gt_real<double>);
      case SLO_NE: return ApplyLogicalOpRealZP<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_ne_real<double>);
      case SLO_LE: return ApplyLogicalOpRealNZ<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_le_real<double>);
      case SLO_GE: return ApplyLogicalOpRealNZ<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_ge_real<double>);
      case SLO_EQ: return ApplyLogicalOpRealNZ<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_eq_real<double>);
	throw Exception("unsupported sparse type/op combination");
      }	
    case FM_COMPLEX:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpComplexZP<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_lt_complex<float>);
      case SLO_GT: return ApplyLogicalOpComplexZP<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_gt_complex<float>);
      case SLO_NE: return ApplyLogicalOpComplexZP<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_ne_complex<float>);
      case SLO_LE: return ApplyLogicalOpComplexNZ<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_le_complex<float>);
      case SLO_GE: return ApplyLogicalOpComplexNZ<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_ge_complex<float>);
      case SLO_EQ: return ApplyLogicalOpComplexNZ<float>(rows,cols,(const float**)Ap,(const float**)Bp,slo_eq_complex<float>);
	throw Exception("unsupported sparse type/op combination");
      }	
    case FM_DCOMPLEX:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpComplexZP<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_lt_complex<double>);
      case SLO_GT: return ApplyLogicalOpComplexZP<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_gt_complex<double>);
      case SLO_NE: return ApplyLogicalOpComplexZP<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_ne_complex<double>);
      case SLO_LE: return ApplyLogicalOpComplexNZ<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_le_complex<double>);
      case SLO_GE: return ApplyLogicalOpComplexNZ<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_ge_complex<double>);
      case SLO_EQ: return ApplyLogicalOpComplexNZ<double>(rows,cols,(const double**)Ap,(const double**)Bp,slo_eq_complex<double>);
	throw Exception("unsupported sparse type/op combination");
      }	
    }
  }

  void* SparseScalarLogicalOp(Class dclass, int rows, int cols, 
			      const void *Ap, const void *Bp, 
			      SparseLogOpID opselect) {
    switch (dclass) {
    case FM_LOGICAL:
      switch (opselect) {
      case SLO_AND: return ApplyLogicalOpRealScalar<uint32>(rows,cols,(const uint32**)Ap,(const uint32*)Bp,slo_and_real<uint32>);
      case SLO_OR: return ApplyLogicalOpRealScalar<uint32>(rows,cols,(const uint32**)Ap,(const uint32*)Bp,slo_or_real<uint32>);
	throw Exception("unsupported sparse type/op combination");
      }
    case FM_INT32:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpRealScalar<int32>(rows,cols,(const int32**)Ap,(const int32*)Bp,slo_lt_real<int32>);
      case SLO_GT: return ApplyLogicalOpRealScalar<int32>(rows,cols,(const int32**)Ap,(const int32*)Bp,slo_gt_real<int32>);
      case SLO_NE: return ApplyLogicalOpRealScalar<int32>(rows,cols,(const int32**)Ap,(const int32*)Bp,slo_ne_real<int32>);
      case SLO_LE: return ApplyLogicalOpRealScalar<int32>(rows,cols,(const int32**)Ap,(const int32*)Bp,slo_le_real<int32>);
      case SLO_GE: return ApplyLogicalOpRealScalar<int32>(rows,cols,(const int32**)Ap,(const int32*)Bp,slo_ge_real<int32>);
      case SLO_EQ: return ApplyLogicalOpRealScalar<int32>(rows,cols,(const int32**)Ap,(const int32*)Bp,slo_eq_real<int32>);
	throw Exception("unsupported sparse type/op combination");
      }	
    case FM_FLOAT:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpRealScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_lt_real<float>);
      case SLO_GT: return ApplyLogicalOpRealScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_gt_real<float>);
      case SLO_NE: return ApplyLogicalOpRealScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_ne_real<float>);
      case SLO_LE: return ApplyLogicalOpRealScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_le_real<float>);
      case SLO_GE: return ApplyLogicalOpRealScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_ge_real<float>);
      case SLO_EQ: return ApplyLogicalOpRealScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_eq_real<float>);
	throw Exception("unsupported sparse type/op combination");
      }	
    case FM_DOUBLE:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpRealScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_lt_real<double>);
      case SLO_GT: return ApplyLogicalOpRealScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_gt_real<double>);
      case SLO_NE: return ApplyLogicalOpRealScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_ne_real<double>);
      case SLO_LE: return ApplyLogicalOpRealScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_le_real<double>);
      case SLO_GE: return ApplyLogicalOpRealScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_ge_real<double>);
      case SLO_EQ: return ApplyLogicalOpRealScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_eq_real<double>);
	throw Exception("unsupported sparse type/op combination");
      }	
    case FM_COMPLEX:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpComplexScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_lt_complex<float>);
      case SLO_GT: return ApplyLogicalOpComplexScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_gt_complex<float>);
      case SLO_NE: return ApplyLogicalOpComplexScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_ne_complex<float>);
      case SLO_LE: return ApplyLogicalOpComplexScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_le_complex<float>);
      case SLO_GE: return ApplyLogicalOpComplexScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_ge_complex<float>);
      case SLO_EQ: return ApplyLogicalOpComplexScalar<float>(rows,cols,(const float**)Ap,(const float*)Bp,slo_eq_complex<float>);
	throw Exception("unsupported sparse type/op combination");
      }	
    case FM_DCOMPLEX:
      switch (opselect) {
      case SLO_LT: return ApplyLogicalOpComplexScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_lt_complex<double>);
      case SLO_GT: return ApplyLogicalOpComplexScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_gt_complex<double>);
      case SLO_NE: return ApplyLogicalOpComplexScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_ne_complex<double>);
      case SLO_LE: return ApplyLogicalOpComplexScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_le_complex<double>);
      case SLO_GE: return ApplyLogicalOpComplexScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_ge_complex<double>);
      case SLO_EQ: return ApplyLogicalOpComplexScalar<double>(rows,cols,(const double**)Ap,(const double*)Bp,slo_eq_complex<double>);
	throw Exception("unsupported sparse type/op combination");
      }	
    }
  }
  
  template <class T>
  T** SparseAbsFunctionReal(int rows, int cols, const T**src) {
    T** dp;
    dp = new T*[cols];
    T* buffer = new T[rows*2];
    for (int p=0;p<cols;p++) {
      RLEEncoder<T> B(buffer,rows);
      RLEDecoder<T> A(src[p],rows);
      A.update(); 
      while (A.more()) {
	B.set(A.row());
	B.push((T)(fabs((double)A.value())));
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  template <class T>
  T** SparseAbsFunctionComplex(int rows, int cols, const T**src) {
    T** dp;
    dp = new T*[cols];
    T* buffer = new T[rows*2];
    for (int p=0;p<cols;p++) {
      RLEEncoder<T> B(buffer,rows);
      RLEDecoderComplex<T> A(src[p],rows);
      A.update(); 
      while (A.more()) {
	B.set(A.row());
	B.push(complex_abs<T>(A.value_real(),A.value_imag()));
	A.advance();
      }
      B.end();
      dp[p] = B.copyout();
    }
    delete[] buffer;
    return dp;
  }

  void* SparseAbsFunction(Class dclass, int rows, int cols, const void *Ap) {
    switch(dclass) {
    case FM_INT32:  return SparseAbsFunctionReal<int32>(rows,cols,(const int32**)Ap);
    case FM_FLOAT:  return SparseAbsFunctionReal<float>(rows,cols,(const float**)Ap);
    case FM_DOUBLE:  return SparseAbsFunctionReal<double>(rows,cols,(const double**)Ap);
    case FM_COMPLEX:  return SparseAbsFunctionComplex<float>(rows,cols,(const float**)Ap);
    case FM_DCOMPLEX:  return SparseAbsFunctionComplex<double>(rows,cols,(const double**)Ap);
    }
    throw Exception("unsupported sparse matrix type in argument to abs");
  }
}
