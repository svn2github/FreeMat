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

#include "Scope.hpp"
#include "Array.hpp"
#include <stdio.h>
#include <algorithm>

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
  
bool Scope::lookupFunction(std::string funcName, FuncPtr& ptr) {
  FuncPtr* ret = codeTab.findSymbol(funcName);
  if (ret) {
    ptr = *ret;
    return true;
  }
  return false;
}
  
Array* Scope::lookupVariable(const std::string& varName) {
  return(symTab.findSymbol(varName));
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

void Scope::clearGlobalVariableList() {
  globalVars.clear();
}

void Scope::clearPersistentVariableList() {
  persistentVars.clear();
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

