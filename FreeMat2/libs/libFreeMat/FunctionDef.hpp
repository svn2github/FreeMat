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

#ifndef __FunctionDef_hpp__
#define __FunctionDef_hpp__

#include "Array.hpp"
#include "AST.hpp"
#include "Interface.hpp"
#include <sys/stat.h>

namespace FreeMat {

  typedef enum {
    FM_M_FUNCTION,
    FM_BUILT_IN_FUNCTION,
    FM_SPECIAL_FUNCTION,
    FM_IMPORTED_FUNCTION
  } FunctionType;

  class WalkTree;

  typedef ArrayVector APtr;
  typedef APtr (*BuiltInFuncPtr) (int,const APtr&);
  typedef APtr (*SpecialFuncPtr) (int,const APtr&,WalkTree*);

  /** Base class for the function types
   * A FunctionDef class is a base class for the different types
   * of function pointers used.  There are three types of functions
   * available:
   *    - M-functions - these are functions or scripts written by the
   *      user in the interpreted language.
   *    - Built-in functions - these are functions coded in C++ that
   *      implement functionality too difficult/impossible to do in
   *      the language itself.
   *    - Special functions - these are functions coded in C++ that
   *      require access to the internals of the interpreter object.
   * All of these functions have in common a name, a script classification
   * (is it a script or not), a well defined number of input arguments,
   * a well defined number of output arguments, and some means of 
   * being evaluated.
   */
  class FunctionDef {
  public:
    /**
     * The name of the function - must follow identifier rules.
     */
    std::string name;
    /**
     * Is this a script?
     */
    bool scriptFlag;
    /**
     * The names of the arguments to the fuction (analogous to returnVals).
     * Should have "varargin" as the last entry for variable argument
     * functions.
     */
    stringVector arguments;
    /**
     * The constructor.
     */
    FunctionDef();
    /** 
     * The virtual destructor
     */
    virtual ~FunctionDef();
    /**
     * The type of the function (FM_M_FUNCTION, FM_BUILT_IN_FUNCTION,
     * FM_SPECIAL_FUNCTION, FM_IMPORTED_FUNCTION).
     */
    virtual const FunctionType type() = 0;
    /**
     * Print a description of the function
     */
    virtual void printMe(Interface* io) = 0;
    /**
     * The number of inputs required by this function (-1 if variable).
     */
    virtual int inputArgCount() = 0;
    /**
     * The number of outputs returned by this function (-1 if variable).
     */
    virtual int outputArgCount() = 0;
    /**
     * Evaluate the function and return its output.
     */
    virtual ArrayVector evaluateFunction(WalkTree *, ArrayVector& , int) = 0;
    /**
     * Compile the function (if it is required).  We guarantee that this
     * function will be called at least once before evaluateFunction is called.
     */
    virtual void updateCode() {}
  };

  typedef FunctionDef* FunctionDefPtr;

  class MFunctionDef;

  /**
   * An MFunctionDef is a FunctionDef for an interpreted function.  The
   * function pointer stores the name of the file where the function is
   * located, and a time stamp as to when the function was last modified.
   * When the updateCode() member function is called, the contents of
   * the file are parsed (if necessary), and the resulting AST code tree
   * is stored.  The number of input and output arguments are computed
   * based on the contents of the returnVals and arguments stringVectors.
   */
  class MFunctionDef : public FunctionDef {
  public:
    /**
     * The names of the return values - this is a vector of strings with
     * one entry for each return value in the declared function.  Thus,
     * if the function is declared as "function [a,b] = foo(x)", then
     * returnVals contains two entries: "a", and "b".  For variable
     * return functions, the last entry should be "varargout".
     */
    stringVector returnVals;
    /**
     * The AST for the code that defines the function (only the body of the
     * function is contained in this AST, not the function declaration itself).
     */
    ASTPtr code;
    /**
     * Flag to indicate if the function has been compiled.
     */
    bool functionCompiled;
    /**
     * Location of the function's defining file in the current filesystem.
     */
    std::string fileName;
    /**
     * Time function was last modified.
     */
    time_t timeStamp;
    /**
     * For some function files, there are multiple functions defined in
     * a single file.  The subsequent functions are local to the function
     * scope of the main function, and override global functions inside
     * the body of the current function (they are essentially hidden inside
     * the scope of the current function).  These functions are parsed
     * and form a linked list of function definitions, with the main function
     * at the head of the list.
     */
    MFunctionDef *nextFunction;
    MFunctionDef *prevFunction;
    /**
     * Set to true for all of the localFunctions.  False for the head of the
     * linked list.
     */
    bool localFunction;
    bool pcodeFunction;
    /**
     * The help text.
     */
    stringVector helpText;
    /**
     * The constructor.
     */
    MFunctionDef();
    /**
     * The destructor
     */
    ~MFunctionDef();
    /** The type of the function
     */
    virtual const FunctionType type() {return FM_M_FUNCTION;}
    /** Print a description of the function
     */
    virtual void printMe(Interface* io);
    /** 
     * The number of inputs required by this function, which is the number of 
     * elements in arguments unless the last element is the keyword "varargin"
     * in which case the answer is -1.
     */
    virtual int inputArgCount();
    /** 
     * The number of outputs returned by this function, which is the number of 
     * elements in returnVals unless the last element is the keyword "varargout"
     * in which case the answer is -1.
     */
    virtual int outputArgCount();
    /**
     * Evaluate the function and return the outputs.
     * Throws an Exception if
     *   - the special variable 'varargout' is not defined in the body of the
     *     of the function as promised by the function declaration.
     *   - the variable 'varargout' contains too few elements to satisfy the
     *     number of return values in the call
     *   - the variable 'varargout' is the wrong type.
     */
    virtual ArrayVector evaluateFunction(WalkTree *, ArrayVector &, int);
    /**
     * Check the timestamp on the file, and if necessary, recompile the 
     * function.  Throws an exception if a syntax error occurs in the
     * parsing of the file (i.e., it cannot be classified as either a
     * script or a function definition).
     */
    virtual void updateCode();
  };

