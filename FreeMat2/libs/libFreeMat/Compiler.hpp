#ifndef __Compiler_hpp__
#define __Compiler_hpp__

#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include <string>

using namespace std;

void ProtoCompile(Interpreter *m_eval, MFunctionDef *m_def, string filename);

#endif
