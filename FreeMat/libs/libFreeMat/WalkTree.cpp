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

#include "WalkTree.hpp"
#include <iostream>
#include <stack>
#include <math.h>
#include <stdio.h>
#include "Exception.hpp"
#include "Math.hpp"
#include "Reserved.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include "ParserInterface.hpp"
#include "LexerInterface.hpp"
#include "Module.hpp"
#include "Interface.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include <signal.h>

namespace FreeMat {
  /**
   * Pending control-C
   */
  bool InterruptPending;

  char* lasterr;
  int endValStackLength;
  int endValStack[1000];

#define DoBinaryOp(func) {Array a(expression(t->down)); Array b(expression(t->down->right)); return func(a,b);}
#define DoUnaryOp(func) {Array a(expression(t->down)); return func(a);}


  void sigInterrupt(int arg) {
    InterruptPending = true;
  }

  void WalkTree::setPrintLimit(int lim) {
    printLimit = lim;
  }
  
  int WalkTree::getPrintLimit() {
    return(printLimit);
  }

  void WalkTree::resetState() {
    state = FM_STATE_OK;
  }

  ArrayVector WalkTree::rowDefinition(ASTPtr t) throw(Exception) {
    if (!t->opNum ==(OP_SEMICOLON)) throw Exception("AST - syntax error!\n");
    ASTPtr s = t->down;
    return expressionList(s,NULL);
  }

  Array WalkTree::matrixDefinition(ASTPtr t) throw(Exception) {
    ArrayMatrix m;
    if (!t->opNum ==(OP_BRACKETS)) throw Exception("AST - syntax error!\n");
    ASTPtr s = t->down;
  
    while (s != NULL) {
      m.push_back(rowDefinition(s));
      s = s->right;
    }
    return Array::matrixConstructor(m);
  }

  Array WalkTree::cellDefinition(ASTPtr t) throw(Exception) {
    ArrayMatrix m;
    if (!t->opNum ==(OP_BRACES)) throw Exception("AST - syntax error!\n");
    ASTPtr s = t->down;

    while (s != NULL) {
      m.push_back(rowDefinition(s));
      s = s->right;
    }
    return Array::cellConstructor(m);
  }

  Array WalkTree::expression(ASTPtr t) throw(Exception) {
    if (t->type == const_int_node)
      return Array::int32Constructor(atoi(t->text));
    if (t->type == const_float_node)
      return Array::floatConstructor(atof(t->text));
    if (t->type == const_double_node)
      return Array::doubleConstructor(atof(t->text));
    if (t->type == string_const_node)
      return Array::stringConstructor(std::string(t->text));
    if (t->type == reserved_node)
      if (t->tokenNumber == FM_END) {
	if (endValStackLength == 0) throw Exception("END keyword illegal!\n");
	return Array::int32Constructor(endValStack[endValStackLength-1]);
      }
    switch (t->opNum) {
    case OP_COLON:
      if ((t->down != NULL) && (t->down->opNum ==(OP_COLON)))
	return doubleColon(t);
      else
	return unitColon(t);
    case OP_EMPTY: { return Array::emptyConstructor();}
    case OP_EMPTY_CELL: {
      Array a(Array::emptyConstructor());
      a.promoteType(FM_CELL_ARRAY);
      return a;
    }
    case OP_BRACKETS: { return matrixDefinition(t); }
    case OP_BRACES: { return cellDefinition(t); }
    case OP_PLUS: {  DoBinaryOp(Add); }
    case OP_SUBTRACT: {  DoBinaryOp(Subtract); }
    case OP_TIMES: {  DoBinaryOp(Multiply); }
    case OP_RDIV: {  DoBinaryOp(RightDivide); }
    case OP_LDIV: {  DoBinaryOp(LeftDivide); }
    case OP_OR: {  DoBinaryOp(Or); }
    case OP_AND: {  DoBinaryOp(And); }
    case OP_LT: {  DoBinaryOp(LessThan); }
    case OP_LEQ: { DoBinaryOp(LessEquals); }
    case OP_GT: {  DoBinaryOp(GreaterThan); }
    case OP_GEQ: { DoBinaryOp(GreaterEquals); }
    case OP_EQ: { DoBinaryOp(Equals); }
    case OP_NEQ: { DoBinaryOp(NotEquals); }
    case OP_DOT_TIMES: { DoBinaryOp(DotMultiply); }
    case OP_DOT_RDIV: { DoBinaryOp(DotRightDivide); }
    case OP_DOT_LDIV: { DoBinaryOp(DotLeftDivide); }
    case OP_NEG: { DoUnaryOp(Negate); }
    case OP_NOT: { DoUnaryOp(Not); }
    case OP_POWER: { DoBinaryOp(Power); }
    case OP_DOT_POWER: { DoBinaryOp(DotPower); }
    case OP_TRANSPOSE: { DoUnaryOp(Transpose); }
    case OP_DOT_TRANSPOSE: { DoUnaryOp(DotTranspose); }
    case OP_RHS: {
      // Test for simple variable lookup
      if (t->down->down == NULL)
	return(rhsExpressionSimple(t->down));
      else {
	ArrayVector m(rhsExpression(t->down));
	if (m.empty())
	  return Array::emptyConstructor();
	else
	  return m[0];
      }
    }
    default:
      throw Exception("Unrecognized expression!");
    }
  }