  /**
   * Routine to freeze an M-function to a serializing
   * stream.
   */
  void FreezeMFunction(MFunctionDef *fptr, Serialize *s);

  /**
   * Routine to thaw an M-function from a stream.
   */
  MFunctionDef* ThawMFunction(Serialize *s);

  class BuiltInFunctionDef : public FunctionDef {
  public:
    /** The number of return args for this function (-1 for variable).
     */
    int retCount;
    /** The number of input args for this function (-1 for variable).
     */
    int argCount;
    /**
     * The pointer to (address of) the function.
     */
    BuiltInFuncPtr fptr;
    /**
     * Default constructor.
     */
    BuiltInFunctionDef();
    /**
     * Default destructor.
     */
    ~BuiltInFunctionDef();
    /**
     * The type of the function is FM_BUILT_IN_FUNCTION.
     */
    virtual const FunctionType type() {return FM_BUILT_IN_FUNCTION;}
    /** Print a description of the function
     */
    virtual void printMe(Interface *io);
    /**
     * The number of inputs required by this function.
     */
    virtual int inputArgCount();
    /**
     * The number of outputs returned by this function.
     */
    virtual int outputArgCount();
    /** 
     * Evaluate the function and return the values.
     */
    virtual ArrayVector evaluateFunction(WalkTree *, ArrayVector &, int);
  };

  typedef FunctionDef* FuncPtr;

  class SpecialFunctionDef : public FunctionDef {
  public:
    /**
     * The number of return args for this function (-1 for variable).
     */
    int retCount;
    /** The number of input args for this function (-1 for variable).
     */
    int argCount;
    /**
     * The pointer to (address of) the function.
     */    
    SpecialFuncPtr fptr;
    /**
     * Default constructor.
     */
    SpecialFunctionDef();
    /**
     * Default destructor.
     */
    ~SpecialFunctionDef();
    /**
     * The type of the function is FM_SPECIAL_FUNCTION.
     */
    virtual const FunctionType type() {return FM_SPECIAL_FUNCTION;}
    /** Print a description of the function
     */
    virtual void printMe(Interface *);
    /**
     * The number of inputs required by this function.
     */
    virtual int inputArgCount() {return argCount;}
    /**
     * The number of outputs returned by this function.
     */
    virtual int outputArgCount() {return retCount;}
    /** 
     * Evaluate the function and return the values.
     */
    virtual ArrayVector evaluateFunction(WalkTree *, ArrayVector& , int);
  };

  typedef void (*GenericFuncPointer)();
  
  class ImportedFunctionDef : public FunctionDef {
  public:
    /**
     * The number of return args for this function (either 0 or 1).
     */
    int retCount;
    /**
     * The number of input args for this function (cannot be variable).
     */
    int argCount;
    /**
     * The pointer to the function to be called.
     */
    GenericFuncPointer address;
    /**
     * The call interface object - changed to a void* to 
     * decouple the ffi lib from libFreeMat.  Should be
     * a better way to do this...
     */
    void *cif;
    /**
     * The types of each argument
     */
    stringVector types;
    /**
     * The guard expressions associated with each argument
     */
    ASTPtrVector sizeCheckExpressions;
    /**
     * The return type of the function
     */
    std::string retType;
    /**
     * Default constructor
     */
    ImportedFunctionDef(GenericFuncPointer address_arg,
			stringVector types_arg,
			stringVector arguments_arg,
			ASTPtrVector sizeChecks,
			std::string retType_arg);
    /**
     * Default destructor
     */
    ~ImportedFunctionDef();
    /**
     * The type of the function is FM_IMPORTED_FUNCTION.
     */
    virtual const FunctionType type() {return FM_IMPORTED_FUNCTION;}
    /** Print a description of the function
     */
    virtual void printMe(Interface *);
    /**
     * The number of inputs required by this function.
     */
    virtual int inputArgCount() {return argCount;}
    /**
     * The number of outputs returned by this function.
     */
    virtual int outputArgCount() {return retCount;}
    /** 
     * Evaluate the function and return the values.
     */
    virtual ArrayVector evaluateFunction(WalkTree *, ArrayVector& , int);    
  };

}
#endif
