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
#include "Utils.hpp"
#include <math.h>

namespace FreeMat {

  /**
   * Less-than function for logical arguments.
   */
  void logicalLess(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const logical *spx, *spy;
    logical *dp;
    uint32 i;

    spx = (const logical*) srcx;
    spy = (const logical*) srcy;
    dp = (logical*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for uint8 arguments.
   */
  void uint8Less(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const uint8 *spx, *spy;
    uint8 *dp;
    uint32 i;

    spx = (const uint8*) srcx;
    spy = (const uint8*) srcy;
    dp = (uint8*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for int8 arguments.
   */
  void int8Less(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const int8 *spx, *spy;
    int8 *dp;
    uint32 i;

    spx = (const int8*) srcx;
    spy = (const int8*) srcy;
    dp = (int8*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for uint16 arguments.
   */
  void uint16Less(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const uint16 *spx, *spy;
    uint16 *dp;
    uint32 i;

    spx = (const uint16*) srcx;
    spy = (const uint16*) srcy;
    dp = (uint16*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for int16 arguments.
   */
  void int16Less(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const int16 *spx, *spy;
    int16 *dp;
    uint32 i;

    spx = (const int16*) srcx;
    spy = (const int16*) srcy;
    dp = (int16*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for uint32 arguments.
   */
  void uint32Less(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const uint32 *spx, *spy;
    uint32 *dp;
    uint32 i;

    spx = (const uint32*) srcx;
    spy = (const uint32*) srcy;
    dp = (uint32*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for int32 arguments.
   */
  void int32Less(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const int32 *spx, *spy;
    int32 *dp;
    uint32 i;

    spx = (const int32*) srcx;
    spy = (const int32*) srcy;
    dp = (int32*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for float arguments.
   */
  void floatLess(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const float *spx, *spy;
    float *dp;
    uint32 i;

    spx = (const float*) srcx;
    spy = (const float*) srcy;
    dp = (float*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for double arguments.
   */
  void doubleLess(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const double *spx, *spy;
    double *dp;
    uint32 i;

    spx = (const double*) srcx;
    spy = (const double*) srcy;
    dp = (double*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] < spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Less-than function for complex arguments.
   */
  void complexLess(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const float *spx, *spy;
    float *dp;
    uint32 i;
    float xmag, ymag;

    spx = (const float*) srcx;
    spy = (const float*) srcy;
    dp = (float*) dst;
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
   * Less-than function for dcomplex arguments.
   */
  void dcomplexLess(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const double *spx, *spy;
    double *dp;
    uint32 i;
    double xmag, ymag;

    spx = (const double*) srcx;
    spy = (const double*) srcy;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      xmag = dcomplex_abs(spx[2*stridex*i],spx[2*stridex*i+1]);
      ymag = dcomplex_abs(spy[2*stridey*i],spy[2*stridey*i+1]);
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
   * Minimum function for logical arguments.
   */
  void logicalMin(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const logical *sp;
    logical *dp;
    logical minval;
    uint32 mindex;
    uint32 i, j;

    sp = (const logical*) src;
    dp = (logical*) dst;
    for (i=0;i<count;i++) {
      minval = sp[majstride*i];
      mindex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] < minval) {
	  minval = sp[majstride*i+stride*j];
	  mindex = j;
	}
      }
      dp[i] = minval;
      iptr[i] = mindex + 1;
    }
  }

  /**
   * Minimum function for uint8 arguments.
   */
  void uint8Min(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const uint8 *sp;
    uint8 *dp;
    uint8 minval;
    uint32 mindex;
    uint32 i, j;

    sp = (const uint8*) src;
    dp = (uint8*) dst;
    for (i=0;i<count;i++) {
      minval = sp[majstride*i];
      mindex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] < minval) {
	  minval = sp[majstride*i+stride*j];
	  mindex = j;
	}
      }
      dp[i] = minval;
      iptr[i] = mindex + 1;
    }
  }

  /**
   * Minimum function for int8 arguments.
   */
  void int8Min(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const int8 *sp;
    int8 *dp;
    int8 minval;
    uint32 mindex;
    uint32 i, j;

    sp = (const int8*) src;
    dp = (int8*) dst;
    for (i=0;i<count;i++) {
      minval = sp[majstride*i];
      mindex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] < minval) {
	  minval = sp[majstride*i+stride*j];
	  mindex = j;
	}
      }
      dp[i] = minval;
      iptr[i] = mindex + 1;
    }
  }

  /**
   * Minimum function for uint16 arguments.
   */
  void uint16Min(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const uint16 *sp;
    uint16 *dp;
    uint16 minval;
    uint32 mindex;
    uint32 i, j;

    sp = (const uint16*) src;
    dp = (uint16*) dst;
    for (i=0;i<count;i++) {
      minval = sp[majstride*i];
      mindex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] < minval) {
	  minval = sp[majstride*i+stride*j];
	  mindex = j;
	}
      }
      dp[i] = minval;
      iptr[i] = mindex + 1;
    }
  }

  /**
   * Minimum function for int16 arguments.
   */
  void int16Min(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const int16 *sp;
    int16 *dp;
    int16 minval;
    uint32 mindex;
    uint32 i, j;

    sp = (const int16*) src;
    dp = (int16*) dst;
    for (i=0;i<count;i++) {
      minval = sp[majstride*i];
      mindex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] < minval) {
	  minval = sp[majstride*i+stride*j];
	  mindex = j;
	}
      }
      dp[i] = minval;
      iptr[i] = mindex + 1;
    }
  }

  /**
   * Minimum function for uint32 arguments.
   */
  void uint32Min(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const uint32 *sp;
    uint32 *dp;
    uint32 minval;
    uint32 mindex;
    uint32 i, j;

    sp = (const uint32*) src;
    dp = (uint32*) dst;
    for (i=0;i<count;i++) {
      minval = sp[majstride*i];
      mindex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] < minval) {
	  minval = sp[majstride*i+stride*j];
	  mindex = j;
	}
      }
      dp[i] = minval;
      iptr[i] = mindex + 1;
    }
  }

  /**
   * Minimum function for int32 arguments.
   */
  void int32Min(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const int32 *sp;
    int32 *dp;
    int32 minval;
    uint32 mindex;
    uint32 i, j;

    sp = (const int32*) src;
    dp = (int32*) dst;
    for (i=0;i<count;i++) {
      minval = sp[majstride*i];
      mindex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] < minval) {
	  minval = sp[majstride*i+stride*j];
	  mindex = j;
	}
      }
      dp[i] = minval;
      iptr[i] = mindex + 1;
    }
  }

  /**
   * Minimum function for float arguments.
   */
  void floatMin(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float minval;
    bool init;
    uint32 mindex;
    uint32 i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      init = false;
      mindex = 0;
      for (j=0;j<length;j++) {
	if (!isnan(sp[majstride*i+stride*j]))
	  if (!init) {
	    init = true;
	    minval = sp[majstride*i+stride*j];
	    mindex = j;
	  } else if (sp[majstride*i+stride*j] < minval) {
	    minval = sp[majstride*i+stride*j];
	    mindex = j;
	  }
      }
      if (init) {
	dp[i] = minval;
	iptr[i] = mindex + 1;
      }
      else {
	dp[i] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  /**
   * Minimum function for double arguments.
   */
  void doubleMin(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double minval;
    bool init;
    uint32 mindex;
    uint32 i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      init = false;
      mindex = 0;
      for (j=0;j<length;j++) {
	if (!isnan(sp[majstride*i+stride*j]))
	  if (!init) {
	    init = true;
	    minval = sp[majstride*i+stride*j];
	    mindex = j;
	  } else if (sp[majstride*i+stride*j] < minval) {
	    minval = sp[majstride*i+stride*j];
	    mindex = j;
	  }
      }
      if (init) {
	dp[i] = minval;
	iptr[i] = mindex + 1;
      } else {
	dp[i] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  /**
   * Minimum function for complex argument - based on magnitude.
   */
  void complexMin(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float minval;
    float minval_r;
    float minval_i;
    float tstval;
    bool init;
    uint32 mindex;
    uint32 i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      init = false;
      mindex = 0;
      for (j=0;j<length;j++) {
	if ((!isnan(sp[2*(majstride*i+stride*j)])) &&
	    (!isnan(sp[2*(majstride*i+stride*j)+1]))) {
	  tstval = complex_abs(sp[2*(majstride*i+stride*j)],
			       sp[2*(majstride*i+stride*j)+1]);
	  if (!init) {
	    init = true;
	    minval = tstval;
	    mindex = j;
	    minval_r = sp[2*(majstride*i+stride*j)];
	    minval_i = sp[2*(majstride*i+stride*j)+1];
	  } else if (tstval < minval) {
	    minval = tstval;
	    mindex = j;
	    minval_r = sp[2*(majstride*i+stride*j)];
	    minval_i = sp[2*(majstride*i+stride*j)+1];
	  }
	}
      }
      if (init) {
	dp[2*i] = minval_r;
	dp[2*i+1] = minval_i;
	iptr[i] = mindex + 1;
      } else {
	dp[2*i] = atof("nan");
	dp[2*i+1] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  /**
   * Minimum function for dcomplex argument - based on magnitude.
   */
  void dcomplexMin(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double minval;
    double minval_r;
    double minval_i;
    double tstval;
    bool init;
    uint32 mindex;
    uint32 i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      init = false;
      mindex = 0;
      for (j=0;j<length;j++) {
	if ((!isnan(sp[2*(majstride*i+stride*j)])) &&
	    (!isnan(sp[2*(majstride*i+stride*j)+1]))) {
	  tstval = dcomplex_abs(sp[2*(majstride*i+stride*j)],
			       sp[2*(majstride*i+stride*j)+1]);
	  if (!init) {
	    init = true;
	    minval = tstval;
	    mindex = j;
	    minval_r = sp[2*(majstride*i+stride*j)];
	    minval_i = sp[2*(majstride*i+stride*j)+1];
	  } else if (tstval < minval) {
	    minval = tstval;
	    mindex = j;
	    minval_r = sp[2*(majstride*i+stride*j)];
	    minval_i = sp[2*(majstride*i+stride*j)+1];
	  }
	}
      }
      if (init) {
	dp[2*i] = minval_r;
	dp[2*i+1] = minval_i;
	iptr[i] = mindex + 1;
      } else {
	dp[2*i] = atof("nan");
	dp[2*i+1] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  /**
   * Greater-than function for logical arguments.
   */
  void logicalGreater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const logical *spx, *spy;
    logical *dp;
    uint32 i;

    spx = (const logical*) srcx;
    spy = (const logical*) srcy;
    dp = (logical*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for uint8 arguments.
   */
  void uint8Greater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const uint8 *spx, *spy;
    uint8 *dp;
    uint32 i;

    spx = (const uint8*) srcx;
    spy = (const uint8*) srcy;
    dp = (uint8*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for int8 arguments.
   */
  void int8Greater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const int8 *spx, *spy;
    int8 *dp;
    uint32 i;

    spx = (const int8*) srcx;
    spy = (const int8*) srcy;
    dp = (int8*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for uint16 arguments.
   */
  void uint16Greater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const uint16 *spx, *spy;
    uint16 *dp;
    uint32 i;

    spx = (const uint16*) srcx;
    spy = (const uint16*) srcy;
    dp = (uint16*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for int16 arguments.
   */
  void int16Greater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const int16 *spx, *spy;
    int16 *dp;
    uint32 i;

    spx = (const int16*) srcx;
    spy = (const int16*) srcy;
    dp = (int16*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for uint32 arguments.
   */
  void uint32Greater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const uint32 *spx, *spy;
    uint32 *dp;
    uint32 i;

    spx = (const uint32*) srcx;
    spy = (const uint32*) srcy;
    dp = (uint32*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for int32 arguments.
   */
  void int32Greater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const int32 *spx, *spy;
    int32 *dp;
    uint32 i;

    spx = (const int32*) srcx;
    spy = (const int32*) srcy;
    dp = (int32*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for float arguments.
   */
  void floatGreater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const float *spx, *spy;
    float *dp;
    uint32 i;

    spx = (const float*) srcx;
    spy = (const float*) srcy;
    dp = (float*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for double arguments.
   */
  void doubleGreater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const double *spx, *spy;
    double *dp;
    uint32 i;

    spx = (const double*) srcx;
    spy = (const double*) srcy;
    dp = (double*) dst;
    for (i=0;i<count;i++)
      dp[i] = (spx[stridex*i] > spy[stridey*i]) ? 
	spx[stridex*i] : spy[stridey*i];
  }

  /**
   * Greater-than function for complex arguments.
   */
  void complexGreater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const float *spx, *spy;
    float *dp;
    uint32 i;
    float xmag, ymag;

    spx = (const float*) srcx;
    spy = (const float*) srcy;
    dp = (float*) dst;
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
   * Greater-than function for dcomplex arguments.
   */
  void dcomplexGreater(const char* srcx, const char* srcy, char* dst, int count, 
		   int stridex, int stridey) {
    const double *spx, *spy;
    double *dp;
    uint32 i;
    double xmag, ymag;

    spx = (const double*) srcx;
    spy = (const double*) srcy;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      xmag = dcomplex_abs(spx[2*stridex*i],spx[2*stridex*i+1]);
      ymag = dcomplex_abs(spy[2*stridey*i],spy[2*stridey*i+1]);
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
   * Maximum function for logical arguments.
   */
  void logicalMax(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const logical *sp;
    logical *dp;
    logical maxval;
    uint32 maxdex;
    uint32 i, j;

    sp = (const logical*) src;
    dp = (logical*) dst;
    for (i=0;i<count;i++) {
      maxval = sp[majstride*i];
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] > maxval) {
	  maxval = sp[majstride*i+stride*j];
	  maxdex = j;
	}
      }
      dp[i] = maxval;
      iptr[i] = maxdex + 1;
    }
  }

  /**
   * Maximum function for uint8 arguments.
   */
  void uint8Max(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const uint8 *sp;
    uint8 *dp;
    uint8 maxval;
    uint32 maxdex;
    uint32 i, j;

    sp = (const uint8*) src;
    dp = (uint8*) dst;
    for (i=0;i<count;i++) {
      maxval = sp[majstride*i];
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] > maxval) {
	  maxval = sp[majstride*i+stride*j];
	  maxdex = j;
	}
      }
      dp[i] = maxval;
      iptr[i] = maxdex + 1;
    }
  }

  /**
   * Maximum function for int8 arguments.
   */
  void int8Max(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const int8 *sp;
    int8 *dp;
    int8 maxval;
    uint32 maxdex;
    uint32 i, j;

    sp = (const int8*) src;
    dp = (int8*) dst;
    for (i=0;i<count;i++) {
      maxval = sp[majstride*i];
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] > maxval) {
	  maxval = sp[majstride*i+stride*j];
	  maxdex = j;
	}
      }
      dp[i] = maxval;
      iptr[i] = maxdex + 1;
    }
  }

  /**
   * Maximum function for uint16 arguments.
   */
  void uint16Max(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const uint16 *sp;
    uint16 *dp;
    uint16 maxval;
    uint32 maxdex;
    uint32 i, j;

    sp = (const uint16*) src;
    dp = (uint16*) dst;
    for (i=0;i<count;i++) {
      maxval = sp[majstride*i];
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] > maxval) {
	  maxval = sp[majstride*i+stride*j];
	  maxdex = j;
	}
      }
      dp[i] = maxval;
      iptr[i] = maxdex + 1;
    }
  }

  /**
   * Maximum function for int16 arguments.
   */
  void int16Max(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const int16 *sp;
    int16 *dp;
    int16 maxval;
    uint32 maxdex;
    uint32 i, j;

    sp = (const int16*) src;
    dp = (int16*) dst;
    for (i=0;i<count;i++) {
      maxval = sp[majstride*i];
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] > maxval) {
	  maxval = sp[majstride*i+stride*j];
	  maxdex = j;
	}
      }
      dp[i] = maxval;
      iptr[i] = maxdex + 1;
    }
  }

  /**
   * Maximum function for uint32 arguments.
   */
  void uint32Max(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const uint32 *sp;
    uint32 *dp;
    uint32 maxval;
    uint32 maxdex;
    uint32 i, j;

    sp = (const uint32*) src;
    dp = (uint32*) dst;
    for (i=0;i<count;i++) {
      maxval = sp[majstride*i];
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] > maxval) {
	  maxval = sp[majstride*i+stride*j];
	  maxdex = j;
	}
      }
      dp[i] = maxval;
      iptr[i] = maxdex + 1;
    }
  }

  /**
   * Maximum function for int32 arguments.
   */
  void int32Max(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const int32 *sp;
    int32 *dp;
    int32 maxval;
    uint32 maxdex;
    uint32 i, j;

    sp = (const int32*) src;
    dp = (int32*) dst;
    for (i=0;i<count;i++) {
      maxval = sp[majstride*i];
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (sp[majstride*i+stride*j] > maxval) {
	  maxval = sp[majstride*i+stride*j];
	  maxdex = j;
	}
      }
      dp[i] = maxval;
      iptr[i] = maxdex + 1;
    }
  }

  /**
   * Maximum function for float arguments.
   */
  void floatMax(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float maxval;
    bool init;
    uint32 maxdex;
    uint32 i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      init = false;
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (!isnan(sp[majstride*i+stride*j]))
	  if (!init) {
	    init = true;
	    maxval = sp[majstride*i+stride*j];
	    maxdex = j;
	  } else if (sp[majstride*i+stride*j] > maxval) {
	    maxval = sp[majstride*i+stride*j];
	    maxdex = j;
	  }
      }
      if (init) {
	dp[i] = maxval;
	iptr[i] = maxdex + 1;
      }
      else {
	dp[i] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  /**
   * Maximum function for double arguments.
   */
  void doubleMax(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double maxval;
    bool init;
    uint32 maxdex;
    uint32 i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      init = false;
      maxdex = 0;
      for (j=0;j<length;j++) {
	if (!isnan(sp[majstride*i+stride*j]))
	  if (!init) {
	    init = true;
	    maxval = sp[majstride*i+stride*j];
	    maxdex = j;
	  } else if (sp[majstride*i+stride*j] > maxval) {
	    maxval = sp[majstride*i+stride*j];
	    maxdex = j;
	  }
      }
      if (init) {
	dp[i] = maxval;
	iptr[i] = maxdex + 1;
      } else {
	dp[i] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  /**
   * Maximum function for complex argument - based on magnitude.
   */
  void complexMax(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float maxval;
    float maxval_r;
    float maxval_i;
    float tstval;
    bool init;
    uint32 maxdex;
    uint32 i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      init = false;
      maxdex = 0;
      for (j=0;j<length;j++) {
	if ((!isnan(sp[2*(majstride*i+stride*j)])) &&
	    (!isnan(sp[2*(majstride*i+stride*j)+1]))) {
	  tstval = complex_abs(sp[2*(majstride*i+stride*j)],
			       sp[2*(majstride*i+stride*j)+1]);
	  if (!init) {
	    init = true;
	    maxval = tstval;
	    maxdex = j;
	    maxval_r = sp[2*(majstride*i+stride*j)];
	    maxval_i = sp[2*(majstride*i+stride*j)+1];
	  } else if (tstval > maxval) {
	    maxval = tstval;
	    maxdex = j;
	    maxval_r = sp[2*(majstride*i+stride*j)];
	    maxval_i = sp[2*(majstride*i+stride*j)+1];
	  }
	}
      }
      if (init) {
	dp[2*i] = maxval_r;
	dp[2*i+1] = maxval_i;
	iptr[i] = maxdex + 1;
      } else {
	dp[2*i] = atof("nan");
	dp[2*i+1] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  /**
   * Maximum function for dcomplex argument - based on magnitude.
   */
  void dcomplexMax(const char* src, char* dst, uint32 *iptr, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double maxval;
    double maxval_r;
    double maxval_i;
    double tstval;
    bool init;
    uint32 maxdex;
    uint32 i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      init = false;
      maxdex = 0;
      for (j=0;j<length;j++) {
	if ((!isnan(sp[2*(majstride*i+stride*j)])) &&
	    (!isnan(sp[2*(majstride*i+stride*j)+1]))) {
	  tstval = dcomplex_abs(sp[2*(majstride*i+stride*j)],
			       sp[2*(majstride*i+stride*j)+1]);
	  if (!init) {
	    init = true;
	    maxval = tstval;
	    maxdex = j;
	    maxval_r = sp[2*(majstride*i+stride*j)];
	    maxval_i = sp[2*(majstride*i+stride*j)+1];
	  } else if (tstval > maxval) {
	    maxval = tstval;
	    maxdex = j;
	    maxval_r = sp[2*(majstride*i+stride*j)];
	    maxval_i = sp[2*(majstride*i+stride*j)+1];
	  }
	}
      }
      if (init) {
	dp[2*i] = maxval_r;
	dp[2*i+1] = maxval_i;
	iptr[i] = maxdex + 1;
      } else {
	dp[2*i] = atof("nan");
	dp[2*i+1] = atof("nan");
	iptr[i] = 0;
      }
    }
  }

  void int32Sum(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const int32 *sp;
    int32 *dp;
    int32 accum;
    int i, j;

    sp = (const int32*) src;
    dp = (int32*) dst;
    for (i=0;i<count;i++) {
      accum = 0;
      for (j=0;j<length;j++) {
	accum += sp[majstride*i+stride*j];
      }
      dp[i] = accum;
    }
  }

  void floatSum(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float accum;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum = 0.0f;
      for (j=0;j<length;j++) {
	accum += sp[majstride*i+stride*j];
      }
      dp[i] = accum;
    }
  }

  void doubleSum(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double accum;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum = 0;
      for (j=0;j<length;j++) {
	accum += sp[majstride*i+stride*j];
      }
      dp[i] = accum;
    }
  }

  void complexSum(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float*sp;
    float *dp;
    float accum_r;
    float accum_i;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum_r = 0;
      accum_i = 0;
      for (j=0;j<length;j++) {
	accum_r += sp[2*(majstride*i+stride*j)];
	accum_i += sp[2*(majstride*i+stride*j)+1];
      }
      dp[2*i] = accum_r;
      dp[2*i+1] = accum_i;
    }
  }

  void dcomplexSum(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double*sp;
    double *dp;
    double accum_r;
    double accum_i;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum_r = 0;
      accum_i = 0;
      for (j=0;j<length;j++) {
	accum_r += sp[2*(majstride*i+stride*j)];
	accum_i += sp[2*(majstride*i+stride*j)+1];
      }
      dp[2*i] = accum_r;
      dp[2*i+1] = accum_i;
    }
  }

  void floatMean(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float accum;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum = 0.0f;
      for (j=0;j<length;j++) {
	accum += sp[majstride*i+stride*j];
      }
      dp[i] = accum/length;
    }
  }

  void doubleMean(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double accum;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum = 0;
      for (j=0;j<length;j++) {
	accum += sp[majstride*i+stride*j];
      }
      dp[i] = accum/length;
    }
  }

  void complexMean(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float*sp;
    float *dp;
    float accum_r;
    float accum_i;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum_r = 0;
      accum_i = 0;
      for (j=0;j<length;j++) {
	accum_r += sp[2*(majstride*i+stride*j)];
	accum_i += sp[2*(majstride*i+stride*j)+1];
      }
      dp[2*i] = accum_r/length;
      dp[2*i+1] = accum_i/length;
    }
  }

  void dcomplexMean(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double*sp;
    double *dp;
    double accum_r;
    double accum_i;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum_r = 0;
      accum_i = 0;
      for (j=0;j<length;j++) {
	accum_r += sp[2*(majstride*i+stride*j)];
	accum_i += sp[2*(majstride*i+stride*j)+1];
      }
      dp[2*i] = accum_r/length;
      dp[2*i+1] = accum_i/length;
    }
  }

  void floatVar(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float accum_first;
    float accum_second;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum_first = 0.0f;
      accum_second = 0.0f;
      for (j=0;j<length;j++) {
	accum_first += sp[majstride*i+stride*j];
	accum_second += sp[majstride*i+stride*j]*sp[majstride*i+stride*j];
	
      }
      dp[i] = accum_second/(length-1.0) - (accum_first/length)*(accum_first/(length-1.0));
    }
  }

  void doubleVar(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double accum_first;
    double accum_second;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum_first = 0.0f;
      accum_second = 0.0f;
      for (j=0;j<length;j++) {
	accum_first += sp[majstride*i+stride*j];
	accum_second += sp[majstride*i+stride*j]*sp[majstride*i+stride*j];
	
      }
      dp[i] = accum_second/(length-1.0) - (accum_first/length)*(accum_first/(length-1.0));
    }
  }

  void complexVar(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float*sp;
    float *dp;
    float accum_r_first;
    float accum_i_first;
    float accum_second;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum_r_first = 0;
      accum_i_first = 0;
      accum_second = 0;
      for (j=0;j<length;j++) {
	accum_r_first += sp[2*(majstride*i+stride*j)];
	accum_i_first += sp[2*(majstride*i+stride*j)+1];
	accum_second += sp[2*(majstride*i+stride*j)]*sp[2*(majstride*i+stride*j)] + 
	  sp[2*(majstride*i+stride*j)+1]*sp[2*(majstride*i+stride*j)+1];
      }
      dp[i] = accum_second/(length-1.0) - 
	(accum_r_first*accum_r_first + accum_i_first*accum_i_first)/(length*(length-1.0));
    }
  }

  void dcomplexVar(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double*sp;
    double *dp;
    double accum_r_first;
    double accum_i_first;
    double accum_second;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum_r_first = 0;
      accum_i_first = 0;
      accum_second = 0;
      for (j=0;j<length;j++) {
	accum_r_first += sp[2*(majstride*i+stride*j)];
	accum_i_first += sp[2*(majstride*i+stride*j)+1];
	accum_second += sp[2*(majstride*i+stride*j)]*sp[2*(majstride*i+stride*j)] + 
	  sp[2*(majstride*i+stride*j)+1]*sp[2*(majstride*i+stride*j)+1];
      }
      dp[i] = accum_second/(length-1.0) - 
	(accum_r_first*accum_r_first + accum_i_first*accum_i_first)/(length*(length-1.0));
    }
  }

  void int32Prod(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const int32 *sp;
    int32 *dp;
    int32 accum;
    int i, j;

    sp = (const int32*) src;
    dp = (int32*) dst;
    for (i=0;i<count;i++) {
      accum = 1;
      for (j=0;j<length;j++) {
	accum *= sp[majstride*i+stride*j];
      }
      dp[i] = accum;
    }
  }

  void floatProd(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float *sp;
    float *dp;
    float accum;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum = 1.0f;
      for (j=0;j<length;j++) {
	accum *= sp[majstride*i+stride*j];
      }
      dp[i] = accum;
    }
  }

  void doubleProd(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double *sp;
    double *dp;
    double accum;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum = 1.0;
      for (j=0;j<length;j++) {
	accum *= sp[majstride*i+stride*j];
      }
      dp[i] = accum;
    }
  }

  void complexProd(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const float*sp;
    float *dp;
    float accum_r;
    float accum_i;
    float t1, t2;
    int i, j;

    sp = (const float*) src;
    dp = (float*) dst;
    for (i=0;i<count;i++) {
      accum_r = 1;
      accum_i = 0;
      for (j=0;j<length;j++) {
	t1 = accum_r*sp[2*(majstride*i+stride*j)] - accum_i*sp[2*(majstride*i+stride*j)+1];
	t2 = accum_r*sp[2*(majstride*i+stride*j)+1] + accum_i*sp[2*(majstride*i+stride*j)];
	accum_r = t1;
	accum_i = t2;
      }
      dp[2*i] = accum_r;
      dp[2*i+1] = accum_i;
    }
  }

  void dcomplexProd(const char* src, char* dst, int count, int stride, int majstride, int length) {
    const double*sp;
    double *dp;
    double accum_r;
    double accum_i;
    double t1, t2;
    int i, j;

    sp = (const double*) src;
    dp = (double*) dst;
    for (i=0;i<count;i++) {
      accum_r = 1;
      accum_i = 0;
      for (j=0;j<length;j++) {
	t1 = accum_r*sp[2*(majstride*i+stride*j)] - accum_i*sp[2*(majstride*i+stride*j)+1];
	t2 = accum_r*sp[2*(majstride*i+stride*j)+1] + accum_i*sp[2*(majstride*i+stride*j)];
	accum_r = t1;
	accum_i = t2;
      }
      dp[2*i] = accum_r;
      dp[2*i+1] = accum_i;
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
      logicalLess((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      uint8Less((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      int8Less((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      uint16Less((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      int16Less((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      uint32Less((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      int32Less((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatLess((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleLess((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      complexLess((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      dcomplexLess((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    retvec.push_back(retval);
    return retvec;
  }

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
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retArray;
      retArray.push_back(arg[0]);
      return retArray;
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
    outDim[workDim] = 1;
    // Calculate the stride...
    int stride = 1;
    int d;
    int length;
    int majstride_in;
    length = inDim[workDim];
    for (d=0;d<workDim;d++)
      stride *= inDim[d];
    if (workDim == 0)
      majstride_in = inDim[0];
    else
      majstride_in = 1;
    // Calculate the number of ops to do...
    int workcount = 1;
    for (d=0;d<inDim.getLength();d++)
      if (d != workDim) workcount *= inDim[d];
    // Allocate the output that contains the indices
    uint32* iptr = (uint32 *) Malloc(sizeof(uint32)*outDim.getElementCount());
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_LOGICAL: {
      char* ptr = (char *) Malloc(sizeof(logical)*outDim.getElementCount());
      logicalMin((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      uint8Min((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      int8Min((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      uint16Min((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      int16Min((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      uint32Min((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      int32Min((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatMin((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleMin((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      complexMin((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      dcomplexMin((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
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
      logicalGreater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      uint8Greater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      int8Greater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      uint16Greater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      int16Greater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      uint32Greater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      int32Greater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatGreater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleGreater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      complexGreater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      dcomplexGreater((const char *)x.getDataPointer(),
		  (const char *)y.getDataPointer(),
		  ptr,outDim.getElementCount(),
		  xStride, yStride);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
    }
    retvec.push_back(retval);
    return retvec;
  }

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
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retArray;
      retArray.push_back(arg[0]);
      return retArray;
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
    outDim[workDim] = 1;
    // Calculate the stride...
    int stride = 1;
    int d;
    int length;
    int majstride_in;
    length = inDim[workDim];
    for (d=0;d<workDim;d++)
      stride *= inDim[d];
    if (workDim == 0)
      majstride_in = inDim[0];
    else
      majstride_in = 1;
    // Calculate the number of ops to do...
    int workcount = 1;
    for (d=0;d<inDim.getLength();d++)
      if (d != workDim) workcount *= inDim[d];
    // Allocate the output that contains the indices
    uint32* iptr = (uint32 *) Malloc(sizeof(uint32)*outDim.getElementCount());
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_LOGICAL: {
      char* ptr = (char *) Malloc(sizeof(logical)*outDim.getElementCount());
      logicalMax((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_LOGICAL,outDim,ptr);
      break;
    }
    case FM_UINT8: {
      char* ptr = (char *) Malloc(sizeof(uint8)*outDim.getElementCount());
      uint8Max((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_UINT8,outDim,ptr);
      break;
    }
    case FM_INT8: {
      char* ptr = (char *) Malloc(sizeof(int8)*outDim.getElementCount());
      int8Max((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT8,outDim,ptr);
      break;
    }
    case FM_UINT16: {
      char* ptr = (char *) Malloc(sizeof(uint16)*outDim.getElementCount());
      uint16Max((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_UINT16,outDim,ptr);
      break;
    }
    case FM_INT16: {
      char* ptr = (char *) Malloc(sizeof(int16)*outDim.getElementCount());
      int16Max((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT16,outDim,ptr);
      break;
    }
    case FM_UINT32: {
      char* ptr = (char *) Malloc(sizeof(uint32)*outDim.getElementCount());
      uint32Max((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_UINT32,outDim,ptr);
      break;
    }
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      int32Max((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatMax((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleMax((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      complexMax((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      dcomplexMax((const char*) input.getDataPointer(),ptr,iptr,workcount,stride,majstride_in,length);
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
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retArray;
      retArray.push_back(arg[0]);
      return retArray;
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
    outDim[workDim] = 1;
    // Calculate the stride...
    int stride = 1;
    int d;
    int length;
    int majstride_in;
    length = inDim[workDim];
    for (d=0;d<workDim;d++)
      stride *= inDim[d];
    if (workDim == 0)
      majstride_in = inDim[0];
    else
      majstride_in = 1;
    // Calculate the number of ops to do...
    int workcount = 1;
    for (d=0;d<inDim.getLength();d++)
      if (d != workDim) workcount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      int32Sum((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatSum((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleSum((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      complexSum((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      dcomplexSum((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
   }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

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
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retArray;
      retArray.push_back(arg[0]);
      return retArray;
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
    outDim[workDim] = 1;
    // Calculate the stride...
    int stride = 1;
    int d;
    int length;
    int majstride_in;
    length = inDim[workDim];
    for (d=0;d<workDim;d++)
      stride *= inDim[d];
    if (workDim == 0)
      majstride_in = inDim[0];
    else
      majstride_in = 1;
    // Calculate the number of ops to do...
    int workcount = 1;
    for (d=0;d<inDim.getLength();d++)
      if (d != workDim) workcount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatMean((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleMean((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      complexMean((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      dcomplexMean((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
   }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

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
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retArray;
      retArray.push_back(arg[0]);
      return retArray;
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
    outDim[workDim] = 1;
    // Calculate the stride...
    int stride = 1;
    int d;
    int length;
    int majstride_in;
    length = inDim[workDim];
    for (d=0;d<workDim;d++)
      stride *= inDim[d];
    if (workDim == 0)
      majstride_in = inDim[0];
    else
      majstride_in = 1;
    // Calculate the number of ops to do...
    int workcount = 1;
    for (d=0;d<inDim.getLength();d++)
      if (d != workDim) workcount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatVar((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleVar((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      complexVar((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      dcomplexVar((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
   }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }

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
	ptr[2*i] = dp[2*i+1];
	ptr[2*i+1] = dp[2*i];
      }
      retval = Array(FM_COMPLEX,tmp.getDimensions(),ptr);
    } else if (argType == FM_DCOMPLEX) {
      int len;
      len = tmp.getLength();
      double *dp = (double*) tmp.getDataPointer();
      double *ptr = (double*) Malloc(sizeof(double)*tmp.getLength()*2);
      for (i=0;i<len;i++) {
	ptr[2*i] = dp[2*i+1];
	ptr[2*i+1] = dp[2*i];
      }
      retval = Array(FM_DCOMPLEX,tmp.getDimensions(),ptr);
    } else
      retval = tmp;
    ArrayVector out;
    out.push_back(retval);
    return out;
  }

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
      retval = Array(FM_DOUBLE,tmp.getDimensions(),ptr);
    } else
      retval = tmp;
    ArrayVector out;
    out.push_back(retval);
    return out;
  }

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

  ArrayVector AbsFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("abs function requires 1 argument");
    Array tmp(arg[0]);
    if (tmp.isReferenceType())
      throw Exception("argument to abs function must be numeric");
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
    if (input.isScalar() || input.isEmpty()) {
      ArrayVector retArray;
      retArray.push_back(arg[0]);
      return retArray;
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
    outDim[workDim] = 1;
    // Calculate the stride...
    int stride = 1;
    int d;
    int length;
    int majstride_in;
    length = inDim[workDim];
    for (d=0;d<workDim;d++)
      stride *= inDim[d];
    if (workDim == 0)
      majstride_in = inDim[0];
    else
      majstride_in = 1;
    // Calculate the number of ops to do...
    int workcount = 1;
    for (d=0;d<inDim.getLength();d++)
      if (d != workDim) workcount *= inDim[d];
    // Allocate the values output, and call the appropriate helper func.
    Array retval;
    switch (argType) {
    case FM_INT32: {
      char* ptr = (char *) Malloc(sizeof(int32)*outDim.getElementCount());
      int32Prod((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_INT32,outDim,ptr);
      break;
    }
    case FM_FLOAT: {
      char* ptr = (char *) Malloc(sizeof(float)*outDim.getElementCount());
      floatProd((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_FLOAT,outDim,ptr);
      break;
    }
    case FM_DOUBLE: {
      char* ptr = (char *) Malloc(sizeof(double)*outDim.getElementCount());
      doubleProd((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DOUBLE,outDim,ptr);
      break;
    }
    case FM_COMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(float)*outDim.getElementCount());
      complexProd((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_COMPLEX,outDim,ptr);
      break;
    }
    case FM_DCOMPLEX: {
      char* ptr = (char *) Malloc(2*sizeof(double)*outDim.getElementCount());
      dcomplexProd((const char*) input.getDataPointer(),ptr,workcount,stride,majstride_in,length);
      retval = Array(FM_DCOMPLEX,outDim,ptr);
      break;
    }
   }
    ArrayVector retArray;
    retArray.push_back(retval);
    return retArray;
  }
}
