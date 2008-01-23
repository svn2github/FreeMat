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
#include "llvm/Support/Debug.h"
#include <fstream>
#include <iostream>

// We want some basic functions to be available to the JIT
// such as sin, cos, abs, x^n, tano
// we also need division to work.

using namespace llvm;

JIT::JIT() {
  //  llvm::DebugFlag = true;
  m = new Module("test");
  //  m->setTargetTriple("i686-pc-linux-gnu");
  mp = new ExistingModuleProvider(m);
  std::string errorstring;
  ee = ExecutionEngine::create(mp,false,&errorstring);
  std::cerr << "Execution engine: " << errorstring << "\n";
  initialized = false;
  // Create the optimizer thingy
  opt = new FunctionPassManager(mp);
  opt->add(new TargetData(*ee->getTargetData()));
  //  opt->add(new TargetData(m));
  opt->add(createVerifierPass());                  // Verify that input is correct
  opt->add((Pass*)createCFGSimplificationPass());    // Clean up disgusting code
  opt->add((Pass*)createPromoteMemoryToRegisterPass());// Kill useless allocas
  opt->add((Pass*)createInstructionCombiningPass()); // Clean up after IPCP & DAE
  opt->add((Pass*)createCFGSimplificationPass());    // Clean up after IPCP & DAE
  opt->add((Pass*)createTailDuplicationPass());      // Simplify cfg by copying code
  opt->add((Pass*)createInstructionCombiningPass()); // Cleanup for scalarrepl.
  opt->add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
  opt->add((Pass*)createScalarReplAggregatesPass()); // Break up aggregate allocas
  opt->add((Pass*)createInstructionCombiningPass()); // Combine silly seq's
  opt->add((Pass*)createCondPropagationPass());      // Propagate conditionals
  opt->add((Pass*)createTailCallEliminationPass());  // Eliminate tail calls
  opt->add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
  opt->add((Pass*)createReassociatePass());          // Reassociate expressions
  opt->add((Pass*)createLoopRotatePass());
  opt->add((Pass*)createLICMPass());                 // Hoist loop invariants
  opt->add((Pass*)createLoopUnswitchPass());         // Unswitch loops.
  opt->add((Pass*)createInstructionCombiningPass()); // Clean up after LICM/reassoc
  opt->add((Pass*)createIndVarSimplifyPass());       // Canonicalize indvars
  opt->add((Pass*)createLoopUnrollPass());           // Unroll small loops
  opt->add((Pass*)createInstructionCombiningPass()); // Clean up after the unroller
  opt->add((Pass*)createLoadValueNumberingPass());   // GVN for load instructions
  opt->add((Pass*)createGCSEPass());                 // Remove common subexprs
  opt->add((Pass*)createSCCPPass());                 // Constant prop with SCCP
  opt->add((Pass*)createInstructionCombiningPass());
  opt->add((Pass*)createCondPropagationPass());      // Propagate conditionals
  opt->add((Pass*)createDeadStoreEliminationPass()); // Delete dead stores
  opt->add((Pass*)createAggressiveDCEPass());        // SSA based 'Aggressive DCE'
  opt->add((Pass*)createCFGSimplificationPass());    // Merge & remove BBs
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

void JIT::OptimizeCode(JITFunction func) {
  opt->run(*func);
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

JITType JIT::DoubleType() {
  return Type::getPrimitiveType(Type::DoubleTyID);
}

JITType JIT::FloatType() {
  return Type::getPrimitiveType(Type::FloatTyID);
}

JITType JIT::Int8Type() {
  return IntegerType::get(8);
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
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on multiply");
  return BinaryOperator::create(Instruction::Mul,A,B,"",ip);
}

JITScalar JIT::Div(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on division");
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return BinaryOperator::create(Instruction::SDiv,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return BinaryOperator::create(Instruction::FDiv,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to divide instruction");
}

JITScalar JIT::Sub(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on subtraction");
  return BinaryOperator::create(Instruction::Sub,A,B,"",ip);
}

JITScalar JIT::Add(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on addition");
  return BinaryOperator::create(Instruction::Add,A,B,"",ip);
}

JITScalar JIT::And(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on AND");
  return BinaryOperator::create(Instruction::And,A,B,"",ip);
}

JITScalar JIT::Or(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on OR");
  return BinaryOperator::create(Instruction::Or,A,B,"",ip);
}

JITScalar JIT::Xor(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on XOR");
  return BinaryOperator::create(Instruction::Xor,A,B,"",ip);
}

JITScalar JIT::Alloc(JITType T, std::string name) {
  return new AllocaInst(T,name,ip);
}

JITScalar JIT::Cast(JITScalar A, JITType T) {
  return CastInst::create(CastInst::getCastOpcode(A,true,T,true),A,T,"",ip);
}

JITScalar JIT::LessThan(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on <");
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_SLT,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OLT,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to less than instruction");
}

JITScalar JIT::LessEquals(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on <=");
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_SLE,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OLE,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to less-equal instruction");  
}

JITScalar JIT::Equals(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on ==");
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_EQ,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OEQ,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to equal instruction");  
}

JITScalar JIT::NotEqual(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on !=");
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_NE,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_ONE,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to not-equal instruction");  
}

JITScalar JIT::GreaterThan(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on >");
  if (A->getType()->isInteger() && B->getType()->isInteger()) 
    return new ICmpInst(ICmpInst::ICMP_SGT,A,B,"",ip);
  else if (A->getType()->isFloatingPoint() && B->getType()->isFloatingPoint())
    return new FCmpInst(FCmpInst::FCMP_OGT,A,B,"",ip);
  else
    throw Exception("Unsupported type combinations to greater than instruction");
}

JITScalar JIT::GreaterEquals(JITScalar A, JITScalar B) {
  if (TypeOf(A) != TypeOf(B))
    throw Exception("Type mismatch on >=");
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

JITScalar JIT::String(string text) {
  std::cout << "Allocate string :" << text << ":\n";
  ArrayType* ty = ArrayType::get(IntegerType::get(8),text.size()+1);
  GlobalVariable* gv = new GlobalVariable(ty,true,
					  GlobalValue::InternalLinkage,0,".str",m);
  Constant* t_str = ConstantArray::get(text.c_str(),true);
  std::vector<Constant*> const_ptr_8_indices;
  Constant* const_int32_9 = Constant::getNullValue(IntegerType::get(32));
  const_ptr_8_indices.push_back(const_int32_9);
  const_ptr_8_indices.push_back(const_int32_9);
  Constant* const_ptr_8 = ConstantExpr::getGetElementPtr(gv, &const_ptr_8_indices[0], const_ptr_8_indices.size() );
  gv->setInitializer(t_str);
  return const_ptr_8;
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
  JITFunction func = (JITFunction) m->getOrInsertFunction(name, functype);
  //  JITFunction func = new Function(functype,GlobalValue::ExternalLinkage,name,m);
  //  func->setLinkage(GlobalValue::ExternalLinkage);
  //  func->setCallingConv(CallingConv::C);
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
  case 'c':
    return Int8Type();
  case 'I':
    return PointerType(Int32Type());
  case 'F':
    return PointerType(FloatType());
  case 'D':
    return PointerType(DoubleType());
  case 'C':
    return PointerType(Int8Type());
  default:
    throw Exception("Invalid type map code");
  }
}

JITFunction JIT::DefineLinkFunction(std::string name, std::string rettype, std::string args) {
  std::vector<JITType> argv;
  for (int i=0;i<args.size();i++)
    argv.push_back(MapTypeCode(args[i]));
  JITFunctionType ty = FunctionType(MapTypeCode(rettype[0]),argv);
  JITFunction fn = DefineFunction(ty,name);
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

void JIT::Dump(JITFunction f) {
  std::cout << (*f);
}

void JIT::Dump() {
  std::cout << (*m);
}

void JIT::Dump( const std::string& fname ) {
    std::ofstream fout( fname.c_str() );
    fout << (*m);
}

void JIT::Dump( const std::string& fname, JITFunction f ) {
    std::ofstream fout( fname.c_str() );
    fout << (*f);
}

JITGeneric JIT::Invoke(JITFunction f, JITGeneric arg) {
  std::vector<JITGeneric> args;
  args.push_back(arg);
  return ee->runFunction(f,args);
}

#endif
