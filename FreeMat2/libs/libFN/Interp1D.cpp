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
#include "Array.hpp"
#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include "Exception.hpp"
#include "Malloc.hpp"

template <class T>
int interv(const T *xt, int lxt, T x, int *left, int *mflag)
{

  /* Initialized data */

  static int ilo = 1;
  
  static int ihi, istep, middle;
  
  /*  from  * a practical guide to splines *  by C. de Boor */
  /* omputes  left = max( i :  xt(i) .lt. xt(lxt) .and.  xt(i) .le. x )  . */

  /* ******  i n p u t  ****** */
  /*  xt.....a real sequence, of length  lxt , assumed to be nondecreasing */
  /*  lxt.....number of terms in the sequence  xt . */
  /*  x.....the point whose location with respect to the sequence  xt  is */
  /*        to be determined. */

  /* ******  o u t p u t  ****** */
  /*  left, mflag.....both ints, whose value is */

  /*   1     -1      if               x .lt.  xt(1) */
  /*   i      0      if   xt(i)  .le. x .lt. xt(i+1) */
  /*   i      0      if   xt(i)  .lt. x .eq. xt(i+1) .eq. xt(lxt) */
  /*   i      1      if   xt(i)  .lt.        xt(i+1) .eq. xt(lxt) .lt. x */

  /*        In particular,  mflag = 0  is the 'usual' case.  mflag .ne. 0 */
  /*        indicates that  x  lies outside the CLOSED interval */
  /*        xt(1) .le. y .le. xt(lxt) . The asymmetric treatment of the */
  /*        intervals is due to the decision to make all pp functions cont- */
  /*        inuous from the right, but, by returning  mflag = 0  even if */
  /*        x = xt(lxt), there is the option of having the computed pp function */
  /*        continuous from the left at  xt(lxt) . */

  /* ******  m e t h o d  ****** */
  /*  The program is designed to be efficient in the common situation that */
  /*  it is called repeatedly, with  x  taken from an increasing or decrea- */
  /*  sing sequence. This will happen, e.g., when a pp function is to be */
  /*  graphed. The first guess for  left  is therefore taken to be the val- */
  /*  ue returned at the previous call and stored in the  l o c a l  varia- */
  /*  ble  ilo . A first check ascertains that  ilo .lt. lxt (this is nec- */
  /*  essary since the present call may have nothing to do with the previ- */
  /*  ous call). Then, if  xt(ilo) .le. x .lt. xt(ilo+1), we set  left = */
  /*  ilo  and are done after just three comparisons. */
  /*     Otherwise, we repeatedly double the difference  istep = ihi - ilo */
  /*  while also moving  ilo  and  ihi  in the direction of  x , until */
  /*                      xt(ilo) .le. x .lt. xt(ihi) , */
  /*  after which we use bisection to get, in addition, ilo+1 = ihi . */
  /*  left = ilo  is then returned. */

  /* Parameter adjustments */
  --xt;

  /* Function Body */
  ihi = ilo + 1;
  if (ihi < lxt) {
    goto L20;
  }
  if (x >= xt[lxt]) {
    goto L110;
  }
  if (lxt <= 1) {
    goto L90;
  }
  ilo = lxt - 1;
  ihi = lxt;

 L20:
  if (x >= xt[ihi]) {
    goto L40;
  }
  if (x >= xt[ilo]) {
    goto L100;
  }

  /*              **** now x .lt. xt(ilo) . decrease  ilo  to capture  x . */
  istep = 1;
 L31:
  ihi = ilo;
  ilo = ihi - istep;
  if (ilo <= 1) {
    goto L35;
  }
  if (x >= xt[ilo]) {
    goto L50;
  }
  istep <<= 1;
  goto L31;
 L35:
  ilo = 1;
  if (x < xt[1]) {
    goto L90;
  }
  goto L50;
  /*              **** now x .ge. xt(ihi) . increase  ihi  to capture  x . */
 L40:
  istep = 1;
 L41:
  ilo = ihi;
  ihi = ilo + istep;
  if (ihi >= lxt) {
    goto L45;
  }
  if (x < xt[ihi]) {
    goto L50;
  }
  istep <<= 1;
  goto L41;
 L45:
  if (x >= xt[lxt]) {
    goto L110;
  }
  ihi = lxt;

  /*           **** now xt(ilo) .le. x .lt. xt(ihi) . narrow the interval. */
 L50:
  middle = (ilo + ihi) / 2;
  if (middle == ilo) {
    goto L100;
  }
  /*     note. it is assumed that middle = ilo in case ihi = ilo+1 . */
  if (x < xt[middle]) {
    goto L53;
  }
  ilo = middle;
  goto L50;
 L53:
  ihi = middle;
  goto L50;
  /* **** set output and return. */
 L90:
  *mflag = -1;
  *left = 1;
  return 0;
 L100:
  *mflag = 0;
  *left = ilo;
  return 0;
 L110:
  *mflag = 1;
  if (x == xt[lxt]) {
    *mflag = 0;
  }
  *left = lxt;
 L111:
  if (*left == 1) {
    return 0;
  }
  --(*left);
  if (xt[*left] < xt[lxt]) {
    return 0;
  }
  goto L111;
} /* interv_ */

