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

#include "AST.hpp"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "Reserved.hpp"
#include "Serialize.hpp"
#include <stdio.h>

namespace FreeMat {

  AST::AST(void) {
    type = non_terminal;
    text = NULL;
    tokenNumber = 0;
    down = NULL;
    right = NULL;
    opNum = OP_NULL;
  }

  AST::AST(NODE_TYPE ntype, char* name, int context) {
    type = ntype;
    text = (char*) malloc(strlen(name)+1);
    strcpy(text,name);
    tokenNumber = 0;
    down = NULL;
    right = NULL;
    opNum = OP_NULL;
    m_context = context;
  }

  AST::AST(NODE_TYPE ntype, int token, int context) {
    type = ntype;
    tokenNumber = token;
    down = NULL;
    right = NULL;
    text = NULL;
    opNum = OP_NULL;
    m_context = context;
  }

  AST::AST(OP_TYPE op, AST* arg, int context) {
    type = non_terminal;
    text = NULL;
    tokenNumber = 0;
    down = arg;
    right = NULL;
    opNum = op;
    m_context = context;
  }

  AST::AST(OP_TYPE op, AST* lt, AST* rt, int context) {
    type = non_terminal;
    text = NULL;
    opNum = op;
    lt->right = rt;
    tokenNumber = 0;
    down = lt;
    right = NULL;
    m_context = context;
  }

  AST::AST(OP_TYPE op, AST* lt, AST* md, AST* rt, int context) {
    type = non_terminal;
    text = NULL;
    opNum = op;
    lt->right = md;
    md->right = rt;
    tokenNumber = 0;
    down = lt;
    right = NULL;
    m_context = context;
  }

  AST::~AST() {
    if (text != NULL) free(text);
  }

  int AST::context() {
    return m_context;
  }
  
  bool AST::match(OP_TYPE test) {
    return (test == opNum);
  }

  stringVector AST::toStringList() {
    stringVector res;
    res.push_back(text);
    if (down != NULL) {
      AST* cp;
      cp = down;
      while (cp != NULL) {
	res.push_back(cp->text);
	cp = cp->right;
      }
    }
    return res;
  }
  
  void AST::addChild(AST* arg) {
    if (down == NULL) {
      down = arg;
      arg->right = NULL;
    } else {
      AST* cp;
      cp = down;
      while (cp->right != NULL)
	cp = cp->right;
      cp->right = arg;
      arg->right = NULL;
    }
  }

  void AST::addPeer(AST* arg) {
    if (right == NULL) {
      right = arg;
      arg->right = NULL;
    } else {
      AST* cp;
      cp = right;
      while (cp->right != NULL)
	cp = cp->right;
      cp->right = arg;
      arg->right = NULL;
    }
  }

  int AST::peerCount() {
    AST* t;
    int count;
  
    count = 0;
    t = down;
    while (t!= NULL) {
      count++;
      t = t->right;
    }
    return count;
  }

  int AST::childCount() {
    AST* t;
    int count;

    count = 0;
    t = down;
    while (t != NULL) {
      count++;
      t = t->down;
    }
    return count;
  }

  bool AST::isEmpty() {
    return ((type == null_node) || (type == non_terminal && opNum == OP_NULL));
  }

  int tabLevel = 0;

