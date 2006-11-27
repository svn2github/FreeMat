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
#include "Malloc.hpp"
#include "PathSearch.hpp"
#include "IEEEFP.hpp"
#include "Sparse.hpp"
#include "helpwidget.hpp"
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "Editor.hpp"
#include "PathTool.hpp"
#include <QtCore>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <qapplication.h>
#include <QDir>
#ifdef WIN32
#include <direct.h>
#define getcwd _getcwd
//#define S_ISDIR(x) (x & _S_IFDIR)
#define P_DELIM ";"
#else
#define P_DELIM ":"
#endif
#include <QtGui>
#include "Module.hpp"

static std::string helppath;
  
void Tokenize(const std::string& str, std::vector<std::string>& tokens,
	      const std::string& delimiters = " \n") {
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
  while (std::string::npos != pos || std::string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

//!
//@Module DBAUTO Control Dbauto Functionality
//@@Section DEBUG
//@@Usage
//The dbauto functionality in FreeMat allows you to debug your
//FreeMat programs.  When @|dbauto| is @|on|, then any error
//that occurs while the program is running causes FreeMat to 
//stop execution at that point and return you to the command line
//(just as if you had placed a @|keyboard| command there).  You can
//then examine variables, modify them, and resume execution using
//@|return|.  Alternately, you can exit out of all running routines
//via a @|retall| statement.  Note that errors that occur inside of
//@|try|/@|catch| blocks do not (by design) cause auto breakpoints.  The
//@|dbauto| function toggles the dbauto state of FreeMat.  The
//syntax for its use is
//@[
//   dbauto(state)
//@]
//where @|state| is either
//@[
//   dbauto('on')
//@]
//to activate dbauto, or
//@[
//   dbauto('off')
//@]
//to deactivate dbauto.  Alternately, you can use FreeMat's string-syntax
//equivalence and enter
//@[
//   dbauto on
//@]
//or 
//@[
//   dbauto off
//@]
//to turn dbauto on or off (respectively).  Entering @|dbauto| with no arguments
//returns the current state (either 'on' or 'off').
//!
ArrayVector DbAutoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) {
    if (eval->AutoStop()) 
      return singleArrayVector(Array::stringConstructor("on"));
    else 
      return singleArrayVector(Array::stringConstructor("off"));
  } else {
    if (!arg[0].isString())
      throw Exception("dbauto function takes only a single, string argument");
    char *txt;
    txt = arg[0].getContentsAsCString();
    if (strcmp(txt,"on") == 0)
      eval->AutoStop(true);
    else if (strcmp(txt,"off") == 0)
      eval->AutoStop(false);
    else
      throw Exception("dbauto function argument needs to be 'on/off'");
  }
  return ArrayVector();
}

bool inBundleMode() {
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  return (dir.dirName() == "Contents");
}

//!
//@Module HELPWIN Online Help Window
//@@Section FREEMAT
//@@Usage
//Brings up the online help window with the FreeMat manual.  The
//@|helpwin| function takes no arguments:
//@[
//  helpwin
//@]
//!
ArrayVector HelpWinFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  QDir dir;
  if (inBundleMode()) {
    dir = QDir(QString(qApp->applicationDirPath() + "/../Resources/help/html"));
  } else {
    dir = QDir(QString(BASEPATH)+"/help/html");
  }
  HelpWindow *m_helpwin = new HelpWindow(dir.canonicalPath());
  m_helpwin->show();
  return ArrayVector();
}

//!
//@Module EDITOR Open Editor Window
//@@Section FREEMAT
//@@Usage
//Brings up the editor window.  The @|editor| function takes no
//arguments:
//@[
//  editor
//@]
//!
ArrayVector EditorFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  static FMEditor *edit = NULL;
  if (edit == NULL) {
    edit = new FMEditor(eval);
    QObject::connect(eval,SIGNAL(RefreshBPLists()),edit,SLOT(RefreshBPLists()));
    QObject::connect(eval,SIGNAL(ShowActiveLine()),edit,SLOT(ShowActiveLine()));
  }
  edit->showNormal();
  edit->raise();
  return ArrayVector();
}

ArrayVector EndFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("End function requires 3 arguments, the array, the end index, and the number of subindexes");
  Dimensions t(arg[0].getDimensions());
  int enddim(ArrayToInt32(arg[1]));
  int totalndxs(ArrayToInt32(arg[2]));
  if (totalndxs == 1)
    return singleArrayVector(Array::int32Constructor(t.getElementCount()));
  return singleArrayVector(Array::int32Constructor(t.get(enddim-1)));
}

//!
//@Module PATHTOOL Open Path Setting Tool
//@@Section FREEMAT
//@@Usage
//Brings up the pathtool dialog.  The @|pathtool| function takes
//no arguments:
//@[
//  pathtool
//@]
//!
ArrayVector PathToolFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  PathTool p;
  p.exec();
  QSettings settings("FreeMat","FreeMat");
  QStringList userPath = settings.value("interpreter/path").toStringList();
  eval->setUserPath(userPath);
  eval->rescanPath();
  return ArrayVector();
}
  