template <class T>
bool TestForMonotonicReal(const T*dp, int len) {
  bool monotonic = true;
  int k = 0;
  while (monotonic && (k<len-1)) {
    monotonic = dp[k] <= dp[k+1];
    k++;
  }
  return monotonic;
}


template <class T>
void DoLinearInterpolationComplex(const T* x1, const T* y1, 
				  int x1count, const T* xi,
				  int xicount, T* yi, int xtrapflag) {
  int left, mflag;
  int k;
  T frac;
  
  for (k=0;k<xicount;k++) {
    interv<T>(x1,x1count,xi[k],&left,&mflag);
    if ((mflag==0) || (xtrapflag == 3)) {
      frac = (xi[k] - x1[left-1])/(x1[left]-x1[left-1]);
      yi[2*k] = y1[2*(left-1)] + frac*(y1[2*left]-y1[2*(left-1)]);
      yi[2*k+1] = y1[2*(left-1)+1] + frac*(y1[2*left+1]-y1[2*(left-1)+1]);
    } else {
      switch (xtrapflag) {
      case 0:
	yi[2*k] = atof("nan");
	yi[2*k+1] = atof("nan");
	break;
      case 1:
	yi[2*k] = 0;
	yi[2*k+1] = 0;
	break;
      case 2:
	if (mflag == -1) {
	  yi[2*k] = y1[0];
	  yi[2*k+1] = y1[1];
	} else if (mflag == 1) {
	  yi[2*k] = y1[2*(x1count-1)];
	  yi[2*k+1] = y1[2*(x1count-1)+1];
	}
	break;
      }
    }
  }
}

template <class T>
void DoLinearInterpolationReal(const T* x1, const T* y1, 
			       int x1count, const T* xi,
			       int xicount, T* yi, int xtrapflag) {
  int left, mflag;
  int k;
  T frac;

  for (k=0;k<xicount;k++) {
    interv<T>(x1,x1count,xi[k],&left,&mflag);
    if ((mflag==0) || (xtrapflag == 3)) {
      frac = (xi[k] - x1[left-1])/(x1[left]-x1[left-1]);
      yi[k] = y1[left-1] + frac*(y1[left]-y1[left-1]);
    } else {
      switch (xtrapflag) {
      case 0:
	yi[k] = atof("nan");
	break;
      case 1:
	yi[k] = 0;
	break;
      case 2:
	if (mflag == -1)
	  yi[k] = y1[0];
	else if (mflag == 1)
	  yi[k] = y1[x1count-1];
	break;
      }
    }
  }
}

bool TestForMonotonic(Array x) {
  switch (x.getDataClass()) {
  case FM_FLOAT:
    return TestForMonotonicReal<float>((const float*) x.getDataPointer(),
				       x.getLength());
  case FM_DOUBLE:
    return TestForMonotonicReal<double>((const double*) x.getDataPointer(),
					x.getLength());
  }
}

