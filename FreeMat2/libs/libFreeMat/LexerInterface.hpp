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

#ifndef __LexerInterface_hpp__
#define __LexerInterface_hpp__

#include <stdio.h>

namespace FreeMat {
  /**
   * Set the string buffer to be processed by the lexer.
   */
  void setLexBuffer(char *buffer);
  /**
   * Set the FILE pointer for the file to be processed
   * by the lexer.
   */
  void setLexFile(FILE *fp);
  /**
   * Lex the file, and then check to see if more input is needed.
   */
  bool lexCheckForMoreInput(int pcount);
  /**
   * Set the contents of the lexical buffer, and reset the
   * continuationCount to zero.
   */
  void lexSetAdditionalInput(char *buffer);
  /**
   * Retrieve the contents of the continuationCount.
   */
  int getContinuationCount();
  /**
   * Get an error message that describes the current token
   */
  void GetLexTokenDescription(char *buffer);
}

#endif
