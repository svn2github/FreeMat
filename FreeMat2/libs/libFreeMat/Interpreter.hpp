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

#ifndef __Interpreter_h__
#define __Interpreter_h__

#include "Tree.hpp"
#include "Context.hpp"
#include "FunctionDef.hpp"
#include "Array.hpp"
#include <stack>
#include <QStringList>
#include <QObject>

extern "C" char* lasterr;

namespace FreeMat {

  class InterpreterContinueException : public std::exception {};
  class InterpreterBreakException : public std::exception {};
  class InterpreterReturnException : public std::exception {};
  class InterpreterRetallException : public std::exception {};
  class InterpreterQuitException : public std::exception {};

  class stackentry {
  public:
    std::string cname;
    std::string detail;
    int tokid;
    
    stackentry(std::string cntxt, std::string detail, int id);
    stackentry();
    ~stackentry();
  };

  typedef Array (*BinaryFunc)(Array, Array);
  typedef Array (*UnaryFunc)(Array);

  class UserClass;

  /**
   * This is the class that implements the interpreter - it generally
   * operates on abstract syntax trees (ASTs).
   */
  class Interpreter : public QObject {
    Q_OBJECT

    int errorCount;
    std::string classPrefix;
    /**
     * The context that the intepreter operates in.
     */
    Context* context;
    /**
     * The debug depth.  Each time the command line interface is invoked
     * via a keyboard command, the debug depth is increased.
     */
    int depth;  
    /**
     * The print limit (how many elements get printed before output 
     * stops).
     */
    int printLimit;
    /**
     * The last error that occured.
     */
    char *lasterr;
    /**
     * autostop storage flag
     */
    bool autostop;
    /**
     * When this flag is active, autostop does nothing.
     */
    bool InCLI;

    // The debug stack - this stack tracks our current location
    // in each file, as well as the files themselves.
    std::string ip_funcname;
    std::string ip_detailname;
    int ip_context;
    std::vector<stackentry> cstack;
    std::vector<stackentry> bpStack;
    bool inStepMode;
    int lineNumber;
    stackentry stepTrap;
    bool bpActive;
    bool ignoreBP;
    int ignoreBPLineNumber;
    std::string ignoreBPName;
    // This data structure stores the class records
    SymbolTable<UserClass> classTable;
    void collectKeywords(tree q, ArrayVector &keyvals,
			 treeVector &keyexpr, stringVector &keywords);
    int* sortKeywords(ArrayVector &m, stringVector &keywords,
		      stringVector arguments, ArrayVector keyvals);
    void handlePassByReference(tree q, stringVector arguments,
			       ArrayVector m,stringVector keywords,
			       treeVector keyexpr, int* argTypeMap);
    Array DoBinaryOperator(tree t, BinaryFunc fnc, std::string fname);
    Array DoUnaryOperator(tree t, UnaryFunc fnc, std::string fname);
    ArrayVector FunctionPointerDispatch(Array r, tree args, int narg_out);
    // Set this flag to stop overloading of functions
    bool stopoverload;
    void doDebugCycle();
    bool processguievents;
  protected:
    QStringList m_basePath;
    QStringList m_userPath;
    void scanDirectory(std::string scdir, bool, std::string prefixo);
    void procFileM(std::string fname, std::string fullname, bool);
    void procFileP(std::string fname, std::string fullname, bool);
    void procFileMex(std::string fname, std::string fullname, bool);
    std::string app_path;
    int m_ncols;
  public:
    std::vector<std::string> GetCompletions(std::string line, int word_end, 
					    std::string &matchString);
    /**
     *  Get/Set the application path 
     */
    std::string getAppPath();
    void setAppPath(std::string);
    /**
     *  The Base Path is the one that contains .m files in the current app bundle
     */
    void setBasePath(QStringList);
    /**
     *  The User Path is the one that the user can tinker with.
     */
    void setUserPath(QStringList);
    /**
     *  Get the current path set for the interface. (user path - legacy interface)
     */
    std::string getPath();
    /**
     *  Get the current path set for the interface (base path + user path)
     */
    std::string getTotalPath();
    /**
     *  Set the path for the interface. (user path - legacy interface)
     */
    void setPath(std::string);
    /**
     *  Set the context for the interface.
     */
    void setContext(Context *ctxt);
    /**
     *  Force a rescan of the current path to look for 
     *  new function files.
     */
    void rescanPath();
    /**
     *  Return the width of the current "terminal" in
     *  characters.
     */
    int getTerminalWidth();
    /**
     *  Set the width of the current "terminal" in characters
     */
    void setTerminalWidth(int ncols);
  public:
    /**
     *  Output the following text message.
     */
    void outputMessage(std::string msg);
    /**
     *  Output the following error message.
     */
    void errorMessage(std::string msg);
    /**
     *  Output the following warning message.
     */
    void warningMessage(std::string msg);
  signals:
    void outputRawText(std::string);
    void CWDChanged();
  public:
    void run();
    void sendGreeting(); // Say hello
  public:
    /**
     *  Get the context for the interface.
     */
    Context* getContext();
    static std::string getVersionString();
    int getErrorCount();
    bool inMethodCall(std::string classname);

