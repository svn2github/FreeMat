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

#define cnum(op,msg) case op: printf(msg); break;
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
    t->opNum = (OP_TYPE) s->getByte();
    t->text = s->getString();
    t->down = ThawAST(s);
    t->right = ThawAST(s);
    return t;
  }  
}
