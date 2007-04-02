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

#ifndef __Scope_hpp__
#define __Scope_hpp__

/**
 * A Scope is a combination of a variable hashtable and a function hashtable.
 */
#include <string>
#include <QMutex>

#include "Array.hpp"
#include "FunctionDef.hpp"
#include "SymbolTable.hpp"

typedef SymbolTable<Array> VariableTable;
typedef SymbolTable<FuncPtr> CodeTable;

/**
 * A Scope is a collection of functions and variables all visible
 * at some point in the execution.  The scope also keeps track of
 * the loop level, and a list of the global and persistent variables
 * relevant to the current scope.
 */
class Scope {
  /**
   * This is a mutex to protect the scope when multiple threads
   * have access to the scope.  For all scopes (except the global
   * one, this mutex is unused.
   */
  QMutex *mutex;
  /**
   * This is the hash-table of Array pointers that forms the
   * symbol table.  Each variable has a name associated with
   * it that must be unique to the Scope.  The Scope owns the
   * variables in the symbol table, and is responsible for
   * destructing them when destroyed.
   */
  VariableTable symTab;
  /**
   * This is a hash-table of function pointers.  The Scope does
   * _not_ own the function pointers, and it must not destroy
   * them when destructed.
   */
  CodeTable codeTab;
  /**
   * The name of the scope.
   */
  std::string name;
  /**
   * The loop level.  This is used to track the depth of nested
   * loops.
   */
  int loopLevel;
  /**
   * These are the global variables as defined in the current
   * scope.  Global variables are not stored in this Scope, but
   * are deferred to the top scope in the Context.
   */
  stringVector globalVars;
  /**
   * Persistent variables are similar to global variables in that
   * they are deferred to the top scope in the Context.  However,
   * unlike global variables, persistent variables are mangled
   * with the name of the scope before being indexed into the global 
   * scope.
   */
  stringVector persistentVars;
  /**
   * The location ID stack - stores information on where in the source
   * file the current token resides.
   */
  std::vector<int> IDstack;
  /**
   * On every call to modify the scope, we have to check the global/persistent
   * variable table.  This is generally expensive, so we cache information
   * about these tables being empty (the usual case).
   */
  bool anyPersistents;
  bool anyGlobals;
public:
  /**
   * Construct a scope with the given name.
   */
  Scope(std::string scopeName) : name(scopeName), loopLevel(0), 
				 anyPersistents(false), anyGlobals(false),
				 mutex(NULL)  {}
  /**
   * Lock the scope's mutex
   */
  inline void lock() {
    if (mutex) mutex->lock();
  }
  /**
   * Unlock the scope's mutex
   */
  inline void unlock() {
    if (mutex) mutex->unlock();
  }
  /**
   * Initialize the scope's mutex - only needed in Global
   * scope.
   */
  inline void mutexSetup() {
    if (mutex) 
      delete mutex;
    mutex = new QMutex(QMutex::Recursive);
  }
  /**
   * Insert a variable with the given name.  If the variable
   * already exists in the Scope, then the previous definition
   * is replaced with the given one.
   */
  inline void insertVariable(const std::string& varName, const Array& val) {
    symTab.insertSymbol(varName,val);
  }
  /**
   * Insert a function pointer into the current scope.  The name of
   * of the function is encoded in the FuncPtr.
   */
  inline void insertFunction(FuncPtr a) {
    FuncPtr *ret = codeTab.findSymbol(a->name);
    if (ret)
      delete ret;
    codeTab.insertSymbol(a->name,a);
  }
  /**
   * Delete a function from the current scope.
   */
  inline void deleteFunction(const std::string& funcName) {
    codeTab.deleteSymbol(funcName);
  }
  /**
   * Lookup a function.  Return true if the function is defined, and
   * assigns the value of the function pointer to the second argument.
   */
  inline bool lookupFunction(std::string funcName, FuncPtr& val) {
    FuncPtr* ret = codeTab.findSymbol(funcName);
    if (ret) {
      val = *ret;
      return true;
    }
    return false;
  }
  /**
   * Lookup a variable.  Return a pointer to the variable in the symbol 
   * table if found and NULL otherwise.  Different than lookupFunction
   * because in write-back assignments (e.g., A(:,346) = b) it is critical 
   * to manage the number of copies.
   */
  inline Array* lookupVariable(const std::string& varName) {
    return(symTab.findSymbol(varName));
  }
  /**
   * Add a variable name to the global variables list.
   */
  inline void addGlobalVariablePointer(std::string varName) {
    if (!isVariableGlobal(varName)) {
      globalVars.push_back(varName);
      anyGlobals = true;
    }
  }
  /**
   * Delete a variable name from the global variables list.
   */
  inline void deleteGlobalVariablePointer(std::string varName) {
    stringVector::iterator i = std::find(globalVars.begin(),
					 globalVars.end(),
					 varName);
    if (*i == varName)
      globalVars.erase(i);
    if (globalVars.empty()) anyGlobals = false;
  }
  /**
   * Check to see if a variable is globally defined.
   */
  inline bool isVariableGlobal(const std::string& varName) {
    if (!anyGlobals) return false;
    bool foundName = false;
    int i = 0;
    i = 0;
    if (globalVars.empty()) return false;
    while (i<globalVars.size() && !foundName) {
      foundName = (globalVars[i] == varName);
      if (!foundName) i++;
    }
    return foundName;
  }
  /**
   * Add a variable name to the persistent variables list.
   */
  inline void addPersistentVariablePointer(std::string varName) {
    if (!isVariablePersistent(varName)) {
      persistentVars.push_back(varName);
      anyPersistents = true;
    }
  }
  /**
   * Delete a variable name from the persistent variables list.
   */
  inline void deletePersistentVariablePointer(std::string varName) {
    stringVector::iterator i = std::find(persistentVars.begin(),
					 persistentVars.end(),
					 varName);
    if (*i == varName)
      persistentVars.erase(i);
    if (persistentVars.empty()) anyPersistents = false;
  }
  /**
   * Check to see if a variable is defined in the persistent
   * list.
   */
  inline bool isVariablePersistent(const std::string& varName) {
    if (!anyPersistents) return false;
    bool foundName = false;
    int i = 0;
    i = 0;
    if (persistentVars.empty()) return false;
    while (i<persistentVars.size() && !foundName) {
      foundName = (persistentVars[i] == varName);
      if (!foundName) i++;
    }
    return foundName;
  }
  /**
   * Mangle the name of a variable by prepending
   * a "_scopename_" to the name of the variable.
   */
  inline std::string getMangledName(std::string varName) {
    return (std::string("_") + name + std::string("_") + varName);
  }
  /**
   * Get the name of the scope.
   */
  inline std::string getName() {
    return name;
  }
  /**
   * Increment the loop counter.
   */
  inline void enterLoop() {
    loopLevel++;
  }
  /**
   * Decrement the loop counter.
   */
  inline void exitLoop() {
    loopLevel--;
  }
  /**
   * Test the loop counter.
   */
  inline bool inLoop() {
    return (loopLevel>0);
  }
  /**
   * Get a list of all possible completions of the given
   * string.
   */
  inline stringVector getCompletions(const std::string& prefix) {
    stringVector codecompletions;
    stringVector varcompletions;
    codecompletions = codeTab.getCompletions(prefix);
    varcompletions = symTab.getCompletions(prefix);
    codecompletions.insert(codecompletions.end(),
			   varcompletions.begin(),
			   varcompletions.end());
    return codecompletions;
  }
  /**
   * Returns a list of all functions
   *
   */
  inline stringVector listAllFunctions() {
    return codeTab.getCompletions("");
  }

  /**
   * Returns a list of all currently defined variables
   * in the active scope.
   */
  inline stringVector listAllVariables() {
    stringVector names(symTab.getCompletions(""));
    int i;
    for (i=0;i<globalVars.size();i++)
      names.push_back(globalVars[i]);
    for (i=0;i<persistentVars.size();i++)
      names.push_back(persistentVars[i]);
    return names;
  }
  /**
   * Delete a variable in this scope.  It does not simply
   * replace the variable with an empty variable, but deletes
   * the variable from the symbol table completely.
   */
  inline void deleteVariable(std::string var) {
    symTab.deleteSymbol(var);
  }
  /**
   * Clear the list of global variable names
   */
  inline void clearGlobalVariableList() {
    globalVars.clear();
    anyGlobals = false;
  }
  /**
   * Clear the list of persistent variable names
   */
  inline void clearPersistentVariableList() {
    persistentVars.clear();
    anyPersistents = false;
  }
};
#endif
