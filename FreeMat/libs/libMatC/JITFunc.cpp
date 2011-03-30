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
// New plans for the JIT:
// 
// Type complexity has been greatly reduced in FreeMat4.  Consider the classic example
// of the Toeplitz matrix creation loop in FreeMat4:
//
//  A = zeros(512);
//  for i=1:512;
//    for j=1:512;
//      A(j,i) = j-i+1;
//    end
//  end
//
// In FreeMat3, this loop would have required the handling of at least 3 seperate types
// (int32, float, double), and would not even have JIT compiled due to the polymorphic
// assignments to the A matrix.
//
// In FreeMat4, the loop is much simpler.  Everything is a double type.  If we start
// with a JIT that only processes double types, I think we will cover a significant
// portion of MATLAB-style code.  We also avoid the issues around saturation arithmetic
// that plague the handling of integer types.  
//
// Do we handle float types too?
//
// Consider the problem of extending the JIT to handle general expressions.  For example:
//   for i=1:512
//     for j=1:512
//       b = [i;j];
//       c = T*b;
//       A(i,j) = A(i,j) + proj(c);
//     end
//   end
// In this case, we have to handle a bunch of operations.
//
// Generally speaking, we now have the situation in which an expression, when evaluated can be an 
// array as well as (or instead of) a scalar.  However, the actual array is not available when
// the JIT is compiled.  Only the type and the fact that it is an array is available.  What is also
// apparent is that the array has to be stored somewhere.  The JIT will then have to create a symbol
// table.  And what happens to intermediate values then?  In the limit, the JIT must address all of 
// the same conditions as the interpreter.  
//
// In the example above, 
//

#ifdef HAVE_LLVM

#include <QString>
#include "JITFunc.hpp"
#include "Context.hpp"
#include "Interpreter.hpp"
#include "Array.hpp"
#include <sstream>
#include <math.h>

#if defined(_MSC_VER )
    #define JIT_EXPORT __declspec( dllexport )
#else
    #define JIT_EXPORT
#endif

JITFunc *save_this;


static JITFunction func_scalar_load_double, func_scalar_load_float, func_scalar_load_bool;
static JITFunction func_scalar_store_double, func_scalar_store_float, func_scalar_store_bool;
static JITFunction func_vector_load_double, func_vector_load_float, func_vector_load_bool;
static JITFunction func_vector_store_double, func_vector_store_float, func_vector_store_bool;
static JITFunction func_matrix_load_double, func_matrix_load_float, func_matrix_load_bool;
static JITFunction func_matrix_store_double, func_matrix_store_float, func_matrix_store_bool;
static JITFunction func_check_for_interrupt;
static JITFunction func_niter_for_loop, func_debug_out_d;

SymbolInfo* JITFunc::add_argument_array(QString name, bool createIfMissing=false) {
  if (symbol_prefix.size() > 0)
    return NULL;
  ArrayReference ptr(eval->getContext()->lookupVariable(name));
  DataClass aclass = Invalid;

  if (!ptr.valid()){
      if( createIfMissing ){
	JITType type(map_dataclass(Double));
	symbols.insertSymbol(name,SymbolInfo(false,argument_count++,NULL,type));
	return symbols.findSymbol(name);
      }
      else{
	  return NULL;
      }
  }

  if (!ptr->is2D())
    throw Exception("Cannot JIT multi-dimensional array:" + name);
  if (ptr->isString() || ptr->isReferenceType())
    throw Exception("Cannot JIT strings or reference types:" + name);
  if (ptr->isComplex())
    throw Exception("Cannot JIT complex arrays:" + name);
  aclass = ptr->dataClass();
  // Map the array class to an llvm type
  JITType type(map_dataclass(aclass));
  symbols.insertSymbol(name,SymbolInfo(false,argument_count++,NULL,type));
  return symbols.findSymbol(name);
}

DataClass JITFunc::map_dataclass(JITScalar val) {
  if (jit->IsDouble(val))
    return Double;
  else if (jit->IsFloat(val))
    return Float;
  else if (jit->IsBool(val))
    return Bool;
  throw Exception("Unhandled type in map_dataclass for JIT");
}

DataClass JITFunc::map_dataclass(JITType type) {
  if (jit->IsDouble(type))
    return Double;
  else if (jit->IsFloat(type))
    return Float;
  else if (jit->IsBool(type))
    return Bool;
  throw Exception("Unhandled type in map_dataclass for JIT");  
}

JITType JITFunc::map_dataclass(DataClass aclass) {
  switch (aclass) {
  default:
    throw Exception("JIT does not support");
  case Bool:
    return jit->BoolType();
  case Float:
    return jit->FloatType();
  case Double:
    return jit->DoubleType();
  }
  return NULL;
}

SymbolInfo* JITFunc::define_local_symbol(QString name, JITScalar val) {
  if (!val) throw Exception("undefined variable or argument " + name);
  JITBlock ip(jit->CurrentBlock());
  jit->SetCurrentBlock(prolog);
  JITScalar address = jit->Alloc(jit->TypeOf(val),name);
  symbols.insertSymbol(name,SymbolInfo(true,-1,address,jit->TypeOf(val)));
  jit->SetCurrentBlock(ip);
  jit->Store(val,address);
  return symbols.findSymbol(name);
}

