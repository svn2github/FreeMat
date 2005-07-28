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

#include "ffi.h"
#include "FunctionDef.hpp"
#include "WalkTree.hpp"
#include "ParserInterface.hpp"
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

#ifdef WIN32
#define snprintf _snprintf
#endif


namespace FreeMat {

#define MSGBUFLEN 2048
  static char msgBuffer[MSGBUFLEN];

  class CType {
  public:
    ffi_type* FFIType;
    Class FMClass;
    CType();
    CType(ffi_type* baseType, Class baseClass);
  };

  CType::CType() {
  }

  CType::CType(ffi_type* baseType, Class baseClass) {
    FFIType = baseType;
    FMClass = baseClass;
  }

  typedef SymbolTable<CType> FFITypeTable;
  static bool ImportTablesInitialized = false;
  FFITypeTable ffiTypes;

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

  void MFunctionDef::printMe(Interface*io) {
    stringVector tmp;
    snprintf(msgBuffer,MSGBUFLEN,"Function name:%s\n",name.c_str());
    io->outputMessage(msgBuffer);
    io->outputMessage("Function class: Compiled M function\n");
    io->outputMessage("returnVals: ");
    tmp = returnVals;
	int i;
    for (i=0;i<tmp.size();i++) {
      snprintf(msgBuffer,MSGBUFLEN,"%s ",tmp[i].c_str());
      io->outputMessage(msgBuffer);
    }
    io->outputMessage("\n");
    io->outputMessage("arguments: ");
    tmp = arguments;
    for (i=0;i<tmp.size();i++) {
      snprintf(msgBuffer,MSGBUFLEN,"%s ",tmp[i].c_str());
      io->outputMessage(msgBuffer);
    }
    io->outputMessage("\ncode: \n");
    printAST(code);
  }

