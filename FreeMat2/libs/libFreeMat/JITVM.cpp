// Still need:
//
//  Function calls - 
//   The most general approach would be to just bridge calls to built in functions.
//   But that seems suboptimal.
//
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

// List of reasonable functions to JIT
//
// sec  - done
// csc  - done
// tan  - done
// atan - done
// cot  - done
// exp  - done
// expm1 - done
// ceil - done
// floor - done
// round - done
// pi, e - done
// float
// single
// double
// int32
// nan/NaN/inf/Inf
// IsNaN/isnan/IsInf/isinf
// eps/feps
//
//  What happens with something like:
//    if (t>0)
//      pi = 3.5;
//    else
//      a = pi;
//    end
//  In this case, we assign to PI in the first block, and
//  read from it in the second.  This should be avoided - 
//  it will not compile correctly -- assignment to a variable 
//  that has the name of a JIT function should disable the JIT
//  mechanism

#include "JITVM.hpp"
#ifdef HAVE_LLVM
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#if 1
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/LinkAllPasses.h"
#endif
#include "llvm/Target/TargetData.h"
#include <fstream>

// We want some basic functions to be available to the JIT
// such as sin, cos, abs, x^n, tan
// we also need division to work.

using namespace llvm;

JIT::JIT() {
  m = new Module("test");
  mp = new ExistingModuleProvider(m);
  ee = ExecutionEngine::create(mp,false);
}

void JIT::OptimizeCode() {
  PassManager PM;
  PM.add(new TargetData(m));
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
  PM.run(*m);
}

JITFunctionType JIT::FunctionType(JITType rettype, std::vector<JITType> args) {
  return llvm::FunctionType::get(rettype,args,false,(ParamAttrsList*) 0);
}

JITFunctionType JIT::FunctionType(std::string rettype, std::string args) {
  std::vector<JITType> argv;
  for (int i=0;i<args.size();i++)
    argv.push_back(MapTypeCode(args[i]));
  JITFunctionType ty = FunctionType(MapTypeCode(rettype[0]),argv);
  return ty;
}

void JIT::LinkFunction(JITFunction func, void* address) {
  ee->addGlobalMapping(func,address);
}

JITType JIT::DoubleType() {
  return Type::getPrimitiveType(Type::DoubleTyID);
}

JITType JIT::FloatType() {
  return Type::getPrimitiveType(Type::FloatTyID);
}

JITType JIT::Int32Type() {
  return IntegerType::get(32);
}

JITType JIT::BoolType() {
  return IntegerType::get(1);
}

JITType JIT::PointerType(JITType t) {
  return PointerType::get(t);
}

JITType JIT::VoidType() {
  return Type::VoidTy;
}

JITType JIT::TypeOf(JITScalar x) {
  return x->getType();
}

JITScalar JIT::Int32Value(int32 x) {
  return ConstantInt::get(Type::Int32Ty,x);
}

JITScalar JIT::DoubleValue(double x) {
  return ConstantFP::get(Type::DoubleTy,x);
}

JITScalar JIT::FloatValue(float x) {
  return ConstantFP::get(Type::FloatTy,x);
}

JITScalar JIT::BoolValue(bool t) {
  if (t)
    return ConstantInt::get(Type::Int1Ty,1);
  else
    return ConstantInt::get(Type::Int1Ty,0);
}

JITScalar JIT::Zero(JITType t) {
  return Constant::getNullValue(t);
}

JITBlock JIT::NewBlock(std::string name) {
  return new BasicBlock(name,func,0);
}

JITScalar JIT::Mul(JITScalar A, JITScalar B) {
  return BinaryOperator::create(Instruction::Mul,A,B,"",ip);
}