//!
//@Module HELP Help
//@@Section FREEMAT
//@@Usage
//Displays help on a function available in FreeMat.  The help function
//takes one argument:
//@[
//  help topic
//@]
//where @|topic| is the topic to look for help on.  For scripts, the 
//result of running @|help| is the contents of the comments at the top
//of the file.  If FreeMat finds no comments, then it simply displays
//the function declaration.
//!
ArrayVector HelpFunction(int nargout, const ArrayVector& arg, Interpreter* eval)
{
  PathSearcher psearch(eval->getTotalPath());

  if (arg.size() != 1)
    throw Exception("help function requires a single argument (the function or script name)");
  Array singleArg(arg[0]);
  char *fname;
  fname = singleArg.getContentsAsCString();
  bool isFun;
  FuncPtr val;
  isFun = eval->getContext()->lookupFunction(fname,val);
  if (isFun && (val->type() == FM_M_FUNCTION)) {
    MFunctionDef *mptr;
    mptr = (MFunctionDef *) val;
    mptr->updateCode();
    for (int i=0;i<mptr->helpText.size();i++)
      eval->outputMessage(mptr->helpText[i].c_str());
    return ArrayVector();
  } else {
    // Check for a mdc file with the given name
    std::string mdcname;
    mdcname = std::string(fname) + ".mdc";
    try {
      mdcname = psearch.ResolvePath(mdcname);
    } catch (Exception& e) {
      throw Exception(std::string("no help available on ") + fname);
    }
    FILE *fp;
    fp = fopen(mdcname.c_str(),"r");
    if (fp) {
      //Found it... relay to the output
      std::vector<std::string> helplines;
      std::string workingline;
      char buffer[4096];
      while (!feof(fp)) {
	fgets(buffer,sizeof(buffer),fp);
	//is this line only a $ sign?
	helplines.push_back(buffer);
      }
      // Write the lines out...
      // Get the output width (in characters)
      int outputWidth = eval->getTerminalWidth() - 20;
      for (int p=0;p<helplines.size();p++) {
	std::vector<std::string> tokens;
	// Tokenize the help line
	Tokenize(helplines[p],tokens);
	// Output words..
	int outlen = 0;
	int tokencount = 0;
	eval->outputMessage("\n          ");
	while ((tokens.size() > 0) && (tokencount < tokens.size())) {
	  // Can the next token be output without wrapping?
	  int tsize;
	  tsize = tokens[tokencount].size();
	  if ((outlen == 0) || ((outlen + tsize) < outputWidth)) {
	    // Yes... send it and move on
	    eval->outputMessage(tokens[tokencount].c_str());
	    eval->outputMessage(" ");
	    outlen += tokens[tokencount++].size()+1;
	  } else {
	    eval->outputMessage("\n          ");
	    outlen = 0;
	  }
	}
      }
      fclose(fp);
    }
    return ArrayVector();
  }
  throw Exception("no help for that topic");
}

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
//!

void ClearVariable(Interpreter* eval, string name) {
  eval->getContext()->deleteVariable(name);
}

void ClearAllFunction(Interpreter* eval) {
  ClearLibs(eval);
  stringVector names = eval->getContext()->getCurrentScope()->listAllVariables();
  for (int i=0;i<names.size();i++)
    ClearVariable(eval,names[i]);
}

void ClearPersistent(Interpreter* eval) {
  stringVector names = eval->getContext()->getGlobalScope()->listAllVariables();
  for (int i=0;i<names.size();i++) {
    if ((names[i].size() >= 1) && (names[i][0] == '_'))
      eval->getContext()->getGlobalScope()->deleteVariable(names[i]);
  }
  eval->getContext()->getGlobalScope()->clearPersistentVariableList();
  eval->getContext()->getCurrentScope()->clearPersistentVariableList();
}

void ClearGlobal(Interpreter* eval) {
  stringVector names = eval->getContext()->getGlobalScope()->listAllVariables();
  for (int i=0;i<names.size();i++) {
    if ((names[i].size() >= 1) && (names[i][0] != '_')) {
      eval->getContext()->getGlobalScope()->deleteVariable(names[i]);
    }
  }
  eval->getContext()->getGlobalScope()->clearGlobalVariableList();
  eval->getContext()->getCurrentScope()->clearGlobalVariableList();
}

ArrayVector ClearFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  stringVector names;
  if (arg.size() == 0) 
    names.push_back("all");
  else
    for (int i=0;i<arg.size();i++) 
      names.push_back(ArrayToString(arg[i]));
  for (int i=0;i<names.size();i++) {
    if (names[i] == "all")
      ClearAllFunction(eval);
    else if (names[i] == "libs")
      ClearLibs(eval);
    else if (names[i] == "persistent")
      ClearPersistent(eval);
    else if (names[i] == "global")
      ClearGlobal(eval);
    else 
      ClearVariable(eval,names[i]);
  }
  return ArrayVector();
}