  Array WalkTree::unitColon(ASTPtr t) {
    Array a, b;

    a = expression(t->down);
    b = expression(t->down->right);
    return UnitColon(a,b);
  }

  Array WalkTree::doubleColon(ASTPtr t) {
    Array a, b, c;

    a = expression(t->down->down);
    b = expression(t->down->down->right);
    c = expression(t->down->right);
    return DoubleColon(a,b,c);
  }

  /**
   * An expressionList allows for expansion of cell-arrays
   * and structure arrays.  Works by first screening rhs-expressions
   * through rhsExpression, which can return
   * a vector of variables.
   */
  ArrayVector WalkTree::expressionList(ASTPtr t, Dimensions* dim) throw(Exception) {
    ArrayVector m;
    ArrayVector n;
    ASTPtr root;
    int index, tmp;
    int endVal;

    root = t;
    index = 0;
    while (t != NULL) {
      if (t->type == non_terminal && t->opNum ==(OP_RHS)) {
	try {
	  n = rhsExpression(t->down);
	} catch (Exception& e) {
	  if (!e.matches("Empty expression!"))
	    throw;
	  else
	    n = ArrayVector();
	}
	for (int i=0;i<n.size();i++)
	  m.push_back(n[i]);
      } else if (t->type == non_terminal && t->opNum ==(OP_ALL)) {
	if (dim == NULL)
	  throw Exception("Illegal use of the ':' keyword in indexing expression");
	if (root->right == NULL) {
	  // Singleton reference, with ':' - return 1:length as column vector...
	  tmp = dim->getElementCount();
	  m.push_back(Array::int32RangeConstructor(1,1,tmp,true));
	} else {
	  tmp = dim->getDimensionLength(index);
	  m.push_back(Array::int32RangeConstructor(1,1,tmp,false));
	}
      } else {
	// Set up the value of the "end" token
	if (dim == NULL)
	  endVal = -1;
	else if (root->right == NULL) 
	  endVal = dim->getElementCount();
	else
	  endVal = dim->getDimensionLength(index);
	// Push it onto the stack
	endValStack[endValStackLength] = endVal;
	endValStackLength++;
	// Call the expression
	m.push_back(expression(t));
	// Pop the endVal stack
	endValStackLength--;
      }
      index++;
      t = t->right;
    }
    return m;
  }

  bool WalkTree::conditionedStatement(ASTPtr t) throw(Exception){
    bool conditionState;
    if (!t->opNum ==(OP_CSTAT)) 
      throw Exception("AST - syntax error!\n");
    ASTPtr s = t->down;
    bool conditionTrue;
    Array condVar;
    condVar = expression(s);
    conditionState = !condVar.isRealAllZeros();
    ASTPtr codeBlock = s->right;
    if (conditionState) 
      block(codeBlock);
    return conditionState;
  }

  /**
   * This somewhat strange test is used by the switch statement.
   * If x is a scalar, and we are a scalar, this is an equality
   * test.  If x is a string and we are a string, this is a
   * strcmp test.  If x is a scalar and we are a cell-array, this
   * test is applied on an element-by-element basis, looking for
   * any matches.  If x is a string and we are a cell-array, then
   * this is applied on an element-by-element basis also.
   */
  bool WalkTree::testCaseStatement(ASTPtr t, Array s) throw(Exception){
    bool caseMatched;
    Array r;
    if (t->type != reserved_node || t->tokenNumber != FM_CASE) 
      throw Exception("AST- syntax error!\n");
    t = t->down;
    r = expression(t);
    caseMatched = s.testForCaseMatch(r);
    if (caseMatched)
      block(t->right);
    return caseMatched;
  }

  void WalkTree::tryStatement(ASTPtr t) {
    try {
      block(t);
    } catch (Exception &e) {
      t = t->right;
      if (t != NULL)
	block(t);
    } 
  }

  void WalkTree::switchStatement(ASTPtr t) throw(Exception){
    Array switchVal;

    // First, extract the value to perform the switch on.
    switchVal = expression(t);
    // Assess its type to determine if this is a scalar switch
    // or a string switch.
    if (!switchVal.isScalar() && !switchVal.isString())
      throw Exception("Switch statements support scalar and string arguments only.\n");
    // Move to the next node in the AST
    t = t->right;
    // Check for additional conditions
    if (t==NULL) return;
    bool caseMatched = false;
    if (t->opNum ==(OP_CASEBLOCK)) {
      ASTPtr s = t->down;
      while (!caseMatched && s != NULL) {
	caseMatched = testCaseStatement(s,switchVal);
	if (state == FM_STATE_ERROR || state == FM_STATE_EXCEPTION) return;
	s = s->right;
      }
    }
    t = t->right;
    if (caseMatched || (t == NULL)) return;
    // Do the "otherwise" code
    block(t);
  }

  void WalkTree::ifStatement(ASTPtr t) {
    bool elseifMatched;

    if (conditionedStatement(t)) return;
    t = t->right;
    // Check for additional conditions
    if (t == NULL) return;
    elseifMatched = false;
    if (t->opNum ==(OP_ELSEIFBLOCK)) {
      ASTPtr s = t->down;
      while (!elseifMatched && s != NULL) {
	elseifMatched = conditionedStatement(s);
	s = s->right;
      }
      t = t->right;
    }
    if (elseifMatched || t == NULL) return;
    return block(t);
  }

