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

#include "avcall.h"
#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "Exception.hpp"
#include <stdio.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <iostream>
#include <signal.h>
#include "SymbolTable.hpp"
#include "Types.hpp"
#include "MexInterface.hpp"

#ifdef WIN32
#define snprintf _snprintf
#endif


#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

MFunctionDef::MFunctionDef() {
  functionCompiled = false;
  timeStamp = 0;
  localFunction = false;
  nextFunction = NULL;
  prevFunction = NULL;
  pcodeFunction = false;
}

MFunctionDef::~MFunctionDef() {
  if (nextFunction != NULL)
    delete nextFunction;
}

int MFunctionDef::inputArgCount() {
  if (arguments.size() == 0) return 0;
  if (arguments[arguments.size()-1] == "varargin")
    return -1;
  else
    return arguments.size();
}

int MFunctionDef::outputArgCount() {
  if (returnVals.size() == 0) return 0;
  if (returnVals[returnVals.size()-1] == "varargout")
    return -1;
  else
    return returnVals.size();
}

void MFunctionDef::printMe(Interpreter*eval) {
  stringVector tmp;
  snprintf(msgBuffer,MSGBUFLEN,"Function name:%s\n",name.c_str());
  eval->outputMessage(msgBuffer);
  eval->outputMessage("Function class: Compiled M function\n");
  eval->outputMessage("returnVals: ");
  tmp = returnVals;
  int i;
  for (i=0;i<tmp.size();i++) {
    snprintf(msgBuffer,MSGBUFLEN,"%s ",tmp[i].c_str());
    eval->outputMessage(msgBuffer);
  }
  eval->outputMessage("\n");
  eval->outputMessage("arguments: ");
  tmp = arguments;
  for (i=0;i<tmp.size();i++) {
    snprintf(msgBuffer,MSGBUFLEN,"%s ",tmp[i].c_str());
    eval->outputMessage(msgBuffer);
  }
  eval->outputMessage("\ncode: \n");
  code.print();
}

