// Still need:

//  Dynamic array resizing.  The current approach is not the best.  Since we no longer cache the
//  addresses and sizes of the arrays in local variables, the best approach is to have the
//  v_resize function use the actual Array class interface to resize the arrays.  This avoids
//  the problems with memory references and leaks.  On the other hand, it does mean that the
//  v_resize function will have to be a member of the JITVM class.  Or at least, it will have
//  to proxy to such a function.
//
//  Range checking.
//  Type promotion on loop entry (and scalar variables)
//
//
// Just for fun, mind you....
//


#include "JITVM.hpp"
#ifdef HAVE_LLVM
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Target/TargetData.h"
#include <fstream>

using namespace llvm;

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

JITScalar JITVM::compile_boolean_op(Instruction::BinaryOps op, JITScalar arg1, JITScalar arg2, string inst) {
  arg1 = cast(arg1,IntegerType::get(1),false,ip,"");
  arg2 = cast(arg2,IntegerType::get(1),false,ip,"");
  return BinaryOperator::create(op, arg1,arg2,"",ip);
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
  const Type* outType;
  if (arg1->getType() == arg2->getType())
    outType = arg1->getType();
  else if ((isi(arg1) && isfd(arg2)) || (isi(arg2) && isfd(arg1)))
    outType = Type::DoubleTy;
  else if ((isf(arg1) && isd(arg2)) || (isd(arg1) && isf(arg2)))
    outType = Type::DoubleTy;
  arg1 = cast(arg1,outType,true,ip);
  arg2 = cast(arg2,outType,true,ip);
  if (outType->isInteger()) {
    switch (op) {
    default:      throw Exception("Unrecognized comparison op");
    case '<':     return new ICmpInst(ICmpInst::ICMP_SLT,arg1,arg2,"",ip);
    case TOK_LE:  return new ICmpInst(ICmpInst::ICMP_SLE,arg1,arg2,"",ip);
    case TOK_EQ:  return new ICmpInst(ICmpInst::ICMP_EQ,arg1,arg2,"",ip);
    case TOK_GE:  return new ICmpInst(ICmpInst::ICMP_SGE,arg1,arg2,"",ip);
    case '>':     return new ICmpInst(ICmpInst::ICMP_SGT,arg1,arg2,"",ip);
    case TOK_NE:  return new ICmpInst(ICmpInst::ICMP_NE,arg1,arg2,"",ip);
    }
  } else {
    switch (op) {
    default:      throw Exception("Unrecognized comparison op");
    case '<':     return new FCmpInst(FCmpInst::FCMP_OLT,arg1,arg2,"",ip);
    case TOK_LE:  return new FCmpInst(FCmpInst::FCMP_OLE,arg1,arg2,"",ip);
    case TOK_EQ:  return new FCmpInst(FCmpInst::FCMP_OEQ,arg1,arg2,"",ip);
    case TOK_GE:  return new FCmpInst(FCmpInst::FCMP_OGE,arg1,arg2,"",ip);
    case '>':     return new FCmpInst(FCmpInst::FCMP_OGT,arg1,arg2,"",ip);
    case TOK_NE:  return new FCmpInst(FCmpInst::FCMP_ONE,arg1,arg2,"",ip);
    }    
  }
}

