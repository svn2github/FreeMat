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
#include <FL/Fl.H>
#include <errno.h>
#include "Class.hpp"

namespace FreeMat {

  stackentry::stackentry(std::string cntxt, std::string det, int id) :
    cname(cntxt), detail(det), tokid(id) {
  }

  stackentry::stackentry() {
  }

  stackentry::~stackentry() {
  }

  /**
   * Pending control-C
   */
  bool InterruptPending;

  /**
   * Stores the current array for which to apply the "end" expression to.
   */
  typedef struct {
    Array endArray;
    bool isvalid;
    int index;
    int count;
  } endData;
  
  std::vector<endData> endStack;
  

  int endValStackLength;
  int endValStack[1000];

  void sigInterrupt(int arg) {
    InterruptPending = true;
  }

  Array WalkTree::DoBinaryOperator(ASTPtr t, BinaryFunc fnc, 
				   std::string funcname) {
    Array a(expression(t->down));
    Array b(expression(t->down->right));
    if (!(a.isUserClass() || b.isUserClass())) 
       return fnc(a,b);
    return ClassBinaryOperator(a,b,funcname,this);
  }

  Array WalkTree::DoUnaryOperator(ASTPtr t, UnaryFunc fnc, 
				  std::string funcname) {
    Array a(expression(t->down));
    if (!a.isUserClass())
       return fnc(a);
    return ClassUnaryOperator(a,funcname,this);
  }

  void WalkTree::pushID(int a) {
    if (!cstack.empty()) 
      cstack.push_back(stackentry(cstack.back().cname,cstack.back().detail,a));
    else
      cstack.push_back(stackentry("base","base",a));
  }

  void WalkTree::popID() {
    if (!cstack.empty())
      cstack.pop_back();
    else
      io->outputMessage("IDERROR\n");
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
  
  void WalkTree::clearStacks() {
    //    cname = "base";
    cstack.clear();
    //    gstack.push_back(cname);
  }

  State WalkTree::getState() {
    return state;
  }

  ArrayVector WalkTree::rowDefinition(ASTPtr t) {
    pushID(t->context());
    if (t->opNum != OP_SEMICOLON) throw Exception("AST - syntax error!");
    ASTPtr s = t->down;
    ArrayVector retval(expressionList(s));
    popID();
    return retval;
  }

  //!
  //@Module MATRIX Matrix Definitions
  //@@Section VARIABLES
  //@@Usage
  //The matrix is the basic datatype of FreeMat.  Matrices can be
  //defined using the following syntax
  //@[
  //  A = [row_def1;row_def2;...,row_defN]
  //@]
  //where each row consists of one or more elements, seperated by
  //commas
  //@[
  //  row_defi = element_i1,element_i2,...,element_iM
  //@]
  //Each element can either be a scalar value or another matrix,
  //provided that the resulting matrix definition makes sense.
  //In general this means that all of the elements belonging
  //to a row have the same number of rows themselves, and that
  //all of the row definitions have the same number of columns.
  //Matrices are actually special cases of N-dimensional arrays
  //where @|N<=2|.  Higher dimensional arrays cannot be constructed
  //using the bracket notation described above.  The type of a
  //matrix defined in this way (using the bracket notation) is
  //determined by examining the types of the elements.  The resulting
  //type is chosen so no information is lost on any of the elements
  //(or equivalently, by choosing the highest order type from those
  //present in the elements).
  //@@Examples
  //Here is an example of a matrix of @|int32| elements (note that
  //untyped integer constants default to type @|int32|).
  //@<
  //A = [1,2;5,8]
  //@>
  //Now we define a new matrix by adding a column to the right of
  //@|A|, and using float constants.
  //@<
  //B = [A,[3.2f;5.1f]]
  //@>
  //Next, we add extend @|B| by adding a row at the bottom.  Note
  //how the use of an untyped floating point constant forces the
  //result to be of type @|double|
  //@<
  //C = [B;5.2,1.0,0.0]
  //@>
  //If we instead add a row of @|complex| values (recall that @|i| is
  //a @|complex| constant, not a @|dcomplex| constant)
  //@<
  //D = [B;2.0f+3.0f*i,i,0.0f]
  //@>
  //Likewise, but using @|dcomplex| constants
  //@<
  //E = [B;2.0+3.0*i,i,0.0]
  //@>
  //Finally, in FreeMat, you can construct matrices with strings
  //as contents, but you have to make sure that if the matrix has
  //more than one row, that all the strings have the same length.
  //@<
  //F = ['hello';'there']
  //@>
  //!
  Array WalkTree::matrixDefinition(ASTPtr t) {
    ArrayMatrix m;
    if (t->opNum != OP_BRACKETS) throw Exception("AST - syntax error!");
    ASTPtr s = t->down;
    pushID(s->context());
    while (s != NULL) {
      m.push_back(rowDefinition(s));
      s = s->right;
    }
    Array retval(Array::matrixConstructor(m));
    popID();
    return retval;
  }

  //!
  //@Module CELL Cell Array Definitions
  //@@Section VARIABLES
  //@@Usage
  //The cell array is a fairly powerful array type that is available
  //in FreeMat.  Generally speaking, a cell array is a heterogenous
  //array type, meaning that different elements in the array can 
  //contain variables of different type (including other cell arrays).
  //For those of you familiar with @|C|, it is the equivalent to the
  //@|void *| array.  The general syntax for their construction is
  //@[
  //   A = {row_def1;row_def2;...;row_defN}
  //@]
  //where each row consists of one or more elements, seperated by
  //commas
  //@[
  //  row_defi = element_i1,element_i2,...,element_iM
  //@]
  //Each element can be any type of FreeMat variable, including
  //matrices, arrays, cell-arrays, structures, strings, etc.  The
  //restriction on the definition is that each row must have the
  //same number of elements in it.
  //@@Examples
  //Here is an example of a cell-array that contains a number,
  //a string, and an array
  //@<
  //A = {14,'hello',[1:10]}
  //@>
  //Note that in the output, the number and string are explicitly
  //printed, but the array is summarized.
  //We can create a 2-dimensional cell-array by adding another
  //row definition
  //@<
  //B = {pi,i;e,-1}
  //@>
  //Finally, we create a new cell array by placing @|A| and @|B|
  //together
  //@<
  //C = {A,B}
  //@>
  //!
  Array WalkTree::cellDefinition(ASTPtr t) {
    ArrayMatrix m;
    if (t->opNum != OP_BRACES) throw Exception("AST - syntax error!");
    ASTPtr s = t->down;
    pushID(s->context());
    while (s != NULL) {
      m.push_back(rowDefinition(s));
      s = s->right;
    }
    Array retval(Array::cellConstructor(m));
    popID();
    return retval;
  }

  Array WalkTree::ShortCutOr(ASTPtr t) {
    pushID(t->context());
    Array a(expression(t->down));
    Array retval;
    if (!a.isScalar()) {
      retval = DoBinaryOperator(t,Or,"or");
    } else {
      // A is a scalar - is it true?
      a.promoteType(FM_LOGICAL);
      if (*((const logical*)a.getDataPointer()))
	retval = a;
      else 
	retval = DoBinaryOperator(t,Or,"or");
    }
    popID();
    return retval;
  }

  Array WalkTree::ShortCutAnd(ASTPtr t) {
    pushID(t->context());
    Array a(expression(t->down));
    Array retval;
    if (!a.isScalar()) {
      retval = DoBinaryOperator(t,And,"and");
    } else {
      // A is a scalar - is it false?
      a.promoteType(FM_LOGICAL);
      if (!*((const logical*)a.getDataPointer()))
	retval = a;
      else 
	retval = DoBinaryOperator(t,And,"and");
    }
    popID();
    return retval;
  }

  Array WalkTree::expression(ASTPtr t) {
    pushID(t->context());
    Array retval;
    if (t->type == const_int_node) {
      int iv;
      double fv;
      iv = strtol(t->text,NULL,10);
      if ((errno == ERANGE) && ((iv == LONG_MAX) || (iv == LONG_MIN))) {
	fv = strtod(t->text,NULL);
	retval = Array::doubleConstructor(fv);
      } else {
	retval = Array::int32Constructor(iv);
      }
    }
    else if (t->type == const_float_node) {
      retval = Array::floatConstructor(atof(t->text));
    }
    else if (t->type == const_double_node) {
      retval = Array::doubleConstructor(atof(t->text));
    }
    else if (t->type == string_const_node) {
      retval = Array::stringConstructor(std::string(t->text));
    }
    else if (t->type == reserved_node) {
      if (t->tokenNumber == FM_END) {
	if (endStack.empty())
	  throw Exception("END keyword illegal (end stack underflow)!");
	endData t(endStack.back());
	if (!t.isvalid)
	  throw Exception("END keyword illegal!");
	retval = EndReference(t.endArray,t.index,t.count);
      } else 
	throw Exception("Unrecognized reserved node in expression tree!");
    }
    else {
      switch (t->opNum) {
      case OP_COLON:
	if ((t->down != NULL) && (t->down->opNum ==(OP_COLON))) {
	  retval = doubleColon(t);
	} else {
	  retval = unitColon(t);
	}
	break;
      case OP_EMPTY: 
	{ 
	  retval = Array::emptyConstructor();
	}
	break;
      case OP_EMPTY_CELL: 
	{
	  Array a(Array::emptyConstructor());
	  a.promoteType(FM_CELL_ARRAY);
	  retval = a;
	}
	break;
      case OP_BRACKETS: 
	{ 
	  retval = matrixDefinition(t); 
	}
	break;
      case OP_BRACES: 
	{ 
	  retval = cellDefinition(t); 
	}
	break;
      case OP_PLUS: 
	{  
	  retval = DoBinaryOperator(t,Add,"plus"); 
	}
	break;
      case OP_SUBTRACT: 
	{  
	  retval = DoBinaryOperator(t,Subtract,"minus"); 
	}
	break;
      case OP_TIMES: 
	{  
	  retval = DoBinaryOperator(t,Multiply,"mtimes"); 
	}
	break;
      case OP_RDIV: 
	{  
	  retval = DoBinaryOperator(t,RightDivide,"mrdivide");
	}
	break;
      case OP_LDIV: 
	{  
	  retval = DoBinaryOperator(t,LeftDivide,"mldivide"); 
	}
	break;
      case OP_SOR: 
      case OP_OR: 
	{  
	  retval = ShortCutOr(t); 
	}
	break;
      case OP_AND: 
      case OP_SAND: 
	{  
	  retval = ShortCutAnd(t); 
	}
	break;
	{  
	  retval = DoBinaryOperator(t,Or,"or"); 
	}
	break;
	{  
	  retval = DoBinaryOperator(t,And,"and"); 
	}
	break;
      case OP_LT: 
	{  
	  retval = DoBinaryOperator(t,LessThan,"lt"); 
	}
	break;
      case OP_LEQ: 
	{ 
	  retval = DoBinaryOperator(t,LessEquals,"le"); 
	}
	break;
      case OP_GT: 
	{  
	  retval = DoBinaryOperator(t,GreaterThan,"gt"); 
	}
	break;
      case OP_GEQ: 
	{ 
	  retval = DoBinaryOperator(t,GreaterEquals,"ge"); 
	}
	break;
      case OP_EQ: 
	{ 
	  retval = DoBinaryOperator(t,Equals,"eq"); 
	}
	break;
      case OP_NEQ: 
	{ 
	  retval = DoBinaryOperator(t,NotEquals,"ne"); 
	}
	break;
      case OP_DOT_TIMES: 
	{ 
	  retval = DoBinaryOperator(t,DotMultiply,"times"); 
	}
	break;
      case OP_DOT_RDIV: 
	{ 
	  retval = DoBinaryOperator(t,DotRightDivide,"rdivide"); 
	}
	break;
      case OP_DOT_LDIV: 
	{ 
	  retval = DoBinaryOperator(t,DotLeftDivide,"ldivide"); 
	}
	break;
      case OP_NEG: 
	{ 
	  retval = DoUnaryOperator(t,Negate,"uminus"); 
	}
	break;
      case OP_NOT: 
	{ 
	  retval = DoUnaryOperator(t,Not,"not"); 
	}
	break;
      case OP_POWER: 
	{ 
	  retval = DoBinaryOperator(t,Power,"mpower"); 
	}
	break;
      case OP_DOT_POWER: 
	{ 
	  retval = DoBinaryOperator(t,DotPower,"power"); 
	}
	break;
      case OP_TRANSPOSE: 
	{ 
	  retval = DoUnaryOperator(t,Transpose,"ctranspose"); 
	}
	break;
      case OP_DOT_TRANSPOSE: 
	{ 
	  retval = DoUnaryOperator(t,DotTranspose,"transpose"); 
	}
	break;
      case OP_ADDRESS:
	{
	  FuncPtr val;
	  ArrayVector dummy;
	  if (!lookupFunction(t->down->text,val,dummy))
	    throw Exception("unable to resolve " + std::string(t->down->text) + 
			    " to a function call");
	  retval = Array::funcPtrConstructor(val);
	  break;
	}
      case OP_RHS: 
	{
	  // Test for simple variable lookup
	  if (t->down->down == NULL) {
	  retval = rhsExpressionSimple(t->down);
	  } else {
	    ArrayVector m(rhsExpression(t->down));
	    if (m.empty()) {
	      retval = Array::emptyConstructor();
	    } else {
	      retval = m[0];
	    }
	  }
	}
	break;
      default:
	throw Exception("Unrecognized expression!");
      }
    }
    popID();
    return retval;
  }

  //!
  //@Module COLON Index Generation Operator
  //@@Section OPERATORS
  //@@Usage
  //There are two distinct syntaxes for the colon @|:| operator - the two argument form
  //@[
  //  y = a : c
  //@]
  //and the three argument form
  //@[
  //  y = a : b : c
  //@]
  //The two argument form is exactly equivalent to @|a:1:c|.  The output @|y| is the vector
  //\[
  //  y = [a,a+b,a+2b,\ldots,a+nb]
  //\]
  //where @|a+nb <= c|.  There is a third form of the colon operator, the 
  //no-argument form used in indexing (see @|indexing| for more details).
  //@@Examples
  //Some simple examples of index generation.
  //@<
  //y = 1:4
  //@>
  //Now by half-steps:
  //@<
  //y = 1:.5:4
  //@>
  //Now going backwards (negative steps)
  //@<
  //y = 4:-.5:1
  //@>
  //If the endpoints are the same, one point is generated, regardless of the step size (middle argument)
  //@<
  //y = 4:1:4
  //@>
  //If the endpoints define an empty interval, the output is an empty matrix:
  //@<
  //y = 5:4
  //@>
  //!
  Array WalkTree::unitColon(ASTPtr t) {
    Array a, b;
    pushID(t->context());
    a = expression(t->down);
    b = expression(t->down->right);
    Array retval(UnitColon(a,b));
    popID();
    return retval;
  }

  Array WalkTree::doubleColon(ASTPtr t) {
    Array a, b, c;
    pushID(t->context());
    a = expression(t->down->down);
    b = expression(t->down->down->right);
    c = expression(t->down->right);
    Array retval(DoubleColon(a,b,c));
    popID();
    return retval;
  }

  /**
   * An expressionList allows for expansion of cell-arrays
   * and structure arrays.  Works by first screening rhs-expressions
   * through rhsExpression, which can return
   * a vector of variables.
   */

  /*
   * called to resolve var(expr,expr) = someval or
   *                   someval = var(expr,expr)
   */
  ArrayVector WalkTree::variableSubIndexExpressions(ASTPtr t, Array subroot) {
    ArrayVector m;
    ArrayVector n;
    ASTPtr root;
    int index, tmp, count;
    int endVal;
    if (t == NULL) return m;
    pushID(t->context());
    root = t;
    index = 0;
    count = 0;
    while (t != NULL) {
      t = t->right;
      count++;
    }
    t = root;
    while (t != NULL) {
      if (t->opNum == OP_KEYWORD) {
	t = t->right;
	continue;
      }
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
	m.push_back(AllColonReference(subroot,index,count));
      } else {
	endData Q;
	Q.endArray = subroot;
	Q.isvalid = true;
	Q.index = index;
	Q.count = count;
	endStack.push_back(Q);
	// Call the expression
	m.push_back(expression(t));
	endStack.pop_back();
      }
      index++;
      t = t->right;
    }
    popID();
    return m;
  }

