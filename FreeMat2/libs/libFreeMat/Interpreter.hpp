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
#include <string>
#include <vector>
#include <QStringList>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>

using namespace std;

class InterpreterContinueException : public exception {};
class InterpreterBreakException : public exception {};
class InterpreterReturnException : public exception {};
class InterpreterRetallException : public exception {};
class InterpreterQuitException : public exception {};
class InterpreterKillException : public exception {};

class stackentry {
public:
  string cname;
  string detail;
  int tokid;
  int number;
  int steptrap;
  int stepcurrentline;
    
  // A number of -1 corresponds to a temporary breakpoint 
  stackentry(string cntxt, string detail, int id, 
	     int num = 0, int strp = 0, int stcl = 0);
  stackentry();
  ~stackentry();
};

class Interpreter;
class UserClass;

typedef Array (*BinaryFunc)(Array, Array, Interpreter*);
typedef Array (*UnaryFunc)(Array, Interpreter*);

/**
 * This is the class that implements the interpreter - it generally
 * operates on abstract syntax trees (ASTs).
 */
class Interpreter : public QThread {
  Q_OBJECT

  /******************************************
   *  Class Members for the Interpreter     *
   ******************************************/

  /**
   * Members related to "end" references.
   */
  ArrayReference endRef;
  int endTotal;
  int endCount;

  /**
   * Debugger related values
   */
  int steptrap;
  int stepcurrentline;
  int tracetrap;
  int tracecurrentline;
  string stepname;