JITScalar JIT::Div(JITScalar A, JITScalar B) {
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return BinaryOperator::create(Instruction::SDiv,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return BinaryOperator::create(Instruction::FDiv,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to divide instruction");
}

JITScalar JIT::Sub(JITScalar A, JITScalar B) {
  return BinaryOperator::create(Instruction::Sub,A,B,"",ip);
}

JITScalar JIT::Add(JITScalar A, JITScalar B) {
  return BinaryOperator::create(Instruction::Add,A,B,"",ip);
}

JITScalar JIT::And(JITScalar A, JITScalar B) {
  return BinaryOperator::create(Instruction::And,A,B,"",ip);
}

JITScalar JIT::Or(JITScalar A, JITScalar B) {
  return BinaryOperator::create(Instruction::Or,A,B,"",ip);
}

JITScalar JIT::Xor(JITScalar A, JITScalar B) {
  return BinaryOperator::create(Instruction::Xor,A,B,"",ip);
}

JITScalar JIT::Alloc(JITType T, std::string name) {
  return new AllocaInst(T,name,ip);
}

JITScalar JIT::Cast(JITScalar A, JITType T) {
  return CastInst::create(CastInst::getCastOpcode(A,true,T,true),A,T,"",ip);
}

JITScalar JIT::LessThan(JITScalar A, JITScalar B) {
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_SLT,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OLT,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to less than instruction");
}

JITScalar JIT::LessEquals(JITScalar A, JITScalar B) {
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_SLE,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OLE,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to less-equal instruction");  
}

JITScalar JIT::Equals(JITScalar A, JITScalar B) {
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_EQ,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OEQ,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to equal instruction");  
}

JITScalar JIT::NotEqual(JITScalar A, JITScalar B) {
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_NE,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_ONE,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to not-equal instruction");  
}

JITScalar JIT::GreaterThan(JITScalar A, JITScalar B) {
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_SGT,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OGT,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to greater than instruction");
}

JITScalar JIT::GreaterEquals(JITScalar A, JITScalar B) {
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_SGE,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OGE,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to greater-equal instruction");  
}

void JIT::Store(JITScalar Value, JITScalar Address) {
  new StoreInst(Value, Address, ip);
}

JITScalar JIT::Load(JITScalar Address) {
  return new LoadInst(Address,"",false,ip);
}

void JIT::Jump(JITBlock B) {
  new BranchInst(B,ip);
}

void JIT::Branch(JITBlock IfTrue, JITBlock IfFalse, JITScalar TestValue) {
  new BranchInst(IfTrue,IfFalse,TestValue,ip);
}

void JIT::SetCurrentBlock(JITBlock B) {
  ip = B;
}

JITBlock JIT::CurrentBlock() {
  return ip;
}

JITScalar JIT::Call(JITFunction F, std::vector<JITScalar> args) {
  CallInst *t = new CallInst(F,args.begin(),args.end(),"",ip);
  t->setTailCall(false);
  return t;
}

JITScalar JIT::Call(JITFunction F, JITScalar arg1) {
  std::vector<JITScalar> args;
  args.push_back(arg1);
  return Call(F,args);
}

JITScalar JIT::Call(JITFunction F, JITScalar arg1, JITScalar arg2) {
  std::vector<JITScalar> args;
  args.push_back(arg1);
  args.push_back(arg2);
  return Call(F,args);
}

JITScalar JIT::Call(JITFunction F, JITScalar arg1, JITScalar arg2, JITScalar arg3) {
  std::vector<JITScalar> args;
  args.push_back(arg1);
  args.push_back(arg2);
  args.push_back(arg3);
  return Call(F,args);
}

JITScalar JIT::Call(JITFunction F, JITScalar arg1, JITScalar arg2, 
		    JITScalar arg3, JITScalar arg4) {
  std::vector<JITScalar> args;
  args.push_back(arg1);
  args.push_back(arg2);
  args.push_back(arg3);
  args.push_back(arg4);
  return Call(F,args);
}

JITScalar JIT::Call(JITFunction F, JITScalar arg1, JITScalar arg2, 
		    JITScalar arg3, JITScalar arg4, JITScalar arg5) {
  std::vector<JITScalar> args;
  args.push_back(arg1);
  args.push_back(arg2);
  args.push_back(arg3);
  args.push_back(arg4);
  args.push_back(arg5);
  return Call(F,args);
}

JITScalar JIT::GetElement(JITScalar BaseAddress, JITScalar Offset) {
  return new GetElementPtrInst(BaseAddress,Offset,"",ip);
}

JITFunction JIT::DefineFunction(JITFunctionType functype, std::string name) {
  JITFunction func = new Function(functype,GlobalValue::ExternalLinkage,name,m);
  func->setCallingConv(CallingConv::C);
  return func;
}

JITType JIT::MapTypeCode(char c) {
  switch (c) {
  case 'v':
    return VoidType();
  case 'i':
    return Int32Type();
  case 'f':
    return FloatType();
  case 'd':
    return DoubleType();
  case 'p':
    return PointerType(Int32Type());
  default:
    throw Exception("Invalid type map code");
  }
}

JITFunction JIT::DefineLinkFunction(std::string name, std::string rettype, std::string args, void *address) {
  std::vector<JITType> argv;
  for (int i=0;i<args.size();i++)
    argv.push_back(MapTypeCode(args[i]));
  JITFunctionType ty = FunctionType(MapTypeCode(rettype[0]),argv);
  JITFunction fn = DefineFunction(ty,name);
  LinkFunction(fn,address);
  return fn;
}

void JIT::SetCurrentFunction(JITFunction f) {
  func = f;
}

JITFunction JIT::CurrentFunction() {
  return func;
}

JITScalar JIT::FunctionArgument(int n, std::string name) {
  Function::arg_iterator args = func->arg_begin();
  for (int i=0;i<n;i++)
    args++;
  JITScalar ret = args;
  ret->setName(name);
  return ret;
}

void JIT::CloseFunction() {
}

bool JIT::IsDouble(JITType t) {
  return t->getTypeID() == Type::DoubleTyID;
}

bool JIT::IsFloat(JITType t) {
  return t->getTypeID() == Type::FloatTyID;
}

bool JIT::IsFP(JITType t) {
  return t->isFloatingPoint();
}

bool JIT::IsInteger(JITType t) {
  return t->isInteger();
}

bool JIT::IsDouble(JITScalar t) {
  return IsDouble(TypeOf(t));
}

bool JIT::IsFloat(JITScalar t) {
  return IsFloat(TypeOf(t));
}

bool JIT::IsFP(JITScalar t) {
  return IsFP(TypeOf(t));
}

bool JIT::IsInteger(JITScalar t) {
  return IsInteger(TypeOf(t));
}

void JIT::Return(JITScalar t) {
  new ReturnInst(t,ip);
}

void JIT::Return() {
  new ReturnInst(NULL,ip);
}

void JIT::Dump() {
  std::cout << (*m);
}

JITGeneric JIT::Invoke(JITFunction f, JITGeneric arg) {
  std::vector<JITGeneric> args;
  args.push_back(arg);
  return ee->runFunction(f,args);
}

double csc(double t) {
  return 1.0/sin(t);
}

float cscf(float t) {
  return 1.0f/sinf(t);
}

double sec(double t) {
  return 1.0/cos(t);
}

float secf(float t) {
  return 1.0f/cosf(t);
}

double cot(double t) {
  return 1.0/tan(t);
}

float cotf(float t) {
  return 1.0f/tanf(t);
}

static JITType array_dereference(JITType t) {
  const PointerType* p = dynamic_cast<const PointerType*>(t);
  if (!p) throw Exception("Expected pointer type in argument to array_dereference");
  return p->getElementType();
}

void JITVM::compile_assignment(Tree* t, Interpreter* m_eval) {
  Tree* s(t->first());
  string symname(s->first()->text());
  JITScalar rhs(compile_expression(t->second(),m_eval));
  JITSymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (s->numChildren() == 1)
      v = add_argument_scalar(symname,m_eval,rhs,false);
    else
      v = add_argument_array(symname,m_eval);
    if (!v) throw Exception("Undefined variable reference:" + symname);
  }
  if (s->numChildren() == 1) {
    if (v->jit_type != jit->TypeOf(rhs))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!v->is_scalar)
      throw Exception("scalar assignment to array variable.");
    if (jit->IsDouble(rhs))
      jit->Call(func_scalar_store_double, this_ptr, jit->Int32Value(v->argument_index), rhs);
    else if (jit->IsFloat(rhs))
      jit->Call(func_scalar_store_float, this_ptr, jit->Int32Value(v->argument_index), rhs);
    else if (jit->IsInteger(rhs))
      jit->Call(func_scalar_store_int32, this_ptr, jit->Int32Value(v->argument_index), rhs);
    else
      throw Exception("Unsupported type in JIT store");
    return;
  }
  if (s->numChildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->is_scalar)
    throw Exception("array indexing of scalar values...");
  Tree* q(s->second());
  if (!q->is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q->numChildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q->numChildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q->numChildren() == 1) {
    JITScalar arg1 = jit->Cast(compile_expression(q->first(),m_eval),jit->Int32Type());
    if (jit->IsDouble(rhs))
      jit->Call(func_vector_store_double, this_ptr, jit->Int32Value(v->argument_index), arg1, rhs);
    else if (jit->IsFloat(rhs))
      jit->Call(func_vector_store_float, this_ptr, jit->Int32Value(v->argument_index), arg1, rhs);
    else if (jit->IsInteger(rhs))
      jit->Call(func_vector_store_int32, this_ptr, jit->Int32Value(v->argument_index), arg1, rhs);
  } else if (q->numChildren() == 2) {
    JITScalar arg1 = jit->Cast(compile_expression(q->first(),m_eval),jit->Int32Type());
    JITScalar arg2 = jit->Cast(compile_expression(q->second(),m_eval),jit->Int32Type());
    if (jit->IsDouble(rhs))
      jit->Call(func_matrix_store_double, this_ptr, jit->Int32Value(v->argument_index), arg1, arg2, rhs);
    else if (jit->IsFloat(rhs))
      jit->Call(func_matrix_store_float, this_ptr, jit->Int32Value(v->argument_index), arg1, arg2, rhs);
    else if (jit->IsInteger(rhs))
      jit->Call(func_matrix_store_int32, this_ptr, jit->Int32Value(v->argument_index), arg1, arg2, rhs);
  }
}

