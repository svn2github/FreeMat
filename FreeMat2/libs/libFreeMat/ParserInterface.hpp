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
