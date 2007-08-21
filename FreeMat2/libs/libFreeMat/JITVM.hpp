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
  int num_rows;
  int num_cols;
  llvm::Value *data_value;
  // Complete constructor
  JITSymbolInfo(bool arg, int arg_index, bool scalar, bool readonly, int rows, int cols, 
		llvm::Value* value) :
    is_argument(arg), argument_index(arg_index), is_scalar(scalar), is_readonly(readonly), 
    num_rows(rows), num_cols(cols), data_value(value) {}
  // Local variable constructor
  JITSymbolInfo(llvm::Value* value) :
    is_argument(false), argument_index(-1), is_scalar(true), is_readonly(false), 
    num_rows(1), num_cols(1), data_value(value) {}
  friend class JITVM;
};

class JITVM {
  SymbolTable<JITSymbolInfo> symbols;
  int argument_count;
  llvm::Value *ptr_inputs;
  llvm::Function *func;
  llvm::BasicBlock *ip, *func_prolog, *func_body, *func_epilog;
  JITSymbolInfo* add_argument(string name, Interpreter* m_eval, bool scalar);
  JITScalar cast(JITScalar value, const llvm::Type *type, bool sgnd, 
		 llvm::BasicBlock* wh, string name="");
public:
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
};

#endif

#endif
