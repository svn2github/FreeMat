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

#include "VectorOps.hpp"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

namespace FreeMat {

/***************************************************************************
 * Vector-Vector arithmetic
 ***************************************************************************/

#define WrapFunInt32(x,y) \
   extern "C" void x(int*, int32*, const int32*, const int*, const int32*, const int*);\
   void y(const int n, int32* c, const int32* a, const int stride1,\
          const int32* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }

#define WrapFunFloat(x,y) \
   extern "C" void x(int*, float*, const float*, const int*, const float*, const int*);\
   void y(const int n, float* c, const float* a, const int stride1,\
          const float* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }
    
#define WrapFunDouble(x,y) \
   extern "C" void x(int*, double*, const double*, const int*, const double*, const int*);\
   void y(const int n, double* c, const double* a, const int stride1,\
          const double* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }
    
#define WrapFunComplex(x,y) \
   extern "C" void x(int*, float*, const float*, const int*, const float*, const int*);\
   void y(const int n, float* c, const float* a, const int stride1,\
          const float* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }
    
#define WrapFunDcomplex(x,y) \
   extern "C" void x(int*, double*, const double*, const int*, const double*, const int*);\
   void y(const int n, double* c, const double* a, const int stride1,\
          const double* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }

#define WrapFunVoid(x,y) \
   extern "C" void x(int*, void*, const void*, const int*, const void*, const int*); \
   void y(const int n, void* c, const void* a, const int stride1, const void* b, const int stride2) {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }
    
#define WrapFunUnaryInt32(x,y) \
   extern "C" void x(int*, int32*, const int32*);\
   void y(const int n, int32* c, const int32* a)  {\
      int n1(n);\
      x(&n1,c,a);\
   }

#define WrapFunUnaryFloat(x,y) \
   extern "C" void x(int*, float*, const float*);\
   void y(const int n, float* c, const float* a)  {\
      int n1(n);\
      x(&n1,c,a);\
   }
    
#define WrapFunUnaryDouble(x,y) \
   extern "C" void x(int*, double*, const double*);\
   void y(const int n, double* c, const double* a)  {\
      int n1(n);\
      x(&n1,c,a);\
   }
    
#define WrapFunUnaryComplex(x,y) \
   extern "C" void x(int*, float*, const float*);\
   void y(const int n, float* c, const float* a)  {\
      int n1(n);\
      x(&n1,c,a);\
   }
    
#define WrapFunUnaryDcomplex(x,y) \
   extern "C" void x(int*, double*, const double*);\
   void y(const int n, double* c, const double* a)  {\
      int n1(n);\
      x(&n1,c,a);\
   }
    
WrapFunInt32(int32add_,int32Add);
WrapFunFloat(floatadd_,floatAdd);
WrapFunDouble(doubleadd_,doubleAdd);
WrapFunComplex(complexadd_,complexAdd);
WrapFunDcomplex(dcomplexadd_,dcomplexAdd);

WrapFunInt32(int32subtract_,int32Subtract);
WrapFunFloat(floatsubtract_,floatSubtract);
WrapFunDouble(doublesubtract_,doubleSubtract);
WrapFunComplex(complexsubtract_,complexSubtract);
WrapFunDcomplex(dcomplexsubtract_,dcomplexSubtract);

WrapFunInt32(int32dotmultiply_,int32DotMultiply);
WrapFunFloat(floatdotmultiply_,floatDotMultiply);
WrapFunDouble(doubledotmultiply_,doubleDotMultiply);
WrapFunComplex(complexdotmultiply_,complexDotMultiply);
WrapFunDcomplex(dcomplexdotmultiply_,dcomplexDotMultiply);

WrapFunInt32(int32dotleftdivide_,int32DotLeftDivide);
WrapFunFloat(floatdotleftdivide_,floatDotLeftDivide);
WrapFunDouble(doubledotleftdivide_,doubleDotLeftDivide);
WrapFunComplex(complexdotleftdivide_,complexDotLeftDivide);
WrapFunDcomplex(dcomplexdotleftdivide_,dcomplexDotLeftDivide);

WrapFunInt32(int32dotrightdivide_,int32DotRightDivide);
WrapFunFloat(floatdotrightdivide_,floatDotRightDivide);
WrapFunDouble(doubledotrightdivide_,doubleDotRightDivide);
WrapFunComplex(complexdotrightdivide_,complexDotRightDivide);
WrapFunDcomplex(dcomplexdotrightdivide_,dcomplexDotRightDivide);

WrapFunVoid(cicpower_,cicPower);
WrapFunVoid(cfcpower_,cfcPower);
WrapFunVoid(zdzpower_,zdzPower);
WrapFunVoid(cccpower_,cccPower);
WrapFunVoid(zzzpower_,zzzPower);
WrapFunVoid(zizpower_,zizPower);
WrapFunVoid(didpower_,didPower);
WrapFunVoid(dddpower_,dddPower);
WrapFunVoid(fifpower_,fifPower);
WrapFunVoid(fffpower_,fffPower);

WrapFunUnaryInt32(int32negate_,int32Negate);
WrapFunUnaryFloat(floatnegate_,floatNegate);
WrapFunUnaryDouble(doublenegate_,doubleNegate);
WrapFunUnaryComplex(complexnegate_,complexNegate);
WrapFunUnaryDcomplex(dcomplexnegate_,dcomplexNegate);

/***************************************************************************
 * Vector-Vector comparitive
 ***************************************************************************/

#define BoolwrapFunInt32(x,y) \
   extern "C" void x(int*, logical*, const int32*, const int*, const int32*, const int*);\
   void y(const int n, logical* c, const int32* a, const int stride1,\
          const int32* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }

#define BoolwrapFunFloat(x,y) \
   extern "C" void x(int*, logical*, const float*, const int*, const float*, const int*);\
   void y(const int n, logical* c, const float* a, const int stride1,\
          const float* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }
    
#define BoolwrapFunDouble(x,y) \
   extern "C" void x(int*, logical*, const double*, const int*, const double*, const int*);\
   void y(const int n, logical* c, const double* a, const int stride1,\
          const double* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }
    
#define BoolwrapFunComplex(x,y) \
   extern "C" void x(int*, logical*, const float*, const int*, const float*, const int*);\
   void y(const int n, logical* c, const float* a, const int stride1,\
          const float* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }
    
#define BoolwrapFunDcomplex(x,y) \
   extern "C" void x(int*, logical*, const double*, const int*, const double*, const int*);\
   void y(const int n, logical* c, const double* a, const int stride1,\
          const double* b, const int stride2)  {\
      int a1(stride1);\
      int a2(stride2);\
      int n1(n);\
      x(&n1,c,a,&a1,b,&a2);\
   }

BoolwrapFunInt32(int32lessthan_,int32LessThan);
BoolwrapFunFloat(floatlessthan_,floatLessThan);
BoolwrapFunDouble(doublelessthan_,doubleLessThan);
BoolwrapFunComplex(complexlessthan_,complexLessThan);
BoolwrapFunDcomplex(dcomplexlessthan_,dcomplexLessThan);

BoolwrapFunInt32(int32lessequals_,int32LessEquals);
BoolwrapFunFloat(floatlessequals_,floatLessEquals);
BoolwrapFunDouble(doublelessequals_,doubleLessEquals);
BoolwrapFunComplex(complexlessequals_,complexLessEquals);
BoolwrapFunDcomplex(dcomplexlessequals_,dcomplexLessEquals);

BoolwrapFunInt32(int32greaterthan_,int32GreaterThan);
BoolwrapFunFloat(floatgreaterthan_,floatGreaterThan);
BoolwrapFunDouble(doublegreaterthan_,doubleGreaterThan);
BoolwrapFunComplex(complexgreaterthan_,complexGreaterThan);
BoolwrapFunDcomplex(dcomplexgreaterthan_,dcomplexGreaterThan);

BoolwrapFunInt32(int32greaterequals_,int32GreaterEquals);
BoolwrapFunFloat(floatgreaterequals_,floatGreaterEquals);
BoolwrapFunDouble(doublegreaterequals_,doubleGreaterEquals);
BoolwrapFunComplex(complexgreaterequals_,complexGreaterEquals);
BoolwrapFunDcomplex(dcomplexgreaterequals_,dcomplexGreaterEquals);

BoolwrapFunInt32(int32equals_,int32Equals);
BoolwrapFunFloat(floatequals_,floatEquals);
BoolwrapFunDouble(doubleequals_,doubleEquals);
BoolwrapFunComplex(complexequals_,complexEquals);
BoolwrapFunDcomplex(dcomplexequals_,dcomplexEquals);

BoolwrapFunInt32(int32notequals_,int32NotEquals);
BoolwrapFunFloat(floatnotequals_,floatNotEquals);
BoolwrapFunDouble(doublenotequals_,doubleNotEquals);
BoolwrapFunComplex(complexnotequals_,complexNotEquals);
BoolwrapFunDcomplex(dcomplexnotequals_,dcomplexNotEquals);

void boolAnd(const int n, logical* c, const logical* a, const int stride1,
	     const logical*b, const int stride2) {
  int m = 0;
  int p = 0;
  for (int i=0;i<n;i++) {
    c[i] = (a[m] && b[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}
  
void boolOr(const int n, logical* c, const logical* a, const int stride1,
	    const logical*b, const int stride2) {
  int m = 0;
  int p = 0;
  for (int i=0;i<n;i++) {
    c[i] = (a[m] || b[p]) ? 1 : 0;
    m += stride1;
    p += stride2;
  }
}  

void boolNot(const int n, logical* c, const logical* a) {
  for (int i=0;i<n;i++)
    c[i] = (a[i]) ? 0 : 1;
}
  
}
