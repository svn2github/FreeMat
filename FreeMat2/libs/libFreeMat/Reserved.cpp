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
