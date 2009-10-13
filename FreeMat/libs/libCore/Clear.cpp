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
#include "Module.hpp"
#include "Class.hpp"

//!
//@Module CLEAR Clear or Delete a Variable
//@@Section INSPECTION
//@@Usage
//Clears a set of variables from the current context, or alternately, 
//delete all variables defined in the current context.  There are
//several formats for the function call.  The first is the explicit form
//in which a list of variables are provided:
//@[
//   clear a1 a2 ...
//@]
//The variables can be persistent or global, and they will be deleted.
//The second form
//@[
//   clear 'all'
//@]
//clears all variables and libraries from the current context.  Alternately, you can
//use the form:
//@[
//   clear 'libs'
//@]
//which will unload any libraries or DLLs that have been @|import|ed. 
//Optionally, you can specify that persistent variables should be cleared via:
//@[
//   clear 'persistent'
//@]
//and similarly for global variables:
//@[
//   clear 'global'
//@]
//You can use
//@[
//   clear 'classes'
//@]
//to clear all definitions of user-defined classes.
//With no arguments, @|clear| defaults to clearing @|'all'|.
//@@Example
//Here is a simple example of using @|clear| to delete a variable.  First, we create a variable called @|a|:
//@<
//a = 53
//@>
//Next, we clear @|a| using the @|clear| function, and verify that it is deleted.
//@<1
//clear a
//a
//@>
//@@Tests
//@{ test_clear1.m
//function test_val = test_clear1
//  A = 1;
//  clear('A');
//  test_val = 0;
//  try
//    B = A;
//  catch
//    test_val = 1;
//  end;
//@}
//@{ test_clear2.m
//function test_val = test_clear2
//  A = 1;
//  clear('all');
//  test_val = 0;
//  try
//    B = A;
//  catch
//    test_val = 1;
//  end;
//@}
//@@Signature
//sfunction clear ClearFunction
//inputs varargin
//outputs none
//!

static void ClearVariable(Interpreter* eval, QString name) {
  Context *context = eval->getContext();
  ParentScopeLocker lock(context);
  context->deleteVariable(name);
}

static void ClearAllFunction(Interpreter* eval) {
  ClearLibs(eval);
  Context *context = eval->getContext();
  StringVector names;
  {
    ParentScopeLocker lock(context);
    names = context->listAllVariables();
  }
  for (int i=0;i<(int)names.size();i++)
    ClearVariable(eval,names[i]);
}

static void ClearPersistent(Interpreter* eval) {
  ParentScopeLocker lock(eval->getContext());
  StringVector names = eval->getContext()->listGlobalVariables();
  for (int i=0;i<(int)names.size();i++) {
    if ((names[i].size() >= 1) && (names[i][0] == '_'))
      eval->getContext()->deleteGlobalVariable(names[i]);
  }
  eval->getContext()->clearPersistentVariableList();
}

static void ClearGlobal(Interpreter* eval) {
  ParentScopeLocker lock(eval->getContext());
  StringVector names = eval->getContext()->listGlobalVariables();
  for (int i=0;i<(int)names.size();i++) {
    if ((names[i].size() >= 1) && (names[i][0] != '_')) {
      eval->getContext()->deleteGlobalVariable(names[i]);
    }
  }
  eval->getContext()->clearGlobalVariableList();
}

ArrayVector ClearFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  StringVector names;
  if (arg.size() == 0) 
    names.push_back("all");
  else
    for (int i=0;i<arg.size();i++) 
      names.push_back(arg[i].asString());
  for (int i=0;i<(int)names.size();i++) {
    if (names[i] == "all")
      ClearAllFunction(eval);
    else if (names[i] == "libs")
      ClearLibs(eval);
    else if (names[i] == "persistent")
      ClearPersistent(eval);
    else if (names[i] == "global")
      ClearGlobal(eval);
    else if (names[i] == "classes")
      clearUserClasses();
    else 
      ClearVariable(eval,names[i]);
  }
  return ArrayVector();
}