// FIXME - Simplify
SymbolInfo* JITFunc::add_argument_scalar(QString name, JITScalar val, bool override) {
  DataClass aclass;
  if (symbol_prefix.size() > 0) 
    return define_local_symbol(name,val);
  ArrayReference ptr(eval->getContext()->lookupVariable(name));
  aclass = Invalid;
  if (!val && !ptr.valid()) return NULL;
  if (!ptr.valid() || override) {
    aclass = map_dataclass(val);
  } else {
    if (!ptr->isScalar())
      throw Exception("Expect " + name + " to be a scalar");
    if (ptr->isString() || ptr->isReferenceType())
      throw Exception("Cannot JIT strings or reference types:" + name);
    if (ptr->isComplex())
      throw Exception("Cannot JIT complex arrays:" + name);
    aclass = ptr->dataClass();
  }
  JITType type(map_dataclass(aclass));
  JITBlock ip(jit->CurrentBlock());
  jit->SetCurrentBlock(prolog);
  JITScalar address = jit->Alloc(type,name);
  symbols.insertSymbol(name,SymbolInfo(true,argument_count++,address,type));
  if (jit->IsDouble(type))
    jit->Store(jit->Call(func_scalar_load_double, this_ptr, jit->DoubleValue(argument_count-1)), address);
  else if (jit->IsFloat(type))
    jit->Store(jit->Call(func_scalar_load_float, this_ptr, jit->DoubleValue(argument_count-1)), address);
  else if (jit->IsBool(type))
    jit->Store(jit->Call(func_scalar_load_bool, this_ptr, jit->DoubleValue(argument_count-1)), address);
  jit->SetCurrentBlock(epilog);
  if (jit->IsDouble(type))
    jit->Call(func_scalar_store_double, this_ptr, jit->DoubleValue(argument_count-1), jit->Load(address));
  else if (jit->IsFloat(type))
    jit->Call(func_scalar_store_float, this_ptr, jit->DoubleValue(argument_count-1), jit->Load(address));
  else if (jit->IsBool(type))
    jit->Call(func_scalar_store_bool, this_ptr, jit->DoubleValue(argument_count-1), jit->Load(address));
  jit->SetCurrentBlock(ip);
  return symbols.findSymbol(name);
}

JITFunc::JITFunc(Interpreter *p_eval) {
  jit = JIT::Instance();
  eval = p_eval;
}

void JITFunc::compile_block(const Tree & t) {
  const TreeList &statements(t.children());
  for (TreeList::const_iterator i=statements.begin();i!=statements.end();i++) 
    compile_statement(*i);
}