    bool getStopOverload();
    void setStopOverload(bool flag);

    void setClassPrefix(std::string prefix);
    std::string getClassPrefix();
    bool debugActive;
    void dbstep(int linecount);
    void debugCLI();
    void pushDebug(std::string fname, std::string detail);
    void popDebug();
    
    void addBreakpoint(stackentry bp, bool registerIt = true);
    bool adjustBreakpoint(stackentry &bp, bool dbstep);
    void adjustBreakpoints();
    void listBreakpoints();
    void deleteBreakpoint(int number);
    void stackTrace(bool includeCurrent);

    bool isUserClassDefined(std::string classname);
    UserClass lookupUserClass(std::string classname);
    void registerUserClass(std::string classname, UserClass cdata);

    /**
     * Construct a Interpreter object with the given context to operate
     * in.
     */
    Interpreter(Context* aContext);
    /**
     * Destruct the Interpreter object.
     */
    ~Interpreter();
    inline void SetContext(int id);
    /**
     * Set the autostop flag - this flag determines what happens when
     * an exception occurs
     */
    bool AutoStop();
    void AutoStop(bool a);
    /**
     * Stop automatic processing of GUI events between statements
     */
    void GUIEventFlag(bool);
    /**
     * Reenable automatic processing of GUI events between statements
     */
    bool GUIEventFlag();
    /**
     * Set the print limit (number of element printed prior to truncation).
     */
    void setPrintLimit(int);
    /**
     * Get the print limit (number of element printed prior to truncation).
     */
    int getPrintLimit();
    /**
     * Clear the context stacks.
     */
    void clearStacks();
    /**
     * Convert an expression list into a vector of Array variables.
     */
    ArrayVector rowDefinition(tree t);
    /**
     * Convert a matrix definition of the form: [expr1,expr2;expr3;expr4] into
     * a vector of row definitions.  The first row is the vector [expr1,expr2], and
     * the second is the vector [expr3,expr4].  The AST input should look like:
     *  []
     *   |
     *   ;-> ; -> ... -> NULL
     *   |   |
     *   |   rowDef
     *   rowDef
     */
    Array matrixDefinition(tree t);
    /**
     * Convert a cell defintion of the form: {expr1,expr2;expr3;expr4} into
     * a vector of row definitions.  The first row is the vector {expr1,expr2}, and
     * the second is the vector {expr3,expr4}.  The AST input should look like:
     *  {}
     *   |
     *   ;-> ; -> ... -> NULL
     *   |   |
     *   |   rowDef
     *   rowDef
     */
    Array cellDefinition(tree t);
    /**
     * Evaluate the expression pointed to by the AST t into a variable.
     */
    Array expression(tree t);
    /**
     * Evaluate a unit colon expression.  The AST input should look like:
     *   :
     *   |
     *   expr1->expr2
     *
     * The output is the expression expr1:expr2, which is the vector
     * [expr1,expr1+1,expr1+2,...,expr1+n], where n is the largest 
     * integer such that expr1+n <= expr2.
     */
    Array unitColon(tree t);
    /**
     * Evaluate a double colon expression.  The AST input should look like:
     *   :
     *   |
     *   :->expr3
     *   |
     *   expr1->expr2
     * The output is the expression expr1:expr2:expr3, which is the
     * vector [expr1,expr1+expr2,expr1+2*expr2,...,expr1+n*expr2], where
     * n is the largest integer such that expr1+n*expr2 <= expr3.
     */
    Array doubleColon(tree t);
    /**
     * Process a sequence of expressions into a vector of Arrays.
     * The input AST must be:
     *   expr1->expr2->expr3...->NULL
     * If the dim argument is non-NULL, then before the nth expression
     * is evaluated, the end value is set to the nth dimension length.
     * Also, if one of the expressions is a multi-valued RHS expression,
     * then it is expanded into the result.  Also, if one of
     * the expressions is the ':' token, then the corresponding
     * expression is replaced with either 1:length (if the ':' is
     * a singleton) or 1:dim->getDimensionLength(k).  This is only
     * valid if we are a subindexing expression list (i.e., 
     * VAR(exprssionlist)), in which case dim != NULL.
     */
    ArrayVector expressionList(treeVector t);
    Array EndReference(Array v, int index, int count);
    Array AllColonReference(Array v, int index, int count);
    void expressionStatement(tree t, bool printIt);
    ArrayVector varExpressionList(treeVector t, Array subroot);
    /**
     * The RHS expression is used to represent an rvalue in an
     * assignment statement (or an implicit assignment such as 
     * -->2+3).  The form of the AST depends on the head.  If the
     * head identifier is a function name, then the form of the
     * AST is:
     *    ident
     *     |
     *     ()
     *     |
     *     expr1->expr2->...
     * On the other hand, if the identifier represents a variable
     * (variables are checked for first, incidentally), then the AST
     * looks like:
     *    ident
     *     |
     *     ()->{}->.
     *     |   |   |
     *     |   |   field
     *     |   expr3->...
     *     expr1->expr2->...
     * Throws an Exception 
     *    - if the identifier cannot be matched to
     *      either a variable or function.  
     *    - 
     */
    ArrayVector rhsExpression(tree t);
    /**
     * Look up an identifier as a potential function name, using a
     * rescan if the identifier is not found on the first pass.
     */
    bool lookupFunction(std::string funcName, FuncPtr& val, ArrayVector& args, 
			bool disableOverload = false);
    /**
     * Special case the single assignment statement 'A = B' for speed.
     */
    void assignmentStatement(tree t, bool printIt);
    inline Array rhsExpressionSimple(tree t);
    /**
     * Process an AST to form an lvalue in an assignment statement.
     * The AST looks like:
     *    ident
     *     |
     *     ()->{}->.
     *     |   |   |
     *     |   |   field
     *     |   expr3->...
     *     expr1->expr2->...
     * The lhsExpression method creates a LeftHandSide object that
     * captures the original object and the subindexing expressions
     * applied to it.  Throws an Exception if the indexing expressions
     * are empty.
     */