ArrayVector MFunctionDef::evaluateFunction(Interpreter *walker, 
					   ArrayVector& inputs, 
					   int nargout) {
  ArrayVector outputs;
  Context* context;
  bool warningIssued;
  int minCount;
    
  if (!code.valid()) return outputs;
  context = walker->getContext();
  context->pushScope(name);
  walker->pushDebug(fileName,name);
  // Push our local functions onto the function scope
  MFunctionDef *cp;
  // Walk up until we get to the head of the list
  cp = this;
  while (cp->prevFunction != NULL) cp = cp->prevFunction;
  cp = cp->nextFunction;
  while (cp != NULL) {
    context->insertFunctionLocally((FuncPtr) cp);
    cp = cp->nextFunction;
  }
  // When the function is called, the number of inputs is
  // at sometimes less than the number of arguments requested.
  // Check the argument count.  If this is a non-varargin
  // argument function, then use the following logic:
  minCount = 0;
  if (inputArgCount() != -1) {
    minCount = (inputs.size() < arguments.size()) ? 
      inputs.size() : arguments.size();
    for (int i=0;i<minCount;i++) {
      std::string arg(arguments[i]);
      if (arg[0] == '&')
	arg.erase(0,1);
      context->insertVariableLocally(arg,inputs[i]);
    }
    context->insertVariableLocally("nargin",
				   Array::int32Constructor(minCount));
  } else {
    // Count the number of supplied arguments
    int inputCount = inputs.size();
    context->insertVariableLocally("nargin",
				   Array::int32Constructor(inputCount));
    // Get the number of explicit arguments
    int explicitCount = arguments.size() - 1;
    // For each explicit argument (that we have an input for),
    // insert it into the scope.
    minCount = (explicitCount < inputCount) ? explicitCount : inputCount;
    int i;
    for (i=0;i<minCount;i++) {
      std::string arg(arguments[i]);
      if (arg[0] == '&')
	arg.erase(0,1);
      context->insertVariableLocally(arg,inputs[i]);
    }
    inputCount -= minCount;
    // Put minCount...inputCount 
    Array varg(FM_CELL_ARRAY);
    varg.vectorResize(inputCount);
    Array* dp = (Array *) varg.getReadWriteDataPointer();
    for (i=0;i<inputCount;i++)
      dp[i] = inputs[i+minCount];
    context->insertVariableLocally("varargin",varg);
  }
  context->insertVariableLocally("nargout",
				 Array::int32Constructor(nargout));
  try {
    try {
      walker->block(code);
    } catch (InterpreterBreakException& e) {
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterReturnException& e) {
    }
    warningIssued = false;
    if (outputArgCount() != -1) {
      // special case - if nargout == 0, and none of the
      // outputs are predefined, we don't do anything
      bool nonpredefed = true;
      for (int i=0;i<returnVals.size()&&nonpredefed;i++) {
	Array *ptr = context->lookupVariableLocally(returnVals[i]);
	nonpredefed = nonpredefed && (!ptr);
      }
      if ((nargout > 0) || 
	  ((nargout == 0) && (!nonpredefed))) {
	outputs = ArrayVector(returnVals.size());
	for (int i=0;i<returnVals.size();i++) {
	  Array *ptr = context->lookupVariableLocally(returnVals[i]);
	  if (!ptr)
	    outputs[i] = Array::emptyConstructor();
	  else
	    outputs[i] = *ptr;
	  if (!ptr && (i < nargout))
	    if (!warningIssued) {
	      walker->warningMessage("one or more outputs not assigned in call (1)");
	      warningIssued = true;
	    }
	}
      }
    } else {
      outputs = ArrayVector(nargout);
      int explicitCount = returnVals.size() - 1;
      // For each explicit argument (that we have), insert it
      // into the scope.
      for (int i=0;i<explicitCount;i++) {
	Array *ptr = context->lookupVariableLocally(returnVals[i]);
	if (!ptr)
	  outputs[i] = Array::emptyConstructor();
	else
	  outputs[i] = *ptr;
	if (!ptr  && (i < nargout)) 
	  if (!warningIssued) {
	    walker->warningMessage("one or more outputs not assigned in call (2)");
	    warningIssued = true;
	  }
      }
      // Are there any outputs not yet filled?
      if (nargout > explicitCount) {
	Array varargout, *ptr;
	// Yes, get a pointer to the "vargout" variable that should be defined
	ptr = context->lookupVariableLocally("varargout");
	if (!ptr)
	  throw Exception("The special variable 'varargout' was not defined as expected");
	varargout = *ptr;
	if (varargout.getDataClass() != FM_CELL_ARRAY)
	  throw Exception("The special variable 'varargout' was not defined as a cell-array");
	// Get the data pointer
	const Array *dp = ((const Array*) varargout.getDataPointer());
	// Get the length
	int varlen = varargout.getLength();
	int toFill = nargout - explicitCount;
	if (toFill > varlen) 
	  throw Exception("Not enough outputs in varargout to satisfy call");
	for (int i=0;i<toFill;i++)
	  outputs[explicitCount+i] = dp[i];
      }
    }
    // Check for arguments that were passed by reference, and 
    // update their values.
    for (int i=0;i<minCount;i++) {
      std::string arg(arguments[i]);
      if (arg[0] == '&')
	arg.erase(0,1);
      Array *ptr = context->lookupVariableLocally(arg);
      if (ptr)
	inputs[i] = *ptr;
    }
    context->popScope();
    walker->popDebug();
    return outputs;
  } catch (Exception& e) {
    context->popScope();
    walker->popDebug();
    throw;
  }
  catch (InterpreterRetallException& e) {
    context->popScope();
    walker->popDebug();
    throw;
  }
}
  
