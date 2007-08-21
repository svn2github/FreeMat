// Still need:

//  Type promotion on loop entry.
//  Range checking.
//  Dynamic array resizing.
//

//
// Just for fun, mind you....
//

#include "JITVM.hpp"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include <fstream>

using namespace llvm;

#if 0
JITScalar JITVM::promote(JITScalar arg, scalar_class outClass) {
  // For scalar types, just promote the thing
  if (arg.type() < c_pointer) {
    if (arg.type() == c_bool) {
      switch (outClass) {
      case c_bool:
	return arg;
      case c_int32:
	if (arg.b())
	  return JITScalar((int32) 1);
	else
	  return JITScalar((int32) 0);
      case c_float:
	if (arg.b())
	  return JITScalar((float) 1);
	else
	  return JITScalar((float) 0);
      case c_double:
	if (arg.b())
	  return JITScalar((double) 1);
	else
	  return JITScalar((double) 0);
      default:
	throw Exception("unhandled case for promote called from bool");
      }
    } else if (arg.type() == c_int32) {
      switch (outClass) {
      case c_bool:
	return JITScalar(arg.i() != 0);
      case c_int32:
	return arg;
      case c_float:
	return JITScalar((float) arg.i());
      case c_double:
	return JITScalar((double) arg.i());
      default:
	throw Exception("unhandled case for promote called from int32");
      }
    } else if (arg.type() == c_float) {
      switch (outClass) {
      case c_bool:
	return JITScalar(arg.f() != 0);
      case c_int32:
	return JITScalar((int) arg.f());
      case c_float:
	return arg;
      case c_double:
	return JITScalar((double) arg.f());
      default:
	throw Exception("unhandled case for promote called from float");
      }
    } else if (arg.type() == c_double) {
      switch (outClass) {
      case c_bool:
	return JITScalar(arg.d() != 0);
      case c_int32:
	return JITScalar((int) arg.d());
      case c_float:
	return JITScalar((float) arg.d());
      case c_double:
	return arg;
      default:
	throw Exception("unhandled case for promote called from double");
      }
    } else
      throw Exception("unhandled case for promote called");
  }
  if (arg.type() == c_register) {
    if (type_of(arg) == outClass) return arg;
    JITScalar result(new_register(outClass));
    switch (outClass) {
    case c_bool:
      push_instruction(op_castb,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    case c_int32:
      push_instruction(op_casti,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    case c_float:
      push_instruction(op_castf,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    case c_double:
      push_instruction(op_castd,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    default:
      throw Exception("unhandled case for promote called");
    }
    return result;
  }
  throw Exception("unhandled case for promote called");
}
#endif

JITScalar JITVM::compile_boolean_op(Instruction::BinaryOps op, JITScalar arg1, JITScalar arg2, string inst) {
//   arg1 = promote(arg1,c_bool);
//   arg2 = promote(arg2,c_bool);
//   JITScalar result(new_register(c_bool));
//   push_instruction(op,type_b,result,compute_ri_code(arg1,arg2),arg1,arg2);
//   return result;
}

static inline bool isi(JITScalar arg) {
  return arg->getType()->isInteger();
}

static inline bool isfd(JITScalar arg) {
  return arg->getType()->isFloatingPoint();
}

static inline bool isf(JITScalar arg) {
  return (arg->getType()->getTypeID() == Type::FloatTyID);
}

static inline bool isd(JITScalar arg) {
  return (arg->getType()->getTypeID() == Type::DoubleTyID);
}

// add immediate to immediate - this instruction can be replaced by 
// constant folding, but for now, leave it in.
JITScalar JITVM::compile_binary_op(BinaryOperator::BinaryOps opcode, 
				   JITScalar arg1, JITScalar arg2, string inst) {
  const Type* outType;
  if (arg1->getType() == arg2->getType()) 
    outType = arg1->getType();
  else if ((isi(arg1) && isfd(arg2)) || (isi(arg2) && isfd(arg1)))
    outType = Type::DoubleTy;
  else if ((isf(arg1) && isd(arg2)) || (isd(arg1) && isf(arg2)))
    outType = Type::DoubleTy;
  arg1 = cast(arg1,outType,true,ip);
  arg2 = cast(arg2,outType,true,ip);
  return BinaryOperator::create(opcode,arg1,arg2,"",ip);
}

JITScalar JITVM::compile_comparison_op(byte op, JITScalar arg1, JITScalar arg2, string inst) {
//   scalar_class outClass;
//   outClass = type_of(arg1);
//   if (type_of(arg2) > outClass) outClass = type_of(arg2);
//   if (outClass == c_bool) outClass = c_int32;
//   JITScalar result(new_register(c_bool));
//   push_instruction(op,compute_oc_code(outClass),result,compute_ri_code(arg1,arg2),arg1,arg2);
//   return result;
}

void JITVM::compile_assignment(tree t, Interpreter* m_eval) {
  tree s(t.first());
  string symname(s.first().text());
  JITSymbolInfo *v = symbols.findSymbol(symname);
  if (!v)
    v = add_argument(symname,m_eval,s.numchildren() == 1);
  JITScalar rhs(compile_expression(t.second(),m_eval));
  if (s.numchildren() == 1) {
    if (v->data_value->getType() != PointerType::get(rhs->getType()))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!v->is_scalar)
      throw Exception("scalar assignment to array variable.");
    new StoreInst(v->data_value,rhs, ip);
    return;
  }
  if (s.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->is_scalar)
    throw Exception("array indexing of scalar values...");
  tree q(s.second());
  if (!q.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q.numchildren() == 1) {
    if (v->data_value->getType() != PointerType::get(rhs->getType()))
      throw Exception("polymorphic assignment to array detected");
    JITScalar arg1 = compile_expression(q.first(),m_eval);
    arg1 = cast(arg1,IntegerType::get(32),false,ip);
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,ConstantInt::get(APInt(32,"1",10)),"",ip);
    JITScalar address = new GetElementPtrInst(v->data_value, arg1, "", ip);
    new StoreInst(rhs, address, false, ip);
  } else if (q.numchildren() == 2) {
    if (v->data_value->getType() != PointerType::get(rhs->getType()))
      throw Exception("polymorphic assignment to array detected");
    JITScalar arg1 = compile_expression(q.first(),m_eval);
    JITScalar arg2 = compile_expression(q.second(),m_eval);
    arg1 = cast(arg1,IntegerType::get(32),false,ip);
    arg2 = cast(arg1,IntegerType::get(32),false,ip);
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,ConstantInt::get(APInt(32,"1",10)),"",ip);
    arg2 = BinaryOperator::create(Instruction::Sub,arg2,ConstantInt::get(APInt(32,"1",10)),"",ip);
    JITScalar lin = BinaryOperator::create(Instruction::Mul,arg2,ConstantInt::get(Type::Int32Ty,v->num_rows),"",ip);
    lin = BinaryOperator::create(Instruction::Add,lin,arg2,"",ip);
    JITScalar address = new GetElementPtrInst(v->data_value, lin, "", ip);
    new StoreInst(rhs, address, false, ip);
  }
}

void JITVM::compile_if_statement(tree t, Interpreter* m_eval) {
#if 0
  vector<JITInstruction*> endInstructions;
  JITScalar cond(compile_expression(t.first(),m_eval));
  push_instruction(op_jif,type_i,cond,arg_ii,JITScalar((int32)(0)));
  JITInstruction *prev_fixup = &data.back();
  compile_block(t.second(),m_eval);
  push_instruction(op_jmp,type_i,JITScalar(0),arg_ii,JITScalar((int32)(0)));
  endInstructions.push_back(&data.back());
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    prev_fixup->arg1 = JITScalar((int32)(data.size()));
    JITScalar ttest(compile_expression(t.child(n).first(),m_eval));
    push_instruction(op_jif,type_i,ttest,arg_ii,JITScalar((int32) 0));
    prev_fixup = &data.back();
    compile_block(t.child(n).second(),m_eval);
    push_instruction(op_jmp,type_i,JITScalar(0),arg_ii,JITScalar((int32)(0)));
    endInstructions.push_back(&data.back());
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    prev_fixup->arg1 = JITScalar(data.size());
    compile_block(t.last().first(),m_eval);
  }
  int end_address = data.size();
  for (int i=0;i<endInstructions.size();i++)
    endInstructions[i]->arg1 = JITScalar((int32)(end_address));
  push_instruction(op_nop,type_i,JITScalar(),arg_ii,JITScalar());
#endif
}

