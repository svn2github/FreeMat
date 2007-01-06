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

#ifndef __Context_hpp__
#define __Context_hpp__


#include "Scope.hpp"
#include "Array.hpp"
#include "FunctionDef.hpp"
#include "Exception.hpp"
#include "Types.hpp"
#include <vector>
#include <QDebug>
#include <QMutex>
#include <stdarg.h>

/**
 * A Context is a stack of scopes with the (peculiar) property that
 * the top and bottom of the stack (global and local scopes respectively)
 * are searched regularly.  The context is responsible for determining
 * if variables and functions exist, and if so, for returning their
 * values and accepting modifications to them.  A context also keeps
 * track of loop depth.
 */
class Context {
  /**
   * The normal stack of scopes.
   */
  std::vector<Scope*> scopestack;
  /**
   * The stack of scopes that have been "bypassed"
   */
  std::vector<Scope*> bypassstack;
  /**
   * List of functions that are "temporary" and should be flushed
   */
  stringVector tempFunctions;
  /**
   * Mutex to control access to this context class.
   */
  QMutex mutex;
  /**
   * Pointer to current scope
   */
  Scope *bottomScope;
  /**
   * Pointer to global scope
   */
  Scope *topScope;
public:
  /**
   * Create a context and initialize it with a global scope and a 
   * base scope.
   */
  Context() : mutex(QMutex::Recursive) {
    pushScope("global");
    pushScope("base");
    topScope = scopestack.front();
    bottomScope = scopestack.back();
  }

  /**
   * Delete the context
   */
  ~Context() {
    while (!scopestack.empty()) {
      delete scopestack.back();
      scopestack.pop_back();
    }
  }

  /**
   * Get the pointer to the mutex that protects this context.
   * This mutex only needs to be used when the GUI thread wants
   * to access the context (which is "owned" by the Interpreter
   * thread).
   */
  inline QMutex* getMutex() {
    return &mutex;
  }