  /**
   * Tracks the number of errors that have occured (used by the documentation
   * compiler helpgen)
   */
  int errorCount;
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
  string lasterr;
  /**
   * autostop storage flag
   */
  bool autostop;
  /**
   * When this flag is active, autostop does nothing.
   */
  bool InCLI;
  /**
   * This mutex protects the command buffer.
   */
  QMutex mutex;
  /**
   * The command buffer
   */
  vector<string> cmd_buffer;
  /**
   * A buffer of return values from graphics calls
   */
  vector<ArrayVector> gfx_buffer;
  /**
   * A flag to indicate that the gfx call failed
   */
  bool gfxErrorOccured;
  /**
   * The corresponding exception.
   */
  string gfxError;
  /**
   * A synchronization variable to wait on when the command buffer is empty
   */
  QWaitCondition bufferNotEmpty;
  /**
   * A synchronization variable to wait on when the gfx buffer is empty
   */
  QWaitCondition gfxBufferNotEmpty;
  /**
   * This is the equivalent of an "instruction pointer".  It stores the
   * function name and some detailed information on our location.
   */
  string ip_funcname;
  string ip_detailname;
  int ip_context;
  /**
   * For technical reasons, the interpreter stores a mirror of the call stack
   * in this member.
   */
  vector<stackentry> cstack;
  /**
   * A list of breakpoints
   */
  vector<stackentry> bpStack;
  /**
   * True if the interpreter is in single-step mode
   */
  bool inStepMode;
  /**
   * The effective location of the next breakpoint (if single stepping)
   */
  stackentry stepTrap;
  /**
   * The class records (store information about each user defined class)
   */
  SymbolTable<UserClass> classTable;
  /**
   * Set this flag to true to stop overloading of methods
   */
  bool stopoverload;
  /**
   * The base path - contains all of the directories for FreeMat-shipped code
   */
  QStringList m_basePath;
  /**
   * The user path - contains all of the directories for user defined FreeMat code
   */
  QStringList m_userPath;
  /**
   * The width of the output in characters
   */
  int m_ncols;
  /**
   * This flag controls the greeting
   */
  bool m_skipflag;
  /**
   * Our thread ID
   */
  int m_threadID;
  /**
   * The thread function we are executing
   */
  FunctionDef *m_threadFunc;
  /**
   * The arguments to the thread function we are executing
   */
  ArrayVector m_threadFuncArgs;
  /**
   * The number of return arguments expected
   */
  int m_threadNargout;
  /**
   * The return value of the thread function
   */
  ArrayVector m_threadFuncRets;
  /**
   * The error state of the thread function (true if an exception
   * occured).
   */
  bool m_threadErrorState;
  /**
   * Interrupt the current thread at the next opportunity
   */
  bool m_interrupt;
   /**
   * Kill the thread at the next opportunity
   */
  bool m_kill;
  /**
   * The current state of diary output
   */
  bool m_diaryState;
  /**
   * The filename for the diary
   */
  string m_diaryFilename;
  /**
   * The capture string
   */
  string m_capture;
  /**
   * The capture state
   */
  bool m_captureState;
  /******************************************
   *  Public Methods for the Interpreter    *
   ******************************************/
public:
  /**
   * Construct a Interpreter object with the given context to operate
   * in.
   */
  Interpreter(Context* aContext);
  /**
   * Destruct the Interpreter object.
   */
  ~Interpreter();
  /**
   * Set the thread ID to the given number...
   */
  inline void setThreadID(int threadID) {m_threadID = threadID;}
  /**
   * Return the thread ID
   */
  inline int getThreadID() const {return m_threadID;}
  /**
   * Set the thread function and the arguments
   */
  inline void setThreadFunc(FunctionDef *threadFunc, 
			    int nargout,
			    ArrayVector threadFuncArgs) { 
    m_threadFunc = threadFunc;  
    m_threadFuncArgs = threadFuncArgs; 
    m_threadNargout = nargout;
    m_threadErrorState = false;
  }
  /**
   * Manipulate the diary state
   */
  inline bool getDiaryState() {return m_diaryState;}
  inline void setDiaryState(bool t) {m_diaryState = t;}
  inline void setDiaryFilename(string name) {m_diaryFilename = name;}
  void diaryMessage(string msg);
  /**
   * Manipulate the capture state
   */
  inline void clearCaptureString() {m_capture = "";}
  inline void setCaptureState(bool t) {m_captureState = t;}
  inline string getCaptureString() {return m_capture;}
  /**
   * Get the result of the thread function evaluation
   */
  inline ArrayVector getThreadFuncReturn() {return m_threadFuncRets;}
  /**
   * Get the error state - is true if the thread function throws
   * an exception.
   */
  inline bool getLastErrorState() {return m_threadErrorState;}
  /**
   * Activate the interrupt for the thread - so that it stops.
   */
  inline void setInterrupt() {m_interrupt = true;}
   /**
   * Activate the kill for the thread - so that it dies.
   */
  inline void setKill() {m_kill = true;}
  /**
   * Run a command-line-interface interface from this thread.
   */
  void doCLI();
  /**
   * Queue a command for execution
   */
  void ExecuteLine(string txt);
  /**
   * Retrieve data about the current location of the instruction pointer
   */
  string getMFileName();
  string getInstructionPointerFileName();
  /**
   * The Base Path is the one that contains .m files in the current app bundle
   */
  void setBasePath(QStringList);
  /**
   * The User Path is the one that the user can tinker with.
   */
  void setUserPath(QStringList);
  /**
   * Get the current path set for the interface. (user path - legacy interface)
   */
  string getPath();
  /**
   * Get the current path set for the interface (base path + user path)
   */
  string getTotalPath();
  /**
   * Set the path for the interface. (user path - legacy interface)
   */
  void setPath(string);
  /**
   * Force a rescan of the current path to look for 
   * new function files.
   */
  void rescanPath();
  /**
   * Return the width of the current "terminal" in
   * characters.
   */
  int getTerminalWidth();
  /**
   * Set the width of the current "terminal" in characters
   */
  void setTerminalWidth(int ncols);
  /**
   * Output the following text message.
   */
  void outputMessage(string msg);
  /**
   * Output the following error message.
   */
  void errorMessage(string msg);
  /**
   * Output the following warning message.
   */
  void warningMessage(string msg);
  /**
   * Start the interpreter running.
   */
  void run();
  /**
   * Get/Set the overload stop flag
   */
  bool getStopOverload();
  void setStopOverload(bool flag);
  /**
   * Retrieve the version string for the interpreter
   */
  static string getVersionString();
  /**
   * Retrieve the number of errors that occured.  The
   * side effect is that the error count is set to zero.
   */
  int getErrorCount();
  /**
   * True if we are currently in the method of a class
   */
  bool inMethodCall(string classname);
  /**
   * Get the context for the interface.
   */
  Context* getContext();
  /**
   * Refresh the breakpoints in the code.  Call this after updating
   * code for functions, or breakpoints will not work.
   */
  void refreshBreakpoints();
  /**
   * Add a new breakpoint
   */
  MFunctionDef* lookupFullPath(string name);
  void addBreakpoint(stackentry bp);
  void addBreakpoint(string name, int line) ;
  /**
   * Activate a breakpoint in the code.  If the line number for the
   * breakpoint is negative, the breakpoint is set as a step trap.  
   * If the line number is positive, and the enable flag is set 
   * then a breakpoint is set.  If the enable flag is false then
   * the breakpoint is cleared.
   */
  void setBreakpoint(stackentry bp, bool enableFlag);
  /**
   * List all breakpoints
   */
  void listBreakpoints();
  /**
   * Delete a breakpoint from the persistent list
   */
  void deleteBreakpoint(int number);
  /**
   * Generate a stacktrace
   */
  void stackTrace(bool includeCurrent);
  /**
   * Returns true if a userclass with the given name is defined
   */
  bool isUserClassDefined(string classname);
  /**
   * Lookup a user class.
   */
  UserClass lookupUserClass(string classname);
  /**
   * Register a new user class.
   */
  void registerUserClass(string classname, UserClass cdata);
  /**
   * Push a function name and detail onto the debug stack
   */
  void pushDebug(string fname, string detail);
  /**
   * Pop the debug stack
   */
  void popDebug();
  /**
   * Step the given number of lines
   */
  void dbstepStatement(const tree &t);
  void dbtraceStatement(const tree &t);
  /**
   * Set the autostop flag - this flag determines what happens when
   * an exception occurs
   */
  bool AutoStop();
  void AutoStop(bool a);
  /**
   * Set the print limit (number of element printed prior to truncation).
   */
  void setPrintLimit(int);
  /**
   * Get the print limit (number of element printed prior to truncation).
   */
  int getPrintLimit();
  /**
   * The workhorse routine - "evaluate" the contents of a string
   * and execute it.  The flag indicates whether or not exceptions
   * are propogated or printed.
   */
  void evaluateString(string line, bool propogateExceptions = false);
  /**
   * Get the last error that occurred.
   */
  string getLastErrorString();
  /**
   * Set the text for the last error.
   */
  void setLastErrorString(string txt);
  /**
   * Set to false to turn off the greeting.
   */
  void setGreetingFlag(bool skip);
  /**
   * Register the result of a gfx call
   */
  void RegisterGfxResults(ArrayVector m);
  /**
   * Register an error that occurs with a gfx call
   */
  void RegisterGfxError(string msg);
  /**
   * Simplified interface for function lookup.
   */
  bool lookupFunction(string funcName, FuncPtr& val);
   /**
   * Prompt the user for input, and return the answer.
   */
  string getLine(string prompt);
  /**
   * Go to sleep for the specified number of milliseconds
   */
  void sleepMilliseconds(unsigned long msecs);
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
  void block(const tree &t);
  /**
   * Start a command line interface.  Statements are retrieved
   * from the console, and executed sequentially until a "return"
   * statement is executed or the user presses 'CTRL-D'.
   */
  void evalCLI();