static string ReadFileIntoString(FILE *fp) {
  struct stat st;
  clearerr(fp);
  fstat(fileno(fp),&st);
  long cpos = st.st_size;
  // Allocate enough for the text, an extra newline, and null
  char *buffer = (char*) calloc(cpos+2,sizeof(char));
  int n = fread(buffer,sizeof(char),cpos,fp);
  buffer[n]='\n';
  buffer[n+1]=0;
  string retval(buffer);
  free(buffer);
  return retval;
}

stringVector IdentifierList(tree t) {
  stringVector retval;
  for (unsigned index=0;index<t.numchildren();index++) {
    if (t.child(index).is('&'))
      retval.push_back("&" + t.child(index).first().text());
    else
      retval.push_back(t.child(index).text());
  }
  return retval;
}

MFunctionDef* ConvertParseTreeToMFunctionDef(tree t, string fileName) {
  MFunctionDef *fp = new MFunctionDef;
  fp->returnVals = IdentifierList(t.first());
  fp->name = t.second().text();
  fp->arguments = IdentifierList(t.child(2));
  fp->code = t.child(3);
  fp->fileName = fileName;
  return fp;
}

MFunctionDef* ConvertParseTreeToMFunctionDefs(treeVector t, 
					      string fileName) {
  MFunctionDef* last = NULL;
  for (int index = t.size()-1;index>=0;index--) {
    MFunctionDef* rp = ConvertParseTreeToMFunctionDef(t[index],fileName);
    if (index>0)
      rp->localFunction = true;
    else
      rp->localFunction = false;
    rp->nextFunction = last;
    if (last)
      last->prevFunction = rp;
    last = rp;
  }
  return last;
}
  
// Compile the function...
bool MFunctionDef::updateCode() {
  if (localFunction) return false;
  if (pcodeFunction) return false;
  // First, stat the file to get its time stamp
  struct stat filestat;
  stat(fileName.c_str(),&filestat);
  if (!functionCompiled || (filestat.st_mtime != timeStamp)) {
    // Record the time stamp
    timeStamp = filestat.st_mtime;
    // Next, open the function's file
    FILE *fp = fopen(fileName.c_str(),"r");
    if (fp == NULL) 
      throw Exception(std::string("Unable to open file :") + fileName);
    // read lines until we get to a non-comment line
    bool commentsOnly;
    commentsOnly = true;
    helpText.clear();
    char buffer[1000];
    while (!feof(fp) && commentsOnly) {
      fgets(buffer,1000,fp);
      char *cp;
      cp = buffer;
      while ((*cp == ' ') || (*cp == '\t'))
	cp++;
      if (*cp == '\n')
	continue;
      if (*cp != '%') 
	commentsOnly = false;
      else
	helpText.push_back(++cp);
    }
    if (helpText.size() == 0)
      helpText.push_back(buffer);
    if (feof(fp)) {
      functionCompiled = true;
      code = NULL;
      return true;
    }
    rewind(fp);
    try {
      // Read the file into a string
      string fileText = ReadFileIntoString(fp);
      Scanner S(fileText,fileName);
      Parser P(S);
      tree pcode = P.Process();
      allCode = pcode;
      if (pcode.is(TOK_FUNCTION_DEFS)) {
	scriptFlag = false;
	MFunctionDef *fp = ConvertParseTreeToMFunctionDefs(pcode.children(),
							   fileName);
	returnVals = fp->returnVals;
	code = fp->code;
	nextFunction = fp->nextFunction;
	if (nextFunction)
	  fp->nextFunction->prevFunction = this;
	prevFunction = false;
	localFunction = false;
	arguments = fp->arguments;
	functionCompiled = true;
      } else {
	scriptFlag = true;
	functionCompiled = true;
	code = pcode.first();
      }
    } catch (Exception &e) {
      if (fp) fclose(fp);
      functionCompiled = false;
      throw;
    }
    return true;
  } 
  return false;
}