void JITFunc::compile_statement_type(const Tree & t) {
  switch(t.token()) {
  case '=': 
    compile_assignment(t);
    break;
  case TOK_MULTI:
    throw Exception("multi function calls do not JIT compile");
  case TOK_SPECIAL:
    throw Exception("special function calls do not JIT compile");
  case TOK_FOR:
    compile_for_block(t);
    break;
  case TOK_WHILE:
    throw Exception("nested while loops do not JIT compile");
  case TOK_IF:
    compile_if_statement(t);
    break;
  case TOK_BREAK:       throw Exception("break is not currently handled by the JIT compiler");
  case TOK_CONTINUE:    throw Exception("continue is not currently handled by the JIT compiler");
  case TOK_DBSTEP:      throw Exception("dbstep is not currently handled by the JIT compiler");
  case TOK_DBTRACE:     throw Exception("dbtrace is not currently handled by the JIT compiler");
  case TOK_RETURN:      throw Exception("return is not currently handled by the JIT compiler");
  case TOK_SWITCH:      throw Exception("switch is not currently handled by the JIT compiler");
  case TOK_TRY:         throw Exception("try is not currently handled by the JIT compiler");
  case TOK_QUIT:        throw Exception("quit is not currently handled by the JIT compiler");
  case TOK_RETALL:      throw Exception("retall is not currently handled by the JIT compiler");
  case TOK_KEYBOARD:    throw Exception("keyboard is not currently handled by the JIT compiler");
  case TOK_GLOBAL:      throw Exception("global is not currently handled by the JIT compiler");
  case TOK_PERSISTENT:  throw Exception("persistent is not currently handled by the JIT compiler");
  case TOK_EXPR:
    compile_expression(t.first());
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void JITFunc::compile_statement(const Tree & t) {
  if (t.is(TOK_STATEMENT) && 
      (t.first().is(TOK_EXPR) || t.first().is(TOK_SPECIAL) ||
       t.first().is(TOK_MULTI) || t.first().is('=')))
    throw Exception("JIT compiler doesn't work with verbose statements");
  compile_statement_type(t.first());
}

JITScalar JITFunc::compile_constant_function(QString symname) {
  JITScalar *val;
  val = constants.findSymbol(symname);
  if (!val) throw Exception("constant not defined");
  return (*val);
}

JITScalar JITFunc::compile_built_in_function_call(const Tree & t) {
  // First, make sure it is a function
  QString symname(t.first().text());
  FuncPtr funcval;
  if (!eval->lookupFunction(symname,funcval)) 
    throw Exception(QString("Couldn't find function ") + symname);
  if (t.numChildren() != 2) 
    return compile_constant_function(symname);
  // Evaluate the argument
  const Tree & s(t.second());
  if (!s.is(TOK_PARENS))
    throw Exception("Expecting function arguments.");
  if (s.numChildren() > 1)
    throw Exception("Cannot JIT functions that take more than one argument");
  if (s.numChildren() == 0) 
    return compile_constant_function(symname);
  else {
    JITScalar arg = compile_expression(s.first());
    JITFunction *func = NULL;
    if (jit->IsFloat(arg)) {
      func = float_funcs.findSymbol(symname);
      if (!func) throw Exception("Cannot find function " + symname);
    } else if (jit->IsDouble(arg)) {
      func = double_funcs.findSymbol(symname);
      if (!func) throw Exception("Cannot find function " + symname);
    } 
    if (!func) throw Exception("No JIT version of function " + symname);
    return jit->Call(*func,arg);
  }
}

static QString uid_string(int uid) {
  return QString("%1").arg(uid);
}

JITScalar JITFunc::compile_m_function_call(const Tree & t) {
  // First, make sure it is a function
  QString symname(t.first().text());
  FuncPtr funcval;
  if (!eval->lookupFunction(symname,funcval)) 
    throw Exception("Couldn't find function " + symname);
  if (funcval->type() != FM_M_FUNCTION)
    throw Exception("Expected M function");
  MFunctionDef *fptr = (MFunctionDef*) funcval;
  if ((fptr->inputArgCount() < 0) || (fptr->outputArgCount() < 0))
    throw Exception("Variable argument functions not handled");
  if (fptr->nestedFunction /*ei || fptr->capturedFunction*/)
    throw Exception("Nested and/or captured functions not handled");
  if (fptr->scriptFlag) 
    throw Exception("scripts not handled");
  // Set up the prefix
  QString new_symbol_prefix = symbol_prefix + "$" + symname + "_" + uid_string(uid);
  uid++;
  // Loop through the arguments to the function,
  // and map them from the defined arguments of the tree
  if (t.numChildren() < 2) 
    throw Exception("function takes no arguments - not currently supported");
  const Tree & s(t.second());
  int args_defed = fptr->arguments.size();
  if (args_defed > s.numChildren())
    args_defed = s.numChildren();
  for (int i=0;i<args_defed;i++) {
    JITScalar arg = compile_expression(s.child(i));
    define_local_symbol(new_symbol_prefix + fptr->arguments[i],arg);
  }
  define_local_symbol(new_symbol_prefix+"nargout",jit->DoubleValue(1));
  define_local_symbol(new_symbol_prefix+"nargin",jit->DoubleValue(args_defed));
  // compile the code for the function
  //  fptr->code.print();
  QString save_prefix = symbol_prefix;
  symbol_prefix = new_symbol_prefix;
  compile_block(fptr->code);
  // Lookup the result and return it
  SymbolInfo *v = symbols.findSymbol(new_symbol_prefix+fptr->returnVals[0]);
  if (!v) throw Exception("function failed to define return value");
  symbol_prefix = save_prefix;
  return jit->Load(v->address);
}

JITScalar JITFunc::compile_function_call(const Tree & t) {
  // First, make sure it is a function
  QString symname(t.first().text());
  FuncPtr funcval;
  if (!eval->lookupFunction(symname,funcval)) 
    throw Exception("Couldn't find function " + symname);
  funcval->updateCode(eval);
  if (funcval->type() == FM_BUILT_IN_FUNCTION)
    return compile_built_in_function_call(t);
  if (funcval->type() == FM_M_FUNCTION)
    return compile_m_function_call(t);
  if (t.numChildren() != 2) 
    return compile_constant_function(symname);
  throw Exception("Unsupported function type");
}

void JITFunc::handle_success_code(JITScalar success_code) {
  JITBlock if_failed = jit->NewBlock("exported_call_failed");
  JITBlock if_success = jit->NewBlock("exported_call_sucess");
  // Add the branch logic
  jit->Branch(if_success,if_failed,success_code);
  jit->SetCurrentBlock(if_failed);
  jit->Store(success_code,retcode);
  jit->Jump(epilog);
  jit->SetCurrentBlock(if_success);
}

JITScalar JITFunc::compile_rhs(const Tree & t) {
  QString symname(symbol_prefix+t.first().text());
  SymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (t.numChildren() == 1)
      v = add_argument_scalar(symname);
    else
      v = add_argument_array(symname);
    if (!v)
      return compile_function_call(t);
  }
  if (t.numChildren() == 1) {
    if (!v->isScalar)
      throw Exception("non-scalar reference returned in scalar context!");
    return jit->Load(v->address);
  }
  if (t.numChildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar)
    throw Exception("array indexing of scalar values...");
  const Tree & s(t.second());
  if (!s.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s.numChildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s.numChildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s.numChildren() == 1) {
    JITScalar arg1 = jit->ToDouble(compile_expression(s.first()));
    JITScalar ret;
    if (jit->IsDouble(v->type))
      ret = jit->Call(func_vector_load_double, this_ptr, jit->DoubleValue(v->argument_num), arg1, retcode);
    else if (jit->IsFloat(v->type))
      ret = jit->Call(func_vector_load_float, this_ptr, jit->DoubleValue(v->argument_num), arg1, retcode);
    else if (jit->IsBool(v->type))
      ret = jit->Call(func_vector_load_bool, this_ptr, jit->DoubleValue(v->argument_num), arg1, retcode);
    else
      throw Exception("Unsupported JIT type in Load");
    handle_success_code(jit->Load(retcode));
    return ret;
  } else if (s.numChildren() == 2) {
    JITScalar arg1 = jit->ToDouble(compile_expression(s.first()));
    JITScalar arg2 = jit->ToDouble(compile_expression(s.second()));
    JITScalar ret;
    if (jit->IsDouble(v->type))
      ret = jit->Call(func_matrix_load_double, this_ptr, jit->DoubleValue(v->argument_num), arg1, arg2, retcode);
    else if (jit->IsFloat(v->type))
      ret = jit->Call(func_matrix_load_float, this_ptr, jit->DoubleValue(v->argument_num), arg1, arg2, retcode);
    else if (jit->IsBool(v->type))
      ret = jit->Call(func_matrix_load_bool, this_ptr, jit->DoubleValue(v->argument_num), arg1, arg2, retcode);
    else
      throw Exception("Unsupported JIT type in Load");
    handle_success_code(jit->Load(retcode));
    return ret;
  }
  throw Exception("dereference not handled yet...");
}

JITScalar JITFunc::compile_expression(const Tree & t) {
  switch(t.token()) {
  case TOK_VARIABLE:     return compile_rhs(t);
  case TOK_REAL:
  case TOK_REALF:
    if( t.array().isScalar() ){
      switch( t.array().dataClass() ){
      case Bool:
	return jit->BoolValue( t.array().constRealScalar<bool>() );
      case Float:
	return jit->FloatValue( t.array().constRealScalar<float>() );
      case Double:
	return jit->DoubleValue( t.array().constRealScalar<double>() );
      default:
	throw Exception("Unsupported scalar type.");
      }
    }
    else
      throw Exception("Unsupported type.");
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF:      throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case '+':
    return jit->Add(compile_expression(t.first()),compile_expression(t.second()));
  case '-': 
    return jit->Sub(compile_expression(t.first()),compile_expression(t.second()));
  case '*': 
  case TOK_DOTTIMES: 
    return jit->Mul(compile_expression(t.first()),compile_expression(t.second()));
  case '/': 
  case TOK_DOTRDIV:
    return jit->Div(compile_expression(t.first()),compile_expression(t.second()));
  case '\\': 
  case TOK_DOTLDIV: 
    return jit->Div(compile_expression(t.second()),compile_expression(t.first()));
  case TOK_SOR: 
    return compile_or_statement(t);
  case '|':
    return jit->Or(compile_expression(t.first()),compile_expression(t.second()));
  case TOK_SAND: 
    return compile_and_statement(t);
  case '&': 
    return jit->And(compile_expression(t.first()),compile_expression(t.second()));
  case '<': 
    return jit->LessThan(compile_expression(t.first()),compile_expression(t.second()));
  case TOK_LE: 
    return jit->LessEquals(compile_expression(t.first()),compile_expression(t.second()));
  case '>': 
    return jit->GreaterThan(compile_expression(t.first()),compile_expression(t.second()));
  case TOK_GE: 
    return jit->GreaterEquals(compile_expression(t.first()),compile_expression(t.second()));
  case TOK_EQ: 
    return jit->Equals(compile_expression(t.first()),compile_expression(t.second()));
  case TOK_NE: 
    return jit->NotEqual(compile_expression(t.first()),compile_expression(t.second()));
  case TOK_UNARY_MINUS: 
    return jit->Negate(compile_expression(t.first()));
  case TOK_UNARY_PLUS: 
    return compile_expression(t.first());
  case '~': 
    return jit->Not(compile_expression(t.first()));
  case '^':               throw Exception("^ is not currently handled by the JIT compiler");
  case TOK_DOTPOWER:      throw Exception(".^ is not currently handled by the JIT compiler");
  case '\'':              throw Exception("' is not currently handled by the JIT compiler");
  case TOK_DOTTRANSPOSE:  throw Exception(".' is not currently handled by the JIT compiler");
  case '@':               throw Exception("@ is not currently handled by the JIT compiler");
  default:                throw Exception("Unrecognized expression!");
  }  
}

void JITFunc::compile_assignment(const Tree & t) {
  const Tree & s(t.first());
  QString symname(symbol_prefix+s.first().text());
  JITScalar rhs(compile_expression(t.second()));
  SymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (s.numChildren() == 1)
      v = add_argument_scalar(symname,rhs,false);
    else
      v = add_argument_array(symname, true /*createIfMissing*/ );
    if (!v) throw Exception("Undefined variable reference:" + symname);
  }
  if (s.numChildren() == 1) {
    if (v->type != jit->TypeOf(rhs))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!v->isScalar)
      throw Exception("scalar assignment to array variable.");
    jit->Store(rhs, v->address);
    return;
  }
  if (s.numChildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar)
    throw Exception("array indexing of scalar values...");
  const Tree & q(s.second());
  if (!q.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q.numChildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q.numChildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q.numChildren() == 1) {
    JITScalar arg1 = jit->ToDouble(compile_expression(q.first()));
    JITScalar success_code;
    if (jit->IsDouble(v->type))
      success_code = jit->Call(func_vector_store_double, this_ptr, jit->DoubleValue(v->argument_num), arg1, 
			       jit->ToType(rhs,v->type));
    else if (jit->IsFloat(v->type))
      success_code = jit->Call(func_vector_store_float, this_ptr, jit->DoubleValue(v->argument_num), arg1, 
			       jit->ToType(rhs,v->type));
    else if (jit->IsBool(v->type))
      success_code = jit->Call(func_vector_store_bool, this_ptr, jit->DoubleValue(v->argument_num), arg1, 
			       jit->ToType(rhs,v->type));
    else
      throw Exception("unhandled type for vector store");
    handle_success_code(success_code);
    return;
  } else if (q.numChildren() == 2) {
    JITScalar arg1 = jit->ToDouble(compile_expression(q.first()));
    JITScalar arg2 = jit->ToDouble(compile_expression(q.second()));
    JITScalar success_code;
    if (jit->IsDouble(v->type))
      success_code = jit->Call(func_matrix_store_double, this_ptr, jit->DoubleValue(v->argument_num), 
			       arg1, arg2, jit->ToType(rhs,v->type));
    else if (jit->IsFloat(v->type))
      success_code = jit->Call(func_matrix_store_float, this_ptr, jit->DoubleValue(v->argument_num), 
			       arg1, arg2, jit->ToType(rhs,v->type));
    else if (jit->IsBool(v->type))
      success_code = jit->Call(func_matrix_store_bool, this_ptr, jit->DoubleValue(v->argument_num), 
			       arg1, arg2, jit->ToType(rhs,v->type));
    else
      throw Exception("unhandled type for matrix store");
    handle_success_code(success_code);
    return;    
  }
}

