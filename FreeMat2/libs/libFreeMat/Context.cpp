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

namespace FreeMat {

  Context::Context() {
    head = new ScopeStack();
    head->data = new Scope("global");
    head->next = NULL;
    tail = head;
    pushScope("base");
  }

  Context::~Context() {
    while (head != NULL) {
      delete head->data;
      tail = head->next;
      delete head;
      head = tail;
    }
  }

  Scope* Context::getCurrentScope() {
    return tail->data;
  }

  Scope* Context::getGlobalScope() {
    return head->data;
  }

  void Context::pushScope(std::string name) {
    tail->next = new ScopeStack();
    tail->next->data = new Scope(name);
    tail->next->next = NULL;
    tail = tail->next;
  }

  void Context::popScope() throw(Exception) {
    ScopeStack *p = head;
    if (p->next == NULL)
      throw Exception("Attempt to pop global scope off of context stack!");
    while (p->next != tail) p = p->next;
    delete(tail->data);
    delete(tail);
    p->next = NULL;
    tail = p;
  }

  void Context::insertVariableLocally(std::string varName, const Array& var) {
    tail->data->insertVariable(varName,var);
  }

  void Context::insertVariable(const std::string& varName, const Array& var) {
    Scope* active;
    std::string mapName;

    if (tail->data->isVariablePersistent(varName)) {
      mapName = tail->data->getMangledName(varName);
      active = head->data;
    } else if (tail->data->isVariableGlobal(varName)) {
      mapName = varName;
      active = head->data;
    } else {
      tail->data->insertVariable(varName,var);
      return;
//       mapName = varName;
//       active = tail->data;
    }
    active->insertVariable(mapName,var);
  }

  bool Context::lookupVariable(const std::string& varName, Array &var) {
    Scope* active;
    std::string mapName;
    
    if (tail->data->isVariablePersistent(varName)) {
      mapName = tail->data->getMangledName(varName);
      active = head->data;
    } else if (tail->data->isVariableGlobal(varName)) {
      mapName = varName;
      active = head->data;
    } else {
      return (tail->data->lookupVariable(varName,var));
//       mapName = varName;
//       active = tail->data;
    }
    return (active->lookupVariable(mapName,var));
  }

  bool Context::isVariableGlobal(const std::string& varName) {
    return tail->data->isVariableGlobal(varName);
  }

  bool Context::isVariablePersistent(const std::string& varName) {
    return tail->data->isVariablePersistent(varName);
  }

  bool Context::lookupVariableLocally(std::string varName, Array &var) {
    return tail->data->lookupVariable(varName,var);
  }

  void Context::insertFunctionLocally(FuncPtr f) {
    tail->data->insertFunction(f);
  }

  void Context::insertFunctionGlobally(FuncPtr f, bool temporary) {
    head->data->insertFunction(f);
    if (temporary)
      tempFunctions.push_back(f->name);
  }

  void Context::flushTemporaryGlobalFunctions() {
    for (int i=0;i<tempFunctions.size();i++)
      head->data->deleteFunction(tempFunctions[i]);
    tempFunctions.clear();
  }

  void Context::addSpecialFunction(char*name,
				   SpecialFuncPtr fptr,
				   int argc_in, int argc_out,
				   stringVector args) {
    SpecialFunctionDef *f2def;
    f2def = new SpecialFunctionDef;
    f2def->retCount = argc_out;
    f2def->argCount = argc_in;
    f2def->name = strdup(name);
    f2def->fptr = fptr;
    f2def->arguments = args;
    head->data->insertFunction(f2def);
  }

  void Context::addFunction(char*name, 
			    BuiltInFuncPtr fptr, 
			    int argc_in, int argc_out,
			    stringVector args) {
    BuiltInFunctionDef *f2def;
    f2def = new BuiltInFunctionDef;
    f2def->retCount = argc_out;
    f2def->argCount = argc_in;
    f2def->name = strdup(name);
    f2def->fptr = fptr;
    f2def->arguments = args;
    head->data->insertFunction(f2def);  
  }

  bool Context::lookupFunction(std::string funcName, FuncPtr& val) {
    bool localFunction;
    if (tail->data->lookupFunction(funcName,val))
      return true;
    return head->data->lookupFunction(funcName,val);
  }

  bool Context::lookupFunctionGlobally(std::string funcName, FuncPtr& val) {
    return head->data->lookupFunction(funcName,val);
  }

  void Context::deleteFunctionGlobally(std::string funcName) {
    head->data->deleteFunction(funcName);
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
    tail->data->enterLoop();
  }

  void Context::exitLoop() {
    tail->data->exitLoop();
  }

  bool Context::inLoop() {
    return tail->data->inLoop();
    //  return false;
  }

  void Context::addPersistentVariable(std::string var) {
    Array dummy;
    // Delete local variables with this name
    tail->data->deleteVariable(var);
    // Delete global variables with this name
    head->data->deleteVariable(var);
    tail->data->addPersistentVariablePointer(var);
    if (!head->data->lookupVariable(tail->data->getMangledName(var),dummy))
      head->data->insertVariable(tail->data->getMangledName(var), Array::emptyConstructor());
  }

  void Context::addGlobalVariable(std::string var) {
    Array dummy;
    // Delete local variables with this name
    tail->data->deleteVariable(var);
    // Delete global persistent variables with this name
    head->data->deleteVariable(tail->data->getMangledName(var));
    // Add a point in the local scope to the global variable
    tail->data->addGlobalVariablePointer(var);
    // Make sure the variable exists
    if (!head->data->lookupVariable(var,dummy))
      head->data->insertVariable(var, Array::emptyConstructor());
  }

  void Context::deleteVariable(std::string var) {
    if (isVariableGlobal(var)) {
      head->data->deleteVariable(var);
      tail->data->deleteGlobalVariablePointer(var);
      return;
    }
    if (isVariablePersistent(var)) {
      head->data->deleteVariable(tail->data->getMangledName(var));
      tail->data->deletePersistentVariablePointer(var);
      return;
    }
    tail->data->deleteVariable(var);
  }
}
