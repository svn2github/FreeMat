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

#ifdef WIN32
#include <SYS\TIMEB.H>
#include <TIME.H>
#include <windows.h>

double getcurrenttime() {
  struct _timeb currSysTime;
  _ftime(&currSysTime);
  return currSysTime.time*1e6 + currSysTime.millitm*1000;
}
#else
#include <sys/time.h>

double getcurrenttime() {
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  return tv.tv_sec*1e6 + tv.tv_usec;
}

#endif // WIN32

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include "Utils.hpp"
#include "IEEEFP.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include "AST.hpp"
#include <math.h>
#include "Types.hpp"
#include <algorithm>
#include "Sparse.hpp"
#include "Math.hpp"


namespace FreeMat {

  ArrayVector HandleEmpty(Array arg) {
    ArrayVector retArray;
    switch (arg.getDataClass()) {
    case FM_LOGICAL:
      retArray.push_back(Array::logicalConstructor(false));
      break;
    case FM_UINT8:
      retArray.push_back(Array::uint8Constructor(0));
      break;
    case FM_INT8:
      retArray.push_back(Array::int8Constructor(0));
      break;
    case FM_UINT16:
      retArray.push_back(Array::uint16Constructor(0));
      break;
    case FM_INT16:
      retArray.push_back(Array::int16Constructor(0));
      break;
    case FM_UINT32:
      retArray.push_back(Array::uint32Constructor(0));
      break;
    case FM_INT32:
      retArray.push_back(Array::int32Constructor(0));
      break;
    case FM_FLOAT:
      retArray.push_back(Array::floatConstructor(0));
      break;
    case FM_DOUBLE:
      retArray.push_back(Array::doubleConstructor(0));
      break;
    case FM_COMPLEX:
      retArray.push_back(Array::complexConstructor(0,0));
      break;
    case FM_DCOMPLEX:
      retArray.push_back(Array::dcomplexConstructor(0,0));
      break;
    }
    return retArray;
  }