  void outTabs() {
    for (int i=0;i<tabLevel;i++)
      printf("   ");
  }

#define cnum(op,msg) case op: printf("%s,<%d,%d>",msg,t->m_context >> 16, t->m_context & 0xffff); break;
  void printAST(ASTPtr t) {
    if (t==NULL) return;
    if (t->isEmpty()) return;
    outTabs();
    if (t->type == reserved_node)
      printf("<%d,%s>\r\n",t->tokenNumber,reservedWord[t->tokenNumber].word);
    else if (t->type == non_terminal) {
      switch (t->opNum) {
	cnum(OP_BLOCK,"block");
	cnum(OP_CASEBLOCK,"caseblock");
	cnum(OP_RHS,"rhs");
	cnum(OP_CSTAT,"cond. statement");
	cnum(OP_ELSEIFBLOCK,"elseifblock");
	cnum(OP_ASSIGN,"assign");
	cnum(OP_MULTICALL,"multicall");
	cnum(OP_COLON,":");
	cnum(OP_PLUS,"+");
	cnum(OP_SUBTRACT,"-");
	cnum(OP_TIMES,"*");
	cnum(OP_RDIV,"/");
	cnum(OP_LDIV,"\\");
	cnum(OP_OR,"|");
	cnum(OP_AND,"&");
	cnum(OP_LT,"<");
	cnum(OP_LEQ,"<=");
	cnum(OP_GT,">");
	cnum(OP_GEQ,">=");
	cnum(OP_EQ,"==");
	cnum(OP_NEQ,"!=");
	cnum(OP_DOT_TIMES,".*");
	cnum(OP_DOT_RDIV,"./");
	cnum(OP_DOT_LDIV,".\\");
	cnum(OP_NEG,"neg");
	cnum(OP_NOT,"~");
	cnum(OP_POWER,"^");
	cnum(OP_DOT_POWER,".^");
	cnum(OP_TRANSPOSE,"'");
	cnum(OP_DOT_TRANSPOSE,".'");
	cnum(OP_EMPTY,"empty []");
	cnum(OP_EMPTY_CELL,"empty {}");
	cnum(OP_PARENS,"()");
	cnum(OP_BRACES,"{}");
	cnum(OP_BRACKETS,"[]");
	cnum(OP_DOT,".");
	cnum(OP_ALL,"all");
	cnum(OP_INDEX_LIST,"index list");
	cnum(OP_ROW_DEF,"row def");
	cnum(OP_SEMICOLON,";");
	cnum(OP_NULL,"null");
	cnum(OP_RSTATEMENT,"end stat.");
	cnum(OP_QSTATEMENT,"end quiet stat.");
	cnum(OP_DEBUG_RSTATEMENT,"debug end stat.");
	cnum(OP_DEBUG_QSTATEMENT,"debug end quiet stat.");
	cnum(OP_SCALL,"special call");
	cnum(OP_KEYWORD,"keyword");
	cnum(OP_DOTDYN,".()");
	cnum(OP_ADDRESS,"@");
      }
      printf("\r\n");
    } else if (t->type == id_node) {
      printf("ident: %s\r\n",t->text);
    } else if (t->type == const_int_node) {
      printf("int: %s\r\n",t->text);
    } else if (t->type == const_double_node) {
      printf("double: %s\r\n",t->text);
    } else if (t->type == const_float_node) {
      printf("float: %s\r\n",t->text);
    } else if (t->type == const_dcomplex_node) {
      printf("double: %si\r\n",t->text);
    } else if (t->type == const_complex_node) {
      printf("float: %si\r\n",t->text);
    } else if (t->type == string_const_node) {
      printf("string: '%s'\r\n",t->text);
    } else if (t->type == null_node) {
    } else {
      printf("context: %s\r\n",t->text);
    }
    tabLevel++;
    printAST(t->down);
    tabLevel--;
    printAST(t->right);  
  }  
  
  void FreezeAST(ASTPtr t, Serialize *s) {
    if (t == NULL) {
      s->putByte(0);
      return;
    }
    s->putByte(1);
    s->putByte(t->type);
    s->putInt(t->tokenNumber);
    s->putInt(t->m_context);
    s->putByte(t->opNum);
    s->putString(t->text);
    FreezeAST(t->down,s);
    FreezeAST(t->right,s);
  }

  ASTPtr ThawAST(Serialize *s) {
    char flag;
    flag = s->getByte();
    if (!flag)
      return NULL;
    ASTPtr t = new AST;
    t->type = (NODE_TYPE) s->getByte();
    t->tokenNumber = s->getInt();
    t->m_context = s->getInt();
    t->opNum = (OP_TYPE) s->getByte();
    t->text = s->getString();
    t->down = ThawAST(s);
    t->right = ThawAST(s);
    return t;
  }  

