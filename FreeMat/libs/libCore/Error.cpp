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

//!
//@Module LASTERR Retrieve Last Error Message
//@@Section FLOW
//@@Usage
//Either returns or sets the last error message.  The
//general syntax for its use is either
//@[
//  msg = lasterr
//@]
//which returns the last error message that occured, or
//@[
//  lasterr(msg)
//@]
//which sets the contents of the last error message.
//@@Example
//Here is an example of using the @|error| function to
//set the last error, and then retrieving it using
//lasterr.
//@<
//try; error('Test error message'); catch; end;
//lasterr
//@>
//Or equivalently, using the second form:
//@<
//lasterr('Test message');
//lasterr
//@>
//@@Tests
//@{ test_lasterr1.m
//function test_val = test_lasterr1
//  try
//    error('Hulabaloo');
//  catch;
//  end;
//  test_val = strcmp(lasterr,'Hulabaloo');
//@}
//@@Signature
//sfunction lasterr LasterrFunction
//inputs msg
//outputs msg
//!
ArrayVector LasterrFunction(int nargout, const ArrayVector& arg,
			    Interpreter* eval) {
  if (arg.size() == 0) 
    return ArrayVector(Array(eval->getLastErrorString()));
  eval->setLastErrorString(arg[0].asString());
  return ArrayVector();
}


//!
//@Module ERRORCOUNT Retrieve the Error Counter for the Interpreter
//@@Section FREEMAT
//@@Usage
//This routine retrieves the internal counter for the interpreter,
//and resets it to zero.  The general syntax for its use is
//@[
//   count = errorcount
//@]
//@@Signature
//sfunction errorcount ErrorCountFunction
//inputs none
//outputs count
//!
ArrayVector ErrorCountFunction(int nargout, const ArrayVector& arg,
			       Interpreter* eval) {
  return ArrayVector(Array(double(eval->getErrorCount())));
}

//!
//@Module WARNING Emits a Warning Message
//@@Section FLOW
//@@Usage
//The @|warning| function causes a warning message to be
//sent to the user.  The general syntax for its use is
//@[
//   warning(s)
//@]
//where @|s| is the string message containing the warning.
//@@Signature
//sfunction warning WarningFunction
//inputs msg
//outputs none
//!
ArrayVector WarningFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("Not enough inputs to warning function");
  if (!(arg[0].isString()))
    throw Exception("Input to error function must be a string");
  eval->warningMessage(arg[0].asString());
  return ArrayVector();
}

//!
//@Module ERROR Causes an Error Condition Raised
//@@Section FLOW
//@@Usage
//The @|error| function causes an error condition (exception
//to be raised).  The general syntax for its use is
//@[
//   error(s),
//@]
//where @|s| is the string message describing the error.  The
//@|error| function is usually used in conjunction with @|try|
//and @|catch| to provide error handling.  If the string @|s|,
//then (to conform to the MATLAB API), @|error| does nothing.
//@@Example
//Here is a simple example of an @|error| being issued by a function
//@|evenoddtest|:
//@{ evenoddtest.m
//function evenoddtest(n)
//  if (n==0)
//    error('zero is neither even nor odd');
//  elseif ( n ~= fix(n) )
//    error('expecting integer argument');
//  end;
//  if (n==int32(n/2)*2)
//    printf('%d is even\n',n);
//  else
//    printf('%d is odd\n',n);
//  end
//@}
//The normal command line prompt @|-->| simply prints the error
//that occured.
//@<2
//evenoddtest(4)
//evenoddtest(5)
//evenoddtest(0)
//evenoddtest(pi)
//@>
//@@Tests
//@{ test_error1.m
//function test_val = test_error1
//  test_val = false;
//  try
//     error('test')
//  catch
//     test_val = true;
//  end
//@}
//@@Signature
//function error ErrorFunction
//inputs string
//outputs none
//!
ArrayVector ErrorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector();
  QString etxt(arg[0].asString());
  if (!etxt.isEmpty()) 
    throw Exception(etxt);
  return ArrayVector();
}
