#include "CodeGen.hpp"
#include "Context.hpp"
#include "Interpreter.hpp"


static JITFunction func_scalar_load_double, func_scalar_load_float, func_scalar_load_int32;
static JITFunction func_scalar_store_double, func_scalar_store_float, func_scalar_store_int32;
static JITFunction func_vector_load_double, func_vector_load_float, func_vector_load_int32;
static JITFunction func_vector_store_double, func_vector_store_float, func_vector_store_int32;
static JITFunction func_matrix_load_double, func_matrix_load_float, func_matrix_load_int32;
static JITFunction func_matrix_store_double, func_matrix_store_float, func_matrix_store_int32;

SymbolInfo* CodeGen::add_argument_array(string name) {
  ArrayReference ptr(eval->getContext()->lookupVariable(name));
  Class aclass = FM_FUNCPTR_ARRAY;
  if (!ptr.valid())
    return NULL;
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

Class CodeGen::map_dataclass(JITType type) {
  if (jit->IsFloat(type))
    return FM_FLOAT;
  else if (jit->IsDouble(type))
    return FM_DOUBLE;
  return FM_INT32;
}

JITType CodeGen::map_dataclass(Class aclass) {
  switch (aclass) {
  default:
    throw Exception("JIT does not support");
  case FM_INT32:
    return jit->Int32Type();
  case FM_FLOAT:
    return jit->FloatType();
  case FM_DOUBLE:
    return jit->DoubleType();
  }
  return NULL;
}

// FIXME - Simplify
SymbolInfo* CodeGen::add_argument_scalar(string name, JITScalar val, bool override) {
  ArrayReference ptr(eval->getContext()->lookupVariable(name));
  Class aclass = FM_FUNCPTR_ARRAY;
  if (!val && !ptr.valid()) return NULL;
  if (!ptr.valid() || override) {
    if (jit->IsInteger(val))
      aclass = FM_INT32;
    else if (jit->IsFloat(val))
      aclass = FM_FLOAT;
    else if (jit->IsDouble(val))
      aclass = FM_DOUBLE;
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
    jit->Store(jit->Call(func_scalar_load_double, this_ptr, jit->Int32Value(argument_count-1)), address);
  else if (jit->IsFloat(type))
    jit->Store(jit->Call(func_scalar_load_float, this_ptr, jit->Int32Value(argument_count-1)), address);
  else if (jit->IsInteger(type))
    jit->Store(jit->Call(func_scalar_load_int32, this_ptr, jit->Int32Value(argument_count-1)), address);
  jit->SetCurrentBlock(epilog);
  if (jit->IsDouble(type))
    jit->Call(func_scalar_store_double, this_ptr, jit->Int32Value(argument_count-1), jit->Load(address));
  else if (jit->IsFloat(type))
    jit->Call(func_scalar_store_float, this_ptr, jit->Int32Value(argument_count-1), jit->Load(address));
  else if (jit->IsInteger(type))
    jit->Call(func_scalar_store_int32, this_ptr, jit->Int32Value(argument_count-1), jit->Load(address));
  jit->SetCurrentBlock(ip);
  return symbols.findSymbol(name);
}

CodeGen::CodeGen(Interpreter *p_eval) {
  jit = p_eval->JITPointer();
  eval = p_eval;
}

void CodeGen::compile_block(Tree* t) {
  const TreeList &statements(t->children());
  for (TreeList::const_iterator i=statements.begin();i!=statements.end();i++) 
    compile_statement(*i);
}

void CodeGen::compile_statement_type(Tree* t) {
  switch(t->token()) {
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
    compile_expression(t->first());
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void CodeGen::compile_statement(Tree* t) {
  if (t->is(TOK_STATEMENT) && 
      (t->first()->is(TOK_EXPR) || t->first()->is(TOK_SPECIAL) ||
       t->first()->is(TOK_MULTI) || t->first()->is('=')))
    throw Exception("JIT compiler doesn't work with verbose statements");
  compile_statement_type(t->first());
}

JITScalar CodeGen::compile_function_call(Tree* t) {
  
}

JITScalar CodeGen::compile_rhs(Tree* t) {
  string symname(t->first()->text());
  SymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (t->numChildren() == 1)
      v = add_argument_scalar(symname);
    else
      v = add_argument_array(symname);
    if (!v)
      return compile_function_call(t);
  }
  if (t->numChildren() == 1) {
    if (!v->isScalar)
      throw Exception("non-scalar reference returned in scalar context!");
    return jit->Load(v->address);
  }
  if (t->numChildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar)
    throw Exception("array indexing of scalar values...");
  Tree* s(t->second());
  if (!s->is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s->numChildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s->numChildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s->numChildren() == 1) {
    JITScalar arg1 = jit->Cast(compile_expression(s->first()),jit->Int32Type());
    if (jit->IsDouble(v->type))
      return jit->Call(func_vector_load_double, this_ptr, jit->Int32Value(v->argument_num), arg1);
    else if (jit->IsFloat(v->type))
      return jit->Call(func_vector_load_float, this_ptr, jit->Int32Value(v->argument_num), arg1);
    else if (jit->IsInteger(v->type))
      return jit->Call(func_vector_load_int32, this_ptr, jit->Int32Value(v->argument_num), arg1);
    throw Exception("Unsupported JIT type in Load");
  } else if (s->numChildren() == 2) {
    JITScalar arg1 = jit->Cast(compile_expression(s->first()),jit->Int32Type());
    JITScalar arg2 = jit->Cast(compile_expression(s->second()),jit->Int32Type());
    if (jit->IsDouble(v->type))
      return jit->Call(func_matrix_load_double, this_ptr, jit->Int32Value(v->argument_num), arg1, arg2);
    else if (jit->IsFloat(v->type))
      return jit->Call(func_matrix_load_float, this_ptr, jit->Int32Value(v->argument_num), arg1, arg2);
    else if (jit->IsInteger(v->type))
      return jit->Call(func_matrix_load_int32, this_ptr, jit->Int32Value(v->argument_num), arg1, arg2);
    throw Exception("Unsupported JIT type in Load");
  }
  throw Exception("dereference not handled yet...");
}

JITScalar CodeGen::compile_expression(Tree* t) {
  switch(t->token()) {
  case TOK_VARIABLE:     return compile_rhs(t);
  case TOK_INTEGER:      return jit->Int32Value(ArrayToInt32(t->array()));
  case TOK_FLOAT:        return jit->FloatValue(ArrayToDouble(t->array()));
  case TOK_DOUBLE:       return jit->DoubleValue(ArrayToDouble(t->array()));
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF:      throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case '+':
    return jit->Add(compile_expression(t->first()),compile_expression(t->second()));
  case '-': 
    return jit->Sub(compile_expression(t->first()),compile_expression(t->second()));
  case '*': 
  case TOK_DOTTIMES: 
    return jit->Mul(compile_expression(t->first()),compile_expression(t->second()));
  case '/': 
  case TOK_DOTRDIV:
    return jit->Div(compile_expression(t->first()),compile_expression(t->second()));
  case '\\': 
  case TOK_DOTLDIV: 
    return jit->Div(compile_expression(t->second()),compile_expression(t->first()));
    // FIXME: Are shortcuts handled correctly here?
  case TOK_SOR: 
  case '|':
    return jit->Or(compile_expression(t->first()),compile_expression(t->second()));
  case TOK_SAND: 
  case '&': 
    return jit->And(compile_expression(t->first()),compile_expression(t->second()));
  case '<': 
    return jit->LessThan(compile_expression(t->first()),compile_expression(t->second()));
  case TOK_LE: 
    return jit->LessEquals(compile_expression(t->first()),compile_expression(t->second()));
  case '>': 
    return jit->GreaterThan(compile_expression(t->first()),compile_expression(t->second()));
  case TOK_GE: 
    return jit->GreaterEquals(compile_expression(t->first()),compile_expression(t->second()));
  case TOK_EQ: 
    return jit->Equals(compile_expression(t->first()),compile_expression(t->second()));
  case TOK_NE: 
    return jit->NotEqual(compile_expression(t->first()),compile_expression(t->second()));
  case TOK_UNARY_MINUS: 
    {
      JITScalar val(compile_expression(t->first()));
      return jit->Sub(jit->Zero(jit->TypeOf(val)),val);
    }
  case TOK_UNARY_PLUS: 
    return compile_expression(t->first());
  case '~': 
    {
      JITScalar val(compile_expression(t->first()));
      return jit->Xor(val, jit->BoolValue(true));
    }
  case '^':               throw Exception("^ is not currently handled by the JIT compiler");
  case TOK_DOTPOWER:      throw Exception(".^ is not currently handled by the JIT compiler");
  case '\'':              throw Exception("' is not currently handled by the JIT compiler");
  case TOK_DOTTRANSPOSE:  throw Exception(".' is not currently handled by the JIT compiler");
  case '@':               throw Exception("@ is not currently handled by the JIT compiler");
  default:                throw Exception("Unrecognized expression!");
  }  
}

void CodeGen::compile_assignment(Tree* t) {
  Tree* s(t->first());
  string symname(s->first()->text());
  JITScalar rhs(compile_expression(t->second()));
  SymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (s->numChildren() == 1)
      v = add_argument_scalar(symname,rhs,false);
    else
      v = add_argument_array(symname);
    if (!v) throw Exception("Undefined variable reference:" + symname);
  }
  if (s->numChildren() == 1) {
    if (v->type != jit->TypeOf(rhs))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!v->isScalar)
      throw Exception("scalar assignment to array variable.");
    jit->Store(rhs, v->address);
    return;
  }
  if (s->numChildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar)
    throw Exception("array indexing of scalar values...");
  Tree* q(s->second());
  if (!q->is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q->numChildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q->numChildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q->numChildren() == 1) {
    JITScalar arg1 = jit->Cast(compile_expression(q->first()),jit->Int32Type());
    if (jit->IsDouble(rhs))
      jit->Call(func_vector_store_double, this_ptr, jit->Int32Value(v->argument_num), arg1, rhs);
    else if (jit->IsFloat(rhs))
      jit->Call(func_vector_store_float, this_ptr, jit->Int32Value(v->argument_num), arg1, rhs);
    else if (jit->IsInteger(rhs))
      jit->Call(func_vector_store_int32, this_ptr, jit->Int32Value(v->argument_num), arg1, rhs);
  } else if (q->numChildren() == 2) {
    JITScalar arg1 = jit->Cast(compile_expression(q->first()),jit->Int32Type());
    JITScalar arg2 = jit->Cast(compile_expression(q->second()),jit->Int32Type());
    if (jit->IsDouble(rhs))
      jit->Call(func_matrix_store_double, this_ptr, jit->Int32Value(v->argument_num), arg1, arg2, rhs);
    else if (jit->IsFloat(rhs))
      jit->Call(func_matrix_store_float, this_ptr, jit->Int32Value(v->argument_num), arg1, arg2, rhs);
    else if (jit->IsInteger(rhs))
      jit->Call(func_matrix_store_int32, this_ptr, jit->Int32Value(v->argument_num), arg1, arg2, rhs);
  }
}

void CodeGen::compile_if_statement(Tree* t) {
  JITScalar main_cond(jit->Cast(compile_expression(t->first()),jit->BoolType()));
  JITBlock if_true = jit->NewBlock("if_true");
  JITBlock if_continue = jit->NewBlock("if_continue");
  JITBlock if_exit = jit->NewBlock("if_exit");
  jit->Branch(if_true,if_continue,main_cond);
  jit->SetCurrentBlock(if_true);
  compile_block(t->second());
  jit->Jump(if_exit);
  int n=2;
  while (n < t->numChildren() && t->child(n)->is(TOK_ELSEIF)) {
    jit->SetCurrentBlock(if_continue);
    JITScalar ttest(jit->Cast(compile_expression(t->child(n)->first()),jit->BoolType()));
    if_true = jit->NewBlock("elseif_true");
    if_continue = jit->NewBlock("elseif_continue");
    jit->Branch(if_true,if_continue,ttest);
    jit->SetCurrentBlock(if_true);
    compile_block(t->child(n)->second());
    jit->Jump(if_exit);
    n++;
  }
  if (t->last()->is(TOK_ELSE)) {
    jit->SetCurrentBlock(if_continue);
    compile_block(t->last()->first());
    jit->Jump(if_exit);
  } else {
    jit->SetCurrentBlock(if_continue);
    jit->Jump(if_exit);
  }
  jit->SetCurrentBlock(if_exit);
}

double CodeGen::scalar_load_double(void* base, int argnum) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  return ((double*)(this_ptr->array_inputs[argnum]->getDataPointer()))[0];
}