// x = a || b
//
// if (a)
//   x = true;
// else if (b)
//   x = true;
// else
//   x = false;
// end

JITScalar JITFunc::compile_or_statement(const Tree & t) {
  JITBlock ip(jit->CurrentBlock());
  jit->SetCurrentBlock(prolog);
  JITScalar address = jit->Alloc(jit->BoolType(),"or_result");
  jit->SetCurrentBlock(ip);
  JITBlock or_true = jit->NewBlock("or_true");
  JITBlock or_first_false = jit->NewBlock("or_first_false");
  JITBlock or_false = jit->NewBlock("or_false");
  JITBlock or_exit = jit->NewBlock("or_exit");
  JITScalar first_value(compile_expression(t.first()));
  jit->Branch(or_true,or_first_false,first_value);
  jit->SetCurrentBlock(or_first_false);
  JITScalar second_value(compile_expression(t.second()));
  jit->Branch(or_true,or_false,second_value);
  jit->SetCurrentBlock(or_true);
  jit->Store(jit->BoolValue(true),address);
  jit->Jump(or_exit);
  jit->SetCurrentBlock(or_false);
  jit->Store(jit->BoolValue(false),address);
  jit->Jump(or_exit);
  jit->SetCurrentBlock(or_exit);
  return jit->Load(address);
}