  ArrayVector MFunctionDef::evaluateFunction(WalkTree *walker, 
					     ArrayVector& inputs, 
					     int nargout) {
    ArrayVector outputs;
    Context* context;
    bool warningIssued;
    int minCount;

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
      } catch (WalkTreeBreakException& e) {
      } catch (WalkTreeContinueException& e) {
      } catch (WalkTreeReturnException& e) {
      }
      warningIssued = false;
      if (outputArgCount() != -1) {
	outputs = ArrayVector(returnVals.size());
	for (int i=0;i<returnVals.size();i++) {
	  Array *ptr = context->lookupVariableLocally(returnVals[i]);
	  if (!ptr)
	    outputs[i] = Array::emptyConstructor();
	  else
	    outputs[i] = *ptr;
	  if (!ptr && (i < nargout))
	    if (!warningIssued) {
	      walker->getInterface()->warningMessage("one or more outputs not assigned in call (1)");
	      warningIssued = true;
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
	      walker->getInterface()->warningMessage("one or more outputs not assigned in call (2)");
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
    catch (WalkTreeRetallException& e) {
      context->popScope();
      walker->popDebug();
      throw;
    }
  }

  // Compile the function...
  void MFunctionDef::updateCode() {
    if (localFunction) return;
    if (pcodeFunction) return;
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
      while (commentsOnly) {
	fgets(buffer,1000,fp);
	char *cp;
	cp = buffer;
	while ((*cp == ' ') || (*cp == '\t'))
	  cp++;
	if (*cp == '\n')
	  break;
	if (*cp != '%') 
	  commentsOnly = false;
	else
	  helpText.push_back(++cp);
      }
      if (helpText.size() == 0)
	helpText.push_back(buffer);
      rewind(fp);
      try {
	ParserState pstate = parseFile(fp,fileName.c_str());
	fclose(fp);
	fp = NULL;
	// If pstate is a FuncDef, then get the parsed data
	if (pstate == FuncDef) {
	  MFunctionDef *cp = getParsedFunctionDef();
	  scriptFlag = false;
	  returnVals = cp->returnVals;
	  arguments = cp->arguments;
	  code = cp->code;
	  functionCompiled = true;
	  nextFunction = cp->nextFunction;
	  prevFunction = cp->prevFunction;
	  RestoreBreakpoints();
	  return;
	} else if (pstate == ScriptBlock) {
	  code = getParsedScriptBlock();
	  scriptFlag = true;
	  functionCompiled = true;
	  RestoreBreakpoints();
	  return;
	} else
	  throw Exception(std::string("Syntax error parsing file:") + fileName + ", expecting a script or function definition");
      } catch (Exception &e) {
	if (fp) fclose(fp);
	functionCompiled = false;
	throw;
      }
    }
  }

  void MFunctionDef::RestoreBreakpoints() {
    for (int i=0;i<breakPoints.size();i++) 
      try {
	AddBreakpoint(breakPoints[i]);
      } catch (Exception &e) {
      }
  }

  void MFunctionDef::RemoveBreakpoint(int bpline) {
    // The way this function works...  We have to
    // find which subfunction to set the breakpoint in.
    // Build a vector of the function defs
    std::vector<ASTPtr> codes;
    codes.push_back(code);
    MFunctionDef *ptr = nextFunction;
    while (ptr) {
      codes.push_back(ptr->code);
      ptr = ptr->nextFunction;
    }
    // Search backward through the subfunction definitions
    int i=codes.size()-1;
    bool found = false;
    while ((i>=0) && !found) {
      found = CheckASTBreakPoint(codes[i],bpline);
      if (!found)
	i--;
    }
    if (!found)
      throw Exception("Unable to clear breakpoint...");
    ClearASTBreakPoint(codes[i],bpline);
  }
  
  void MFunctionDef::AddBreakpoint(int bpline) {
    // The way this function works...  We have to
    // find which subfunction to set the breakpoint in.
    // Build a vector of the function defs
    std::vector<ASTPtr> codes;
    codes.push_back(code);
    MFunctionDef *ptr = nextFunction;
    while (ptr) {
      codes.push_back(ptr->code);
      ptr = ptr->nextFunction;
    }
    // Search backward through the subfunction definitions
    int i=codes.size()-1;
    bool found = false;
    while ((i>=0) && !found) {
      found = CheckASTBreakPoint(codes[i],bpline);
      if (!found)
	i--;
    }
    if (!found) {
      char buffer[1000];
      sprintf(buffer,"Unable to set a breakpoint at line %d of routine %s",bpline,name.c_str());
      throw Exception(buffer);
    }
    SetASTBreakPoint(codes[i],bpline);
  }

  void MFunctionDef::SetBreakpoint(int bpline) {
    // Add it to the list of breakpoints for this function
    AddBreakpoint(bpline);
    breakPoints.push_back(bpline);
  }

  void MFunctionDef::DeleteBreakpoint(int bpline) {
    for (std::vector<int>::iterator i=breakPoints.begin();
	 i!=breakPoints.end();i++)
      if (*i == bpline)
	breakPoints.erase(i);
    RemoveBreakpoint(bpline);
  }

  void FreezeMFunction(MFunctionDef *fptr, Serialize *s) {
    s->putString(fptr->name.c_str());
    s->putBool(fptr->scriptFlag);
    s->putStringVector(fptr->arguments);
    s->putStringVector(fptr->returnVals);
    s->putBool(fptr->functionCompiled);
    s->putBool(fptr->localFunction);
    s->putStringVector(fptr->helpText);
    FreezeAST(fptr->code,s);
    if (fptr->nextFunction) {
      s->putBool(true);
      FreezeMFunction(fptr->nextFunction,s);
    } else {
      s->putBool(false);
    }
  }

  MFunctionDef* ThawMFunction(Serialize *s) {
    MFunctionDef *t = new MFunctionDef();
    t->name = std::string(s->getString());
    t->scriptFlag = s->getBool();
    t->arguments = s->getStringVector();
    t->returnVals = s->getStringVector();
    t->functionCompiled = s->getBool();
    t->localFunction = s->getBool();
    t->helpText = s->getStringVector();
    t->code = ThawAST(s);
    bool nextFun = s->getBool();
    if (nextFun) {
      t->nextFunction = ThawMFunction(s);
      t->nextFunction->prevFunction = t;
    } else
      t->nextFunction = NULL;
    return t;
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

  void BuiltInFunctionDef::printMe(Interface *io) {
    stringVector tmp;
    snprintf(msgBuffer,MSGBUFLEN," Function name:%s\n",name.c_str());
    io->outputMessage(msgBuffer);
    io->outputMessage(" Function class: Built in\n");
    snprintf(msgBuffer,MSGBUFLEN," Return count: %d\n",retCount);
    io->outputMessage(msgBuffer);
    snprintf(msgBuffer,MSGBUFLEN," Argument count: %d\n",argCount);
    io->outputMessage(msgBuffer);
    snprintf(msgBuffer,MSGBUFLEN," Address of function: %08x\n",
	     ((int) fptr));
    io->outputMessage(msgBuffer);
  }


  ArrayVector BuiltInFunctionDef::evaluateFunction(WalkTree *walker, ArrayVector& inputs, 
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
    catch (WalkTreeRetallException& e) {
      walker->popDebug();
      throw;
    }
  }

  SpecialFunctionDef::SpecialFunctionDef() {
  }

  SpecialFunctionDef::~SpecialFunctionDef() {
  }

  ArrayVector SpecialFunctionDef::evaluateFunction(WalkTree *walker, 
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
    catch (WalkTreeRetallException& e) {
      walker->popDebug();
      throw;
    }
  }

  void SpecialFunctionDef::printMe(Interface *io) {
  }

  FunctionDef::FunctionDef() {
    scriptFlag = false;
  }

  FunctionDef::~FunctionDef() {
  }

  static Class mapTypeNameToClass(std::string type) {
    CType *ret = ffiTypes.findSymbol(type);
    if (!ret)
      throw Exception("import type " + type + " not defined in type table");
    return ret->FMClass;
  }

  static ffi_type* mapTypeNameToFFTType(std::string type) {
    CType *ret = ffiTypes.findSymbol(type);
    if (!ret)
      throw Exception("import type " + type + " not defined in type table");
    return ret->FFIType;
  }

  void SetupImportTables() {
    ffiTypes.insertSymbol("logical",CType(&ffi_type_uint8,FM_LOGICAL));
    ffiTypes.insertSymbol("uint8",CType(&ffi_type_uint8,FM_UINT8));
    ffiTypes.insertSymbol("int8",CType(&ffi_type_sint8,FM_INT8));
    ffiTypes.insertSymbol("uint16",CType(&ffi_type_uint16,FM_UINT16));
    ffiTypes.insertSymbol("int16",CType(&ffi_type_sint16,FM_INT16));
    ffiTypes.insertSymbol("uint32",CType(&ffi_type_uint32,FM_UINT32));
    ffiTypes.insertSymbol("int32",CType(&ffi_type_sint32,FM_INT32));
    ffiTypes.insertSymbol("float",CType(&ffi_type_float,FM_FLOAT));
    ffiTypes.insertSymbol("complex",CType(&ffi_type_float,FM_COMPLEX));    
    ffiTypes.insertSymbol("double",CType(&ffi_type_double,FM_DOUBLE));
    ffiTypes.insertSymbol("dcomplex",CType(&ffi_type_double,FM_DCOMPLEX));
    ffiTypes.insertSymbol("string",CType(&ffi_type_pointer,FM_STRING));
    ffiTypes.insertSymbol("void",CType(&ffi_type_void,FM_UINT32));
    ImportTablesInitialized = true;
  }

  ImportedFunctionDef::ImportedFunctionDef(GenericFuncPointer address_arg,
					   stringVector types_arg,
					   stringVector arguments_arg,
					   ASTPtrVector sizeChecks,
					   std::string retType_arg) {
    if (!ImportTablesInitialized)
      SetupImportTables();
    address = address_arg;
    types = types_arg;
    arguments = arguments_arg;
    sizeCheckExpressions = sizeChecks;
    retType = retType_arg;
    /*
     * Set up the cif...
     */
    argCount = types_arg.size();
    ffi_type **args;
    args = (ffi_type**) malloc(sizeof(ffi_type*)*argCount);
    for (int i=0;i<argCount;i++) {
      if (arguments[i][0] == '&' || types[i] == "string" ||
	  sizeCheckExpressions[i] != NULL)
	args[i] = &ffi_type_pointer;
      else {
	args[i] = mapTypeNameToFFTType(types[i]);
      }
    }
    cif = (ffi_cif*) new ffi_cif;
    if (ffi_prep_cif((ffi_cif*) cif, FFI_DEFAULT_ABI, argCount,
		     mapTypeNameToFFTType(retType), args) != FFI_OK)
      throw Exception("unable to import function through fft!");
    if (retType == "void") 
      retCount = 0;
    else
      retCount = 1;
  }

  ImportedFunctionDef::~ImportedFunctionDef() {
  }

  void ImportedFunctionDef::printMe(Interface *) {
  }

  ArrayVector ImportedFunctionDef::evaluateFunction(WalkTree *walker,
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
	  (sizeCheckExpressions[j] != NULL))
	passByReference++;
    /**
     * Next, we check to see if any bounds-checking expressions are
     * active.
     */
    bool boundsCheckActive = false;
    int m=0;
    while (m < inputs.size() && !boundsCheckActive)
      boundsCheckActive = (sizeCheckExpressions[m++] != NULL);
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
	  if (sizeCheckExpressions[i] != NULL) {
	    //	    printAST(sizeCheckExpressions[i]);
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
      } catch (WalkTreeRetallException& e) {
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
	if ((arguments[i][0] == '&') || (sizeCheckExpressions[i] != NULL)) {
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
    /*
     * Based on the return type, we call the function...
     */
    if ((retType == "uint8") || (retType == "logical")) {
      uint8 retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::uint8Constructor(retval);
    } else if (retType == "int8") {
      int8 retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::int8Constructor(retval);
    } else if (retType == "uint16") {
      uint16 retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::uint16Constructor(retval);
    } else if (retType == "int16") {
      int16 retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::int16Constructor(retval);
    } else if (retType== "uint32") {
      uint32 retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::uint32Constructor(retval);
    } else if (retType == "int32") {
      int32 retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::int32Constructor(retval);
    } else if ((retType == "float") || (retType == "complex")) {
      float retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::floatConstructor(retval);
    } else if ((retType == "double") || (retType == "dcomplex")) {
      double retval;					   
      ffi_call((ffi_cif*) cif, address, &retval, values);
      retArray = Array::doubleConstructor(retval);
    } else {
      int dummy;
      ffi_call((ffi_cif*) cif, address, &dummy, values);
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

#if 0
  void RegisterImportStruct(std::string structName, 
			    stringVector types_arg,
			    stringVector arguments_arg) {
    if (!ImportTablesInitialized)
      SetupImportTables();
    ffi_type *stype;
    ffi_type **elements;
    stype = (ffi_type *) malloc(sizeof(ffi_type));
    stype->size = 0;
    stype->alignment = 0;
    stype->elements = (ffi_type **) malloc(sizeof(ffi_type*)*(types_arg.size()+1));
    for (int i=0;i<types_arg.size();i++) {
      if (arguments_arg[i][0] == '&')
	stype->elements[i] = &ffi_type_pointer;
      else {
	CType lookup;
	if (!ffiTypes.findSymbol(types_arg[i],lookup))
	  throw Exception("struct definition requires type " + types_arg[i] + " which is undefined!");
	stype->elements[i] = lookup.FFIType;
      }
    }
    stype->elements[types_arg.size()] = NULL;
    CType newtype;
    newtype.isStructure = true;
    newtype.types = types_arg;
    newtype.names = arguments_arg;
    newtype.FFIType = stype;
    newtype.FMClass = FM_STRUCT_ARRAY;
    ffiTypes.insertSymbol(structName,newtype);
  }
#endif
}
