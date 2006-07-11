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

#ifndef __SYMBOLTABLE_HPP__
#define __SYMBOLTABLE_HPP__
#include <string>
#include <vector>
#include "Types.hpp"

typedef std::string key_type;

#define SYMTAB 128

template<class T>
class SymbolTable {
  typedef T value_type;
  struct Entry {
    key_type key;
    value_type val;
    Entry* next;
    Entry(key_type k, value_type v, Entry *n) :
      key(k), val(v), next(n) { }
  };
    
  Entry* hashTable[SYMTAB];
    
  size_t hashKey(const key_type& key) {
    size_t res = 0;
    const char *p = key.c_str();
    while (*p) res = (res << 1) ^ (*p++);
    return res;
  }
    
public:
  SymbolTable() {
    memset(hashTable,0,sizeof(Entry*)*SYMTAB);
  }

  ~SymbolTable() {
    for (int i=0;i<SYMTAB;i++) {
      if (hashTable[i] != NULL) {
	Entry *ptr, *nxt;
	ptr = hashTable[i];
	while (ptr) {
	  nxt = ptr;
	  ptr = ptr->next;
	  delete nxt;
	}
      }
    }
  }

  value_type* findSymbol(const key_type& key) {
    size_t i = hashKey(key)%SYMTAB; // Hash
    Entry* ptr;
    ptr = hashTable[i];
    while (ptr) {
      if (ptr->key == key) 
	return (&ptr->val);
      ptr = ptr->next;
    }
    return NULL;
  }

  void deleteSymbol(const key_type& key) {
    size_t i = hashKey(key)%SYMTAB; // Hash
    Entry *ptr;
    ptr = hashTable[i];
    if (!ptr) return;
    // Check for the first element in the table matching
    // the key.
    if (ptr->key == key) {
      hashTable[i] = ptr->next;
      delete ptr;
      return;
    }
    // No - its not, set a next pointer
    Entry *nxt;
    nxt = ptr->next;
    while (nxt != NULL) {
      if (nxt->key == key) {
	ptr->next = nxt->next;
	delete nxt;
	return;
      }
      nxt = nxt->next;
      ptr = ptr->next;
    }
  }

  void insertSymbol(const key_type& key, const value_type& val) {
    size_t i = hashKey(key)%SYMTAB; // Hash
    Entry *ptr;
    ptr = hashTable[i];
    if (!ptr) {
      hashTable[i] = new Entry(key,val,NULL);
      return;
    }
    while (ptr) {
      if (ptr->key == key) {
	ptr->val = val;
	return;
      }
      ptr = ptr->next;
    }
    hashTable[i] = new Entry(key,val,hashTable[i]);
  }
    
  stringVector getCompletions(const std::string& prefix) {
    stringVector retlist;
    // Search through the symbol table...
    for (int i=0;i<SYMTAB;i++) {
      if (hashTable[i] != NULL) {
	Entry *ptr;
	ptr = hashTable[i];
	while (ptr != NULL) {
#if (__GNUG__ == 2)
	  if ((ptr->key.length() >= prefix.length()) &&
	      (ptr->key.compare(prefix,0,prefix.length()) == 0))
	    retlist.push_back(ptr->key);
#else
	  if ((ptr->key.length() >= prefix.length()) &&
	      (ptr->key.compare(0,prefix.length(),prefix) == 0))
	    retlist.push_back(ptr->key);
#endif
	  ptr = ptr->next;
	}
      }
    }
    return retlist;
  }
};

#endif
