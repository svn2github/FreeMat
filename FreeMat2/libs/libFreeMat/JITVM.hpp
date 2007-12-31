#ifndef __JITVM_hpp__
#define __JITVM_hpp__


#ifdef HAVE_LLVM

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/ModuleProvider.h"
#include "llvm/CallingConv.h"
#include "llvm/Value.h"
#include "llvm/Function.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/GenericValue.h"

#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include <string>

using namespace std;

typedef llvm::Value* JITScalar;
typedef const llvm::Type* JITType;
typedef const llvm::FunctionType* JITFunctionType;
typedef llvm::Function* JITFunction;
typedef llvm::BasicBlock* JITBlock;
typedef llvm::ExecutionEngine* JITEngine;
typedef llvm::Module* JITModule;
typedef llvm::ModuleProvider* JITModuleProvider;
typedef llvm::GenericValue JITGeneric;

// A wrapper interface - this is a way to abstract out the details of the
// LLVM interface.
class JIT {
private:
  JITBlock ip;
  JITFunction func;
  JITEngine ee;
  JITModule m;
  JITModuleProvider mp;
public:
  JIT();
  void            OptimizeCode();
  JITFunctionType FunctionType(JITType rettype, std::vector<JITType> args);
  JITFunctionType FunctionType(std::string rettype, std::string args);
  void            LinkFunction(JITFunction func, void* address);
  JITType         DoubleType();
  JITType         FloatType();
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
  // Type codes are "v - void, i - int32, f - float, d - double, p - pointer"
  JITFunction     DefineLinkFunction(std::string name, std::string rettype, std::string args, void* address);
  JITScalar       FunctionArgument(int n, std::string name);
  void            CloseFunction();
  void            Return(JITScalar t);
  void            Return();
  void            Dump();
  JITGeneric      Invoke(JITFunction f, JITGeneric arg);
};

class JITVM;

class JITSymbolInfo {
  // The location of this symbol in the argument list
  int argument_index;
  // Is the variable a known scalar
  bool is_scalar;
  // The class of the scalar if inferred
  Class inferred_type;
  // The JIT type of the array or scalar
  JITType jit_type;
  // Complete constructor
  JITSymbolInfo(int arg_index, bool scalar, Class i_type, JITType j_type) :
    argument_index(arg_index), is_scalar(scalar), inferred_type(i_type), jit_type(j_type) {}
  friend class JITVM;
};

class JITFunc {
  JITType retType;
  JITType argType;
  JITFunctionType funcType;
  JITFunction funcAddress;
  JITFunc(string fun, JITType ret, JITType arg, JITModule mod);
  friend class JITVM;
};

class JITVM {
  SymbolTable<JITSymbolInfo> symbols;
  SymbolTable<JITFunc> JITDoubleFuncs;
  SymbolTable<JITFunc> JITFloatFuncs;
  SymbolTable<JITFunc> JITIntFuncs;
  SymbolTable<JITScalar> JITScalars;
  int argument_count;
  vector<Array*> array_inputs;
  llvm::Value *ptr_inputs, *v_resize_func_ptr, *m_resize_func_ptr, *this_ptr;
  void **args;
  llvm::Function *func;
  llvm::BasicBlock *ip, *func_prolog, *func_body, *func_epilog;
  llvm::Module *M;
  llvm::ExistingModuleProvider *MP;
  llvm::ExecutionEngine *EE;
  llvm::FunctionType *vResizeFuncTy, *mResizeFuncTy;
  llvm::Function *vResizeFunc, *mResizeFunc;
  JITFunction func_scalar_load_double, func_scalar_load_float, func_scalar_load_int32;
  JITFunction func_vector_load_double, func_vector_load_float, func_vector_load_int32;
  JITFunction func_matrix_load_double, func_matrix_load_float, func_matrix_load_int32;
  JITFunction func_scalar_store_double, func_scalar_store_float, func_scalar_store_int32;
  JITFunction func_vector_store_double, func_vector_store_float, func_vector_store_int32;
  JITFunction func_matrix_store_double, func_matrix_store_float, func_matrix_store_int32;
  JIT *jit;
  JITScalar return_val;
  JITSymbolInfo* add_argument_array(string name, Interpreter* m_eval);
  JITSymbolInfo* add_argument_scalar(string name, Interpreter* m_eval, JITScalar val = NULL, bool override = false);
  const llvm::Type* map_dataclass_type(Class aclass);
public:
  ~JITVM() {delete EE;}
  JITScalar compile_expression(Tree* t, Interpreter* m_eval);
  JITScalar compile_rhs(Tree* t, Interpreter* m_eval);
  JITScalar compile_function_call(Tree* t, Interpreter* m_eval);
  JITScalar compile_scalar_function(string symname, Interpreter* m_eval);
  void compile_if_statement(Tree* t, Interpreter* m_eval);
  void compile_for_block(Tree* t, Interpreter *m_eval);
  void compile_assignment(Tree* t, Interpreter *m_eval);
  void compile_statement(Tree* t, Interpreter *m_eval);
  void compile_statement_type(Tree* t, Interpreter *m_eval);
  void compile_block(Tree* t, Interpreter *m_eval);
  void compile(Tree* t, Interpreter *m_eval);
  void run(Interpreter *m_eval);
  static double scalar_load_double(void* this_ptr, int32 argnum);
  static float scalar_load_float(void* this_ptr, int32 argnum);
  static int32 scalar_load_int32(void* this_ptr, int32 argnum);
  static double vector_load_double(void* this_ptr, int32 argnum, int32 ndx);
  static float vector_load_float(void* this_ptr, int32 argnum, int32 ndx);
  static int32 vector_load_int32(void* this_ptr, int32 argnum, int32 ndx);
  static double matrix_load_double(void* this_ptr, int32 argnum, int32 row, int32 col);
  static float matrix_load_float(void* this_ptr, int32 argnum, int32 row, int32 col);
  static int32 matrix_load_int32(void* this_ptr, int32 argnum, int32 row, int32 col);
  static void scalar_store_double(void* this_ptr, int32 argnum, double rhs);
  static void scalar_store_float(void* this_ptr, int32 argnum, float rhs);
  static void scalar_store_int32(void* this_ptr, int32 argnum, int32 rhs);
  static void vector_store_double(void* this_ptr, int32 argnum, int32 ndx, double rhs);
  static void vector_store_float(void* this_ptr, int32 argnum, int32 ndx, float rhs);
  static void vector_store_int32(void* this_ptr, int32 argnum, int32 ndx, int32 rhs);
  static void matrix_store_double(void* this_ptr, int32 argnum, int32 row, int32 col, double rhs);
  static void matrix_store_float(void* this_ptr, int32 argnum, int32 row, int32 col, float rhs);
  static void matrix_store_int32(void* this_ptr, int32 argnum, int32 row, int32 col, int32 rhs);
};

#else

class JITVM {
  // Empty class declaration
};

#endif

#endif
