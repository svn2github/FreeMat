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
    if (!isFun)
      throw Exception("unable to find a definition for '" + std::string(fname) + "'");
    if (val->type() != FM_M_FUNCTION) 
      throw Exception("no inline help for built-in functions");
    MFunctionDef *mptr;
    Interface *io;
    mptr = (MFunctionDef *) val;
    mptr->updateCode();
    io = eval->getInterface();
    for (int i=0;i<mptr->helpText.size();i++)
      io->outputMessage(mptr->helpText[i].c_str());
    return ArrayVector();
  }

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

  ArrayVector LengthFunction(int nargout, const ArrayVector& arg) {
    Array A(Array::int32Constructor(arg[0].getDimensions().getMax()));
    ArrayVector retval;
    retval.push_back(A);
    return retval;
  }

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