  void WalkTree::whileStatement(ASTPtr t) {
    ASTPtr testCondition;
    Array condVar;
    ASTPtr codeBlock;
    bool conditionTrue;
    bool breakEncountered;
  
    testCondition = t;
    codeBlock = t->right;
    breakEncountered = false;
    condVar = expression(testCondition);
    conditionTrue = !condVar.isRealAllZeros();
    context->enterLoop();
    while (conditionTrue && !breakEncountered) {
      block(codeBlock);
      if (state == FM_STATE_RETURN || 
	  state == FM_STATE_RETALL ||
	  state == FM_STATE_QUIT) break;
      if (state == FM_STATE_CONTINUE) 
	state = FM_STATE_OK;
      breakEncountered = (state == FM_STATE_BREAK);
      if (!breakEncountered) {
	condVar = expression(testCondition);
	conditionTrue = !condVar.isRealAllZeros();
      } else 
	state = FM_STATE_OK;
    }
    context->exitLoop();
  }

  void WalkTree::forStatement(ASTPtr t) {
    ASTPtr  codeBlock;
    Array indexSet;
    Array indexNum;
    char    *indexVarName;
    Array indexVar;
    int     elementNumber;
    int     elementCount;

    /* Get the name of the indexing variable */
    indexVarName = t->text;
    /* Evaluate the index set */
    indexSet = expression(t->down);
    /* Get the code block */
    codeBlock = t->right;
    elementCount = indexSet.getLength();
    context->enterLoop();
    for (elementNumber=0;elementNumber < elementCount;elementNumber++) {
      indexNum = Array::int32Constructor(elementNumber+1);
      indexVar = indexSet.getVectorSubset(indexNum);
      context->insertVariable(indexVarName,indexVar);
      block(codeBlock);
      if (state == FM_STATE_RETURN || 
	  state == FM_STATE_RETALL ||
	  state == FM_STATE_QUIT) {
	break;
      }
      if (state == FM_STATE_CONTINUE) state = FM_STATE_OK;
      if (state == FM_STATE_BREAK) {
	state = FM_STATE_OK;
	break;
      }
    }
    context->exitLoop();
  }

  void WalkTree::globalStatement(ASTPtr t) {
    while (t!=NULL) {
      context->addGlobalVariable(t->text);
      t = t->down;
    }
  }

  void WalkTree::persistentStatement(ASTPtr t) {
    while (t!=NULL) {
      context->addPersistentVariable(t->text);
      t = t->down;
    }
  }

  void WalkTree::statementType(ASTPtr t, bool printIt) throw(Exception){
    ArrayVector m;
    FunctionDef *fdef;

    if (t->isEmpty()) {
      /* Empty statement */
    } else if (t->opNum ==(OP_ASSIGN)) {
      if (t->down->down == NULL) {
	Array b(expression(t->down->right));
	context->insertVariable(t->down->text,b);
	if (printIt) {
	  interface->outputMessage(t->down->text);
	  interface->outputMessage(" = \n");
	  b.printMe(printLimit,interface->getTerminalWidth());
	}	  
      } else {
	Array expr(expression(t->down->right));
	Array c(assignExpression(t->down,expr));
	context->insertVariable(t->down->text,c);
	if (printIt) {
	  interface->outputMessage(t->down->text);
	  interface->outputMessage(" = \n");
	  c.printMe(printLimit,interface->getTerminalWidth());
	}
      }
    } else if (t->opNum ==(OP_MULTICALL)) {
      multiFunctionCall(t->down,printIt);
    } else if (t->opNum ==(OP_SCALL)) {
      specialFunctionCall(t->down,printIt);
    } else if (t->type == reserved_node) {
      switch (t->tokenNumber) {
      case FM_FOR:
	forStatement(t->down);
	break;
      case FM_WHILE:
	whileStatement(t->down);
	break;
      case FM_IF:
	ifStatement(t->down);
	break;
      case FM_BREAK:
	if (context->inLoop()) state = FM_STATE_BREAK;
	break;
      case FM_CONTINUE:
	if (context->inLoop()) state = FM_STATE_CONTINUE;
	break;
      case FM_RETURN:
	state = FM_STATE_RETURN;
	break;
      case FM_SWITCH:
	switchStatement(t->down);
	break;
      case FM_TRY:
	tryStatement(t->down);
	break;
      case FM_QUIT:
	state = FM_STATE_QUIT;
	break;
      case FM_RETALL:
	state = FM_STATE_RETALL;
	break;
      case FM_KEYBOARD:
	depth++;
	evalCLI();
	if (state != FM_STATE_QUIT &&
	    state != FM_STATE_RETALL)
	  state = FM_STATE_OK;
	depth--;
	break;
      case FM_GLOBAL:
	globalStatement(t->down);
	break;
      case FM_PERSISTENT:
	persistentStatement(t->down);
	break;
      default:
	throw Exception("Unrecognized statement type\n");
      }
    } else {
      // There is a special case to consider here - when a 
      // function call is made as a statement, we do not require
      // that the function have an output.
      Array b;
      if (t->opNum ==(OP_RHS) && 
	  !context->lookupVariable(t->down->text,b) &&
	  lookupFunctionWithRescan(t->down->text,fdef)) {
	m = functionExpression(fdef,t->down,1,true);
	if (m.size() == 0) 
	  b = Array::emptyConstructor();
	else 
	  b = m[0];
	if (printIt && (fdef->outputArgCount() != 0)) {
	  interface->outputMessage("ans = \n");
	  b.printMe(printLimit,interface->getTerminalWidth());
	} 
      }
      else if (t->opNum == OP_RHS) {
	m = rhsExpression(t->down);
	if (m.size() == 0)
	  b = Array::emptyConstructor();
	else {
	  b = m[0];
	  if (printIt) {
	    interface->outputMessage("ans = \n");
	    for (int j=0;j<m.size();j++) {
	      char buffer[1000];
	      if (m.size() > 1) {
		sprintf(buffer,"\n%d of %d:\n",j,m.size());
		interface->outputMessage(buffer);
	      }
	      m[j].printMe(printLimit,interface->getTerminalWidth());
	    }
	  }
	}
      } else {
	b = expression(t);
	if (printIt) {
	  interface->outputMessage("ans = \n");
	  b.printMe(printLimit,interface->getTerminalWidth());
	} 
      }
      if (state == FM_STATE_QUIT || 
	  state == FM_STATE_RETALL)
	return;
      context->insertVariable("ans",b);
    }
  }