JITScalar JITVM::cast(JITScalar value, const Type *type, bool sgnd, BasicBlock *where, string name) {
  return CastInst::create(CastInst::getCastOpcode(value,sgnd,type,sgnd),
			  value, type, name, where);
}

JITSymbolInfo* JITVM::add_argument(string name, Interpreter* m_eval, bool scalar) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
  if (!ptr.valid())
    throw Exception("Undefined variable reference:" + name);
  if (!ptr->is2D())
    throw Exception("Cannot JIT multi-dimensional array:" + name);
  if (ptr->isString() || ptr->isReferenceType())
    throw Exception("Cannot JIT strings or reference types:" + name);
  if (ptr->isComplex())
    throw Exception("Cannot JIT complex arrays:" + name);
  if (!ptr->isScalar() && scalar)
    throw Exception("JIT requires " + name + " be a scalar");
  Array* a = &(*ptr);
  Value* t, *s;
  // Get a 
  switch (a->dataClass()) {
  case FM_FUNCPTR_ARRAY:
  case FM_CELL_ARRAY:
  case FM_STRUCT_ARRAY:
  case FM_LOGICAL:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT32:
  case FM_UINT64:
  case FM_INT64:
  case FM_COMPLEX:
  case FM_DCOMPLEX:
  case FM_STRING:
    throw Exception("JIT does not support");
  case FM_INT32:
    s = new GetElementPtrInst(ptr_inputs,ConstantInt::get(Type::Int32Ty,argument_count),
			      "",func_prolog);
    s = new LoadInst(s, "", false, func_prolog);
    t = cast(s,PointerType::get(IntegerType::get(32)),false,func_prolog,name);
    break;
  case FM_FLOAT:
    s = new GetElementPtrInst(ptr_inputs,ConstantInt::get(Type::Int32Ty,argument_count),
			      "",func_prolog);
    s = new LoadInst(s, "", false, func_prolog);
    t = cast(s,PointerType::get(Type::FloatTy),false,func_prolog,name);
    break;
  case FM_DOUBLE:
    s = new GetElementPtrInst(ptr_inputs,ConstantInt::get(Type::Int32Ty,argument_count),
			      "",func_prolog);
    s = new LoadInst(s, "", false, func_prolog);
    t = cast(s,PointerType::get(Type::DoubleTy),false,func_prolog,name);
    break;
  }
  symbols.insertSymbol(name,JITSymbolInfo(true,argument_count,scalar,true,
					  a->rows(),a->columns(),t));
  argument_count++;
  return symbols.findSymbol(name);
}

