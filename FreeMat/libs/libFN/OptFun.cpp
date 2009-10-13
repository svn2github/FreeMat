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
#include "FuncPtr.hpp"

#include "lm.h"

typedef struct {
  ArrayVector params;
  Array xval;
  Array yval;
  Array wval;
  Interpreter *a_eval;
  FunctionDef *a_funcDef;
} FncBlock;

void fcnstub(double *p, double *hx, int m, int n, void *adata) {
  FncBlock *q = (FncBlock*) adata;
  double *xp, *yp, *rp, *wp;
  xp = q->xval.real<double>().data();
  yp = q->yval.real<double>().data();
  wp = q->wval.real<double>().data();
  memcpy(xp,p,sizeof(double)*m);
  ArrayVector tocall(q->params);
  tocall.push_front(q->xval);
  ArrayVector cval(q->a_eval->doFunction(q->a_funcDef,tocall,1));
  if (cval.size() == 0)
    throw Exception("function to be optimized does not return any outputs!");
  if (int(cval[0].length()) != n)
    throw Exception("function output does not match size of vector 'y'");
  Array f(cval[0]);
  f = f.asDenseArray().toClass(Double);
  rp = (double*) f.real<double>().data();
  int i;
  for (i=0;i<n;i++) {
    hx[i] = wp[i] * rp[i];//wp[i]*(yp[i] - rp[i]);
  }
}

//!
//@Module FITFUN Fit a Function
//@@Section CURVEFIT
//@@Usage
//Fits @|n| (non-linear) functions of @|m| variables using least squares
//and the Levenberg-Marquardt algorithm.  The general syntax for its usage
//is
//@[
//  [xopt,yopt] = fitfun(fcn,xinit,y,weights,tol,params...)
//@]
//Where @|fcn| is the name of the function to be fit, @|xinit| is the
//initial guess for the solution (required), @|y| is the right hand side,
//i.e., the vector @|y| such that:
//\[
//   xopt = \arg \min_{x} \|\mathrm{diag}(weights)*(f(x) - y)\|_2^2,
//\]
//the output @|yopt| is the function @|fcn| evaluated at @|xopt|.  
//The vector @|weights| must be the same size as @|y|, and contains the
//relative weight to assign to an error in each output value.  Generally,
//the ith weight should reflect your confidence in the ith measurement.
//The parameter @|tol| is the tolerance used for convergence.
//The function @|fcn| must return a vector of the same size as @|y|,
//and @|params| are passed to @|fcn| after the argument @|x|, i.e.,
//\[
//  y = fcn(x,param1,param2,...).
//\]
//Note that both @|x| and @|y| (and the output of the function) must all
//be real variables.  Complex variables are not handled yet.
//@@Tests
//@{ test_fitfun1.m
//% Test the fitfun bug (bug 1514605)
//function test_val = test_fitfun1
//O = 3/4*pi;
//y0 = cos(O);
//[x y] = fitfun('cos',3.5/4*pi,y0,1,0.0001);
//test_val = abs((x-O)/O*100)<.1;
//@}
//@{ test_fitfun2.m
//% Test the nested fitfun bug (bug 1741350)
//function test_val = test_fitfun2
//a = 2;
//b = 0;
//init = [a,b];
//y = [1:100];
//weights = y*0+1;
//tol = 1.e-08;
//junk = rand(100);
//[xopt,yopt] = fitfun('fitfunc_func1',init,y,weights,tol,junk);
//test_val = abs(xopt(1)-1) < 1e-6;
//@}
//@{ fitfunc_func1.m
//function y = fitfunc_func1(init,junk)
// c = 2;
// d = 0;
// init2 = [c,d];
// y2 = [1:100]*3;
// weights2 = y2*0+1;
// tol2 = 1.e-08;
// junk2 = rand(100);
// [xopt2,yopt2] = fitfun('fitfunc_func2',init2,y2,weights2,tol2,junk2);
// a = init(1);
// b = init(2);
// x = [1:100];
// y = a*x + b;
//@}
//@{ fitfunc_func2.m
//function y = fitfunc_func2(init,junk);
// a = init(1);
// b = init(2);
// x = [1:100];
// y = a*x+b;
//@}
//@{ test_fitfun3.m
//% Test the fitfun with a local function
//function test_val = test_fitfun3
//O = 3/4*pi;
//y0 = cos(O);
//[x y] = fitfun(@locos,3.5/4*pi,y0,1,0.0001);
//test_val = abs((x-O)/O*100)<.1;
//
//function y = locos(x)
//  y = cos(x);
//@}
//@@Signature
//sfunction fitfun FitFunFunction
//inputs fcn xinit y weights tol varargin
//outputs xopt yopt
//!
ArrayVector FitFunFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size()<4) 
    throw Exception("fitfun requires at least four arguments");
  FuncPtr funcDef;
  Array fptr(arg[0]);
  if (fptr.isString())  {
    QString fname = fptr.asString();
    Context *context = eval->getContext();
    eval->rescanPath();
    if (!context->lookupFunction(fname,funcDef))
      throw Exception(QString("function ") + fname + " undefined!");
    funcDef->updateCode(eval);
    if (funcDef->scriptFlag)
      throw Exception("cannot use feval on a script");
  } else {
    if (fptr.isUserClass() && (fptr.className() == "functionpointer")) 
      funcDef = FuncPtrLookup(eval,fptr);
    else
      throw Exception("first argument to fitfun must be the name of a function (i.e., a string) or a function pointer");
  }
  FncBlock q;
  q.a_funcDef = funcDef;
  q.a_eval = eval;
  // Get the initial guess vector
  Array xinit(arg[1].asDenseArray().toClass(Double));
  int m, n;
  m = int(xinit.length());
  // Get the right hand side vector
  Array yvec(arg[2].asDenseArray().toClass(Double));
  n = int(yvec.length());
  q.yval = yvec;
  q.xval = xinit;
  q.wval = arg[3].asDenseArray().toClass(Double);
  if (int(q.wval.length()) != n)
    throw Exception("weight vector must be the same size as the output vector y");
  // Multiply the weights time the RHS
  for (index_t i=1;i<=n;i++) {
    q.yval.real<double>()[i] = q.yval.constReal<double>()[i] * 
      q.wval.constReal<double>()[i];
  }
  // Get the tolerance
  double tol;
  if (arg.size() > 4)
    tol = arg[4].asDouble();
  else
    tol = LM_STOP_THRESH;
  // Copy the arg array
  q.params = arg;
  q.params.pop_front();
  q.params.pop_front();
  q.params.pop_front();
  q.params.pop_front();
  q.params.pop_front();
  // Test to make sure the function works....
  ArrayVector tocall(q.params);
  tocall.push_front(xinit);
  ArrayVector cval(eval->doFunction(funcDef,tocall,1));
  if (cval.size() == 0)
    throw Exception("function to be optimized does not return any outputs!");
  if (int(cval[0].length()) != n)
    throw Exception("function output does not match size of vector 'y'");
  double opts[5];
  opts[0] = LM_INIT_MU;
  opts[1] = tol;
  opts[2] = tol;
  opts[3] = tol*1e-5;
  opts[4] = LM_DIFF_DELTA;
  dlevmar_dif(fcnstub,&(q.xval.real<double>()[1]),&(q.yval.real<double>()[1]),m,n,200*(m+1),opts,NULL,NULL,NULL,&q);
  tocall = q.params;
  tocall.push_front(q.xval);
  cval = eval->doFunction(funcDef,tocall,1);
  ArrayVector retval;
  retval.push_back(q.xval);
  retval.push_back(cval[0]);
  return retval;
}
