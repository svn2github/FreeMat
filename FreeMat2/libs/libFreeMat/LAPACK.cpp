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

#include "LAPACK.hpp"

namespace FreeMat {

  double getEPS() {
    char CMACH = 'E';
    return dlamch_(&CMACH);
  }


  float getFloatEPS() {
    char CMACH = 'E';
    return slamch_(&CMACH);
  }

  // Utility function used to handle stride changes between columns
  void changeStrideDouble(double*dst, int dstStride, double*src, int srcStride, int rowCount, int colCount){
    int i;
    for (i=0;i<colCount;i++)
      memcpy(dst + i*dstStride, src + i*srcStride, rowCount*sizeof(double));
  }

  void changeStrideFloat(float*dst, int dstStride, float*src, int srcStride, int rowCount, int colCount){
    int i;
    for (i=0;i<colCount;i++)
      memcpy(dst + i*dstStride, src + i*srcStride, rowCount*sizeof(float));
  }
}
