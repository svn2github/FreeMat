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

JITScalar JITVM::boolean_op(opcode op, JITScalar arg1, JITScalar arg2, string inst) {
  arg1 = promote(arg1,c_bool);
  arg2 = promote(arg2,c_bool);
  JITScalar result(new_register(c_bool));
  push_instruction(op,type_b,result,compute_ri_code(arg1,arg2),arg1,arg2);
  return result;
}

bool JITVM::isp(JITScalar x) {
  return (type_of(x) == c_pointer);
}
  
bool JITVM::isi(JITScalar x) {
  return (type_of(x) == c_int32);
}

bool JITVM::isfd(JITScalar x) {
  return ((type_of(x) == c_float) || (type_of(x) == c_double));
}

// add immediate to immediate - this instruction can be replaced by 
// constant folding, but for now, leave it in.
JITScalar JITVM::binary_op(opcode op, JITScalar arg1, JITScalar arg2, string inst) {
  scalar_class outClass;
  outClass = type_of(arg1);
  if (type_of(arg2) > outClass) outClass = type_of(arg2);
  if ((isi(arg1) && isfd(arg2)) || (isi(arg2) && isfd(arg1)))  outClass = c_double;
  if (outClass == c_bool) outClass = c_int32;
  if (outClass >= c_pointer) throw Exception("invalid type argument to " + inst + " instruction");
  arg1 = promote(arg1,outClass);
  arg2 = promote(arg2,outClass);
  JITScalar result(new_register(outClass));
  push_instruction(op,compute_oc_code(outClass),result,compute_ri_code(arg1,arg2),arg1,arg2);
  return result;
}

JITScalar JITVM::comparison_op(opcode op, JITScalar arg1, JITScalar arg2, string inst) {
  scalar_class outClass;
  outClass = type_of(arg1);
  if (type_of(arg2) > outClass) outClass = type_of(arg2);
  if (outClass == c_bool) outClass = c_int32;
  JITScalar result(new_register(c_bool));
  push_instruction(op,compute_oc_code(outClass),result,compute_ri_code(arg1,arg2),arg1,arg2);
  return result;

}
#endif