  bool isBPSet(QString fname, int lineNumber);
  bool isInstructionPointer(QString fname, int lineNumber);
  void toggleBP(QString fname, int lineNumber);

  /******************************************
   *  Signals for the Interpreter           *
   ******************************************/
signals:
  /**
   * Send a string of text to a terminal somewhere
   */
  void outputRawText(string);
  /**
   * User has changed the current working directory
   */
  void CWDChanged();
  /**
   * Change the prompt
   */
  void SetPrompt(string);
  /**
   * Dispatch a graphics call
   */
  void doGraphicsCall(Interpreter*, FuncPtr, ArrayVector, int);
  /**
   * All done.
   */
  void QuitSignal();
  /**
   * Something went wrong...
   */
  void CrashedSignal();
  /**
   * Refresh the breakpoints 
   */
  void RefreshBPLists();
  /**
   * Show the current active line
   */
  void ShowActiveLine();

  /******************************************
   *  Private Methods for the Interpreter   *
   ******************************************/
private:
  /**
   * Execute a function that accesses the graphics subsystem.  All such
   * accesses must be made through the main thread.  So the interpreter
   * sends a signal out that a graphics call needs to be made, and then
   * waits for the result.
   */
  ArrayVector doGraphicsFunction(FuncPtr f, ArrayVector m, int narg_out);
  /**
   * Collect information about keyword usage and the relevant 
   * expressions from a function call
   */
  void collectKeywords(const tree &q, ArrayVector &keyvals,
		       treeVector &keyexpr, stringVector &keywords);
  /**
   * Sort keywords into a proper call order.
   */
  int* sortKeywords(ArrayVector &m, stringVector &keywords,
		    stringVector arguments, ArrayVector keyvals);
  /**
   * For values passed by reference, update the caller's variables.
   */
  void handlePassByReference(const tree &q, stringVector arguments,
			     ArrayVector m,stringVector keywords,
			     treeVector keyexpr, int* argTypeMap);
  /**
   * Perform a binary operator with the given name
   */
  Array DoBinaryOperator(const tree &t, BinaryFunc fnc, string fname);
  /**
   * Perform a unary operator with the given name
   */
  Array DoUnaryOperator(const tree &t, UnaryFunc fnc, string fname);
  /**
   * Dispatch a function pointer
   */
  ArrayVector FunctionPointerDispatch(Array r, const tree &args, int narg_out);
  /**
   * Set the context of the interpreter.  This is an integer that indicates
   * where in the source file we are.
   */
  inline void SetContext(int id);
  /**
   * Clear the context stacks.
   */
  void clearStacks();
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
  Array matrixDefinition(const tree &t);
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
  Array cellDefinition(const tree &t);
  /**
   * Evaluate the expression pointed to by the AST t into a variable.
   */
  Array expression(const tree &t);