//!
//@Module WHO Describe Currently Defined Variables
//@@Section INSPECTION
//@@Usage
//Reports information on either all variables in the current context
//or on a specified set of variables.  For each variable, the @|who|
//function indicates the size and type of the variable as well as 
//if it is a global or persistent.  There are two formats for the 
//function call.  The first is the explicit form, in which a list
//of variables are provided:
//@[
//  who a1 a2 ...
//@]
//In the second form
//@[
//  who
//@]
//the @|who| function lists all variables defined in the current 
//context (as well as global and persistent variables). Note that
//there are two alternate forms for calling the @|who| function:
//@[
//  who 'a1' 'a2' ...
//@]
//and
//@[
//  who('a1','a2',...)
//@]
//@@Example
//Here is an example of the general use of @|who|, which lists all of the variables defined.
//@<
//c = [1,2,3];
//f = 'hello';
//p = randn(1,256);
//who
//@>
//In the second case, we examine only a specific variable:
//@<
//who c
//who('c')
//@>
//!
ArrayVector WhoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  int i;
  stringVector names;
  char buffer[1000];
  if (arg.size() == 0) {
    names = eval->getContext()->getCurrentScope()->listAllVariables();
  } else {
    for (i=0;i<arg.size();i++) {
      Array varName(arg[i]);
      names.push_back(varName.getContentsAsCString());
    }
  }
  std::sort(names.begin(),names.end());
  sprintf(buffer,"  Variable Name      Type   Flags             Size\n");
  eval->outputMessage(buffer);
  for (i=0;i<names.size();i++) {
    Array lookup, *ptr;
    sprintf(buffer,"% 15s",names[i].c_str());
    eval->outputMessage(buffer);
    ptr = eval->getContext()->lookupVariable(names[i]);
    if (!ptr)
      eval->outputMessage("   <undefined>");
    else {
      lookup = *ptr;
      Class t = lookup.getDataClass();
      switch(t) {
      case FM_CELL_ARRAY:
	sprintf(buffer,"% 10s","cell");
	break;
      case FM_STRUCT_ARRAY:
	if (lookup.isUserClass())
	  sprintf(buffer,"% 10s",lookup.getClassName().back().c_str());
	else
	  sprintf(buffer,"% 10s","struct");
	break;
      case FM_LOGICAL:
	sprintf(buffer,"% 10s","logical");
	break;
      case FM_UINT8:
	sprintf(buffer,"% 10s","uint8");
	break;
      case FM_INT8:
	sprintf(buffer,"% 10s","int8");
	break;
      case FM_UINT16:
	sprintf(buffer,"% 10s","uint16");
	break;
      case FM_INT16:
	sprintf(buffer,"% 10s","int16");
	break;
      case FM_UINT32:
	sprintf(buffer,"% 10s","uint32");
	break;
      case FM_INT32:
	sprintf(buffer,"% 10s","int32");
	break;
      case FM_UINT64:
	sprintf(buffer,"% 10s","uint64");
	break;
      case FM_INT64:
	sprintf(buffer,"% 10s","int64");
	break;
      case FM_FLOAT:
	sprintf(buffer,"% 10s","float");
	break;
      case FM_DOUBLE:
	sprintf(buffer,"% 10s","double");
	break;
      case FM_COMPLEX:
	sprintf(buffer,"% 10s","complex");
	break;
      case FM_DCOMPLEX:
	sprintf(buffer,"% 10s","dcomplex");
	break;
      case FM_STRING:
	sprintf(buffer,"% 10s","string");
	break;
      case FM_FUNCPTR_ARRAY:
	sprintf(buffer,"% 10s","func ptr");
	break;
      }
      eval->outputMessage(buffer);
      if (lookup.isSparse())
	eval->outputMessage("   sparse");
      else
	eval->outputMessage("         ");	  
      if (eval->getContext()->isVariableGlobal(names[i])) {
	sprintf(buffer,"  global ");
	eval->outputMessage(buffer);
      } else if (eval->getContext()->isVariablePersistent(names[i])) {
	sprintf(buffer," persist ");
	eval->outputMessage(buffer);
      } else {
	sprintf(buffer,"         ");
	eval->outputMessage(buffer);
      }
      eval->outputMessage("  ");
      lookup.getDimensions().printMe(eval);
    }
    eval->outputMessage("\n");
  }
  return ArrayVector();
}

//!
//@Module FIELDNAMES Fieldnames of a Structure
//@@Section INSPECTION
//@@Usage
//Returns a cell array containing the names of the fields in
//a structure array.  The syntax for its use is
//@[
//   x = fieldnames(y)
//@]
//where @|y| is a structure array of object array.  The result
//is a cell array, with one entry per field in @|y|.
//@@Example
//We define a simple structure array:
//@<
//y.foo = 3; y.goo = 'hello';
//x = fieldnames(y)
//@>
//!
ArrayVector FieldNamesFunction(int nargout, const ArrayVector& arg) {
  ArrayVector retval;
  if (arg.size() < 1)
    throw Exception("fieldnames function requires at least one argument");
  Array a(arg[0]);
  if (a.getDataClass() != FM_STRUCT_ARRAY) {
    Array ret(Array::emptyConstructor());
    ret.promoteType(FM_CELL_ARRAY);
    return singleArrayVector(ret);
  }
  rvstring names(a.getFieldNames());
  ArrayMatrix m;
  for (int i=0;i<names.size();i++)
    m.push_back(singleArrayVector(Array::stringConstructor(names.at(i))));
  return singleArrayVector(Array::cellConstructor(m));
}

