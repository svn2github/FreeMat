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

#include "Array.hpp"
#include "WalkTree.hpp"
#include "Malloc.hpp"
#include "PathSearch.hpp"
#include "IEEEFP.hpp"
#include "Sparse.hpp"
#include <stdio.h>

namespace FreeMat {

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
  //@Module AUTOSTOP Control Autostop Functionality
  //@@Section FLOW
  //@@Usage
  //The autostop functionality in FreeMat allows you to debug your
  //FreeMat programs.  When @|autostop| is @|on|, then any error
  //that occurs while the program is running causes FreeMat to 
  //stop execution at that point and return you to the command line
  //(just as if you had placed a @|keyboard| command there).  You can
  //then examine variables, modify them, and resume execution using
  //@|return|.  Alternately, you can exit out of all running routines
  //via a @|retall| statement.  Note that errors that occur inside of
  //@|try|/@|catch| blocks do not (by design) cause autostops.  The
  //@|autostop| function toggles the autostop state of FreeMat.  The
  //syntax for its use is
  //@[
  //   autostop(state)
  //@]
  //where @|state| is either
  //@[
  //   autostop('on')
  //@]
  //to activate autostop, or
  //@[
  //   autostop('off')
  //@]
  //to deactivate autostop.  Alternately, you can use FreeMat's string-syntax
  //equivalence and enter
  //@[
  //   autostop on
  //@]
  //or 
  //@[
  //   autostop off
  //@]
  //to turn autostop on or off (respectively).  Entering @|autostop| with no arguments
  //returns the current state (either 'on' or 'off').
  //!
  ArrayVector AutoStopFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() < 1) {
      if (eval->AutoStop()) 
	return singleArrayVector(Array::stringConstructor("on"));
      else 
	return singleArrayVector(Array::stringConstructor("off"));
    } else {
      if (!arg[0].isString())
	throw Exception("autostop function takes only a single, string argument");
      char *txt;
      txt = arg[0].getContentsAsCString();
      if (strcmp(txt,"on") == 0)
	eval->AutoStop(true);
      else if (strcmp(txt,"off") == 0)
	eval->AutoStop(false);
      else
	throw Exception("Grid function argument needs to be 'on/off'");
    }
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
  ArrayVector HelpFunction(int nargout, const ArrayVector& arg, WalkTree* eval)
  {
    Interface *io;
    io = eval->getInterface();
    PathSearcher psearch(io->getPath());

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
	io->outputMessage(mptr->helpText[i].c_str());
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
	bool verbatimMode = false;
	while (!feof(fp)) {
	  fgets(buffer,sizeof(buffer),fp);
	  //is this line only a $ sign?
	  if ((buffer[0] == '$') && strlen(buffer)<3)
	    verbatimMode = !verbatimMode;
	  else {
	    if (verbatimMode) {
	      helplines.push_back(buffer);
	      workingline.erase();
	    } else {
	      //is this line empty? if so, push
	      //workingline to helplines and clear it.
	      if (strlen(buffer)<2) {
		helplines.push_back(workingline);
		helplines.push_back("\n");
		workingline.erase();
	      } else {
		buffer[strlen(buffer)-1] = 0;
		workingline = workingline + " " + buffer;
	      }
	    }
	  }
	}
	// Write the lines out...
	// Get the output width (in characters)
	int outputWidth = io->getTerminalWidth() - 20;
	for (int p=0;p<helplines.size();p++) {
	  std::vector<std::string> tokens;
	  // Tokenize the help line
	  Tokenize(helplines[p],tokens);
	  // Output words..
	  int outlen = 0;
	  int tokencount = 0;
	  io->outputMessage("\n          ");
	  while ((tokens.size() > 0) && (tokencount < tokens.size())) {
	    // Can the next token be output without wrapping?
		int tsize;
		tsize = tokens[tokencount].size();
	    if ((outlen == 0) || ((outlen + tsize) < outputWidth)) {
	      // Yes... send it and move on
	      io->outputMessage(tokens[tokencount].c_str());
	      io->outputMessage(" ");
	      outlen += tokens[tokencount++].size()+1;
	    } else {
	      io->outputMessage("\n          ");
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
  //two formats for the function call.  The first is the explicit form
  //in which a list of variables are provided:
  //@[
  //   clear a1 a2 ...
  //@]
  //The variables can be persistent or global, and they will be deleted.
  //The second form
  //@[
  //   clear 'all'
  //@]
  //clears all variables from the current context.
  //@@Example
  //Here is a simple example of using @|clear| to delete a variable.  First, we create a variable called @|a|:
  //@<
  //a = 53
  //@>
  //Next, we clear @|a| using the @|clear| function, and verify that it is deleted.
  //@<
  //clear a
  //a
  //@>
  //!
  ArrayVector ClearFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    int i;
    stringVector names;
    if (arg.size() == 0) 
      throw Exception("clear function expects either a list of variables to clear or the argument 'all' to clear all currently defined variables");
    if (arg.size() == 1) {
      Array singleArg(arg[0]);
      char * singleArgC;
      singleArgC = singleArg.getContentsAsCString();
      if (strcmp(singleArgC,"all") == 0)
	names = eval->getContext()->getCurrentScope()->listAllVariables();
      else
	names.push_back(singleArgC);
    } else {
      for (i=0;i<arg.size();i++) {
	Array varName(arg[i]);
	names.push_back(varName.getContentsAsCString());
      }
    }
    for (i=0;i<names.size();i++) {
      eval->getContext()->deleteVariable(names[i]);
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
  ArrayVector WhoFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    int i;
    stringVector names;
    char buffer[1000];
    Interface *io;
    if (arg.size() == 0) {
      names = eval->getContext()->getCurrentScope()->listAllVariables();
    } else {
      for (i=0;i<arg.size();i++) {
	Array varName(arg[i]);
	names.push_back(varName.getContentsAsCString());
      }
    }
    io = eval->getInterface();
    sprintf(buffer,"  Variable Name      Type   Flags             Size\n");
    io->outputMessage(buffer);
    for (i=0;i<names.size();i++) {
      Array lookup;
      sprintf(buffer,"% 15s",names[i].c_str());
      io->outputMessage(buffer);
      if (!eval->getContext()->lookupVariable(names[i],lookup))
	io->outputMessage("   <undefined>");
      else {
	Class t = lookup.getDataClass();
	switch(t) {
	case FM_CELL_ARRAY:
	  sprintf(buffer,"% 10s","cell");
	  break;
	case FM_STRUCT_ARRAY:
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
	}
	io->outputMessage(buffer);
	if (lookup.isSparse())
	  io->outputMessage("   sparse");
	else
	  io->outputMessage("         ");	  
	if (eval->getContext()->isVariableGlobal(names[i])) {
	  sprintf(buffer,"  global ");
	  io->outputMessage(buffer);
	} else if (eval->getContext()->isVariablePersistent(names[i])) {
	  sprintf(buffer," persist ");
	  io->outputMessage(buffer);
	} else {
	  sprintf(buffer,"         ");
	  io->outputMessage(buffer);
	}
	io->outputMessage("  ");
	lookup.getDimensions().printMe(io);
      }
      io->outputMessage("\n");
    }
    return ArrayVector();
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
    if (arg.size() == 1) {
      Dimensions sze(arg[0].getDimensions());
      if (nargout > 1) {
	ArrayVector retval(nargout);
	for (int i=0;i<nargout;i++)
	  retval[i] = Array::uint32Constructor(sze[i]);
	return retval;
      } else {
	uint32 *dims = (uint32 *) Malloc(sizeof(uint32)*sze.getLength());
	for (int i=0;i<sze.getLength();i++)
	  dims[i] = sze[i];
	Dimensions retDim(2);
	retDim[0] = 1;
	retDim[1] = sze.getLength();
	Array ret = Array(FM_UINT32,retDim,dims);
	retval.push_back(ret);
	return retval;
      } 
    }
    if (arg.size() == 2) {
      Array tmp(arg[1]);
      int dimval = tmp.getContentsAsIntegerScalar();
      if (dimval<1)
	throw Exception("illegal value for dimension argument in call to size function");
      Dimensions sze(arg[0].getDimensions());
      retval.push_back(Array::uint32Constructor(sze[dimval-1]));
      return retval;
    }
    throw Exception("size function requires either one or two arguments");
  }

  //   ArrayVector LengthFunction(int nargout, const ArrayVector& arg) {
  //     Array A(Array::int32Constructor(arg[0].getDimensions().getMax()));
  //     ArrayVector retval;
  //     retval.push_back(A);
  //     return retval;
  //   }

  //!
  //@Module EXIST Text Existence of a Variable
  //@@Section INSPECTION
  //@@Usage
  //Tests for the existence of a variable.  The general syntax for
  //its use is
  //@[
  //  y = exist('varname')
  //@]
  //The return is @|1| if a variable with the name @|varname| exists in
  //the current workspace and is not empty.  This function is primarily
  //useful when keywords are used in function arguments.
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
  ArrayVector ExistFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("exist function takes one argument - the name of the variable to check for");
    Array tmp(arg[0]);
    char *fname;
    fname = tmp.getContentsAsCString();
    bool isDefed;
    Array d;
    isDefed = eval->getContext()->lookupVariableLocally(fname, d);
    bool existCheck;
    if (isDefed && !d.isEmpty())
      existCheck = true;
    else
      existCheck = false;
    Array A(Array::logicalConstructor(existCheck));
    ArrayVector retval;
    retval.push_back(A);
    return retval;	    
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
  ArrayVector WhereFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    eval->stackTrace(false);
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
  //file.  
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
  ArrayVector WhichFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("which function takes one string argument (the name of the function to look up)");
    char *fname;
    fname = arg[0].getContentsAsCString();
    bool isFun;
    FuncPtr val;
    isFun = eval->getContext()->lookupFunction(fname,val);
    Interface *io = eval->getInterface();
    char buffer[1000];
    if (isFun) {
      if (val->type() == FM_M_FUNCTION) {
	MFunctionDef *mptr;
	mptr = (MFunctionDef *) val;
	mptr->updateCode();
	if (mptr->pcodeFunction) {
	  if (mptr->scriptFlag) {
	    sprintf(buffer,"Function %s, P-code script\n",fname);
	    io->outputMessage(buffer);
	  } else {
	    sprintf(buffer,"Function %s, P-code function\n",fname);
	    io->outputMessage(buffer);
	  }
	} else {
	  if (mptr->scriptFlag) {
	    sprintf(buffer,"Function %s, M-File script in file '%s'\n",fname,mptr->fileName.c_str());
	    io->outputMessage(buffer);
	  } else {
	    sprintf(buffer,"Function %s, M-File function in file '%s'\n",fname,mptr->fileName.c_str());
	    io->outputMessage(buffer);
	  }
	}
      } else if ((val->type() == FM_BUILT_IN_FUNCTION) || (val->type() == FM_SPECIAL_FUNCTION) ) {
	sprintf(buffer,"Function %s is a built in function\n",fname);
	io->outputMessage(buffer);
      } else {
	sprintf(buffer,"Function %s is an imported function\n",fname);
	io->outputMessage(buffer);
      }
    } else {
      sprintf(buffer,"Function %s is unknown!\n",fname);
      io->outputMessage(buffer);
    }
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
    retDim[0] = nonZero;
    retDim[1] = 1;
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
    retDim[0] = nonZero;
    retDim[1] = 1;
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
    retDim[0] = nonZero;
    retDim[1] = 1;
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
    retDim[0] = nonZero;
    retDim[1] = 1;
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
    retDim[0] = nnz;
    retDim[1] = 1;
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
	rows[i] = rows[i] + cols[i]*numrows;
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
  //This form is particularly useful for converting sparse matrices
  //into IJV form.
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
  ArrayVector FindFunction(int nargout, const ArrayVector& arg) {
    // Detect the Find mode...
    if (arg.size() != 1)
      throw Exception("find function takes one argument");
    Array tmp(arg[0]);
    if (tmp.isReferenceType())
      throw Exception("find does not work on reference types (cell-arrays or structure arrays)");
    if ((nargout == 1) && !tmp.isSparse())
      return SingleFindModeFull(tmp);
    if ((nargout == 2) && !tmp.isSparse())
      return RCFindModeFull(tmp);
     if ((nargout == 3) && !tmp.isSparse())
       return RCVFindModeFull(tmp);
     if (nargout > 3)
       throw Exception("Do not understand syntax of find call (too many output arguments).");
     return FindModeSparse(tmp,nargout);
    return ArrayVector();
  }
}
