// Copyright (c) 2004 Samit Basu
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

namespace FreeMat {
  //!
  //@Module GAMMA Gamma Function
  //@@Usage
  //Computes the gamma function for real arguments.  The @|gamma|
  //function takes only a single argument
  //@[
  //  y = gamma(x)
  //@]
  //where @|x| is either a @|float| or @|double| array.  The output
  //vector @|y| is the same size (and type) as @|x|.
  //@@Function Internals
  //The gamma function is defined by the integral:
  //\[
  //  \Gamma(x) = \int_{0}^{\infty} e^{-t} t^{x-1} \, dt
  //\]
  //The gamma function obeys the interesting relationship
  //\[
  //  \Gamma(x) = (x-1)\Gamma(x-1),
  //\]
  //and for integer arguments, is equivalent to the factorial function.
  //@@Example
  //Here is a plot of the gamma function over the range @|[-5,5]|.
  //@<
  //x = linspace(-5,5);
  //y = gamma(x);
  //plot(x,y); xlabel('x'); ylabel('gamma(x)');
  //axis([-5,5,-5,5]);
  //mprintplot gamma1
  //@>
  //which results in the following plot.
  //@figure gamma1
  //!
  ArrayVector GammaFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("gamma requires at least one argument");
    Array tmp(arg[0]);
    if (tmp.getDataClass() < FM_FLOAT)
      tmp.promoteType(FM_DOUBLE);
    if (tmp.isComplex())
      throw Exception("gamma does not work with complex arguments");
    if (tmp.isReferenceType() || tmp.isString())
      throw Exception("gamma function requires numerical arguments");
    if (tmp.getDataClass() == FM_FLOAT) {
      Dimensions odims(tmp.getDimensions());
      int olen(odimst.getLength());
      float *sp = (float*) tmp.getDataPointer();
      float *dp = (float*) Array::allocateArray(FM_FLOAT,olen);
      for (int i=0;i<olen;i++)
	dp[i] = gamma_(sp+i);
      return singleArrayVector(Array(FM_FLOAT,odims,dp));
    } else if (tmp.getDataClass() == FM_DOUBLE) {
      Dimensions odims(tmp.getDimensions());
      int olen(odimst.getLength());
      double *sp = (double*) tmp.getDataPointer();
      double *dp = (double*) Array::allocateArray(FM_DOUBLE,olen);
      for (int i=0;i<olen;i++)
	dp[i] = dgamma_(sp+i);
      return singleArrayVector(Array(FM_DOUBLE,odims,dp));
    }
    return ArrayVector();
  }
}