//!
//@Module SIZE Size of a Variable
//@@Section INSPECTION
//@@Usage
//Returns the size of a variable.  There are two syntaxes for its
//use.  The first syntax returns the size of the array as a vector
//of integers, one integer for each dimension
//@[
//  [d1,d2,...,dn] = size(x)
//@]
//The other format returns the size of @|x| along a particular
//dimension:
//@[
//  d = size(x,n)
//@]
//where @|n| is the dimension along which to return the size.
//@@Example
//@<
//a = randn(23,12,5);
//size(a)
//@>
//Here is an example of the second form of @|size|.
//@<
//size(a,2)
//@>
//!
ArrayVector SizeFunction(int nargout, const ArrayVector& arg) {
  ArrayVector retval;
  if (arg.size() < 1)
    throw Exception("size function requires either one or two arguments");
  Dimensions sze;
  if (arg[0].isEmpty()) {
    sze = Dimensions(0,0);
  } else
    sze = arg[0].getDimensions();
  if (arg.size() == 1) {
    if (nargout > 1) {
      ArrayVector retval(nargout);
      for (int i=0;i<nargout;i++)
	retval[i] = Array::uint32Constructor(sze.get(i));
      return retval;
    } else {
      uint32 *dims = (uint32 *) Malloc(sizeof(uint32)*sze.getLength());
      for (int i=0;i<sze.getLength();i++)
	dims[i] = sze.get(i);
      Array ret = Array(FM_UINT32,Dimensions(1,sze.getLength()),dims);
      retval.push_back(ret);
      return retval;
    } 
  }
  if (arg.size() == 2) {
    Array tmp(arg[1]);
    int dimval = tmp.getContentsAsIntegerScalar();
    if (dimval<1)
      throw Exception("illegal value for dimension argument in call to size function");
    retval.push_back(Array::uint32Constructor(sze.get(dimval-1)));
    return retval;
  }
}

//   ArrayVector LengthFunction(int nargout, const ArrayVector& arg) {
//     Array A(Array::int32Constructor(arg[0].getDimensions().getMax()));
//     ArrayVector retval;
//     retval.push_back(A);
//     return retval;
//   }

int ExistBuiltinFunction(char* fname, Interpreter* eval) {    
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

int ExistDirFunction(char* fname, Interpreter* eval) {
  struct stat filestat;
  // Check for extra termination
  if ((fname[strlen(fname)-1] == '/') ||
      (fname[strlen(fname)-1] == '\\'))
    fname[strlen(fname)-1] = 0;
  if (stat(fname,&filestat)==-1) return 0;
  if (S_ISDIR(filestat.st_mode)) return 7;
  return 0;
}

int ExistFileFunction(char* fname, Interpreter* eval) {
  PathSearcher src(eval->getPath());
  try {
    src.ResolvePath(fname);
    return 2;
  } catch (Exception &e) {
  }
  bool isDefed;
  FuncPtr d;
  isDefed = eval->getContext()->lookupFunction(fname,d);
  if (isDefed && (d->type() == FM_M_FUNCTION))
    return 2;
  return 0;
}

int ExistVariableFunction(char* fname, Interpreter* eval) {
  bool isDefed = (eval->getContext()->lookupVariable(fname) != NULL);
  if (isDefed)
    return 1;
  else
    return 0;
}

int ExistAllFunction(char* fname, Interpreter* eval) {
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
//@Module ISSET Test If Variable Set
//@@Section INSPECTION
//@@Usage
//Tests for the existence and non-emptiness of a variable.
//the general syntax for its use is
//@[
//   y = isset('name')
//@]
//where @|name| is the name of the variable to test.  This
//is functionally equivalent to 
//@[
//   y = exist('name','var') & ~isempty(name)
//@]
//It returns a @|logical| 1 if the variable is defined 
//in the current workspace, and is not empty, and returns
//a 0 otherwise.
//@@Example
//Some simple examples of using @|isset|
//@<
//who
//isset('a')
//a = [];
//isset('a')
//a = 2;
//isset('a')
//@>
//!
ArrayVector IsSetFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("isset function takes at least one argument - the name of the variable to check for");
  Array tmp(arg[0]);
  char *fname;
  fname = tmp.getContentsAsCString();
  bool isDefed;
  Array *d = eval->getContext()->lookupVariable(fname);
  isDefed = (d != NULL);
  if (isDefed && !d->isEmpty())
    return singleArrayVector(Array::logicalConstructor(1));
  else
    return singleArrayVector(Array::logicalConstructor(0));
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
//!
ArrayVector ExistFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1)
    throw Exception("exist function takes at least one argument - the name of the object to check for");
  Array tmp(arg[0]);
  char *fname;
  fname = tmp.getContentsAsCString();
  char *stype;
  if (arg.size() > 1) {
    Array tmp2(arg[1]);
    stype = tmp2.getContentsAsCString();
  } else {
    stype = "all";
  }
  int retval;
  if (strcmp(stype,"all")==0)
    retval = ExistAllFunction(fname,eval);
  else if (strcmp(stype,"builtin")==0)
    retval = ExistBuiltinFunction(fname,eval);
  else if (strcmp(stype,"dir")==0)
    retval = ExistDirFunction(fname,eval);
  else if (strcmp(stype,"file")==0)
    retval = ExistFileFunction(fname,eval);
  else if (strcmp(stype,"var")==0)
    retval = ExistVariableFunction(fname,eval);
  else throw Exception("Unrecognized search type for function 'exist'");
  return singleArrayVector(Array::int32Constructor(retval));
}

//!
//@Module NNZ Number of Nonzeros
//@@Section SPARSE
//@@Usage
//Returns the number of nonzero elements in a matrix.
//The general format for its use is
//@[
//   y = nnz(x)
//@]
//This function returns the number of nonzero elements
//in a matrix or array.  This function works for both
//sparse and non-sparse arrays.  For 
//@@Example
//@<
//a = [1,0,0,5;0,3,2,0]
//nnz(a)
//A = sparse(a)
//nnz(A)
//@>
//!
ArrayVector NNZFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("nnz function takes one argument - the array");
  Array tmp(arg[0]);
  return singleArrayVector(Array::int32Constructor(tmp.nnz()));
}