JITScalar JITVM::compile_rhs(tree t, Interpreter* m_eval) {
  string symname(t.first().text());
  JITSymbolInfo *v = symbols.findSymbol(symname);
  if (!v)
    v = add_argument(symname,m_eval,t.numchildren() == 1);
  if (t.numchildren() == 1) {
    if (!v->is_scalar)
      throw Exception("non-scalar reference returned in scalar context!");
    return new LoadInst(v->data_value, "", false, ip);
  }
  if (t.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->is_scalar)
    throw Exception("array indexing of scalar values...");
  tree s(t.second());
  if (!s.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s.numchildren() == 1) {
    JITScalar arg1 = compile_expression(s.first(),m_eval);
    arg1 = cast(arg1,IntegerType::get(32),false,ip);
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,ConstantInt::get(APInt(32,"1",10)),"",ip);
    JITScalar address = new GetElementPtrInst(v->data_value, arg1, "", ip);
    return new LoadInst(address, "", false, ip);
  } else if (s.numchildren() == 2) {
    JITScalar arg1 = compile_expression(s.first(),m_eval);
    JITScalar arg2 = compile_expression(s.second(),m_eval);
    arg1 = cast(arg1,IntegerType::get(32),false,ip);
    arg2 = cast(arg1,IntegerType::get(32),false,ip);
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,ConstantInt::get(APInt(32,"1",10)),"",ip);
    arg2 = BinaryOperator::create(Instruction::Sub,arg2,ConstantInt::get(APInt(32,"1",10)),"",ip);
    JITScalar lin = BinaryOperator::create(Instruction::Mul,arg2,ConstantInt::get(Type::Int32Ty,v->num_rows),"",ip);
    lin = BinaryOperator::create(Instruction::Add,lin,arg2,"",ip);
    JITScalar address = new GetElementPtrInst(v->data_value, lin, "", ip);
    return new LoadInst(address, "", false, ip);
  }
  throw Exception("dereference not handled yet...");
}

