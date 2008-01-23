#ifndef __JIT_hpp__
#define __JIT_hpp__

#ifdef HAVE_LLVM

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include "Types.hpp"

#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/ModuleProvider.h"
#include "llvm/CallingConv.h"
#include "llvm/Value.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/GenericValue.h"

#include <string>

typedef llvm::Value* JITScalar;
typedef const llvm::Type* JITType;
typedef const llvm::FunctionType* JITFunctionType;
typedef llvm::Function* JITFunction;
typedef llvm::BasicBlock* JITBlock;
typedef llvm::ExecutionEngine* JITEngine;
typedef llvm::Module* JITModule;
typedef llvm::ModuleProvider* JITModuleProvider;
typedef llvm::GenericValue JITGeneric;
typedef llvm::FunctionPassManager* JITOptimizer;

// A wrapper interface - this is a way to abstract out the details of the
// LLVM interface.
class JIT {
private:
  JITBlock ip;
  JITFunction func;
  JITEngine ee;
  JITModule m;
  JITModuleProvider mp;
  JITOptimizer opt;
  bool initialized;
public:
  JIT();
  ~JIT();
  void            OptimizeCode(JITFunction func);
  JITFunctionType FunctionType(JITType rettype, std::vector<JITType> args);
  JITFunctionType FunctionType(std::string rettype, std::string args);
  bool            Initialized();
  void            SetInitialized(bool t);
  JITType         DoubleType();
  JITType         FloatType();
  JITType         Int8Type();
  JITType         Int32Type();
  JITType         BoolType();
  JITType         VoidType();
  JITType         MapTypeCode(char c);
  bool            IsDouble(JITType t);
  bool            IsFloat(JITType t);
  bool            IsInteger(JITType t);
  bool            IsFP(JITType t);
  bool            IsDouble(JITScalar t);
  bool            IsFloat(JITScalar t);
  bool            IsInteger(JITScalar t);
  bool            IsFP(JITScalar t);
  JITType         PointerType(JITType t);
  JITType         TypeOf(JITScalar x);
  JITScalar       Int32Value(int32 x);
  JITScalar       DoubleValue(double x);
  JITScalar       FloatValue(float x);
  JITScalar       BoolValue(bool t);
  JITScalar       Zero(JITType t);
  JITBlock        NewBlock(std::string name);
  JITScalar       And(JITScalar A, JITScalar B);
  JITScalar       Or(JITScalar A, JITScalar B);
  JITScalar       Xor(JITScalar A, JITScalar B);
  JITScalar       Mul(JITScalar A, JITScalar B);
  JITScalar       Div(JITScalar A, JITScalar B);
  JITScalar       Sub(JITScalar A, JITScalar B);
  JITScalar       Add(JITScalar A, JITScalar B);
  JITScalar       Alloc(JITType T, std::string name);
  JITScalar       Cast(JITScalar A, JITType T);
  JITScalar       LessThan(JITScalar A, JITScalar B);
  JITScalar       LessEquals(JITScalar A, JITScalar B);
  JITScalar       Equals(JITScalar A, JITScalar B);
  JITScalar       GreaterEquals(JITScalar A, JITScalar B);
  JITScalar       GreaterThan(JITScalar A, JITScalar B);
  JITScalar       NotEqual(JITScalar A, JITScalar B);
  void            Store(JITScalar Value, JITScalar Address);
  JITScalar       Load(JITScalar Address);
  JITScalar       String(string text);
  void            Jump(JITBlock B);
  void            Branch(JITBlock IfTrue, JITBlock IfFalse, JITScalar TestValue);
  void            SetCurrentBlock(JITBlock B);
  JITBlock        CurrentBlock();
  JITScalar       Call(JITFunction, std::vector<JITScalar> args);
  JITScalar       Call(JITFunction, JITScalar arg1);
  JITScalar       Call(JITFunction, JITScalar arg1, JITScalar arg2);
  JITScalar       Call(JITFunction, JITScalar arg1, JITScalar arg2, 
		       JITScalar arg3);
  JITScalar       Call(JITFunction, JITScalar arg1, JITScalar arg2,
		       JITScalar arg3, JITScalar arg4);
  JITScalar       Call(JITFunction, JITScalar arg1, JITScalar arg2, 
		       JITScalar arg3, JITScalar arg4, JITScalar arg5);
  JITScalar       GetElement(JITScalar BaseAddress, JITScalar Offset);
  JITFunction     DefineFunction(JITFunctionType functype, std::string name);
  void            SetCurrentFunction(JITFunction A);
  JITFunction     CurrentFunction();
  // Shortcut to define a non-JIT function
  // Type codes are "v - void, i - int32, f - float, d - double,
  //                           I - int32 pointer, F - float pointer, D - double pointer
  JITFunction     DefineLinkFunction(std::string name, std::string rettype, std::string args);
  JITScalar       FunctionArgument(int n, std::string name);
  void            CloseFunction();
  void            Return(JITScalar t);
  void            Return();
  void            Dump();
  void            Dump(JITFunction f);
  void		  Dump( const std::string& fname );
  void		  Dump( const std::string& fname, JITFunction f );
  JITGeneric      Invoke(JITFunction f, JITGeneric arg);
};

#else

class JIT {
};

#endif

#endif
