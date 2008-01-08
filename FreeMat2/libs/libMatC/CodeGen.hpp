#ifndef __CodeGen_hpp__
#define __CodeGen_hpp__

#include "JIT.hpp"
#include "SymbolTable.hpp"
#include "Tree.hpp"

class SymbolInfo {
  bool isScalar;
  int argument_num;
  JITScalar address;
  JITType type;
  SymbolInfo(bool is_scalar, int arg_num, JITScalar base_address, JITType base_type) :
    isScalar(is_scalar), argument_num(arg_num), address(base_address), type(base_type) {}
  friend class CodeGen;
};

class CodeGen {
public:
  SymbolTable<SymbolInfo> symbols;
  JIT *jit;
  Interpreter *eval;
  vector<Array*> array_inputs;
  int argument_count;
  JITFunction func;
  JITBlock prolog, main_body, epilog;
  JITScalar this_ptr, retcode;
  Exception exception_store;
  SymbolInfo* add_argument_array(string name);
  SymbolInfo* add_argument_scalar(string name, JITScalar val = NULL, bool override = false);
  JITType map_dataclass(Class aclass);
  Class map_dataclass(JITType type);
  void handle_success_code(JITScalar success);
  void initialize();
  CodeGen(Interpreter *eval);
  JITScalar compile_expression(Tree* t);
  JITScalar compile_rhs(Tree* t);
  JITScalar compile_function_call(Tree* t);
  JITScalar compile_scalar_function(string symname);
  void compile_if_statement(Tree* t);
  void compile_for_block(Tree* t);
  void compile_assignment(Tree* t);
  void compile_statement(Tree* t);
  void compile_statement_type(Tree* t);
  void compile_block(Tree* t);
  void compile(Tree* t);
  void run();
};


#endif
