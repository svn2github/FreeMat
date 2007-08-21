#ifndef __JITVM_hpp__
#define __JITVM_hpp__

#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include <string>

#ifdef HAVE_LLVM

#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/ModuleProvider.h"
#include "llvm/CallingConv.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/GenericValue.h"

using namespace llvm;
using namespace std;

typedef Value* JITScalar;

class JITVM {
  SymbolTable<JITScalar> symbols;
  Function *func;
  BasicBlock *ip;
public:
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