void JITVM::compile_assignment(tree t, Interpreter* m_eval) {
  tree s(t.first());
  string symname(s.first().text());
  JITScalar rhs(compile_expression(t.second(),m_eval));
  JITSymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (s.numchildren() == 1)
      v = add_argument_scalar(symname,m_eval,rhs,false);
    else
      v = add_argument_array(symname,m_eval);
  }
  if (s.numchildren() == 1) {
    if (v->data_value->getType() != PointerType::get(rhs->getType()))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!v->is_scalar)
      throw Exception("scalar assignment to array variable.");
    new StoreInst(rhs, v->data_value, ip);
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
    if (v->data_value->getType() != PointerType::get(rhs->getType())) {
      // Handle type promotion.  The only case that I can think of 
      // (with int, float and double) is if the destination array is
      // double, in which case, we can promote the RHS to type double
      const Type* array_type = v->data_value->getType();
      const PointerType* p_array = dynamic_cast<const PointerType*>(array_type);
      if (p_array && p_array->getElementType()->getTypeID() == 
	  Type::DoubleTyID) {
	rhs = cast(rhs,p_array->getElementType(),true,ip,"");
      } else 
	throw Exception("polymorphic assignment to array detected");
    }
    JITScalar arg1 = compile_expression(q.first(),m_eval);
    arg1 = cast(arg1,IntegerType::get(32),false,ip);
    // Add code to check the address against the bounds and resize if necessary
    Value* under_range = new ICmpInst(ICmpInst::ICMP_SLT,arg1,int32_const(1),"",ip);
    BasicBlock *bb1 = new BasicBlock("under_range",func,0);
    BasicBlock *bb2 = new BasicBlock("not_under_range",func,0);
    new BranchInst(bb1,bb2,under_range,ip);
    new StoreInst(int32_const(2),return_val,false,bb1);
    new BranchInst(func_epilog,bb1);
    Value* over_range = new ICmpInst(ICmpInst::ICMP_SGT,
				     arg1,new LoadInst(v->num_length,"",false,bb2),"",bb2); 
    BasicBlock *bb3 = new BasicBlock("need_resize",func,0);
    BasicBlock *bb4 = new BasicBlock("valid_range",func,0);
    new BranchInst(bb3,bb4,over_range,bb2);
    // Need to resize 
    // OK - First, get a pointer to the 
    std::vector<Value*> resize_params;
    resize_params.push_back(this_ptr);
    resize_params.push_back(int32_const(v->argument_index));
    resize_params.push_back(arg1);
    new CallInst(resize_func_ptr, &resize_params[0], 3, "", bb3);
    Value *s = new GetElementPtrInst(ptr_inputs,int32_const(3*v->argument_index),"",bb3);
    s = new LoadInst(s, "", false, bb3);
    v->data_value = cast(s,v->data_value->getType(),false,bb3);
    new BranchInst(bb4,bb3);
    ip = bb4;
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,int32_const(1),"",ip);
    JITScalar address = new GetElementPtrInst(v->data_value, arg1, "", ip);
    new StoreInst(rhs, address, false, ip);
  } else if (q.numchildren() == 2) {
    if (v->data_value->getType() != PointerType::get(rhs->getType()))
      throw Exception("polymorphic assignment to array detected");
    JITScalar arg1 = compile_expression(q.first(),m_eval);
    JITScalar arg2 = compile_expression(q.second(),m_eval);
    arg1 = cast(arg1,IntegerType::get(32),false,ip);
    arg2 = cast(arg2,IntegerType::get(32),false,ip);
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,int32_const(1),"",ip);
    arg2 = BinaryOperator::create(Instruction::Sub,arg2,int32_const(1),"",ip);
    JITScalar lin = BinaryOperator::create(Instruction::Mul,arg2,
					   new LoadInst(v->num_rows,"",false,ip),
					   "",ip);
    lin = BinaryOperator::create(Instruction::Add,lin,arg1,"",ip);
    JITScalar address = new GetElementPtrInst(v->data_value, lin, "", ip);
    new StoreInst(rhs, address, false, ip);
  }
}

void JITVM::compile_if_statement(tree t, Interpreter* m_eval) {
  JITScalar main_cond(cast(compile_expression(t.first(),m_eval),
			   IntegerType::get(1),false,ip,""));
  BasicBlock *if_true = new BasicBlock("if_true",func,0);
  BasicBlock *if_continue = new BasicBlock("if_continue",func,0);
  BasicBlock *if_exit = new BasicBlock("if_exit",func,0);
  new BranchInst(if_true,if_continue,main_cond,ip);
  ip = if_true;
  compile_block(t.second(),m_eval);
  new BranchInst(if_exit,ip);
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    ip = if_continue;
    JITScalar ttest(cast(compile_expression(t.child(n).first(),m_eval),
			 IntegerType::get(1),false,ip,""));
    if_true = new BasicBlock("elseif_true",func,0);
    if_continue = new BasicBlock("elseif_continue",func,0);
    new BranchInst(if_true,if_continue,ttest,ip);
    ip = if_true;
    compile_block(t.child(n).second(),m_eval);
    new BranchInst(if_exit,ip);
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    ip = if_continue;
    compile_block(t.last().first(),m_eval);
    new BranchInst(if_exit,ip);
  } else {
    new BranchInst(if_exit,if_continue);
  }
  ip = if_exit;
}

JITScalar JITVM::bool_const(int32 x) {
  return ConstantInt::get(Type::Int1Ty,x);
}