//!
//@Module ISSPARSE Test for Sparse Matrix
//@@Section INSPECTION
//@@Usage
//Test a matrix to see if it is sparse or not.  The general
//format for its use is 
//@[
//   y = issparse(x)
//@]
//This function returns true if @|x| is encoded as a sparse
//matrix, and false otherwise.
//@@Example
//Here is an example of using @|issparse|:
//@<
//a = [1,0,0,5;0,3,2,0]
//issparse(a)
//A = sparse(a)
//issparse(A)
//@>
//!
ArrayVector IsSparseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("issparse function takes one argument - the array to test");
  Array tmp(arg[0]);
  return singleArrayVector(Array::logicalConstructor(tmp.isSparse()));
}

//!
//@Module ISNAN Test for Not-a-Numbers
//@@Section INSPECTION
//@@Usage
//Returns true for entries of an array that are NaN's (i.e.,
//Not-a-Numbers).  The usage is
//@[
//   y = isnan(x)
//@]
//The result is a logical array of the same size as @|x|,
//which is true if @|x| is not-a-number, and false otherwise.
//Note that for @|complex| or @|dcomplex| data types that
//the result is true if either the real or imaginary parts
//are NaNs.
//@@Example
//Suppose we have an array of floats with one element that
//is @|nan|:
//@<
//a = [1.2 3.4 nan 5]
//isnan(a)
//@>
//!
ArrayVector IsNaNFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isnan function takes one argument - the array to test");
  Array tmp(arg[0]);
  if (tmp.isReferenceType())
    throw Exception("isnan is not defined for reference types");
  ArrayVector retval;
  int len(tmp.getLength());
  logical *op = (logical *) Malloc(len*sizeof(logical));
  switch (tmp.getDataClass()) {
  case FM_STRING:
  case FM_LOGICAL:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT32:
  case FM_INT32:
  case FM_UINT64:
  case FM_INT64:
    break;
  case FM_FLOAT: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsNaN(dp[i]) ? 1 : 0;
    break;
  }
  case FM_DOUBLE: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsNaN(dp[i]) ? 1 : 0;
    break;
  }
  case FM_COMPLEX: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsNaN(dp[2*i]) || IsNaN(dp[2*i+1])) ? 1 : 0;
    break;
  }
  case FM_DCOMPLEX: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsNaN(dp[2*i]) || IsNaN(dp[2*i+1])) ? 1 : 0;
    break;
  }
  }
  retval.push_back(Array(FM_LOGICAL,tmp.getDimensions(),op));
  return(retval);
}

//!
//@Module ISINF Test for infinities
//@@Section INSPECTION
//@@Usage
//Returns true for entries of an array that are infs (i.e.,
//infinities).  The usage is
//@[
//   y = isinf(x)
//@]
//The result is a logical array of the same size as @|x|,
//which is true if @|x| is not-a-number, and false otherwise.
//Note that for @|complex| or @|dcomplex| data types that
//the result is true if either the real or imaginary parts
//are infinite.
//@@Example
//Suppose we have an array of floats with one element that
//is @|inf|:
//@<
//a = [1.2 3.4 inf 5]
//isinf(a)
//b = 3./[2 5 0 3 1]
//@>
//!
ArrayVector IsInfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("isinf function takes one argument - the array to test");
  Array tmp(arg[0]);
  if (tmp.isReferenceType())
    throw Exception("isinf is not defined for reference types");
  ArrayVector retval;
  int len(tmp.getLength());
  logical *op = (logical *) Malloc(len*sizeof(logical));
  switch (tmp.getDataClass()) {
  case FM_STRING:
  case FM_LOGICAL:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT32:
  case FM_INT32:
  case FM_UINT64:
  case FM_INT64:
    break;
  case FM_FLOAT: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsInfinite(dp[i]) ? 1 : 0;
    break;
  }
  case FM_DOUBLE: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = IsInfinite(dp[i]) ? 1 : 0;
    break;
  }
  case FM_COMPLEX: {
    const float *dp = (const float *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsInfinite(dp[2*i]) || IsInfinite(dp[2*i+1])) ? 1 : 0;
    break;
  }
  case FM_DCOMPLEX: {
    const double *dp = (const double *)tmp.getDataPointer();
    for (int i=0;i<len;i++)
      op[i] = (IsInfinite(dp[2*i]) || IsInfinite(dp[2*i+1])) ? 1 : 0;
    break;
  }
  }
  retval.push_back(Array(FM_LOGICAL,tmp.getDimensions(),op));
  return(retval);
}

//!
//@Module WHERE Get Information on Program Stack
//@@Section INSPECTION
//@@Usage
//Returns information on the current stack.  The usage is
//@[
//   where
//@]
//The result is a kind of stack trace that indicates the state
//of the current call stack, and where you are relative to the
//stack.
//@@Example
//Suppose we have the following chain of functions.
//@{ chain1.m
//function chain1
//  a = 32;
//  b = a + 5;
//  chain2(b)
//@}
//@{ chain2.m
//function chain2(d)
//  d = d + 5;
//  chain3
//@}
//@{ chain3.m
//function chain3
//  g = 54;
//  f = g + 1;
//  keyboard
//@}
//The execution of the @|where| command shows the stack trace.
//@<
//chain1
//where
//@>
//!
ArrayVector WhereFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  //    eval->stackTrace(false);
  eval->stackTrace(true);
  return ArrayVector();
}