  void TagWithDebug(ASTPtr t, int context) {
    if (t==NULL) return;
    if (t->isEmpty()) return;
    if ((t->type == non_terminal)&& 
	((t->opNum == OP_RSTATEMENT) ||
	 (t->opNum == OP_QSTATEMENT) ||
	 (t->opNum == OP_DEBUG_RSTATEMENT) ||
	 (t->opNum == OP_DEBUG_QSTATEMENT)) &&
	(t->m_context == context)) {
      if (t->opNum == OP_RSTATEMENT)
	t->opNum = OP_DEBUG_RSTATEMENT;
      if (t->opNum == OP_QSTATEMENT)
	t->opNum = OP_DEBUG_QSTATEMENT;
    }
    TagWithDebug(t->right,context);
    TagWithDebug(t->down,context);
  }

  void TagWithNoDebug(ASTPtr t, int context) {
    if (t==NULL) return;
    if (t->isEmpty()) return;
    if ((t->type == non_terminal)&& 
	((t->opNum == OP_RSTATEMENT) ||
	 (t->opNum == OP_QSTATEMENT) ||
	 (t->opNum == OP_DEBUG_RSTATEMENT) ||
	 (t->opNum == OP_DEBUG_QSTATEMENT)) &&
	(t->m_context == context)) {
      if (t->opNum == OP_DEBUG_RSTATEMENT)
	t->opNum = OP_RSTATEMENT;
      if (t->opNum == OP_DEBUG_QSTATEMENT)
	t->opNum = OP_QSTATEMENT;
    }
    TagWithNoDebug(t->right,context);
    TagWithNoDebug(t->down,context);
  }

  int SearchASTMinimumContext(ASTPtr t, int lineNo, int contextval) {
    int bestcontext = contextval;
    if (t==NULL) return contextval;
    if (t->isEmpty()) return contextval;
    if (t->type == non_terminal && 
	((t->opNum == OP_RSTATEMENT) ||
	 (t->opNum == OP_QSTATEMENT) ||
	 (t->opNum == OP_DEBUG_RSTATEMENT) ||
	 (t->opNum == OP_DEBUG_QSTATEMENT))) {
      // decompose the context into a line number and position
      int my_lineno = t->m_context & 0xffff;
      int my_column = t->m_context >> 16;
      // decompose the arg context into a line number and position
      int arg_lineno = contextval & 0xffff;
      int arg_column = contextval >> 16;
      // Check to see if we are better than the arg context
      if (((my_lineno >= lineNo) && (my_lineno < arg_lineno)) ||
	  ((my_lineno == arg_lineno) && (my_column < arg_column))) 
	bestcontext = t->m_context;
      else
	bestcontext = contextval;
    }
    bestcontext = SearchASTMinimumContext(t->down, lineNo, bestcontext);
    bestcontext = SearchASTMinimumContext(t->right, lineNo, bestcontext);
    return bestcontext;
  }

  void SetASTBreakPoint(ASTPtr t, int lineNumber) {
    // First, do a recursive search for all statements
    // that match the target line number, recording
    // the minimum context value for each
    int contextMinimum = SearchASTMinimumContext(t, lineNumber, 0xffffffff);
    if (contextMinimum == 0xffffffff) return;
    // Found a closest match...  Tag the node (should be unique)
    TagWithDebug(t,contextMinimum);
  }

  bool CheckASTBreakPoint(ASTPtr t, int lineNumber) {
    int contextMinimum = SearchASTMinimumContext(t, lineNumber, 0xffffffff);
    if (contextMinimum == 0xffffffff) return false;
    return true;
  }

  void ClearASTBreakPoint(ASTPtr t, int lineNumber) {
    // First, do a recursive search for all statements
    // that match the target line number, recording
    // the minimum context value for each
    int contextMinimum = SearchASTMinimumContext(t, lineNumber, 0xffffffff);
    if (contextMinimum == 0xffffffff) return;
    // Found a closest match...  Tag the node (should be unique)
    TagWithNoDebug(t,contextMinimum);
  }

}
