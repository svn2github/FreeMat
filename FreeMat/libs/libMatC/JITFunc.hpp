/*
 * Copyright (c) 2009 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __JITFunc_hpp__
#define __JITFunc_hpp__

#ifdef HAVE_LLVM

#include <QString>
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

public:
  SymbolInfo(const SymbolInfo &other) { this->isScalar = other.isScalar; this->argument_num = other.argument_num; this->address = other.address; this->type = other.type; }
  SymbolInfo &operator=(const SymbolInfo &other) { this->isScalar = other.isScalar; this->argument_num = other.argument_num; this->address = other.address; this->type = other.type; return *this;}
  SymbolInfo() {} //need default constructor for SymbolTable
};

class JITFunc {
public:
  FM::SymbolTable<SymbolInfo> symbols;
  FM::SymbolTable<SymbolInfo> deferred_symbols;
  FM::SymbolTable<JITFunction> double_funcs;
  FM::SymbolTable<JITFunction> float_funcs;
  FM::SymbolTable<JITScalar> constants;
  JIT *jit;
  Interpreter *eval;
  Array** array_inputs;
  void** cache_array_bases;
  double* cache_array_rows;
  double* cache_array_cols;
  int argument_count;
  JITFunction func;
  JITBlock prolog, main_body, epilog;
  JITScalar this_ptr, retcode;
  Exception exception_store;
  QString symbol_prefix;
  int uid;
  void register_std_function(QString name);
  SymbolInfo* add_argument_array(QString name, bool createIfMissing);
  SymbolInfo* add_argument_scalar(QString name, JITScalar val = NULL, bool override = false);
  JITType map_dataclass(DataClass aclass);
  DataClass map_dataclass(JITType type);
  DataClass map_dataclass(JITScalar val);
  void handle_success_code(JITScalar success);
  void initialize();
  SymbolInfo* define_local_symbol(QString name, JITScalar val);
  JITFunc(Interpreter *eval);
  JITScalar compile_expression(const Tree & t);
  JITScalar compile_rhs(const Tree & t);
  JITScalar compile_function_call(const Tree & t);
  JITScalar compile_m_function_call(const Tree & t);
  JITScalar compile_built_in_function_call(const Tree & t);
  JITScalar compile_constant_function(QString symname);
  JITScalar compile_or_statement(const Tree & t);
  JITScalar compile_and_statement(const Tree & t);
  void compile_if_statement(const Tree & t);
  void compile_for_block(const Tree & t);
  void compile_assignment(const Tree & t);
  void compile_statement(const Tree & t);
  void compile_statement_type(const Tree & t);
  void compile_block(const Tree & t);
  void compile(const Tree & t);
  void prep();
  void run();
};

#endif


#endif
