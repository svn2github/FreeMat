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

#ifndef __VectorOps_hpp__
#define __VectorOps_hpp__

#include "Types.hpp"

namespace FreeMat {

/**
 * This module contains the definitions of the low level driver
 * routines.  These are designed to be optimized to the max, and
 * all checking is assumed to have taken place prior to calling
 * these routines.  The naming of the routines is based on an
 * abbreviated form of name mangling.
 */

/**
 * Add functions.
 */
void int32Add(const int n, int *c, const int *a, const int stride1, const int *b, const int stride2);
void floatAdd(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleAdd(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);
void complexAdd(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexAdd(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * Subtract functions.
 */
void int32Subtract(const int n, int *c, const int *a, const int stride1, const int *b, const int stride2);
void floatSubtract(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleSubtract(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);
void complexSubtract(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexSubtract(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * DotMultiply functions.
 */
void int32DotMultiply(const int n, int *c, const int *a, const int stride1, const int *b, const int stride2);
void floatDotMultiply(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleDotMultiply(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);
void complexDotMultiply(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexDotMultiply(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * DotLeftDivide functions.
 */
void int32DotLeftDivide(const int n, int *c, const int *a, const int stride1, const int *b, const int stride2);
void floatDotLeftDivide(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleDotLeftDivide(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);
void complexDotLeftDivide(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexDotLeftDivide(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * DotRightDivide functions.
 */
void int32DotRightDivide(const int n, int *c, const int *a, const int stride1, const int *b, const int stride2);
void floatDotRightDivide(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleDotRightDivide(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);
void complexDotRightDivide(const int n, float *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexDotRightDivide(const int n, double *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * DotPower related functions - these are minimal wrappers to FORTRAN functions.  The
 * arguments are encoded using the following scheme:
 *   c - complex
 *   f - float
 *   d - double
 *   i - int32
 *   z - dcomplex
 */
void cicPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void cfcPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void zdzPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void cccPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void zzzPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void zizPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void didPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void dddPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void fifPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);
void fffPower(const int n, void* c, const void *a, const int stride1, const void *b, const int stride2);


/**
 * Negate functions.
 */
void int32Negate(const int n, int *c, const int *a);
void floatNegate(const int n, float *c, const float *a);
void doubleNegate(const int n, double *c, const double *a);
void complexNegate(const int n, float *c, const float *a);
void dcomplexNegate(const int n, double *c, const double *a);

/**
 * LessThan functions.
 */
void int32LessThan(const int n, logical *c, const int *a, const int stride1, const int *b, const int stride2);
void floatLessThan(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleLessThan(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);
void complexLessThan(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexLessThan(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * LessEquals functions.
 */
void int32LessEquals(const int n, logical *c, const int *a, const int stride1, const int *b, const int stride2);
void floatLessEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleLessEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);
void complexLessEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexLessEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * GreaterThan functions.
 */
void int32GreaterThan(const int n, logical *c, const int *a, const int stride1, const int *b, const int stride2);
void floatGreaterThan(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleGreaterThan(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);
void complexGreaterThan(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexGreaterThan(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * GreaterEquals functions.
 */
void int32GreaterEquals(const int n, logical *c, const int *a, const int stride1, const int *b, const int stride2);
void floatGreaterEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleGreaterEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);
void complexGreaterEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexGreaterEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * Equals functions.
 */
void int32Equals(const int n, logical *c, const int *a, const int stride1, const int *b, const int stride2);
void floatEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);
void complexEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * NotEquals functions.
 */
void int32NotEquals(const int n, logical *c, const int *a, const int stride1, const int *b, const int stride2);
void floatNotEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void doubleNotEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);
void complexNotEquals(const int n, logical *c, const float *a, const int stride1, const float *b, const int stride2);
void dcomplexNotEquals(const int n, logical *c, const double *a, const int stride1, const double *b, const int stride2);

/**
 * And functions.
 */
void boolAnd(const int n, logical* c, const logical* a, const int stride1, const logical* b, const int stride2);

/**
 * Or functions.
 */
void boolOr(const int n, logical* c, const logical* a, const int stride1, const logical* b, const int stride2);

/**
 * Not function.
 */
void boolNot(const int n, logical* c, const logical* a);

}

#endif
