// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include <math.h>

namespace FreeMat {

#ifdef WIN32
  extern "C" {
    int cffti_(int*,float*);
    int cfftf_(int*,float*,float*);
    int cfftb_(int*,float*,float*);
    int zffti_(int*,double*);
    int zfftf_(int*,double*,double*);
    int zfftb_(int*,double*,double*);
  };
#else
  extern "C" {
    void cffti_(int*,float*);
    void cfftf_(int*,float*,float*);
    void cfftb_(int*,float*,float*);
    void zffti_(int*,double*);
    void zfftf_(int*,double*,double*);
    void zfftb_(int*,double*,double*);
  };
#endif

  static float *cwp = NULL;
  static int cN = 0;
  static double *zwp = NULL;
  static int zN = 0;

  void complex_fft_init(int Narg) {
    int i;

    if (cN == Narg) return;
    cN = Narg;
    if (cwp) free(cwp);
    cwp = (float*) malloc(sizeof(float)*(4*cN+15));
    memset(cwp,0,sizeof(float)*(4*cN+15));
    cffti_(&cN,cwp);
  }

  void complex_fft_forward(int Narg, float *dp) {
    if (cN != Narg) complex_fft_init(Narg);
    cfftf_(&cN,dp,cwp);
  }

  void complex_fft_backward(int Narg, float *dp) {
    int i;

    if (cN != Narg) complex_fft_init(Narg);
    cfftb_(&cN,dp,cwp);
    for (i=0;i<(2*cN);i++)
      dp[i] /= ((float) Narg);
  }

  void dcomplex_fft_init(int Narg) {
    int i;

    if (zN == Narg) return;
    zN = Narg;
    if (zwp) free(zwp);
    zwp = (double*) malloc(sizeof(double)*(4*zN+15));
    memset(zwp,0,sizeof(double)*(4*zN+15));
    zffti_(&zN,zwp);
  }

  void dcomplex_fft_forward(int Narg, double *dp) {
    if (zN != Narg) dcomplex_fft_init(Narg);
    zfftf_(&zN,dp,zwp);
  }

  void dcomplex_fft_backward(int Narg, double *dp) {
    int i;

    if (zN != Narg) dcomplex_fft_init(Narg);
    zfftb_(&zN,dp,zwp);
    for (i=0;i<(2*zN);i++)
      dp[i] /= ((double) Narg);
  }