// Find the smallest line number larger than the argument
// if our line number is larger than the target, then we
// 
void TreeLine(tree t, unsigned &bestLine, unsigned lineTarget) {
  if (!t.valid()) return;
  if (t.is(TOK_QSTATEMENT) || t.is(TOK_STATEMENT)) {
    unsigned myLine = (t.context() & 0xffff);
    if ((myLine >= lineTarget) && (myLine < bestLine))
      bestLine = myLine;
  }
  for (int i=0;i<t.numchildren();i++)
    TreeLine(t.child(i),bestLine,lineTarget);
}

bool SetTreeBreakPoint(tree t, int lineNumber, byte flags) {
  if (!t.valid()) return false;
  if (t.is(TOK_QSTATEMENT) || t.is(TOK_STATEMENT)) {
    if ((t.context() & 0xffff) == lineNumber) {
      t.setflag(flags);
      return true;
    }
  }
  for (int i=0;i<t.numchildren();i++)
    if (SetTreeBreakPoint(t.child(i),lineNumber,flags)) return true;
  return false;
}

bool ClearTreeBreakPoint(tree t, int lineNumber, byte flags) {
  if (!t.valid()) return false;
  if (t.is(TOK_QSTATEMENT) || t.is(TOK_STATEMENT)) {
    if ((t.context() & 0xffff) == lineNumber) {
      t.clearflag(flags);
      return true;
    }
  }
  for (int i=0;i<t.numchildren();i++)
    if (ClearTreeBreakPoint(t.child(i),lineNumber,flags)) return true;
  return false;
}


// Find the closest line number to the requested 
unsigned MFunctionDef::ClosestLine(unsigned line) {
  unsigned bestline;
  bestline = 1000000000;
  TreeLine(allCode,bestline,line);
  if (bestline == 1000000000)
    throw Exception(string("Unable to find a line close to ") + 
		    line + string(" in routine ") + name);
  return bestline;
}

void MFunctionDef::SetBreakpoint(int bpline, byte flags) {
  if (!SetTreeBreakPoint(allCode,bpline,flags)) 
    throw Exception(string("Unable to set a breakpoint at line ") + bpline + 
		    string(" of routine ") + name);
}

void MFunctionDef::DeleteBreakpoint(int bpline, byte flags) {
  ClearTreeBreakPoint(allCode,bpline,flags);
}

void FreezeMFunction(MFunctionDef *fptr, Serialize *s) {
  //     s->putString(fptr->name.c_str());
  //     s->putBool(fptr->scriptFlag);
  //     s->putStringVector(fptr->arguments);
  //     s->putStringVector(fptr->returnVals);
  //     s->putBool(fptr->functionCompiled);
  //     s->putBool(fptr->localFunction);
  //     s->putStringVector(fptr->helpText);
  //     FreezeAST(fptr->code,s);
  //     if (fptr->nextFunction) {
  //       s->putBool(true);
  //       FreezeMFunction(fptr->nextFunction,s);
  //     } else {
  //       s->putBool(false);
  //     }
}

MFunctionDef* ThawMFunction(Serialize *s) {
  //     MFunctionDef *t = new MFunctionDef();
  //     t->name = std::string(s->getString());
  //     t->scriptFlag = s->getBool();
  //     t->arguments = s->getStringVector();
  //     t->returnVals = s->getStringVector();
  //     t->functionCompiled = s->getBool();
  //     t->localFunction = s->getBool();
  //     t->helpText = s->getStringVector();
  //     t->code = ThawAST(s);
  //     bool nextFun = s->getBool();
  //     if (nextFun) {
  //       t->nextFunction = ThawMFunction(s);
  //       t->nextFunction->prevFunction = t;
  //     } else
  //       t->nextFunction = NULL;
  //     return t;
}

BuiltInFunctionDef::BuiltInFunctionDef() {
}

BuiltInFunctionDef::~BuiltInFunctionDef() {
}

int BuiltInFunctionDef::inputArgCount() {
  return argCount;
}

int BuiltInFunctionDef::outputArgCount() {
  return retCount;
}