  /*
   * 
   */
  ArrayVector WalkTree::expressionList(ASTPtr t) {
    ArrayVector m;
    ArrayVector n;
    ASTPtr root;
    int index, tmp;
    int endVal;
    if (t == NULL) return m;
    pushID(t->context());
    root = t;
    index = 0;
    while (t != NULL) {
      if (t->opNum == OP_KEYWORD) {
	t = t->right;
	continue;
      }
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
	throw Exception("Illegal use of the ':' keyword in indexing expression");
      } else {
// 	endData Q;
// 	Q.isvalid = false;
// 	endStack.push_back(Q);
	// Call the expression
	m.push_back(expression(t));
// 	endStack.pop_back();
      }
      index++;
      t = t->right;
    }
    popID();
    return m;
  }

  bool WalkTree::conditionedStatement(ASTPtr t) {
    bool conditionState;
    if (t->opNum != OP_CSTAT)
      throw Exception("AST - syntax error!");
    ASTPtr s = t->down;
    pushID(s->context());
    bool conditionTrue;
    Array condVar;
    condVar = expression(s);
    conditionState = !condVar.isRealAllZeros();
    ASTPtr codeBlock = s->right;
    if (conditionState) 
      block(codeBlock);
    popID();
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
  bool WalkTree::testCaseStatement(ASTPtr t, Array s) {
    bool caseMatched;
    Array r;
    pushID(t->context());
    if (t->type != reserved_node || t->tokenNumber != FM_CASE) 
      throw Exception("AST- syntax error!");
    t = t->down;
    r = expression(t);
    caseMatched = s.testForCaseMatch(r);
    if (caseMatched)
      block(t->right);
    popID();
    return caseMatched;
  }

  //!
  //@Module TRY-CATCH Try and Catch Statement
  //@@Section FLOW
  //@@Usage
  //The @|try| and @|catch| statements are used for error handling
  //and control.  A concept present in @|C++|, the @|try| and @|catch|
  //statements are used with two statement blocks as follows
  //@[
  //   try
  //     statements_1
  //   catch
  //     statements_2
  //   end
  //@]
  //The meaning of this construction is: try to execute @|statements_1|,
  //and if any errors occur during the execution, then execute the
  //code in @|statements_2|.  An error can either be a FreeMat generated
  //error (such as a syntax error in the use of a built in function), or
  //an error raised with the @|error| command.
  //@@Examples
  //Here is an example of a function that uses error control via @|try|
  //and @|catch| to check for failures in @|fopen|.
  //@{ read_file.m
  //function c = read_file(filename)
  //try
  //   fp = fopen(filename,'r');
  //   c = fgetline(fp);
  //   fclose(fp);
  //catch
  //   c = ['could not open file because of error :' lasterr]
  //end
  //@}
  //Now we try it on an example file - first one that does not exist,
  //and then on one that we create (so that we know it exists).
  //@<
  //read_file('this_filename_is_invalid')
  //fp = fopen('test_text.txt','w');
  //fprintf(fp,'a line of text\n');
  //fclose(fp);
  //read_file('test_text.txt')
  //@>
  //!
  void WalkTree::tryStatement(ASTPtr t) {
    // Turn off autostop for this statement block
    bool autostop_save = autostop;
    autostop = false;
    // Get the state of the IDnum stack and the
    // contextStack and the cnameStack
    int stackdepth;
    stackdepth = cstack.size();
    try {
      block(t);
    } catch (Exception &e) {
      while (cstack.size() > stackdepth) cstack.pop_back();
      //      cname = cstack.back().cname;
      t = t->right;
      if (t != NULL) {
	autostop = autostop_save;
	block(t);
      }
    } 
    autostop = autostop_save;
  }


  bool WalkTree::AutoStop() {
    return autostop;
  }
  

  void WalkTree::AutoStop(bool a) {
    autostop = a;
  }

  //!
  //@Module SWITCH Switch statement
  //@@Section FLOW
  //@@Usage
  //The @|switch| statement is used to selective execute code
  //based on the value of either scalar value or a string.
  //The general syntax for a @|switch| statement is
  //@[
  //  switch(expression)
  //    case test_expression_1
  //      statements
  //    case test_expression_2
  //      statements
  //    otherwise:
  //      statements
  //  end
  //@]
  //The @|otherwise| clause is optional.  Note that each test
  //expression can either be a scalar value, a string to test
  //against (if the switch expression is a string), or a 
  //@|cell-array| of expressions to test against.  Note that
  //unlike @|C| @|switch| statements, the FreeMat @|switch|
  //does not have fall-through, meaning that the statements
  //associated with the first matching case are executed, and
  //then the @|switch| ends.  Also, if the @|switch| expression
  //matches multiple @|case| expressions, only the first one
  //is executed.
  //@@Examples
  //Here is an example of a @|switch| expression that tests
  //against a string input:
  //@{ switch_test.m
  //function c = switch_test(a)
  //  switch(a)
  //    case {'lima beans','root beer'}
  //      c = 'food';
  //    case {'red','green','blue'}
  //      c = 'color';
  //    otherwise
  //      c = 'not sure';
  //  end
  //@}
  //Now we exercise the switch statements
  //@<
  //switch_test('root beer')
  //switch_test('red')
  //switch_test('carpet')
  //@>
  //!
  void WalkTree::switchStatement(ASTPtr t) {
    Array switchVal;
    pushID(t->context());
    // First, extract the value to perform the switch on.
    switchVal = expression(t);
    // Assess its type to determine if this is a scalar switch
    // or a string switch.
    if (!switchVal.isScalar() && !switchVal.isString())
      throw Exception("Switch statements support scalar and string arguments only.");
    // Move to the next node in the AST
    t = t->right;
    // Check for additional conditions
    if (t!=NULL) {
      bool caseMatched = false;
      if (t->opNum ==(OP_CASEBLOCK)) {
	ASTPtr s = t->down;
	while (!caseMatched && s != NULL) {
	  caseMatched = testCaseStatement(s,switchVal);
	  s = s->right;
	}
      }
      t = t->right;
      if (!(caseMatched || (t == NULL)))
      // Do the "otherwise" code
	block(t);
    }
    popID();
  }

  //!
  //@Module IF-ELSEIF-ELSE Conditional Statements
  //@@Section FLOW
  //@@Usage
  //The @|if| and @|else| statements form a control structure for
  //conditional execution.  The general syntax involves an @|if|
  //test, followed by zero or more @|elseif| clauses, and finally
  //an optional @|else| clause:
  //@[
  //  if conditional_expression_1
  //    statements_1
  //  elseif conditional_expression_2
  //    statements_2
  //  elseif conditional_expresiion_3
  //    statements_3
  //  ...
  //  else
  //    statements_N
  //  end
  //@]
  //Note that a conditional expression is considered true if 
  //the real part of the result of the expression contains
  //any non-zero elements (this strange convention is adopted
  //for compatibility with MATLAB).
  //@@Examples
  //Here is an example of a function that uses an @|if| statement
  //@{ if_test.m
  //function c = if_test(a)
  //  if (a == 1)
  //     c = 'one';
  //  elseif (a==2)
  //     c = 'two';
  //  elseif (a==3)
  //     c = 'three';
  //  else
  //     c = 'something else';
  //  end
  //@}
  //Some examples of @|if_test| in action:
  //@<
  //if_test(1)
  //if_test(2)
  //if_test(3)
  //if_test(pi)
  //@>
  //!
  void WalkTree::ifStatement(ASTPtr t) {
    bool elseifMatched;
    pushID(t->context());
    bool condStat = conditionedStatement(t);
    if (!condStat) {
      t = t->right;
      // Check for additional conditions
      if (t != NULL) {
	elseifMatched = false;
	if (t->opNum ==(OP_ELSEIFBLOCK)) {
	  ASTPtr s = t->down;
	  while (!elseifMatched && s != NULL) {
	    elseifMatched = conditionedStatement(s);
	    s = s->right;
	  }
	  t = t->right;
	}
	if (!(elseifMatched || t == NULL))
	  block(t);
      }
    }
    popID();
  }

  //!
  //@Module WHILE While Loop
  //@@Section FLOW
  //@@Usage
  //The @|while| loop executes a set of statements as long as
  //a the test condition remains @|true|.  The syntax of a 
  //@|while| loop is
  //@[
  //  while test_expression
  //     statements
  //  end
  //@]
  //Note that a conditional expression is considered true if 
  //the real part of the result of the expression contains
  //any non-zero elements (this strange convention is adopted
  //for compatibility with MATLAB).
  //@@Examples
  //Here is a @|while| loop that adds the integers from @|1|
  //to @|100|:
  //@<
  //accum = 0;
  //k=1;
  //while (k<100), accum = accum + k; k = k + 1; end
  //accum
  //@>
  //!
  void WalkTree::whileStatement(ASTPtr t) {
    ASTPtr testCondition;
    Array condVar;
    ASTPtr codeBlock;
    bool conditionTrue;
    bool breakEncountered;
    
    pushID(t->context());
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
    popID();
  }

  //!
  //@Module FOR For Loop
  //@@Section FLOW
  //@@Usage
  //The @|for| loop executes a set of statements with an 
  //index variable looping through each element in a vector.
  //The syntax of a @|for| loop is one of the following:
  //@[
  //  for (variable=expression)
  //     statements
  //  end
  //@]
  //Alternately, the parenthesis can be eliminated
  //@[
  //  for variable=expression
  //     statements
  //  end
  //@]
  //or alternately, the index variable can be pre-initialized
  //with the vector of values it is going to take:
  //@[
  //  for variable
  //     statements
  //  end
  //@]
  //The third form is essentially equivalent to @|for variable=variable|,
  //where @|variable| is both the index variable and the set of values
  //over which the for loop executes.  See the examples section for
  //an example of this form of the @|for| loop.
  //@@Examples
  //Here we write @|for| loops to add all the integers from
  //@|1| to @|100|.  We will use all three forms of the @|for|
  //statement.
  //@<
  //accum = 0;
  //for (i=1:100); accum = accum + i; end
  //accum
  //@>
  //The second form is functionally the same, without the
  //extra parenthesis
  //@<
  //accum = 0;
  //for i=1:100; accum = accum + i; end
  //accum
  //@>
  //In the third example, we pre-initialize the loop variable
  //with the values it is to take
  //!
  void WalkTree::forStatement(ASTPtr t) {
    ASTPtr  codeBlock;
    Array indexSet;
    Array indexNum;
    char    *indexVarName;
    Array indexVar;
    int     elementNumber;
    int     elementCount;
    pushID(t->context());
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
    popID();
  }

  //!
  //@Module GLOBAL Global Variables
  //@@Section VARIABLES
  //@@Usage
  //Global variables are shared variables that can be
  //seen and modified from any function or script that 
  //declares them.  The syntax for the @|global| statement
  //is
  //@[
  //  global variable_1 variable_2 ...
  //@]
  //The @|global| statement must occur before the variables
  //appear.
  //@@Example
  //Here is an example of two functions that use a global
  //variable to communicate an array between them.  The
  //first function sets the global variable.
  //@{ set_global.m
  //function set_global(x)
  //  global common_array
  //  common_array = x;
  //@}
  //The second function retrieves the value from the global
  //variable
  //@{ get_global.m
  //function x = get_global
  //  global common_array
  //  x = common_array;
  //@}
  //Here we exercise the two functions
  //@<
  //set_global('Hello')
  //get_global
  //@>
  //!
  void WalkTree::globalStatement(ASTPtr t) {
    if (t) {
      pushID(t->context());
      context->addGlobalVariable(t->text);
      t = t->down;
      while (t) {
	context->addGlobalVariable(t->text);
	t = t->right;
      }
      popID();
    }
  }

  //!
  //@Module PERSISTENT Persistent Variables
  //@@Section VARIABLES
  //@@Usage
  //Persistent variables are variables whose value persists between
  //calls to a function or script.  The general syntax for its
  //use is
  //@[
  //   persistent variable1 variable2 ... variableN
  //@]
  //The @|persistent| statement must occur before the variable
  //is the tagged as persistent.
  //@@Example
  //Here is an example of a function that counts how many
  //times it has been called.
  //@{ count_calls.m
  //function count_calls
  //  persistent ccount
  //  if (isempty(ccount)) ccount = 0; end;
  //  ccount = ccount + 1;
  //  printf('Function has been called %d times\n',ccount);
  //@}
  //We now call the function several times:
  //@<
  //for i=1:10; count_calls; end
  //@>
  //!
  void WalkTree::persistentStatement(ASTPtr t) {
    if (t) {
      pushID(t->context());
      context->addPersistentVariable(t->text);
      t = t->down;
      while (t) {
	context->addPersistentVariable(t->text);
	t = t->right;
      }
      popID();
    }
  }

  //!
  //@Module CONTINUE Continue Execution In Loop
  //@@Section FLOW
  //@@Usage
  //The @|continue| statement is used to change the order of
  //execution within a loop.  The @|continue| statement can
  //be used inside a @|for| loop or a @|while| loop.  The
  //syntax for its use is 
  //@[
  //   continue
  //@]
  //inside the body of the loop.  The @|continue| statement
  //forces execution to start at the top of the loop with
  //the next iteration.  The examples section shows how
  //the @|continue| statement works.
  //@@Example
  //Here is a simple example of using a @|continue| statement.
  //We want to sum the integers from @|1| to @|10|, but not
  //the number @|5|.  We will use a @|for| loop and a continue
  //statement.
  //@{ continue_ex.m
  //function accum = continue_ex
  //  accum = 0;
  //  for i=1:10
  //    if (i==5)
  //      continue;
  //    end
  //    accum = accum + 1; %skipped if i == 5!
  //  end
  //@}
  //The function is exercised here:
  //@<
  //continue_ex
  //sum([1:4,6:10])
  //@>
  //!

  //!
  //@Module BREAK Exit Execution In Loop
  //@@Section FLOW
  //@@Usage
  //The @|break| statement is used to exit a loop prematurely.
  //It can be used inside a @|for| loop or a @|while| loop.  The
  //syntax for its use is
  //@[
  //   break
  //@]
  //inside the body of the loop.  The @|break| statement forces
  //execution to exit the loop immediately.
  //@@Example
  //Here is a simple example of how @|break| exits the loop.
  //We have a loop that sums integers from @|1| to @|10|, but
  //that stops prematurely at @|5| using a @|break|.  We will
  //use a @|while| loop.
  //@{ break_ex.m
  //function accum = break_ex
  //  accum = 0;
  //  i = 1;
  //  while (i<=10) 
  //    accum = accum + i;
  //    if (i == 5)
  //      break;
  //    end
  //    i = i + 1;
  //  end
  //@}
  //The function is exercised here:
  //@<
  //break_ex
  //sum(1:5)
  //@>
  //!

  //!
  //@Module RETURN Return From Function
  //@@Section FLOW
  //@@Usage
  //The @|return| statement is used to immediately return from
  //a function, or to return from a @|keyboard| session.  The 
  //syntax for its use is
  //@[
  //  return
  //@]
  //Inside a function, a @|return| statement causes FreeMat
  //to exit the function immediately.  When a @|keyboard| session
  //is active, the @|return| statement causes execution to
  //resume where the @|keyboard| session started.
  //@@Example
  //In the first example, we define a function that uses a
  //@|return| to exit the function if a certain test condition 
  //is satisfied.
  //@{ return_func.m
  //function ret = return_func(a,b)
  //  ret = 'a is greater';
  //  if (a > b)
  //    return;
  //  end
  //  ret = 'b is greater';
  //  printf('finishing up...\n');
  //@}
  //Next we exercise the function with a few simple test
  //cases:
  //@<
  //return_func(1,3)
  //return_func(5,2)
  //@>
  //In the second example, we take the function and rewrite
  //it to use a @|keyboard| statement inside the @|if| statement.
  //@{ return_func2.m
  //function ret = return_func2(a,b)
  //  if (a > b)
  //     ret = 'a is greater';
  //     keyboard;
  //  else
  //     ret = 'b is greater';
  //  end
  //  printf('finishing up...\n');
  //@}
  //Now, we call the function with a larger first argument, which
  //triggers the @|keyboard| session.  After verifying a few
  //values inside the @|keyboard| session, we issue a @|return|
  //statement to resume execution.
  //@<
  //return_func2(2,4)
  //return_func2(5,1)
  //ret
  //a
  //b
  //return
  //@>
  //!

  //!
  //@Module QUIT Quit Program
  //@@Section FREEMAT
  //@@Usage
  //The @|quit| statement is used to immediately exit the FreeMat
  //application.  The syntax for its use is
  //@[
  //   quit
  //@]
  //!

  //!
  //@Module RETALL Return From All Keyboard Sessions
  //@@Section FLOW
  //@@Usage
  //The @|retall| statement is used to return to the base workspace
  //from a nested @|keyboard| session.  It is equivalent to forcing
  //execution to return to the main prompt, regardless of the level
  //of nesting of @|keyboard| sessions, or which functions are 
  //running.  The syntax is simple
  //@[
  //   retall
  //@]
  //The @|retall| is a convenient way to stop debugging.  In the
  //process of debugging a complex program or set of functions,
  //you may find yourself 5 function calls down into the program
  //only to discover the problem.  After fixing it, issueing
  //a @|retall| effectively forces FreeMat to exit your program
  //and return to the interactive prompt.
  //@@Example
  //Here we demonstrate an extreme example of @|retall|.  We
  //are debugging a recursive function @|self| to calculate the sum
  //of the first N integers.  When the function is called,
  //a @|keyboard| session is initiated after the function
  //has called itself N times.  At this @|keyboard| prompt,
  //we issue another call to @|self| and get another @|keyboard|
  //prompt, this time with a depth of 2.  A @|retall| statement
  //returns us to the top level without executing the remainder
  //of either the first or second call to @|self|:
  //@{ self.m
  //function y = self(n)
  //  if (n>1)
  //    y = n + self(n-1);
  //    printf('y is %d\n',y);
  //  else
  //    y = 1;
  //    printf('y is initialized to one\n');
  //    keyboard
  //  end
  //@}
  //@<
  //self(4)
  //self(6)
  //retall
  //@>
  //!

  //!
  //@Module KEYBOARD Initiate Interactive Debug Session
  //@@Section FLOW
  //@@Usage
  //The @|keyboard| statement is used to initiate an
  //interactive session at a specific point in a function.
  //The general syntax for the @|keyboard| statement is
  //@[
  //   keyboard
  //@]
  //A @|keyboard| statement can be issued in a @|script|,
  //in a @|function|, or from within another @|keyboard| session.
  //The result of a @|keyboard| statement is that execution
  //of the program is halted, and you are given a prompt
  //of the form:
  //@[
  // [scope,n] -->
  //@]
  //where @|scope| is the current scope of execution (either
  //the name of the function we are executing, or @|base| otherwise).
  //And @|n| is the depth of the @|keyboard| session. If, for example,
  //we are in a @|keyboard| session, and we call a function that issues
  //another @|keyboard| session, the depth of that second session 
  //will be one higher.  Put another way, @|n| is the number of @|return|
  //statements you have to issue to get back to the base workspace.
  //Incidentally, a @|return| is how you exit the @|keyboard| session
  //and resume execution of the program from where it left off.  A
  //@|retall| can be used to shortcut execution and return to the base
  //workspace.
  //
  //The @|keyboard| statement is an excellent tool for debugging
  //FreeMat code, and along with @|eval| provide a unique set of
  //capabilities not usually found in compiled environments.  Indeed,
  //the @|keyboard| statement is equivalent to a debugger breakpoint in 
  //more traditional environments, but with significantly more inspection
  //power.
  //@@Example
  //Here we demonstrate a two-level @|keyboard| situation.  We have
  //a simple function that calls @|keyboard| internally:
  //@{ key_one.m
  //function c = key_one(a,b)
  //c = a + b;
  //keyboard
  //@}
  //Now, we execute the function from the base workspace, and
  //at the @|keyboard| prompt, we call it again.  This action
  //puts us at depth 2.  We can confirm that we are in the second
  //invocation of the function by examining the arguments.  We
  //then issue two @|return| statements to return to the base
  //workspace.
  //@<
  //key_one(1,2)
  //key_one(5,7)
  //a
  //b
  //c
  //return
  //a
  //b
  //c
  //return
  //@>
  //!

  void WalkTree::debugCLI() {
    depth++;
    bpActive = true;
    evalCLI();
    bpActive = false;
    if (state == FM_STATE_RETURN) state = FM_STATE_OK;
    depth--;
  }

  void WalkTree::handleDebug(int fullcontext) {
    int linenumber;
    linenumber = fullcontext & 0xffff;
    if (debugActive) {
      if (inStepMode) {
	if ((stepTrap.cname == cstack.back().cname) &&
	    (stepTrap.tokid == linenumber)) {
	  // Finished stepping...
	  inStepMode = false;
	  char buffer[2048];
	  sprintf(buffer,"Finished stepping to %s, line %d\n",
		  stepTrap.cname.c_str(),
		  linenumber);
	  io->outputMessage(buffer);
	  debugCLI();
	}
      } else {
	// check the breakpoint list
	bool found = false;
	int j=0;
	while ((j<bpStack.size()) && !found) {
	  // Is this a resolved breakpoint?
	  if ((bpStack[j].tokid >> 16) != 0) {
	    if ((bpStack[j].cname == cstack.back().cname) && 
		(bpStack[j].tokid == fullcontext)) {
	      found = true;
	    } else {
	      found = false;
	      j++;
	    }
	  } else {
	    if ((bpStack[j].cname == cstack.back().cname) && 
		(bpStack[j].tokid == linenumber)) {
	      found = true;
	      bpStack[j].tokid = fullcontext;
	    } else {
	      found = false;
	      j++;
	    }
	  }
	}
	if (found) {
	  stepTrap = bpStack[j];
	  char buffer[2048];
	  sprintf(buffer,"Encountered breakpoint at %s, line %d\n",
		  bpStack[j].cname.c_str(),
		  linenumber);
	  io->outputMessage(buffer);
	  debugCLI();
	}
      }
    }
  }

  void WalkTree::statementType(ASTPtr t, bool printIt) {
    ArrayVector m;
    FunctionDef *fdef;
    Fl::wait(0);
    pushID(t->context());

    // check the debug flag
    int fullcontext = t->context();
    handleDebug(fullcontext);
    if (t->isEmpty()) {
      /* Empty statement */
    } else if (t->opNum ==(OP_ASSIGN)) {
      if (t->down->down == NULL) {
	Array b(expression(t->down->right));
	context->insertVariable(t->down->text,b);
	if (printIt) {
	  io->outputMessage(t->down->text);
	  io->outputMessage(" = \n");
	  b.printMe(printLimit,io->getTerminalWidth());
	}	  
      } else {
	Array expr(expression(t->down->right));
	Array c(assignExpression(t->down,expr));
	context->insertVariable(t->down->text,c);
	if (printIt) {
	  io->outputMessage(t->down->text);
	  io->outputMessage(" = \n");
	  c.printMe(printLimit,io->getTerminalWidth());
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
	throw Exception("Unrecognized statement type");
      }
    } else {
      // There is a special case to consider here - when a 
      // function call is made as a statement, we do not require
      // that the function have an output.
      Array b;
      if (t->opNum ==(OP_RHS) && 
	  !context->lookupVariable(t->down->text,b)) {
	m = functionExpression(t->down,0,true);
	if (m.size() == 0) 
	  b = Array::emptyConstructor();
	else 
	  b = m[0];
	if (printIt && !b.isEmpty()
	    && (state != FM_STATE_QUIT) && (state != FM_STATE_RETALL)) {
	  io->outputMessage("ans = \n");
	  b.printMe(printLimit,io->getTerminalWidth());
	}
      }
      else if (t->opNum == OP_RHS) {
	m = rhsExpression(t->down);
	if (m.size() == 0)
	  b = Array::emptyConstructor();
	else {
	  b = m[0];
	  if (printIt && (state != FM_STATE_QUIT) && (state != FM_STATE_RETALL)) {
	    io->outputMessage("ans = \n");
	    for (int j=0;j<m.size();j++) {
	      char buffer[1000];
	      if (m.size() > 1) {
		sprintf(buffer,"\n%d of %d:\n",j+1,m.size());
		io->outputMessage(buffer);
	      }
	      m[j].printMe(printLimit,io->getTerminalWidth());
	    }
	  }
	}
      } else {
	b = expression(t);
	if (printIt && (state != FM_STATE_QUIT) && (state != FM_STATE_RETALL)) {
	  io->outputMessage("ans = \n");
	  b.printMe(printLimit,io->getTerminalWidth());
	} 
      }
      if (state == FM_STATE_QUIT || 
	  state == FM_STATE_RETALL) {
	popID();
	return;
      }
      context->insertVariable("ans",b);
    }
    popID();
  }


  //Trapping at the statement level is much better! - two
  //problems... try/catch and multiline statements (i.e.,atell.m)
  //The try-catch one is easy, I think...  When a try occurs,
  //we capture the stack depth... if an exception occurs, we 
  //unwind the stack to this depth..
  //The second one is trickier - suppose we have a conditional
  //statement
  //if (a == 3)
  //    bfunc
  //else
  //    cfunc
  //end
  //this is represented in the parse tree as a single construct...
  //

  // 
  void WalkTree::statement(ASTPtr t) {
    try {
      pushID(t->context());
      if (t->opNum ==(OP_QSTATEMENT)) 
	statementType(t->down,false);
      else if (t->opNum ==(OP_RSTATEMENT))
	statementType(t->down,true);
      else
	statementType(t->down,true);
      popID();
    } catch (Exception& e) {
      if (autostop && !InCLI) {
	char buffer[4096];
	e.printMe(io);
	stackTrace(true);
	debugCLI();
	if (state != FM_STATE_QUIT &&
	    state != FM_STATE_RETALL)
	  state = FM_STATE_OK;
	popID();
      } else  {
	popID();
	throw;
      }
    }
  }

  void WalkTree::block(ASTPtr t) {
    try {
      if (!t) return;
      ASTPtr s;
      s = t->down;
      if ((state != FM_STATE_QUIT) && (state != FM_STATE_RETALL))
	state = FM_STATE_OK;
      while (state != FM_STATE_RETALL &&
	     state != FM_STATE_QUIT && 
	     s != NULL) {
	if (InterruptPending) {
	  io->outputMessage("Interrupt (ctrl-c) encountered\n");
	  stackTrace(true);
	  InterruptPending = false;
	  debugCLI();
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
    
    if (t->opNum ==(OP_PARENS)) {
      m = variableSubIndexExpressions(t->down,r);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!");
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
      m = variableSubIndexExpressions(t->down,r);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!");
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
    if (t->opNum ==(OP_DOTDYN)) {
      char *field;
      try {
	Array fname(expression(t->down));
	field = fname.getContentsAsCString();
      } catch (Exception &e) {
	throw Exception("dynamic field reference to structure requires a string argument");
      }
      try {
	return(r.getField(field));
      } catch (Exception &e) {
	return(Array::emptyConstructor());
      }
    }
    return(Array::emptyConstructor());
  }

  void WalkTree::simpleAssign(Array& r, ASTPtr t, Array& value) {
    ArrayVector vec;
    
    vec.push_back(value);
    simpleAssign(r,t,vec);
  }

  void WalkTree::simpleAssign(Array& r, ASTPtr t, ArrayVector& value) {
    Dimensions rhsDimensions;
    ArrayVector m;
    Array assignVal;
    pushID(t->context());
    if (!r.isEmpty()) {
      assignVal = r;
    } else if (t->opNum != OP_BRACES) {
      assignVal = value[0];
    } 
    if (t->opNum ==(OP_PARENS)) {
      m = variableSubIndexExpressions(t->down,assignVal);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!");
      else if (m.size() == 1) {
	r.setVectorSubset(m[0],value[0]);
	popID();
	return;
      } else {
	r.setNDimSubset(m,value[0]);
	popID();
	return;
      }
    }
    if (t->opNum ==(OP_BRACES)) {
      m = variableSubIndexExpressions(t->down,assignVal);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!");
      else if (m.size() == 1) {
	r.setVectorContentsAsList(m[0],value);
	popID();
	return;
      } else {
	r.setNDimContentsAsList(m,value);
	popID();
	return;
      }
    }
    if (t->opNum ==(OP_DOT)) {
      r.setFieldAsList(t->down->text,value);
	popID();
      return;
    }    
    if (t->opNum == (OP_DOTDYN)) {
      char *field;
      try {
	Array fname(expression(t->down));
	field = fname.getContentsAsCString();
      } catch (Exception &e) {
	throw Exception("dynamic field reference to structure requires a string argument");
      }
      r.setFieldAsList(field,value);
      popID();
      return;
    }
    popID();
  }

  int WalkTree::countLeftHandSides(ASTPtr t) {
    Array lhs;
    if (!context->lookupVariable(t->text,lhs))
      lhs = Array::emptyConstructor();
    ASTPtr s = t->down;
    if (s == NULL) {
      return 1;
    }
    pushID(s->context());
    while (s->right != NULL) {
      if (!lhs.isEmpty())
	lhs = simpleSubindexExpression(lhs,s);
      s = s->right;
    }
    // We are down to the last subindexing expression...
    // We have to special case this one
    ArrayVector m;
    if (s->opNum == (OP_PARENS)) {
	m = variableSubIndexExpressions(s->down,lhs);
	if (m.size() == 0)
	  throw Exception("Expected indexing expression!");
	if (m.size() == 1) {
	  // m[0] should have only one element...
	  m[0].toOrdinalType();
	  if (m[0].getLength() > 1)
	    throw Exception("Parenthetical expression in the left hand side of a function call must resolve to a single element.");
	  popID();
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
	  popID();
	  return (outputCount);
	}
    }
    if (s->opNum ==(OP_BRACES)) {
      m = variableSubIndexExpressions(s->down,lhs);
      if (m.size() == 0)
	throw Exception("Expected indexing expression!");
      if (m.size() == 1) {
	// m[0] should have only one element...
	m[0].toOrdinalType();
	popID();
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
	popID();
	return (outputCount);
      }
    }
    if (s->opNum ==(OP_DOT)) {
      popID();
      return std::max(1,lhs.getLength());
    }
    popID();
    return 1;
  }

  Array WalkTree::assignExpression(ASTPtr t, Array& val) {
    ArrayVector vec;

    vec.push_back(val);
    return assignExpression(t,vec);
  }

  Array WalkTree::EndReference(Array v, int index, int count) {
    FuncPtr val;
    if (v.isUserClass() && ClassResolveFunction(this,v,"end",val)) {
      // User has overloaded "end" operator
      val->updateCode();
      ArrayVector argvec;
      argvec.push_back(Array::int32Constructor(index+1));
      argvec.push_back(Array::int32Constructor(count));
      ArrayVector retvec(val->evaluateFunction(this,argvec,1));
      return retvec[0];
    }
    Dimensions dim(v.getDimensions());
    if (count == 1)
      return Array::int32Constructor(dim.getElementCount());
    else
      return Array::int32Constructor(dim.getDimensionLength(index));
  }

  Array WalkTree::AllColonReference(Array v, int index, int count) {
    if (v.isUserClass()) return Array::emptyConstructor();
    Dimensions dim(v.getDimensions());
    if (count == 1)
      return Array::int32RangeConstructor(1,1,dim.getElementCount(),true);
    else
      return Array::int32RangeConstructor(1,1,dim.getDimensionLength(index),true);
  }
  
  // If we got this far, we must have at least one subindex
  Array WalkTree::assignExpression(ASTPtr t, ArrayVector& value) {
    pushID(t->context());
    if (t->down == NULL) {
      Array retval(value[0]);
      value.erase(value.begin());
      popID();
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
    popID();
    return lhs;
  }

  void WalkTree::specialFunctionCall(ASTPtr t, bool printIt) {
    ASTPtr fAST;
    ArrayVector m;
    stringVector args;
    args.push_back(t->text);
    ASTPtr s = t->right;
    while (s) {
      args.push_back(s->text);
      s = s->right;
    }
    if (args.empty()) return;
    pushID(t->context());
    ArrayVector n;
    for (int i=1;i<args.size();i++)
      n.push_back(Array::stringConstructor(args[i].c_str()));
    FuncPtr val;
    if (!lookupFunction(args[0],val,n))
      throw Exception("unable to resolve " + args[0] + " to a function call");
    val->updateCode();
    bool CLIFlagsave = InCLI;
    InCLI = false;
    try {
      m = val->evaluateFunction(this,n,0);
    } catch(Exception& e) {
      InCLI = CLIFlagsave;
      throw;
    }
    InCLI = CLIFlagsave;
    popID();

  }

  void WalkTree::addBreakpoint(stackentry bp) {
    bpStack.push_back(bp);
    adjustBreakpoints();
    debugActive = true;
  }



  //Some notes on the multifunction call...  This one is pretty complicated, and the current logic is hardly transparent.  Assume we have an expression of the form:
  //
  //[expr1 expr2 ... exprn] = fcall
  //
  //where fcall is a function call (obviously).  Now, we want to determine how many output arguments fcall should have.  There are several interesting cases to consider:
  //
  //expr_i is an existing numeric variable -- lhscount += 1
  //
  //expr_i is an existing cell array -- lhscount += size(expr_i)
  //
  //expr_i is an existing struct array -- lhscount += size(expr_i)
  //
  //expr_i does not exist -- lhscount += 1
  //
  //Where this will fail is in one case.  If expr_i is a cell reference for a variable that does not exist, and has a sized argument, something like
  //[eg{1:3}]
  //in which case the lhscount += 3, even though eg does not exist. 
  void WalkTree::multiFunctionCall(ASTPtr t, bool printIt) {
    ArrayVector m;
    ASTPtr s, fAST, saveLHS;
    Array c;
    int lhsSize;
    int lhsCounter;

    fAST = t->right;
    pushID(fAST->context());

    if (t->opNum != OP_BRACKETS)
      throw Exception("Illegal left hand side in multifunction expression");
    s = t->down;
    if (s->opNum != OP_SEMICOLON)
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
    // Trap the special case where function pointers are used
    Array r;
    bool isVar = context->lookupVariable(fAST->text,r);
    if (isVar && (r.getDataClass() == FM_FUNCPTR_ARRAY) &&
	r.isScalar()) 
      m = FunctionPointerDispatch(r,fAST->down,lhsCount);
    else
      m = functionExpression(fAST,lhsCount,false);
    s = saveLHS;
    while ((s != NULL) && (m.size() > 0)) {
      Array c(assignExpression(s->down,m));
      context->insertVariable(s->down->text,c);
      if (printIt) {
	io->outputMessage(s->down->text);
	io->outputMessage(" = \n");
	c.printMe(printLimit,io->getTerminalWidth());
      }
      s = s->right;
    }
    if (s != NULL)
      io->warningMessage("one or more outputs not assigned in call.");
    popID();
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

  //!
  //@Module FUNCTION Function Declarations
  //@@Section FUNCTIONS
  //@@Usage
  //There are several forms for function declarations in FreeMat.
  //The most general syntax for a function declaration is the 
  //following:
  //@[
  //  function [out_1,...,out_M,varargout] = fname(in_1,...,in_N,varargin)
  //@]
  //where @|out_i| are the output parameters, @|in_i| are the input
  //parameters, and @|varargout| and @|varargin| are special keywords
  //used for functions that have variable inputs or outputs.  For 
  //functions with a fixed number of input or output parameters, the 
  //syntax is somewhat simpler:
  //@[
  //  function [out_1,...,out_M] = fname(in_1,...,in_N)
  //@]
  //Note that functions that have no return arguments can omit
  //the return argument list (of @|out_i|) and the equals sign:
  //@[
  //  function fname(in_1,...,in_N)
  //@]
  //Likewise, a function with no arguments can eliminate the list
  //of parameters in the declaration:
  //@[
  //  function [out_1,...,out_M] = fname
  //@]
  //Functions that return only a single value can omit the brackets
  //@[
  //  function out_1 = fname(in_1,...,in_N)
  //@]
  //
  //In the body of the function @|in_i| are initialized with the
  //values passed when the function is called.  Also, the function
  //must assign values for @|out_i| to pass values to the caller.
  //Note that by default, FreeMat passes arguments by value, meaning
  //that if we modify the contents of @|in_i| inside the function,
  //it has no effect on any variables used by the caller.  Arguments
  //can be passed by reference by prepending an ampersand @|&|
  //before the name of the input, e.g.
  //@[
  //  function [out1,...,out_M] = fname(in_1,&in_2,in_3,...,in_N)
  //@]
  //in which case @|in_2| is passed by reference and not by value.
  //Also, FreeMat works like @|C| in that the caller does not have
  //to supply the full list of arguments.  Also, when @|keywords|
  //(see help @|keywords|) are used, an arbitrary subset of the 
  //parameters may be unspecified. To assist in deciphering 
  //the exact parameters that were passed,
  //FreeMat also defines two variables inside the function context:
  //@|nargin| and @|nargout|, which provide the number of input
  //and output parameters of the caller, respectively. See help for 
  //@|nargin| and @|nargout| for more details.  In some 
  //circumstances, it is necessary to have functions that
  //take a variable number of arguments, or that return a variable
  //number of results.  In these cases, the last argument to the 
  //parameter list is the special argument @|varargin|.  Inside
  //the function, @|varargin| is a cell-array that contains
  //all arguments passed to the function that have not already
  //been accounted for.  Similarly, the function can create a
  //cell array named @|varargout| for variable length output lists.
  //See help @|varargin| and @|varargout| for more details.
  //
  //The function name @|fname| can be any legal FreeMat identifier.
  //Functions are stored in files with the @|.m| extension.  Note
  //that the name of the file (and not the function name @|fname| 
  //used in the declaration) is how the function appears in FreeMat.
  //So, for example, if the file is named @|foo.m|, but the declaration
  //uses @|bar| for the name of the function, in FreeMat, it will 
  //still appear as function @|foo|.  Note that this is only true
  //for the first function that appears in a @|.m| file.  Additional
  //functions that appear after the first function are known as
  //@|helper functions| or @|local| functions.  These are functions that
  //can only be called by other functions in the same @|.m| file.  Furthermore
  //the names of these helper functions are determined by their declaration
  //and not by the name of the @|.m| file.  An example of using
  //helper functions is included in the examples.
  //
  //Another important feature of functions, as opposed to, say @|scripts|,
  //is that they have their own @|scope|.  That means that variables
  //defined or modified inside a function do not affect the scope of the
  //caller.  That means that a function can freely define and use variables
  //without unintentionally using a variable name reserved elsewhere.  The
  //flip side of this fact is that functions are harder to debug than
  //scripts without using the @|keyboard| function, because the intermediate
  //calculations used in the function are not available once the function
  //exits.
  //@@Examples
  //Here is an example of a trivial function that adds its
  //first argument to twice its second argument:
  //@{ addtest.m
  //function c = addtest(a,b)
  //  c = a + 2*b;
  //@}
  //@<
  //addtest(1,3)
  //addtest(3,0)
  //@>
  //Suppose, however, we want to replace the value of the first 
  //argument by the computed sum.  A first attempt at doing so
  //has no effect:
  //@{ addtest2.m
  //function addtest2(a,b)
  //  a = a + 2*b;
  //@}
  //@<
  //arg1 = 1
  //arg2 = 3
  //addtest2(arg1,arg2)
  //arg1
  //arg2
  //@>
  //The values of @|arg1| and @|arg2| are unchanged, because they are
  //passed by value, so that any changes to @|a| and @|b| inside 
  //the function do not affect @|arg1| and @|arg2|.  We can change
  //that by passing the first argument by reference:
  //@{ addtest3.m
  //function addtest3(&a,b)
  //  a = a + 2*b
  //@}
  //Note that it is now illegal to pass a literal value for @|a| when
  //calling @|addtest3|:
  //@<
  //addtest(3,4)
  //addtest(arg1,arg2)
  //arg1
  //arg2
  //@>
  //The first example fails because we cannot pass a literal like the
  //number @|3| by reference.  However, the second call succeeds, and
  //note that @|arg1| has now changed.  Note: please be careful when
  //passing by reference - this feature is not available in MATLAB
  //and you must be clear that you are using it.
  //
  //As variable argument and return functions are covered elsewhere,
  //as are keywords, we include one final example that demonstrates
  //the use of helper functions, or local functions, where
  //multiple function declarations occur in the same file.
  //@{ euclidlength.m
  //function y = foo(x,y)
  //  square_me(x);
  //  square_me(y);
  //  y = sqrt(x+y);
  //
  //function square_me(&t)
  //  t = t^2;
  //@}
  //@<
  //euclidlength(3,4)
  //euclidlength(2,0)
  //@>
  //!

  //!
  //@Module KEYWORDS Function Keywords
  //@@Section FUNCTIONS
  //@@Usage
  //A feature of IDL that FreeMat has adopted is a modified
  //form of @|keywords|.  The purpose of @|keywords| is to 
  //allow you to call a function with the arguments to the
  //function specified in an arbitrary order.  To specify
  //the syntax of @|keywords|, suppose there is a function 
  //with prototype
  //@[
  //  function [out_1,...,out_M] = foo(in_1,...,in_N)
  //@]
  //Then the general syntax for calling function @|foo| using keywords
  //is
  //@[
  //  foo(val_1, val_2, /in_k=3)
  //@]
  //which is exactly equivalent to
  //@[
  //  foo(val_1, val_2, [], [], ..., [], 3),
  //@]
  //where the 3 is passed as the k-th argument, or alternately,
  //@[
  //  foo(val_1, val_2, /in_k)
  //@]
  //which is exactly equivalent to
  //@[
  //  foo(val_1, val_2, [], [], ..., [], logical(1)),
  //@]
  //Note that you can even pass reference arguments using keywords.
  //@@Example
  //The most common use of keywords is in controlling options for
  //functions.  For example, the following function takes a number
  //of binary options that control its behavior.  For example,
  //consider the following function with two arguments and two
  //options.  The function has been written to properly use and
  //handle keywords.  The result is much cleaner than the MATLAB
  //approach involving testing all possible values of @|nargin|,
  //and forcing explicit empty brackets for don't care parameters.
  //@{ keyfunc.m
  //function c = keyfunc(a,b,operation,printit)
  //  if (~isset('a') | ~isset('b')) 
  //    error('keyfunc requires at least the first two 2 arguments'); 
  //  end;
  //  if (~isset('operation'))
  //    % user did not define the operation, default to '+'
  //    operation = '+';
  //  end
  //  if (~isset('printit'))
  //    % user did not specify the printit flag, default is false
  //    printit = 0;
  //  end
  //  % simple operation...
  //  eval(['c = a ' operation ' b;']);
  //  if (printit) 
  //    printf('%f %s %f = %f\n',a,operation,b,c);
  //  end
  //@}
  //Now some examples of how this function can be called using
  //@|keywords|.
  //@<
  //keyfunc(1,3)                % specify a and b, defaults for the others
  //keyfunc(1,3,/printit)       % specify printit is true
  //keyfunc(/operation='-',2,3) % assigns a=2, b=3
  //keyfunc(4,/operation='*',/printit) % error as b is unspecified
  //@>
  //!

  //!
  //@Module VARARGIN Variable Input Arguments
  //@@Section FUNCTIONS
  //@@Usage
  //FreeMat functions can take a variable number of input arguments
  //by setting the last argument in the argument list to @|varargin|.
  //This special keyword indicates that all arguments to the
  //function (beyond the last non-@|varargin| keyword) are assigned
  //to a cell array named @|varargin| available to the function.
  //Variable argument functions are usually used when writing 
  //driver functions, i.e., functions that need to pass arguments
  //to another function.  The general syntax for a function that
  //takes a variable number of arguments is
  //@[
  //  function [out_1,...,out_M] = fname(in_1,..,in_M,varargin)
  //@]
  //Inside the function body, @|varargin| collects the arguments 
  //to @|fname| that are not assigned to the @|in_k|.
  //@@Example
  //Here is a simple wrapper to @|feval| that demonstrates the
  //use of variable arguments functions.
  //@{ wrapcall.m
  //function wrapcall(fname,varargin)
  //  feval(fname,varargin{:});
  //@}
  //Now we show a call of the @|wrapcall| function with a number
  //of arguments
  //@<
  //wrapcall('printf','%f...%f\n',pi,e)
  //@>
  //A more serious driver routine could, for example, optimize
  //a one dimensional function that takes a number of auxilliary
  //parameters that are passed through @|varargin|.
  //!

  //!
  //@Module VARARGOUT Variable Output Arguments
  //@@Section FUNCTIONS
  //@@Usage
  //FreeMat functions can return a variable number of output arguments
  //by setting the last argument in the argument list to @|varargout|.
  //This special keyword indicates that the number of return values
  //is variable.  The general syntax for a function that returns
  //a variable number of outputs is
  //@[
  //  function [out_1,...,out_M,varargout] = fname(in_1,...,in_M)
  //@]
  //The function is responsible for ensuring that @|varargout| is
  //a cell array that contains the values to assign to the outputs
  //beyond @|out_M|.  Generally, variable output functions use
  //@|nargout| to figure out how many outputs have been requested.
  //@@Example
  //This is a function that returns a varying number of values
  //depending on the value of the argument.
  //@{ varoutfunc.m
  //function [varargout] = varoutfunc
  //  switch(nargout)
  //    case 1
  //      varargout = {'one of one'};
  //    case 2
  //      varargout = {'one of two','two of two'};
  //    case 3
  //      varargout = {'one of three','two of three','three of three'};
  //  end
  //@}
  //Here are some examples of exercising @|varoutfunc|:
  //@<
  //[c1] = varoutfunc
  //[c1,c2] = varoutfunc
  //[c1,c2,c3] = varoutfunc
  //@>
  //!

  //!
  //@Module SCRIPT Script Files
  //@@Section FUNCTIONS
  //@@Usage
  //A script is a sequence of FreeMat commands contained in a
  //@|.m| file.  When the script is called (via the name of the
  //file), the effect is the same as if the commands inside the
  //script file were issued one at a time from the keyboard.
  //Unlike @|function| files (which have the same extension,
  //but have a @|function| declaration), script files share
  //the same environment as their callers.  Hence, assignments,
  //etc, made inside a script are visible to the caller (which
  //is not the case for functions.
  //@@Example
  //Here is an example of a script that makes some simple 
  //assignments and @|printf| statements.
  //@{ tscript.m
  //a = 13;
  //printf('a is %d\n',a);
  //b = a + 32
  //@}
  //If we execute the script and then look at the defined variables
  //@<
  //tscript
  //who
  //@>
  //we see that @|a| and @|b| are defined appropriately.
  //!

  //!
  //@Module NARGIN Number of Input Arguments
  //@@Section FUNCTIONS
  //@@Usage
  //The special variable @|nargin| is defined inside of all
  //functions.  It indicates how many arguments were passed
  //to the function when it was called.  FreeMat allows for
  //fewer arguments to be passed to a function than were declared,
  //and @|nargin|, along with @|isset| can be used to determine
  //exactly what subset of the arguments were defined.
  //There is no syntax for the use of @|nargin| - it is 
  //automatically defined inside the function body.
  //@@Example
  //Here is a function that is declared to take five 
  //arguments, and that simply prints the value of @|nargin|
  //each time it is called.
  //@{ nargintest.m
  //function nargintest(a1,a2,a3,a4,a5)
  //  printf('nargin = %d\n',nargin);
  //@}
  //@<
  //nargintest(3);
  //nargintest(3,'h');
  //nargintest(3,'h',1.34);
  //nargintest(3,'h',1.34,pi,e);
  //@>
  //!

  //!
  //@Module NARGOUT Number of Output Arguments
  //@@Section FUNCTIONS
  //@@Usage
  //The special variable @|nargout| is defined inside of all
  //functions.  It indicates how many return values were requested from
  //the function when it was called.  FreeMat allows for
  //fewer return values to be requested from a function than were declared,
  //and @|nargout| can be used to determine exactly what subset of 
  //the functions outputs are required.  There is no syntax for 
  //the use of @|nargout| - it is automatically defined inside 
  //the function body.
  //@@Example
  //Here is a function that is declared to return five 
  //values, and that simply prints the value of @|nargout|
  //each time it is called.
  //@{ nargouttest.m
  //function [a1,a2,a3,a4,a5] = nargouttest
  //  printf('nargout = %d\n',nargout);
  //  a1 = 1; a2 = 2; a3 = 3; a4 = 4; a5 = 5;
  //@}
  //@<
  //a1 = nargouttest
  //[a1,a2] = nargouttest
  //[a1,a2,a3] = nargouttest
  //[a1,a2,a3,a4,a5] = nargouttest
  //@>
  //!
  
  //!
  //@Module SPECIAL Special Calling Syntax
  //@@Section FUNCTIONS
  //@@Usage
  //To reduce the effort to call certain functions, FreeMat supports
  //a special calling syntax for functions that take string arguments.
  //In particular, the three following syntaxes are equivalent, with
  //one caveat:
  //@[
  //   functionname('arg1','arg2',...,'argn')
  //@]
  //or the parenthesis and commas can be removed
  //@[
  //   functionname 'arg1' 'arg2' ... 'argn'
  //@]
  //The quotes are also optional (providing, of course, that the
  //argument strings have no spaces in them)
  //@[
  //   functionname arg1 arg2 ... argn
  //@]
  //This special syntax enables you to type @|hold on| instead of
  //the more cumbersome @|hold('on')|.  The caveat is that FreeMat
  //currently only recognizes the special calling syntax as the
  //first statement on a line of input.  Thus, the following construction
  //@[
  //  for i=1:10; plot(vec(i)); hold on; end
  //@]
  //would not work.  This limitation may be removed in a future
  //version.
  //@@Example
  //Here is a function that takes two string arguments and
  //returns the concatenation of them.
  //@{ strcattest.m
  //function strcattest(str1,str2)
  //  str3 = [str1,str2];
  //  printf('str1 = %s, str2 = %s, str3 = %s\n',str1,str2,str3);
  //@}
  //We call @|strcattest| using all three syntaxes.
  //@<
  //strcattest('hi','ho')
  //strcattest 'hi' 'ho'
  //strcattest hi ho
  //@>
  //!

  void WalkTree::collectKeywords(ASTPtr q, ArrayVector &keyvals,
				 ASTPtrVector &keyexpr, stringVector &keywords) {
    // Search for the keyword uses - 
    // To handle keywords, we make one pass through the arguments,
    // recording a list of keywords used and using ::expression to
    // evaluate their values. 
    while (q != NULL) {
      if (q->opNum == OP_KEYWORD) {
	keywords.push_back(q->down->text);
	if (q->down->right != NULL)
	  keyvals.push_back(expression(q->down->right));
	else
	  keyvals.push_back(Array::logicalConstructor(true));
	keyexpr.push_back(q->down->right);
      }
      q = q->right;
    }
  }
					

  int* WalkTree::sortKeywords(ArrayVector &m, stringVector &keywords,
			      stringVector arguments, ArrayVector keyvals) {
    // If keywords were used, we have to permute the
    // entries of the arrayvector to the correct order.
    int *keywordNdx = new int[keywords.size()];
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
    bool *filled = new bool[totalCount];
    int *argTypeMap = new int[totalCount];
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
    delete[] keywordNdx;
    // Reassign
    m = toFill;
    return argTypeMap;
  }

  void WalkTree::handlePassByReference(ASTPtr q, stringVector arguments,
				       ArrayVector m,stringVector keywords, 
				       ASTPtrVector keyexpr, int* argTypeMap) {
    ASTPtr p;
    // M functions can modify their arguments
    int maxsearch = m.size(); 
    if (maxsearch > arguments.size()) maxsearch = arguments.size();
    for (int i=0;i<maxsearch;i++) {
      // Was this argument passed out of order?
      if ((keywords.size() > 0) && (argTypeMap[i] == -1)) continue;
      if ((keywords.size() > 0) && (argTypeMap[i] >=0)) {
	p = keyexpr[argTypeMap[i]];
      } else {
	p = q;
	if (q != NULL)
	  q = q->right;
      }
      std::string args(arguments[i]);
      if (args[0] == '&') {
	args.erase(0,1);
	// This argument was passed by reference
	if (p == NULL || !(p->type == non_terminal && p->opNum == OP_RHS))
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

  static ArrayVector mergeVecs(ArrayVector a, ArrayVector b) {
    for (int i=0;i<b.size();i++)
      a.push_back(b[i]);
    return a;
  }

  ArrayVector WalkTree::functionExpression(ASTPtr t, 
					   int narg_out, 
					   bool outputOptional) {
    ArrayVector m, n;
    ASTPtr s;
    stringVector keywords;
    ArrayVector keyvals;
    ASTPtrVector keyexpr;
    int i;
    char buffer[2048];
    FuncPtr funcDef;
    int* argTypeMap;
    bool CLIFlagsave;
    CLIFlagsave = InCLI;
    
    pushID(t->context());
    try {
      // Because of the introduction of user-defined classes, we have to 
      // first evaluate the keywords and the arguments, before we know
      // which function to call.
      // First, check for arguments
      if (t->down != NULL) {
	// Collect all the arguments
	s = t->down;
	if (s->opNum ==(OP_PARENS)) {
	  s = s->down;
	  // Collect keywords
	  collectKeywords(s,keyvals,keyexpr,keywords);
	  // Evaluate function arguments
	  m = expressionList(s);
	} else
	  throw Exception(std::string("Unknown function or variable ") + t->text);
      }
      // Now that the arguments have been evaluated, we have to 
      // find the dominant class
      if (!lookupFunction(t->text,funcDef,m))
	throw Exception(std::string("Undefined function or variable ") + 
			t->text);
      funcDef->updateCode();
      if (funcDef->scriptFlag) {
	if (t->down != NULL)
	  throw Exception(std::string("Cannot use arguments in a call to a script."));
	if ((narg_out > 0) && !outputOptional)
	  throw Exception(std::string("Cannot assign outputs in a call to a script."));
	CLIFlagsave = InCLI;
	InCLI = false;
	pushDebug(((MFunctionDef*)funcDef)->fileName,std::string("script"));
	block(((MFunctionDef*)funcDef)->code);
	popDebug();
	InCLI = CLIFlagsave;
      } else {
	// We can now adjust the keywords (because we know the argument list)
	// Apply keyword mapping
	if (!keywords.empty()) 
	  argTypeMap = sortKeywords(m,keywords,funcDef->arguments,keyvals);
	else
		argTypeMap = NULL;
	if ((funcDef->inputArgCount() >= 0) && 
	    (m.size() > funcDef->inputArgCount()))
	  throw Exception(std::string("Too many inputs to function ")+t->text);
	if ((funcDef->outputArgCount() >= 0) && 
	    (narg_out > funcDef->outputArgCount() && !outputOptional))
	  throw Exception(std::string("Too many outputs to function ")+t->text);
	CLIFlagsave = InCLI;
	InCLI = false;
	n = funcDef->evaluateFunction(this,m,narg_out);
	InCLI = CLIFlagsave;
	if (state == FM_STATE_RETALL)
	  return n;
	// Check for any pass by reference
	if ((t->down != NULL) && (funcDef->arguments.size() > 0)) 
	  handlePassByReference(s,funcDef->arguments,m,keywords,keyexpr,argTypeMap);
      }
      // Some routines (e.g., min and max) will return more outputs
      // than were actually requested... so here we have to trim 
      // any elements received that we didn't ask for.
      // preserve one output if we were called as an expression (for ans)
      if (outputOptional) narg_out = (narg_out == 0) ? 1 : narg_out;
      while (n.size() > narg_out)
	n.pop_back();
      popID();
      return n;
    } catch (Exception& e) {
      InCLI = CLIFlagsave;
      throw;
    }
    popID();
  }

  int COST(int a, int b) {
	return (((a) >= (b)) ? ((a)-(b)) : 10000);
  }

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

  int GetClosestLineNumber(ASTPtr t, int lineno) {
    if (t == NULL) return 10000;
    int linedwn = GetClosestLineNumber(t->down,lineno);
    int linerght = GetClosestLineNumber(t->right,lineno);
    int retval = (t->context() & 0xffff);;
    int costthis = COST(retval,lineno);
    return(MIN(linedwn,MIN(linerght,costthis)));
  }

  void WalkTree::listBreakpoints() {
    for (int i=0;i<bpStack.size();i++) {
      char buffer[2048];
      sprintf(buffer,"%d   %s line %d\n",i+1,bpStack[i].cname.c_str(),bpStack[i].tokid & 0xffff);
      io->outputMessage(buffer);
    }
  }

  void WalkTree::deleteBreakpoint(int number) {
    char buffer[2048];
    if ((number >= 1) && (number <= bpStack.size())) {
      bpStack.erase(bpStack.begin() + number - 1);
    } else {
      sprintf(buffer,"Unable to delete breakpoint %d\n",number);
      throw Exception(buffer);
    }
  }

  bool WalkTree::adjustBreakpoint(stackentry& bp, bool dbstep) {
    char *cname = strdup(bp.detail.c_str());
    bool isFun;
    FuncPtr val;
    ArrayVector dummy;
    isFun = lookupFunction(cname,val,dummy);
    char buffer[1000];
    if (!isFun) return false;
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      mptr->updateCode();
      int clinenum = 10000;
      int nxt;
      while (mptr) {
	ASTPtr code = mptr->code;
	nxt = GetClosestLineNumber(code,bp.tokid & 0xffff);
	clinenum = MIN(clinenum,nxt);
	mptr = mptr->nextFunction;
      }
      if (clinenum == 10000) {
	char buffer[2048];
	if (dbstep) {
	  sprintf(buffer,"Unable to step the specified number of lines, execution will continue\n");
	  inStepMode = false;
	} else 
	  sprintf(buffer,"Failed to set breakpoint in %s at line %d - breakpoint is disabled\n",
		  cname, bp.tokid & 0xffff);
	io->warningMessage(buffer);
	return false;
      } else 
	if (clinenum != 0)
	  bp.tokid = (bp.tokid & 0xffff) + clinenum;
    } else {
      return false;
    }
    return true;
  }
  
  void WalkTree::adjustBreakpoints() {
    std::vector<stackentry>::iterator i=bpStack.begin();
    while (i!=bpStack.end()) {
      if (!adjustBreakpoint(*i,false))
	bpStack.erase(i);
      else
	i++;
    }
    if (inStepMode)
      adjustBreakpoint(stepTrap,true);
  }

  void WalkTree::stackTrace(bool includeCurrent) {
    stringVector outstack;
    char buffer[4096];
    int i=0;
    while (i<cstack.size()) {
      if (cstack[i].tokid == 0) {
	// This is a new line in the stack trace - we search forward
	// until we get the last line in the current function.  This
	// is the "branch point" for that function.
	int j = i+1;
	while ((j < cstack.size()) && (cstack[j].cname == cstack[i].cname)
	       && (cstack[j].detail == cstack[i].detail) 
	       && (cstack[j].tokid != 0)) j++;
	sprintf(buffer,"In %s(%s), line %d, column %d\n",
		cstack[j-1].cname.c_str(),
		cstack[j-1].detail.c_str(),
		cstack[j-1].tokid & 0x0000FFFF,
		cstack[j-1].tokid >> 16);
	outstack.push_back(buffer);
	i = j;
      } else
	i++;
    }
    for (i=outstack.size()-1;i>=0;i--)
      io->outputMessage(outstack[i].c_str());
  }

  void WalkTree::pushDebug(std::string fname, std::string detail) {
    cstack.push_back(stackentry(fname,detail,0));
  }

  void WalkTree::popDebug() {
    if (!cstack.empty())
      cstack.pop_back();
    else
      io->outputMessage("IDERROR\n");
 }

  Interface* WalkTree::getInterface() {
    return io;
  }

  bool WalkTree::isUserClassDefined(std::string classname) {
    UserClass ret;
    return classTable.findSymbol(classname,ret);
  }
  
  UserClass WalkTree::lookupUserClass(std::string classname) {
    UserClass ret;
    classTable.findSymbol(classname,ret);
    return ret;
  }

  void WalkTree::registerUserClass(std::string classname, UserClass cdata) {
    classTable.insertSymbol(classname,cdata);
  }

  void WalkTree::setClassPrefix(std::string prefix) {
    classPrefix = prefix;
  }

  // Look up a function by name.  Use the arguments (if available) to assist
  // in resolving method calls for objects
  bool WalkTree::lookupFunction(std::string funcName, FuncPtr& val, 
				ArrayVector &args, bool disableOverload) {
    int passcount = 0;
    while(passcount < 2) {
      // This is the order for function dispatch according to the Matlab manual
      // Subfunctions
      if (context->lookupFunctionLocally(funcName,val))
	return true;
      // Private functions (not implemented yet)
      // Class constructor functions
      if (context->lookupFunctionGlobally(ClassMangleName(funcName,funcName),val))
	return true;
      if (!(disableOverload || stopoverload)) {
	// Look for a class method
	// Are any of the arguments classes?
	bool anyClasses = false;
	int i=0;
	while ((!anyClasses) && (i < args.size())) {
	  anyClasses = args[i].isUserClass();
	  if (!anyClasses) i++;
	}
	// Yes, try and resolve the call to a method
	if (anyClasses && ClassResolveFunction(this,args[i],funcName,val));
      }
      if (context->lookupFunction(funcName,val))
	return true;
      if (passcount == 0)
	io->rescanPath();
      passcount++;
    }
    return false;
  }

  Array WalkTree::rhsExpressionSimple(ASTPtr t) {
    Array r;
    ArrayVector m;
    bool isVar;
    bool isFun;
    pushID(t->context());
    // Try to satisfy the rhs expression with what functions we have already
    // loaded.
    isVar = context->lookupVariable(t->text,r);
    if (isVar && (t->down == NULL)) {
      popID();
      return r;
    }
    if (!isVar) {
      m = functionExpression(t,1,false);
      if (m.empty()) {
	popID();
	return Array::emptyConstructor();
      } else {
	popID();
	return m[0];
      }
    }
    popID();
    return Array::emptyConstructor();
  }

  //!
  //@Module Function Handles
  //@@Section VARIABLES
  //@@Usage
  //Starting with version 1.11, FreeMat now supports @|function handles|,
  //or @|function pointers|.  A @|function handle| is an alias for a function
  //or script that is stored in a variable.  First, the way to assign
  //a function handle is to use the notation
  //@[
  //    handle = @func
  //@]
  //where @|func| is the name to point to.  The function @|func| must exist
  //at the time we make the call.  It can be a local function (i.e., a
  //subfunction).  To use the @|handle|, we can either pass it to @|feval|
  //via 
  //@[
  //   [x,y] = feval(handle,arg1,arg2).
  //@]
  //Alternately, you can the function directly using the notation
  //@[
  //   [x,y] = handle(arg1,arg2)
  //@]
  //!
  ArrayVector WalkTree::FunctionPointerDispatch(Array r, ASTPtr args, 
						int narg_out) {
    const FunctionDef** dp;
    bool CLIFlagsave;
    dp = (const FunctionDef**) r.getDataPointer();
    FunctionDef* fun = (FunctionDef*) dp[0];
    if (!fun) return ArrayVector();
    if (args->opNum != OP_PARENS)
      throw Exception("Expected either '()' or function arguments inside parenthesis");
    ArrayVector m = expressionList(args->down);
    ArrayVector n;
    fun->updateCode();
    if (fun->scriptFlag) {
      if (!m.empty())
	throw Exception(std::string("Cannot use arguments in a call to a script."));
      CLIFlagsave = InCLI;
      InCLI = false;
      pushDebug(((MFunctionDef*)fun)->fileName,std::string("script"));
      block(((MFunctionDef*)fun)->code);
      popDebug();
      InCLI = CLIFlagsave;
    } else {
      CLIFlagsave = InCLI;
      InCLI = false;
      //HACK!
      int narg_out = 1;
      n = fun->evaluateFunction(this,m,narg_out);
      InCLI = CLIFlagsave;
    }
    return n;
  }

  //!
  //@Module INDEXING Indexing Expressions
  //@@Section VARIABLES
  //@@Usage
  //There are three classes of indexing expressions available 
  //in FreeMat: @|()|, @|{}|, and @|.|  Each is explained below
  //in some detail, and with its own example section.
  //@@Array Indexing
  //We start with array indexing @|()|,
  //which is the most general indexing expression, and can be
  //used on any array.  There are two general forms for the 
  //indexing expression - the N-dimensional form, for which 
  //the general syntax is
  //@[
  //  variable(index_1,index_2,...,index_n)
  //@]
  //and the vector form, for which the general syntax is
  //@[
  //  variable(index)
  //@]
  //Here each index expression is either a scalar, a range
  //of integer values, or the special token @|:|, which is
  //shorthand for @|1:end|.  The keyword @|end|, when included
  //in an indexing expression, is assigned the length of the 
  //array in that dimension.  The concept is easier to demonstrate
  //than explain.  Consider the following examples:
  //@<
  //A = zeros(4)
  //B = float(randn(2))
  //A(2:3,2:3) = B
  //@>
  //Here the array indexing was used on the left hand side only.
  //It can also be used for right hand side indexing, as in
  //@<
  //C = A(2:3,1:end)
  //@>
  //Note that we used the @|end| keyword to avoid having to know
  //that @|A| has 4 columns.  Of course, we could also use the 
  //@|:| token instead:
  //@<
  //C = A(2:3,:)
  //@>
  //An extremely useful example of @|:| with array indexing is for
  //slicing.  Suppose we have a 3-D array, that is @|2 x 2 x 3|,
  //and we want to set the middle slice:
  //@<
  //D = zeros(2,2,3)
  //D(:,:,2) = int32(10*rand(2,2))
  //@>
  //In another level of nuance, the assignment expression will
  //automatically fill in the indexed rectangle on the left using
  //data from the right hand side, as long as the lengths match.
  //So we can take a vector and roll it into a matrix using this
  //approach:
  //@<
  //A = zeros(4)
  //v = [1;2;3;4]
  //A(2:3,2:3) = v
  //@>
  //
  //The N-dimensional form of the variable index is limited
  //to accessing only (hyper-) rectangular regions of the 
  //array.  You cannot, for example, use it to access only
  //the diagonal elements of the array.  To do that, you use
  //the second form of the array access (or a loop).  The
  //vector form treats an arbitrary N-dimensional array as though
  //it were a column vector.  You can then access arbitrary 
  //subsets of the arrays elements (for example, through a @|find|
  //expression) efficiently.  Note that in vector form, the @|end|
  //keyword takes the meaning of the total length of the array
  //(defined as the product of its dimensions), as opposed to the
  //size along the first dimension.
  //@@Cell Indexing
  //The second form of indexing operates, to a large extent, in
  //the same manner as the array indexing, but it is by no means
  //interchangable.  As the name implies, @|cell|-indexing applies
  //only to @|cell| arrays.  For those familiar with @|C|, cell-
  //indexing is equivalent to pointer derefencing in @|C|.  First,
  //the syntax:
  //@[
  //  variable{index_1,index_2,...,index_n}
  //@]
  //and the vector form, for which the general syntax is
  //@[
  //  variable{index}
  //@]
  //The rules and interpretation for N-dimensional and vector indexing
  //are identical to @|()|, so we will describe only the differences.
  //In simple terms, applying @|()| to a cell-array returns another
  //cell array that is a subset of the original array.  On the other
  //hand, applying @|{}| to a cell-array returns the contents of that
  //cell array.  A simple example makes the difference quite clear:
  //@<
  //A = {1, 'hello', [1:4]}
  //A(1:2)
  //A{1:2}
  //@>
  //You may be surprised by the response to the last line.  The output
  //is multiple assignments to @|ans|!.  The output of a cell-array
  //dereference can be used anywhere a list of expressions is required.
  //This includes arguments and returns for function calls, matrix
  //construction, etc.  Here is an example of using cell-arrays to pass
  //parameters to a function:
  //@<
  //A = {[1,3,0],[5,2,7]}
  //max(A{1:end})
  //@>
  //And here, cell-arrays are used to capture the return.
  //@<
  //[K{1:2}] = max(randn(1,4))
  //@>
  //Here, cell-arrays are used in the matrix construction process:
  //@<
  //C = [A{1};A{2}]
  //@>
  //Note that this form of indexing is used to implement variable
  //length arguments to function.  See @|varargin| and @|varargout|
  //for more details.
  //@@Structure Indexing
  //The third form of indexing is structure indexing.  It can only
  //be applied to structure arrays, and has the general syntax
  //@[
  //  variable.fieldname
  //@]
  //where @|fieldname| is one of the fields on the structure.  Note that
  //in FreeMat, fields are allocated dynamically, so if you reference
  //a field that does not exist in an assignment, it is created automatically
  //for you.  If variable is an array, then the result of the @|.| 
  //reference is an expression list, exactly like the @|{}| operator.  Hence,
  //we can use structure indexing in a simple fashion:
  //@<
  //clear A
  //A.color = 'blue'
  //B = A.color
  //@>
  //Or in more complicated ways using expression lists for function arguments
  //@<
  //clear A
  //A(1).maxargs = [1,6,7,3]
  //A(2).maxargs = [5,2,9,0]
  //max(A.maxargs)
  //@>
  //or to store function outputs
  //@<
  //clear A
  //A(1).maxreturn = [];
  //A(2).maxreturn = [];
  //[A.maxreturn] = max(randn(1,4))
  //@>
  //FreeMat now also supports the so called dynamic-field indexing 
  //expressions.  In this mode, the fieldname is supplied through 
  //an expression instead of being explicitly provided.  For example,
  //suppose we have a set of structure indexed by color,
  //@<
  //x.red = 430;
  //x.green = 240;
  //x.blue = 53;
  //x.yello = 105
  //@>
  //Then we can index into the structure @|x| using a dynamic field
  //reference:
  //@<
  //y = 'green'
  //a = x.(y)
  //@>
  //Note that the indexing expression has to resolve to a string for
  //dynamic field indexing to work.
  //@@Complex Indexing
  //The indexing expressions described above can be freely combined
  //to affect complicated indexing expressions.  Here is an example
  //that exercises all three indexing expressions in one assignment.
  //@<
  //Z{3}.foo(2) = pi
  //@>
  //From this statement, FreeMat infers that Z is a cell-array of 
  //length 3, that the third element is a structure array (with one
  //element), and that this structure array contains a field named
  //'foo' with two double elements, the second of which is assigned
  //a value of pi.
  //!
  ArrayVector WalkTree::rhsExpression(ASTPtr t) {
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
    pushID(t->context());
    // Try to satisfy the rhs expression with what functions we have already
    // loaded.
    isVar = context->lookupVariable(t->text,r);
    if (isVar && (t->down == NULL)) {
      ArrayVector rv;
      rv.push_back(r);
      popID();
      return rv;
    }
    if (!isVar) {
      m = functionExpression(t,1,false);
      popID();
      return m;
    }
    // Check for a scalar function pointer element
    if (r.getDataClass() == FM_FUNCPTR_ARRAY &&
	r.isScalar()) {
      return FunctionPointerDispatch(r,t->down,1);
    }
    // If r is a user defined object, we have to divert to the
    // class function... (unless overloading is turned off)
    if (r.isUserClass() && !stopoverload) 
      return ClassRHSExpression(r,t->down,this);
    t = t->down;
    while (t != NULL) {
      if (!rv.empty()) 
	throw Exception("Cannot reindex an expression that returns multiple values.");
      if (t->opNum ==(OP_PARENS)) {
	m = variableSubIndexExpressions(t->down,r);
	if (m.size() == 0) 
	  throw Exception("Expected indexing expression!");
	else if (m.size() == 1) {
	  q = r.getVectorSubset(m[0]);
	  r = q;
	} else {
	  q = r.getNDimSubset(m);
	  r = q;
	}
      }
      if (t->opNum ==(OP_BRACES)) {
	m = variableSubIndexExpressions(t->down,r);
	if (m.size() == 0) 
	  throw Exception("Expected indexing expression!");
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
      if (t->opNum == (OP_DOTDYN)) {
      char *field;
      try {
	Array fname(expression(t->down));
	field = fname.getContentsAsCString();
      } catch (Exception &e) {
	throw Exception("dynamic field reference to structure requires a string argument");
      }
      rv = r.getFieldAsList(field);
      if (rv.size() <= 1) {
	r = rv[0];
	rv = ArrayVector();
      }      
      }
      t = t->right;
    }
    if (rv.empty())
      rv.push_back(r);
    popID();
    return rv;
  }

  WalkTree::WalkTree(Context* aContext, Interface* aInterface) {
    lasterr = NULL;
    context = aContext;
    state = FM_STATE_OK;
    endValStackLength = 0;
    endValStack[endValStackLength] = 0;
    depth = 0;
    io = aInterface;
    Array::setArrayIOInterface(io);
    InterruptPending = false;
    signal(SIGINT,sigInterrupt);
    printLimit = 1000;
    autostop = true;
    InCLI = false;
    debugActive = false;
    inStepMode = false;
    bpActive = false;
    stopoverload = false;
    clearStacks();
  }

  bool WalkTree::getStopOverload() {
    return stopoverload;
  }

  void WalkTree::setStopOverload(bool flag) {
    stopoverload = flag;
  }

  void WalkTree::dbstep(int linecount) {
    if (bpActive) {
      stepTrap.tokid = (stepTrap.tokid & 0xffff) + linecount;
      inStepMode = true;
      adjustBreakpoints();
      state = FM_STATE_RETURN;
    }
  }

  static std::string EvalPrep(char *line) {
    char buffer1[4096];
    char buffer2[4096];
    strcpy(buffer1,line);
    if (buffer1[strlen(buffer1)-1] == '\n')
      buffer1[strlen(buffer1)-1] = 0;
    if (buffer1[strlen(buffer1)-1] == '\r')
      buffer1[strlen(buffer1)-1] = 0;
    sprintf(buffer2,"%s",buffer1);
    if (strlen(buffer2) > 20) {
      buffer2[22] = buffer2[21] = buffer2[20] = '.'; 
      buffer2[23] = 0;
    }
    return std::string(buffer2);
  }

  bool WalkTree::evaluateString(char *line, bool propogateExceptions) {
    ASTPtr tree;
    ParserState parserState;
    
    InterruptPending = false;
    try{
      parserState = parseString(line);
    } catch(Exception &e) {
      e.printMe(io);
      return false;
    }

    if (parserState != ScriptBlock)
      return false;

    tree = getParsedScriptBlock();
    try {
      pushDebug("Eval",EvalPrep(line));
      block(tree);
      if (state == FM_STATE_RETURN) {
	if (depth > 0) {
	  popDebug();
	  return true;
	}
      }
      if (state == FM_STATE_QUIT || state == FM_STATE_RETALL) {
	popDebug();
	return true;
      }
    } catch(Exception &e) {
      if (propogateExceptions)
	throw;
      e.printMe(io);
    }
    popDebug();
    return false;
  }
  
  char* WalkTree::getLastErrorString() {
    if (lasterr)
      return lasterr;
    else
      return "";
  }

  void WalkTree::setLastErrorString(char* txt) {
    if (lasterr) free(lasterr);
    lasterr = txt;
  }

  void WalkTree::evalCLI() {
    char *line;
    char dataline[4096];
    char prompt[150];
    int lastCount;

    if ((depth == 0) || (cstack.size() == 0))
      if (bpActive)
	sprintf(prompt,"D-> ");
      else
	sprintf(prompt,"--> ");
    else
      if (bpActive)	
	sprintf(prompt,"[%s,%d] D-> ",cstack.back().detail.c_str(),
		cstack.back().tokid & 0xffff);
      else
	sprintf(prompt,"[%s,%d] --> ",cstack.back().detail.c_str(),
		cstack.back().tokid & 0xffff);
    while(1) {
      line = io->getLine(prompt);
      if (!line)
	continue;
      // scan the line and tokenize it
      setLexBuffer(line);
      lastCount = 0;
      try {
	if (lexCheckForMoreInput(0)) {
	  lastCount = getContinuationCount();
	  // Need multiple lines..  This code is _really_ bad - I need
	  // a better interface...
	  strcpy(dataline,line);
	  bool enoughInput = false;
	  // Loop until we have enough input
	  while (!enoughInput) {
	    // Get more text
	    line = io->getLine("");
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
      } catch (Exception &e) {
	e.printMe(io);
	line = NULL;
      }
      InCLI = true;
      if (line && (strlen(line) > 0)) {
	int stackdepth;
	stackdepth = cstack.size();
	bool tresult =  evaluateString(line);
	while (cstack.size() > stackdepth) cstack.pop_back();
	if (tresult) {
	  InCLI = false;
	  return;
	}
      }
    }
  }
}

