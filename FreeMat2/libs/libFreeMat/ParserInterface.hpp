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

#ifndef __ParserInterface_hpp__
#define __ParserInterface_hpp__

#include "AST.hpp"
#include "FunctionDef.hpp"
#include <stdio.h>

namespace FreeMat {
  /**
   * These are the different parser states that
   * the parser can be in after a successful parse:
   *    - ScriptBlock corresponds to a parse of a sequence
   *      of statements
   *    - FuncDef corresponds to a parse of a function def
   *    - ParseError corresponds to a syntax error when 
   *      parsing.
   */
  typedef enum {
    ScriptBlock,
    FuncDef,
    ParseError
  } ParserState;
  /**
   * Reset the parser to its default state.
   */
  void resetParser();
  /**
   * Get the AST from a parse of a script or statement sequence.
   */
  ASTPtr getParsedScriptBlock();
  /**
   * Get the function definition from a parse of a function-definition.
   */
  MFunctionDef* getParsedFunctionDef();
  /**
   * Get the current parser state.
   */
  ParserState parseState();
  /**
   * Parse the given string.
   */
  ParserState parseString(char *);
  /**
   * Parse the given file (with the given filename).
   */
  ParserState parseFile(FILE*, const char*);
}

#endif
