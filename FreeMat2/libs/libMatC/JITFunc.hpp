#ifndef __JITFunc_hpp__
#define __JITFunc_hpp__

#include "JIT.hpp"
#include "SymbolTable.hpp"
#include "Context.hpp"
#include "Tree.hpp"

class SymbolInfo {
  bool isScalar;
  int argument_num;
  JITScalar address;
  JITType type;
  SymbolInfo(bool is_scalar, int arg_num, JITScalar base_address, JITType base_type) :
    isScalar(is_scalar), argument_num(arg_num), address(base_address), type(base_type) {}
  friend class JITFunc;
};

class JITFunc {
public:
  SymbolTable<SymbolInfo> symbols;
  SymbolTable<JITFunction> double_funcs, float_funcs, int_funcs;
  SymbolTable<JITScalar> constants;
  JIT *jit;
  Interpreter *eval;
  Array** array_inputs;
  int argument_count;
  JITFunction func;
  JITBlock prolog, main_body, epilog;
  JITScalar this_ptr, retcode;
  Exception exception_store;
  string symbol_prefix;
  int uid;
  void register_std_function(std::string name);
  SymbolInfo* add_argument_array(string name);
  SymbolInfo* add_argument_scalar(string name, JITScalar val = NULL, bool override = false);
  JITType map_dataclass(Class aclass);
  Class map_dataclass(JITType type);
  Class map_dataclass(JITScalar val);
  void handle_success_code(JITScalar success);
  void initialize();
  SymbolInfo* define_local_symbol(string name, JITScalar val);
  JITFunc(Interpreter *eval);
  JITScalar compile_expression(Tree* t);
  JITScalar compile_rhs(Tree* t);
  JITScalar compile_function_call(Tree* t);
  JITScalar compile_m_function_call(Tree* t);
  JITScalar compile_built_in_function_call(Tree* t);
  JITScalar compile_scalar_function(string symname);
  void compile_if_statement(Tree* t);
  void compile_for_block(Tree* t);
  void compile_assignment(Tree* t);
  void compile_statement(Tree* t);
  void compile_statement_type(Tree* t);
  void compile_block(Tree* t);
  void compile(Tree* t);
  void prep();
  void run();
};


#endif
