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
#include <QMutexLocker>
#include <QMutex>
#include "Serialize.hpp"

QMutex scopemutex;

void Scope::countlock() {
  QMutexLocker lockit(&scopemutex);
  refcount++;
}

void Scope::countunlock() {
  QMutexLocker lockit(&scopemutex);
  refcount--;
}
  
bool Scope::referenced() {
  QMutexLocker lockit(&scopemutex);
  return (refcount>0);
}

ScopePtr ThawScope(Serialize *s) {
  string name(s->getString());
  stringVector globalVars(s->getStringVector());
  stringVector persistentVars(s->getStringVector());
  stringVector variablesAccessed(s->getStringVector());
  stringVector localVariables(s->getStringVector());
  bool isNested(s->getBool());
  Scope *q = new Scope(name,isNested);
  q->setVariablesAccessed(variablesAccessed);
  q->setLocalVariables(localVariables);
  for (int i=0;i<persistentVars.size();i++)
    q->addPersistentVariablePointer(persistentVars[i]);
  for (int i=0;i<globalVars.size();i++)
    q->addGlobalVariablePointer(globalVars[i]);
  stringVector names(s->getStringVector());
  for (int i=0;i<names.size();i++) {
    bool arrayDefed = s->getBool();
    if (arrayDefed) {
      Array t;
      s->getArray(t);
      q->insertVariable(names[i],t);
    }
  }
  return ScopePtr(q);
}

void FreezeScope(ScopePtr scope, Serialize *s) {
  s->putString(scope->getName().c_str());
  s->putStringVector(scope->getGlobalVariablesList());
  s->putStringVector(scope->getPersistentVariablesList());
  s->putStringVector(scope->getVariablesAccessedList());
  s->putStringVector(scope->getLocalVariablesList());
  s->putBool(scope->isnested());
  stringVector names(scope->getCompletions(""));
  s->putStringVector(names);
  for (int i=0;i<names.size();i++) {
    Array* p = scope->lookupVariable(names[i]);
    if (p) {
      s->putBool(true);
      s->putArray(*p);
    } else {
      s->putBool(false);
    }
  }
}
