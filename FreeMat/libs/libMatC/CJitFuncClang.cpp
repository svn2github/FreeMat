#include "CJitFuncClang.hpp"
#include "CArray.hpp"
#include <iostream>
#include <fstream>

#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Config/config.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Config/config.h"
#include "llvm/LLVMContext.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/raw_ostream.h"
#ifdef LLVM_28
#include "llvm/System/Host.h"
#include "llvm/System/Path.h"
#else
#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"
#endif
#include "llvm/Target/TargetSelect.h"
#include "llvm/Target/TargetOptions.h"



#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/ExecutionEngine/JIT.h"
//#include "llvm/ExecutionEngine/Interpreter.h"




#include <QTemporaryFile>
#include <QDir>

using namespace clang;
using namespace clang::driver;


CJitFuncClang::CJitFuncClang(Interpreter* eval)
{
  m_eval = eval;
  ctxt = new llvm::LLVMContext;
}

CJitFuncClang::~CJitFuncClang()
{
  delete EE;
  delete comp;
}
QString GetRootPath();

bool CJitFuncClang::compile(const std::string &filename, 
			    const std::string &funcname) 
{
  llvm::InitializeNativeTarget();
  TextDiagnosticPrinter *DiagClient =
    new TextDiagnosticPrinter(llvm::errs(), DiagnosticOptions());
#ifdef LLVM_28
  Diagnostic Diags(DiagClient);
#else
  llvm::IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  Diagnostic Diags(DiagID, DiagClient);
#endif
  Driver TheDriver("", llvm::sys::getHostTriple(),
                   "a.out", /*IsProduction=*/false, /*CXXIsProduction=*/false,
                   Diags);
  TheDriver.setTitle("FreeMat JIT");
  llvm::SmallVector<const char *, 16> Args;
  Args.push_back("FreeMat");
  Args.push_back(filename.c_str());
  Args.push_back("-fsyntax-only");
  Args.push_back("-O3");
  Args.push_back("-v");
  llvm::OwningPtr<Compilation> C(TheDriver.BuildCompilation(Args.size(),
                                                            Args.data()));
  if (!C) return false;
  const driver::JobList &Jobs = C->getJobs();
  if (Jobs.size() != 1 || !isa<driver::Command>(Jobs.begin())) {
    llvm::SmallString<256> Msg;
    llvm::raw_svector_ostream OS(Msg);
    C->PrintJob(OS, C->getJobs(), "; ", true);
    Diags.Report(diag::err_fe_expected_compiler_job) << OS.str();
    return false;
  }
  const driver::Command *Cmd = cast<driver::Command>(*Jobs.begin());
  if (llvm::StringRef(Cmd->getCreator().getName()) != "clang") {
    Diags.Report(diag::err_fe_expected_clang_command);
    return false;
  }
  // Initialize a compiler invocation object from the clang (-cc1) arguments.
  const driver::ArgStringList &CCArgs = Cmd->getArguments();
  llvm::OwningPtr<CompilerInvocation> CI(new CompilerInvocation);
  CompilerInvocation::CreateFromArgs(*CI,
                                     const_cast<const char **>(CCArgs.data()),
                                     const_cast<const char **>(CCArgs.data()) +
                                       CCArgs.size(),
                                     Diags);
  QString path = GetRootPath() + "/toolbox/jit";
  CI->getHeaderSearchOpts().AddPath(path.toStdString().c_str(),frontend::Quoted,true,false,false);

  // FIXME: This is copied from cc1_main.cpp; simplify and eliminate.
  // Create a compiler instance to handle the actual work.
  comp = new clang::CompilerInstance;
#ifdef LLVM_28
  comp->setLLVMContext(new llvm::LLVMContext);
#endif
  comp->setInvocation(CI.take());
  // Create the compilers actual diagnostics engine.
  comp->createDiagnostics(int(CCArgs.size()),const_cast<char**>(CCArgs.data()));
  if (!comp->hasDiagnostics()) return false;
  // Create and execute the frontend to generate an LLVM bitcode module.
#ifdef LLVM_28
  llvm::OwningPtr<CodeGenAction> Act(new EmitLLVMOnlyAction());
#else
  // Pass the LLVM context to the code gen action.  Otherwise, the action
  // creates a new context and then promptly deletes it when it goes out
  // of scope. :P
  llvm::OwningPtr<CodeGenAction> Act(new EmitLLVMOnlyAction(ctxt));
#endif
  if (!comp->ExecuteAction(*Act)) return false;
  llvm::JITExceptionHandling = true;
  if (llvm::Module *Module = Act->takeModule())
    {
      std::string Error;
      EE = llvm::ExecutionEngine::createJIT(Module,&Error);
      if (!EE) return false;
      func =  Module->getFunction(funcname);
      return true;
    }
  return false;
}