//!
//@Module INTERPLIN1 Linear 1-D Interpolation
//@@Section CURVEFIT
//@@Usage
//Given a set of monotonically increasing @|x| coordinates and a 
//corresponding set of @|y| values, performs simple linear 
//interpolation to a new set of @|x| coordinates. The general syntax
//for its usage is
//@[
//   yi = interplin1(x1,y1,xi)
//@]
//where @|x1| and @|y1| are vectors of the same length, and the entries
//in @|x1| are monotoniccally increasing.  The output vector @|yi| is
//the same size as the input vector @|xi|.  For each element of @|xi|,
//the values in @|y1| are linearly interpolated.  For values in @|xi| 
//that are outside the range of @|x1| the default value returned is
//@|nan|.  To change this behavior, you can specify the extrapolation
//flag:
//@[
//   yi = interplin1(x1,y1,xi,extrapflag)
//@]
//Valid options for @|extrapflag| are:
//\begin{itemize}
//\item @|'nan'| - extrapolated values are tagged with @|nan|s
//\item @|'zero'| - extrapolated values are set to zero
//\item @|'endpoint'| - extrapolated values are set to the endpoint values 
//\item @|'extrap'| - linear extrapolation is performed
//\end{itemize}
//The @|x1| and @|xi| vectors must be real, although complex types
//are allowed for @|y1|.
//@@Example
//Here is an example of simple linear interpolation with the different
//extrapolation modes.  We start with a fairly coarse sampling of a 
//cosine.
//@<
//x = linspace(-pi*7/8,pi*7/8,15);
//y = cos(x);
//plot(x,y,'ro');
//mprint interplin1_1
//@>
//which is shown here
//@figure interplin1_1
//Next, we generate a finer sampling over a slightly broader range
//(in this case @|[-pi,pi]|).  First, we demonstrate the @|'nan'| 
//extrapolation method
//@<
//xi = linspace(-4,4,100);
//yi_nan = interplin1(x,y,xi,'nan');
//yi_zero = interplin1(x,y,xi,'zero');
//yi_endpoint = interplin1(x,y,xi,'endpoint');
//yi_extrap = interplin1(x,y,xi,'extrap');
//plot(x,y,'ro',xi,yi_nan,'g-x',xi,yi_zero,'g-x',xi,yi_endpoint,'g-x',xi,yi_extrap,'g-x');
//mprint interplin1_2
//@>
//which is shown here
//@figure interplin1_2
//!
ArrayVector Interplin1Function(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("interplin1 requires at least three arguments (x1,y1,xi)");
  Array x1(arg[0]);
  Array y1(arg[1]);
  Array xi(arg[2]);
    
  // Verify that x1 are numerical
  if (x1.isReferenceType() || y1.isReferenceType() || xi.isReferenceType())
    throw Exception("arguments to interplin1 must be numerical arrays");
  if (x1.isComplex() || xi.isComplex())
    throw Exception("x-coordinates cannot be complex in interplin1");
  if (x1.getDataClass() < y1.getDataClass())
    x1.promoteType(y1.getDataClass());
  else
    y1.promoteType(x1.getDataClass());
  if (x1.getDataClass() < FM_FLOAT)
    x1.promoteType(FM_FLOAT);
  if (xi.getDataClass() > x1.getDataClass())
    x1.promoteType(xi.getDataClass());
  if (x1.getDataClass() > xi.getDataClass())
    xi.promoteType(x1.getDataClass());
  // Make sure x1 and y1 are the same length
  if (x1.getLength() != y1.getLength())
    throw Exception("vectors x1 and y1 must be the same length");
  if (!TestForMonotonic(x1))
    throw Exception("vector x1 must be monotonically increasing");
  // Check for extrapolation flag
  int xtrap = 0;
  if (arg.size() == 4) {
    Array xtrapFlag(arg[3]);
    if (!xtrapFlag.isString())
      throw Exception("extrapolation flag must be a string");
    char *xtrap_c = xtrapFlag.getContentsAsCString();
    if (strcmp(xtrap_c,"nan")==0)
      xtrap = 0;
    else if (strcmp(xtrap_c,"zero")==0)
      xtrap = 1;
    else if (strcmp(xtrap_c,"endpoint")==0)
      xtrap = 2;
    else if (strcmp(xtrap_c,"extrap")==0)
      xtrap = 3;
    else
      throw Exception("unrecognized extrapolation type flag to routine interplin1");
  }
  Array retval;
  char *dp;
  switch(y1.getDataClass()) {
  case FM_FLOAT: {
    dp = (char*) Malloc(sizeof(float)*xi.getLength());
    DoLinearInterpolationReal<float>((const float*) x1.getDataPointer(),
				     (const float*) y1.getDataPointer(),
				     x1.getLength(),
				     (const float*) xi.getDataPointer(),
				     xi.getLength(), (float*) dp, xtrap);
    break;
  }
  case FM_DOUBLE: {
    dp = (char*) Malloc(sizeof(double)*xi.getLength());
    DoLinearInterpolationReal<double>((const double*) x1.getDataPointer(),
				      (const double*) y1.getDataPointer(),
				      x1.getLength(),
				      (const double*) xi.getDataPointer(),
				      xi.getLength(), (double*) dp, xtrap);
    break;
  }
  case FM_COMPLEX: {
    dp = (char*) Malloc(sizeof(float)*xi.getLength()*2);
    DoLinearInterpolationComplex<float>((const float*) x1.getDataPointer(),
					(const float*) y1.getDataPointer(),
					x1.getLength(),
					(const float*) xi.getDataPointer(),
					xi.getLength(), (float*) dp, xtrap);
    break;
  }
  case FM_DCOMPLEX: {
    dp = (char*) Malloc(sizeof(double)*xi.getLength()*2);
    DoLinearInterpolationComplex<double>((const double*) x1.getDataPointer(),
					 (const double*) y1.getDataPointer(),
					 x1.getLength(),
					 (const double*) xi.getDataPointer(),
					 xi.getLength(), (double*) dp, 
					 xtrap);
    break;
  }
  }
  return singleArrayVector(Array(y1.getDataClass(),
				 xi.getDimensions(),
				 dp));
      
}