  void WalkTree::statement(ASTPtr t) {
    if (t->opNum ==(OP_QSTATEMENT)) {
      if (t->down->type == context_node) {
	interface->setMessageContext(t->down->text);
	statementType(t->down->down,false);
      } else {
	interface->setMessageContext(NULL);
	statementType(t->down,false);
      }
    } else if (t->opNum ==(OP_RSTATEMENT)) {
      if (t->down->type == context_node) {
	interface->setMessageContext(t->down->text);
	statementType(t->down->down,true);
      } else {
	interface->setMessageContext(NULL);
	statementType(t->down,true);
      }
    }
  }

  void WalkTree::block(ASTPtr t) throw(Exception){
    try {
      ASTPtr s;
      s = t->down;
      if ((state != FM_STATE_QUIT) && (state != FM_STATE_RETALL))
	state = FM_STATE_OK;
      while (state != FM_STATE_RETALL &&
	     state != FM_STATE_QUIT && 
	     s != NULL) {
	if (InterruptPending) {
	  depth++;
	  InterruptPending = false;
	  evalCLI();
	  if (state != FM_STATE_QUIT &&
	      state != FM_STATE_RETALL)
	    state = FM_STATE_OK;
	  depth--;
	} else {
	  statement(s);
	  if (state == FM_STATE_BREAK || 
	      state == FM_STATE_CONTINUE ||
	      state == FM_STATE_RETURN ||
	      state == FM_STATE_RETALL ||
	      state == FM_STATE_QUIT) break;
	  s = s->right;
	}
      }
    } catch (Exception &e) {
      free(lasterr);
      lasterr = e.getMessageCopy();
      throw;
    }
  }

  Context* WalkTree::getContext() {
    return context;
  }

  Array WalkTree::simpleSubindexExpression(Array& r, ASTPtr t) {
    Dimensions rhsDimensions;
    ArrayVector m;

    rhsDimensions = r.getDimensions();
    if (t->opNum ==(OP_PARENS)) {
      m = expressionList(t->down,&rhsDimensions);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!\n");
      else if (m.size() == 1) {
	try {
	  return(r.getVectorSubset(m[0]));
	} catch (Exception &e) {
	  return(Array::emptyConstructor());
	}
      }
      else {
	try {
	  return(r.getNDimSubset(m));
	} catch (Exception &e) {
	  return(Array::emptyConstructor());
	}
      }
    }
    if (t->opNum ==(OP_BRACES)) {
      m = expressionList(t->down,&rhsDimensions);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!\n");
      else if (m.size() == 1) {
	try {
	  return(r.getVectorContents(m[0]));
	} catch (Exception &e) {
	  return(Array::emptyConstructor());
	}
      }
      else {
	try {
	  return(r.getNDimContents(m));
	} catch (Exception &e) {
	  return(Array::emptyConstructor());
	}
      }
    }
    if (t->opNum ==(OP_DOT)) {
      try {
	return(r.getField(t->down->text));
      } catch (Exception &e) {
	return(Array::emptyConstructor());
      }
    }
  }

  void WalkTree::simpleAssign(Array& r, ASTPtr t, Array& value) {
    ArrayVector vec;

    vec.push_back(value);
    simpleAssign(r,t,vec);
  }

  void WalkTree::simpleAssign(Array& r, ASTPtr t, ArrayVector& value) {
    Dimensions rhsDimensions;
    ArrayVector m;

    if (!r.isEmpty()) {
      rhsDimensions = r.getDimensions();
    } else if (t->opNum != OP_BRACES) {
      rhsDimensions = value[0].getDimensions();
    } else
      rhsDimensions.makeScalar();

    if (t->opNum ==(OP_PARENS)) {
      m = expressionList(t->down,&rhsDimensions);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!\n");
      else if (m.size() == 1) {
	r.setVectorSubset(m[0],value[0]);
	return;
      } else {
	r.setNDimSubset(m,value[0]);
	return;
      }
    }
    if (t->opNum ==(OP_BRACES)) {
      m = expressionList(t->down,&rhsDimensions);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!\n");
      else if (m.size() == 1) {
	r.setVectorContentsAsList(m[0],value);
	return;
      } else {
	r.setNDimContentsAsList(m,value);
	return;
      }
    }
    if (t->opNum ==(OP_DOT)) {
      r.setFieldAsList(t->down->text,value);
      return;
    }    
  }