    int countLeftHandSides(tree t);
    
    Array assignExpression(tree t, Array &value);
    Array assignExpression(tree t, ArrayVector &value, bool multipleLHS = true);

    /**
     * Evaluate a function and return the results of the function as
     * an ArrayVector.  For scripts, the body of the function is
     * evaluated directly.  Otherwise, the function evaluates itself
     * (using the FunctionDef::evaluateFunction member function).  The
     * arguments to the function are unpacked from the AST ast follows
     *   ident
     *    |
     *    ()
     *    |
     *    expr1->expr2->...
     * The outputOptional flag allows the function to not assign an 
     * output.
     * Throws an exception if 
     *    - something other than a parenthesis pair "()" follows the 
     *      identifier.  
     *    - if too many arguments are passed to the function.
     *    - too many outputs are requested from the function.
     */
    ArrayVector functionExpression(tree t, int narg_out, bool outputOptional);
    /**
     * A multifunction call is an expression of the sort
     * [expr1,expr2,...,exprn] = func(args).  The AST is
     *    multiCall
     *       |
     *       []->functionExpression_AST
     *       |
     *       ;->NULL
     *       |
     *       rowDefs
     * When calculating the number of left hand sides for the
     * multifunction, single variables without subindexing expressions
     * are counted as a single left hand side.  Thus, an expression
     * such as [A{1:3},B] = func(args) counts for four left hand
     * sides, regardless of the contents of B.
     * If the printIt flag is set, each of the outputs is to be
     * written to the console.
     * Throws an exception if the AST is malformed (i.e., the '[]' is
     * missing, or there are multiple rows in the left hand side.).
     */
    void multiFunctionCall(tree t, bool printIt);
    /**
     * A special function call is an expression of the form
     * >> func arg1 arg2 arg3
     * which is represented in an AST is
     *     scall
     *       |
     *       fname->arg
     */
    void specialFunctionCall(tree t, bool printIt);
    /**
     * Handles an if statement, corresponding to an if, a number
     * of elseif blocks and an optional else statement.  The AST looks
     * like
     *     cstat->elseIfBlock->elseCode
     *      |        |
     *      |       cstat->cstat->cstat
     *     expr->codeBlock
     * where each of the elseIf blocks are tested sequentially until
     * one of them succeeds.
     */
    void ifStatement(tree t);
    /**
     * Handle a switch statement.  The switch statement tests
     * an expression against a number of case blocks.  If a 
     * case block matches, the code in the case block is 
     * executed, and the switch statement quits.  If none of the
     * case blocks match, the otherwise code is executed. The 
     * AST looks like
     *     expr->caseBlock->otherwiseCode
     *              |
     *             testCase->testCase->...->NULL
     * Where testCase is the AST passed to testCaseStatement. For
     * a switch statement, the switch value must be either a 
     * scalar value or a string.  The test values in each case
     * statement can either be the same type, or a cell array.
     * If it is a cell array, then the switch value is compared
     * with each entry in the case expression.
     * Throws an Exception if the switch expression is not
     * either a scalar or a string.
     */
    void switchStatement(tree t);
    /**
     * Implements the for control statement.  The AST looks like
     *     ident->codeBlock
     *       |
     *      expr
     * The identifier used as the control variable in the for 
     * statement.  The variable defined by the identifier takes
     * on each of the values in the expression.  For each such
     * assignment, the code in the codeBlock is executed.
     */
    void forStatement(tree t);
    /**
     * Implements the while control statement.  The AST looks like
     *     expr->codeBlock
     * The test expression is evaluated until it fails, and for each
     * successful expression, the codeBlock is executed.
     */
    void whileStatement(tree t);
    /**
     * Implements the try statement.  The AST looks like
     *     block->catchBlock
     * The code in block is executed.  If an exception occurs, then
     * the code in catchBlock is executed.
     */
    void tryStatement(tree t);
    /**
     * Implements the global statement (really a global declaration).
     * The AST looks like:
     *    ident
     *     |
     *    ident
     *     |
     *    etc.
     * Each identifier is added to the global variable list of
     * the current context.
     */
    void globalStatement(tree t);
    /**
     * Implements the persistent statement (really a persistent declaration).
     * The AST looks like:
     *    ident
     *     |
     *    ident
     *     |
     *    etc.
     * Each identifier is added to the persistent variable list of
     * the current context.
     */
    void persistentStatement(tree t);
    /**
     * This somewhat strange test is used by the switch statement.
     * If x is a scalar, and we are a scalar, this is an equality
     * test.  If x is a string and we are a string, this is a
     * strcmp test.  If x is a scalar and we are a cell-array, this
     * test is applied on an element-by-element basis, looking for
     * any matches.  If x is a string and we are a cell-array, then
     * this is applied on an element-by-element basis also.
     * The AST for this looks like:
     *     FM_CASE
     *       |
     *      expr->codeBlock
     * The expression is compared to x using Array::testForCaseMatch.
     * If a match is found, then the codeBlock is executed.  The result of
     * the test is returned.  Throws an exception if the AST is
     * malformed.
     */
    bool testCaseStatement(tree t, Array x);
    /**
     * Execute the statement described by the AST - the printIt flag
     * determines if the result of the statement should be printed to
     * the console.  The form of the AST required depends on the
     * type of statement being executed.  The various statement types
     * handled are as follows:
     *   =          Assignment of expression to LHS
     *   | 
     *  LHS->expr
     *
     *  multicall   Multifunction call
     *   |
     *  multicallBody
     *
     *  FM_FOR      For statement
     *   |
     *  forBody
     *
     *  FM_WHILE    While statement
     *   |
     *  whileBody
     *
     *  FM_IF       If statement
     *   |
     *  ifBody
     *
     *  FM_BREAK    Break statement - change the state of the interpreter to
     *              FM_STATE_BREAK
     *  
     *  FM_CONTINUE Continue statement - change the state of the interpreter
     *              to FM_STATE_CONTINUE
     *
     *  FM_RETURN   Return statement - change the state to FM_STATE_RETURN
     *
     *  FM_SWITCH   Switch statement
     *   |
     *  switchBody
     *
     *  FM_TRY      Try statement
     *   |
     *  tryBody  
     *
     *  FM_KEYBOARD Enter another CLI session
     *
     *  FM_GLOBAL   A global declaration
     *   |
     *  globalBody
     *
     *  FM_PERSISTENT A persistent declaration
     *   |
     *  persistentBody
     *
     *  rhs         A function call being evaluated as a statement
     *   |
     *  funcName
     *
     *  expr        An expression being evaluated as a statement
     *
     * The function call is trapped before the expression to handle
     * the special variable "ans".
     * Throws an Exception if the statement type is not recognized.
     */
    void statementType(tree t, bool printIt);
    /**
     * The statement method simply screens out the need for the
     * printIt flag.  It also retrieves the statement context
     * for use in error reporting.  The AST looks like:
     *   qstatement          A quiet statement (printIt -> false)
     *      |
     *    context (optional)
     *      |
     *  statementBody
     *
     *   rstatement          A normal statement (printIt -> true)
     *      |
     *    context (optional)
     *      |
     *  statementBody
     * The context data is supplied by the parse (indicates the
     * line number and filename if necessary).
     */
    void statement(tree t);
    /**
     * Executes a sequence of statements, trapping exceptions
     * as necessary.  The AST looks like
     *   <ignored>
     *      |
     *    statement->statement->statement
     * If an exception occurs, it is caught and rethrown.  The
     * lasterr string is also set to the contents of the exception.
     *
     */
    void block(tree t);
    /**
     * Start a command line interface.  Statements are retrieved
     * from the console, and executed sequentially until a "return"
     * statement is executed or the user presses 'CTRL-D'.
     */
    //    void evalCLI();
    /**
     * The workhorse routine - "evaluate" the contents of a string
     * and execute it.  The flag indicates whether or not exceptions
     * are propogated or printed.
     */
    void evaluateString(string line, bool propogateExceptions = false);
    /**
     * Get the last error that occurred.
     */
    char* getLastErrorString();
    /**
     * Set the text for the last error.
     */
    void setLastErrorString(char* txt);
    /**
     * Handles the logistics of shortcut evaluation
     */
    Array ShortCutOr(tree t);
    Array ShortCutAnd(tree t);
    /**
     * Display an array - added so user classes divert to "display" function
     */
    void displayArray(Array b);
    /**
     * Retrieve data about the current location of the instruction pointer
     */
    std::string getMFileName();
    /**
     * Mangle a function name to get the private version (if it exists)
     */
    std::string getPrivateMangledName(std::string fname);
    ArrayVector subsindex(ArrayVector m);
    ArrayVector subsrefParen(Array r, tree t);
    ArrayVector subsrefBrace(Array r, tree t);
    ArrayVector subsrefDot(Array r, tree t);
    ArrayVector subsrefDotDyn(Array r, tree t);
    ArrayVector subsrefSingle(Array r, tree t);
    ArrayVector subsref(Array r, treeVector t);
    void subsassignParen(Array *r, tree t, ArrayVector& value);
    void subsassignBrace(Array *r, tree t, ArrayVector& value);
    void subsassignDot(Array *r, tree t, ArrayVector& value);
    void subsassignDotDyn(Array *r, tree t, ArrayVector& value);
    void subassignSingle(Array *r, tree t, ArrayVector& value);
    void subassign(Array *r, tree t, ArrayVector& value);
    int countSubExpressions(tree t);

    void ExecuteLine(std::string txt);
  };
  void sigInterrupt(int arg);

  std::string TrimFilename(std::string);

  char* TildeExpand(char* path);

}

#endif
