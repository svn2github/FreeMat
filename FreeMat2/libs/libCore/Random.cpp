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
#include "Malloc.hpp"
#include <math.h>
/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3. The names of its contributors may not be used to endorse or promote 
   products derived from this software without specific prior written 
   permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/

#include <stdio.h>

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
  mt[0]= s & 0xffffffffUL;
  for (mti=1; mti<N; mti++) {
    mt[mti] = 
      (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    mt[mti] &= 0xffffffffUL;
    /* for >32 bit machines */
  }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array(unsigned long init_key[], unsigned long key_length)
{
  int i, j, k;
  init_genrand(19650218UL);
  i=1; j=0;
  k = (N>key_length ? N : key_length);
  for (; k; k--) {
    mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
      + init_key[j] + j; /* non linear */
    mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++; j++;
    if (i>=N) { mt[0] = mt[N-1]; i=1; }
    if (j>=key_length) j=0;
  }
  for (k=N-1; k; k--) {
    mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
      - i; /* non linear */
    mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++;
    if (i>=N) { mt[0] = mt[N-1]; i=1; }
  }

  mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
  unsigned long y;
  static unsigned long mag01[2]={0x0UL, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (mti >= N) { /* generate N words at one time */
    int kk;

    if (mti == N+1)   /* if init_genrand() has not been called, */
      init_genrand(5489UL); /* a default initial seed is used */

    for (kk=0;kk<N-M;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (;kk<N-1;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
    mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0;
  }
  
  y = mt[mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
  return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
  return genrand_int32()*(1.0/4294967295.0); 
  /* divided by 2^32-1 */ 
}

/* Generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
  return genrand_int32()*(1.0/4294967296.0); 
  /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
  return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
  /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
  unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
  return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */

namespace FreeMat { 
  static bool initialized = false;

  ArrayVector SeedFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Seed function requires a single integer argument");
    Array tmp(arg[0]);
    uint32 seedval;
    seedval = tmp.getContentsAsIntegerScalar();
    init_genrand(seedval);
    initialized = true;
    return ArrayVector();
  }

  ArrayVector RandnFunction(int nargout, const ArrayVector& arg) {
	  int i;
    unsigned long init[4]={0x923, 0x234, 0x405, 0x456}, length=4;
    if (!initialized) {
      init_by_array(init, length);
      initialized = true;
    }
    Array t, s;
    Dimensions dims;
    int32 *dp;
    if (arg.size() == 0)
      dims.makeScalar();
    else {
      // Case 1 - all of the entries are scalar
      bool allScalars;
      allScalars = true;
      for (i=0;i<arg.size();i++)
	allScalars &= arg[i].isScalar();
      if (allScalars) {
	t = arg[0];
	if (arg.size() == 1) {
	  // If all scalars and only one argument - we want a square zero matrix
	  dims[0] = t.getContentsAsIntegerScalar();
	  dims[1] = dims[0];
	} else {
	  // If all scalars and and multiple arguments, we count dimensions
	  for (i=0;i<arg.size();i++) {
	    t = arg[i];
	    dims[i] = t.getContentsAsIntegerScalar();
	  }
	}
      } else {
	if (arg.size() > 1)
	  throw Exception("Arguments to randn function must be either all scalars or a single vector");
	t = arg[0];
	t.promoteType(FM_UINT32);
	dp = (int*) t.getDataPointer();
	for (i=0;i<t.getLength();i++)
	  dims[i] = dp[i];
      }
      bool allPositive;
      allPositive = true;
      for (i=0;i<dims.getLength();i++)
	allPositive &= (dims[i] >= 0);
      if (!allPositive)
	throw Exception("Randn function requires positive arguments");
    }
    double *qp;
    qp = (double*) Malloc(sizeof(double)*dims.getElementCount());
    int len;
    int j;
    len = dims.getElementCount();
    for (j=0;j<len;j+=2) {
      double x1, x2, w, y1, y2;
      do {
	x1 = 2.0*genrand_res53()-1.0;
	x2 = 2.0*genrand_res53()-1.0;
	w = x1 * x1 + x2 * x2;
      } while ( w >= 1.0 );
      w = sqrt( (-2.0 * log( w ) ) / w );
      y1 = x1 * w;
      y2 = x2 * w;
      qp[j] = y1;
      if (j<len-1)
	qp[j+1] = y2;
    }
    s = Array(FM_DOUBLE,dims,qp);
    ArrayVector retval;
    retval.push_back(s);
    return retval;
  }
  
  ArrayVector RandFunction(int nargout, const ArrayVector& arg) {
	  int i;
    unsigned long init[4]={0x923, 0x234, 0x405, 0x456}, length=4;
    if (!initialized) {
      init_by_array(init, length);
      initialized = true;
    }
    Array t, s;
    Dimensions dims;
    int32 *dp;
    if (arg.size() == 0)
      dims.makeScalar();
    else {
      // Case 1 - all of the entries are scalar
      bool allScalars;
      allScalars = true;
      for (i=0;i<arg.size();i++)
	allScalars &= arg[i].isScalar();
      if (allScalars) {
	t = arg[0];
	if (arg.size() == 1) {
	  // If all scalars and only one argument - we want a square zero matrix
	  dims[0] = t.getContentsAsIntegerScalar();
	  dims[1] = dims[0];
	} else {
	  // If all scalars and and multiple arguments, we count dimensions
	  for (i=0;i<arg.size();i++) {
	    t = arg[i];
	    dims[i] = t.getContentsAsIntegerScalar();
	  }	  
	}
      } else {
	if (arg.size() > 1)
	  throw Exception("Arguments to rand function must be either all scalars or a single vector");
	t = arg[0];
	t.promoteType(FM_UINT32);
	dp = (int*) t.getDataPointer();
	for (i=0;i<t.getLength();i++)
	  dims[i] = dp[i];
      }
      bool allPositive;
      allPositive = true;
      for (i=0;i<dims.getLength();i++)
	allPositive &= (dims[i] >= 0);
      if (!allPositive)
	throw Exception("Rand function requires positive arguments");
    }
    double *qp;
    qp = (double*) Malloc(sizeof(double)*dims.getElementCount());
    int len;
    int j;
    len = dims.getElementCount();
    for (j=0;j<len;j++)
      qp[j] = genrand_res53();
    s = Array(FM_DOUBLE,dims,qp);
    ArrayVector retval;
    retval.push_back(s);
    return retval;
  }
}