  int WalkTree::countLeftHandSides(ASTPtr t) {
    Array lhs;
    if (!context->lookupVariable(t->text,lhs))
      lhs = Array::emptyConstructor();
    ASTPtr s = t->down;
    if (s == NULL) return 1;
    while (s->right != NULL) {
      if (!lhs.isEmpty())
	lhs = simpleSubindexExpression(lhs,s);
      s = s->right;
    }
    // We are down to the last subindexing expression...
    // We have to special case this one
    Dimensions rhsDimensions(lhs.getDimensions());
    ArrayVector m;
    if (s->opNum == (OP_PARENS)) {
	m = expressionList(s->down,&rhsDimensions);
	if (m.size() == 0)
	  throw Exception("Expected indexing expression!\n");
	if (m.size() == 1) {
	  // m[0] should have only one element...
	  m[0].toOrdinalType();
	  if (m[0].getLength() > 1)
	    throw Exception("Parenthetical expression in the left hand side of a function call must resolve to a single element.");
	  return (m[0].getLength());
	}
	else {
	  int i=0;
	  int outputCount=1;
	  while (i<m.size()) {
	    m[i].toOrdinalType();
	    outputCount *= m[i].getLength();
	    i++;
	  }
	  if (outputCount > 1)
	    throw Exception("Parenthetical expression in the left hand side of a function call must resolve to a single element.");
	  return (outputCount);
	}
    }
    if (s->opNum ==(OP_BRACES)) {
      m = expressionList(s->down,&rhsDimensions);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!\n");
      if (m.size() == 1) {
	// m[0] should have only one element...
	m[0].toOrdinalType();
	return (m[0].getLength());
      }
      else {
	int i=0;
	int outputCount=1;
	while (i<m.size()) {
	  m[i].toOrdinalType();
	  outputCount *= m[i].getLength();
	  i++;
	}
	return (outputCount);
      }
    }
    if (s->opNum ==(OP_DOT))
      return lhs.getLength();
    return 1;
  }

  Array WalkTree::assignExpression(ASTPtr t, Array& val) throw(Exception) {
    ArrayVector vec;

    vec.push_back(val);
    return assignExpression(t,vec);
  }
  
  // If we got this far, we must have at least one subindex
  Array WalkTree::assignExpression(ASTPtr t, ArrayVector& value) throw(Exception) {
    if (t->down == NULL) {
      Array retval(value[0]);
      value.erase(value.begin());
      return retval;
    }
    // Get the variable in question
    Array lhs;
    if (!context->lookupVariable(t->text,lhs))
      lhs = Array::emptyConstructor();
    // Set up a stack
    ArrayVector stack;
    ASTPtrVector ref;
    ASTPtr s = t->down;
    Array data;
    data = lhs;
    // Subindex
    while (s->right != NULL) {
      if (!data.isEmpty())
	data = simpleSubindexExpression(data,s);
      stack.push_back(data);
      ref.push_back(s);
      s = s->right;
    }
    // Do the assignment on the last temporary
    Array tmp(data);
    simpleAssign(tmp,s,value);
    Array rhs(tmp);
    if (stack.size() > 0) {
      stack.pop_back();
      // Now we have to "unwind" the stack
      while(stack.size() > 0) {
	// Grab the next temporary off the stack
	tmp = stack.back();
	// Make the assignment
	simpleAssign(tmp,ref.back(),rhs);
	// Assign this temporary to be the RHS of the next temporary
	rhs = tmp;
	// Pop the two stacks
	stack.pop_back();
	ref.pop_back();
      }
      // Complete the final assignment:
      // Last assignment is to lhs
      simpleAssign(lhs,ref.back(),tmp);
    } else
      lhs = tmp;
    return lhs;
  }

#if 0
  LeftHandSide WalkTree::lhsExpression(ASTPtr t) throw(Exception){
    Array n;
    ASTPtr s;
    ArrayVector m;
    int peerCnt;
    int dims;
    Dimensions rhsDimensions;

    // Get a pointer to the variable in the context
    if (!context->lookupVariable(t->text,n))
      n = Array::emptyConstructor();
    // Create a LHS object using this variable
    LeftHandSide r(n);
    // Process the subindexing expressions
    t = t->down;
    while (t!=NULL) { 
      rhsDimensions = r.getDimensions();
      if (t->opNum ==(OP_PARENS)) {
	m = expressionList(t->down,&rhsDimensions);
	if (m.size() == 0)
	  throw Exception("Expected indexing expression!\n");
	if (m.size() == 1)
	  r.subIndexVectorSubset(m[0]);
	else
	  r.subIndexNDimSubset(m);
      }
      if (t->opNum ==(OP_BRACES)) {
	m = expressionList(t->down,&rhsDimensions);
	if (m.size() == 0)
	  throw Exception("Expected indexing expression!\n");
	if (m.size() == 1)
	  r.subIndexVectorContents(m[0]);
	else
	  r.subIndexNDimContents(m);
      }
      if (t->opNum ==(OP_DOT)) {
	r.subIndexField(t->down->text);
      }
      t = t->right;
    }
    return r;
  }
#endif