JITScalar JITVM::compile_expression(tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return compile_rhs(t,m_eval);
  case TOK_INTEGER:
    return ConstantInt::get(Type::Int32Ty,ArrayToInt32(t.array()),true);
  case TOK_FLOAT: 
    return ConstantFP::get(Type::FloatTy,ArrayToDouble(t.array()));
  case TOK_DOUBLE:
    return ConstantFP::get(Type::DoubleTy,ArrayToDouble(t.array()));
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF: 
    throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case '+':
    return compile_binary_op(Instruction::Add,
			     compile_expression(t.first(),m_eval),
			     compile_expression(t.second(),m_eval),"add");
  case '-': 
    return compile_binary_op(Instruction::Sub,
			     compile_expression(t.first(),m_eval),
			     compile_expression(t.second(),m_eval),"sub");
  case '*': 
  case TOK_DOTTIMES: 
    return compile_binary_op(Instruction::Mul,
			     compile_expression(t.first(),m_eval),
			     compile_expression(t.second(),m_eval),"mul");
  case '/': 
  case TOK_DOTRDIV: 
    //     return binary_div(expression(t.first(),m_eval),
    // 		      expression(t.second(),m_eval));
    throw Exception("Division is not supported yet.");
  case '\\': 
  case TOK_DOTLDIV: 
    //     return binary_div(expression(t.second(),m_eval),
    // 		      expression(t.first(),m_eval));
    throw Exception("Division is not supported yet.");
    // FIXME: Are shortcuts handled correctly here?
  case TOK_SOR: 
  case '|':
    return compile_boolean_op(Instruction::Or,
 			      compile_expression(t.first(),m_eval),
 			      compile_expression(t.second(),m_eval),"or");
  case TOK_SAND: 
  case '&': 
    return compile_boolean_op(Instruction::And,
			      compile_expression(t.first(),m_eval),
			      compile_expression(t.second(),m_eval),"and");
  case '<': 
    return compile_comparison_op(t.token(),
				 compile_expression(t.first(),m_eval),
				 compile_expression(t.second(),m_eval),"lt");
  case TOK_LE: 
    return compile_comparison_op(t.token(),
				 compile_expression(t.first(),m_eval),
				 compile_expression(t.second(),m_eval),"le");
  case '>': 
    return compile_comparison_op(t.token(),
				 compile_expression(t.first(),m_eval),
				 compile_expression(t.second(),m_eval),"gt");
  case TOK_GE: 
    return compile_comparison_op(t.token(),
				 compile_expression(t.first(),m_eval),
				 compile_expression(t.second(),m_eval),"ge");
  case TOK_EQ: 
    return compile_comparison_op(t.token(),
				 compile_expression(t.first(),m_eval),
				 compile_expression(t.second(),m_eval),"eq");
  case TOK_NE: 
    return compile_comparison_op(t.token(),
				 compile_expression(t.first(),m_eval),
				 compile_expression(t.second(),m_eval),"ne");
  case TOK_UNARY_MINUS: 
    throw Exception("unary minus not supported yet.");
  case TOK_UNARY_PLUS: 
    return compile_expression(t.first(),m_eval);
  case '~': 
    throw Exception("unary not is not supported yet.");
  case '^': 
    throw Exception("^ is not currently handled by the JIT compiler");
    break;
  case TOK_DOTPOWER: 
    throw Exception(".^ is not currently handled by the JIT compiler");
    break;
  case '\'': 
    throw Exception("' is not currently handled by the JIT compiler");
    break;
  case TOK_DOTTRANSPOSE: 
    throw Exception(".' is not currently handled by the JIT compiler");
    break;
  case '@':
    throw Exception("@ is not currently handled by the JIT compiler");
  default:
    std::cout << "******************************************************\r\n";
    t.print();
    throw Exception("Unrecognized expression!");
  }
}

