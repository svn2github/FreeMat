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
  
  void InitializeHelpDirectory(std::string path) {
    helppath = path;
  }

  //!
  //@Module HELP Help
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
    if (arg.size() != 1)
      throw Exception("help function requires a single argument (the function or script name)");
    Array singleArg(arg[0]);
    char *fname;
    fname = singleArg.getContentsAsCString();
    bool isFun;
    FuncPtr val;
    isFun = eval->getContext()->lookupFunction(fname,val);
    Interface *io = eval->getInterface();
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
      mdcname = helppath + "/help/" + fname + ".mdc";
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
	  while (tokencount < tokens.size()) {
	    // Can the next token be output without wrapping?
	    if (outlen + tokens[tokencount].size() < outputWidth) {
	      // Yes... send it and move on
	      io->outputMessage(tokens[tokencount].c_str());
	      io->outputMessage(" ");
	      outlen += tokens[tokencount++].size();
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
    sprintf(buffer,"  Variable Name      Type   Flags   Size\n");
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
  //@Module FIND Find Non-zero Elements of An Array
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
  //!  
  ArrayVector FindFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("find function takes one argument");
    Array tmp(arg[0]);
    if (tmp.isReferenceType())
      throw Exception("find does not work on reference types (cell-arrays or structure arrays)");
    tmp.promoteType(FM_LOGICAL);
    const logical *dp;
    dp = (const logical*) tmp.getDataPointer();
    int len;
    len = tmp.getLength();
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
    ArrayVector retval;
    Dimensions retDim(2);
    retDim[0] = nonZero;
    retDim[1] = 1;
    retval.push_back(Array(FM_UINT32,retDim,op));
    return retval;
  }
}