// x = a && b
//
// if (!a)
//   x = false;
// else if (!b)
//   x = false;
// else
//   x = true;
// end

JITScalar JITFunc::compile_and_statement(const Tree & t) {
  JITBlock ip(jit->CurrentBlock());
  jit->SetCurrentBlock(prolog);
  JITScalar address = jit->Alloc(jit->BoolType(),"and_result");
  jit->SetCurrentBlock(ip);
  JITBlock and_true = jit->NewBlock("and_true");
  JITBlock and_first_true = jit->NewBlock("and_first_true");
  JITBlock and_false = jit->NewBlock("and_false");
  JITBlock and_exit = jit->NewBlock("and_exit");
  JITScalar first_value(compile_expression(t.first()));
  jit->Branch(and_first_true,and_false,first_value);
  jit->SetCurrentBlock(and_first_true);
  JITScalar second_value(compile_expression(t.second()));
  jit->Branch(and_true,and_false,second_value);
  jit->SetCurrentBlock(and_true);
  jit->Store(jit->BoolValue(true),address);
  jit->Jump(and_exit);
  jit->SetCurrentBlock(and_false);
  jit->Store(jit->BoolValue(false),address);
  jit->Jump(and_exit);
  jit->SetCurrentBlock(and_exit);
  return jit->Load(address);
}

void JITFunc::compile_if_statement(const Tree & t) {
  JITScalar main_cond(jit->ToBool(compile_expression(t.first())));
  JITBlock if_true = jit->NewBlock("if_true");
  JITBlock if_continue = jit->NewBlock("if_continue");
  JITBlock if_exit = jit->NewBlock("if_exit");
  jit->Branch(if_true,if_continue,main_cond);
  jit->SetCurrentBlock(if_true);
  bool failed = false;
  try {
    compile_block(t.second());
  } catch(Exception &e) {
    exception_store = e;
    failed = true;
  }
  jit->Jump(if_exit);
  int n=2;
  while (n < t.numChildren() && t.child(n).is(TOK_ELSEIF)) {
    jit->SetCurrentBlock(if_continue);
    JITScalar ttest(jit->ToBool(compile_expression(t.child(n).first())));
    if_true = jit->NewBlock("elseif_true");
    if_continue = jit->NewBlock("elseif_continue");
    jit->Branch(if_true,if_continue,ttest);
    jit->SetCurrentBlock(if_true);
    try {
      compile_block(t.child(n).second());
    } catch(Exception &e) {
      exception_store = e;
      failed = true;
    }
    jit->Jump(if_exit);
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    jit->SetCurrentBlock(if_continue);
    try {
      compile_block(t.last().first());
    } catch(Exception &e) {
      exception_store = e;
      failed = true;
    }
    jit->Jump(if_exit);
  } else {
    jit->SetCurrentBlock(if_continue);
    jit->Jump(if_exit);
  }
  jit->SetCurrentBlock(if_exit);
  if (failed) throw exception_store;
}

template<class T> 
inline T scalar_load(void* base, double argnum) {
  JITFunc *tptr = static_cast<JITFunc*>(base);
  Array* a = tptr->array_inputs[(int)(argnum)];
  return a->constRealScalar<T>();
}

template<class T>
inline void scalar_store(void* base, double argnum, T value) {
  JITFunc *tptr = static_cast<JITFunc*>(base);
  Array* a = tptr->array_inputs[(int)(argnum)];
  if( a->isArray() )
    a->set(1,Array(value));
  else
    a->realScalar<T>() = value;
}

template<class T>
inline T vector_load(void* base, double argnum, double ndx, bool *success) {
  int iargnum = argnum;
  try {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    if (ndx < 1) {
      tptr->exception_store = Exception("Array index < 1 not allowed");
      success[0] = false;
      return 0;
    }
    if (ndx > tptr->cache_array_rows[iargnum]*tptr->cache_array_cols[iargnum]) {
      tptr->exception_store = Exception("Array bounds exceeded in A(n) expression");
      success[0] = false;
      return 0;
    }
    success[0] = true;
    return (((T*)(tptr->cache_array_bases[iargnum]))[(int64)(ndx-1)]);
  } catch (Exception &e) {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    tptr->exception_store = e;
    success[0] = false;
    return 0;
  }
  success[0] = false;
  return 0;
}

template<class T>
inline bool vector_store(void* base, double argnum, double ndx, T value) {
  int iargnum = argnum;
  try {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    if (ndx < 1) {
      tptr->exception_store = Exception("Array index < 1 not allowed");
      return false;      
    }
    if (ndx > tptr->cache_array_rows[iargnum]*tptr->cache_array_cols[iargnum]) {
      Array* a = tptr->array_inputs[(int)argnum];
      a->set(ndx, Array( value ));
      tptr->cache_array_bases[iargnum] = (void*)(a->real<T>().data());
      tptr->cache_array_rows[iargnum] = a->rows();
      tptr->cache_array_cols[iargnum] = a->cols();
    } else
      ((T*)(tptr->cache_array_bases[iargnum]))[(int64)(ndx-1)] = value;
    return true;
  } catch (Exception &e) {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    tptr->exception_store = e;
    return false;
  }
  return true;
}