void BuiltInFunctionDef::printMe(Interpreter *eval) {
  stringVector tmp;
  snprintf(msgBuffer,MSGBUFLEN," Function name:%s\n",name.c_str());
  eval->outputMessage(msgBuffer);
  eval->outputMessage(" Function class: Built in\n");
  snprintf(msgBuffer,MSGBUFLEN," Return count: %d\n",retCount);
  eval->outputMessage(msgBuffer);
  snprintf(msgBuffer,MSGBUFLEN," Argument count: %d\n",argCount);
  eval->outputMessage(msgBuffer);
}


ArrayVector BuiltInFunctionDef::evaluateFunction(Interpreter *walker, ArrayVector& inputs, 
						 int nargout) {
  ArrayVector outputs;
  int i;
  walker->pushDebug(name,"built in");
  try {
    outputs = fptr(nargout,inputs);
    walker->popDebug();
    return outputs;
  } catch(Exception& e) {
    walker->popDebug();
    throw;
  }
  catch (InterpreterRetallException& e) {
    walker->popDebug();
    throw;
  }
}

SpecialFunctionDef::SpecialFunctionDef() {
}

SpecialFunctionDef::~SpecialFunctionDef() {
}

ArrayVector SpecialFunctionDef::evaluateFunction(Interpreter *walker, 
						 ArrayVector& inputs, int nargout) {
  ArrayVector outputs;
  walker->pushDebug(name,"built in");
  try {
    outputs = fptr(nargout,inputs,walker);
    walker->popDebug();
    return outputs;
  } catch(Exception& e) {
    walker->popDebug();
    throw;
  }
  catch (InterpreterRetallException& e) {
    walker->popDebug();
    throw;
  }
}

void SpecialFunctionDef::printMe(Interpreter *eval) {
}

FunctionDef::FunctionDef() {
  scriptFlag = false;
  graphicsFunction = false;
}

FunctionDef::~FunctionDef() {
}


ImportedFunctionDef::ImportedFunctionDef(GenericFuncPointer address_arg,
					 stringVector types_arg,
					 stringVector arguments_arg,
					 treeVector sizeChecks,
					 std::string retType_arg) {
  address = address_arg;
  types = types_arg;
  arguments = arguments_arg;
  sizeCheckExpressions = sizeChecks;
  retType = retType_arg;
  /*
   * Set up the cif...
   */
  argCount = types_arg.size();
  if (retType == "void") 
    retCount = 0;
  else
    retCount = 1;
}

ImportedFunctionDef::~ImportedFunctionDef() {
}

void ImportedFunctionDef::printMe(Interpreter *) {
}

Class mapTypeNameToClass(std::string name) {
  if (name == "logical") return FM_LOGICAL;
  if (name == "uint8") return FM_UINT8;
  if (name == "int8") return FM_INT8;
  if (name == "uint16") return FM_UINT16;
  if (name == "int16") return FM_INT16;
  if (name == "uint32") return FM_UINT32;
  if (name == "int32") return FM_INT32;
  if (name == "float") return FM_FLOAT;
  if (name == "complex") return FM_COMPLEX;  
  if (name == "double") return FM_DOUBLE;
  if (name == "dcomplex") return FM_DCOMPLEX;
  if (name == "string") return FM_STRING;
  if (name == "void") return FM_UINT32;
  throw Exception("unrecognized type " + name + " in imported function setup");
}

