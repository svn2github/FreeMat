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

#ifndef __Scope_hpp__
#define __Scope_hpp__

/**
 * A Scope is a combination of a variable hashtable and a function hashtable.
 */
#include <string>

#include "Array.hpp"
#include "FunctionDef.hpp"
#include "SymbolTable.hpp"

namespace FreeMat {
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
  public:
    /**
     * Construct a scope with the given name.
     */
    Scope(std::string scopeName);
    /**
     * Default destructor.
     */
    ~Scope();
    /**
     * Insert a variable with the given name.  If the variable
     * already exists in the Scope, then the previous definition
     * is replaced with the given one.
     */
    void insertVariable(const std::string& varName, const Array& val);
    /**
     * Insert a function pointer into the current scope.  The name of
     * of the function is encoded in the FuncPtr.
     */
    void insertFunction(FuncPtr);
    /**
     * Delete a function from the current scope.
     */
    void deleteFunction(const std::string& funcName);
    /**
     * Lookup a function.  Return true if the function is defined, and
     * assigns the value of the function pointer to the second argument.
     */
    bool lookupFunction(std::string funcName, FuncPtr& val);
    /**
     * Lookup a variable.  Return true if the variable is defined, and
     * assigns the value of the variable to the second argument.
     */
    bool lookupVariable(const std::string& funcName, Array& val);
    /**
     * Add a variable name to the global variables list.
     */
    void addGlobalVariablePointer(std::string varName);
    /**
     * Delete a variable name from the global variables list.
     */
    void deleteGlobalVariablePointer(std::string varName);
    /**
     * Check to see if a variable is globally defined.
     */
    bool isVariableGlobal(const std::string& varName);
    /**
     * Add a variable name to the persistent variables list.
     */
    void addPersistentVariablePointer(std::string varName);
    /**
     * Delete a variable name from the persistent variables list.
     */
    void deletePersistentVariablePointer(std::string varName);
    /**
     * Check to see if a variable is defined in the persistent
     * list.
     */
    bool isVariablePersistent(const std::string& varName);
    /**
     * Mangle the name of a variable by prepending
     * a "_scopename_" to the name of the variable.
     */
    std::string getMangledName(std::string varName);
    /**
     * Dump the scope.
     */
    void printMe();
    /**
     * Dump only the variables in the scope (not the functions).
     */
    void printData();
    /**
     * Get the name of the scope.
     */
    std::string getName();
    /**
     * Increment the loop counter.
     */
    void enterLoop();
    /**
     * Decrement the loop counter.
     */
    void exitLoop();
    /**
     * Test the loop counter.
     */
    bool inLoop();
    /**
     * Get a list of all possible completions of the given
     * string.
     */
    stringVector getCompletions(const std::string& prefix);
    /**
     * Returns a list of all currently defined variables
     * in the active scope.
     */
    stringVector listAllVariables();
    /**
     * Delete a variable in this scope.  It does not simply
     * replace the variable with an empty variable, but deletes
     * the variable from the symbol table completely.
     */
    void deleteVariable(std::string var);
  };
}
#endif