//!
//@Module WHICH Get Information on Function
//@@Section INSPECTION
//@@Usage
//Returns information on a function (if defined).  The usage is
//@[
//   which(fname)
//@]
//where @|fname| is a @|string| argument that contains the name of the 
//function.  For functions and scripts defined
//via @|.m| files, the @|which| command returns the location of the source
//file:
//@[
//   y = which(fname)
//@]
//will return the filename for the @|.m| file corresponding to the given
//function, and an empty string otherwise.
//@@Example
//First, we apply the @|which| command to a built in function.
//@<
//which fft
//@>
//Next, we apply it to a function defined via a @|.m| file.
//@<
//which fliplr
//@>
//!
ArrayVector WhichFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("which function takes one string argument (the name of the function to look up)");
  char *fname;
  fname = arg[0].getContentsAsCString();
  bool isFun;
  FuncPtr val;
  isFun = eval->getContext()->lookupFunction(fname,val);
  char buffer[1000];
  Array ret(Array::emptyConstructor());
  if (isFun) {
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      mptr->updateCode();
      if (mptr->pcodeFunction) {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, P-code script\n",fname);
	    eval->outputMessage(buffer);
	  }
	} else {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, P-code function\n",fname);
	    eval->outputMessage(buffer);
	  }
	}
      } else {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, M-File script in file '%s'\n",fname,mptr->fileName.c_str());
	    eval->outputMessage(buffer);
	  } else 
	    ret = Array::stringConstructor(mptr->fileName.c_str());
	} else {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, M-File function in file '%s'\n",fname,mptr->fileName.c_str());
	    eval->outputMessage(buffer);
	  } else
	    ret = Array::stringConstructor(mptr->fileName.c_str());
	}
      }
    } else if ((val->type() == FM_BUILT_IN_FUNCTION) || (val->type() == FM_SPECIAL_FUNCTION) ) {
      if (nargout == 0) {
	sprintf(buffer,"Function %s is a built in function\n",fname);
	eval->outputMessage(buffer);
      }
    } else {
      if (nargout == 0) {
	sprintf(buffer,"Function %s is an imported function\n",fname);
	eval->outputMessage(buffer);
      }
    }
  } else {
    if (nargout == 0) {
      sprintf(buffer,"Function %s is unknown!\n",fname);
      eval->outputMessage(buffer);
    }
  }
  if (nargout > 0)
    return singleArrayVector(ret);
  else
    return ArrayVector();
}

ArrayVector SingleFindModeFull(Array x) {
  x.promoteType(FM_LOGICAL);
  const logical *dp;
  dp = (const logical*) x.getDataPointer();
  int len;
  len = x.getLength();
  // Count the number of non-zero entries
  int nonZero;
  nonZero = 0;
  int i;
  for (i=0;i<len;i++)
    if (dp[i]) nonZero++;
  // Setup the output array
  uint32 *op;
  op = (uint32*) Malloc(nonZero*sizeof(uint32));
  int ndx;
  ndx = 0;
  for (i=0;i<len;i++)
    if (dp[i])
      op[ndx++] = i + 1;
  Dimensions retDim(2);
  if (x.isRowVector()) {
    retDim.set(0,1);
    retDim.set(1,nonZero);
  } else {
    retDim.set(0,nonZero);
    retDim.set(1,1);
  }
  return singleArrayVector(Array(FM_UINT32,retDim,op));
}
  
ArrayVector RCFindModeFull(Array x) {
  x.promoteType(FM_LOGICAL);
  const logical *dp;
  dp = (const logical*) x.getDataPointer();
  int len;
  len = x.getLength();
  // Count the number of non-zero entries
  int nonZero;
  nonZero = 0;
  int i;
  for (i=0;i<len;i++)
    if (dp[i]) nonZero++;
  // Setup the output array
  uint32 *op_row;
  op_row = (uint32*) Malloc(nonZero*sizeof(uint32));
  uint32 *op_col;
  op_col = (uint32*) Malloc(nonZero*sizeof(uint32));
  int ndx;
  int rows = x.getDimensionLength(0);
  int cols = x.getDimensionLength(1);
  ndx = 0;
  for (i=0;i<len;i++)
    if (dp[i]) {
      op_row[ndx] = (i % rows) + 1;
      op_col[ndx++] = (i / rows) + 1;
    }
  Dimensions retDim(2);
  if (x.isRowVector()) {
    retDim.set(0,1);
    retDim.set(1,nonZero);
  } else {
    retDim.set(0,nonZero);
    retDim.set(1,1);
  }
  ArrayVector retval;
  retval.push_back(Array(FM_UINT32,retDim,op_row));
  retval.push_back(Array(FM_UINT32,retDim,op_col));
  return retval;
}

template <class T>
ArrayVector RCVFindModeFullReal(Array x) {
  const T* dp;
  dp = (const T*) x.getDataPointer();
  int len;
  len = x.getLength();
  // Count the number of non-zero entries
  int nonZero;
  nonZero = 0;
  int i;
  for (i=0;i<len;i++)
    if (dp[i]) nonZero++;
  // Setup the output array
  uint32 *op_row;
  op_row = (uint32*) Malloc(nonZero*sizeof(uint32));
  uint32 *op_col;
  op_col = (uint32*) Malloc(nonZero*sizeof(uint32));
  T* op_val;
  op_val = (T*) Malloc(nonZero*sizeof(T));
  int ndx;
  int rows = x.getDimensionLength(0);
  int cols = x.getDimensionLength(1);
  ndx = 0;
  for (i=0;i<len;i++)
    if (dp[i]) {
      op_row[ndx] = (i % rows) + 1;
      op_col[ndx] = (i / rows) + 1;
      op_val[ndx++] = dp[i];
    }
  Dimensions retDim(2);
  if (x.isRowVector()) {
    retDim.set(0,1);
    retDim.set(1,nonZero);
  } else {
    retDim.set(0,nonZero);
    retDim.set(1,1);
  }
  ArrayVector retval;
  retval.push_back(Array(FM_UINT32,retDim,op_row));
  retval.push_back(Array(FM_UINT32,retDim,op_col));
  retval.push_back(Array(x.getDataClass(),retDim,op_val));
  return retval;
}