template<class T>
inline T matrix_load(void* base, double argnum, double row, double col, bool* success) {
  int iargnum = argnum;
  try {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    if ((row < 1) || (col < 1)) {
      tptr->exception_store = Exception("Array index < 1 not allowed");
      success[0] = false;
      return 0;      
    }
    if ((row > tptr->cache_array_rows[iargnum]) || 
	(col > tptr->cache_array_cols[iargnum])) {
      tptr->exception_store = Exception("Array index exceed bounds");
      success[0] = false;
      return 0;
    }
    success[0] = true;
    return (((T*)(tptr->cache_array_bases[iargnum]))
	    [(int64)(row-1+(col-1)*tptr->cache_array_rows[iargnum])]);
  } catch (Exception &e) {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    tptr->exception_store = e;
    success[0] = false;
    return 0;
  }
  success[0] = false;
  return 0;
}

// Nominal time 280 ms
// 
template<class T>
inline bool matrix_store(void* base, double argnum, double row, double col, T value) {
  int iargnum = argnum;
  try {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    if ((row < 1) || (col < 1)) {
      tptr->exception_store = Exception("Array index < 1 not allowed");
      return false;      
    }
    if ((row > tptr->cache_array_rows[iargnum]) || 
	(col > tptr->cache_array_cols[iargnum])) {
      Array *a = tptr->array_inputs[iargnum];
      a->set(NTuple(row,col),Array(value));
      tptr->cache_array_bases[iargnum] = (void*)(a->real<T>().data());
      tptr->cache_array_rows[iargnum] = a->rows();
      tptr->cache_array_cols[iargnum] = a->cols();
    } else
      ((T*)(tptr->cache_array_bases[iargnum]))
	[(int64)(row-1+(col-1)*tptr->cache_array_rows[iargnum])] = value;
    return true;
  } catch (Exception &e) {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    tptr->exception_store = e;
    return false;
  }
  return true;
}

extern "C" {
  JIT_EXPORT double scalar_load_double(void* base, double argnum) {
    return scalar_load<double>(base,argnum);
  }
  JIT_EXPORT float scalar_load_float(void* base, double argnum) {
    return scalar_load<float>(base,argnum);
  }
  JIT_EXPORT bool scalar_load_bool(void* base, double argnum) {
    return scalar_load<bool>(base,argnum);
  }
  JIT_EXPORT void scalar_store_double(void* base, double argnum, double val) {
    scalar_store<double>(base,argnum,val);
  }
  JIT_EXPORT void scalar_store_float(void* base, double argnum, float val) {
    scalar_store<float>(base,argnum,val);
  }
  JIT_EXPORT void scalar_store_bool(void* base, double argnum, bool val) {
    scalar_store<bool>(base,argnum,val);
  }
  JIT_EXPORT double vector_load_double(void* base, double argnum, double ndx, bool* success) {
    return vector_load<double>(base,argnum,ndx,success);
  }
  JIT_EXPORT float vector_load_float(void* base, double argnum, double ndx, bool* success) {
    return vector_load<float>(base,argnum,ndx,success);
  }
  JIT_EXPORT bool vector_load_bool(void* base, double argnum, double ndx, bool* success) {
    return vector_load<bool>(base,argnum,ndx,success);
  }
  JIT_EXPORT bool vector_store_double(void* base, double argnum, double ndx, double val) {
    return vector_store<double>(base,argnum,ndx,val);
  }
  JIT_EXPORT bool vector_store_float(void* base, double argnum, double ndx, float val) {
    return vector_store<float>(base,argnum,ndx,val);
  }
  JIT_EXPORT bool vector_store_bool(void* base, double argnum, double ndx, bool val) {
    return vector_store<bool>(base,argnum,ndx,val);
  }
  JIT_EXPORT double matrix_load_double(void* base, double argnum, double row, double col, bool* success) {
    return matrix_load<double>(base,argnum,row,col,success);
  }
  JIT_EXPORT float matrix_load_float(void* base, double argnum, double row, double col, bool* success) {
    return matrix_load<float>(base,argnum,row,col,success);
  }
  JIT_EXPORT bool matrix_load_bool(void* base, double argnum, double row, double col, bool* success) {
    return matrix_load<bool>(base,argnum,row,col,success);
  }
  JIT_EXPORT bool matrix_store_double(void* base, double argnum, double row, double col, double val) {
    return matrix_store<double>(base,argnum,row,col,val);
  }
  JIT_EXPORT bool matrix_store_float(void* base, double argnum, double row, double col, float val) {
    return matrix_store<float>(base,argnum,row,col,val);
  }
  JIT_EXPORT bool matrix_store_bool(void* base, double argnum, double row, double col, bool val) {
    return matrix_store<bool>(base,argnum,row,col,val);
  }
  JIT_EXPORT double csc(double t) {
    return 1.0/sin(t);
  }  
  JIT_EXPORT float cscf(float t) {
    return 1.0f/sinf(t);
  }
  JIT_EXPORT double sec(double t) {
    return 1.0/cos(t);
  }
  JIT_EXPORT float secf(float t) {
    return 1.0f/cosf(t);
  }
  JIT_EXPORT double cot(double t) {
    return 1.0/tan(t);
  }
  JIT_EXPORT float cotf(float t) {
    return 1.0f/tanf(t);
  }
  JIT_EXPORT bool check_for_interrupt(void *base) {
    JITFunc *tptr = static_cast<JITFunc*>(base);
    if (!tptr->eval->interrupt()) return false;
    tptr->eval->outputMessage("Interrupt (ctrl-b) encountered in JIT code\n");
    tptr->eval->stackTrace(0);
    return true;
  }
  JIT_EXPORT double niter_for_loop( double first, double step, double last ){
    return (double)(num_for_loop_iter( first, step, last ));
  }
  JIT_EXPORT void debug_out_d( double t ){
    qDebug() << t;
  }

}

