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
#include "Scanner.hpp"
#include "Parser.hpp"
#include "System.hpp"
#include <QtCore>
#include "Algorithms.hpp"
#include "PathSearch.hpp"

#include "CArray.hpp"
#include "CJIT.hpp"
#include "CJitFuncClang.hpp"


//!
//@Module SIMKEYS Simulate Keypresses from the User
//@@Section FREEMAT
//@@Usage
//This routine simulates keystrokes from the user on FreeMat.
//The general syntax for its use is
//@[
//   otext = simkeys(text)
//@]
//where @|text| is a string to simulate as input to the console.
//The output of the commands are captured and returned in the 
//string @|otext|.  This is primarily used by the testing 
//infrastructure.
//@@Signature
//sfunction simkeys SimKeysFunction
//inputs itext
//outputs otext
//!
ArrayVector SimKeysFunction(int nargout, const ArrayVector& arg,
			    Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("simkeys requires at least one argument (the cell array of strings to simulate)");
  ParentScopeLocker lock(eval->getContext());
  eval->clearCaptureString();
  eval->setCaptureState(true);
  if (arg[0].dataClass() != CellArray)
    throw Exception("simkeys requires a cell array of strings");
  const BasicArray<Array> &dp(arg[0].constReal<Array>());
  for (index_t i=1;i<=dp.length();i++) {
    QString txt(dp[i].asString());
    if ((txt.size() > 0) && (!txt.endsWith('\n')))
      txt.push_back('\n');
    eval->ExecuteLine(txt);
  }
  eval->ExecuteLine("quit\n");
  try {
    while(1) 
      eval->evalCLI();
  } catch (InterpreterContinueException& e) {
  } catch (InterpreterBreakException& e) {
  } catch (InterpreterReturnException& e) {
  } catch (InterpreterRetallException& e) {
  } catch (InterpreterQuitException& e) {
  }
  eval->setCaptureState(false);
  return ArrayVector(Array(eval->getCaptureString()));
}

//!
//@Module DIARY Create a Log File of Console
//@@Section FREEMAT
//@@Usage
//The @|diary| function controls the creation of a log file that duplicates
//the text that would normally appear on the console.
//The simplest syntax for the command is simply:
//@[
//   diary
//@]
//which toggles the current state of the diary command.  You can also explicitly
//set the state of the diary command via the syntax
//@[
//   diary off
//@]
//or
//@[
//   diary on
//@]
//To specify a filename for the log (other than the default of @|diary|), you 
//can use the form:
//@[
//   diary filename
//@]
//or
//@[
//   diary('filename')
//@]
//which activates the diary with an output filename of @|filename|.  Note that the
//@|diary| command is thread specific, but that the output is appended to a given
//file.  That means that if you call @|diary| with the same filename on multiple 
//threads, their outputs will be intermingled in the log file (just as on the console).
//Because the @|diary| state is tied to individual threads, you cannot retrieve the
//current diary state using the @|get(0,'Diary')| syntax from MATLAB.  Instead, you
//must call the @|diary| function with no inputs and one output:
//@[
//   state = diary
//@]
//which returns a logical @|1| if the output of the current thread is currently going to
//a diary, and a logical @|0| if not.
//@@Signature
//sfunction diary DiaryFunction
//inputs x
//outputs state
//!
ArrayVector DiaryFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (nargout == 1) {
    if (arg.size() > 0)
      throw Exception("diary function with an assigned return value (i.e, 'x=diary') does not support any arguments");
    return ArrayVector(Array(bool(eval->getDiaryState())));
  }
  if (arg.size() == 0) {
    eval->setDiaryState(!eval->getDiaryState());
    return ArrayVector();
  }
  QString diaryString(arg[0].asString());
  if (diaryString.toLower() == "on")
    eval->setDiaryState(true);
  else if (diaryString.toLower() == "off")
    eval->setDiaryState(false);
  else {
    eval->setDiaryFilename(diaryString);
    eval->setDiaryState(true);
  }
  return ArrayVector();
}