bool CJitFuncClang::compile(const Tree & t, JITControlFlag flag)
{
  m_flag = flag;
  CJitFunc mcomp;
  mcomp.set_interpreter(m_eval);
  mcomp.compile_tree(t,std::string("testfunc"));
  if (m_flag == JITOn)
    {
      QTemporaryFile file(QDir::tempPath()+"/jitXXXXXX.cpp");
      file.open();
      std::string codename = file.fileName().toStdString();
      mcomp.writeCode(codename);
      return compile(codename,"testfunc");
    }
  if (m_flag == JITTrace)
    {
      QString name = QDir::tempPath()+"/jittrace.cpp";
      std::string codename = name.toStdString();
      mcomp.writeCode(codename);
      return compile(codename,"testfunc");
    }
}

int CJitFuncClang::run()
{
  std::vector<llvm::GenericValue> args(1);
  args[0].PointerVal = m_eval;
  llvm::GenericValue ret = EE->runFunction(func,args);
  int retval = ret.IntVal.getSExtValue();
  if (retval == CJIT_Runfail)
    throw Exception(m_eval->getLastErrorString());
  return retval;
}

void force_linkage()
{
  carray_empty();
  carray_scalar(0,0);
  carray_create(0,0,0,0,0);
  carray_download_scalar(0,0,0,0);
  carray_download_array(0,0,0,0);
  carray_download_function(0,0,0);
  carray_upload_scalar(0,0,0,0);
  carray_upload_array(0,0,0);
  carray_copy(0);
  carray_free(0);
  carray_rows(0);
  carray_cols(0);
  carray_set_ss(0,0,0,0,0);
  carray_set_s(0,0,0,0);
  carray_set_aa(0,0,0,0,0);
  carray_set_a(0,0,0,0);
  carray_get_ss(0,0,0,0,0);
  carray_get_s(0,0,0,0);
  carray_get_aa(0,0,0,0,0);
  carray_get_a(0,0,0,0);
  carray_duplicate(0,0,0);
  carray_add(0,0,0,0);
  carray_hcat(0,0,0,0);
  carray_vcat(0,0,0,0);
  carray_sub(0,0,0,0);
  carray_times(0,0,0,0);
  carray_pow(0,0,0,0);
  carray_dpow(0,0,0,0);
  carray_dtimes(0,0,0,0);
  carray_rdiv(0,0,0,0);
  carray_drdiv(0,0,0,0);
  carray_ldiv(0,0,0,0);
  carray_dldiv(0,0,0,0);
  carray_colon(0,0,0,0);
  carray_dcolon(0,0,0,0,0);
  carray_or(0,0,0,0);
  carray_and(0,0,0,0);
  carray_lt(0,0,0,0);
  carray_le(0,0,0,0);
  carray_gt(0,0,0,0);
  carray_ge(0,0,0,0);
  carray_eq(0,0,0,0);
  carray_neq(0,0,0,0);
  carray_pos(0,0,0);
  carray_neg(0,0,0);
  carray_not(0,0,0);
  carray_transpose(0,0,0);
  carray_dottranspose(0,0,0);
  carray_any(0,0,0);
  carray_all(0,0,0);
  carray_invoke_1(0,0,0,0);
  carray_invoke_2(0,0,0,0,0);
}