template <class T>
ArrayVector RCVFindModeFullComplex(Array x) {
  const T* dp;
  dp = (const T*) x.getDataPointer();
  int len;
  len = x.getLength();
  // Count the number of non-zero entries
  int nonZero;
  nonZero = 0;
  int i;
  for (i=0;i<len;i++)
    if (dp[2*i] || dp[2*i+1]) nonZero++;
  // Setup the output array
  uint32 *op_row;
  op_row = (uint32*) Malloc(nonZero*sizeof(uint32));
  uint32 *op_col;
  op_col = (uint32*) Malloc(nonZero*sizeof(uint32));
  T* op_val;
  op_val = (T*) Malloc(2*nonZero*sizeof(T));
  int ndx;
  int rows = x.getDimensionLength(0);
  int cols = x.getDimensionLength(1);
  ndx = 0;
  for (i=0;i<len;i++)
    if (dp[2*i] || dp[2*i+1]) {
      op_row[ndx] = (i % rows) + 1;
      op_col[ndx] = (i / rows) + 1;
      op_val[2*ndx] = dp[2*i];
      op_val[2*ndx+1] = dp[2*i+1];
      ndx++;
    }
  Dimensions retDim(2);
  if (x.isRowVector()) {
    retDim.set(0,1);
    retDim.set(1,nonZero);
  } else {
    retDim.set(0,nonZero);
    retDim.set(1,1);
  }
  ArrayVector retval;
  retval.push_back(Array(FM_UINT32,retDim,op_row));
  retval.push_back(Array(FM_UINT32,retDim,op_col));
  retval.push_back(Array(x.getDataClass(),retDim,op_val));
  return retval;
}

ArrayVector RCVFindModeFull(Array x) {
  switch (x.getDataClass()) {
  case FM_LOGICAL:
    return RCVFindModeFullReal<logical>(x);
  case FM_UINT8:
    return RCVFindModeFullReal<uint8>(x);
  case FM_INT8:
    return RCVFindModeFullReal<int8>(x);
  case FM_UINT16:
    return RCVFindModeFullReal<uint16>(x);
  case FM_INT16:
    return RCVFindModeFullReal<int16>(x);
  case FM_UINT32:
    return RCVFindModeFullReal<uint32>(x);
  case FM_INT32:
    return RCVFindModeFullReal<int32>(x);
  case FM_UINT64:
    return RCVFindModeFullReal<uint64>(x);
  case FM_INT64:
    return RCVFindModeFullReal<int64>(x);
  case FM_FLOAT:
    return RCVFindModeFullReal<float>(x);
  case FM_DOUBLE:
    return RCVFindModeFullReal<double>(x);
  case FM_COMPLEX:
    return RCVFindModeFullComplex<float>(x);
  case FM_DCOMPLEX:
    return RCVFindModeFullComplex<double>(x);
  case FM_STRING:
    return RCVFindModeFullReal<char>(x);
  }
}

ArrayVector FindModeSparse(Array x, int nargout) {
  // Convert the sparse matrix to RCV mode
  void *dp;
  uint32 *rows;
  uint32 *cols;
  int nnz;
  dp = SparseToIJV(x.getDataClass(), x.getDimensionLength(0),
		   x.getDimensionLength(1), x.getSparseDataPointer(),
		   rows, cols, nnz);
  Dimensions retDim(2);
  if (x.isRowVector()) {
    retDim.set(0,1);
    retDim.set(1,nnz);
  } else {
    retDim.set(0,nnz);
    retDim.set(1,1);
  }
  ArrayVector retval;
  // Decide how to combine the arrays depending on nargout
  if (nargout == 3) {
    retval.push_back(Array(FM_UINT32,retDim,rows));
    retval.push_back(Array(FM_UINT32,retDim,cols));
    retval.push_back(Array(x.getDataClass(),retDim,dp));
  } else if (nargout == 2) {
    retval.push_back(Array(FM_UINT32,retDim,rows));
    retval.push_back(Array(FM_UINT32,retDim,cols));
    Free(dp);
  } else {
    int numrows;
    numrows = x.getDimensionLength(0);
    for (int i=0;i<nnz;i++)
      rows[i] = rows[i] + (cols[i]-1)*numrows;
    Free(cols);
    Free(dp);
    retval.push_back(Array(FM_UINT32,retDim,rows));      
  }
  return retval;
}
  