JITScalar JITVM::int32_const(int32 x) {
  return ConstantInt::get(Type::Int32Ty,x);
}

JITScalar JITVM::cast(JITScalar value, const Type *type, bool sgnd, BasicBlock *where, string name) {
  return CastInst::create(CastInst::getCastOpcode(value,sgnd,type,sgnd),
			  value, type, name, where);
}

JITScalar JITVM::get_input_argument(int arg, BasicBlock* where) {
  Value *s = new GetElementPtrInst(ptr_inputs,int32_const(arg),"",where);
  s = new LoadInst(s,"",false,where);
  return s;
}

void JITVM::copy_value(JITScalar source, JITScalar dest, BasicBlock* where) {
  new StoreInst(new LoadInst(source, "", false, where), dest, false, where);
}

JITSymbolInfo* JITVM::add_argument_array(string name, Interpreter* m_eval) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
  Class aclass = FM_FUNCPTR_ARRAY;
  if (!ptr.valid())
      throw Exception("Undefined (array) variable reference:" + name);
  if (!ptr->is2D())
    throw Exception("Cannot JIT multi-dimensional array:" + name);
  if (ptr->isString() || ptr->isReferenceType())
    throw Exception("Cannot JIT strings or reference types:" + name);
  if (ptr->isComplex())
    throw Exception("Cannot JIT complex arrays:" + name);
  aclass = ptr->dataClass();
  // Map the array class to an llvm type
  Type* ctype(map_dataclass_type(aclass));
  // Allocate local variables for the row, column and pointer to data
  Value* r = new AllocaInst(IntegerType::get(32),name+"_rows",func_prolog);
  Value* c = new AllocaInst(IntegerType::get(32),name+"_cols",func_prolog);
  Value* p = new AllocaInst(PointerType::get(ctype),name+"_data",func_prolog);
  // Get pointers to the argument array elements
  Value* r_arg = cast(get_input_argument(3*argument_count+1,func_prolog),
		      PointerType::get(IntegerType::get(32)),false,func_prolog,name+"_rows_in");
  Value* c_arg = cast(get_input_argument(3*argument_count+2,func_prolog),
		      PointerType::get(IntegerType::get(32)),false,func_prolog,name+"_cols_in");
  Value* p_arg = cast(get_input_argument(3*arguement_count,func_prolog),
		      PointerType::get(ctype),false,func_prolog,name+"_data_in");
  // Initialize the local variables from the argument array
  copy_value(r_arg,r,func_prolog);
  copy_value(c_arg,c,func_prolog);
  copy_value(p_arg,p,func_prolog);

  
  Value* t, *s;
  Value* r_in, *c_in;
  Value* r_val, *c_val;
  Value* r, *c, *l;
  s = new GetElementPtrInst(ptr_inputs,int32_const(3*argument_count+1),"",func_prolog);
  s = new LoadInst(s, "", false, func_prolog);
  r_in = cast(s,PointerType::get(IntegerType::get(32)),false,func_prolog,
	      name+"_rows_in");
  r_val = new LoadInst(r_in, "", false, func_prolog);
  //   r = new AllocaInst(IntegerType::get(32),name+"_rows",func_prolog);
  //   new StoreInst(r_val, r, false, func_prolog);
  //   new StoreInst(new LoadInst(r, "", false, func_prolog), r_in, false, func_epilog);
  s = new GetElementPtrInst(ptr_inputs,int32_const(3*argument_count+2),"",func_prolog);
  s = new LoadInst(s, "", false, func_prolog);
  c_in = cast(s,PointerType::get(IntegerType::get(32)),false,func_prolog,name+"_cols_in");
  c_val = new LoadInst(c_in, "", false, func_prolog);
  //   c = new AllocaInst(IntegerType::get(32),name+"_cols",func_prolog);
  //   new StoreInst(c_val, c, false, func_prolog);
  //   new StoreInst(new LoadInst(c, "", false, func_prolog), c_in, false, func_epilog);
  l = new AllocaInst(IntegerType::get(32),name+"_count",func_prolog);
  new StoreInst(BinaryOperator::create(Instruction::Mul,c_val,r_val,"",func_prolog),
		l,false,func_prolog);
  s = new GetElementPtrInst(ptr_inputs,int32_const(3*argument_count),"",func_prolog);
  
  s = new LoadInst(s, "", false, func_prolog);
  Type* ctype;
  switch (aclass) {
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
    ctype = PointerType::get(IntegerType::get(32)); break;
  case FM_FLOAT:
    ctype = PointerType::get(Type::FloatTy); break;
  case FM_DOUBLE:
    ctype = PointerType::get(Type::DoubleTy); break;
  }
  symbols.insertSymbol(name,JITSymbolInfo(true,argument_count,false,true,
					  r_in,c_in,l,t,aclass,false,ctype));
  argument_count++;
  return symbols.findSymbol(name);
}

