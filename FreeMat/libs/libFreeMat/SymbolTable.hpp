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

#ifndef __SYMBOLTABLE_HPP__
#define __SYMBOLTABLE_HPP__
#include <string>
#include <vector>
#include "Types.hpp"

typedef std::string key_type;

#define SYMTAB 4096

namespace FreeMat {
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

    bool findSymbol(const key_type& key, value_type& dest) {
      size_t i = hashKey(key)%SYMTAB; // Hash
      Entry* ptr;
      ptr = hashTable[i];
      while (ptr) {
	if (ptr->key == key) {
	  dest = ptr->val;
	  return true;
	}
	ptr = ptr->next;
      }
      return false;
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
}

#endif