//!
//@Module FIND Find Non-zero Elements of An Array
//@@Section ARRAY
//@@Usage
//Returns a vector that contains the indicies of all non-zero elements 
//in an array.  The usage is
//@[
//   y = find(x)
//@]
//The indices returned are generalized column indices, meaning that if 
//the array @|x| is of size @|[d1,d2,...,dn]|, and the
//element @|x(i1,i2,...,in)| is nonzero, then @|y|
//will contain the integer
//\[
//   i_1 + (i_2-1) d_1 + (i_3-1) d_1 d_2 + \dots
//\]
//The second syntax for the @|find| command is
//@[
//   [r,c] = find(x)
//@]
//which returns the row and column index of the nonzero entries of @|x|.
//The third syntax for the @|find| command also returns the values
//@[
//   [r,c,v] = find(x).
//@]
//Note that if the argument is a row vector, then the returned vectors
//are also row vectors. This form is particularly useful for converting 
//sparse matrices into IJV form.
//
//The @|find| command also supports some additional arguments.  Each of the
//above forms can be combined with an integer indicating how many results
//to return:
//@[
//   y = find(x,k)
//@]
//where @|k| is the maximum number of results to return.  This form will return
//the first @|k| results.  You can also specify an optional flag indicating 
//whether to take the first or last @|k| values:
//@[
//   y = find(x,k,'first')
//   y = find(x,k,'last')
//@]
//in the case of the @|'last'| argument, the last @|k| values are returned.
//@@Example
//Some simple examples of its usage, and some common uses of @|find| in FreeMat programs.
//@<
//a = [1,2,5,2,4];
//find(a==2)
//@>
//Here is an example of using find to replace elements of @|A| that are @|0| with the number @|5|.
//@<
//A = [1,0,3;0,2,1;3,0,0]
//n = find(A==0)
//A(n) = 5
//@>
//Incidentally, a better way to achieve the same concept is:
//@<
//A = [1,0,3;0,2,1;3,0,0]
//A(A==0) = 5
//@>
//Now, we can also return the indices as row and column indices using the two argument
//form of @|find|:
//@<
//A = [1,0,3;0,2,1;3,0,0]
//[r,c] = find(A)
//@>
//Or the three argument form of @|find|, which returns the value also:
//@<
//[r,c,v] = find(A)
//@>
//!  

ArrayVector FindTrim(ArrayVector a, int cnt, bool first_flag) {
  if (cnt < 0) return a;
  if (a.size() == 0) return a;
  int N = a[0].getLength();
  if (cnt > N) return a;
  ArrayVector ret;
  Array ndx;
  bool vertflag = !(a[0].isRowVector());
  if (first_flag)
    ndx = Array::int32RangeConstructor(1,1,cnt,vertflag);
  else
    ndx = Array::int32RangeConstructor((N-cnt)+1,1,N,vertflag);
  for (int i=0;i<a.size();i++) 
    ret.push_back(a[i].getVectorSubset(ndx));
  return ret;
}

ArrayVector FindFunction(int nargout, const ArrayVector& arg) {
  // Detect the Find mode...
  if (arg.size() < 1)
    throw Exception("find function takes at least one argument");
  Array tmp(arg[0]);
  int k = -1;
  bool first_flag = true;
  if (arg.size() > 1)
    k  = ArrayToInt32(arg[1]);
  if (arg.size() == 3) {
    const char *flag = ArrayToString(arg[2]);
    if (strcmp(flag,"first") == 0)
      first_flag = true;
    else if (strcmp(flag,"last") == 0)
      first_flag = false;
    else
      throw Exception("third option to find must be either 'first' or 'last'");
  }
  if (tmp.isReferenceType())
    throw Exception("find does not work on reference types (cell-arrays or structure arrays)");
  if ((nargout <= 1) && !tmp.isSparse())
    return FindTrim(SingleFindModeFull(tmp),k,first_flag);
  if ((nargout == 2) && !tmp.isSparse())
    return FindTrim(RCFindModeFull(tmp),k,first_flag);
  if ((nargout == 3) && !tmp.isSparse())
    return FindTrim(RCVFindModeFull(tmp),k,first_flag);
  if (nargout > 3)
    throw Exception("Do not understand syntax of find call (too many output arguments).");
  return FindTrim(FindModeSparse(tmp,nargout),k,first_flag);
}

//!
//@Module MFILENAME Name of Current Function
//@@Section FreeMat
//@@Usage
//Returns a string describing the name of the current function.  For M-files
//this string will be the complete filename of the function.  This is true even
//for subfunctions.  The syntax for its use is
//@[
//    y = mfilename
//@]
//!

static std::string fname_only(std::string name) {
  int ndx;
  ndx = name.rfind("/");
  if (ndx>=0)
    name.erase(0,ndx+1);
  ndx = name.rfind(".");
  if (ndx>=0)
    name.erase(ndx,name.size());
  return name;
}

ArrayVector MFilenameFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return singleArrayVector(Array::stringConstructor(fname_only(eval->getMFileName())));
}

//!
//@Module COMPUTER Computer System FreeMat is Running On
//@@Section FreeMat
//@@Usage
//Returns a string describing the name of the system FreeMat is running on.
//The exact value of this string is subject to change, although the @|'MAC'|
//and @|'PCWIN'| values are probably fixed.
//@[
//  str = computer
//@]
//Currently, the following return values are defined
//\begin{itemize}
//  \item @|'PCWIN'| - MS Windows
//  \item @|'MAC'| - Mac OS X
//  \item @|'UNIX'| - All others
//\end{itemize}
//!
ArrayVector ComputerFunction(int nargout, const ArrayVector& arg) {
#ifdef WIN32
  return singleArrayVector(Array::stringConstructor("PCWIN"));
#elif defined(__APPLE__)
  return singleArrayVector(Array::stringConstructor("MAC"));
#else
  return singleArrayVector(Array::stringConstructor("UNIX"));
#endif
}

