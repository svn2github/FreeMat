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

#include "Exception.hpp"
#include "Array.hpp"
#include "Operators.hpp"
#include <math.h>
#if HAVE_FFTW | HAVE_FFTWF
#include "fftw3.h"
#endif

#if HAVE_FFTWF
class WrapFFTWF
{
  fftwf_complex *inf, *outf;
  fftwf_plan pf_forward, pf_backward;
  int N;
public:
  WrapFFTWF(int Narg)
  {
    inf = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*Narg);
    outf = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*Narg);
    pf_forward = fftwf_plan_dft_1d(Narg,inf,outf,FFTW_FORWARD,FFTW_ESTIMATE);
    pf_backward = fftwf_plan_dft_1d(Narg,inf,outf,FFTW_BACKWARD,FFTW_ESTIMATE);
    N = Narg;
  }
  ~WrapFFTWF()
  {
    fftwf_destroy_plan(pf_forward);
    fftwf_destroy_plan(pf_backward);
    fftwf_free(inf);
    fftwf_free(outf);
  }
  void Forward(float *dp)
  {
    memcpy(inf,dp,sizeof(float)*N*2);
    fftwf_execute(pf_forward);
    memcpy(dp,outf,sizeof(float)*N*2);
  }
  void Backward(float *dp)
  {
    memcpy(inf,dp,sizeof(float)*N*2);
    fftwf_execute(pf_backward);
    memcpy(dp,outf,sizeof(float)*N*2);
    for (int i=0;i<(2*N);i++)
      dp[i] /= ((float) N);
  }
};
#endif

#if HAVE_FFTW
class WrapFFTW
{
  fftw_complex *inf, *outf;
  fftw_plan pf_forward, pf_backward;
  int N;
public:
  WrapFFTW(int Narg)
  {
    inf = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*Narg);
    outf = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*Narg);
    pf_forward = fftw_plan_dft_1d(Narg,inf,outf,FFTW_FORWARD,FFTW_ESTIMATE);
    pf_backward = fftw_plan_dft_1d(Narg,inf,outf,FFTW_BACKWARD,FFTW_ESTIMATE);
    N = Narg;
  }
  ~WrapFFTW()
  {
    fftw_destroy_plan(pf_forward);
    fftw_destroy_plan(pf_backward);
    fftw_free(inf);
    fftw_free(outf);
  }
  void Forward(double *dp)
  {
    memcpy(inf,dp,sizeof(double)*N*2);
    fftw_execute(pf_forward);
    memcpy(dp,outf,sizeof(double)*N*2);
  }
  void Backward(double *dp)
  {
    memcpy(inf,dp,sizeof(double)*N*2);
    fftw_execute(pf_backward);
    memcpy(dp,outf,sizeof(double)*N*2);
    for (int i=0;i<(2*N);i++)
      dp[i] /= ((double) N);
  }
};
#endif