void JITVM::compile_if_statement(Tree* t, Interpreter* m_eval) {
  JITScalar main_cond = jit->Cast(compile_expression(t->first(),m_eval),jit->BoolType());
  JITBlock if_true = jit->NewBlock("if_true");
  JITBlock if_continue = jit->NewBlock("if_continue");
  JITBlock if_exit = jit->NewBlock("if_exit");
  jit->Branch(if_true,if_continue,main_cond);
  jit->SetCurrentBlock(if_true);
  compile_block(t->second(),m_eval);
  jit->Jump(if_exit);
  int n=2;
  while (n < t->numChildren() && t->child(n)->is(TOK_ELSEIF)) {
    jit->SetCurrentBlock(if_continue);
    JITScalar ttest = jit->Cast(compile_expression(t->child(n)->first(),m_eval),jit->BoolType());
    if_true = jit->NewBlock("elseif_true");
    if_continue = jit->NewBlock("elseif_continue");
    jit->Branch(if_true,if_continue,ttest);
    jit->SetCurrentBlock(if_true);
    compile_block(t->child(n)->second(),m_eval);
    jit->Jump(if_exit);
    n++;
  }
  if (t->last()->is(TOK_ELSE)) {
    jit->SetCurrentBlock(if_continue);
    compile_block(t->last()->first(),m_eval);
    jit->Jump(if_exit);
  } else {
    jit->SetCurrentBlock(if_continue);
    jit->Jump(if_exit);
  }
  jit->SetCurrentBlock(if_exit);
}

