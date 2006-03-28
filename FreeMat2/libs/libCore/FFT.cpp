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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include <math.h>
#include "fftw3.h"

namespace FreeMat {

  static fftwf_complex *inf, *outf;
  static fftwf_plan pf_forward;
  static fftwf_plan pf_backward;
  static int cN = 0;
  static fftw_complex *in, *out;
  static fftw_plan p_forward;
  static fftw_plan p_backward;
  static int zN = 0;

  void complex_fft_init(int Narg) {
    if (cN == Narg) return;
    if (cN != 0) {
      fftwf_destroy_plan(pf_forward);
      fftwf_destroy_plan(pf_backward);
      fftwf_free(inf);
      fftwf_free(outf);
    }
    inf = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*Narg);
    outf = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*Narg);
    pf_forward = fftwf_plan_dft_1d(Narg,inf,outf,FFTW_FORWARD,FFTW_MEASURE);
    pf_backward = fftwf_plan_dft_1d(Narg,inf,outf,FFTW_BACKWARD,FFTW_MEASURE);
    cN = Narg;
  }

  void complex_fft_forward(int Narg, float *dp) {
    if (cN != Narg) complex_fft_init(Narg);
    memcpy(inf,dp,sizeof(float)*Narg*2);
    fftwf_execute(pf_forward);
    memcpy(dp,outf,sizeof(float)*Narg*2);
  }
  
  void complex_fft_backward(int Narg, float *dp) {
    if (cN != Narg) complex_fft_init(Narg);
    memcpy(inf,dp,sizeof(float)*Narg*2);
    fftwf_execute(pf_backward);
    memcpy(dp,outf,sizeof(float)*Narg*2);
    for (int i=0;i<(2*cN);i++)
      dp[i] /= ((float) Narg);
  }

  void dcomplex_fft_init(int Narg) {
    if (zN == Narg) return;
    if (zN != 0) {
      fftw_destroy_plan(p_forward);
      fftw_destroy_plan(p_backward);
      fftw_free(in);
      fftw_free(out);
    }
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*Narg);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*Narg);
    p_forward = fftw_plan_dft_1d(Narg,in,out,FFTW_FORWARD,FFTW_MEASURE);
    p_backward = fftw_plan_dft_1d(Narg,in,out,FFTW_BACKWARD,FFTW_MEASURE);
    zN = Narg;
  }

  void dcomplex_fft_forward(int Narg, double *dp) {
    if (zN != Narg) dcomplex_fft_init(Narg);
    memcpy(in,dp,sizeof(double)*Narg*2);
    fftw_execute(p_forward);
    memcpy(dp,out,sizeof(double)*Narg*2);
  }

  void dcomplex_fft_backward(int Narg, double *dp) {
    if (zN != Narg) dcomplex_fft_init(Narg);
    memcpy(in,dp,sizeof(double)*Narg*2);
    fftw_execute(p_backward);
    memcpy(dp,out,sizeof(double)*Narg*2);
    for (int i=0;i<(2*zN);i++)
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