  void WalkTree::specialFunctionCall(ASTPtr t, bool printIt) throw(Exception){
    FunctionDef *fptr;
    ASTPtr fAST;
    ArrayVector m;

    if (!lookupFunctionWithRescan(t->text,fptr))
      throw Exception(std::string("Undefined function ") + t->text);
    m = functionExpression(fptr,t,0,false);
  }

  void WalkTree::multiFunctionCall(ASTPtr t, bool printIt) throw(Exception){
    ArrayVector m;
    ASTPtr s, fAST, saveLHS;
    Array c;
    int lhsSize;
    int lhsCounter;
    FunctionDef *fptr;
  
    fAST = t->right;
    if (!lookupFunctionWithRescan(fAST->text,fptr))
      throw Exception(std::string("Undefined function ") + fAST->text);
    if (!t->opNum ==(OP_BRACKETS)) 
      throw Exception("Illegal left hand side in multifunction expression");
    s = t->down;
    if (!s->opNum ==(OP_SEMICOLON))
      throw Exception("Illegal left hand side in multifunction expression");
    if (s->right != NULL)
      throw Exception("Multiple rows not allowed in left hand side of multifunction expression");
    // We have to make multiple passes through the LHS part of the AST.
    // The first pass is to count how many function outputs are actually
    // being requested. 
    // Calculate how many lhs objects there are
    lhsSize = s->peerCount();
    s = s->down;
    saveLHS = s;
    // Get the lhs objects into rset
    int lhsCount = 0;
    ASTPtr mptr = s;
    while (mptr != NULL) {
      int dmp = countLeftHandSides(mptr->down);
      lhsCount += dmp;
      mptr = mptr->right;
    }
    m = functionExpression(fptr,fAST,lhsCount,false);
    s = saveLHS;
    while ((s != NULL) && (m.size() > 0)) {
      Array c(assignExpression(s->down,m));
      context->insertVariable(s->down->text,c);
//       LeftHandSide r(lhsExpression(s->down));
//       c = r.assign(m);
//       context->insertVariable(s->down->text,c);
      if (printIt) {
	std::cout << s->down->text << " = \n";
	c.printMe(printLimit,interface->getTerminalWidth());
      }
      s = s->right;
    }
    if (s != NULL)
      interface->warningMessage("Warning! one or more outputs not assigned in call.");
  }

  int getArgumentIndex(stringVector list, std::string t) {
    bool foundArg = false;
    std::string q;
    uint32 i;
    i = 0;
    while (i<list.size() && !foundArg) {
      q = list[i];
      if (q[0] == '&')
	q.erase(0,1);
      foundArg = (q == t);
      if (!foundArg) i++;
    }
    if (foundArg)
      return i;
    else
      return -1;
  }

