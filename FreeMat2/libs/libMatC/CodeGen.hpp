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
  SymbolTable<SymbolInfo> symbols;
  JIT *jit;
  Interpreter *eval;
  vector<Array*> array_inputs;
  int argument_count;
  JITFunction func;
  JITBlock prolog, main_body, epilog;
  JITScalar this_ptr;
  SymbolInfo* add_argument_array(string name);
  SymbolInfo* add_argument_scalar(string name, JITScalar val = NULL, bool override = false);
  JITType map_dataclass(Class aclass);
  Class map_dataclass(JITType type);
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
  void initialize();
public:
  CodeGen(Interpreter *eval);
  JITScalar compile_expression(Tree* t);
  JITScalar compile_rhs(Tree* t);
  JITScalar compile_function_call(Tree* t);
  //  JITScalar compile_scalar_function(string symname);
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