void JITVM::compile_assignment(tree t, Interpreter* m_eval) {
#if 0
  tree s(t.first());
  string symname(s.first().text());
  JITScalar *v = find_symbol(symname);
  if (!v)
    v = add_argument(symname,m_eval,s.numchildren() == 1);
  JITScalar rhs(compile_expression(t.second(),m_eval));
  if (s.numchildren() == 1) {
    if (type_of(*v) != type_of(rhs))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!isscalar(*v))
      throw Exception("scalar assignment to array variable.");
    push_instruction(op_set,compute_oc_code(type_of(*v)),*v,compute_ri_code(rhs),rhs);
    return;
  }
  if (s.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (isscalar(*v))
    throw Exception("array indexing of scalar values...");
  tree q(s.second());
  if (!q.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q.numchildren() == 1) {
    if (map_array_type(v->p()) != type_of(rhs))
      throw Exception("polymorphic assignment to array detected");
    JITScalar arg1 = promote(compile_expression(q.first(),m_eval),c_int32);
    push_instruction(op_vstore,compute_oc_code(type_of(rhs)),*v,
		     compute_ri_code(arg1,rhs),arg1,rhs);
  } else if (s.numchildren() == 2) {
    if (map_array_type(v->p()) != type_of(rhs))
      throw Exception("polymorphic assignment to array detected");
    JITScalar arg1 = promote(compile_expression(q.first(),m_eval),c_int32);
    JITScalar arg2 = promote(compile_expression(q.second(),m_eval),c_int32);
    push_instruction(op_mset,type_b,JITScalar(),compute_ri_code(arg1,arg2),arg1,arg2);
    push_instruction(op_mstore,compute_oc_code(type_of(rhs)),*v,
		     compute_ri_code(rhs,rhs),rhs);
  }
#endif
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

#if 0
JITScalar* JITVM::add_argument(string name, Interpreter* m_eval, bool scalar) {
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
  scalar_class oclass = map_array_type((void*)a);
  if (scalar) {
    // We use a register to 
    JITScalar result(new_register(oclass));
    add_symbol(name,result);
    JITScalar arg1;
    switch (oclass) {
    case c_bool:
      arg1 = JITScalar((bool) ((const logical*)(a->getDataPointer()))[0]);
      break;
    case c_int32:
      arg1 = JITScalar((int32) ArrayToInt32(*a));
      break;      
    case c_float:
      arg1 = JITScalar((float) ArrayToDouble(*a));
      break;
    case c_double:
      arg1 = JITScalar((double) ArrayToDouble(*a));
      break;
    }
    push_instruction(op_set,compute_oc_code(oclass),result,
		     compute_ri_code(arg1,arg1),arg1,arg1);
  } else 
    add_symbol(name,JITScalar((void*) a));
  return find_symbol(name);
}
#endif

JITScalar JITVM::compile_rhs(tree t, Interpreter* m_eval) {
#if 0
  string symname(t.first().text());
  JITScalar *v = find_symbol(symname);
  if (!v)
    v = add_argument(symname,m_eval,t.numchildren() == 1);
  if (t.numchildren() == 1) {
    if (v->isp())
      throw Exception("non-scalar reference returned in scalar context!");
    return *v;
  }
  if (t.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (!v->isp())
    throw Exception("array indexing of scalar values...");
  tree s(t.second());
  if (!s.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s.numchildren() == 1) {
    scalar_class output_type(map_array_type(v->p()));
    JITScalar result(new_register(output_type));
    JITScalar arg1 = promote(compile_expression(s.first(),m_eval),c_int32);
    push_instruction(op_vload,compute_oc_code(output_type),
		     result,compute_ri_code(*v,arg1),*v,arg1);
    return result;
  } else if (s.numchildren() == 2) {
    scalar_class output_type(map_array_type(v->p()));
    JITScalar result(new_register(output_type));
    JITScalar arg1 = promote(compile_expression(s.first(),m_eval),c_int32);
    JITScalar arg2 = promote(compile_expression(s.second(),m_eval),c_int32);
    push_instruction(op_mset,type_b,JITScalar(),compute_ri_code(arg1,arg2),arg1,arg2);
    push_instruction(op_mload,compute_oc_code(output_type),
		     result,compute_ri_code(*v,*v),*v);
    return result;
  }
  throw Exception("dereference not handled yet...");
#endif
}

JITScalar JITVM::compile_expression(tree t, Interpreter* m_eval) {
#if 0
  switch(t.token()) {
  case TOK_VARIABLE: 
    return compile_rhs(t,m_eval);
  case TOK_INTEGER:
    return JITScalar(ArrayToInt32(t.array()));
  case TOK_FLOAT:
    return JITScalar((float) ArrayToDouble(t.array()));
  case TOK_DOUBLE:
    return JITScalar((double) ArrayToDouble(t.array()));
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF: 
    throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case '+': 
    return binary_op(op_add,compile_expression(t.first(),m_eval),
		     compile_expression(t.second(),m_eval),"add");
  case '-': 
    return binary_op(op_sub,compile_expression(t.first(),m_eval),
		     compile_expression(t.second(),m_eval),"sub");
  case '*': 
  case TOK_DOTTIMES: 
    return binary_op(op_mul,compile_expression(t.first(),m_eval),
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
  case TOK_SOR: 
  case '|':
    return boolean_op(op_or,compile_expression(t.first(),m_eval),
		      compile_expression(t.second(),m_eval),"or");
  case TOK_SAND: 
  case '&': 
    return boolean_op(op_and,compile_expression(t.first(),m_eval),
		      compile_expression(t.second(),m_eval),"and");
  case '<': 
    return comparison_op(op_lt,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"lt");
  case TOK_LE: 
    return comparison_op(op_le,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"le");
  case '>': 
    return comparison_op(op_gt,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"gt");
  case TOK_GE: 
    return comparison_op(op_ge,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"ge");
  case TOK_EQ: 
    return comparison_op(op_eq,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"eq");
  case TOK_NE: 
    return comparison_op(op_ne,compile_expression(t.first(),m_eval),
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
#endif
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
  // Create 4 blocks
  BasicBlock *loopentry = new BasicBlock("for_entry",func,ip);
  BasicBlock *loopbody = new BasicBlock("for_body",func,ip);
  BasicBlock *looptest = new BasicBlock("for_test",func,ip);
  BasicBlock *loopexit = new BasicBlock("for_exit",func,ip);
  // Allocate a slot for the loop index register
  JITScalar loop_index_address = new AllocaInst(IntegerType::get(32), 
						loop_index, loopentry);
  symbols.insertSymbol(loop_index,loop_index_address);
  new StoreInst(ConstantInt::get(APInt(32, loop_start, 10)), loop_index_address, 
		false, loopentry);
  new BranchInst(looptest, loopentry);
  compile_block(loopbody,t.second(),m_eval);
  new BranchInst(looptest, loopbody);
  JITScalar loop_index_value = new LoadInst(loop_index_address, "", false, looptest);
  JITScalar loop_comparison = new ICmpInst(ICmpInst::ICMP_SLT, loop_index_value,
					   ConstantInt::get(APInt(32, loop_stop, 10)), 
					   "", looptest);
  new BranchInst(loopbody, loopexit, loop_comparison, looptest);
  ip = loopexit;
}

void JITVM::compile(tree t, Interpreter *m_eval) {

  Module *M = new Module("test");
  std::vector<const Type*> FuncTy_0_args;
  PointerType* Float_PointerTy = PointerType::get(Type::FloatTy);
  FuncTy_0_args.push_back(Float_PointerTy);
  llvm::FunctionType* FuncTy_0 = llvm::FunctionType::get(Type::VoidTy,FuncTy_0_args,false,(ParamAttrsList *)0);
  func = new Function(FuncTy_0,
		      GlobalValue::ExternalLinkage,
		      "initArray", M);
  
  func->setCallingConv(CallingConv::C);
  ip = 0;
  compile_for_block(t,m_eval);
  new ReturnInst(ip);
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