ArrayVector ImportedFunctionDef::evaluateFunction(Interpreter *walker,
						  ArrayVector& inputs,
						  int nargout) {
  walker->pushDebug(name,"imported");
  /**
   * To actually evaluate the function, we have to process each of
   * the arguments and get them into the right form.
   */
  int i;
  for (i=0;i<inputs.size();i++)
    inputs[i].promoteType(mapTypeNameToClass(types[i]));
  /**
   * Next, we count how many of the inputs are to be passed by
   * reference.
   */
  int passByReference = 0;
  for (int j=0;j<inputs.size();j++)
    if ((arguments[j][0] == '&') || (types[j] == "string") ||
	(sizeCheckExpressions[j].valid()))
      passByReference++;
  /**
   * Next, we check to see if any bounds-checking expressions are
   * active.
   */
  bool boundsCheckActive = false;
  int m=0;
  while (m < inputs.size() && !boundsCheckActive)
    boundsCheckActive = (sizeCheckExpressions[m++].valid());
  if (boundsCheckActive) {
    /**
     * If the bounds-checking is active, we have to create a 
     * new context, and insert the defined arguments into the
     * context (much as for an M-function call).
     */
    Context* context;
    context = walker->getContext();
    context->pushScope("temp");
    walker->pushDebug(name,"bounds check");
    try {
      for (i=0;i<inputs.size();i++)
	context->insertVariableLocally(arguments[i],inputs[i]);
      /*
       * Next, evaluate each size check expression
       */
      for (i=0;i<inputs.size();i++) {
	if (sizeCheckExpressions[i].valid()) {
	  Array ret(walker->expression(sizeCheckExpressions[i]));
	  ret.promoteType(FM_INT32);
	  int len;
	  len = ret.getContentsAsIntegerScalar();
	  if (len != inputs[i].getLength()) {
	    throw Exception("array input " + arguments[i] + 
			    " length different from computed bounds" + 
			    " check length");
	  }
	}
      }
    } catch (Exception& e) {
      context->popScope();
      throw;
    } catch (InterpreterRetallException& e) {
      context->popScope();
      throw;
    }
    context->popScope();
    walker->popDebug();
  }
      
  /**
   * Allocate an array of pointers to store for variables passed
   * by reference.
   */
  void **refPointers;
  refPointers = (void**) malloc(sizeof(void*)*passByReference);
  /** 
   * Allocate an array of value pointers...
   */
  void **values;
  values = (void**) malloc(sizeof(void*)*inputs.size());
  int ptr = 0;
  for (i=0;i<inputs.size();i++) {
    if (types[i] != "string") {
      if ((arguments[i][0] == '&') || (sizeCheckExpressions[i].valid())) {
	refPointers[ptr] = inputs[i].getReadWriteDataPointer();
	values[i] = &refPointers[ptr];
	ptr++;
      } else {
	values[i] = inputs[i].getReadWriteDataPointer();
      }
    } else {
      refPointers[ptr] = inputs[i].getContentsAsCString();
      values[i] = &refPointers[ptr];
      ptr++;
    }
  }


  Array retArray;
  // The argument list object
  av_alist alist;
  // Holders for the return values
  uint8 ret_uint8;
  int8 ret_int8;
  uint16 ret_uint16;
  int16 ret_int16;
  uint32 ret_uint32;
  int32 ret_int32;
  float ret_float;
  double ret_double;

  // First pass - based on the return type, call the right version of av_start_*
  if ((retType == "uint8") || (retType == "logical")) {
    av_start_uchar(alist,address,&ret_uint8);
  } else if (retType == "int8") {      
    av_start_schar(alist,address,&ret_int8);
  } else if (retType == "uint16") {
    av_start_ushort(alist,address,&ret_uint16);
  } else if (retType == "int16") {
    av_start_short(alist,address,&ret_int16);
  } else if (retType == "uint32") {
    av_start_uint(alist,address,&ret_uint32);
  } else if (retType == "int32") {
    av_start_int(alist,address,&ret_int32);
  } else if (retType == "float") {
    av_start_float(alist,address,&ret_float);
  } else if (retType == "double") {
    av_start_double(alist,address,&ret_double);
  } else if (retType == "void") {
    av_start_void(alist,address);
  } else
    throw Exception("Unsupported return type " + retType + " in imported function call");
    
  // Second pass - Loop through the arguments
  for (int i=0;i<types.size();i++) {
    if (arguments[i][0] == '&' || types[i] == "string" ||
	sizeCheckExpressions[i].valid())
      av_ptr(alist,void*,*((void**)values[i]));
    else {
      if ((types[i] == "logical") || (types[i] == "uint8"))
	av_uchar(alist,*((uint8*)values[i]));
      else if (types[i] == "int8")
	av_char(alist,*((int8*)values[i]));
      else if (types[i] == "uint16")
	av_ushort(alist,*((uint16*)values[i]));
      else if (types[i] == "int16")
	av_short(alist,*((int16*)values[i]));
      else if (types[i] == "uint32")
	av_uint(alist,*((uint32*)values[i]));
      else if (types[i] == "int32")
	av_int(alist,*((int32*)values[i]));
      else if ((types[i] == "float") || (types[i] == "complex"))
	av_float(alist,*((float*)values[i]));
      else if ((types[i] == "double") || (types[i] == "dcomplex"))
	av_double(alist,*((double*)values[i]));
    }
  }

  // Call the function
  av_call(alist);

  // Extract the return value
  if ((retType == "uint8") || (retType == "logical")) {
    retArray = Array::uint8Constructor(ret_uint8);
  } else if (retType == "int8") {
    retArray = Array::int8Constructor(ret_int8);
  } else if (retType == "uint16") {
    retArray = Array::uint16Constructor(ret_uint16);
  } else if (retType == "int16") {
    retArray = Array::int16Constructor(ret_int16);
  } else if (retType== "uint32") {
    retArray = Array::uint32Constructor(ret_uint32);
  } else if (retType == "int32") {
    retArray = Array::int32Constructor(ret_int32);
  } else if ((retType == "float") || (retType == "complex")) {
    retArray = Array::floatConstructor(ret_float);
  } else if ((retType == "double") || (retType == "dcomplex")) {
    retArray = Array::doubleConstructor(ret_double);
  } else {
    retArray = Array::emptyConstructor();
  }
    
  // Strings that were passed by reference have to be
  // special-cased
  ptr = 0;
  for (i=0;i<inputs.size();i++) {
    if ((arguments[i][0] == '&') || (types[i] == "string")) {
      if ((types[i] == "string") && (arguments[i][0] == '&'))
	inputs[i] = Array::stringConstructor((char*) refPointers[ptr]);
      ptr++;
    }
  }

  free(refPointers);
  free(values);
  walker->popDebug();
  return singleArrayVector(retArray);
}