JITSymbolInfo* JITVM::add_argument_scalar(string name, Interpreter* m_eval, JITScalar val, bool override) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
  Class aclass = FM_FUNCPTR_ARRAY;
  if (!ptr.valid() || override) {
    if (isi(val))
      aclass = FM_INT32;
    else if (isf(val))
      aclass = FM_FLOAT;
    else if (isd(val))
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
  Value* t, *s;
  Value* r, *c;
  s = new GetElementPtrInst(ptr_inputs,int32_const(3*argument_count),"",func_prolog);
  s = new LoadInst(s, "", false, func_prolog);
  Type* ctype;
  switch (aclass) {
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
    r = cast(s,PointerType::get(IntegerType::get(32)),false,func_prolog,name+"_in");
    t = new AllocaInst(IntegerType::get(32),name,func_prolog);
    break;
  case FM_FLOAT:
    r = cast(s,PointerType::get(Type::FloatTy),false,func_prolog,name);
    t = new AllocaInst(Type::FloatTy,name,func_prolog);
    break;
  case FM_DOUBLE:
    r = cast(s,PointerType::get(Type::DoubleTy),false,func_prolog,name);
    t = new AllocaInst(Type::DoubleTy,name,func_prolog);
    break;
  }
  new StoreInst(new LoadInst(r, "", false, func_prolog), t, false, func_prolog);
  new StoreInst(new LoadInst(t, "", false, func_epilog), r, false, func_epilog);
  symbols.insertSymbol(name,JITSymbolInfo(true,argument_count,true,true,NULL,
					  NULL,NULL,t,aclass,false,ctype));
  argument_count++;
  return symbols.findSymbol(name);
}

JITScalar JITVM::compile_rhs(tree t, Interpreter* m_eval) {
  string symname(t.first().text());
  JITSymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (t.numchildren() == 1)
      v = add_argument_scalar(symname,m_eval);
    else
      v = add_argument_array(symname,m_eval);
  }
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
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,int32_const(1),"",ip);
    Value* under_range = new ICmpInst(ICmpInst::ICMP_SLT,arg1,int32_const(0),"",ip);
    Value* over_range = new ICmpInst(ICmpInst::ICMP_SGE,
				     arg1,
				     new LoadInst(v->num_length,"",false,ip),
				     "",ip);
    Value* out_range = BinaryOperator::create(Instruction::Or,
 					      under_range,over_range,"",ip);
    BasicBlock *bb1 = new BasicBlock("",func,0);
    BasicBlock *bb2 = new BasicBlock("",func,0);
    new BranchInst(bb1, bb2, out_range, ip);
    new StoreInst(int32_const(1),return_val,false,bb1);
    new BranchInst(func_epilog,bb1);
    ip = bb2;
    JITScalar address = new GetElementPtrInst(v->data_value, arg1, "", ip);
    return new LoadInst(address, "", false, ip);
  } else if (s.numchildren() == 2) {
    JITScalar arg1 = compile_expression(s.first(),m_eval);
    JITScalar arg2 = compile_expression(s.second(),m_eval);
    arg1 = cast(arg1,IntegerType::get(32),false,ip);
    arg2 = cast(arg2,IntegerType::get(32),false,ip);
    arg1 = BinaryOperator::create(Instruction::Sub,arg1,int32_const(1),"",ip);
    arg2 = BinaryOperator::create(Instruction::Sub,arg2,int32_const(1),"",ip);
    Value* under_range_1 = new ICmpInst(ICmpInst::ICMP_SLT,arg1,int32_const(0),"",ip);
    Value* under_range_2 = new ICmpInst(ICmpInst::ICMP_SLT,arg2,int32_const(0),"",ip);
    Value* over_range_1 = new ICmpInst(ICmpInst::ICMP_SGE,
				       arg1,
				       new LoadInst(v->num_rows,"",false,ip),
				       "",ip);
    Value* over_range_2 = new ICmpInst(ICmpInst::ICMP_SGE,
				       arg2,
				       new LoadInst(v->num_cols,"",false,ip),
				       "",ip);
    Value* out_range = BinaryOperator::create(Instruction::Or,
					      BinaryOperator::create(Instruction::Or,
								     under_range_1,
								     under_range_2,"",ip),
					      BinaryOperator::create(Instruction::Or,
								     over_range_1,
								     over_range_2,"",ip),
					      "",ip);
    BasicBlock *bb1 = new BasicBlock("",func,0);
    BasicBlock *bb2 = new BasicBlock("",func,0);
    new BranchInst(bb1, bb2, out_range, ip);
    new StoreInst(int32_const(1),return_val,false,bb1);
    new BranchInst(func_epilog,bb1);
    ip = bb2;
    JITScalar lin = BinaryOperator::create(Instruction::Mul,arg2,
					   new LoadInst(v->num_rows,"",false,ip),
					   "",ip);
    lin = BinaryOperator::create(Instruction::Add,lin,arg1,"",ip);
    JITScalar address = new GetElementPtrInst(v->data_value, lin, "", ip);
    return new LoadInst(address, "", false, ip);
  }
  throw Exception("dereference not handled yet...");
}

