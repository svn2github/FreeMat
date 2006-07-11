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
public:
  /**
   * Create a context and initialize it with a global scope and a 
   * base scope.
   */
  Context();
  /**
   * Delete the context
   */
  ~Context();
  /**
   * Bypass the prescribed number of scopes.  These scopes are
   * placed on the bypassstack.  This effectively makes a different
   * scope active, and then allows us to restore the bypass scopes.
   * a count of -1 means all scopes are bypassed (except the base scope)
   */
  void bypassScope(int count);
  /**
   * Every call to bypassScope should be matched by a call to 
   * restoreBypassedScopes, or memory leaks will occur.
   */
  void restoreBypassedScopes();
  /**
   * Push the given scope onto the bottom of the scope stack.
   */
  void pushScope(std::string);
  /**
   * Pop the bottom scope off of the scope stack.  The scope is
   * deleted.
   * Throws an Exception if the global scope is popped.
   */
  void popScope() throw(Exception);
  /**
   * Insert the given variable into the right scope - the global
   * scope if the array is in the global list, and mangled in
   * global list if it is persistent.  
   */
  void insertVariable(const std::string& varName, const Array&);
  /**
   * Insert a variable into the local scope - do not check the
   * global list.
   */
  void insertVariableLocally(std::string varName, const Array&);
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
  Array* lookupVariable(const std::string& varName);
  /**
   * Look for a variable, but only locally.
   */
  Array* lookupVariableLocally(std::string varName);
  /**
   * Insert a function definition into the local scope (bottom of
   * the scope stack).
   */
  void insertFunctionLocally(FuncPtr);
  /**
   * Insert a function definition into the global scope (top of the
   * scope stack).
   */
  void insertFunctionGlobally(FuncPtr, bool temporary);
  /**
   * Flush temporary function definitions from the global context
   */
  void flushTemporaryGlobalFunctions();
  /**
   * Add a built in function to the global scope with the given name.
   */
  void addFunction(char*name, BuiltInFuncPtr fptr, int argin, int argout, ...);
  /**
   * Add a special function to the global scope with the given name.
   */
  void addSpecialFunction(char*name, SpecialFuncPtr fptr, int argin, int argout, ...);

  bool lookupFunction(std::string funcName, FuncPtr& val);
    
  bool lookupFunctionLocally(std::string funcName, FuncPtr& val);

  bool lookupFunctionGlobally(std::string funcName, FuncPtr& val);
    
  void deleteFunctionGlobally(std::string funcName);
  /**
   * Add a persistent variable to the local stack.  This involves
   * two steps:
   *   - the name of the variable is added to the persistent variable list
   *     in the current scope.
   *   - the global scope is checked for the mangled name of the 
   *     persistent variable.  If the variable does not exist in the
   *     global scope, then an empty variable is inserted.
   */
  void addPersistentVariable(std::string var);
  /**
   * Add a variable name into the global variable list of the current
   * scope.  If the variable does not exist in the global scope, an
   * empty variable is added.
   */
  void addGlobalVariable(std::string var);
  /**
   * Delete a variable if its defined.  Handles global and persistent
   * variables also.
   */
  void deleteVariable(std::string var);
  /**
   * Get the global scope off the top of the scope stack.
   */
  Scope *getGlobalScope();
  /**
   * Get the current (active) scope
   */
  Scope *getCurrentScope();
  /**
   * Print the context.
   */
  void printMe();
  /**
   * Increment the loop depth counter in the local scope.
   */
  void enterLoop();
  /**
   * Decrement the loop depth counter in the local scope.
   */
  void exitLoop();
  /**
   * Returns true if the current (local) scope indicates a
   * non-zero loop depth.
   */
  bool inLoop();
  /**
   * Returns true if the given variable is global.
   */
  bool isVariableGlobal(const std::string& varName);
  /**
   * Returns true if the given variable is persistent
   */
  bool isVariablePersistent(const std::string& varName);
};

#endif
