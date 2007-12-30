#ifndef __Parser_hpp__
#define __Parser_hpp__

#include <string>
#include "Scanner.hpp"
#include "Tree.hpp"

using namespace std;

class Parser {
  Scanner &m_lex;
  unsigned lastpos;
  string lasterr;
  void serror(string);
  bool match(byte t);
  void consume();
  const Token& expect(byte t);
  const Token& next();
  bool matchNumber();
  void flushSeperators();
private:
  Tree* transposeFixup(Tree* base);
  Tree* forStatement();
  Tree* functionDefinition();
  Tree* forIndexExpression();
  Tree* singletonStatement();
  Tree* dBStepOrTraceStatement();
  Tree* whileStatement();
  Tree* ifStatement();
  Tree* switchStatement();
  Tree* tryStatement();
  Tree* expression();
  Tree* statement();
  Tree* declarationStatement();
  Tree* statementSeperator();
  Tree* identifier();
  Tree* assignmentStatement();
  Tree* variableDereference(bool blankRefOK = true);
  Tree* multiFunctionCall();
  Tree* matDef(byte basetok, byte closebracket);
  Tree* matrixDefinition();
  Tree* rowVectorDefinition();
  Tree* specialFunctionCall();
  Tree* keyword();
  Tree* exp(unsigned p);
  Tree* primaryExpression();
  Tree* anonymousFunction();
  Tree* statementList();
public:
  Parser(Scanner& lex);
  CodeBlock process();
  CodeBlock processStatementList();
  string lastErr() {return lasterr;}
  unsigned lastPos() {return lastpos;}
  void dump(); 

  friend CodeBlock ParseExpressionString(string);
};

CodeBlock ParseString(string arg);
CodeBlock ParseExpressionString(string arg);

#endif