MexFunctionDef::MexFunctionDef(std::string fullpathname) {
  fullname = fullpathname;
  importSuccess = false;
  lib = new DynLib(fullname);
  try {
    address = (mexFuncPtr) lib->GetSymbol("mexFunction");  
    importSuccess = true;
  } catch (Exception& e) {
  }
}

bool MexFunctionDef::LoadSuccessful() {
  return importSuccess;
}
  
MexFunctionDef::~MexFunctionDef() {
}

void MexFunctionDef::printMe(Interpreter *) {
}
  
ArrayVector MexFunctionDef::evaluateFunction(Interpreter *walker, 
					     ArrayVector& inputs, 
					     int nargout) {
  // Convert arguments to mxArray
  mxArray** args = new mxArray*[inputs.size()];
  for (int i=0;i<inputs.size();i++)
    args[i] = MexArrayFromArray(inputs[i]);
  // Allocate output array vector
  int lhsCount = nargout;
  lhsCount = (lhsCount < 1) ? 1 : lhsCount;
  mxArray** rets = new mxArray*[lhsCount];
  try {
    address(lhsCount,rets,inputs.size(),(const mxArray**)args);
  } catch (std::string &e) {
    throw Exception(e);
  }
  ArrayVector retvec;
  for (int i=0;i<lhsCount;i++) {
    retvec.push_back(ArrayFromMexArray(rets[i]));
    mxDestroyArray(rets[i]);
  }
  delete[] rets;
  return retvec;
}