  ArrayVector WalkTree::functionExpression(FunctionDef *funcDef,
					   ASTPtr t, int narg_out, 
					   bool outputOptional) throw(Exception) {
    ArrayVector m, n;
    ASTPtr s, q, p;
    stringVector keywords;
    ArrayVector keyvals;
    ASTPtrVector keyexpr;
    int *keywordNdx;
    int *argTypeMap;

    funcDef->updateCode();
    if (funcDef->scriptFlag) {
      if (t->down != NULL)
	throw Exception(std::string("Cannot use arguments in a call to a script."));
      if ((narg_out > 0) && !outputOptional)
	throw Exception(std::string("Cannot assign outputs in a call to a script."));
      block(((MFunctionDef*)funcDef)->code);
    } else {
      // Look for arguments
      if (t->down != NULL) {
	s = t->down;
	if (s->opNum ==(OP_PARENS)) {
	  s = s->down;
	  // Search for the keyword uses - 
	  // To handle keywords, we make one pass through the arguments,
	  // recording a list of keywords used and using ::expression to
	  // evaluate their values. 
	  q = s;
	  while (q != NULL) {
	    if (q->opNum == OP_KEYWORD) {
	      keywords.push_back(q->down->text);
	      keyvals.push_back(expression(q->down->right));
	      keyexpr.push_back(q->down->right);
	    }
	    q = q->right;
	  }
 	  // If any keywords were found, make another pass through the
 	  // arguments and remove them.
	  if (keywords.size() > 0) {
// 	    if (funcDef->type() != FM_M_FUNCTION)
// 	      throw Exception("out of order argument passing only supported for M files");
	    while (s != NULL && s->opNum == OP_KEYWORD)
	      s = s->right;
	    if (s != NULL) {
	      q = s;
	      while (q->right != NULL) {
		if (q->right->opNum == OP_KEYWORD)
		  q->right = q->right->right;
		else
		  q = q->right;
	      }
	    }
	  }
	  m = expressionList(s,NULL);
	  // Check for keywords
	  if (keywords.size() > 0) {
	    // If keywords were used, we have to permute the
	    // entries of the arrayvector to the correct order.
	    stringVector arguments;
	    // Get the arguments from the MFunction pointer.
	    arguments = funcDef->arguments;
	    keywordNdx = new int[keywords.size()];
	    int maxndx;
	    maxndx = 0;
	    // Map each keyword to an argument number
	    for (int i=0;i<keywords.size();i++) {
	      int ndx;
	      ndx = getArgumentIndex(arguments,keywords[i]);
	      if (ndx == -1)
		throw Exception("out-of-order argument /" + keywords[i] + " is not defined in the called function!");
	      keywordNdx[i] = ndx;
	      if (ndx > maxndx) maxndx = ndx;
	    }
	    // Next, we have to determine how many "holes" there are
	    // in the argument list - we get the maximum list
	    int holes;
	    holes = maxndx + 1 - keywords.size();
	    // At this point, holes is the number of missing arguments
	    // If holes > m.size(), then the total number of arguments
	    // is just maxndx+1.  Otherwise, its 
	    // maxndx+1+(m.size() - holes)
	    int totalCount;
	    if (holes > m.size())
	      totalCount = maxndx+1;
	    else
	      totalCount = maxndx+1+(m.size() - holes);
	    // Next, we allocate a vector to hold the values
	    ArrayVector toFill(totalCount);
	    bool *filled;
	    filled = new bool[totalCount];
	    argTypeMap = new int[totalCount];
	    for (int i=0;i<totalCount;i++) {
	      filled[i] = false;
	      argTypeMap[i] = -1;
	    }
	    // Finally...
	    // Copy the keyword values in
	    for (int i=0;i<keywords.size();i++) {
	      toFill[keywordNdx[i]] = keyvals[i];
	      filled[keywordNdx[i]] = true;
	      argTypeMap[keywordNdx[i]] = i;
	    }
	    // Fill out the rest of the values from m
	    int n = 0;
	    int p = 0;
	    while (n < m.size()) {
	      if (!filled[p]) {
		toFill[p] = m[n];
		filled[p] = true;
		argTypeMap[p] = -2;
		n++;
	      } 
	      p++;
	    }
	    // Finally, fill in empty matrices for the
	    // remaining arguments
	    for (int i=0;i<totalCount;i++)
	      if (!filled[i])
		toFill[i] = Array::emptyConstructor();
	    // Clean up
	    delete[] filled;
	    // delete[] keywordNdx;
	    // Reassign
	    m = toFill;
	  }
	} else
	  throw Exception("Illegal expression in function expression");
      } else
	m = ArrayVector();
      if ((funcDef->inputArgCount() >= 0) && 
	  (m.size() > funcDef->inputArgCount()))
	throw Exception(std::string("Too many inputs to function ")+t->text);
      if ((funcDef->outputArgCount() >= 0) && 
	  (narg_out > funcDef->outputArgCount() && !outputOptional))
	throw Exception(std::string("Too many outputs to function ")+t->text);
      n = funcDef->evaluateFunction(this,m,narg_out);
      // Check for any pass by reference
      if (funcDef->arguments.size() > 0) {
	// Get the argument list
	stringVector arguments;
	arguments = funcDef->arguments;
	// M functions can modify their arguments
	q = s;
	// 	std::cout << "q tree...\n";
	// 	printAST(q);
	int maxsearch;
	maxsearch = m.size(); 
	if (maxsearch > arguments.size()) maxsearch = arguments.size();
	for (int i=0;i<maxsearch;i++) {
	  // Was this argument passed out of order?
	  if (argTypeMap[i] == -1) continue;
	  if ((keywords.size() > 0) && (argTypeMap[i] >=0))
	    p = keyexpr[argTypeMap[i]];
	  else {
	    p = q;
	    if (q != NULL)
	      q = q->right;
	  }
	  std::string args(arguments[i]);
	  if (args[0] == '&') {
	    args.erase(0,1);
	    // This argument was passed by reference
	    if (!(p->type == non_terminal && p->opNum == OP_RHS))
	      throw Exception("Must have lvalue in argument passed by reference");
	    if (p->down->down == NULL && p->down->type == id_node) {
	      context->insertVariable(p->down->text,m[i]);
	    } else {
	      Array c(assignExpression(p->down,m[i]));
	      context->insertVariable(p->down->text,c);
	    }
	  }
	}
      }
    }
    return n;
  }

  Interface* WalkTree::getInterface() {
    return interface;
  }

  bool WalkTree::lookupFunctionWithRescan(std::string funcName, FuncPtr& val) {
    bool isFun;
    isFun = context->lookupFunction(funcName,val);
    if (!isFun) {
      interface->rescanPath();
      isFun = context->lookupFunction(funcName,val);
    }
    return isFun;
  }

  Array WalkTree::rhsExpressionSimple(ASTPtr t) throw(Exception) {
    Array r;
    ArrayVector m;
    bool isVar;
    bool isFun;
    FunctionDef *funcDef;

    // Try to satisfy the rhs expression with what functions we have already
    // loaded.
    isVar = context->lookupVariable(t->text,r);
    if (isVar && (t->down == NULL)) {
      return r;
    }
    if (!isVar)
      isFun = lookupFunctionWithRescan(t->text,funcDef);
    if (!isVar && isFun) {
      m = functionExpression(funcDef,t,1,false);
      if (m.empty())
	return Array::emptyConstructor();
      else
	return m[0];
    }
    if (!isVar && !isFun)
      throw Exception(std::string("Undefined function or variable ") + t->text);
  }