  template <class T>
  void TRealLess(const T* spx, const T* spy, T* dp, int count, 
	     int stridex, int stridey) {
    uint32 i;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  template <class T>
  void TComplexLess(const T* spx, const T* spy, T* dp, int count, 
	     int stridex, int stridey) {
    uint32 i;
    T xmag, ymag;
    for (i=0;i<count;i++) {
      xmag = complex_abs(spx[2*stridex*i],spx[2*stridex*i+1]);
      ymag = complex_abs(spy[2*stridey*i],spy[2*stridey*i+1]);
      if (xmag < ymag) {
	dp[2*i] = spx[2*stridex*i];
	dp[2*i+1] = spx[2*stridex*i+1];
      } else {
	dp[2*i] = spy[2*stridey*i];
	dp[2*i+1] = spy[2*stridey*i+1];
      }
    }
  }

  /**
   * Minimum function for integer-type arguments.
   */
  template <class T>
  void TIntMin(const T* sp, T* dp, uint32 *iptr, int planes, int planesize, int linesize) {
    T minval;
    uint32 mindex;
    uint32 i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	minval = sp[i*planesize*linesize + j];
	mindex = 0;
	for (k=0;k<linesize;k++)
	  if (sp[i*planesize*linesize + j + k*planesize] < minval) {
	    minval = sp[i*planesize*linesize + j + k*planesize];
	    mindex = k;
	  }
	dp[i*planesize + j] = minval;
	iptr[i*planesize + j] = mindex + 1;
      }
    }
  }

  /**
   * Minimum function for float arguments.
   */
  template <class T>
  void TRealMin(const T* sp, T* dp, uint32 *iptr, int planes, int planesize, int linesize) {
    T minval;
    uint32 mindex;
    uint32 i, j, k;
    bool init;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	init = false;
	mindex = 0;
	for (k=0;k<linesize;k++) {
	  if (!IsNaN(sp[i*planesize*linesize + j + k*planesize]))
	    if (!init) {
	      init = true;
	      minval = sp[i*planesize*linesize + j + k*planesize];
	      mindex = k;
	    } else if (sp[i*planesize*linesize + j + k*planesize] < minval) {
	      minval = sp[i*planesize*linesize + j + k*planesize];
	      mindex = k;
	    }
	}
	if (init) {
	  dp[i*planesize + j] = minval;
	  iptr[i*planesize + j] = mindex + 1;
	}
	else {
	  dp[i*planesize + j] = atof("nan");
	  iptr[i*planesize + j] = 0;
	}
      }
    }
  }

  /**
   * Minimum function for complex argument - based on magnitude.
   */
  template <class T>
  void TComplexMin(const T* sp, T* dp, uint32 *iptr, int planes, int planesize, int linesize) {
    T minval, minval_r, minval_i;
    T tstval;
    uint32 mindex;
    uint32 i, j, k;
    bool init;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	init = false;
	mindex = 0;
	for (k=0;k<linesize;k++) {
	  if ((!IsNaN(sp[2*(i*planesize*linesize+j+k*planesize)])) &&
	      (!IsNaN(sp[2*(i*planesize*linesize+j+k*planesize)+1]))) {
	    tstval = complex_abs(sp[2*(i*planesize*linesize+j+k*planesize)],
				 sp[2*(i*planesize*linesize+j+k*planesize)+1]);
	    if (!init) {
	      init = true;
	      minval = tstval;
	      mindex = j;
	      minval_r = sp[2*(i*planesize*linesize+j+k*planesize)];
	      minval_i = sp[2*(i*planesize*linesize+j+k*planesize)+1];
	    } else if (tstval < minval) {
	      minval = tstval;
	      mindex = j;
	      minval_r = sp[2*(i*planesize*linesize+j+k*planesize)];
	      minval_i = sp[2*(i*planesize*linesize+j+k*planesize)+1];
	    }
	  }
	}
	if (init) {
	  dp[2*(i*planesize+j)] = minval_r;
	  dp[2*(i*planesize+j)+1] = minval_i;
	  iptr[i*planesize+j] = mindex + 1;
	} else {
	  dp[2*(i*planesize+j)] = atof("nan");
	  dp[2*(i*planesize+j)+1] = atof("nan");
	  iptr[i*planesize+j] = 0;
	}
      }
    }
  }

  template <class T>
  void TRealGreater(const T* spx, const T* spy, T* dp, int count, 
		    int stridex, int stridey) {
    uint32 i;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  template <class T>
  void TComplexGreater(const T* spx, const T* spy, T* dp, int count, 
		       int stridex, int stridey) {
    uint32 i;
    T xmag, ymag;
    for (i=0;i<count;i++) {
      xmag = complex_abs(spx[2*stridex*i],spx[2*stridex*i+1]);
      ymag = complex_abs(spy[2*stridey*i],spy[2*stridey*i+1]);
      if (xmag > ymag) {
	dp[2*i] = spx[2*stridex*i];
	dp[2*i+1] = spx[2*stridex*i+1];
      } else {
	dp[2*i] = spy[2*stridey*i];
	dp[2*i+1] = spy[2*stridey*i+1];
      }
    }
  }

  /**
   * Minimum function for integer-type arguments.
   */
  template <class T>
  void TIntMax(const T* sp, T* dp, uint32 *iptr, int planes, int planesize, int linesize) {
    T maxval;
    uint32 maxdex;
    uint32 i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	maxval = sp[i*planesize*linesize + j];
	maxdex = 0;
	for (k=0;k<linesize;k++)
	  if (sp[i*planesize*linesize + j + k*planesize] > maxval) {
	    maxval = sp[i*planesize*linesize + j + k*planesize];
	    maxdex = k;
	  }
	dp[i*planesize + j] = maxval;
	iptr[i*planesize + j] = maxdex + 1;
      }
    }
  }

  /**
   * Maximum function for float arguments.
   */
  template <class T>
  void TRealMax(const T* sp, T* dp, uint32 *iptr, int planes, int planesize, int linesize) {
    T maxval;
    uint32 maxdex;
    uint32 i, j, k;
    bool init;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	init = false;
	maxdex = 0;
	for (k=0;k<linesize;k++) {
	  if (!IsNaN(sp[i*planesize*linesize + j + k*planesize]))
	    if (!init) {
	      init = true;
	      maxval = sp[i*planesize*linesize + j + k*planesize];
	      maxdex = k;
	    } else if (sp[i*planesize*linesize + j + k*planesize] > maxval) {
	      maxval = sp[i*planesize*linesize + j + k*planesize];
	      maxdex = k;
	    }
	}
	if (init) {
	  dp[i*planesize + j] = maxval;
	  iptr[i*planesize + j] = maxdex + 1;
	}
	else {
	  dp[i*planesize + j] = atof("nan");
	  iptr[i*planesize + j] = 0;
	}
      }
    }
  }

  /**
   * Maximum function for complex argument - based on magnitude.
   */
  template <class T>
  void TComplexMax(const T* sp, T* dp, uint32 *iptr, int planes, int planesize, int linesize) {
    T maxval, maxval_r, maxval_i;
    T tstval;
    uint32 maxdex;
    uint32 i, j, k;
    bool init;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	init = false;
	maxdex = 0;
	for (k=0;k<linesize;k++) {
	  if ((!IsNaN(sp[2*(i*planesize*linesize+j+k*planesize)])) &&
	      (!IsNaN(sp[2*(i*planesize*linesize+j+k*planesize)+1]))) {
	    tstval = complex_abs(sp[2*(i*planesize*linesize+j+k*planesize)],
				 sp[2*(i*planesize*linesize+j+k*planesize)+1]);
	    if (!init) {
	      init = true;
	      maxval = tstval;
	      maxdex = j;
	      maxval_r = sp[2*(i*planesize*linesize+j+k*planesize)];
	      maxval_i = sp[2*(i*planesize*linesize+j+k*planesize)+1];
	    } else if (tstval > maxval) {
	      maxval = tstval;
	      maxdex = j;
	      maxval_r = sp[2*(i*planesize*linesize+j+k*planesize)];
	      maxval_i = sp[2*(i*planesize*linesize+j+k*planesize)+1];
	    }
	  }
	}
	if (init) {
	  dp[2*(i*planesize+j)] = maxval_r;
	  dp[2*(i*planesize+j)+1] = maxval_i;
	  iptr[i*planesize+j] = maxdex + 1;
	} else {
	  dp[2*(i*planesize+j)] = atof("nan");
	  dp[2*(i*planesize+j)+1] = atof("nan");
	  iptr[i*planesize+j] = 0;
	}
      }
    }
  }

  template <class T>
  void TRealCumsum(const T* sp, T* dp, int planes, int planesize, int linesize) {
    T accum;
    int i, j, k;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum = 0;
	for (k=0;k<linesize;k++) {
	  accum += sp[i*planesize*linesize + j + k*planesize];
	  dp[i*planesize*linesize + j + k*planesize] = accum;
	}
      }
    }    
  }

  template <class T>
  void TRealSum(const T* sp, T* dp, int planes, int planesize, int linesize) {
    T accum;
    int i, j, k;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum = 0;
	for (k=0;k<linesize;k++)
	  accum += sp[i*planesize*linesize + j + k*planesize];
	dp[i*planesize + j] = accum;
      }
    }
  }

  template <class T>
  void TComplexCumsum(const T* sp, T* dp, int planes, int planesize, int linesize) {
    T accum_r;
    T accum_i;
    int i, j, k;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum_r = 0;
	accum_i = 0;
	for (k=0;k<linesize;k++) {
	  accum_r += sp[2*(i*planesize*linesize + j + k*planesize)];
	  accum_i += sp[2*(i*planesize*linesize + j + k*planesize)+1];
	  dp[2*(i*planesize*linesize + j + k*planesize)] = accum_r;
	  dp[2*(i*planesize*linesize + j + k*planesize)+1] = accum_i;
	}
      }
    }    
  }

  template <class T>
  void TComplexSum(const T* sp, T* dp, int planes, int planesize, int linesize) {
    T accum_r;
    T accum_i;
    int i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum_r = 0;
	accum_i = 0;
	for (k=0;k<linesize;k++) {
	  accum_r += sp[2*(i*planesize*linesize + j + k*planesize)];
	  accum_i += sp[2*(i*planesize*linesize + j + k*planesize)+1];
	}
	dp[2*(i*planesize + j)] = accum_r;
	dp[2*(i*planesize + j)+1] = accum_i;
      }
    }
  }

  template <class T>
  void TRealMean(const T* sp, T* dp, int planes, int planesize, int linesize) {
    double accum;
    int i, j, k;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum = 0;
	for (k=0;k<linesize;k++)
	  accum += sp[i*planesize*linesize + j + k*planesize];
	dp[i*planesize + j] = accum/linesize;
      }
    }
  }

  template <class T>
  void TComplexMean(const T* sp, T* dp, int planes, int planesize, int linesize) {
    double accum_r;
    double accum_i;
    int i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum_r = 0;
	accum_i = 0;
	for (k=0;k<linesize;k++) {
	  accum_r += sp[2*(i*planesize*linesize + j + k*planesize)];
	  accum_i += sp[2*(i*planesize*linesize + j + k*planesize)+1];
	}
	dp[2*(i*planesize + j)] = accum_r/linesize;
	dp[2*(i*planesize + j)+1] = accum_i/linesize;
      }
    }
  }

  template <class T>
  void TRealVariance(const T* sp, T* dp, int planes, int planesize, int linesize) {
    double accum_first;
    double accum_second;
    int i, j, k;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	// Calculate the mean
	accum_first = 0;
	for (k=0;k<linesize;k++)
	  accum_first += sp[i*planesize*linesize + j + k*planesize]/linesize;
	// The variance is 1/(linesize-1)
	accum_second = 0;
	for (k=0;k<linesize;k++) {
	  double tmp;
	  tmp = sp[i*planesize*linesize + j + k*planesize]-accum_first;
	  accum_second += tmp*tmp/(linesize-1.0);
	}
	dp[i*planesize + j] = accum_second;
      }
    }
  }
  
  template <class T>
  void TComplexVariance(const T* sp, T* dp, int planes, int planesize, int linesize) {
    double accum_r_first;
    double accum_i_first;
    double accum_second;
    int i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum_r_first = 0;
	accum_i_first = 0;
	for (k=0;k<linesize;k++) {
	  accum_r_first += sp[2*(i*planesize*linesize + j + k*planesize)]/linesize;
	  accum_i_first += sp[2*(i*planesize*linesize + j + k*planesize)+1]/linesize;
	}
	accum_second = 0;
	for (k=0;k<linesize;k++) {
	  double tmp_r;
	  double tmp_i;

	  tmp_r = sp[2*(i*planesize*linesize + j + k*planesize)]-accum_r_first;
	  tmp_i = sp[2*(i*planesize*linesize + j + k*planesize)]-accum_r_first;
	  accum_second += (tmp_r*tmp_r + tmp_i*tmp_i)/(linesize-1.0);
	}
	dp[i*planesize + j] = accum_second;
      }
    }
  }

  template <class T>
  void TRealProd(const T* sp, T* dp, int planes, int planesize, int linesize) {
    T accum;
    int i, j, k;

    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum = 1;
	for (k=0;k<linesize;k++)
	  accum *= sp[i*planesize*linesize + j + k*planesize];
	dp[i*planesize + j] = accum;
      }
    }
  }

  template <class T>
  void TComplexProd(const T* sp, T* dp, int planes, int planesize, int linesize) {
    T accum_r;
    T accum_i;
    T t1, t2;
    int i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	accum_r = 1;
	accum_i = 0;
	for (k=0;k<linesize;k++) {
	  t1 = accum_r*sp[2*(i*planesize*linesize + j + k*planesize)] - 
	    accum_i*sp[2*(i*planesize*linesize + j + k*planesize)+1];
	  t2 = accum_r*sp[2*(i*planesize*linesize + j + k*planesize)+1] + 
	    accum_i*sp[2*(i*planesize*linesize + j + k*planesize)];
	  accum_r = t1;
	  accum_r = t2;
	}
	dp[2*(i*planesize + j)] = accum_r;
	dp[2*(i*planesize + j)+1] = accum_i;
      }
    }
  }

  ArrayVector LessThan(int nargout, const ArrayVector& arg) {
    ArrayVector retvec;
    Array x(arg[0]);
    Array y(arg[1]);
    if (x.isReferenceType() || y.isReferenceType())
      throw Exception("min not defined for reference types");
    if (x.isEmpty()) {
      retvec.push_back(y);
      return retvec;
    }
    if (y.isEmpty()) {
      retvec.push_back(x);
      return retvec;
    }
    // Calculate the stride & output size
    Dimensions xSize(x.getDimensions());
    Dimensions ySize(y.getDimensions());
    Dimensions outDim;
    xSize.simplify();
    ySize.simplify();
    int xStride, yStride;
    if (xSize.isScalar() || ySize.isScalar() ||
        xSize.equals(ySize)) {
      if (xSize.isScalar()) {
	outDim = ySize;
	xStride = 0;
	yStride = 1;
      } else if (ySize.isScalar()) {
	outDim = xSize;
	xStride = 1;
	yStride = 0;
      } else {
	outDim = xSize;
	xStride = 1;
	yStride = 1;
      }
    } else
      throw Exception("either both array arguments to min must be the same size, or one must be a scalar.");
    // Determine the type of the output
    Class outType;
    if (x.getDataClass() > y.getDataClass()) {
      outType = x.getDataClass();
      y.promoteType(x.getDataClass());
    } else {
      outType = y.getDataClass();
      x.promoteType(y.getDataClass());
    }
    // Based on the type of the output... call the associated helper function
    Array retval;
    switch(outType) {
    case FM_LOGICAL: {
      char* ptr = (char *) Malloc(sizeof(logical)*outDim.getElementCount());
      TRealLess<logical>((const logical *) x.getDataPointer(),
			 (const logical *) y.getDataPointer(),
			 (logical *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      TRealLess<uint8>((const uint8 *) x.getDataPointer(),
			 (const uint8 *) y.getDataPointer(),
			 (uint8 *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      TRealLess<int8>((const int8 *) x.getDataPointer(),
			 (const int8 *) y.getDataPointer(),
			 (int8 *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      TRealLess<uint16>((const uint16 *) x.getDataPointer(),
			 (const uint16 *) y.getDataPointer(),
			 (uint16 *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      TRealLess<int16>((const int16 *) x.getDataPointer(),
			 (const int16 *) y.getDataPointer(),
			 (int16 *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      TRealLess<uint32>((const uint32 *) x.getDataPointer(),
			 (const uint32 *) y.getDataPointer(),
			 (uint32 *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealLess<int32>((const int32 *) x.getDataPointer(),
			 (const int32 *) y.getDataPointer(),
			 (int32 *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealLess<float>((const float *) x.getDataPointer(),
			 (const float *) y.getDataPointer(),
			 (float *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealLess<double>((const double *) x.getDataPointer(),
			 (const double *) y.getDataPointer(),
			 (double *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexLess<float>((const float *) x.getDataPointer(),
			  (const float *) y.getDataPointer(),
			  (float *) ptr, outDim.getElementCount(),
			  xStride, yStride);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexLess<double>((const double *) x.getDataPointer(),
			   (const double *) y.getDataPointer(),
			   (double *) ptr, outDim.getElementCount(),
			   xStride, yStride);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    retvec.push_back(retval);
    return retvec;
  }


  //!
  //@Module MIN Minimum Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the minimum of an array along a given dimension, or alternately, 
  //computes two arrays (entry-wise) and keeps the smaller value for each array.
  //As a result, the @|min| function has a number of syntaxes.  The first
  //one computes the minimum of an array along a given dimension.
  //The first general syntax for its use is either
  //@[
  //   [y,n] = min(x,[],d)
  //@]
  //where @|x| is a multidimensional array of numerical type, in which case the
  //output @|y| is the minimum of @|x| along dimension @|d|.  
  //The second argument @|n| is the index that results in the minimum.
  //In the event that multiple minima are present with the same value,
  //the index of the first minimum is used. 
  //The second general syntax for the use of the @|min| function is
  //@[
  //   [y,n] = min(x)
  //@] 
  //In this case, the minimum is taken along the first non-singleton 
  //dimension of @|x|.  For complex data types,
  //the minimum is based on the magnitude of the numbers.  NaNs are
  //ignored in the calculations.
  //The third general syntax for the use of the @|min| function is as 
  //a comparison function for pairs of arrays.  Here, the general syntax is
  //@[
  //   y = min(x,z)
  //@]
  //where @|x| and @|z| are either both numerical arrays of the same dimensions,
  //or one of the two is a scalar.  In the first case, the output is the 
  //same size as both arrays, and is defined elementwise by the smaller of the
  //two arrays.  In the second case, the output is defined elementwise by the 
  //smaller of the array entries and the scalar.
  //@@Function Internals
  //In the general version of the @|min| function which is applied to
  //a single array (using the @|min(x,[],d)| or @|min(x)| syntaxes),
  //The output is computed via
  //\[
  //y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
  //\min_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}),
  //\]
  //and the output array @|n| of indices is calculated via
  //\[
  //n(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = \arg
  //\min_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
  //\]
  //In the two-array version (@|min(x,z)|), the single output is computed as
  //\[
  //  y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
  //\begin{cases}
  //  x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) & x(\cdots) \leq z(\cdots) \\
  //  z(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) & z(\cdots) < x(\cdots).
  //\end{cases}
  //\]
  //@@Example
  //The following piece of code demonstrates various uses of the minimum
  //function.  We start with the one-array version.
  //@<
  //A = [5,1,3;3,2,1;0,3,1]
  //@>
  //We first take the minimum along the columns, resulting in a row vector.
  //@<
  //min(A)
  //@>
  //Next, we take the minimum along the rows, resulting in a column vector.
  //@<
  //min(A,[],2)
  //@>
  //When the dimension argument is not supplied, @|min| acts along the first 
  //non-singular dimension.  For a row vector, this is the column direction:
  //@<
  //min([5,3,2,9])
  //@>
  //
  //For the two-argument version, we can compute the smaller of two arrays,
  //as in this example:
  //@<
  //a = int8(100*randn(4))
  //b = int8(100*randn(4))
  //min(a,b)
  //@>
  //Or alternately, we can compare an array with a scalar
  //@<
  //a = randn(2)
  //min(a,0)
  //@>
  //!
  ArrayVector MinFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1 || arg.size() > 3)
      throw Exception("min requires at least one argument, and at most three arguments");
    // Determine if this is a call to the Min function or the LessThan function
    // (the internal version of the two array min function)
    if (arg.size() == 2)
      return LessThan(nargout,arg);
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("min only defined for numeric types");
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      if (!arg[1].isEmpty())
	throw Exception("Single array syntax for min function must have an empty array as the second argument");
      Array WDim(arg[2]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to min should be positive");
    }
    if (input.isScalar())
      return singleArrayVector(input);
    if (input.isEmpty())
      return HandleEmpty(input);
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    outDim[workDim] = 1;
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the output that contains the indices
    uint32* iptr = (uint32 *) Malloc(sizeof(uint32)*outDim.getElementCount());
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_LOGICAL: {
      char* ptr = (char *) Malloc(sizeof(logical)*outDim.getElementCount());
      TIntMin<logical>((const logical *) input.getDataPointer(),
		       (logical *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      TIntMin<uint8>((const uint8 *) input.getDataPointer(),
		       (uint8 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      TIntMin<int8>((const int8 *) input.getDataPointer(),
		       (int8 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      TIntMin<uint16>((const uint16 *) input.getDataPointer(),
		       (uint16 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      TIntMin<int16>((const int16 *) input.getDataPointer(),
		       (int16 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      TIntMin<uint32>((const uint32 *) input.getDataPointer(),
		       (uint32 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TIntMin<int32>((const int32 *) input.getDataPointer(),
		       (int32 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealMin<float>((const float *) input.getDataPointer(),
		      (float *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealMin<double>((const double *) input.getDataPointer(),
		      (double *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexMin<float>((const float *) input.getDataPointer(),
			 (float *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexMin<double>((const double *) input.getDataPointer(),
			  (double *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    Array iretval(FM_UINT32,outDim,iptr);
    ArrayVector retArray;
    retArray.push_back(retval);
    retArray.push_back(iretval);
    return retArray;
  }

  ArrayVector GreaterThan(int nargout, const ArrayVector& arg) {
    ArrayVector retvec;
    Array x(arg[0]);
    Array y(arg[1]);
    if (x.isReferenceType() || y.isReferenceType())
      throw Exception("max not defined for reference types");
    if (x.isEmpty()) {
      retvec.push_back(y);
      return retvec;
    }
    if (y.isEmpty()) {
      retvec.push_back(x);
      return retvec;
    }
    // Calculate the stride & output size
    Dimensions xSize(x.getDimensions());
    Dimensions ySize(y.getDimensions());
    Dimensions outDim;
    xSize.simplify();
    ySize.simplify();
    int xStride, yStride;
    if (xSize.isScalar() || ySize.isScalar() ||
        xSize.equals(ySize)) {
      if (xSize.isScalar()) {
	outDim = ySize;
	xStride = 0;
	yStride = 1;
      } else if (ySize.isScalar()) {
	outDim = xSize;
	xStride = 1;
	yStride = 0;
      } else {
	outDim = xSize;
	xStride = 1;
	yStride = 1;
      }
    } else
      throw Exception("either both array arguments to max must be the same size, or one must be a scalar.");
    // Determine the type of the output
    Class outType;
    if (x.getDataClass() > y.getDataClass()) {
      outType = x.getDataClass();
      y.promoteType(x.getDataClass());
    } else {
      outType = y.getDataClass();
      x.promoteType(y.getDataClass());
    }
    // Based on the type of the output... call the associated helper function
    Array retval;
    switch(outType) {
    case FM_LOGICAL: {
      char* ptr = (char *) Malloc(sizeof(logical)*outDim.getElementCount());
      TRealGreater<logical>((const logical *) x.getDataPointer(),
			    (const logical *) y.getDataPointer(),
			    (logical *) ptr, outDim.getElementCount(),
			    xStride, yStride);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      TRealGreater<uint8>((const uint8 *) x.getDataPointer(),
			  (const uint8 *) y.getDataPointer(),
			  (uint8 *) ptr, outDim.getElementCount(),
			  xStride, yStride);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      TRealGreater<int8>((const int8 *) x.getDataPointer(),
			 (const int8 *) y.getDataPointer(),
			 (int8 *) ptr, outDim.getElementCount(),
			 xStride, yStride);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      TRealGreater<uint16>((const uint16 *) x.getDataPointer(),
			   (const uint16 *) y.getDataPointer(),
			   (uint16 *) ptr, outDim.getElementCount(),
			   xStride, yStride);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      TRealGreater<int16>((const int16 *) x.getDataPointer(),
			  (const int16 *) y.getDataPointer(),
			  (int16 *) ptr, outDim.getElementCount(),
			  xStride, yStride);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      TRealGreater<uint32>((const uint32 *) x.getDataPointer(),
			  (const uint32 *) y.getDataPointer(),
			  (uint32 *) ptr, outDim.getElementCount(),
			  xStride, yStride);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealGreater<int32>((const int32 *) x.getDataPointer(),
			  (const int32 *) y.getDataPointer(),
			  (int32 *) ptr, outDim.getElementCount(),
			  xStride, yStride);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealGreater<float>((const float *) x.getDataPointer(),
			  (const float *) y.getDataPointer(),
			  (float *) ptr, outDim.getElementCount(),
			  xStride, yStride);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealGreater<double>((const double *) x.getDataPointer(),
			  (const double *) y.getDataPointer(),
			  (double *) ptr, outDim.getElementCount(),
			  xStride, yStride);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexGreater<float>((const float *) x.getDataPointer(),
			     (const float *) y.getDataPointer(),
			     (float *) ptr, outDim.getElementCount(),
			     xStride, yStride);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexGreater<double>((const double *) x.getDataPointer(),
			      (const double *) y.getDataPointer(),
			      (double *) ptr, outDim.getElementCount(),
			      xStride, yStride);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    retvec.push_back(retval);
    return retvec;
  }

  //!
  //@Module MAX Maximum Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the maximum of an array along a given dimension, or alternately, 
  //computes two arrays (entry-wise) and keeps the smaller value for each array.
  //As a result, the @|max| function has a number of syntaxes.  The first
  //one computes the maximum of an array along a given dimension.
  //The first general syntax for its use is either
  //@[
  //   [y,n] = max(x,[],d)
  //@]
  //where @|x| is a multidimensional array of numerical type, in which case the
  //output @|y| is the maximum of @|x| along dimension @|d|.  
  //The second argument @|n| is the index that results in the maximum.
  //In the event that multiple maxima are present with the same value,
  //the index of the first maximum is used. 
  //The second general syntax for the use of the @|max| function is
  //@[
  //   [y,n] = max(x)
  //@] 
  //In this case, the maximum is taken along the first non-singleton 
  //dimension of @|x|.  For complex data types,
  //the maximum is based on the magnitude of the numbers.  NaNs are
  //ignored in the calculations.
  //The third general syntax for the use of the @|max| function is as 
  //a comparison function for pairs of arrays.  Here, the general syntax is
  //@[
  //   y = max(x,z)
  //@]
  //where @|x| and @|z| are either both numerical arrays of the same dimensions,
  //or one of the two is a scalar.  In the first case, the output is the 
  //same size as both arrays, and is defined elementwise by the smaller of the
  //two arrays.  In the second case, the output is defined elementwise by the 
  //smaller of the array entries and the scalar.
  //@@Function Internals
  //In the general version of the @|max| function which is applied to
  //a single array (using the @|max(x,[],d)| or @|max(x)| syntaxes),
  //The output is computed via
  //\[
  //y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
  //\max_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}),
  //\]
  //and the output array @|n| of indices is calculated via
  //\[
  //n(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = \arg
  //\max_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
  //\]
  //In the two-array version (@|max(x,z)|), the single output is computed as
  //\[
  //  y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
  //\begin{cases}
  //  x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) & x(\cdots) \leq z(\cdots) \\
  //  z(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) & z(\cdots) < x(\cdots).
  //\end{cases}
  //\]
  //@@Example
  //The following piece of code demonstrates various uses of the maximum
  //function.  We start with the one-array version.
  //@<
  //A = [5,1,3;3,2,1;0,3,1]
  //@>
  //We first take the maximum along the columns, resulting in a row vector.
  //@<
  //max(A)
  //@>
  //Next, we take the maximum along the rows, resulting in a column vector.
  //@<
  //max(A,[],2)
  //@>
  //When the dimension argument is not supplied, @|max| acts along the first non-singular dimension.  For a row vector, this is the column direction:
  //@<
  //max([5,3,2,9])
  //@>
  //
  //For the two-argument version, we can compute the smaller of two arrays,
  //as in this example:
  //@<
  //a = int8(100*randn(4))
  //b = int8(100*randn(4))
  //max(a,b)
  //@>
  //Or alternately, we can compare an array with a scalar
  //@<
  //a = randn(2)
  //max(a,0)
  //@>
  //!
  ArrayVector MaxFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1 || arg.size() > 3)
      throw Exception("max requires at least one argument, and at most three arguments");
    // Determine if this is a call to the Max function or the GreaterThan function
    // (the internal version of the two array max function)
    if (arg.size() == 2)
      return GreaterThan(nargout,arg);
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("max only defined for numeric types");
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      if (!arg[1].isEmpty())
	throw Exception("Single array syntax for max function must have an empty array as the second argument");
      Array WDim(arg[2]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to max should be positive");
    }
    if (input.isScalar())
      return singleArrayVector(input);
    if (input.isEmpty())
      return HandleEmpty(input);
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    outDim[workDim] = 1;
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the output that contains the indices
    uint32* iptr = (uint32 *) Malloc(sizeof(uint32)*outDim.getElementCount());
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_LOGICAL: {
      char* ptr = (char *) Malloc(sizeof(logical)*outDim.getElementCount());
      TIntMax<logical>((const logical *) input.getDataPointer(),
		       (logical *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      TIntMax<uint8>((const uint8 *) input.getDataPointer(),
		       (uint8 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      TIntMax<int8>((const int8 *) input.getDataPointer(),
		       (int8 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      TIntMax<uint16>((const uint16 *) input.getDataPointer(),
		       (uint16 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      TIntMax<int16>((const int16 *) input.getDataPointer(),
		       (int16 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      TIntMax<uint32>((const uint32 *) input.getDataPointer(),
		       (uint32 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TIntMax<int32>((const int32 *) input.getDataPointer(),
		       (int32 *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealMax<float>((const float *) input.getDataPointer(),
		      (float *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealMax<double>((const double *) input.getDataPointer(),
		      (double *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexMax<float>((const float *) input.getDataPointer(),
			 (float *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexMax<double>((const double *) input.getDataPointer(),
			  (double *) ptr, iptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    Array iretval(FM_UINT32,outDim,iptr);
    ArrayVector retArray;
    retArray.push_back(retval);
    retArray.push_back(iretval);
    return retArray;
  }

  //!
  //@Module CEIL Ceiling Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the ceiling of an n-dimensional array elementwise.  The
  //ceiling of a number is defined as the smallest integer that is
  //larger than or equal to that number. The general syntax for its use
  //is
  //@[
  //   y = ceil(x)
  //@]
  //where @|x| is a multidimensional array of numerical type.  The @|ceil| 
  //function preserves the type of the argument.  So integer arguments 
  //are not modified, and @|float| arrays return @|float| arrays as 
  //outputs, and similarly for @|double| arrays.  The @|ceil| function 
  //is not defined for @|complex| or @|dcomplex| types.
  //@@Example
  //The following demonstrates the @|ceil| function applied to various
  //(numerical) arguments.  For integer arguments, the ceil function has
  //no effect:
  //@<
  //ceil(3)
  //ceil(-3)
  //@>
  //Next, we take the @|ceil| of a floating point value:
  //@<
  //ceil(3.023f)
  //ceil(-2.341f)
  //@>
  //Note that the return type is a @|float| also.  Finally, for a @|double|
  //type:
  //@<
  //ceil(4.312)
  //ceil(-5.32)
  //@>
  //!
  ArrayVector CeilFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("ceil requires one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("ceil only defined for numeric types");
    Array retval;
    switch (argType) {
    case FM_LOGICAL:
    case FM_UINT8: 
    case FM_INT8:
    case FM_UINT16:
    case FM_INT16:
    case FM_UINT32:
    case FM_INT32: 
      retval = input;
      break;
    case FM_FLOAT: {
      float* dp = (float *) Malloc(sizeof(float)*input.getLength());
      const float* sp = (const float *) input.getDataPointer();
      int cnt;
      cnt = input.getLength();
      for (int i = 0;i<cnt;i++)
	dp[i] = ceilf(sp[i]);
      retval = Array(FM_FLOAT,input.getDimensions(),dp);
      break;
    }
    case FM_DOUBLE: {
      double* dp = (double *) Malloc(sizeof(double)*input.getLength());
      const double* sp = (const double *) input.getDataPointer();
      int cnt;
      cnt = input.getLength();
      for (int i = 0;i<cnt;i++)
	dp[i] = ceil(sp[i]);
      retval = Array(FM_DOUBLE,input.getDimensions(),dp);
      break;
    }
    case FM_COMPLEX:
    case FM_DCOMPLEX: 
      throw Exception("ceil not defined for complex arguments");
    }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

  //!
  //@Module FLOOR Floor Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the floor of an n-dimensional array elementwise.  The
  //floor of a number is defined as the smallest integer that is
  //less than or equal to that number. The general syntax for its use
  //is
  //@[
  //   y = floor(x)
  //@]
  //where @|x| is a multidimensional array of numerical type.  The @|floor| 
  //function preserves the type of the argument.  So integer arguments 
  //are not modified, and @|float| arrays return @|float| arrays as 
  //outputs, and similarly for @|double| arrays.  The @|floor| function 
  //is not defined for @|complex| or @|dcomplex| types.
  //@@Example
  //The following demonstrates the @|floor| function applied to various
  //(numerical) arguments.  For integer arguments, the floor function has
  //no effect:
  //@<
  //floor(3)
  //floor(-3)
  //@>
  //Next, we take the @|floor| of a floating point value:
  //@<
  //floor(3.023f)
  //floor(-2.341f)
  //@>
  //Note that the return type is a @|float| also.  Finally, for a @|double|
  //type:
  //@<
  //floor(4.312)
  //floor(-5.32)
  //@>
 //!
  ArrayVector FloorFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("floor requires one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("floor only defined for numeric types");
    Array retval;
    switch (argType) {
    case FM_LOGICAL:
    case FM_UINT8: 
    case FM_INT8:
    case FM_UINT16:
    case FM_INT16:
    case FM_UINT32:
    case FM_INT32: 
      retval = input;
      break;
    case FM_FLOAT: {
      float* dp = (float *) Malloc(sizeof(float)*input.getLength());
      const float* sp = (const float *) input.getDataPointer();
      int cnt;
      cnt = input.getLength();
      for (int i = 0;i<cnt;i++)
	dp[i] = floorf(sp[i]);
      retval = Array(FM_FLOAT,input.getDimensions(),dp);
      break;
    }
    case FM_DOUBLE: {
      double* dp = (double *) Malloc(sizeof(double)*input.getLength());
      const double* sp = (const double *) input.getDataPointer();
      int cnt;
      cnt = input.getLength();
      for (int i = 0;i<cnt;i++)
	dp[i] = floor(sp[i]);
      retval = Array(FM_DOUBLE,input.getDimensions(),dp);
      break;
    }
    case FM_COMPLEX:
    case FM_DCOMPLEX: 
      throw Exception("floor not defined for complex arguments");
    }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;    
  }

  //!
  //@Module ROUND Round Function
  //@@Section ELEMENTARY
  //@@Usage
  //Rounds an n-dimensional array to the nearest integer elementwise.
  //The general syntax for its use is
  //@[
  //   y = round(x)
  //@]
  //where @|x| is a multidimensional array of numerical type.  The @|round| 
  //function preserves the type of the argument.  So integer arguments 
  //are not modified, and @|float| arrays return @|float| arrays as 
  //outputs, and similarly for @|double| arrays.  The @|round| function 
  //is not defined for @|complex| or @|dcomplex| types.
  //@@Example
  //The following demonstrates the @|round| function applied to various
  //(numerical) arguments.  For integer arguments, the round function has
  //no effect:
  //@<
  //round(3)
  //round(-3)
  //@>
  //Next, we take the @|round| of a floating point value:
  //@<
  //round(3.023f)
  //round(-2.341f)
  //@>
  //Note that the return type is a @|float| also.  Finally, for a @|double|
  //type:
  //@<
  //round(4.312)
  //round(-5.32)
  //@>
 //!
  ArrayVector RoundFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("round requires one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("round only defined for numeric types");
    Array retval;
    switch (argType) {
    case FM_LOGICAL:
    case FM_UINT8: 
    case FM_INT8:
    case FM_UINT16:
    case FM_INT16:
    case FM_UINT32:
    case FM_INT32: 
      retval = input;
      break;
    case FM_FLOAT: {
      float* dp = (float *) Malloc(sizeof(float)*input.getLength());
      const float* sp = (const float *) input.getDataPointer();
      int cnt;
      cnt = input.getLength();
      for (int i = 0;i<cnt;i++)
	    dp[i] = rint(sp[i]);
      retval = Array(FM_FLOAT,input.getDimensions(),dp);
      break;
    }
    case FM_DOUBLE: {
      double* dp = (double *) Malloc(sizeof(double)*input.getLength());
      const double* sp = (const double *) input.getDataPointer();
      int cnt;
      cnt = input.getLength();
      for (int i = 0;i<cnt;i++)
	dp[i] = rint(sp[i]);
      retval = Array(FM_DOUBLE,input.getDimensions(),dp);
      break;
    }
    case FM_COMPLEX:
    case FM_DCOMPLEX: 
      throw Exception("round not defined for complex arguments");
    }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;    
  }

  //!
  //@Module CUMSUM Cumulative Summation Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the cumulative sum of an n-dimensional array along a given
  //dimension.  The general syntax for its use is
  //@[
  //  y = cumsum(x,d)
  //@]
  //where @|x| is a multidimensional array of numerical type, and @|d|
  //is the dimension along which to perform the cumulative sum.  The
  //output @|y| is the same size of @|x|.  Integer types are promoted
  //to @|int32|. If the dimension @|d| is not specified, then the
  //cumulative sum is applied along the first non-singular dimension.
  //@@Function Internals
  //The output is computed via
  //\[
  //  y(m_1,\ldots,m_{d-1},j,m_{d+1},\ldots,m_{p}) = 
  //  \sum_{k=1}^{j} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}).
  //\]
  //@@Example
  //The default action is to perform the cumulative sum along the
  //first non-singular dimension.
  //@<
  //A = [5,1,3;3,2,1;0,3,1]
  //cumsum(A)
  //@>
  //To compute the cumulative sum along the columns:
  //@<
  //cumsum(A,2)
  //@>
  //The cumulative sum also works along arbitrary dimensions
  //@<
  //B(:,:,1) = [5,2;8,9];
  //B(:,:,2) = [1,0;3,0]
  //cumsum(B,3)
  //@>
  //!  
  ArrayVector CumsumFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("cumsum requires at least one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("sum only defined for numeric types");
    if ((argType >= FM_LOGICAL) && (argType < FM_INT32)) {
      input.promoteType(FM_INT32);
      argType = FM_INT32;
    }    
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      Array WDim(arg[1]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to cumsum should be positive");
    }
    if (input.isEmpty()) 
      return HandleEmpty(input);
    if (input.isScalar())
      return singleArrayVector(input);
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealCumsum<int32>((const int32 *) input.getDataPointer(),
			 (int32 *) ptr, planecount, planesize, linesize);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealCumsum<float>((const float *) input.getDataPointer(),
			 (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealCumsum<double>((const double *) input.getDataPointer(),
			  (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexCumsum<float>((const float *) input.getDataPointer(),
			    (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexCumsum<double>((const double *) input.getDataPointer(),
			     (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

  //!
  //@Module SUM Sum Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the summation of an array along a given dimension.  The general
  //syntax for its use is
  //@[
  //  y = sum(x,d)
  //@]
  //where @|x| is an @|n|-dimensions array of numerical type.
  //The output is of the same numerical type as the input.  The argument
  //@|d| is optional, and denotes the dimension along which to take
  //the summation.  The output @|y| is the same size as @|x|, except
  //that it is singular along the summation direction.  So, for example,
  //if @|x| is a @|3 x 3 x 4| array, and we compute the summation along
  //dimension @|d=2|, then the output is of size @|3 x 1 x 4|.
  //@@Function Internals
  //The output is computed via
  //\[
  //y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
  //\sum_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
  //\]
  //If @|d| is omitted, then the summation is taken along the 
  //first non-singleton dimension of @|x|. 
  //@@Example
  //The following piece of code demonstrates various uses of the summation
  //function
  //@<
  //A = [5,1,3;3,2,1;0,3,1]
  //@>
  //We start by calling @|sum| without a dimension argument, in which 
  //case it defaults to the first nonsingular dimension (in this case, 
  //along the columns or @|d = 1|).
  //@<
  //sum(A)
  //@>
  //Next, we take the sum along the rows.
  //@<
  //sum(A,2)
  //@>
  //!
  ArrayVector SumFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("sum requires at least one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("sum only defined for numeric types");
    if ((argType >= FM_LOGICAL) && (argType < FM_INT32)) {
      input.promoteType(FM_INT32);
      argType = FM_INT32;
    }    
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      Array WDim(arg[1]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to sum should be positive");
    }
    if (input.isEmpty()) 
      return HandleEmpty(input);
    if (input.isScalar())
      return singleArrayVector(input);
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    outDim[workDim] = 1;
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    // Special case Sparse Matrices
    if (input.isSparse()) {
      if (workDim == 0)
	return singleArrayVector(Array(input.getDataClass(),
				       outDim,
				       SparseMatrixSumColumns(input.getDataClass(),
							      input.getDimensionLength(0),
							      input.getDimensionLength(1),
							      input.getSparseDataPointer()),
				       true));
      else if (workDim == 1)
	return singleArrayVector(Array(input.getDataClass(),
				       outDim,
				       SparseMatrixSumRows(input.getDataClass(),
							   input.getDimensionLength(0),
							   input.getDimensionLength(1),
							   input.getSparseDataPointer()),
				       true));
      else
	return singleArrayVector(input);
    }
    Array retval;
    switch (argType) {
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSum<int32>((const int32 *) input.getDataPointer(),
		      (int32 *) ptr, planecount, planesize, linesize);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealSum<float>((const float *) input.getDataPointer(),
		      (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealSum<double>((const double *) input.getDataPointer(),
		       (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexSum<float>((const float *) input.getDataPointer(),
			 (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexSum<double>((const double *) input.getDataPointer(),
			 (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
   }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

  //!
  //@Module MEAN Mean Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the mean of an array along a given dimension.  The general
  //syntax for its use is
  //@[
  //  y = mean(x,d)
  //@]
  //where @|x| is an @|n|-dimensions array of numerical type.
  //The output is of the same numerical type as the input.  The argument
  //@|d| is optional, and denotes the dimension along which to take
  //the mean.  The output @|y| is the same size as @|x|, except
  //that it is singular along the mean direction.  So, for example,
  //if @|x| is a @|3 x 3 x 4| array, and we compute the mean along
  //dimension @|d=2|, then the output is of size @|3 x 1 x 4|.
  //@@Function Internals
  //The output is computed via
  //\[
  //y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = \frac{1}{N}
  //\sum_{k=1}^{N} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
  //\]
  //If @|d| is omitted, then the mean is taken along the 
  //first non-singleton dimension of @|x|. 
  //@@Example
  //The following piece of code demonstrates various uses of the mean
  //function
  //@<
  //A = [5,1,3;3,2,1;0,3,1]
  //@>
  //We start by calling @|mean| without a dimension argument, in which 
  //case it defaults to the first nonsingular dimension (in this case, 
  //along the columns or @|d = 1|).
  //@<
  //mean(A)
  //@>
  //Next, we take the mean along the rows.
  //@<
  //mean(A,2)
  //@>
  //!
  ArrayVector MeanFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("mean requires at least one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("mean only defined for numeric types");
    if ((argType >= FM_LOGICAL) && (argType <= FM_INT32)) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }    
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      Array WDim(arg[1]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to mean should be positive");
    }
    if (input.isEmpty()) 
      return HandleEmpty(input);
    if (input.isScalar())
      return singleArrayVector(input);
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    outDim[workDim] = 1;
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealMean<float>((const float *) input.getDataPointer(),
		       (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealMean<double>((const double *) input.getDataPointer(),
			(double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexMean<float>((const float *) input.getDataPointer(),
			  (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexMean<double>((const double *) input.getDataPointer(),
			 (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
   }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

  //!
  //@Module VAR Variance Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the variance of an array along a given dimension.  The general
  //syntax for its use is
  //@[
  //  y = var(x,d)
  //@]
  //where @|x| is an @|n|-dimensions array of numerical type.
  //The output is of the same numerical type as the input.  The argument
  //@|d| is optional, and denotes the dimension along which to take
  //the variance.  The output @|y| is the same size as @|x|, except
  //that it is singular along the mean direction.  So, for example,
  //if @|x| is a @|3 x 3 x 4| array, and we compute the mean along
  //dimension @|d=2|, then the output is of size @|3 x 1 x 4|.
  //@@Function Internals
  //The output is computed via
  //\[
  //y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = \frac{1}{N-1}
  //\sum_{k=1}^{N} \left(x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p}) 
  // - \bar{x}\right)^2,
  //\]
  //where 
  //\[
  //\bar{x}  = \frac{1}{N}
  //\sum_{k=1}^{N} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
  //\]
  //If @|d| is omitted, then the mean is taken along the 
  //first non-singleton dimension of @|x|. 
  //@@Example
  //The following piece of code demonstrates various uses of the var
  //function
  //@<
  //A = [5,1,3;3,2,1;0,3,1]
  //@>
  //We start by calling @|var| without a dimension argument, in which 
  //case it defaults to the first nonsingular dimension (in this case, 
  //along the columns or @|d = 1|).
  //@<
  //var(A)
  //@>
  //Next, we take the variance along the rows.
  //@<
  //var(A,2)
  //@>
  //!
  ArrayVector VarFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("var requires at least one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("var only defined for numeric types");
    if ((argType >= FM_LOGICAL) && (argType <= FM_INT32)) {
      input.promoteType(FM_DOUBLE);
      argType = FM_DOUBLE;
    }    
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      Array WDim(arg[1]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to var should be positive");
    }
    if (input.isScalar() || input.isEmpty()) 
      return HandleEmpty(input);
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    outDim[workDim] = 1;
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealVariance<float>((const float *) input.getDataPointer(),
		      (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealVariance<double>((const double *) input.getDataPointer(),
		       (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexVariance<float>((const float *) input.getDataPointer(),
			 (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexVariance<double>((const double *) input.getDataPointer(),
			  (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

  //!
  //@Module CONJ Conjugate Function
  //@@Section ELEMENTARY
  //@@Usage
  //Returns the complex conjugate of the input array for all elements.  The 
  //general syntax for its use is
  //@[
  //   y = conj(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.  The output 
  //is the same numerical type as the input.  The @|conj| function does
  //nothing to real and integer types.
  //@@Example
  //The following demonstrates the complex conjugate applied to a complex scalar.
  //@<
  //conj(3+4*i)
  //@>
  //The @|conj| function has no effect on real arguments:
  //@<
  //conj([2,3,4])
  //@>
  //For a double-precision complex array,
  //@<
  //conj([2.0+3.0*i,i])
  //@>
  //!
  ArrayVector ConjFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("conj function requires 1 argument");
    Array tmp(arg[0]);
    if (tmp.isReferenceType())
      throw Exception("argument to conjugate function must be numeric");
    Class argType(tmp.getDataClass());
    Array retval;
    int i;
    if (argType == FM_COMPLEX) {
      int len;
      len = tmp.getLength();
      float *dp = (float*) tmp.getDataPointer();
      float *ptr = (float*) Malloc(sizeof(float)*tmp.getLength()*2);
      for (i=0;i<len;i++) {
	ptr[2*i] = dp[2*i];
	ptr[2*i+1] = -dp[2*i+1];
      }
      retval = Array(FM_COMPLEX,tmp.getDimensions(),ptr);
    } else if (argType == FM_DCOMPLEX) {
      int len;
      len = tmp.getLength();
      double *dp = (double*) tmp.getDataPointer();
      double *ptr = (double*) Malloc(sizeof(double)*tmp.getLength()*2);
      for (i=0;i<len;i++) {
	ptr[2*i] = dp[2*i];
	ptr[2*i+1] = -dp[2*i+1];
      }
      retval = Array(FM_DCOMPLEX,tmp.getDimensions(),ptr);
    } else
      retval = tmp;
    ArrayVector out;
    out.push_back(retval);
    return out;
  }

  //!
  //@Module REAL Real Function
  //@@Section ELEMENTARY
  //@@Usage
  //Returns the real part of the input array for all elements.  The 
  //general syntax for its use is
  //@[
  //   y = real(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.  The output 
  //is the same numerical type as the input, unless the input is @|complex|
  //or @|dcomplex|.  For @|complex| inputs, the real part is a floating
  //point array, so that the return type is @|float|.  For @|dcomplex|
  //inputs, the real part is a double precision floating point array, so that
  //the return type is @|double|.  The @|real| function does
  //nothing to real and integer types.
  //@@Example
  //The following demonstrates the @|real| applied to a complex scalar.
  //@<
  //real(3+4*i)
  //@>
  //The @|real| function has no effect on real arguments:
  //@<
  //real([2,3,4])
  //@>
  //For a double-precision complex array,
  //@<
  //real([2.0+3.0*i,i])
  //@>
  //!
  ArrayVector RealFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("real function requires 1 argument");
    Array tmp(arg[0]);
    if (tmp.isReferenceType())
      throw Exception("argument to real function must be numeric");
    Class argType(tmp.getDataClass());
    Array retval;
    int i;
    if (argType == FM_COMPLEX) {
      int len;
      len = tmp.getLength();
      float *dp = (float*) tmp.getDataPointer();
      float *ptr = (float*) Malloc(sizeof(float)*tmp.getLength());
      for (i=0;i<len;i++)
	ptr[i] = dp[2*i];
      retval = Array(FM_FLOAT,tmp.getDimensions(),ptr);
    } else if (argType == FM_DCOMPLEX) {
      int len;
      len = tmp.getLength();
      double *dp = (double*) tmp.getDataPointer();
      double *ptr = (double*) Malloc(sizeof(double)*tmp.getLength());
      for (i=0;i<len;i++)
	ptr[i] = dp[2*i];
      retval = Array(FM_DOUBLE,tmp.getDimensions(),ptr);    } else
      retval = tmp;
    ArrayVector out;
    out.push_back(retval);
    return out;
  }

  //!
  //@Module IMAG Imaginary Function
  //@@Section ELEMENTARY
  //@@Usage
  //Returns the imaginary part of the input array for all elements.  The 
  //general syntax for its use is
  //@[
  //   y = imag(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.  The output 
  //is the same numerical type as the input, unless the input is @|complex|
  //or @|dcomplex|.  For @|complex| inputs, the imaginary part is a floating
  //point array, so that the return type is @|float|.  For @|dcomplex|
  //inputs, the imaginary part is a double precision floating point array, so that
  //the return type is @|double|.  The @|imag| function returns zeros for 
  //real and integer types.
  //@@Example
  //The following demonstrates @|imag| applied to a complex scalar.
  //@<
  //imag(3+4*i)
  //@>
  //The imaginary part of real and integer arguments is a vector of zeros, the
  //same type and size of the argument.
  //@<
  //imag([2,4,5,6])
  //@>
  //For a double-precision complex array,
  //@<
  //imag([2.0+3.0*i,i])
  //@>
  //!
  ArrayVector ImagFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("imag function requires 1 argument");
    Array tmp(arg[0]);
    if (tmp.isReferenceType())
      throw Exception("argument to imag function must be numeric");
    Class argType(tmp.getDataClass());
    Array retval;
    int i;
    if (argType == FM_COMPLEX) {
      int len;
      len = tmp.getLength();
      float *dp = (float*) tmp.getDataPointer();
      float *ptr = (float*) Malloc(sizeof(float)*tmp.getLength());
      for (i=0;i<len;i++)
	ptr[i] = dp[2*i+1];
      retval = Array(FM_FLOAT,tmp.getDimensions(),ptr);
    } else if (argType == FM_DCOMPLEX) {
      int len;
      len = tmp.getLength();
      double *dp = (double*) tmp.getDataPointer();
      double *ptr = (double*) Malloc(sizeof(double)*tmp.getLength());
      for (i=0;i<len;i++)
	ptr[i] = dp[2*i+1];
      retval = Array(FM_DOUBLE,tmp.getDimensions(),ptr);
    } else {
      retval = tmp;
      int cnt;
      cnt = retval.getByteSize();
      char *dp = (char*) retval.getReadWriteDataPointer();
      memset(dp,0,cnt);
    }
    ArrayVector out;
    out.push_back(retval);
    return out;
  }

  //!
  //@Module ABS Absolute Value Function
  //@@Section ELEMENTARY
  //@@Usage
  //Returns the absolute value of the input array for all elements.  The 
  //general syntax for its use is
  //@[
  //   y = abs(x)
  //@]
  //where @|x| is an @|n|-dimensional array of numerical type.  The output 
  //is the same numerical type as the input, unless the input is @|complex|
  //or @|dcomplex|.  For @|complex| inputs, the absolute value is a floating
  //point array, so that the return type is @|float|.  For @|dcomplex|
  //inputs, the absolute value is a double precision floating point array, so that
  //the return type is @|double|.
  //@@Example
  //The following demonstrates the @|abs| applied to a complex scalar.
  //@<
  //abs(3+4*i)
  //@>
  //The @|abs| function applied to integer and real values:
  //@<
  //abs([-2,3,-4,5])
  //@>
  //For a double-precision complex array,
  //@<
  //abs([2.0+3.0*i,i])
  //@>
  //!
  ArrayVector AbsFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("abs function requires 1 argument");
    Array tmp(arg[0]);
    if (tmp.isReferenceType())
      throw Exception("argument to abs function must be numeric");
    if (tmp.isSparse()) {
      Class rettype;
      if (tmp.getDataClass() == FM_LOGICAL) return singleArrayVector(tmp);
      rettype = tmp.getDataClass();
      if (tmp.getDataClass() == FM_COMPLEX) rettype = FM_FLOAT;
      if (tmp.getDataClass() == FM_DCOMPLEX) rettype = FM_DOUBLE;
      Array retval(rettype,tmp.getDimensions(),
		   SparseAbsFunction(tmp.getDataClass(),
				     tmp.getDimensionLength(0),
				     tmp.getDimensionLength(1),
				     tmp.getSparseDataPointer()),true);
      return singleArrayVector(retval);
    }
    Class argType(tmp.getDataClass());
    Array retval;
    int i;
    switch (argType) {
    case FM_LOGICAL:
    case FM_UINT8:
    case FM_UINT16:
    case FM_UINT32:
      retval = tmp;
      break;
    case FM_INT8:
      {
	int len = tmp.getLength();
	int8 *sp = (int8*) tmp.getDataPointer();
	int8 *op = (int8*) Malloc(sizeof(int8)*len);
	for (i=0;i<len;i++)
	  op[i] = abs(sp[i]);
	retval = Array(FM_INT8,tmp.getDimensions(),op);
      }
      break;
    case FM_INT16:
      {
	int len = tmp.getLength();
	int16 *sp = (int16*) tmp.getDataPointer();
	int16 *op = (int16*) Malloc(sizeof(int16)*len);
	for (i=0;i<len;i++)
	  op[i] = abs(sp[i]);
	retval = Array(FM_INT16,tmp.getDimensions(),op);
      }
      break;
    case FM_INT32:
      {
	int len = tmp.getLength();
	int32 *sp = (int32*) tmp.getDataPointer();
	int32 *op = (int32*) Malloc(sizeof(int32)*len);
	for (i=0;i<len;i++)
	  op[i] = abs(sp[i]);
	retval = Array(FM_INT32,tmp.getDimensions(),op);
      }
      break;
    case FM_FLOAT:
      {
	int len = tmp.getLength();
	float *sp = (float*) tmp.getDataPointer();
	float *op = (float*) Malloc(sizeof(float)*len);
	for (i=0;i<len;i++)
	  op[i] = fabs(sp[i]);
	retval = Array(FM_FLOAT,tmp.getDimensions(),op);
      }
      break;
    case FM_DOUBLE:
      {
	int len = tmp.getLength();
	double *sp = (double*) tmp.getDataPointer();
	double *op = (double*) Malloc(sizeof(double)*len);
	for (i=0;i<len;i++)
	  op[i] = fabs(sp[i]);
	retval = Array(FM_DOUBLE,tmp.getDimensions(),op);
      }
      break;
    case FM_COMPLEX:
      {
	int len = tmp.getLength();
	float *sp = (float*) tmp.getDataPointer();
	float *op = (float*) Malloc(sizeof(float)*len);
	for (i=0;i<len;i++)
	  op[i] = complex_abs(sp[2*i],sp[2*i+1]);
	retval = Array(FM_FLOAT,tmp.getDimensions(),op);
      }
      break;
    case FM_DCOMPLEX:
      {
	int len = tmp.getLength();
	double *sp = (double*) tmp.getDataPointer();
	double *op = (double*) Malloc(sizeof(double)*len);
	for (i=0;i<len;i++)
	  op[i] = complex_abs(sp[2*i],sp[2*i+1]);
	retval = Array(FM_DOUBLE,tmp.getDimensions(),op);
      }
      break;
    }
    ArrayVector out;
    out.push_back(retval);
    return out;
  }

  //!
  //@Module PROD Product Function
  //@@Section ELEMENTARY
  //@@Usage
  //Computes the product of an array along a given dimension.  The general
  //syntax for its use is
  //@[
  //   y = prod(x,d)
  //@]
  //where @|x| is an @|n|-dimensions array of numerical type.
  //The output is of the same numerical type as the input, except 
  //for integer types, which are automatically promoted to @|int32|.
  // The argument @|d| is optional, and denotes the dimension along 
  //which to take the product.  The output is computed via
  //\[
  //  y(m_1,\ldots,m_{d-1},1,m_{d+1},\ldots,m_{p}) = 
  //    \prod_{k} x(m_1,\ldots,m_{d-1},k,m_{d+1},\ldots,m_{p})
  //\]
  //If @|d| is omitted, then the product is taken along the 
  //first non-singleton dimension of @|x|. 
  //@@Example
  //The following piece of code demonstrates various uses of the product
  //function
  //@<
  //A = [5,1,3;3,2,1;0,3,1]
  //@>
  //We start by calling @|prod| without a dimension argument, in which case it defaults to the first nonsingular dimension (in this case, along the columns or @|d = 1|).
  //@<
  //prod(A)
  //@>
  //Next, we take the product along the rows.
  //@<
  //prod(A,2)
  //@>
  //!
  ArrayVector ProdFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("prod requires at least one argument");
    Array input(arg[0]);
    Class argType(input.getDataClass());
    if (input.isReferenceType() || input.isString())
      throw Exception("prod only defined for numeric types");
    if ((argType >= FM_LOGICAL) && (argType < FM_INT32)) {
      input.promoteType(FM_INT32);
      argType = FM_INT32;
    }    
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      Array WDim(arg[1]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to prod should be positive");
    }
    if (input.isEmpty())
      return HandleEmpty(input);
    if (input.isScalar())
      return singleArrayVector(input);
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    outDim[workDim] = 1;
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealProd<int32>((const int32 *) input.getDataPointer(),
		       (int32 *) ptr, planecount, planesize, linesize);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      TRealProd<float>((const float *) input.getDataPointer(),
		       (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      TRealProd<double>((const double *) input.getDataPointer(),
		       (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      TComplexProd<float>((const float *) input.getDataPointer(),
			  (float *) ptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      TComplexProd<double>((const double *) input.getDataPointer(),
			   (double *) ptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
   }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

  //!
  //@Module INT2BIN Convert Integer Arrays to Binary
  //@@Section ARRAY
  //@@Usage
  //Computes the binary decomposition of an integer array to the specified
  //number of bits.  The general syntax for its use is
  //@[
  //   y = int2bin(x,n)
  //@]
  //where @|x| is a multi-dimensional integer array, and @|n| is the number
  //of bits to expand it to.  The output array @|y| has one extra dimension
  //to it than the input.  The bits are expanded along this extra dimension.
  //@@Example
  //The following piece of code demonstrates various uses of the int2bin
  //function.  First the simplest example:
  //@<
  //A = [2;5;6;2]
  //int2bin(A,8)
  //A = [1,2;-5;2]
  //int2bin(A,8)
  //@>
  //!
  ArrayVector Int2BinFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
      throw Exception("int2bin requires at least two arguments");
    Array x(arg[0]);
    x.promoteType(FM_UINT32);
    Array n(arg[1]);
    int numbits;
    numbits = n.getContentsAsIntegerScalar();
    if (numbits<1)
      numbits = 1;
    Dimensions xdim(x.getDimensions());
    int slicesize;
    slicesize = xdim.getElementCount();
    xdim.simplify();
    if (xdim[xdim.getLength()-1] == 1)
      xdim[xdim.getLength()-1] = numbits;
    else
      xdim[xdim.getLength()] = numbits;
    logical *dp;
    dp = (logical *) Malloc(sizeof(logical)*xdim.getElementCount());
    int32 *sp;
    sp = (int32 *) x.getDataPointer();
    int i, j;
    for (i=0;i<slicesize;i++) {
      int32 v;
      v = sp[i];
      for (j=0;j<numbits;j++) {
	dp[(numbits-1-j)*slicesize+i] = v & 1;
	v >>= 1;
      }
    }
    ArrayVector retval;
    retval.push_back(Array(FM_LOGICAL,xdim,dp));
    return retval;
  }

  //!
  //@Module BIN2INT Convert Binary Arrays to Integer
  //@@Section ARRAY
  //@@Usage
  //Converts the binary decomposition of an integer array back
  //to an integer array.  The general syntax for its use is
  //@[
  //   y = bin2int(x)
  //@]
  //where @|x| is a multi-dimensional logical array, where the last
  //dimension indexes the bit planes (see @|int2bin| for an example).
  //By default, the output of @|bin2int| is unsigned @|uint32|.  To
  //get a signed integer, it must be typecast correctly.
  //@@Example
  //The following piece of code demonstrates various uses of the int2bin
  //function.  First the simplest example:
  //@<
  //A = [2;5;6;2]
  //B = int2bin(A,8)
  //bin2int(B)
  //A = [1,2;-5;2]
  //B = int2bin(A,8)
  //bin2int(B)
  //int32(bin2int(B))
  //@>
  //!
  ArrayVector Bin2IntFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("bin2int requires at least one arguments");
    Array x(arg[0]);
    x.promoteType(FM_LOGICAL);
    bool signflag;
    signflag = false;
    if (arg.size() > 1) {
      Array flag(arg[1]);
      char *flag_value = flag.getContentsAsCString();
      if (strcmp(flag_value,"signed") == 0)
	signflag = true;
    }
    Dimensions xdim(x.getDimensions());
    int numbits;
    numbits = xdim[xdim.getLength()-1];
    int slicesize;
    slicesize = xdim.getElementCount()/numbits;
    xdim[xdim.getLength()-1] = 1;
    xdim.simplify();
    ArrayVector retval;
    uint32 *dp;
    dp = (uint32*) Malloc(sizeof(uint32)*xdim.getElementCount());
    logical *sp;
    sp = (logical*) x.getDataPointer();
    int i, j;
    for (i=0;i<slicesize;i++) {
      uint32 v;
      v = sp[i];
      for (j=1;j<numbits;j++) {
	v <<= 1;
	v |= sp[j*slicesize+i];
      }
      dp[i] = v;
    }
    retval.push_back(Array(FM_UINT32,xdim,dp));
    return retval;
  }

  //!
  //@Module PCODE Convert a Script or Function to P-Code
  //@@Section FREEMAT
  //@@Usage
  //Writes out a script or function as a P-code function.
  //The general syntax for its use is:
  //@[
  //   pcode fun1 fun2 ...
  //@]
  //The compiled functions are written to the current
  //directory.
  //!
  ArrayVector PCodeFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    int i;
    for (i=0;i<arg.size();i++) {
      Array func(arg[i]);
      if (!func.isString())
	throw Exception("arguments to pcode must be function names");
      char *fname = func.getContentsAsCString();
      FunctionDef *funcDef;
      ArrayVector m;
      bool isFun;
      char buffer[1024];
      char buffer2[1024];
      int n;
      n = strlen(fname);
      isFun = eval->lookupFunction(fname,funcDef,m);
      if ((n>3) && (fname[n-1] == 'm' || fname[n-1] == 'M')
	  && (fname[n-2] == '.')) {
	fname[n-2] = 0;
	isFun = eval->lookupFunction(fname,funcDef,m);
      }
      if (!isFun) {
	sprintf(buffer,"could not find definition for %s",fname);
	eval->getInterface()->warningMessage(buffer);
      } else {
	sprintf(buffer,"Translating %s to P-Code\n",fname);
	eval->getInterface()->outputMessage(buffer);
	funcDef->updateCode();
	if (funcDef->type() != FM_M_FUNCTION) {
	  sprintf(buffer,"function %s is not an M-file",fname);
	  eval->getInterface()->warningMessage(buffer);
	}
	sprintf(buffer2,"%s.p",fname);
	File *stream = new File(buffer2,"wb");
	Serialize *s = new Serialize(stream);
	s->handshakeServer();
	s->sendSignature('p',1);
	FreezeMFunction((MFunctionDef*)funcDef,s);
	delete s;
	delete stream;
      }
    }
    return ArrayVector();
  }

  bool sortreverse;
  
  template <class T>
  class XNEntry {
  public:
    uint32 n;
    T x;
  };
  
  template <class T>
  bool operator<(const XNEntry<T>& a, const XNEntry<T>& b) {
    if (!sortreverse) {
      if (IsNaN(b.x) & !IsNaN(a.x)) return true;
      return (a.x < b.x);
    } else {
      if (IsNaN(a.x) & !IsNaN(b.x)) return true;
      return (b.x < a.x);
    }
  }

  template <class T>
  void TRealSort(const T* sp, T* dp, int32 *ip, int planes, int planesize, int linesize) {
    XNEntry<T> *buf = new XNEntry<T>[linesize];
    int i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	for (k=0;k<linesize;k++) {
	  buf[k].x = sp[i*planesize*linesize + j + k*planesize];
	  buf[k].n = k+1;
	}
	std::sort(buf,buf+linesize);
	for (k=0;k<linesize;k++) {
	  dp[i*planesize*linesize + j + k*planesize] = buf[k].x;
	  ip[i*planesize*linesize + j + k*planesize] = buf[k].n;
	}
      }    
    }
    delete[] buf;
  }
  
  template <class T>
  class XNComplexEntry {
  public:
    uint32 n;
    T xr;
    T xi;
  };
  
  template <class T>
  bool operator<(const XNComplexEntry<T>& a, const XNComplexEntry<T>& b) {
    T a_abs, b_abs;
    if (!sortreverse) {
      a_abs = complex_abs(a.xr,a.xi);
      b_abs = complex_abs(b.xr,b.xi);
      if (IsNaN(b_abs) & !IsNaN(a_abs)) return true;
      if (a_abs < b_abs)
	return true;
      else if (a_abs == b_abs)
	return atan2(a.xi,a.xr) < atan2(b.xi,b.xr);
      else
	return false;
    } else {
      a_abs = complex_abs(b.xr,b.xi);
      b_abs = complex_abs(a.xr,a.xi);
      if (IsNaN(b_abs) & !IsNaN(a_abs)) return true;
      if (a_abs < b_abs)
	return true;
      else if (a_abs == b_abs)
	return atan2(b.xi,b.xr) < atan2(a.xi,a.xr);
      else
	return false;
    }
  }
  
  template <class T>
  void TComplexSort(const T* sp, T* dp, int32 *ip, int planes, int planesize, int linesize) {
    XNComplexEntry<T> *buf = new XNComplexEntry<T>[linesize];
    int i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	for (k=0;k<linesize;k++) {
	  buf[k].xr = sp[2*(i*planesize*linesize + j + k*planesize)];
	  buf[k].xi = sp[2*(i*planesize*linesize + j + k*planesize)+1];
	  buf[k].n = k+1;
	}
	std::sort(buf,buf+linesize);
	for (k=0;k<linesize;k++) {
	  dp[2*(i*planesize*linesize + j + k*planesize)] = buf[k].xr;
	  dp[2*(i*planesize*linesize + j + k*planesize)+1] = buf[k].xi;
	  ip[i*planesize*linesize + j + k*planesize] = buf[k].n;
	}
      }    
    }
    delete[] buf;
  }

  bool VerifyAllStrings(Array *ptr, int count) {
    bool allStrings;
    int i;
    i = 0;
    allStrings = true;
    while (allStrings && (i < count)) {
      allStrings = allStrings && (ptr[i].isString() || ptr[i].isEmpty());
      i++;
    }
    return(allStrings);
  }

  class XSEntry {
  public:
    uint32 n;
    char* x;
  };
  
  bool operator<(const XSEntry& a, const XSEntry& b) {
    if (!sortreverse) 
      return (strcmp(a.x,b.x) < 0);
    else
      return (strcmp(b.x,a.x) < 0);
  }

  bool operator==(const XSEntry& a, const XSEntry& b) {
    return (strcmp(a.x,b.x) == 0);
  }

  void StringSort(const Array* sp, Array* dp, int32 *ip, 
		  int planes, int planesize, int linesize) {
    XSEntry *buf = new XSEntry[linesize];
    int i, j, k;
    
    for (i=0;i<planes;i++) {
      for (j=0;j<planesize;j++) {
	for (k=0;k<linesize;k++) {
	  buf[k].x = sp[i*planesize*linesize + j + k*planesize].getContentsAsCString();
	  buf[k].n = k+1;
	}
	std::sort(buf,buf+linesize);
	for (k=0;k<linesize;k++) {
	  dp[i*planesize*linesize + j + k*planesize] = 
	    sp[i*planesize*linesize + j + (buf[k].n-1)*planesize];
	  ip[i*planesize*linesize + j + k*planesize] = buf[k].n;
	  Free(buf[k].x);
	}
      }    
    }
    delete[] buf;
  }

  template <class T>
  class UniqueEntryReal {
  public:
    uint32 n;
    uint32 len;
    uint32 stride;
    const T* data;
  };

  template <class T>
  bool operator<(const UniqueEntryReal<T>& a, const UniqueEntryReal<T>& b) {
    int i;
    i = 0;
    while (i<a.len) {
      if (a.data[i*a.stride] < b.data[i*b.stride]) 
	return true; 
      else if (a.data[i*a.stride] > b.data[i*b.stride]) 
	return false; 
      i++;
    }
    return false;
  }
  
  template <class T>
  bool operator==(const UniqueEntryReal<T>& a, const UniqueEntryReal<T>& b) {
    int i;
    i = 0;
    while (i<a.len) {
      if (a.data[i*a.stride] != b.data[i*b.stride]) return false;
      i++;
    }
    return true;
  }
  
  template <class T>
  class UniqueEntryComplex {
  public:
    uint32 n;
    uint32 len;
    uint32 stride;
    const T* data;
  };

  template <class T>
  bool operator<(const UniqueEntryComplex<T>& a, const UniqueEntryComplex<T>& b) {
    int i;
    T a_abs, b_abs;
    i = 0;
    while (i<a.len) {
      a_abs = complex_abs(a.data[2*i*a.stride],a.data[2*i*a.stride+1]);
      b_abs = complex_abs(b.data[2*i*b.stride],b.data[2*i*b.stride+1]);
      if (a_abs < b_abs) 
	return true;
      else if (a_abs > b_abs)
	return false;
      i++;
    }
    return false;
  }
  
  template <class T>
  bool operator==(const UniqueEntryComplex<T>& a, const UniqueEntryComplex<T>& b) {
    int i;
    i = 0;
    while (i<a.len) {
      if ((a.data[2*i*a.stride] != b.data[2*i*b.stride]) || 
	  (a.data[2*i*a.stride+1] != b.data[2*i*b.stride+1])) return false;
      i++;
    }
    return true;
  }
  
  
  //!
  //@Module UNIQUE Unique
  //@@Section ARRAY
  //@@Usage
  //Returns a vector containing the unique elements of an array.  The first
  //form is simply
  //@[
  //   y = unique(x)
  //@]
  //where @|x| is either a numerical array or a cell-array of strings.  The 
  //result is sorted in increasing order.  You can also retrieve two sets
  //of index vectors
  //@[
  //   [y, m, n] = unique(x)
  //@]
  //such that @|y = x(m)| and @|x = y(n)|.  If the argument @|x| is a matrix,
  //you can also indicate that FreeMat should look for unique rows in the
  //matrix via
  //@[
  //   y = unique(x,'rows')
  //@]
  //and
  //@[
  //   [y, m, n] = unique(x,'rows')
  //@]
  //@@Example
  //Here is an example in row mode
  //@<
  //A = randi(1,3*ones(15,3))
  //unique(A,'rows')
  //[b,m,n] = unique(A,'rows');
  //b
  //A(m,:)
  //b(n,:)
  //@>
  //Here is an example in vector mode
  //@<
  //A = randi(1,5*ones(10,1))
  //unique(A)
  //[b,m,n] = unique(A,'rows');
  //b
  //A(m)
  //b(n)
  //@>
  //For cell arrays of strings.
  //@<
  //A = {'hi','bye','good','tell','hi','bye'}
  //unique(A)
  //@>
  //!

  template <class T>
  ArrayVector UniqueFunctionRowModeComplex(int nargout, Array& input) {
    const T* dp = (const T*) input.getDataPointer();
    int rows = input.getDimensionLength(0);
    int cols = input.getDimensionLength(1); 
    int len = rows;
    Class cls(input.getDataClass());
    int i, j;
    int cnt;
    UniqueEntryComplex<T> *sp = new UniqueEntryComplex<T>[len];
    for (i=0;i<len;i++) {
      sp[i].n = i;
      sp[i].len = cols;
      sp[i].stride = rows;
      sp[i].data = dp + 2*i;
    }
    std::sort(sp,sp+len);
    i = 1;
    cnt = 1;
    while (i < len) {
      if (!(sp[i] == sp[i-1]))
	cnt++;
      i++;
    }
    int tcnt = cnt;
    if (nargout <= 1) {
      T* op = (T*) Malloc(sizeof(T)*cnt*2*cols);
      for (j=0;j<cols;j++) {
	op[0+j*2*tcnt] = sp[0].data[0+j*2*rows];
	op[1+j*2*tcnt] = sp[0].data[1+j*2*rows];
      }
      i = 1;
      cnt = 1;
      while (i < len) {
	if (!(sp[i] == sp[i-1])) {
	  for (j=0;j<cols;j++) {
	    op[2*cnt+j*2*tcnt] = sp[i].data[0+j*2*rows];
	    op[2*cnt+j*2*tcnt+1] = sp[i].data[1+j*2*rows];
	  }
	  cnt++;
	}
	i++;
      }
      delete[] sp;
      return singleArrayVector(Array(cls,Dimensions(cnt,cols),op));
    } else {
      uint32* np = (uint32*) Malloc(sizeof(int32)*len);
      uint32* mp = (uint32*) Malloc(sizeof(int32)*cnt);
      T* op = (T*) Malloc(sizeof(T)*cnt*2*cols);
      for (j=0;j<cols;j++) {
	op[0+j*2*tcnt] = sp[0].data[0+j*2*rows];
	op[1+j*2*tcnt] = sp[0].data[1+j*2*rows];
      }
      i = 1;
      cnt = 1;
      np[sp[0].n] = 1;
      mp[0] = sp[0].n + 1;
      while (i < len) {
	if (!(sp[i] == sp[i-1])) {
	  for (j=0;j<cols;j++) {
	    op[2*cnt+j*2*tcnt] = sp[i].data[0+j*2*rows];
	    op[2*cnt+j*2*tcnt+1] = sp[i].data[1+j*2*rows];
	  }
	  mp[cnt] = sp[i].n + 1;
	  cnt++;
	}
	np[sp[i].n] = cnt;
	i++;
      }
      delete[] sp;
      ArrayVector retval;
      retval.push_back(Array(cls,Dimensions(cnt,cols),op));
      retval.push_back(Array(FM_UINT32,Dimensions(cnt,1),mp));
      retval.push_back(Array(FM_UINT32,Dimensions(len,1),np));
      return retval;
    }
  }

  template <class T>
  ArrayVector UniqueFunctionRowModeReal(int nargout, Array& input) {
    const T* dp = (const T*) input.getDataPointer();
    int rows = input.getDimensionLength(0);
    int cols = input.getDimensionLength(1); 
    int len = rows;
    Class cls(input.getDataClass());
    int i, j;
    int cnt;
    UniqueEntryReal<T> *sp = new UniqueEntryReal<T>[len];
    for (i=0;i<len;i++) {
      sp[i].n = i;
      sp[i].len = cols;
      sp[i].stride = rows;
      sp[i].data = dp + i;
    }
    std::sort(sp,sp+len);
    i = 1;
    cnt = 1;
    while (i < len) {
      if (!(sp[i] == sp[i-1]))
	cnt++;
      i++;
    }
    int tcnt = cnt;
    if (nargout <= 1) {
      T* op = (T*) Malloc(sizeof(T)*cnt*cols);
      for (j=0;j<cols;j++)
	op[0+j*tcnt] = sp[0].data[0+j*rows];
      i = 1;
      cnt = 1;
      while (i < len) {
	if (!(sp[i] == sp[i-1])) {
	  for (j=0;j<cols;j++)
	    op[cnt+j*tcnt] = sp[i].data[0+j*rows];
	  cnt++;
	}
	i++;
      }
      delete[] sp;
      return singleArrayVector(Array(cls,Dimensions(cnt,cols),op));
    } else {
      uint32* np = (uint32*) Malloc(sizeof(int32)*len);
      uint32* mp = (uint32*) Malloc(sizeof(int32)*cnt);
      T* op = (T*) Malloc(sizeof(T)*cnt*cols); 
      for (j=0;j<cols;j++)
	op[0+j*tcnt] = sp[0].data[0+j*rows];
      i = 1;
      cnt = 1;
      np[sp[0].n] = 1;
      mp[0] = sp[0].n + 1;
      while (i < len) {
	if (!(sp[i] == sp[i-1])) {
	  for (j=0;j<cols;j++)
	    op[cnt+j*tcnt] = sp[i].data[0+j*rows];
	  mp[cnt] = sp[i].n + 1;
	  cnt++;
	}
	np[sp[i].n] = cnt;
	i++;
      }
      delete[] sp;
      ArrayVector retval;
      retval.push_back(Array(cls,Dimensions(cnt,cols),op));
      retval.push_back(Array(FM_UINT32,Dimensions(cnt,1),mp));
      retval.push_back(Array(FM_UINT32,Dimensions(len,1),np));
      return retval;
    }
  }

  ArrayVector UniqueFunctionString(int nargout, Array& input) {
    int len(input.getLength());
    if (!VerifyAllStrings((Array*) input.getDataPointer(),len))
      throw Exception("when 'unique' is applied to cell arrays, each cell must contain a string");
    XSEntry *buf = new XSEntry[len];
    Array *sp = (Array*) input.getDataPointer();
    int i;
    for (i=0;i<len;i++) {
      buf[i].x = sp[i].getContentsAsCString();
      buf[i].n = i;
    }
    sortreverse = false;
    std::sort(buf,buf+len);
    i = 1;
    int cnt = 1;
    while (i < len) {
      if (!(buf[i] == buf[i-1]))
	cnt++;
      i++;
    }
    int tcnt = cnt;
    if (nargout <= 1) {
      Array *op = new Array[cnt];
      op[0] = sp[buf[0].n];
      i = 1;
      cnt = 1;
      while (i < len) {
	if (!(buf[i] == buf[i-1])) {
	  op[cnt] = sp[buf[i].n];
	  cnt++;
	}
	i++;
      }
      delete[] buf;
      return singleArrayVector(Array(FM_CELL_ARRAY,Dimensions(cnt,1),op));
    } else {
      uint32* np = (uint32*) Malloc(sizeof(int32)*len);
      uint32* mp = (uint32*) Malloc(sizeof(int32)*cnt);
      Array *op = new Array[cnt];
      op[0] = sp[buf[0].n];
      i = 1;
      cnt = 1;
      np[buf[0].n] = 1;
      mp[0] = buf[0].n + 1;
      while (i < len) {
	if (!(buf[i] == buf[i-1])) {
	  op[cnt] = sp[buf[i].n];
	  mp[cnt] = buf[i].n + 1;
	  cnt++;
	}
	np[buf[i].n] = cnt;
	i++;
      }
      delete[] buf;
      ArrayVector retval;
      retval.push_back(Array(FM_CELL_ARRAY,Dimensions(cnt,1),op));
      retval.push_back(Array(FM_UINT32,Dimensions(cnt,1),mp));
      retval.push_back(Array(FM_UINT32,Dimensions(len,1),np));
      return retval;
    }
  }
  
  ArrayVector UniqueFunctionAux(int nargout, Array input, bool rowmode) {
    if ((input.getDataClass() == FM_CELL_ARRAY) || (!rowmode)) {
      Dimensions newdim(input.getLength(),1);
      input.reshape(newdim);
    }
    Class argType(input.getDataClass());
    switch (argType) {
    case FM_INT8: 
      return UniqueFunctionRowModeReal<int8>(nargout, input);
    case FM_UINT8:
      return UniqueFunctionRowModeReal<uint8>(nargout, input);
    case FM_INT16: 
      return UniqueFunctionRowModeReal<int16>(nargout, input);
    case FM_UINT16:
      return UniqueFunctionRowModeReal<uint16>(nargout, input);
    case FM_INT32: 
      return UniqueFunctionRowModeReal<int32>(nargout, input);
    case FM_UINT32:
      return UniqueFunctionRowModeReal<uint32>(nargout, input);
    case FM_FLOAT: 
      return UniqueFunctionRowModeReal<float>(nargout, input);
    case FM_DOUBLE:
      return UniqueFunctionRowModeReal<double>(nargout, input);
    case FM_COMPLEX: 
      return UniqueFunctionRowModeComplex<float>(nargout, input);
    case FM_DCOMPLEX:
      return UniqueFunctionRowModeComplex<double>(nargout, input);
    case FM_CELL_ARRAY:
      return UniqueFunctionString(nargout, input);
    }
    throw Exception("Unsupported type in call to unique");
    return ArrayVector();
  }

  ArrayVector UniqueFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("unique function requires at least one argument");
    Array input(arg[0]);
    if (input.isEmpty()) {
      if (nargout <= 1)
	return singleArrayVector(Array::emptyConstructor());
      else {
	ArrayVector retval;
	retval.push_back(Array::emptyConstructor());
	retval.push_back(Array::emptyConstructor());
	retval.push_back(Array::emptyConstructor());
	return retval;
      }
    }
    bool rowmode = false;
    if (arg.size() == 2) {
      Array Sdir(arg[1]);
      if (!Sdir.isString())
	throw Exception("second argument to unique must be 'rows'");
      const char *dp = (const char*) Sdir.getDataPointer();
      if ((dp[0] == 'r') || (dp[0] == 'R'))
	rowmode = true;
      else
	throw Exception("second argument to unique must be 'rows'");
    }
    // Get the input dimensions
    Dimensions inDim(input.getDimensions());
    if (rowmode && (inDim.getLength() != 2))
      throw Exception("'rows' mode only works for matrix (2D) arguments");
    return UniqueFunctionAux(nargout, input, rowmode);
  }

  //!
  //@Module TIC Start Stopwatch Timer
  //@@Section FREEMAT
  //@@Usage
  //Starts the stopwatch timer, which can be used to time tasks in FreeMat.
  //The @|tic| takes no arguments, and returns no outputs.  You must use
  //@|toc| to get the elapsed time.  The usage is
  //@[
  //  tic
  //@]
  //@@Example
  //Here is an example of timing the solution of a large matrix equation.
  //@<
  //A = rand(100);
  //b = rand(100,1);
  //tic; c = A\b; toc
  //@>
  //!
  
  static double ticvalue = 0;

  ArrayVector TicFunction(int nargout, const ArrayVector& arg) {
    ticvalue = getcurrenttime();
    return ArrayVector();
  }

  //!
  //@Module CLOCK Get Current Time
  //@@Section FreeMat
  //@@Usage
  //Returns the current date and time as a vector.  The syntax for its use is
  //@[
  //   y = clock
  //@]
  //where @|y| has the following format:
  //@[
  //   y = [year month day hour minute seconds]
  //@]
  //@@Example
  //Here is the time that this manual was last built:
  //@<
  //clock
  //@>
  //!
  ArrayVector ClockFunction(int nargout, const ArrayVector& arg) {
    struct tm *breakdown;
    time_t timeval;
    double curtime = getcurrenttime();
    timeval = (time_t) (curtime/1e6);
    breakdown = localtime(&timeval);
    Array retvec(Array::doubleVectorConstructor(6));
    double *dp = (double*) retvec.getReadWriteDataPointer();
    dp[0] = breakdown->tm_year+1900;
    dp[1] = breakdown->tm_mon+1;
    dp[2] = breakdown->tm_mday+1;
    dp[3] = breakdown->tm_hour;
    dp[4] = breakdown->tm_min;
    dp[5] = breakdown->tm_sec + curtime/1e6 - timeval;
    return singleArrayVector(retvec);
  }

  //!
  //@Module CLOCKTOTIME Convert Clock Vector to Epoch Time
  //@@Section FreeMat
  //@@Usage
  //Given the output of the @|clock| command, this function computes
  //the epoch time, i.e, the time in seconds since January 1,1970 
  //at 00:00:00 UTC.  This function is most useful for calculating elapsed
  //times using the clock, and should be accurate to less than a millisecond
  //(although the true accuracy depends on accuracy of the argument vector). 
  //The usage for @|clocktotime| is
  //@[
  //   y = clocktotime(x)
  //@]
  //where @|x| must be in the form of the output of @|clock|, that is
  //@[
  //   x = [year month day hour minute seconds]
  //@]
  //@@Example
  //Here is an example of using @|clocktotime| to time a delay of 1 second
  //@<
  //x = clock
  //sleep(1)
  //y = clock
  //clocktotime(y) - clocktotime(x)
  //@>
  //!
  ArrayVector ClockToTimeFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
    Array targ(arg[0]);
    targ.promoteType(FM_DOUBLE);
    if (targ.getLength() != 6)
      throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
    const double *dp = (const double*) targ.getDataPointer();
    struct tm breakdown;
    breakdown.tm_year = dp[0] - 1900;
    breakdown.tm_mon = dp[1] - 1;
    breakdown.tm_mday = dp[2] - 1;
    breakdown.tm_hour = dp[3];
    breakdown.tm_min = dp[4];
    breakdown.tm_sec = (int) dp[5];
    time_t qtime = mktime(&breakdown);
    double retval;
    retval = qtime + (dp[5] - (int) dp[5]);
    return singleArrayVector(Array::doubleConstructor(retval));
  }
  

  //!
  //@Module TOC Start Stopwatch Timer
  //@@Section FREEMAT
  //@@Usage
  //Starts the stopwatch timer, which can be used to time tasks in FreeMat.
  //The @|tic| takes no arguments, and returns no outputs.  You must use
  //@|toc| to get the elapsed time.  The usage is
  //@[
  //  tic
  //@]
  //@@Example
  //Here is an example of timing the solution of a large matrix equation.
  //@<
  //A = rand(100);
  //b = rand(100,1);
  //tic; c = A\b; toc
  //@>
  //!
  
  ArrayVector TocFunction(int nargout, const ArrayVector& arg) {
    double outtime = (getcurrenttime() - ticvalue)/1e6;
    return singleArrayVector(Array::doubleConstructor(outtime));
  }

  //!
  //@Module SORT Sort 
  //@@Section ARRAY
  //@@Usage
  //Sorts an n-dimensional array along the specified dimensional.  The first
  //form sorts the array along the first non-singular dimension.
  //@[
  //  B = sort(A)
  //@]
  //Alternately, the dimension along which to sort can be explicitly specified
  //@[
  //  B = sort(A,dim)
  //@]
  //FreeMat does not support vector arguments for @|dim| - if you need @|A| to be
  //sorted along multiple dimensions (i.e., row first, then columns), make multiple
  //calls to @|sort|.  Also, the direction of the sort can be specified using the 
  //@|mode| argument
  //@[
  //  B = sort(A,dim,mode)
  //@]
  //where @|mode = 'ascend'| means to sort the data in ascending order (the default),
  //and @|mode = 'descend'| means to sort the data into descending order.  
  //
  //When two outputs are requested from @|sort|, the indexes are also returned.
  //Thus, for 
  //@[
  //  [B,IX] = sort(A)
  //  [B,IX] = sort(A,dim)
  //  [B,IX] = sort(A,dim,mode)
  //@]
  //an array @|IX| of the same size as @|A|, where @|IX| records the indices of @|A|
  //(along the sorting dimension) corresponding to the output array @|B|. 
  //
  //Two additional issues worth noting.  First, a cell array can be sorted if each 
  //cell contains a @|string|, in which case the strings are sorted by lexical order.
  //The second issue is that FreeMat uses the same method as MATLAB to sort complex
  //numbers.  In particular, a complex number @|a| is less than another complex
  //number @|b| if @|abs(a) < abs(b)|.  If the magnitudes are the same then we 
  //test the angle of @|a|, i.e. @|angle(a) < angle(b)|, where @|angle(a)| is the
  //phase of @|a| between @|-pi,pi|.
  //@@Example
  //Here are some examples of sorting on numerical arrays.
  //@<
  //A = int32(10*rand(4,3))
  //[B,IX] = sort(A)
  //[B,IX] = sort(A,2)
  //[B,IX] = sort(A,1,'descend')
  //@>
  //Here we sort a cell array of strings.
  //@<
  //a = {'hello','abba','goodbye','jockey','cake'}
  //b = sort(a)
  //@>
  //!
  ArrayVector SortFunction(int nargout, const ArrayVector& arg) {
    // Get the data argument
    if (arg.size() < 1)
      throw Exception("sort requires at least one argument");
    Array input(arg[0]);
    if (input.isScalar()) return singleArrayVector(input);
    Class argType(input.getDataClass());
    // Get the dimension argument (if supplied)
    int workDim = -1;
    if (arg.size() > 1) {
      Array WDim(arg[1]);
      workDim = WDim.getContentsAsIntegerScalar() - 1;
      if (workDim < 0)
	throw Exception("Dimension argument to sort should be positive");
    }
    // Get the sort direction (if supplied)
    int sortdir = 1;
    if (arg.size() > 2) {
      Array Sdir(arg[2]);
      if (!Sdir.isString())
	throw Exception("Sort direction must be either the string 'ascend' or 'descend'");
      const char *dp = (const char*) Sdir.getDataPointer();
      if ((dp[0] == 'd') || (dp[0] == 'D'))
	sortdir = -1;
      else if ((dp[0] == 'a') || (dp[0] == 'A'))
	sortdir = 1;
      else
	throw Exception("Sort direction must be either the string 'ascend' or 'descend'");
    }
    sortreverse = (sortdir == -1);
    // Determine the type of the sort
    if (input.isEmpty()) {
      ArrayVector ret;
      ret.push_back(Array::emptyConstructor());
      for (int n=1;n<nargout;n++)
	ret.push_back(Array::emptyConstructor());
      return ret;
    }
    // No dimension supplied, look for a non-singular dimension
    Dimensions inDim(input.getDimensions());
    if (workDim == -1) {
      int d = 0;
      while (inDim[d] == 1) 
	d++;
      workDim = d;      
    }
    // Calculate the output size
    Dimensions outDim(inDim);
    // Calculate the stride...
    int d;
    int planecount;
    int planesize;
    int linesize;
    linesize = inDim[workDim];
    planesize = 1;
    for (d=0;d<workDim;d++)
      planesize *= inDim[d];
    planecount = 1;
    for (d=workDim+1;d<inDim.getLength();d++)
      planecount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval, ndxval;
    // Sort with index information
    switch (argType) {
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<int8>((const int8 *) input.getDataPointer(),
			 (int8 *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_INT8,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<uint8>((const uint8 *) input.getDataPointer(),
			 (uint8 *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT8,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_STRING: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<uint8>((const uint8 *) input.getDataPointer(),
			 (uint8 *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_STRING,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<int16>((const int16 *) input.getDataPointer(),
			 (int16 *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_INT16,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<uint16>((const uint16 *) input.getDataPointer(),
			 (uint16 *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT16,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<int32>((const int32 *) input.getDataPointer(),
			 (int32 *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_INT32,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<uint32>((const uint32 *) input.getDataPointer(),
			 (uint32 *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_UINT32,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<float>((const float *) input.getDataPointer(),
			 (float *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_FLOAT,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TRealSort<double>((const double *) input.getDataPointer(),
			  (double *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_DOUBLE,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TComplexSort<float>((const float *) input.getDataPointer(),
			    (float *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_COMPLEX,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      char* iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      TComplexSort<double>((const double *) input.getDataPointer(),
			     (double *) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_CELL_ARRAY: {
      // Make sure the source array is all strings
      if (!VerifyAllStrings((Array*) input.getDataPointer(),outDim.getElementCount()))
	throw Exception("Cannot sort a cell array if all the entries are not strings");
      Array* ptr = new Array[outDim.getElementCount()];
      char *iptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      StringSort((const Array*) input.getDataPointer(),
		 (Array*) ptr, (int32 *) iptr, planecount, planesize, linesize);
      retval = Array(FM_CELL_ARRAY,outDim,ptr);
      ndxval = Array(FM_INT32,outDim,iptr);
      break;
    }
    case FM_STRUCT_ARRAY:
      throw Exception("Cannot sort a structure array");
    }
    ArrayVector retArray;
    retArray.push_back(retval);
    if (nargout == 2)
      retArray.push_back(ndxval);
    return retArray;
  }

}