void JITVM::compile_statement_type(tree t, Interpreter *m_eval) {
  switch(t.token()) {
  case '=': 
    compile_assignment(t,m_eval);
    break;
  case TOK_MULTI:
    throw Exception("multi function calls do not JIT compile");
  case TOK_SPECIAL:
    throw Exception("special function calls do not JIT compile");
  case TOK_FOR:
    compile_for_block(t,m_eval);
    break;
  case TOK_WHILE:
    throw Exception("nested while loops do not JIT compile");
  case TOK_IF:
    compile_if_statement(t,m_eval);
    break;
  case TOK_BREAK:
    throw Exception("break is not currently handled by the JIT compiler");
    break;
  case TOK_CONTINUE:
    throw Exception("continue is not currently handled by the JIT compiler");
    break;
  case TOK_DBSTEP:
    throw Exception("dbstep is not currently handled by the JIT compiler");
    break;
  case TOK_DBTRACE:
    throw Exception("dbtrace is not currently handled by the JIT compiler");
    break;
  case TOK_RETURN:
    throw Exception("return is not currently handled by the JIT compiler");
    break;
  case TOK_SWITCH:
    throw Exception("switch is not currently handled by the JIT compiler");
    break;
  case TOK_TRY:
    throw Exception("try is not currently handled by the JIT compiler");
    break;
  case TOK_QUIT:
    throw Exception("quit is not currently handled by the JIT compiler");
    break;
  case TOK_RETALL:
    throw Exception("retall is not currently handled by the JIT compiler");
    break;
  case TOK_KEYBOARD:
    throw Exception("keyboard is not currently handled by the JIT compiler");
    break;
  case TOK_GLOBAL:
    throw Exception("global is not currently handled by the JIT compiler");
    break;
  case TOK_PERSISTENT:
    throw Exception("persistent is not currently handled by the JIT compiler");
    break;
  case TOK_EXPR:
    compile_expression(t.first(),m_eval);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void JITVM::compile_statement(tree t, Interpreter *m_eval) {
  if (t.is(TOK_STATEMENT) && 
      (t.first().is(TOK_EXPR) || t.first().is(TOK_SPECIAL) ||
       t.first().is(TOK_MULTI) || t.first().is('=')))
    throw Exception("JIT compiler doesn't work with verbose statements");
  compile_statement_type(t.first(),m_eval);
}

void JITVM::compile_block(tree t, Interpreter *m_eval) {
  const treeVector &statements(t.children());
  for (treeVector::const_iterator i=statements.begin();
       i!=statements.end();i++) 
    compile_statement(*i,m_eval);
}

void JITVM::compile_for_block(tree t, Interpreter *m_eval) {
  if (!(t.first().is('=') && t.first().second().is(':') &&
	t.first().second().first().is(TOK_INTEGER) &&
	t.first().second().second().is(TOK_INTEGER))) 
    throw Exception("For loop cannot be compiled - need integer bounds");
  string loop_start(t.first().second().first().text());
  string loop_stop(t.first().second().second().text());
  string loop_index(t.first().first().text());
  // Allocate a slot for the loop index register
  JITScalar loop_index_address = new AllocaInst(IntegerType::get(32), 
						loop_index, func_prolog);
  symbols.insertSymbol(loop_index,JITSymbolInfo(loop_index_address));
  new StoreInst(ConstantInt::get(APInt(32, loop_start, 10)), loop_index_address, 
		false, ip);
  BasicBlock *loopbody = new BasicBlock("for_body",func,0);
  BasicBlock *looptest = new BasicBlock("for_test",func,0);
  BasicBlock *loopexit = new BasicBlock("for_exit",func,0);
  new BranchInst(looptest, ip);
  // Create 3 blocks
  ip = loopbody;
  compile_block(t.second(),m_eval);
  JITScalar loop_index_value = new LoadInst(loop_index_address, "", false, loopbody);
  JITScalar next_loop_value = BinaryOperator::create(Instruction::Add,
						     loop_index_value,
						     ConstantInt::get(APInt(32, "1", 10)),
						     "", loopbody);
  new StoreInst(next_loop_value, loop_index_address, false, loopbody);
  new BranchInst(looptest, loopbody);

  loop_index_value = new LoadInst(loop_index_address, "", false, looptest);
  JITScalar loop_comparison = new ICmpInst(ICmpInst::ICMP_SLT, loop_index_value,
					   ConstantInt::get(APInt(32, loop_stop, 10)), 
					   "", looptest);
  new BranchInst(loopbody, loopexit, loop_comparison, looptest);
  ip = loopexit;
}

void JITVM::compile(tree t, Interpreter *m_eval) {
  // The signature for the compiled function should be:
  // int func(void** inputs);
  Module *M = new Module("test");
  std::vector<const Type*> FuncTy_0_args;
  PointerType* void_pointer = PointerType::get(IntegerType::get(8));
  PointerType* void_pointer_pointer = PointerType::get(void_pointer);
  FuncTy_0_args.push_back(void_pointer_pointer);
  llvm::FunctionType* FuncTy_0 = llvm::FunctionType::get(IntegerType::get(32),
							 FuncTy_0_args,
							 false,
							 (ParamAttrsList *)0);
  func = new Function(FuncTy_0,
		      GlobalValue::ExternalLinkage,
		      "initArray", M);  
  func->setCallingConv(CallingConv::C);
  ptr_inputs = func->arg_begin();
  ptr_inputs->setName("inputs");
  ip = 0;
  argument_count = 0;
  func_prolog = new BasicBlock("func_prolog",func,0);
  func_body = new BasicBlock("func_body",func,0);
  ip = func_body;
  compile_for_block(t,m_eval);
  func_epilog = new BasicBlock("func_epilog",func,0);
  new BranchInst(func_body,func_prolog);
  new BranchInst(func_epilog,ip);
  new ReturnInst(ConstantInt::get(APInt(32, "0", 10)),func_epilog);
  verifyModule(*M, PrintMessageAction);
  std::cout << *M;
  std::ofstream p("tmp.bc", ofstream::binary);
  WriteBitcodeToFile(M,p);
  p.close();
}

#if 0
int JITtest() {
  Constant* phi_left = Constant::getNullValue(IntegerType::get(32));
  // Get the loop limits
  ConstantInt* loop_start = ConstantInt::get(APInt(32, "0", 10));
  ConstantInt* loop_increment = ConstantInt::get(APInt(32, "1", 10));
  ConstantInt* loop_stop = ConstantInt::get(APInt(32, "1000000", 10));
  // Add A as an argument to the function
  Function::arg_iterator args = F->arg_begin();
  Value* ptr_A = args++;
  ptr_A->setName("A");
  // Add a loop
  BasicBlock* bb_loop_entry = new BasicBlock("",F,0);
  BasicBlock* bb_loop_body = new BasicBlock("",F,0);
  BasicBlock* bb_loop_exit = new BasicBlock("",F,0);
  
  // Check for legal loop limits
  ICmpInst* legal_loop_limits = new ICmpInst(ICmpInst::ICMP_SGT, loop_stop, loop_start, "", bb_loop_entry);
  new BranchInst(bb_loop_body, bb_loop_exit, legal_loop_limits, bb_loop_entry);
  
  // Setup the PHINode
  Argument* fwdref = new Argument(IntegerType::get(32));
  PHINode*  loop_index = new PHINode(IntegerType::get(32), "", bb_loop_body);
  loop_index->reserveOperandSpace(2);
  loop_index->addIncoming(phi_left, bb_loop_entry);
  loop_index->addIncoming(fwdref, bb_loop_body);
  
  // Cast the loop index to a float value
  Value* float_tmp1 = new SIToFPInst(loop_index, Type::FloatTy, "", bb_loop_body);
  Value* ptr_tmp4 = new GetElementPtrInst(ptr_A, loop_index, "",bb_loop_body);
  Value* void_7 = new StoreInst(float_tmp1, ptr_tmp4, bb_loop_body);
  
  // Update the loop index
  Value* loop_next = BinaryOperator::create(Instruction::Add, loop_index, loop_increment, "", bb_loop_body);
  // Compare the loop index to the loop termination
  Value* exit_test = new ICmpInst(ICmpInst::ICMP_EQ, loop_next, loop_stop, "", bb_loop_body);
  new BranchInst(bb_loop_exit, bb_loop_body, exit_test, bb_loop_body);
  
  new ReturnInst(bb_loop_exit);
  
  fwdref->replaceAllUsesWith(loop_next); delete fwdref;

  std::cout << *M;
  
  M->setTargetTriple("i686-pc-linux-gnu");

  ExistingModuleProvider* MP = new ExistingModuleProvider(M);
  ExecutionEngine* EE = ExecutionEngine::create(MP, false);

  {
    float *G = new float[1000000];
    std::vector<GenericValue> args;
    args.push_back(GenericValue(G));
    std::cout << "\r\nSTART\r\n";
    GenericValue gv = EE->runFunction(F,args);
    
    std::cout << "G[92932] = " << G[92932] << "\r\n";
    delete G;
  }
  return 0;
}

int JITmain() {
  // Create some module to put our function into it.
  Module *M = new Module("test");

  // Create the add1 function entry and insert this entry into module M.  The
  // function will have a return type of "int" and take an argument of "int".
  // The '0' terminates the list of argument types.
  Function *Add1F =
    cast<Function>(M->getOrInsertFunction("add1", Type::Int32Ty, Type::Int32Ty,
                                          (Type *)0));

  // Add a basic block to the function. As before, it automatically inserts
  // because of the last argument.
  BasicBlock *BB = new BasicBlock("EntryBlock", Add1F);

  // Get pointers to the constant `1'.
  Value *One = ConstantInt::get(Type::Int32Ty, 1);

  // Get pointers to the integer argument of the add1 function...
  assert(Add1F->arg_begin() != Add1F->arg_end()); // Make sure there's an arg
  Argument *ArgX = Add1F->arg_begin();  // Get the arg
  ArgX->setName("AnArg");            // Give it a nice symbolic name for fun.

  // Create the add instruction, inserting it into the end of BB.
  Instruction *Add = BinaryOperator::createAdd(One, ArgX, "addresult", BB);

  // Create the return instruction and add it to the basic block
  new ReturnInst(Add, BB);

  // Now, function add1 is ready.


  // Now we going to create function `foo', which returns an int and takes no
  // arguments.
  Function *FooF =
    cast<Function>(M->getOrInsertFunction("foo", Type::Int32Ty, (Type *)0));

  // Add a basic block to the FooF function.
  BB = new BasicBlock("EntryBlock", FooF);

  // Get pointers to the constant `10'.
  Value *Ten = ConstantInt::get(Type::Int32Ty, 10);

  // Pass Ten to the call call:
  CallInst *Add1CallRes = new CallInst(Add1F, Ten, "add1", BB);
  Add1CallRes->setTailCall(true);

  // Create the return instruction and add it to the basic block.
  new ReturnInst(Add1CallRes, BB);

  // Now we create the JIT.
  ExistingModuleProvider* MP = new ExistingModuleProvider(M);
  ExecutionEngine* EE = ExecutionEngine::create(MP, false);

  std::cout << "We just constructed this LLVM module:\n\n" << *M;
  std::cout << "\n\nRunning foo: " << std::flush;

  // Call the `foo' function with no arguments:
  std::vector<GenericValue> noargs;
  GenericValue gv = EE->runFunction(FooF, noargs);

  // Import result of execution:
  std::cout << "Result: " << gv.IntVal.toString(10) << "\n";
  return 0;
}

#endif

void JITVM::run(Interpreter *m_eval) {
//   void (*tptr)(JITScalar*,int32,int32) = &add_local<int32>;
//   for (int i=0;i<1000*1000*10;i++) {
//     //    (this->*opcode_table_i[op_add])(0,50,50);
//     tptr(&reg[0],50,50);
//   } 
//  JITtest();
  return;
//   ip = 0;
//   unsigned ops_max = data.size();
//   while (ip < ops_max)
//     dispatch(data[ip++]);
}

