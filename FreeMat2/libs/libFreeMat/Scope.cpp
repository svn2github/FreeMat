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

#include "Scope.hpp"
#include "Array.hpp"
#include <stdio.h>
#include <algorithm>

namespace FreeMat {
#define MSGBUFLEN 2048
  static char msgBuffer[MSGBUFLEN];

  Scope::Scope(std::string scopeName) {
    name = scopeName;
    loopLevel = 0;
  }

  Scope::~Scope() {
    //   CodeTable::iterator it2 = codeTab.begin();
    //   while (it2 != codeTab.end()) {
    //     delete (it2->second);
    //     it2++;
    //   }
  }

  void Scope::insertFunction(FuncPtr a) {
    codeTab.insertSymbol(a->name,a);
  }

  void Scope::deleteFunction(const std::string& funcName) {
    codeTab.deleteSymbol(funcName);
  }

  bool Scope::lookupFunction(std::string funcName, FuncPtr& val) {
    return codeTab.findSymbol(funcName,val);
  }
  
  bool Scope::lookupVariable(const std::string& varName, Array& val) {
    return symTab.findSymbol(varName,val);
  }

  std::string Scope::getName() {
    return name;
  }

  void Scope::insertVariable(const std::string& varName, const  Array& var) {
    symTab.insertSymbol(varName,var);
  }

  void Scope::printMe() {
  }

  void Scope::printData() {
  }

  void Scope::enterLoop() {
    loopLevel++;
  }

  void Scope::exitLoop() {
    loopLevel--;
  }

  bool Scope::inLoop() {
    return (loopLevel>0);
  }

  void Scope::addGlobalVariablePointer(std::string varName) {
    if (!isVariableGlobal(varName))
      globalVars.push_back(varName);
  }

  void Scope::deleteGlobalVariablePointer(std::string varName) {
	  stringVector::iterator i = std::find(globalVars.begin(),
				    globalVars.end(),
				    varName);
    if (*i == varName)
      globalVars.erase(i);
  }

  bool Scope::isVariableGlobal(const std::string& varName) {
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

  void Scope::addPersistentVariablePointer(std::string varName) {
    if (!isVariablePersistent(varName))
      persistentVars.push_back(varName);
  }

  void Scope::deletePersistentVariablePointer(std::string varName) {
	  stringVector::iterator i = std::find(persistentVars.begin(),
				    persistentVars.end(),
				    varName);
    if (*i == varName)
      persistentVars.erase(i);
  }

  bool Scope::isVariablePersistent(const std::string& varName) {
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

  std::string Scope::getMangledName(std::string varName) {
    return (std::string("_") + name + std::string("_") + varName);
  }

  stringVector Scope::getCompletions(const std::string& prefix) {
    stringVector codecompletions;
    stringVector varcompletions;
    codecompletions = codeTab.getCompletions(prefix);
    varcompletions = symTab.getCompletions(prefix);
    codecompletions.insert(codecompletions.end(),
			   varcompletions.begin(),
			   varcompletions.end());
    return codecompletions;
  }
  
  stringVector Scope::listAllVariables() {
    stringVector names(symTab.getCompletions(""));
    int i;
    for (i=0;i<globalVars.size();i++)
      names.push_back(globalVars[i]);
    for (i=0;i<persistentVars.size();i++)
      names.push_back(persistentVars[i]);
    return names;
  }

  void Scope::deleteVariable(std::string var) {
    symTab.deleteSymbol(var);
  }
}
