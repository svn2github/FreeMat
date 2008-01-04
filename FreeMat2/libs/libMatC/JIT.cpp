// Just for fun, mind you....
//

#include "JIT.hpp"
#ifdef HAVE_LLVM
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Target/TargetData.h"
#include <fstream>
#include <iostream>

// We want some basic functions to be available to the JIT
// such as sin, cos, abs, x^n, tan
// we also need division to work.

using namespace llvm;

JIT::JIT() {
  m = new Module("test");
  m->setTargetTriple("i686-pc-linux-gnu");
  mp = new ExistingModuleProvider(m);
  ee = ExecutionEngine::create(mp,false);
  initialized = false;
}

JIT::~JIT() {
  delete m;
  delete mp;
  delete ee;
}

bool JIT::Initialized() {
  return initialized;
}

void JIT::SetInitialized(bool t) {
  initialized = t;
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

#endif
