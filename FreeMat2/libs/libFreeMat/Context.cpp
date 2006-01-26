// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Context.hpp"
#include "Array.hpp"
#include <stdarg.h>

namespace FreeMat {

  Context::Context() {
    pushScope("global");
    pushScope("base");
  }

  Context::~Context() {
    while (!scopestack.empty()) {
      delete scopestack.back();
      scopestack.pop_back();
    }
  }

  Scope* Context::getCurrentScope() {
    return scopestack.back();
  }

  Scope* Context::getGlobalScope() {
    return scopestack.front();
  }
  
  void Context::pushScope(std::string name) {
    if (scopestack.size() > 100)
      throw Exception("Allowable stack depth exceeded...");
    scopestack.push_back(new Scope(name));
  }

  void Context::popScope() throw(Exception) {
    if (scopestack.size() == 1)
      throw Exception("Attempt to pop global scope off of context stack!");
    delete scopestack.back();
    scopestack.pop_back();
  }

  void Context::insertVariableLocally(std::string varName, const Array& var) {
    scopestack.back()->insertVariable(varName,var);
  }

  void Context::insertVariable(const std::string& varName, const Array& var) {
    Scope* active;
    std::string mapName;

    if (scopestack.back()->isVariablePersistent(varName)) {
      mapName = scopestack.back()->getMangledName(varName);
      active = scopestack.front();
    } else if (scopestack.back()->isVariableGlobal(varName)) {
      mapName = varName;
      active = scopestack.front();
    } else {
      scopestack.back()->insertVariable(varName,var);
      return;
    }
    active->insertVariable(mapName,var);
  }

  Array* Context::lookupVariable(const std::string& varName) {
    Scope* active;
    std::string mapName;
    
    if (scopestack.back()->isVariablePersistent(varName)) {
      mapName = scopestack.back()->getMangledName(varName);
      active = scopestack.front();
    } else if (scopestack.back()->isVariableGlobal(varName)) {
      mapName = varName;
      active = scopestack.front();
    } else {
      return (scopestack.back()->lookupVariable(varName));
    }
    return (active->lookupVariable(mapName));
  }

  bool Context::isVariableGlobal(const std::string& varName) {
    return scopestack.back()->isVariableGlobal(varName);
  }

  bool Context::isVariablePersistent(const std::string& varName) {
    return scopestack.back()->isVariablePersistent(varName);
  }

  Array* Context::lookupVariableLocally(std::string varName) {
    return scopestack.back()->lookupVariable(varName);
  }

  void Context::insertFunctionLocally(FuncPtr f) {
    scopestack.back()->insertFunction(f);
  }

  void Context::insertFunctionGlobally(FuncPtr f, bool temporary) {
    scopestack.front()->insertFunction(f);
    if (temporary)
      tempFunctions.push_back(f->name);
  }

  void Context::flushTemporaryGlobalFunctions() {
    for (int i=0;i<tempFunctions.size();i++)
      scopestack.front()->deleteFunction(tempFunctions[i]);
    tempFunctions.clear();
  }

  void Context::addSpecialFunction(char*name,
				   SpecialFuncPtr fptr,
				   int argc_in, int argc_out,...) {
    stringVector args;
    va_list argp;
    if (argc_in>0) {
      va_start(argp,argc_out);
      for (int i=0;i<argc_in;i++)
	args.push_back(va_arg(argp,const char *));
      va_end(argp);
    }
    SpecialFunctionDef *f2def;
    f2def = new SpecialFunctionDef;
    f2def->retCount = argc_out;
    f2def->argCount = argc_in;
    f2def->name = strdup(name);
    f2def->fptr = fptr;
    f2def->arguments = args;
    scopestack.front()->insertFunction(f2def);
  }

  void Context::addFunction(char*name, 
			    BuiltInFuncPtr fptr, 
			    int argc_in, int argc_out,...) {
    stringVector args;
    va_list argp;
    if (argc_in>0) {
      va_start(argp,argc_out);
      for (int i=0;i<argc_in;i++)
	args.push_back(va_arg(argp,const char *));
      va_end(argp);
    }
    BuiltInFunctionDef *f2def;
    f2def = new BuiltInFunctionDef;
    f2def->retCount = argc_out;
    f2def->argCount = argc_in;
    f2def->name = strdup(name);
    f2def->fptr = fptr;
    f2def->arguments = args;
    scopestack.front()->insertFunction(f2def);  
  }

  bool Context::lookupFunction(std::string funcName, FuncPtr& val) {
    bool localFunction;
    if (scopestack.back()->lookupFunction(funcName,val))
      return true;
    return scopestack.front()->lookupFunction(funcName,val);
  }

  bool Context::lookupFunctionLocally(std::string funcName, FuncPtr& val) {
    return scopestack.back()->lookupFunction(funcName,val);
  }

  bool Context::lookupFunctionGlobally(std::string funcName, FuncPtr& val) {
    return scopestack.front()->lookupFunction(funcName,val);
  }


  void Context::deleteFunctionGlobally(std::string funcName) {
    scopestack.front()->deleteFunction(funcName);
  }

  void Context::printMe() {
//     ScopeStack *p;

//     p = head;
//     outputMessage("*** Context Begin ***\n");
//     while (p != NULL) {
//       p->data->printMe();
//       p = p->next;
//     }
//     outputMessage("*** Context End ***\n");
  }

  void Context::enterLoop() {
    scopestack.back()->enterLoop();
  }

  void Context::exitLoop() {
    scopestack.back()->exitLoop();
  }

  bool Context::inLoop() {
    return scopestack.back()->inLoop();
    //  return false;
  }

  void Context::addPersistentVariable(std::string var) {
    // Delete local variables with this name
    scopestack.back()->deleteVariable(var);
    // Delete global variables with this name
    scopestack.front()->deleteVariable(var);
    scopestack.back()->addPersistentVariablePointer(var);
    if (!scopestack.front()->lookupVariable(scopestack.back()->getMangledName(var)))
      scopestack.front()->insertVariable(scopestack.back()->getMangledName(var), Array::emptyConstructor());
  }

  void Context::addGlobalVariable(std::string var) {
    // Delete local variables with this name
    scopestack.back()->deleteVariable(var);
    // Delete global persistent variables with this name
    scopestack.front()->deleteVariable(scopestack.back()->getMangledName(var));
    // Add a point in the local scope to the global variable
    scopestack.back()->addGlobalVariablePointer(var);
    // Make sure the variable exists
    if (!scopestack.front()->lookupVariable(var))
      scopestack.front()->insertVariable(var, Array::emptyConstructor());
  }

  void Context::deleteVariable(std::string var) {
    if (isVariableGlobal(var)) {
      scopestack.front()->deleteVariable(var);
      scopestack.back()->deleteGlobalVariablePointer(var);
      return;
    }
    if (isVariablePersistent(var)) {
      scopestack.front()->deleteVariable(scopestack.back()->getMangledName(var));
      scopestack.back()->deletePersistentVariablePointer(var);
      return;
    }
    scopestack.back()->deleteVariable(var);
  }

  void Context::bypassScope(int count) {
    if (count < 0)
      count = scopestack.size();
    while ((count > 0) && (scopestack.back()->getName() != "base")) {
      bypassstack.push_back(scopestack.back());
      scopestack.pop_back();
      count--;
    }
  }

  void Context::restoreBypassedScopes() {
    for (int i=0;i<bypassstack.size();i++)
      scopestack.push_back(bypassstack[i]);
    bypassstack.clear();
  }

}