JITScalar JITVM::compile_expression(tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case TOK_VARIABLE:     return compile_rhs(t,m_eval);
  case TOK_INTEGER:      return int32_const(ArrayToInt32(t.array()));
  case TOK_FLOAT:        return ConstantFP::get(Type::FloatTy,ArrayToDouble(t.array()));
  case TOK_DOUBLE:       return ConstantFP::get(Type::DoubleTy,ArrayToDouble(t.array()));
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF:      throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
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
    {
      JITScalar val(compile_expression(t.first(),m_eval));
      return BinaryOperator::create(Instruction::Sub,
				    Constant::getNullValue(val->getType()),
				    val,"",ip);
    }
  case TOK_UNARY_PLUS: 
    return compile_expression(t.first(),m_eval);
  case '~': 
    {
      JITScalar val(compile_expression(t.first(),m_eval));
      val = cast(val,IntegerType::get(1),false,ip);
      return BinaryOperator::create(Instruction::Xor,val,bool_const(1),"",ip);
    }
  case '^':               throw Exception("^ is not currently handled by the JIT compiler");
  case TOK_DOTPOWER:      throw Exception(".^ is not currently handled by the JIT compiler");
  case '\'':              throw Exception("' is not currently handled by the JIT compiler");
  case TOK_DOTTRANSPOSE:  throw Exception(".' is not currently handled by the JIT compiler");
  case '@':               throw Exception("@ is not currently handled by the JIT compiler");
  default:                throw Exception("Unrecognized expression!");
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
  JITSymbolInfo* v = add_argument_scalar(loop_index,m_eval,ConstantInt::get(APInt(32, loop_start, 10)),true);
  JITScalar loop_index_address = v->data_value;
  new StoreInst(ConstantInt::get(APInt(32, loop_start, 10)), loop_index_address, 
		false, ip);
  BasicBlock *loopbody = new BasicBlock("for_body",func,0);
  BasicBlock *looptest = new BasicBlock("for_test",func,0);
  BasicBlock *loopexit = new BasicBlock("for_exit",func,0);
  new BranchInst(looptest, ip);
  // Create 3 blocks
  ip = loopbody;
  compile_block(t.second(),m_eval);
  JITScalar loop_index_value = new LoadInst(loop_index_address, "", false, ip);
  JITScalar next_loop_value = BinaryOperator::create(Instruction::Add,loop_index_value,int32_const(1),"",ip);
  new StoreInst(next_loop_value, loop_index_address, false, ip);
  new BranchInst(looptest, ip);

  loop_index_value = new LoadInst(loop_index_address, "", false, looptest);
  JITScalar loop_comparison = new ICmpInst(ICmpInst::ICMP_SLE, loop_index_value,
					   ConstantInt::get(APInt(32, loop_stop, 10)), 
					   "", looptest);
  new BranchInst(loopbody, loopexit, loop_comparison, looptest);
  ip = loopexit;
}