//!
//@Module QUIET Control the Verbosity of the Interpreter
//@@Section FREEMAT
//@@Usage
//The @|quiet| function controls how verbose the interpreter
//is when executing code.  The syntax for the function is
//@[
//   quiet flag
//@]
//where @|flag| is one of
//\begin{itemize}
//\item @|'normal'| - normal output from the interpreter
//\item @|'quiet'| - only intentional output (e.g. @|printf| calls and
//@|disp| calls) is printed.  The output of expressions that are not
//terminated in semicolons are not printed.
//\item @|'silent'| - nothing is printed to the output.
//\end{itemize}
//The @|quiet| command also returns the current quiet flag.
//@@Signature
//sfunction quiet QuietFunction
//inputs mode
//outputs mode
//!
ArrayVector QuietFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() > 0) {
    QString qtype(arg[0].asString().toUpper());
    if (qtype == "NORMAL")
      eval->setQuietLevel(0);
    else if (qtype == "QUIET")
      eval->setQuietLevel(1);
    else if (qtype == "SILENT")
      eval->setQuietLevel(2);
    else
      throw Exception("quiet function takes one argument - the quiet level (normal, quiet, or silent) as a string");
  }
  QString rtype;
  if (eval->getQuietLevel() == 0)
    rtype = "normal";
  else if (eval->getQuietLevel() == 1)
    rtype = "quiet";
  else if (eval->getQuietLevel() == 2)
    rtype = "silent";
  return ArrayVector(Array(rtype));
}

//!
//@Module SOURCE Execute an Arbitrary File
//@@Section FREEMAT
//@@Usage
//The @|source| function executes the contents of the given
//filename one line at a time (as if it had been typed at
//the @|-->| prompt).  The @|source| function syntax is
//@[
//  source(filename)
//@]
//where @|filename| is a @|string| containing the name of
//the file to process. 
//@@Example
//First, we write some commands to a file (note that it does
//not end in the usual @|.m| extension):
//@{ source_test
//a = 32;
//b = a;
//printf('a is %d and b is %d\n',a,b);
//@}
//Now we source the resulting file.
//@<
//clear a b
//source source_test
//@>
//@@Tests
//@{ source_test_script.m
//n = 1;
//n = n + 1;
//@}
//@{ test_source.m
//% Check that the source function does not double-execute the last line of the script
//function test_val = test_source
//myloc = which('test_source');
//[path,name,sfx] = fileparts(myloc);
//source([path '/source_test_script.m'])
//test_val = test(n == 2);
//@}
//@@Signature
//sfunction source SourceFunction
//inputs filename
//outputs none
//!
ArrayVector SourceFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("source function takes exactly one argument - the filename of the script to execute");
  QString filename = arg[0].asString();
  QFile fp(filename);
  if (!fp.open(QFile::ReadOnly))
    throw Exception("unable to open file " + filename + " for reading");
  QTextStream fstr(&fp);
  QString scriptText(fstr.readAll());
  if (!scriptText.endsWith("\n")) scriptText += "\n";
  Scanner S(scriptText,filename);
  Parser P(S);
  Tree pcode(P.process());
  if (pcode.is(TOK_FUNCTION_DEFS))
    throw Exception("only scripts can be source-ed, not functions");
  Tree code = pcode.first();
  ParentScopeLocker lock(eval->getContext());
  eval->block(code);
  return ArrayVector();
}