JITType JITVM::map_dataclass_type(Class aclass) {
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

JITSymbolInfo* JITVM::add_argument_array(string name, Interpreter* m_eval) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
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
  symbols.insertSymbol(name,JITSymbolInfo(argument_count,false,aclass,map_dataclass_type(aclass)));
  argument_count++;
  return symbols.findSymbol(name);
}

// FIXME - Simplify
JITSymbolInfo* JITVM::add_argument_scalar(string name, Interpreter* m_eval, JITScalar val, bool override) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
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
  symbols.insertSymbol(name,JITSymbolInfo(argument_count,true,aclass,map_dataclass_type(aclass)));
  argument_count++;
  return symbols.findSymbol(name);
}

JITScalar JITVM::compile_scalar_function(string symname, Interpreter* m_eval) {
#if 0
  // Look up the function in the set of scalars
  JITScalar *val = JITScalars.findSymbol(symname);
  if (!val) throw Exception("No JIT version of function " + symname);
  return *val;
#endif
}

JITScalar JITVM::compile_function_call(Tree* t, Interpreter* m_eval) {
#if 0
  // First, make sure it is a function
  string symname(t->first()->text());
  FuncPtr funcval;
  if (!m_eval->lookupFunction(symname,funcval)) 
    throw Exception("Couldn't find function " + symname);
  if (funcval->type() != FM_BUILT_IN_FUNCTION)
    throw Exception("Can only JIT built in functions - not " + symname);
  if (t->numChildren() != 2) 
    return compile_scalar_function(symname,m_eval);
  // Evaluate the argument
  Tree* s(t->second());
  if (!s->is(TOK_PARENS))
    throw Exception("Expecting function arguments.");
  if (s->numChildren() > 1)
    throw Exception("Cannot JIT functions that take more than one argument");
  if (s->numChildren() == 0) 
    return compile_scalar_function(symname,m_eval);
  else {
    JITScalar arg = compile_expression(s->first(),m_eval);
    // First look up direct functions - also try double arg functions, as type
    // promotion means sin(int32) --> sin(double)
    JITFunc *fun = NULL;
    if (isi(arg)) {
      fun = JITIntFuncs.findSymbol(symname);
      if (!fun) fun = JITDoubleFuncs.findSymbol(symname);
    } else if (isf(arg)) {
      fun = JITFloatFuncs.findSymbol(symname);
      if (!fun) fun = JITDoubleFuncs.findSymbol(symname);
    } else if (isd(arg)) {
      fun = JITDoubleFuncs.findSymbol(symname);
    }
    if (!fun) throw Exception("No JIT version of function " + symname);
    if (!fun->argType) throw Exception("JIT version of function " + symname + " takes no arguments");
    //The function exists and is defined - call it
    return new CallInst(fun->funcAddress,cast(arg,fun->argType,false,ip,""),"",ip);
  }
#endif
}