  Array complexFFTFunction(const Array& input, int FFTLen, int FFTDim,
			   bool inverse) {
    Dimensions inDim(input.getDimensions());
    // Allocate the output vector...
    Dimensions outDim(inDim);
    outDim[FFTDim] = FFTLen;
    
    // Calculate the stride...
    int d;
    int workcount;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[FFTDim];
    planesize = 1;
    for (d=0;d<FFTDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=FFTDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    
    // Allocate the buffer for the FFT
    float *buffer;
    buffer = (float*) Malloc(sizeof(float)*FFTLen*2);
    // Allocate the output vector...
    float *ob;
    ob = (float*) Malloc(sizeof(float)*2*outDim.getElementCount());
    int copyIn;
    if (inDim[FFTDim] < FFTLen)
      copyIn = inDim[FFTDim];
    else
      copyIn = FFTLen;
    // Get the data pointer
    float *dp;
    dp = (float*) input.getDataPointer();
    // Do the ffts...d
    for (int i=0;i<planecount;i++) {
      for (int j=0;j<planesize;j++) {
	// Reset the buffer
	memset(buffer,0,sizeof(float)*FFTLen*2);
	// Copy the data
	int k;
	for (k=0;k<copyIn;k++) {
	  buffer[2*k] = dp[2*(i*planesize*linesize + j + k*planesize)];
	  buffer[2*k+1] = dp[2*(i*planesize*linesize + j + k*planesize)+1];
	}
	// Take the FFT
	if (!inverse)
	  complex_fft_forward(FFTLen,buffer);
	else
	  complex_fft_backward(FFTLen,buffer);
	// Copy the result out
	for (k=0;k<FFTLen;k++) {
	  ob[2*(i*planesize*FFTLen + j + k*planesize)] = buffer[2*k];
	  ob[2*(i*planesize*FFTLen + j + k*planesize) + 1] = buffer[2*k+1];
	}
      }
    }
    Free(buffer);
    return Array(FM_COMPLEX,outDim,ob);
  }

  Array dcomplexFFTFunction(const Array& input, int FFTLen, int FFTDim,
			    bool inverse) {
    Dimensions inDim(input.getDimensions());
    // Allocate the output vector...
    Dimensions outDim(inDim);
    outDim[FFTDim] = FFTLen;
    
    // Calculate the stride...
    int d;
    int workcount;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[FFTDim];
    planesize = 1;
    for (d=0;d<FFTDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=FFTDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    
    // Allocate the buffer for the FFT
    double *buffer;
    buffer = (double*) Malloc(sizeof(double)*FFTLen*2);
    // Allocate the output vector...
    double *ob;
    ob = (double*) Malloc(sizeof(double)*2*outDim.getElementCount());
    int copyIn;
    if (inDim[FFTDim] < FFTLen)
      copyIn = inDim[FFTDim];
    else
      copyIn = FFTLen;
    // Get the data pointer
    double *dp;
    dp = (double*) input.getDataPointer();
    // Do the ffts...
    for (int i=0;i<planecount;i++) {
      for (int j=0;j<planesize;j++) {
	// Reset the buffer
	memset(buffer,0,sizeof(double)*FFTLen*2);
	// Copy the data
	int k;
	for (k=0;k<copyIn;k++) {
	  buffer[2*k] = dp[2*(i*planesize*linesize + j + k*planesize)];
	  buffer[2*k+1] = dp[2*(i*planesize*linesize + j + k*planesize)+1];
	}
	// Take the FFT
	if (!inverse)
	  dcomplex_fft_forward(FFTLen,buffer);
	else
	  dcomplex_fft_backward(FFTLen,buffer);
	// Copy the result out
	for (k=0;k<FFTLen;k++) {
	  ob[2*(i*planesize*FFTLen + j + k*planesize)] = buffer[2*k];
	  ob[2*(i*planesize*FFTLen + j + k*planesize) + 1] = buffer[2*k+1];
	}
      }
    }
    Free(buffer);
    return Array(FM_DCOMPLEX,outDim,ob);
  }

  //!
  //@Module FFT (Inverse) Fast Fourier Transform Function
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
  //netlib at \href{http://www.netlib.org}.  Generally speaking, the 
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
  //mprintplot('fft1');
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
  //mprintplot('fft2');
  //@>
  //The resulting plot is:
  //@figure fft2
  //!
  ArrayVector FFTFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("FFT requires at least one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (argType <= FM_COMPLEX && argType != FM_DOUBLE)
      input.promoteType(FM_COMPLEX);
    else
      input.promoteType(FM_DCOMPLEX);
    // Get the length of the zero pad
    int FFTLength = -1;
    if ((arg.size() > 1) && (!arg[1].isEmpty())) {
      Array FLen(arg[1]);
      FFTLength = FLen.getContentsAsIntegerScalar();
      if (FFTLength <= 0)
	throw Exception("Length argument to FFT should be positive");
    }
    // Get the dimension to FFT along.
    int FFTDim = -1;
    if (arg.size() > 2) {
      Array FDim(arg[2]);
      FFTDim = FDim.getContentsAsIntegerScalar() - 1;
      if (FFTDim < 0)
	throw Exception("Dimension argument to FFT should be positive");
    }
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(arg[0]);
      return retval;
    }
    // Was the dimension specified?  If not, search for it...
    if (FFTDim == -1) {
      Dimensions inDim(input.getDimensions());
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      FFTDim = d;
    }
    if (FFTLength == -1)
      FFTLength = input.getDimensionLength(FFTDim);
    // Handle the fft based on the case...
    ArrayVector retval;
    if (input.getDataClass() == FM_COMPLEX)
      retval.push_back(complexFFTFunction(input,FFTLength,FFTDim,false));
    else
      retval.push_back(dcomplexFFTFunction(input,FFTLength,FFTDim,false));
    return retval;
  }

  ArrayVector IFFTFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("IFFT requires at least one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (argType <= FM_COMPLEX  && argType != FM_DOUBLE)
      input.promoteType(FM_COMPLEX);
    else
      input.promoteType(FM_DCOMPLEX);
    // Get the length of the zero pad
    int FFTLength = -1;
    if ((arg.size() > 1) && (!arg[1].isEmpty())) {
      Array FLen(arg[1]);
      FFTLength = FLen.getContentsAsIntegerScalar();
      if (FFTLength <= 0)
	throw Exception("Length argument to IFFT should be positive");
    }
    // Get the dimension to FFT along.
    int FFTDim = -1;
    if (arg.size() > 2) {
      Array FDim(arg[2]);
      FFTDim = FDim.getContentsAsIntegerScalar() - 1;
      if (FFTDim < 0)
	throw Exception("Dimension argument to IFFT should be positive");
    }
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retval;
      retval.push_back(arg[0]);
      return retval;
    }
    // Was the dimension specified?  If not, search for it...
    if (FFTDim == -1) {
      Dimensions inDim(input.getDimensions());
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      FFTDim = d;
    }
    if (FFTLength == -1)
      FFTLength = input.getDimensionLength(FFTDim);
    // Handle the fft based on the case...
    ArrayVector retval;
    if (input.getDataClass() == FM_COMPLEX)
      retval.push_back(complexFFTFunction(input,FFTLength,FFTDim,true));
    else
      retval.push_back(dcomplexFFTFunction(input,FFTLength,FFTDim,true));
    return retval;
  }
}