//!
//@Module BUILTIN Evaulate Builtin Function
//@@Section FREEMAT
//@@Usage
//The @|builtin| function evaluates a built in function
//with the given name, bypassing any overloaded functions.
//The syntax of @|builtin| is
//@[
//  [y1,y2,...,yn] = builtin(fname,x1,x2,...,xm)
//@]
//where @|fname| is the name of the function to call.  Apart
//from the fact that @|fname| must be a string, and that @|builtin|
//always calls the non-overloaded method, it operates exactly like
//@|feval|.  Note that unlike MATLAB, @|builtin| does not force
//evaluation to an actual compiled function.  It simply subverts
//the activation of overloaded method calls.
//@@Tests
//@{ test_builtin1.m
//function test_val = test_builtin1
//  a = -1;
//  b = builtin('abs',a);
//  test_val = (b == 1);
//end 
//function y = abs(x)
//  y = x;
//end
//@}
//@@Signature
//sfunction builtin BuiltinFunction
//inputs fname varargin
//outputs varargout
//!
ArrayVector BuiltinFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("builtin function requires at least one argument");
  if (!(arg[0].isString()))
    throw Exception("first argument to builtin must be the name of a function (i.e., a string)");
  FuncPtr funcDef;
  QString fname = arg[0].asString();
  Context *context = eval->getContext();
  if (!context->lookupFunction(fname,funcDef))
    throw Exception("function " + fname + " undefined!");
  funcDef->updateCode(eval);
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  ArrayVector newarg(arg);
  newarg.pop_front();
  bool flagsave = eval->getStopOverload();
  eval->setStopOverload(true);
  ArrayVector tmp(eval->doFunction(funcDef,newarg,nargout));
  eval->setStopOverload(flagsave);
  return tmp;
}

//!
//@Module STARTUP Startup Script
//@@Section FREEMAT
//@@Usage
//Upon starting, FreeMat searches for a script names @|startup.m|, and
//if it finds it, it executes it.  This script can be in the current
//directory, or on the FreeMat path (set using @|setpath|).  The contents
//of startup.m must be a valid script (not a function).  
//!

//!
//@Module DOCLI Start a Command Line Interface
//@@Section FREEMAT
//@@Usage
//The @|docli| function is the main function that you interact with 
//when you run FreeMat.  I am not sure why you would want to use
//it, but hey - its there if you want to use it.
//@@Signature
//sfunction docli DoCLIFunction
//inputs none
//outputs none
//!
ArrayVector DoCLIFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  Context *context = eval->getContext();
  context->deactivateCurrentScope();
  FuncPtr funcDef;
  if (eval->lookupFunction("startup",funcDef)) {
    funcDef->updateCode(eval);
    if (funcDef->scriptFlag) {
      try {
	eval->block(((MFunctionDef*)funcDef)->code);
      } catch (Exception& e) {
	eval->errorMessage("Startup script error:\n" + e.msg());
      }
    } else {
      eval->outputMessage(QString("startup.m must be a script"));
    }
  }
  eval->doCLI();
  return ArrayVector();
}

//!
//@Module GETENV Get the Value of an Environment Variable
//@@Section OS
//@@Usage
//The @|getenv| function returns the value for an environment
//variable from the underlying OS.  The syntax for the @|getenv|
//function is 
//@[
//   y = getenv(environment_variable)
//@]
//where @|environment_variable| is the name of the environment
//variable to return.  The return is a string.
//@@Example
//Here is an example of using the @|getenv| function to
//get the value for the @|HOME| variable
//@<
//getenv('HOME')
//@>
//@@Signature
//function getenv GetEnvFunction
//inputs var
//outputs value
//!
ArrayVector GetEnvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) throw Exception("getenv requires one string argument");
  QString name(arg[0].asString());
  QByteArray ret = qgetenv(qPrintable(name));
  return Array(QString(ret));
}

//!
//@Module SYSTEM Call an External Program
//@@Section OS
//@@Usage
//The @|system| function allows you to call an external
//program from within FreeMat, and capture the output.
//The syntax of the @|system| function is
//@[
//  y = system(cmd)
//@]
//where @|cmd| is the command to execute.  The return
//array @|y| is of type @|cell-array|, where each entry
//in the array corresponds to a line from the output.
//@@Example
//Here is an example of calling the @|ls| function (the
//list files function under Un*x-like operating system).
//@<
//y = system('ls a*.m')
//y{1}
//@>
//@@Signature
//function system SystemFunction
//inputs cmd
//outputs results
//!
ArrayVector SystemFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("System function takes one string argument");
  QString systemArg(arg[0].asString());
  if (systemArg.size() == 0) 
    return ArrayVector();
  StringVector cp(DoSystemCallCaptured(systemArg));
  return CellArrayFromStringVector(cp);
}





