/*
 * Copyright (c) 2009 Samit Basu
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
#include "PathSearch.hpp"
#include <QtCore>

static int ExistBuiltinFunction(QString fname, Interpreter* eval) {    
  bool isDefed;
  FuncPtr d;
  isDefed = eval->getContext()->lookupFunction(fname,d);
  if (isDefed && ((d->type() == FM_BUILT_IN_FUNCTION) ||
		  (d->type() == FM_SPECIAL_FUNCTION)))
    return 5;
  else
    return 0;
  return 0;
}

static int ExistDirFunction(QString fname, Interpreter* eval) {
  // Check for extra termination
  int flen = fname.size();
  if ((fname[flen-1] == '/') ||
      (fname[flen-1] == '\\'))
    fname[flen-1] = 0;
  QFileInfo filestat(fname);
  if (!filestat.exists()) return 0;
  if (filestat.isDir()) return 7;
  return 0;
}

static int ExistFileFunction(QString fname, Interpreter* eval) {
  PathSearcher src(eval->getPath());
  QString path = src.ResolvePath(fname);
  if( !path.isNull() )
	return 2;

  bool isDefed;
  FuncPtr d;
  isDefed = eval->getContext()->lookupFunction(fname,d);
  if (isDefed && (d->type() == FM_M_FUNCTION))
    return 2;
  return 0;
}

static int ExistVariableFunction(QString fname, Interpreter* eval) {
  ParentScopeLocker lock(eval->getContext());
  bool isDefed = (eval->getContext()->lookupVariable(fname).valid());
  if (isDefed)
    return 1;
  else
    return 0;
}

static int ExistAllFunction(QString fname, Interpreter* eval) {
  int ret;
  ret = ExistVariableFunction(fname,eval);
  if (ret) return ret;
  ret = ExistFileFunction(fname,eval);
  if (ret) return ret;
  ret = ExistDirFunction(fname,eval);
  if (ret) return ret;
  ret = ExistBuiltinFunction(fname,eval);
  if (ret) return ret;
  return 0;
}

//!
//@Module EXIST Test for Existence
//@@Section INSPECTION
//@@Usage
//Tests for the existence of a variable, function, directory or
//file.  The general syntax for its use is
//@[
//  y = exist(item,kind)
//@]
//where @|item| is a string containing the name of the item
//to look for, and @|kind| is a string indicating the type 
//of the search.  The @|kind| must be one of 
//\begin{itemize}
//\item @|'builtin'| checks for built-in functions
//\item @|'dir'| checks for directories
//\item @|'file'| checks for files
//\item @|'var'| checks for variables
//\item @|'all'| checks all possibilities (same as leaving out @|kind|)
//\end{itemize}
//You can also leave the @|kind| specification out, in which case
//the calling syntax is
//@[
//  y = exist(item)
//@]
//The return code is one of the following:
//\begin{itemize}
//\item 0 - if @|item| does not exist
//\item 1 - if @|item| is a variable in the workspace
//\item 2 - if @|item| is an M file on the search path, a full pathname
// to a file, or an ordinary file on your search path
//\item 5 - if @|item| is a built-in FreeMat function
//\item 7 - if @|item| is a directory
//\end{itemize}
//Note: previous to version @|1.10|, @|exist| used a different notion
//of existence for variables: a variable was said to exist if it 
//was defined and non-empty.  This test is now performed by @|isset|.
//@@Example
//Some examples of the @|exist| function.  Note that generally @|exist|
//is used in functions to test for keywords.  For example,
//@[
//  function y = testfunc(a, b, c)
//  if (~exist('c'))
//    % c was not defined, so establish a default
//    c = 13;
//  end
//  y = a + b + c;
//@]
//An example of @|exist| in action.
//@<
//a = randn(3,5,2)
//b = []
//who
//exist('a')
//exist('b')
//exist('c')
//@>
//@@Tests
//@{ test_exist1.m
//function x = test_exist1
//x = test_exist1_assist;
//x = test_exist1_assist;
//
//function y = test_exist1_assist
//persistent x
//if (exist('x'))
//  y = 1;
//else
//  y = 0;
//  x = 1;
//end
//@}
//@{ test_exist2.m
//function x = test_exist2
//persistent y
//x = 1;
//if (~exist('y'))
//  x = 0;
//end
//@}
//@@Signature
//sfunction exist ExistFunction
//inputs item kind
//outputs y
//!
ArrayVector ExistFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("exist function takes at least one argument - the name of the object to check for");
  QString fname = arg[0].asString();
  QString stype;
  if (arg.size() > 1) {
    stype = arg[1].asString();
  } else {
    stype = "all";
  }
  int retval;
  if (stype=="all")
    retval = ExistAllFunction(fname,eval);
  else if (stype=="builtin")
    retval = ExistBuiltinFunction(fname,eval);
  else if (stype=="dir")
    retval = ExistDirFunction(fname,eval);
  else if (stype=="file")
    retval = ExistFileFunction(fname,eval);
  else if (stype=="var")
    retval = ExistVariableFunction(fname,eval);
  else throw Exception("Unrecognized search type for function 'exist'");
  return ArrayVector(Array(double(retval)));
}