  /**
   * Bypass the prescribed number of scopes.  These scopes are
   * placed on the bypassstack.  This effectively makes a different
   * scope active, and then allows us to restore the bypass scopes.
   * a count of -1 means all scopes are bypassed (except the base scope)
   */
  inline void bypassScope(int count) {
    if (count < 0)
      count = scopestack.size();
    while ((count > 0) && (scopestack.back()->getName() != "base")) {
      bypassstack.push_back(scopestack.back());
      scopestack.pop_back();
      count--;
    }
    bottomScope = scopestack.back();
  }
  /**
   * Every call to bypassScope should be matched by a call to 
   * restoreBypassedScopes, or memory leaks will occur.
   */
  inline void restoreBypassedScopes() {
    for (int i=0;i<bypassstack.size();i++)
      scopestack.push_back(bypassstack[i]);
    bypassstack.clear();
    bottomScope = scopestack.back();
  }
  /**
   * Push the given scope onto the bottom of the scope stack.
   */
  inline void pushScope(std::string name) {
    if (scopestack.size() > 100)
      throw Exception("Allowable stack depth exceeded...");
    scopestack.push_back(new Scope(name));
    bottomScope = scopestack.back();
  }
  /**
   * Pop the bottom scope off of the scope stack.  The scope is
   * deleted.
   * Throws an Exception if the global scope is popped.
   */
  inline void popScope() {
    if (scopestack.size() == 1)
      throw Exception("Attempt to pop global scope off of context stack!");
    delete scopestack.back();
    scopestack.pop_back();
    bottomScope = scopestack.back();
  }
  /**
   * Insert the given variable into the right scope - the global
   * scope if the array is in the global list, and mangled in
   * global list if it is persistent.  
   */
  inline void insertVariable(const std::string& varName, const Array& var) {
    Scope* active;
    std::string mapName;
    
    if (bottomScope->isVariablePersistent(varName)) {
      mapName = bottomScope->getMangledName(varName);
      active = topScope;
    } else if (bottomScope->isVariableGlobal(varName)) {
      mapName = varName;
      active = topScope;
    } else {
      bottomScope->insertVariable(varName,var);
      return;
    }
    active->insertVariable(mapName,var);
  }
  /**
   * Insert a variable into the local scope - do not check the
   * global list.
   */
  inline void insertVariableLocally(std::string varName, const Array& var) {
    bottomScope->insertVariable(varName,var);
  }
  /**
   * Return a pointer to the given variable.  The search
   * logic is:
   *   - If the variable is persistent in the current scope
   *     (at the bottom of the scope stack), mangle its name
   *     using the scope, and look for it in the global scope.
   *   - If the variable is global in the current scope (at the
   *     bottom of the scope stack, look for it in the global
   *     scope.
   *   - Look for the variable in the local scope.
   * If the variable is not found, an empty variable is constructed
   * with the given name, and inserted into the scope that was
   * searched.  A pointer to this newly created variable is returned.
   */
  inline Array* lookupVariable(const std::string& varName) {
    Scope* active;
    std::string mapName;
    
    if (bottomScope->isVariablePersistent(varName)) {
      mapName = bottomScope->getMangledName(varName);
      active = topScope;
    } else if (bottomScope->isVariableGlobal(varName)) {
      mapName = varName;
      active = topScope;
    } else {
      return (bottomScope->lookupVariable(varName));
    }
    return (active->lookupVariable(mapName));
  }
  /**
   * Look for a variable, but only locally.
   */
  inline Array* lookupVariableLocally(std::string varName) {
    return bottomScope->lookupVariable(varName);
  }
  /**
   * Insert a function definition into the local scope (bottom of
   * the scope stack).
   */
  inline void insertFunctionLocally(FuncPtr f) {
    bottomScope->insertFunction(f);
  }
  /**
   * Insert a function definition into the global scope (top of the
   * scope stack).
   */
  inline void insertFunctionGlobally(FuncPtr f, bool temporary) {
    topScope->insertFunction(f);
    if (temporary)
      tempFunctions.push_back(f->name);
  }
  /**
   * Flush temporary function definitions from the global context
   */
  inline void flushTemporaryGlobalFunctions() {
    for (int i=0;i<tempFunctions.size();i++)
      topScope->deleteFunction(tempFunctions[i]);
    tempFunctions.clear();
  }
  /**
   * Add a built in function to the global scope with the given name.
   */
  inline void addFunction(char *name, BuiltInFuncPtr fptr, int argc_in, int argc_out, ...) {
    stringVector args;
    va_list argp;
    if (argc_in>0) {
      va_start(argp,argc_out);
      for (int i=0;i<argc_in;i++) {
	const char *t = va_arg(argp, const char *);
	if (!t) {
	  qDebug() << "addFunction for function " << name << " is wrong!\n";
	  exit(1);
	}
	args.push_back(t);
      }
      if (va_arg(argp,const char *) != NULL) {
	qDebug() << "addFunction for function " << name << " is wrong!\n";
	exit(1);
      }
      va_end(argp);
    }
    BuiltInFunctionDef *f2def;
    f2def = new BuiltInFunctionDef;
    f2def->retCount = argc_out;
    f2def->argCount = argc_in;
    f2def->name = strdup(name);
    f2def->fptr = fptr;
    f2def->arguments = args;
    topScope->insertFunction(f2def);  
  }
  /**
   * Add a special function to the global scope with the given name.
   */
  inline void addSpecialFunction(char*name, SpecialFuncPtr fptr, int argc_in, int argc_out, ...) {
    stringVector args;
    va_list argp;
    if (argc_in>0) {
      va_start(argp,argc_out);
      for (int i=0;i<argc_in;i++) {
	const char *t = va_arg(argp, const char *);
	if (!t) {
	  qDebug() << "addSpecialFunction for function " << name << " is wrong!\n";
	  exit(1);
	}
	args.push_back(t);
      }
      if (va_arg(argp,const char *) != NULL) {
	qDebug() << "addSpecialFunction for function " << name << " is wrong!\n";
	exit(1);
      }
      va_end(argp);
    }
    SpecialFunctionDef *f2def;
    f2def = new SpecialFunctionDef;
    f2def->retCount = argc_out;
    f2def->argCount = argc_in;
    f2def->name = strdup(name);
    f2def->fptr = fptr;
    f2def->arguments = args;
    topScope->insertFunction(f2def);
  }
  /**
   * Add a built in function to the global scope with the given name
   * and tag it as a graphics function
   */
  inline void addGfxFunction(char*name, BuiltInFuncPtr fptr, int argc_in, int argc_out, ...) {
    stringVector args;
    va_list argp;
    if (argc_in>0) {
      va_start(argp,argc_out);
      for (int i=0;i<argc_in;i++) {
	const char *t = va_arg(argp, const char *);
	if (!t) {
	  qDebug() << "addGfxFunction for function " << name << " is wrong!\n";
	  exit(1);
	}
	args.push_back(t);
      }
      if (va_arg(argp,const char *) != NULL) {
	qDebug() << "addGfxFunction for function " << name << " is wrong!\n";
	exit(1);
      }
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
    topScope->insertFunction(f2def);  
  }
  /**
   * Add a special function to the global scope with the given name, and
   * tag it as a graphics function
   */
  inline void addGfxSpecialFunction(char*name, SpecialFuncPtr fptr, int argc_in, int argc_out, ...) {
    stringVector args;
    va_list argp;
    if (argc_in>0) {
      va_start(argp,argc_out);
      for (int i=0;i<argc_in;i++) {
	const char *t = va_arg(argp, const char *);
	if (!t) {
	  qDebug() << "addGfxSpecialFunction for function " << name << " is wrong!\n";
	  exit(1);
	}
	args.push_back(t);
      }
      if (va_arg(argp,const char *) != NULL) {
	qDebug() << "addGfxSpecialFunction for function " << name << " is wrong!\n";
	exit(1);
      }
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
    topScope->insertFunction(f2def);
  }
  
  inline stringVector listAllFunctions() {
    stringVector retval(topScope->listAllFunctions());
    stringVector bottomFunctions(bottomScope->listAllFunctions());
    for (int i=0;i<bottomFunctions.size();i++)
      retval.push_back(bottomFunctions[i]);
    return retval;
  }

  inline bool lookupFunction(std::string funcName, FuncPtr& val) {
    bool localFunction;
    if (bottomScope->lookupFunction(funcName,val))
      return true;
    return topScope->lookupFunction(funcName,val);
  }
    
  inline bool lookupFunctionLocally(std::string funcName, FuncPtr& val) {
    return bottomScope->lookupFunction(funcName,val);
  }

  inline bool lookupFunctionGlobally(std::string funcName, FuncPtr& val) {
    return topScope->lookupFunction(funcName,val);
  }
    
  inline void deleteFunctionGlobally(std::string funcName) {
    topScope->deleteFunction(funcName);
  }
  /**
   * Add a persistent variable to the local stack.  This involves
   * two steps:
   *   - the name of the variable is added to the persistent variable list
   *     in the current scope.
   *   - the global scope is checked for the mangled name of the 
   *     persistent variable.  If the variable does not exist in the
   *     global scope, then an empty variable is inserted.
   */
  inline void addPersistentVariable(std::string var) {
    // Delete local variables with this name
    bottomScope->deleteVariable(var);
    // Delete global variables with this name
    //  topScope->deleteVariable(var);
    bottomScope->addPersistentVariablePointer(var);
  }
  /**
   * Add a variable name into the global variable list of the current
   * scope.  If the variable does not exist in the global scope, an
   * empty variable is added.
   */
  inline void addGlobalVariable(std::string var) {
    // Delete local variables with this name
    bottomScope->deleteVariable(var);
    // Delete global persistent variables with this name
    topScope->deleteVariable(bottomScope->getMangledName(var));
    // Add a point in the local scope to the global variable
    bottomScope->addGlobalVariablePointer(var);
  }
  /**
   * Delete a variable if its defined.  Handles global and persistent
   * variables also.
   */
  inline void deleteVariable(std::string var) {
    if (isVariableGlobal(var)) {
      topScope->deleteVariable(var);
      bottomScope->deleteGlobalVariablePointer(var);
      return;
    }
    if (isVariablePersistent(var)) {
      topScope->deleteVariable(bottomScope->getMangledName(var));
      bottomScope->deletePersistentVariablePointer(var);
      return;
    }
    bottomScope->deleteVariable(var);
  }
  /**
   * Get the global scope off the top of the scope stack.
   */
  inline Scope *getGlobalScope() {
    return topScope;
  }
  /**
   * Get the current (active) scope
   */
  inline Scope *getCurrentScope() {
    return bottomScope;
  }
  /**
   * Increment the loop depth counter in the local scope.
   */
  inline void enterLoop() {
    bottomScope->enterLoop();
  }
  /**
   * Decrement the loop depth counter in the local scope.
   */
  inline void exitLoop() {
    bottomScope->exitLoop();
  }
  /**
   * Returns true if the current (local) scope indicates a
   * non-zero loop depth.
   */
  inline bool inLoop() {
    return bottomScope->inLoop();
  }
  /**
   * Returns true if the given variable is global.
   */
  inline bool isVariableGlobal(const std::string& varName) {
    return bottomScope->isVariableGlobal(varName);
  }
  /**
   * Returns true if the given variable is persistent
   */
  inline bool isVariablePersistent(const std::string& varName) {
    return bottomScope->isVariablePersistent(varName);
  }
};

#endif