// FIXME - Memory leak
void JITVM::v_resize(void* base, int argnum, int r_new) { 
  JITVM *this_ptr = static_cast<JITVM*>(base);
  if (!this_ptr) throw Exception("vector resize failed");
  this_ptr->array_inputs[argnum]->vectorResize(r_new);
  this_ptr->args[3*argnum] = (void*) this_ptr->array_inputs[argnum]->getReadWriteDataPointer();
  *((int*)(this_ptr->args[3*argnum+1])) = this_ptr->array_inputs[argnum]->rows();
  *((int*)(this_ptr->args[3*argnum+2])) = this_ptr->array_inputs[argnum]->columns();
}

void JITVM::compile(tree t, Interpreter *m_eval) {
  // The signature for the compiled function should be:
  // int func(void** inputs);
  M = new Module("test");
  std::vector<const Type*> DispatchFuncArgs;
  PointerType* void_pointer = PointerType::get(IntegerType::get(8));
  PointerType* void_pointer_pointer = PointerType::get(void_pointer);
  PointerType* int32_pointer = PointerType::get(IntegerType::get(32));
  std::vector<const Type*> ResizeFuncArgs;
  ResizeFuncArgs.push_back(void_pointer);
  ResizeFuncArgs.push_back(IntegerType::get(32));
  ResizeFuncArgs.push_back(IntegerType::get(32));
  ResizeFuncTy = llvm::FunctionType::get(Type::VoidTy,
					 ResizeFuncArgs,false,
					 (ParamAttrsList *) 0);
  DispatchFuncArgs.push_back(void_pointer_pointer);
  DispatchFuncArgs.push_back(PointerType::get(ResizeFuncTy));
  DispatchFuncArgs.push_back(void_pointer);
  llvm::FunctionType* DispatchFuncType = llvm::FunctionType::get(IntegerType::get(32),
								 DispatchFuncArgs,
								 false,
								 (ParamAttrsList *)0);
  func = new Function(DispatchFuncType,
		      GlobalValue::ExternalLinkage,
		      "initArray", M);  
  func->setCallingConv(CallingConv::C);
  Function::arg_iterator args = func->arg_begin();
  ptr_inputs = args++;
  ptr_inputs->setName("inputs");
  resize_func_ptr = args++;
  resize_func_ptr->setName("resize_func");
  this_ptr = args++;
  this_ptr->setName("this_ptr");
  ip = 0;
  argument_count = 0;
  func_prolog = new BasicBlock("func_prolog",func,0);
  func_body = new BasicBlock("func_body",func,0);
  func_epilog = new BasicBlock("func_epilog",func,0);
  return_val = new AllocaInst(IntegerType::get(32),"return_code",func_prolog);
  new StoreInst(const_int(0),return_val,false,func_prolog);
  ip = func_body;
  compile_for_block(t,m_eval);
  new BranchInst(func_body,func_prolog);
  new BranchInst(func_epilog,ip);
  new ReturnInst(new LoadInst(return_val, "", false, func_epilog),func_epilog);

  std::cout << (*M);

  if (0) {
  PassManager PM;
  PM.add(new TargetData(M));
  PM.add(createVerifierPass());                  // Verify that input is correct

  PM.add((Pass*)createLowerSetJmpPass());          // Lower llvm.setjmp/.longjmp

  // If the -strip-debug command line option was specified, do it.
  PM.add((Pass*)createRaiseAllocationsPass());     // call %malloc -> malloc inst
  PM.add((Pass*)createCFGSimplificationPass());    // Clean up disgusting code
  PM.add((Pass*)createPromoteMemoryToRegisterPass());// Kill useless allocas
  PM.add((Pass*)createGlobalOptimizerPass());      // Optimize out global vars
  PM.add((Pass*)createGlobalDCEPass());            // Remove unused fns and globs
  PM.add((Pass*)createIPConstantPropagationPass());// IP Constant Propagation
  PM.add((Pass*)createDeadArgEliminationPass());   // Dead argument elimination
  PM.add((Pass*)createInstructionCombiningPass()); // Clean up after IPCP & DAE
  PM.add((Pass*)createCFGSimplificationPass());    // Clean up after IPCP & DAE

  PM.add((Pass*)createPruneEHPass());              // Remove dead EH info

  PM.add((Pass*)createFunctionInliningPass());   // Inline small functions
  PM.add((Pass*)createArgumentPromotionPass());    // Scalarize uninlined fn args

  PM.add((Pass*)createTailDuplicationPass());      // Simplify cfg by copying code
  PM.add((Pass*)createInstructionCombiningPass()); // Cleanup for scalarrepl.
  PM.add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
  PM.add((Pass*)createScalarReplAggregatesPass()); // Break up aggregate allocas
  PM.add((Pass*)createInstructionCombiningPass()); // Combine silly seq's
  PM.add((Pass*)createCondPropagationPass());      // Propagate conditionals

  PM.add((Pass*)createTailCallEliminationPass());  // Eliminate tail calls
  PM.add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
  PM.add((Pass*)createReassociatePass());          // Reassociate expressions
  PM.add((Pass*)createLoopRotatePass());
  PM.add((Pass*)createLICMPass());                 // Hoist loop invariants
  PM.add((Pass*)createLoopUnswitchPass());         // Unswitch loops.
  PM.add((Pass*)createInstructionCombiningPass()); // Clean up after LICM/reassoc
  PM.add((Pass*)createIndVarSimplifyPass());       // Canonicalize indvars
  PM.add((Pass*)createLoopUnrollPass());           // Unroll small loops
  PM.add((Pass*)createInstructionCombiningPass()); // Clean up after the unroller
  PM.add((Pass*)createLoadValueNumberingPass());   // GVN for load instructions
  PM.add((Pass*)createGCSEPass());                 // Remove common subexprs
  PM.add((Pass*)createSCCPPass());                 // Constant prop with SCCP

  // Run instcombine after redundancy elimination to exploit opportunities
  // opened up by them.
  PM.add((Pass*)createInstructionCombiningPass());
  PM.add((Pass*)createCondPropagationPass());      // Propagate conditionals

  PM.add((Pass*)createDeadStoreEliminationPass()); // Delete dead stores
  PM.add((Pass*)createAggressiveDCEPass());        // SSA based 'Aggressive DCE'
  PM.add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
  PM.add((Pass*)createSimplifyLibCallsPass());     // Library Call Optimizations
  PM.add((Pass*)createDeadTypeEliminationPass());  // Eliminate dead types
  PM.add((Pass*)createConstantMergePass());        // Merge dup global constants
  PM.run(*M);
  }
}