class OpVecFFT {
  WrapFFTWF fft_float;
  WrapFFTW  fft_double;
public:
  OpVecFFT(int N) : fft_float(N), fft_double(N) {}
  template <typename T>
  inline void func(const ConstSparseIterator<T> & src, SparseSlice<T>& dest) {
    throw Exception("fft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const ConstComplexSparseIterator<T> & src, 
		   SparseSlice<T>& dest_real,
		   SparseSlice<T>& dest_imag) {
    throw Exception("fft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const BasicArray<T> & src, BasicArray<T>& dest) {
    throw Exception("fft not defined for real arrays");
  }
  // We want to take a length N FFT.  N is the length of the output
  // P is the length of the input.
  //
  //  if P>N, we take only the first N values from the input, into a 
  //     buffer of size N
  //  if P<N, we take all P values from the input into a buffer of size
  //     N
  inline void func(const BasicArray<float> & src_real,
		   const BasicArray<float> & src_imag,
		   BasicArray<float>& dest_real,
		   BasicArray<float>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<float> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_float.Forward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }  
  inline void func(const BasicArray<double> & src_real,
		   const BasicArray<double> & src_imag,
		   BasicArray<double>& dest_real,
		   BasicArray<double>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<double> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_double.Forward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }  
};

class OpVecIFFT {
  WrapFFTWF fft_float;
  WrapFFTW fft_double;
public:
  OpVecIFFT(int N) : fft_float(N), fft_double(N) {}
  template <typename T>
  inline void func(const ConstSparseIterator<T> & src, SparseSlice<T>& dest) {
    throw Exception("ifft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    throw Exception("ifft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const BasicArray<T> & src, BasicArray<T>& dest) {
    throw Exception("ifft not defined for real arrays");
  }
  inline void func(const BasicArray<float> & src_real,
			  const BasicArray<float> & src_imag,
			  BasicArray<float>& dest_real,
			  BasicArray<float>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<float> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_float.Backward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }
  inline void func(const BasicArray<double> & src_real,
			  const BasicArray<double> & src_imag,
			  BasicArray<double>& dest_real,
			  BasicArray<double>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<double> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_double.Backward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }
};

//!
//@Module FFT (Inverse) Fast Fourier Transform Function
//@@Section TRANSFORMS
//@@Usage
//Computes the Discrete Fourier Transform (DFT) of a vector using the
//Fast Fourier Transform technique.  The general syntax for its use is
//@[
//  y = fft(x,n,d)
//@]
//where @|x| is an @|n|-dimensional array of numerical type.
//Integer types are promoted to the @|double| type prior to 
//calculation of the DFT. The argument @|n| is the length of the
//FFT, and @|d| is the dimension along which to take the DFT.  If
//|n| is larger than the length of @|x| along dimension @|d|,
//then @|x| is zero-padded (by appending zeros) prior to calculation
//of the DFT.  If @|n| is smaller than the length of @|x|  along
//the given dimension, then @|x| is truncated (by removing elements
//at the end) to length @|n|.  
//
//If @|d| is omitted, then the DFT is taken along the first 
//non-singleton dimension of @|x|.  If @|n| is omitted, then
//the DFT length is chosen to match of the length of @|x| along
//dimension @|d|.  
//
//Note that FFT support on Linux builds requires availability
//of the FFTW libraries at compile time.  On Windows and Mac OS
//X, single and double precision FFTs are available by default.
//@@Function Internals
//The output is computed via
//\[
//y(m_1,\ldots,m_{d-1},l,m_{d+1},\ldots,m_{p}) = 
//\sum_{k=1}^{n} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
//e^{-\frac{2\pi(k-1)l}{n}}.
//\]
//
//For the inverse DFT, the calculation is similar, and the arguments
//have the same meanings as the DFT:
//\[
//y(m_1,\ldots,m_{d-1},l,m_{d+1},\ldots,m_{p}) = 
//\frac{1}{n} \sum_{k=1}^{n} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
//e^{\frac{2\pi(k-1)l}{n}}.
//\]
//The FFT is computed using the FFTPack library, available from 
//netlib at @|http://www.netlib.org|.  Generally speaking, the 
//computational cost for a FFT is (in worst case) @|O(n^2)|.
//However, if @|n| is composite, and can be factored as
//\[
//n = \prod_{k=1}^{p} m_k,
//\]
//then the DFT can be computed in 
//\[
//O(n \sum_{k=1}^{p} m_k)
//\]
//operations.  If @|n| is a power of 2, then the FFT can be
//calculated in @|O(n log_2 n)|.  The calculations for the
//inverse FFT are identical.
//@@Example
//The following piece of code plots the FFT for a sinusoidal signal:
//@<
//t = linspace(0,2*pi,128);
//x = cos(15*t);
//y = fft(x);
//plot(t,abs(y));
//mprint('fft1');
//@>
//The resulting plot is:
//@figure fft1
//
//The FFT can also be taken along different dimensions, and with padding 
//and/or truncation.  The following example demonstrates the Fourier
//Transform being computed along each column, and then along each row.
//@<
//A = [2,5;3,6]
//real(fft(A,[],1))
//real(fft(A,[],2))
//@>
//Fourier transforms can also be padded using the @|n| argument.  This
//pads the signal with zeros prior to taking the Fourier transform.  Zero
//padding in the time domain results in frequency interpolation.  The
//following example demonstrates the FFT of a pulse (consisting of 10 ones)
//with (red line) and without (green circles) padding.
//@<
//delta(1:10) = 1;
//plot((0:255)/256*pi*2,real(fft(delta,256)),'r-');
//hold on
//plot((0:9)/10*pi*2,real(fft(delta)),'go');
//mprint('fft2');
//@>
//The resulting plot is:
//@figure fft2
//@@Tests
//@{ test_parallel_fft1.m
//% Test FFT running in parallel threads
//function test_val = test_parallel_fft1
//a = rand(1000,1000);
//b = rand(500,1000);
//id1 = threadnew;
//id2 = threadnew;
//threadstart(id1,'fft',1,a);
//threadstart(id2,'fft',1,b);
//A = threadvalue(id1);
//B = threadvalue(id2);
//threadfree(id1);
//threadfree(id2);
//test_val = issame(A,fft(a)) && issame(B,fft(b));
//@}
//@$near#y1=fft(x1)
//@$near#y1=fft(x1,2)
//@$near#y1=fft(x1,3)
//@$near#y1=ifft(x1)
//@$near#y1=ifft(x1,2)
//@$near#y1=ifft(x1,3)
//@@Signature
//function fft FFTFunction
//inputs x len dim
//outputs y
//@@Signature
//function ifft IFFTFunction
//inputs x len dim
//outputs y
//!
ArrayVector FFTFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("FFT requires at least one argument");
  // Get the length of the zero pad
  int FFTLength = -1;
  if ((arg.size() > 1) && (!arg[1].isEmpty())) {
    FFTLength = arg[1].asInteger();
    if (FFTLength <= 0)
      throw Exception("Length argument to FFT should be positive");
  }
  // Get the dimension to FFT along.
  int FFTDim;
  if (arg.size() > 2) {
    FFTDim = arg[2].asInteger() - 1;
    if (FFTDim < 0)
      throw Exception("Dimension argument to FFT should be positive");
  } else {
    if (arg[0].isScalar())
      FFTDim = 1;
    else
      FFTDim = arg[0].dimensions().firstNonsingular();
  }
  if (FFTLength == -1)
    FFTLength = int(arg[0].dimensions()[FFTDim]);
  Array arg0(arg[0]);
  if (arg0.dataClass() != Float)
    arg0 = arg0.toClass(Double);
  if (arg0.dimensions() == NTuple(0,0))
    return arg0;
  arg0.forceComplex();
  OpVecFFT op(FFTLength);
  return ArrayVector(VectorOpDynamic<OpVecFFT>(arg0,FFTLength,FFTDim,op));
}

ArrayVector IFFTFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("IFFT requires at least one argument");
  // Get the length of the zero pad
  int FFTLength = -1;
  if ((arg.size() > 1) && (!arg[1].isEmpty())) {
    FFTLength = arg[1].asInteger();
    if (FFTLength <= 0)
      throw Exception("Length argument to IFFT should be positive");
  }
  // Get the dimension to FFT along.
  int FFTDim;
  if (arg.size() > 2) {
    FFTDim = arg[2].asInteger() - 1;
    if (FFTDim < 0)
      throw Exception("Dimension argument to IFFT should be positive");
  } else {
    if (arg[0].isScalar())
      FFTDim = 1;
    else
      FFTDim = arg[0].dimensions().firstNonsingular();
  }
  if (FFTLength == -1)
    FFTLength = int(arg[0].dimensions()[FFTDim]);
  Array arg0(arg[0]);
  if (arg0.dataClass() != Float)
    arg0 = arg0.toClass(Double);
  if (arg0.dimensions() == NTuple(0,0))
    return arg0;
  arg0.forceComplex();
  OpVecIFFT op(FFTLength);
  return ArrayVector(VectorOpDynamic<OpVecIFFT>(arg0,FFTLength,FFTDim,op));
}