  ArrayVector WalkTree::rhsExpression(ASTPtr t) throw(Exception){
    ASTPtr s;
    Array r, q;
    Array n, p;
    ArrayVector m;
    ArrayVector rv;
    int peerCnt;
    int dims;
    bool isVar;
    bool isFun;
    Dimensions rhsDimensions;
    FunctionDef *funcDef;

    // Try to satisfy the rhs expression with what functions we have already
    // loaded.
    isVar = context->lookupVariable(t->text,r);
    if (isVar && (t->down == NULL)) {
      ArrayVector rv;
      rv.push_back(r);
      return rv;
    }
    if (!isVar)
      isFun = lookupFunctionWithRescan(t->text,funcDef);
    if (!isVar && isFun) {
      m = functionExpression(funcDef,t,1,false);
      return m;
    }
    if (!isVar && !isFun)
      throw Exception(std::string("Undefined function or variable ") + t->text);
    t = t->down;
    while (t != NULL) {
      rhsDimensions = r.getDimensions();
      if (!rv.empty())
	throw Exception("Cannot reindex an expression that returns multiple values.");
      if (t->opNum ==(OP_PARENS)) {
	m = expressionList(t->down,&rhsDimensions);
	if (m.size() == 0)
	  throw Exception("Expected indexing expression!\n");
	else if (m.size() == 1) {
	  q = r.getVectorSubset(m[0]);
	  r = q;
	} else {
	  q = r.getNDimSubset(m);
	  r = q;
	}
      }
      if (t->opNum ==(OP_BRACES)) {
	m = expressionList(t->down,&rhsDimensions);
	if (m.size() == 0)
	  throw Exception("Expected indexing expression!\n");
	else if (m.size() == 1)
	  rv = r.getVectorContentsAsList(m[0]);
	else
	  rv = r.getNDimContentsAsList(m);
	if (rv.size() == 1) {
	  r = rv[0];
	  rv = ArrayVector();
	} else if (rv.size() == 0) {
	  throw Exception("Empty expression!");
	  r = Array::emptyConstructor();
	}
      }
      if (t->opNum ==(OP_DOT)) {
	rv = r.getFieldAsList(t->down->text);
	if (rv.size() <= 1) {
	  r = rv[0];
	  rv = ArrayVector();
	}
      }
      t = t->right;
    }
    if (rv.empty())
      rv.push_back(r);
    return rv;
  }

  WalkTree::WalkTree(Context* aContext, Interface* aInterface) {
    context = aContext;
    state = FM_STATE_OK;
    endValStackLength = 0;
    endValStack[endValStackLength] = 0;
    depth = 0;
    interface = aInterface;
    Array::setArrayIOInterface(interface);
    InterruptPending = false;
    signal(SIGINT,sigInterrupt);
    printLimit = 1000;
  }

  bool WalkTree::evaluateString(char *line) {
    ASTPtr tree;
    ParserState parserState;

    InterruptPending = false;
    try{
      parserState = parseString(line);
      switch (parserState) {
      case ScriptBlock:
	tree = getParsedScriptBlock();
	//	printAST(tree);
	try {
	  block(tree);
	  if (state == FM_STATE_RETURN) {
	    if (depth > 0) 
	      return true;
	  }
	  if (state == FM_STATE_QUIT)
	    return true;
	  if (state == FM_STATE_RETALL) {
	    if (depth > 0)
	      return true;
	    else {
	      state = FM_STATE_OK;
	    }
	  }
	} catch(Exception &e) {
	  e.printMe(interface);
	}
	break;
      case FuncDef:
      case ParseError:
	break;
      }
    } catch(Exception &e) {
      e.printMe(interface);
    }
    return false;
  }

  void WalkTree::evalCLI() {
    char *line;
    char dataline[4096];
    char prompt[20];
    int lastCount;

    if (depth == 0)
      sprintf(prompt,"--> ");
    else
      sprintf(prompt,"[%d] --> ",depth);

    while(1) {
      line = interface->getLine(prompt);
      if (!line)
	continue;
      // scan the line and tokenize it
      setLexBuffer(line);
      if (lexCheckForMoreInput(0)) {
	lastCount = getContinuationCount();
	// Need multiple lines..  This code is _really_ bad - I need
	// a better interface...
	strcpy(dataline,line);
	bool enoughInput = false;
	// Loop until we have enough input
	while (!enoughInput) {
	  // Get more text
	  line = interface->getLine("");
	  // User pressed ctrl-D (or equivalent) - stop looking for
	  // input
	  if (!line) 
	    enoughInput = true;
	  else {
	    // User didn't press ctrl-D - 
	    // tack the new text onto the dataline
	    strcat(dataline,line);
	    setLexBuffer(dataline);
	    // Update the check
	    enoughInput = !lexCheckForMoreInput(lastCount);
	    lastCount = getContinuationCount();
	    //	    strcat(dataline,line);
	    if (enoughInput) strcat(dataline,"\n");
	  }
	}
	line = dataline;
      }
      if (evaluateString(line)) return;
    }
  }

}