  ArrayVector handleReindexing(const tree &t, const ArrayVector &p);
  
  /**
   * Evaluate the expression into a variable-array
   */
  void multiexpr(const tree &t, ArrayVector& m, int lhsCount = 1);

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
  Array unitColon(const tree &t);
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
  Array doubleColon(const tree &t);
  /**
   * Decode references to ":" inside variable dereferences.
   */
  Array AllColonReference(Array v, int index, int count);
  /**
   * Handle statements that are simply expressions
   */
  void expressionStatement(const tree &t, bool printIt);
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
  ArrayVector rhsExpression(const tree &t, int lhsCount = 1);

  void assign(ArrayReference r, const tree &s, Array &data);

  void multiassign(ArrayReference r, const tree &s, ArrayVector& m);

  void deref(Array &r, const tree &s);

  Array rhs(const tree &t);
  /**
   * Look up an identifier as a potential function name, using a
   * rescan if the identifier is not found on the first pass.
   */
  bool lookupFunction(string funcName, FuncPtr& val, 
		      ArrayVector& args, bool disableOverload = false);
  /**
   * Special case the single assignment statement 'A = B' for speed.
   */
  void assignment(const tree &var, bool printIt, Array &b);
  /**
   * Try to figure out how many outputs there should be to a multifunction
   * call.  In particular, logic is used to figure out what to do about
   * undefined variables.
   */
  int countLeftHandSides(const tree &t);
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
  void functionExpression(const tree &t, int narg_out, bool outputOptional, ArrayVector &output);
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
  void multiFunctionCall(const tree &t, bool printIt);
  /**
   * A special function call is an expression of the form
   * >> func arg1 arg2 arg3
   * which is represented in an AST is
   *     scall
   *       |
   *       fname->arg
   */
  void specialFunctionCall(const tree &t, bool printIt);
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
  void ifStatement(const tree &t);
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
  void switchStatement(const tree &t);
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
  void forStatement(const tree &t);
  /**
   * Implements the while control statement.  The AST looks like
   *     expr->codeBlock
   * The test expression is evaluated until it fails, and for each
   * successful expression, the codeBlock is executed.
   */
  void whileStatement(const tree &t);
  /**
   * Implements the try statement.  The AST looks like
   *     block->catchBlock
   * The code in block is executed.  If an exception occurs, then
   * the code in catchBlock is executed.
   */
  void tryStatement(const tree &t);
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
  void globalStatement(const tree &t);
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
  void persistentStatement(const tree &t);
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
  bool testCaseStatement(const tree &t, Array x);
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
  void processBreakpoints(const tree &t);
  void statementType(const tree &t, bool printIt);
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
  void statement(const tree &t);
  /**
   * Handles the logistics of shortcut evaluation
   */
  Array ShortCutOr(const tree &t);
  Array ShortCutAnd(const tree &t);
  /**
   * Display an array - added so user classes divert to "display" function
   */
  void displayArray(Array b);
  /**
   * Mangle a function name to get the private version (if it exists)
   */
  string getPrivateMangledName(string fname);
  /**
   * Convert variables into indexes, calls "subsindex" for user classes.
   */
  Array subsindex(const Array &m);
  void subsindex(ArrayVector& m);
  /**
   * Enter a debug cycle
   */
  void doDebugCycle();
  /**
   * Send a greeting to the user.
   */
  void sendGreeting();
  /**
   * Scan the given directory for .m, .p, classes, etc.  If tempfunc
   * is true, then these functions are marked as temporary (so changing
   * the working directory flushes them).
   */
  void scanDirectory(string scdir, bool tempfunc, string prefixo);
  /**
   * Add the specified .m file to the current context
   */
  void procFileM(string fname, string fullname, bool tempfunc);
  /**
   * Add the specified .p file to the current context
   */
  void procFileP(string fname, string fullname, bool tempfunc);
  /**
   * Add the specified .mex file to the current context
   */
  void procFileMex(string fname, string fullname, bool tempfunc);

  friend Array IndexExpressionToStruct(Interpreter*, const tree&, Array);
  friend ArrayVector ClassRHSExpression(Array, const tree&, Interpreter*);
  friend ArrayVector PCodeFunction(int, const ArrayVector&, Interpreter*);
  friend class MFunctionDef;
  friend class ImportedFunctionDef;
};

void sigInterrupt(int arg);
string TrimFilename(string);
QString TildeExpand(string);
#endif
