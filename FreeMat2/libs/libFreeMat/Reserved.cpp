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

#include "Reserved.hpp"
#include <string.h>
#include <iostream>

namespace FreeMat {

  /**
   * NOTE: This list must be sorted alphabetically!
   */
  reservedWordStruct reservedWord[RESWORDCOUNT] = {
    {"break",BREAK,FM_BREAK},
    {"case",CASE,FM_CASE},
    {"catch",CATCH,FM_CATCH},
    {"continue",CONTINUE,FM_CONTINUE},
    {"else",ELSE,FM_ELSE},
    {"elseif",ELSEIF,FM_ELSEIF},
    {"end",END,FM_END},
    {"for",FOR,FM_FOR},
    {"function",FUNCTION,FM_FUNCTION},
    {"global",GLOBAL,FM_GLOBAL},
    {"if",IF,FM_IF},
    {"keyboard",KEYBOARD,FM_KEYBOARD},
    {"otherwise",OTHERWISE,FM_OTHERWISE},
    {"persistent",PERSISTENT,FM_PERSISTENT},
    {"quit",QUIT,FM_QUIT},
    {"retall",RETALL,FM_RETALL},
    {"return",RETURN,FM_RETURN},
    {"switch",SWITCH,FM_SWITCH},
    {"try",TRY,FM_TRY},
    {"while",WHILE,FM_WHILE}
  };

  int compareReservedWord(const void* a, const void* b) {
    return strcmp(((reservedWordStruct*)a)->word,
		  ((reservedWordStruct*)b)->word);
  }
}
