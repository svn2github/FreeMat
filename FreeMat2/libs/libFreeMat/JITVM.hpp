#ifndef __JITVM_hpp__
#define __JITVM_hpp__


#ifdef HAVE_LLVM

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

class JITVM;

class JITSymbolInfo {
  bool is_argument;
  int argument_index;
  bool is_scalar;
  bool is_readonly;
  llvm::Value *num_rows;
  llvm::Value *num_cols;
  llvm::Value *num_length;
  llvm::Value *data_value;
  Class inferred_type;
  llvm::Type *data_type;
  bool  type_mutable;
  // Complete constructor
  JITSymbolInfo(bool arg, int arg_index, bool scalar, bool readonly, 
		llvm::Value* rows, llvm::Value* cols, 
		llvm::Value* length, llvm::Value* value,
		Class i_type, bool t_mutable, llvm::Type* type) :
    is_argument(arg), argument_index(arg_index), is_scalar(scalar), 
    is_readonly(readonly), num_rows(rows), num_cols(cols), num_length(length),
    data_value(value), inferred_type(i_type), type_mutable(t_mutable), 
    data_type(type) {}
  friend class JITVM;
};

class JITVM {
  SymbolTable<JITSymbolInfo> symbols;
  int argument_count;
  vector<Array*> array_inputs;
  llvm::Value *ptr_inputs, *resize_func_ptr, *this_ptr;
  void **args;
  llvm::Function *func;
  llvm::BasicBlock *ip, *func_prolog, *func_body, *func_epilog;
  llvm::Module *M;
  llvm::FunctionType *ResizeFuncTy;
  JITScalar return_val;
  JITSymbolInfo* add_argument_array(string name, Interpreter* m_eval);
  JITSymbolInfo* add_argument_scalar(string name, Interpreter* m_eval, JITScalar val = NULL, bool override = false);
  JITScalar cast(JITScalar value, const llvm::Type *type, bool sgnd, 
		 llvm::BasicBlock* wh, string name="");
  JITScalar get_input_argument(int arg, llvm::BasicBlock* where);
  const llvm::Type* map_dataclass_type(Class aclass);
public:
  static JITScalar int32_const(int32 x);
  static JITScalar bool_const(int32 x);
  static void copy_value(JITScalar source, JITScalar dest, llvm::BasicBlock* where);
  JITScalar compile_binary_op(llvm::Instruction::BinaryOps, JITScalar arg1, 
			      JITScalar arg2, string inst);
  JITScalar compile_boolean_op(llvm::Instruction::BinaryOps, JITScalar arg1, 
			       JITScalar arg2, string inst);
  JITScalar compile_comparison_op(byte op, JITScalar arg1, JITScalar arg2, string inst);
  JITScalar compile_expression(tree t, Interpreter* m_eval);
  JITScalar compile_rhs(tree t, Interpreter* m_eval);
  void compile_if_statement(tree t, Interpreter* m_eval);
  void compile_for_block(tree t, Interpreter *m_eval);
  void compile_assignment(tree t, Interpreter *m_eval);
  void compile_statement(tree t, Interpreter *m_eval);
  void compile_statement_type(tree t, Interpreter *m_eval);
  void compile_block(tree t, Interpreter *m_eval);
  void compile(tree t, Interpreter *m_eval);
  void run(Interpreter *m_eval);
  static void v_resize(void* this_ptr, int argnum, int new_rows);
};

#endif

#endif