void JITFunc::register_std_function(QString name) {
  double_funcs.insertSymbol(name,jit->DefineLinkFunction(name,"d","d"));
  float_funcs.insertSymbol(name,jit->DefineLinkFunction(name+"f","f","f"));
}

void JITFunc::initialize() {
  symbol_prefix = "";
  uid = 0;
  // Initialize the standard function
  register_std_function("cos"); register_std_function("sin");
  register_std_function("sec"); register_std_function("csc");
  register_std_function("tan"); register_std_function("atan");
  register_std_function("cot"); register_std_function("exp");
  register_std_function("expm1"); register_std_function("ceil");
  register_std_function("floor"); register_std_function("round");
  double_funcs.insertSymbol("rint",jit->DefineLinkFunction("rint","d","d"));
  double_funcs.insertSymbol("abs",jit->DefineLinkFunction("fabs","d","d"));
  float_funcs.insertSymbol("abs",jit->DefineLinkFunction("fabsf","f","f"));
  constants.insertSymbol("pi",jit->DoubleValue(4.0*atan(1.0)));
  constants.insertSymbol("e",jit->DoubleValue(exp(1.0)));
  func_scalar_load_bool = jit->DefineLinkFunction("scalar_load_bool","b","Vd");
  func_scalar_load_double = jit->DefineLinkFunction("scalar_load_double","d","Vd");
  func_scalar_load_float = jit->DefineLinkFunction("scalar_load_float","f","Vd");
  func_scalar_store_bool = jit->DefineLinkFunction("scalar_store_bool","v","Vdb");
  func_scalar_store_double = jit->DefineLinkFunction("scalar_store_double","v","Vdd");
  func_scalar_store_float = jit->DefineLinkFunction("scalar_store_float","v","Vdf");
  func_vector_load_bool = jit->DefineLinkFunction("vector_load_bool","b","VddB");
  func_vector_load_double = jit->DefineLinkFunction("vector_load_double","d","VddB");
  func_vector_load_float = jit->DefineLinkFunction("vector_load_float","f","VddB");
  func_vector_store_bool = jit->DefineLinkFunction("vector_store_bool","b","Vddb");
  func_vector_store_double = jit->DefineLinkFunction("vector_store_double","b","Vddd");
  func_vector_store_float = jit->DefineLinkFunction("vector_store_float","b","Vddf");
  func_matrix_load_bool = jit->DefineLinkFunction("matrix_load_bool","b","VdddB");
  func_matrix_load_double = jit->DefineLinkFunction("matrix_load_double","d","VdddB");
  func_matrix_load_float = jit->DefineLinkFunction("matrix_load_float","f","VdddB");
  func_matrix_store_bool = jit->DefineLinkFunction("matrix_store_bool","b","Vdddb");
  func_matrix_store_double = jit->DefineLinkFunction("matrix_store_double","b","Vdddd");
  func_matrix_store_float = jit->DefineLinkFunction("matrix_store_float","b","Vdddf");
  func_check_for_interrupt = jit->DefineLinkFunction("check_for_interrupt","b","V");
  func_niter_for_loop = jit->DefineLinkFunction("niter_for_loop","d","ddd");
  //  func_debug_out_i = jit->DefineLinkFunction("debug_out_i","v","i");
  func_debug_out_d = jit->DefineLinkFunction("debug_out_d","d","d");
}

static int countm = 0;

void JITFunc::compile(const Tree & t) {
  // The signature for the compiled function should be:
  // bool func(void** inputs);
  countm++;
  initialize();
  argument_count = 0;
  func = jit->DefineFunction(jit->FunctionType("b","V"),QString("main_%1").arg(countm));
  jit->SetCurrentFunction(func);
  prolog = jit->NewBlock("prolog");
  main_body = jit->NewBlock("main_body");
  epilog = jit->NewBlock("epilog");
  jit->SetCurrentBlock(prolog);
  retcode = jit->Alloc(jit->BoolType(),"_retcode");
  jit->Store(jit->BoolValue(true),retcode);
  jit->SetCurrentBlock(main_body);
  llvm::Function::arg_iterator args = func->arg_begin();
  this_ptr = args;
  this_ptr->setName("this_ptr");
  bool failed = false;
  try {
    compile_for_block(t);
  } catch (Exception &e) {
    failed = true;
    exception_store = e;
  }
  jit->Jump(epilog);
  jit->SetCurrentBlock(prolog);
  jit->Jump(main_body);
  jit->SetCurrentBlock(epilog);
  jit->Return(jit->Load(retcode));
  //#ifndef NDEBUG
  //  jit->Dump("unoptimized.bc.txt",func);
  //#endif
  jit->OptimizeCode();
  //#ifndef NDEBUG
  //  jit->Dump("optimized.bc.txt",func);
  //#endif
  if (failed) throw exception_store;
}

