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
#include <string>
#include "DebugStream.hpp"

// We want some basic functions to be available to the JIT
// such as sin, cos, abs, x^n, tan
// we also need division to work.

using namespace llvm;

JIT::JIT() {
  //  llvm::DebugFlag = true;
  m = new Module("test");
#if 0
  #if defined(_MSC_VER)  
    m->setDataLayout("e-p:32:32-f64:32:64-i64:32:64");
  #endif
    //  m->setTargetTriple("i686-pc-linux-gnu");
  m->setTargetTriple("i686-apple-darwin9.7.0");
#endif

  mp = new ExistingModuleProvider(m);
  std::string errorstring;
  ee = ExecutionEngine::create(mp,false,&errorstring);
  dbout << "Execution engine: " << QString::fromStdString(errorstring) << "\n";
  initialized = false;
  // Create the optimizer thingy
  opt = new FunctionPassManager(mp);
  opt->add(new TargetData(*ee->getTargetData()));
  //  opt->add(new TargetData(m));
  opt->add((Pass*)createVerifierPass());                  // Verify that input is correct
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
  //  opt->add((Pass*) createLoopDeletionPass());         // Delete dead loops
  opt->add((Pass*)createLoopUnrollPass());           // Unroll small loops
  opt->add((Pass*)createInstructionCombiningPass()); // Clean up after the unroller
  opt->add((Pass*)createGVNPass());   // GVN for load instructions
  opt->add((Pass*)createMemCpyOptPass());                 // Remove memcpy / form memset
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

void JIT::OptimizeCode() {
  opt->run(*func);
}

JITFunctionType JIT::FunctionType(JITType rettype, std::vector<JITType> args) {
  return llvm::FunctionType::get(rettype,args,false);
}

JITFunctionType JIT::FunctionType(QString rettype, QString args) {
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

JITType JIT::BoolType() {
  return IntegerType::get(1);
}

JITType JIT::PointerType(JITType t) {
  return PointerType::getUnqual(t);
}

JITType JIT::VoidType() {
  return Type::VoidTy;
}

JITType JIT::TypeOf(JITScalar x) {
  return x->getType();
}

JITScalar JIT::DoubleValue(double x) {
  return ConstantFP::get(Type::DoubleTy,x);
}

JITScalar JIT::FloatValue(float x) {
  return ConstantFP::get(Type::FloatTy,x);
}

JITScalar JIT::BoolValue(bool x) {
  if (x)
    return ConstantInt::get(Type::Int1Ty,1);
  else
    return ConstantInt::get(Type::Int1Ty,0);
}

JITScalar JIT::ToBool(JITScalar x) {
  if (IsBool(x)) return x;
  return new FCmpInst(FCmpInst::FCMP_ONE,x,DoubleValue(0),"",ip);
}

JITScalar JIT::ToDouble(JITScalar x) {
  if (IsDouble(x)) return x;
  return CastInst::Create(CastInst::getCastOpcode(x,true,BoolType(),true),
			  x,BoolType(),"",ip);
}

JITScalar JIT::ToFloat(JITScalar x) {
  if (IsFloat(x)) return x;
  return CastInst::Create(CastInst::getCastOpcode(x,true,FloatType(),true),
			  x,FloatType(),"",ip);
}

JITScalar JIT::ToType(JITScalar x, JITType t) {
  if (IsBool(t))
    return ToBool(x);
  else
    return CastInst::Create(CastInst::getCastOpcode(x,true,t,true),x,t,"",ip);
}

bool JIT::IsBool(JITScalar x) {
  return (IsBool(TypeOf(x)));
}

bool JIT::IsDouble(JITScalar x) {
  return (IsDouble(TypeOf(x)));
}

bool JIT::IsFloat(JITScalar x) {
  return (IsFloat(TypeOf(x)));
}

bool JIT::IsBool(JITType x) {
  return (x->isInteger());
}

bool JIT::IsDouble(JITType x) {
  return (x->getTypeID() == Type::DoubleTyID);
}

bool JIT::IsFloat(JITType x) {
  return (x->getTypeID() == Type::FloatTyID);
}

JITType JIT::MapTypeCode(QChar c) {
  switch (c.toAscii()) {
  case 'v':
    return VoidType();
  case 'b':
    return BoolType();
  case 'c':
    return IntegerType::get(8);
  case 's':
    return IntegerType::get(16);
  case 'i':
    return IntegerType::get(32);
  case 'l':
    return IntegerType::get(64);
  case 'f':
    return FloatType();
  case 'd':
    return DoubleType();
  case 'B':
    return PointerType(BoolType());
  case 'C':
    return PointerType(IntegerType::get(8));
  case 'S':
    return PointerType(IntegerType::get(16));
  case 'I':
    return PointerType(IntegerType::get(32));
  case 'L':
    return PointerType(IntegerType::get(64));
  case 'F':
    return PointerType(FloatType());
  case 'D':
    return PointerType(DoubleType());
  case 'V':
    return PointerType(IntegerType::get(8));
  default:
    throw Exception(QString("Invalid type map code '") + c + QString("'") );
  }
}

JITBlock JIT::NewBlock(QString name) {
  return BasicBlock::Create(name.toStdString(),func,0);
}

JITScalar JIT::JITBinOp(Instruction::BinaryOps op, JITScalar A, JITScalar B) {
  // Mimics ComputeTypes
  if (IsFloat(A) && IsFloat(B))
    return BinaryOperator::Create(op,A,B,"",ip);
  if (IsFloat(A) || IsFloat(B))
    return ToFloat(BinaryOperator::Create(op,ToDouble(A),ToDouble(B),"",ip));
  return BinaryOperator::Create(op,ToDouble(A),ToDouble(B),"",ip);  
}

JITScalar JIT::Mul(JITScalar A, JITScalar B) {
  return JITBinOp(Instruction::Mul,A,B);
}

JITScalar JIT::Div(JITScalar A, JITScalar B) {
  return JITBinOp(Instruction::FDiv,A,B);
}

JITScalar JIT::Sub(JITScalar A, JITScalar B) {
  return JITBinOp(Instruction::Sub,A,B);
}

JITScalar JIT::Add(JITScalar A, JITScalar B) {
  return JITBinOp(Instruction::Add,A,B);
}

JITScalar JIT::And(JITScalar A, JITScalar B) {
  return BinaryOperator::Create(Instruction::And,ToBool(A),ToBool(B),"",ip);
}

JITScalar JIT::Or(JITScalar A, JITScalar B) {
  return BinaryOperator::Create(Instruction::Or,ToBool(A),ToBool(B),"",ip);
}

JITScalar JIT::Xor(JITScalar A, JITScalar B) {
  return BinaryOperator::Create(Instruction::Xor,ToBool(A),ToBool(B),"",ip);
}

JITScalar JIT::LessThan(JITScalar A, JITScalar B) {
  return new FCmpInst(FCmpInst::FCMP_OLT,ToDouble(A),ToDouble(B),"",ip);
}

JITScalar JIT::LessEquals(JITScalar A, JITScalar B) {
  return new FCmpInst(FCmpInst::FCMP_OLE,ToDouble(A),ToDouble(B),"",ip);
}

JITScalar JIT::Equals(JITScalar A, JITScalar B) {
  return new FCmpInst(FCmpInst::FCMP_OEQ,ToDouble(A),ToDouble(B),"",ip);
}

JITScalar JIT::NotEqual(JITScalar A, JITScalar B) {
  return new FCmpInst(FCmpInst::FCMP_ONE,ToDouble(A),ToDouble(B),"",ip);
}

JITScalar JIT::GreaterThan(JITScalar A, JITScalar B) {
  return new FCmpInst(FCmpInst::FCMP_OGT,ToDouble(A),ToDouble(B),"",ip);
}

JITScalar JIT::GreaterEquals(JITScalar A, JITScalar B) {
  return new FCmpInst(FCmpInst::FCMP_OGE,ToDouble(A),ToDouble(B),"",ip);
}

JITScalar JIT::Negate(JITScalar A) {
  return BinaryOperator::CreateNeg(A,"",ip);
}

JITScalar JIT::Not(JITScalar A) {
  return BinaryOperator::CreateNot(A,"",ip);
}

void JIT::Store(JITScalar Value, JITScalar Address) {
  new StoreInst(Value, Address, ip);
}

JITScalar JIT::Alloc(JITType T, QString name) {
  return new AllocaInst(T,name.toStdString(),ip);
}

JITScalar JIT::String(QString text) {
  ArrayType* ty = ArrayType::get(IntegerType::get(8),text.size()+1);
  GlobalVariable* gv = new GlobalVariable(ty,true,
					  GlobalValue::InternalLinkage,0,".str",m);
  Constant* t_str = ConstantArray::get(text.toStdString(),true);
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
  BranchInst::Create(B,ip);
}

void JIT::Branch(JITBlock IfTrue, JITBlock IfFalse, JITScalar TestValue) {
  BranchInst::Create(IfTrue,IfFalse,TestValue,ip);
}

void JIT::SetCurrentBlock(JITBlock B) {
  ip = B;
}

JITBlock JIT::CurrentBlock() {
  return ip;
}

JITScalar JIT::Call(JITFunction F, std::vector<JITScalar> args) {
  return CallInst::Create(F,args.begin(),args.end(),"",ip);
}

JITScalar JIT::Call(JITScalar F, std::vector<JITScalar> args) {
  return CallInst::Create(F,args.begin(),args.end(),"",ip);
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
  return GetElementPtrInst::Create(BaseAddress,Offset,"",ip);
}

JITFunction JIT::DefineFunction(JITFunctionType functype, QString name) {
  return ((JITFunction) m->getOrInsertFunction(name.toStdString(), functype));
}

static std::vector<JITFunctionType> bank;

JITFunction JIT::DefineLinkFunction(QString name, QString rettype, QString args) {
  std::vector<JITType> argv;
  for (int i=0;i<args.size();i++)
    argv.push_back(MapTypeCode(args[i]));
  JITFunctionType ty = FunctionType(MapTypeCode(rettype[0]),argv);
  bank.push_back(ty);
  JITFunction fn = DefineFunction(ty,name);
  return fn;
}

void JIT::SetCurrentFunction(JITFunction f) {
  func = f;
}

JITFunction JIT::CurrentFunction() {
  return func;
}

JITScalar JIT::FunctionArgument(int n, QString name) {
  Function::arg_iterator args = func->arg_begin();
  for (int i=0;i<n;i++)
    args++;
  JITScalar ret = args;
  ret->setName(name.toStdString());
  return ret;
}

void JIT::CloseFunction() {
}

void JIT::Return(JITScalar t) {
  ReturnInst::Create(t,ip);
}

void JIT::Return() {
  ReturnInst::Create(NULL,ip);
}

void JIT::Dump(JITFunction f) {
  std::stringstream str;
  str << (*f);
  dbout << str.str();
}

void JIT::Dump() {
  std::stringstream str;
  str << (*m);
  dbout << str.str();
}

void JIT::Dump( const QString& fname ) {
  std::ofstream fout( fname.toAscii() );
  fout << (*m);
}

void JIT::Dump( const QString& fname, JITFunction f ) {
  std::ofstream fout( fname.toAscii() );
  fout << (*f);
}

JITGeneric JIT::Invoke(JITFunction f, JITGeneric arg) {
  std::vector<JITGeneric> args;
  args.push_back(arg);
  return ee->runFunction(f,args);
}

void JIT::Invoke(JITFunction f, void** arg) {
	void *FPtr = ee->getPointerToFunction(f);

    assert(FPtr && "Invoke getPointerToFunction");
    void (*PF)(void** arg) =
      (void (*)(void**))FPtr;

    // Call the function.
    PF( arg );
    return;

	//return ee->runFunction(f,args);
}


JITGeneric JIT::Invoke(JITFunction f, std::vector<JITGeneric> arg) {
  return ee->runFunction(f,arg);
}

static JIT* g_theJITPointer = NULL;

JIT* JIT::Instance() {
  if (!g_theJITPointer) 
    g_theJITPointer = new JIT;
  return g_theJITPointer;
}

#endif