float CodeGen::scalar_load_float(void* base, int argnum) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  return ((float*)(this_ptr->array_inputs[argnum]->getDataPointer()))[0];
}

int32 CodeGen::scalar_load_int32(void* base, int argnum) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  return ((int32*)(this_ptr->array_inputs[argnum]->getDataPointer()))[0];
}

void CodeGen::scalar_store_double(void* base, int argnum, double rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  ((double*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[0] = rhs;
}

void CodeGen::scalar_store_float(void* base, int argnum, float rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  ((float*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[0] = rhs;
}

void CodeGen::scalar_store_int32(void* base, int argnum, int32 rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  ((int32*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[0] = rhs;
}

double CodeGen::vector_load_double(void* base, int argnum, int32 ndx) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  return ((double*)(this_ptr->array_inputs[argnum]->getDataPointer()))[ndx-1];
}

float CodeGen::vector_load_float(void* base, int argnum, int32 ndx) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  return ((float*)(this_ptr->array_inputs[argnum]->getDataPointer()))[ndx-1];
}

int32 CodeGen::vector_load_int32(void* base, int argnum, int32 ndx) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  return ((int32*)(this_ptr->array_inputs[argnum]->getDataPointer()))[ndx-1];
}

void CodeGen::vector_store_double(void* base, int argnum, int32 ndx, double rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  ((double*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[ndx-1] = rhs;
}

void CodeGen::vector_store_float(void* base, int argnum, int32 ndx, float rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  ((float*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[ndx-1] = rhs;
}

void CodeGen::vector_store_int32(void* base, int argnum, int32 ndx, int32 rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  ((int32*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[ndx-1] = rhs;
}

double CodeGen::matrix_load_double(void* base, int argnum, int32 row, int32 col) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  return ((double*)(this_ptr->array_inputs[argnum]->getDataPointer()))[(col-1)*rows+row-1];
}

float CodeGen::matrix_load_float(void* base, int argnum, int32 row, int32 col) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  return ((float*)(this_ptr->array_inputs[argnum]->getDataPointer()))[(col-1)*rows+row-1];
}

int32 CodeGen::matrix_load_int32(void* base, int argnum, int32 row, int32 col) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  return ((int32*)(this_ptr->array_inputs[argnum]->getDataPointer()))[(col-1)*rows+row-1];
}

void CodeGen::matrix_store_double(void* base, int argnum, int32 row, int32 col, double rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  ((double*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[(col-1)*rows+row-1] = rhs;
}

void CodeGen::matrix_store_float(void* base, int argnum, int32 row, int32 col, float rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  ((float*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[(col-1)*rows+row-1] = rhs;
}

void CodeGen::matrix_store_int32(void* base, int argnum, int32 row, int32 col, int32 rhs) {
  CodeGen *this_ptr = static_cast<CodeGen*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  ((int32*)(this_ptr->array_inputs[argnum]->getReadWriteDataPointer()))[(col-1)*rows+row-1] = rhs;
}

void CodeGen::initialize() {
  func_scalar_load_int32 = jit->DefineLinkFunction("func_scalar_load_int32","i","pi",(void*)CodeGen::scalar_load_int32);
  func_scalar_load_double = jit->DefineLinkFunction("func_scalar_load_double","d","pi",(void*)CodeGen::scalar_load_double);
  func_scalar_load_float = jit->DefineLinkFunction("func_scalar_load_float","f","pi",(void*)CodeGen::scalar_load_float);
  func_vector_load_int32 = jit->DefineLinkFunction("func_vector_load_int32","i","pii",(void*)CodeGen::vector_load_int32);
  func_vector_load_double = jit->DefineLinkFunction("func_vector_load_double","d","pii",(void*)CodeGen::vector_load_double);
  func_vector_load_float = jit->DefineLinkFunction("func_vector_load_float","f","pii",(void*)CodeGen::vector_load_float);
  func_matrix_load_int32 = jit->DefineLinkFunction("func_matrix_load_int32","i","piii",(void*)CodeGen::matrix_load_int32);
  func_matrix_load_double = jit->DefineLinkFunction("func_matrix_load_double","d","piii",(void*)CodeGen::matrix_load_double);
  func_matrix_load_float = jit->DefineLinkFunction("func_matrix_load_float","f","piii",(void*)CodeGen::matrix_load_float);
  func_scalar_store_int32 = jit->DefineLinkFunction("func_scalar_store_int32","v","pii",(void*)CodeGen::scalar_store_int32);
  func_scalar_store_double = jit->DefineLinkFunction("func_scalar_store_double","v","pid",(void*)CodeGen::scalar_store_double);
  func_scalar_store_float = jit->DefineLinkFunction("func_scalar_store_float","v","pif",(void*)CodeGen::scalar_store_float);
  func_vector_store_int32 = jit->DefineLinkFunction("func_vector_store_int32","v","piii",(void*)CodeGen::vector_store_int32);
  func_vector_store_double = jit->DefineLinkFunction("func_vector_store_double","v","piid",(void*)CodeGen::vector_store_double);
  func_vector_store_float = jit->DefineLinkFunction("func_vector_store_float","v","piif",(void*)CodeGen::vector_store_float);
  func_matrix_store_int32 = jit->DefineLinkFunction("func_matrix_store_int32","v","piiii",(void*)CodeGen::matrix_store_int32);
  func_matrix_store_double = jit->DefineLinkFunction("func_matrix_store_double","v","piiid",(void*)CodeGen::matrix_store_double);
  func_matrix_store_float = jit->DefineLinkFunction("func_matrix_store_float","v","piiif",(void*)CodeGen::matrix_store_float);  
}

static int countm = 0;

void CodeGen::compile(Tree* t) {
  // The signature for the compiled function should be:
  // int func(void** inputs);
  if (!jit->Initialized()) {
    initialize();
    std::cout << "Initializing\r\n";
    jit->SetInitialized(true);
  }
  argument_count = 0;
  func = jit->DefineFunction(jit->FunctionType("v","p"),std::string("main") + countm++);
  jit->SetCurrentFunction(func);
  prolog = jit->NewBlock("prolog");
  main_body = jit->NewBlock("main_body");
  epilog = jit->NewBlock("epilog");
  jit->SetCurrentBlock(main_body);
  llvm::Function::arg_iterator args = func->arg_begin();
  this_ptr = args;
  this_ptr->setName("this_ptr");
  compile_for_block(t);
  jit->Jump(epilog);
  jit->SetCurrentBlock(prolog);
  jit->Jump(main_body);
  jit->SetCurrentBlock(epilog);
  jit->Return();
  std::cout << "************************************************************\n";
  std::cout << "*  Before optimization \n";
  jit->Dump();
  jit->OptimizeCode();
  std::cout << "************************************************************\n";
  std::cout << "*  After optimization \n";
  jit->Dump();
}

#warning - How to detect non-integer loop bounds?
void CodeGen::compile_for_block(Tree* t) {
  if (!(t->first()->is('=') && t->first()->second()->is(':'))) 
    throw Exception("For loop cannot be compiled - need scalar bounds");
  if (t->first()->second()->first()->is(':'))
    throw Exception("For loop does not work with triple format bounds");
  JITScalar loop_start = compile_expression(t->first()->second()->first());
  JITScalar loop_stop = compile_expression(t->first()->second()->second());
  string loop_index_name(t->first()->first()->text());
  SymbolInfo* v = add_argument_scalar(loop_index_name,loop_start,true);
  JITScalar loop_index_address = v->address;
  jit->Store(loop_start,loop_index_address);
  JITBlock loopbody = jit->NewBlock("for_body");
  JITBlock looptest = jit->NewBlock("for_test");
  JITBlock loopexit = jit->NewBlock("for_exit");
  jit->Jump(looptest);
  // Create 3 blocks
  jit->SetCurrentBlock(loopbody);
  compile_block(t->second());
  JITScalar loop_index_value = jit->Load(loop_index_address);
  JITScalar next_loop_value = jit->Add(loop_index_value,jit->DoubleValue(1));
  jit->Store(next_loop_value,loop_index_address);
  jit->Jump(looptest);
  jit->SetCurrentBlock(looptest);
  loop_index_value = jit->Load(loop_index_address);
  JITScalar loop_comparison = jit->LessEquals(loop_index_value,loop_stop);
  jit->Branch(loopbody,loopexit,loop_comparison);
  jit->SetCurrentBlock(loopexit);
}

void CodeGen::run() {
  // Collect the list of arguments
  StringVector argumentList(symbols.getCompletions(""));
  // Allocate the argument array
  // For each argument in the array, retrieve it from the interpreter
  array_inputs.reserve(argumentList.size());
  for (int i=0;i<argumentList.size();i++) {
    SymbolInfo* v = symbols.findSymbol(argumentList[i]);
    if (v) {
      ArrayReference ptr(eval->getContext()->lookupVariable(argumentList[i]));
      if (!ptr.valid()) {
	if (!v->isScalar) throw Exception("cannot create array types in the loop");
	eval->getContext()->insertVariable(argumentList[i],
					   Array(map_dataclass(v->type),
						 Dimensions(1,1),
						 Array::allocateArray(map_dataclass(v->type),1)));
	ptr = eval->getContext()->lookupVariable(argumentList[i]);
	if (!ptr.valid()) throw Exception("unable to create variable " + argumentList[i]);
      }
      array_inputs[v->argument_num] = &(*ptr);
    }
  }
  
  jit->Invoke(func,JITGeneric((void*) this));
  
}