//TODO: handle other types for loop variables
void JITFunc::compile_for_block(const Tree & t) {
  JITScalar loop_start, loop_stop, loop_step;

  if (!(t.first().is('=') && t.first().second().is(':'))) 
    throw Exception("For loop cannot be compiled - need scalar bounds");
  
  if (t.first().second().first().is(':')){ //triple format 
    loop_start = jit->ToDouble( compile_expression(t.first().second().first().first()) );
    loop_step = jit->ToDouble( compile_expression(t.first().second().first().second()) );
    loop_stop = jit->ToDouble( compile_expression(t.first().second().second()) );
  }
  else{ //double format
    loop_start = jit->ToDouble( compile_expression(t.first().second().first()) );
    loop_step = jit->DoubleValue( 1 );
    loop_stop = jit->ToDouble( compile_expression(t.first().second().second()) );
  }
  QString loop_index_name(t.first().first().text());
  SymbolInfo* v = add_argument_scalar(loop_index_name,loop_start,true);
  JITScalar loop_index_address = v->address;
  jit->Store(loop_start,loop_index_address);

  JITScalar loop_nsteps = jit->Call( func_niter_for_loop, loop_start, loop_step, loop_stop );

  JITBlock ip(jit->CurrentBlock());
  jit->SetCurrentBlock(prolog);
  JITScalar loop_ind = jit->Alloc(jit->DoubleType(),"loop_ind_"+loop_index_name); 
  jit->SetCurrentBlock(ip);
  jit->Store(jit->DoubleValue(0),loop_ind); 

  JITBlock loopbody = jit->NewBlock("for_body");
  JITBlock loopcheck = jit->NewBlock("for_check");
  JITBlock looptest = jit->NewBlock("for_test");
  JITBlock loopincr = jit->NewBlock("for_increment");
  JITBlock loopexit = jit->NewBlock("for_exit");
  jit->Jump(looptest);
  // Create 3 blocks
  jit->SetCurrentBlock(loopbody);
  bool failed = false;
  try {
    compile_block(t.second());
  } catch(Exception &e) {
    exception_store = e;
    failed = true;
  }
  jit->Jump(loopcheck);
  jit->SetCurrentBlock(loopcheck);
  JITScalar abort_called = jit->Call(func_check_for_interrupt, this_ptr);
  jit->Branch(epilog,looptest,abort_called);
  jit->SetCurrentBlock(looptest);
  JITScalar loop_comparison = jit->LessThan( jit->Load( loop_ind ), loop_nsteps);
  jit->Branch(loopincr,loopexit,loop_comparison);
  jit->SetCurrentBlock(loopincr);
  //loop variable equal: loop_start+loop_ind*loop_step
  JITScalar next_loop_value = jit->Add(loop_start, jit->Mul( jit->Load( loop_ind ), loop_step ) );
  jit->Store(next_loop_value,loop_index_address);
  jit->Store( jit->Add( jit->Load( loop_ind ), jit->DoubleValue( 1 ) ), loop_ind );
  jit->Jump( loopbody );
  jit->SetCurrentBlock(loopexit);
  if (failed) throw exception_store;
}

void JITFunc::prep() {
  // Collect the list of arguments
  StringVector argumentList(symbols.getCompletions(""));
  // Allocate the argument array
  // For each argument in the array, retrieve it from the interpreter
  array_inputs = new Array*[argument_count];
  cache_array_bases = new void*[argument_count];
  cache_array_rows = new double[argument_count];
  cache_array_cols = new double[argument_count];
  //  array_inputs.resize(argumentList.size());
  for (int i=0;i<argumentList.size();i++) {
    SymbolInfo* v = symbols.findSymbol(argumentList[i]);
    if (v && (v->argument_num>=0)) {
      ArrayReference ptr(eval->getContext()->lookupVariable(argumentList[i]));
      if (!ptr.valid()) {
	//if (!v->isScalar) throw Exception("cannot create array types in the loop");
	eval->getContext()->insertVariable(argumentList[i],
					   Array(map_dataclass(v->type),
						 NTuple(1,1)));
	ptr = eval->getContext()->lookupVariable(argumentList[i]);
	if (!ptr.valid()) throw Exception("unable to create variable " + argumentList[i]);
      } 
      if (v->isScalar && (!ptr->isScalar()))
	throw Exception("Expected symbol to be a scalar, and it is not");
      array_inputs[v->argument_num] = ptr.pointer();
      if (v->type != map_dataclass(array_inputs[v->argument_num]->dataClass()))
	throw Exception("DATA mismatch!");
      if (!v->isScalar) {
	  if( !ptr.pointer()->isScalar() ){
	    if ((array_inputs[v->argument_num]->dataClass() == Float))
	      cache_array_bases[v->argument_num] = 
		(void*)(array_inputs[v->argument_num]->real<float>().data());
	    if ((array_inputs[v->argument_num]->dataClass() == Double))
	      cache_array_bases[v->argument_num] = 
		(void*)(array_inputs[v->argument_num]->real<double>().data());
	    if ((array_inputs[v->argument_num]->dataClass() == Bool))
	      cache_array_bases[v->argument_num] = 
		(void*)(array_inputs[v->argument_num]->real<bool>().data());
	    cache_array_rows[v->argument_num] = array_inputs[v->argument_num]->rows();
	    cache_array_cols[v->argument_num] = array_inputs[v->argument_num]->cols();
	  }
	  else{
	    if ((array_inputs[v->argument_num]->dataClass() == Float))
	      cache_array_bases[v->argument_num] = 
	      (void*)(&(array_inputs[v->argument_num]->realScalar<float>()));
	    if ((array_inputs[v->argument_num]->dataClass() == Double))
	      cache_array_bases[v->argument_num] = 
		(void*)(&(array_inputs[v->argument_num]->realScalar<double>()));
	    if ((array_inputs[v->argument_num]->dataClass() == Bool))
	      cache_array_bases[v->argument_num] = 
		(void*)(&(array_inputs[v->argument_num]->realScalar<bool>()));
	    cache_array_rows[v->argument_num] = 1;
	    cache_array_cols[v->argument_num] = 1;
	  }
      } else {
	cache_array_bases[v->argument_num] = NULL;
	cache_array_rows[v->argument_num] = 1;
	cache_array_cols[v->argument_num] = 1;
      }
    } 
  }
}

void JITFunc::run() {
  save_this = this;
  JITGeneric gv = jit->Invoke(func,JITGeneric((void*) this));
  if (gv.IntVal == 0)
    throw exception_store;
}

#endif