JITScalar JITVM::compile_rhs(Tree* t, Interpreter* m_eval) {
  string symname(t->first()->text());
  JITSymbolInfo *v = symbols.findSymbol(symname);
  if (!v) {
    if (t->numChildren() == 1)
      v = add_argument_scalar(symname,m_eval);
    else
      v = add_argument_array(symname,m_eval);
    if (!v)
      return compile_function_call(t,m_eval);
  }
  if (t->numChildren() == 1) {
    if (!v->is_scalar)
      throw Exception("non-scalar reference returned in scalar context!");
    if (jit->IsDouble(v->jit_type))
      return jit->Call(func_scalar_load_double, this_ptr, jit->Int32Value(v->argument_index));
    else if (jit->IsFloat(v->jit_type))
      return jit->Call(func_scalar_load_float, this_ptr, jit->Int32Value(v->argument_index));
    else if (jit->IsInteger(v->jit_type))
      return jit->Call(func_scalar_load_int32, this_ptr, jit->Int32Value(v->argument_index));
  }
  if (t->numChildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->is_scalar)
    throw Exception("array indexing of scalar values...");
  Tree* s(t->second());
  if (!s->is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s->numChildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s->numChildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s->numChildren() == 1) {
    JITScalar arg1 = jit->Cast(compile_expression(s->first(),m_eval),jit->Int32Type());
    if (jit->IsDouble(v->jit_type))
      return jit->Call(func_vector_load_double, this_ptr, jit->Int32Value(v->argument_index), arg1);
    else if (jit->IsFloat(v->jit_type))
      return jit->Call(func_vector_load_float, this_ptr, jit->Int32Value(v->argument_index), arg1);
    else if (jit->IsInteger(v->jit_type))
      return jit->Call(func_vector_load_int32, this_ptr, jit->Int32Value(v->argument_index), arg1);
    throw Exception("Unsupported JIT type in Load");
  } else if (s->numChildren() == 2) {
    JITScalar arg1 = compile_expression(s->first(),m_eval);
    JITScalar arg2 = compile_expression(s->second(),m_eval);
    if (jit->IsDouble(v->jit_type))
      return jit->Call(func_matrix_load_double, this_ptr, jit->Int32Value(v->argument_index), arg1, arg2);
    else if (jit->IsFloat(v->jit_type))
      return jit->Call(func_matrix_load_float, this_ptr, jit->Int32Value(v->argument_index), arg1, arg2);
    else if (jit->IsInteger(v->jit_type))
      return jit->Call(func_matrix_load_int32, this_ptr, jit->Int32Value(v->argument_index), arg1, arg2);
    throw Exception("Unsupported JIT type in Load");
  }
  throw Exception("dereference not handled yet...");
}