void JITVM::run(Interpreter *m_eval) {
  // Collect the list of arguments
  stringVector argumentList(symbols.getCompletions(""));
  // Allocate the argument array
  args = (void**) malloc(sizeof(void*)*argumentList.size()*3);
  // For each argument in the array, retrieve it from the interpreter
  array_inputs.reserve(argumentList.size());
  for (int i=0;i<argumentList.size();i++) {
    JITSymbolInfo* v = symbols.findSymbol(argumentList[i]);
    if (v) {
      ArrayReference ptr(m_eval->getContext()->lookupVariable(argumentList[i]));
      if (!ptr.valid()) {
	if (!v->is_scalar) throw Exception("cannot create array types in the loop");
	m_eval->getContext()->insertVariable(argumentList[i],
					     Array(v->inferred_type,
						   Dimensions(1,1),
						   Array::allocateArray(v->inferred_type,1)));
	ptr = m_eval->getContext()->lookupVariable(argumentList[i]);
	if (!ptr.valid()) throw Exception("unable to create variable " + argumentList[i]);
      }
      args[3*v->argument_index] = (void*) ptr->getReadWriteDataPointer();
      args[3*v->argument_index+1] = (int*) malloc(sizeof(int));
      *((int*)(args[3*v->argument_index+1])) = ptr->rows();
      args[3*v->argument_index+2] = (int*) malloc(sizeof(int));;
      *((int*)(args[3*v->argument_index+2])) = ptr->columns();
      array_inputs[v->argument_index] = &(*ptr);
    }
  }

  //  std::ofstream p("jit.bc");
  //  WriteBitcodeToFile(M,p);
  //  p.close();
  
  ExistingModuleProvider* MP = new ExistingModuleProvider(M);
  ExecutionEngine* EE = ExecutionEngine::create(MP, false);
  std::vector<GenericValue> GVargs;
  GVargs.push_back(GenericValue(args));
  GVargs.push_back(GenericValue((void*) &v_resize));
  GVargs.push_back(GenericValue((void*) this));
  GenericValue gv = EE->runFunction(func,GVargs);
  delete EE;

  if (gv.IntVal == 1)
    throw Exception("Index exceeds variable dimensions");
}

#endif
