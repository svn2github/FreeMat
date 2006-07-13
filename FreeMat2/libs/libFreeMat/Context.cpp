/*
 * Copyright (c) 2002-2006 Samit Basu
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

#include "Context.hpp"
#include "Array.hpp"
#include <stdarg.h>

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

void Context::addGfxSpecialFunction(char*name,
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
  f2def->graphicsFunction = true;
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

void Context::addGfxFunction(char*name, 
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
  f2def->graphicsFunction = true;
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