JITScalar JITVM::compile_expression(Tree* t, Interpreter* m_eval) {
  switch(t->token()) {
  case TOK_VARIABLE:     return compile_rhs(t,m_eval);
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
    return jit->Add(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case '-': 
    return jit->Sub(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case '*': 
  case TOK_DOTTIMES: 
    return jit->Mul(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case '/': 
  case TOK_DOTRDIV:
    return jit->Div(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case '\\': 
  case TOK_DOTLDIV: 
    return jit->Div(compile_expression(t->second(),m_eval),compile_expression(t->first(),m_eval));
#warning shortcut evaluation
  case TOK_SOR: 
  case '|':
    return jit->Or(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case TOK_SAND: 
  case '&': 
    return jit->And(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case '<': 
    return jit->LessThan(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case TOK_LE: 
    return jit->LessEquals(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case '>': 
    return jit->GreaterThan(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case TOK_GE: 
    return jit->GreaterEquals(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case TOK_EQ: 
    return jit->Equals(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case TOK_NE:
    return jit->NotEqual(compile_expression(t->first(),m_eval),compile_expression(t->second(),m_eval));
  case TOK_UNARY_MINUS: 
    {
      JITScalar val(compile_expression(t->first(),m_eval));
      return jit->Sub(jit->Zero(jit->TypeOf(val)),compile_expression(t->first(),m_eval));
    }
  case TOK_UNARY_PLUS: 
    return compile_expression(t->first(),m_eval);
  case '~':
    {
      JITScalar val(compile_expression(t->first(),m_eval));
      return jit->Xor(jit->Cast(val,jit->BoolType()),jit->BoolValue(true));
    }
  case '^':               throw Exception("^ is not currently handled by the JIT compiler");
  case TOK_DOTPOWER:      throw Exception(".^ is not currently handled by the JIT compiler");
  case '\'':              throw Exception("' is not currently handled by the JIT compiler");
  case TOK_DOTTRANSPOSE:  throw Exception(".' is not currently handled by the JIT compiler");
  case '@':               throw Exception("@ is not currently handled by the JIT compiler");
  default:                throw Exception("Unrecognized expression!");
  }
}

void JITVM::compile_statement_type(Tree* t, Interpreter *m_eval) {
  switch(t->token()) {
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
    compile_expression(t->first(),m_eval);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void JITVM::compile_statement(Tree* t, Interpreter *m_eval) {
  if (t->is(TOK_STATEMENT) && 
      (t->first()->is(TOK_EXPR) || t->first()->is(TOK_SPECIAL) ||
       t->first()->is(TOK_MULTI) || t->first()->is('=')))
    throw Exception("JIT compiler doesn't work with verbose statements");
  compile_statement_type(t->first(),m_eval);
}

void JITVM::compile_block(Tree* t, Interpreter *m_eval) {
  const TreeList &statements(t->children());
  for (TreeList::const_iterator i=statements.begin();i!=statements.end();i++) 
    compile_statement(*i,m_eval);
}

void JITVM::compile_for_block(Tree* t, Interpreter *m_eval) {
  if (!(t->first()->is('=') && t->first()->second()->is(':'))) 
    throw Exception("For loop cannot be compiled - need scalar bounds");
  if (t->first()->second()->first()->is(':'))
    throw Exception("For loop does not work with triple format bounds");
  JITScalar loop_start = compile_expression(t->first()->second()->first(),m_eval);
  JITScalar loop_stop = compile_expression(t->first()->second()->second(),m_eval);
  string loop_index(t->first()->first()->text());
  // Allocate a slot for the loop index register
  JITSymbolInfo* v = add_argument_scalar(loop_index,m_eval,loop_start,true);
  if (jit->IsDouble(loop_start))
    jit->Call(func_scalar_store_double, this_ptr, jit->Int32Value(v->argument_index), loop_start);
  else 
    throw Exception("Unsupported loop variable type");
  JITBlock loopbody = jit->NewBlock("for_body");
  JITBlock looptest = jit->NewBlock("for_test");
  JITBlock loopexit = jit->NewBlock("for_exit");
  jit->Jump(looptest);
  // Create 3 blocks
  jit->SetCurrentBlock(loopbody);
  compile_block(t->second(),m_eval);
  JITScalar loop_index_value = jit->Call(func_scalar_load_double, this_ptr, jit->Int32Value(v->argument_index));
  JITScalar next_loop_value = jit->Add(loop_index_value,jit->DoubleValue(1.0));
  jit->Call(func_scalar_store_double, this_ptr, jit->Int32Value(v->argument_index), next_loop_value);
  jit->Jump(looptest);
  jit->SetCurrentBlock(looptest);
  loop_index_value = jit->Call(func_scalar_load_double, this_ptr, jit->Int32Value(v->argument_index));
  JITScalar loop_comparison = jit->LessEquals(loop_index_value,loop_stop);
  jit->Branch(loopbody,loopexit,loop_comparison);
  jit->SetCurrentBlock(loopexit);
}

double JITVM::scalar_load_double(void* base, int argnum) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  return ((double*)this_ptr->array_inputs[argnum]->getDataPointer())[0];
}

float JITVM::scalar_load_float(void* base, int argnum) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  return ((float*)this_ptr->array_inputs[argnum]->getDataPointer())[0];
}

int32 JITVM::scalar_load_int32(void* base, int argnum) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  return ((int32*)this_ptr->array_inputs[argnum]->getDataPointer())[0];
}

void JITVM::scalar_store_double(void* base, int argnum, double rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  ((double*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[0] = rhs;
}

void JITVM::scalar_store_float(void* base, int argnum, float rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  ((float*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[0] = rhs;
}

void JITVM::scalar_store_int32(void* base, int argnum, int32 rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  ((int32*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[0] = rhs;
}

double JITVM::vector_load_double(void* base, int argnum, int32 ndx) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  return ((double*)this_ptr->array_inputs[argnum]->getDataPointer())[ndx-1];
}

float JITVM::vector_load_float(void* base, int argnum, int32 ndx) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  return ((float*)this_ptr->array_inputs[argnum]->getDataPointer())[ndx-1];
}

int32 JITVM::vector_load_int32(void* base, int argnum, int32 ndx) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  return ((int32*)this_ptr->array_inputs[argnum]->getDataPointer())[ndx-1];
}

void JITVM::vector_store_double(void* base, int argnum, int32 ndx, double rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  ((double*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[ndx-1] = rhs;
}

void JITVM::vector_store_float(void* base, int argnum, int32 ndx, float rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  ((float*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[ndx-1] = rhs;
}

void JITVM::vector_store_int32(void* base, int argnum, int32 ndx, int32 rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  ((int32*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[ndx-1] = rhs;
}

double JITVM::matrix_load_double(void* base, int argnum, int32 row, int32 col) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  return ((double*)this_ptr->array_inputs[argnum]->getDataPointer())[(col-1)*rows+row-1];
}

float JITVM::matrix_load_float(void* base, int argnum, int32 row, int32 col) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  return ((float*)this_ptr->array_inputs[argnum]->getDataPointer())[(col-1)*rows+row-1];
}

int32 JITVM::matrix_load_int32(void* base, int argnum, int32 row, int32 col) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  return ((int32*)this_ptr->array_inputs[argnum]->getDataPointer())[(col-1)*rows+row-1];
}

void JITVM::matrix_store_double(void* base, int argnum, int32 row, int32 col, double rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  ((double*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[(col-1)*rows+row-1] = rhs;
}

void JITVM::matrix_store_float(void* base, int argnum, int32 row, int32 col, float rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  ((float*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[(col-1)*rows+row-1] = rhs;
}

void JITVM::matrix_store_int32(void* base, int argnum, int32 row, int32 col, int32 rhs) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  int rows = this_ptr->array_inputs[argnum]->rows();
  ((int32*)this_ptr->array_inputs[argnum]->getReadWriteDataPointer())[(col-1)*rows+row-1] = rhs;
}

#if 0
void JITVM::v_resize(void* base, int argnum, int r_new) { 
  JITVM *this_ptr = static_cast<JITVM*>(base);
  if (!this_ptr) throw Exception("vector resize failed");
  this_ptr->array_inputs[argnum]->vectorResize(r_new);
  this_ptr->args[3*argnum] = (void*) this_ptr->array_inputs[argnum]->getReadWriteDataPointer();
  *((int*)(this_ptr->args[3*argnum+1])) = this_ptr->array_inputs[argnum]->rows();
  *((int*)(this_ptr->args[3*argnum+2])) = this_ptr->array_inputs[argnum]->columns();
}

void JITVM::m_resize(void* base, int argnum, int r_new, int c_new) {
  JITVM *this_ptr = static_cast<JITVM*>(base);
  if (!this_ptr) throw Exception("matrix resize failed");
  Dimensions newDim(r_new,c_new);
  this_ptr->array_inputs[argnum]->resize(newDim);
  this_ptr->args[3*argnum] = (void*) this_ptr->array_inputs[argnum]->getReadWriteDataPointer();
  *((int*)(this_ptr->args[3*argnum+1])) = this_ptr->array_inputs[argnum]->rows();
  *((int*)(this_ptr->args[3*argnum+2])) = this_ptr->array_inputs[argnum]->columns();
}
#endif

void JITVM::compile(Tree* t, Interpreter *m_eval) {
  // The signature for the compiled function should be:
  // int func(void** inputs);
  argument_count = 0;
  jit = new JIT;
  func_scalar_load_int32 = jit->DefineLinkFunction("func_scalar_load_int32","i","pi",(void*)JITVM::scalar_load_int32);
  func_scalar_load_double = jit->DefineLinkFunction("func_scalar_load_double","d","pi",(void*)JITVM::scalar_load_double);
  func_scalar_load_float = jit->DefineLinkFunction("func_scalar_load_float","f","pi",(void*)JITVM::scalar_load_float);
  func_vector_load_int32 = jit->DefineLinkFunction("func_vector_load_int32","i","pii",(void*)JITVM::vector_load_int32);
  func_vector_load_double = jit->DefineLinkFunction("func_vector_load_double","d","pii",(void*)JITVM::vector_load_double);
  func_vector_load_float = jit->DefineLinkFunction("func_vector_load_float","f","pii",(void*)JITVM::vector_load_float);
  func_matrix_load_int32 = jit->DefineLinkFunction("func_matrix_load_int32","i","piii",(void*)JITVM::matrix_load_int32);
  func_matrix_load_double = jit->DefineLinkFunction("func_matrix_load_double","d","piii",(void*)JITVM::matrix_load_double);
  func_matrix_load_float = jit->DefineLinkFunction("func_matrix_load_float","f","piii",(void*)JITVM::matrix_load_float);

  func_scalar_store_int32 = jit->DefineLinkFunction("func_scalar_store_int32","v","pii",(void*)JITVM::scalar_store_int32);
  func_scalar_store_double = jit->DefineLinkFunction("func_scalar_store_double","v","pid",(void*)JITVM::scalar_store_double);
  func_scalar_store_float = jit->DefineLinkFunction("func_scalar_store_float","v","pif",(void*)JITVM::scalar_store_float);
  func_vector_store_int32 = jit->DefineLinkFunction("func_vector_store_int32","v","piii",(void*)JITVM::vector_store_int32);
  func_vector_store_double = jit->DefineLinkFunction("func_vector_store_double","v","piid",(void*)JITVM::vector_store_double);
  func_vector_store_float = jit->DefineLinkFunction("func_vector_store_float","v","piif",(void*)JITVM::vector_store_float);
  func_matrix_store_int32 = jit->DefineLinkFunction("func_matrix_store_int32","v","piiii",(void*)JITVM::matrix_store_int32);
  func_matrix_store_double = jit->DefineLinkFunction("func_matrix_store_double","v","piiid",(void*)JITVM::matrix_store_double);
  func_matrix_store_float = jit->DefineLinkFunction("func_matrix_store_float","v","piiif",(void*)JITVM::matrix_store_float);
  
  func = jit->DefineFunction(jit->FunctionType("v","p"),"main");
  jit->SetCurrentFunction(func);
  jit->SetCurrentBlock(jit->NewBlock("main_block"));
  Function::arg_iterator args = func->arg_begin();
  this_ptr = args;
  this_ptr->setName("this_ptr");
  compile_for_block(t,m_eval);
  jit->Return();

  std::cout << "************************************************************\n";
  std::cout << "*  Before optimization \n";
  jit->Dump();

  //  jit->OptimizeCode();

  std::cout << "************************************************************\n";
  std::cout << "*  After optimization \n";
  jit->Dump();
}

void JITVM::run(Interpreter *m_eval) {
  // Collect the list of arguments
  StringVector argumentList(symbols.getCompletions(""));
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
      array_inputs[v->argument_index] = &(*ptr);
    }
  }
  jit->Invoke(func,JITGeneric((void*) this));
}

#endif
