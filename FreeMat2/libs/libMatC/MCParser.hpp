#ifndef __MCParser_hpp__
#define __MCParser_hpp__

#include <string>
#include "MCScanner.hpp"
#include "Tree.hpp"

using namespace std;

class MCParser {
  MCScanner &m_lex;
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
  Tree* whileStatement();
  Tree* ifStatement();
  Tree* switchStatement();
  Tree* tryStatement();
  Tree* expression();
  Tree* statement();
  Tree* typeDeclarationStatement();
  Tree* statementSeperator();
  Tree* identifier();
  Tree* assignmentStatement();
  Tree* variableDereference(bool blankRefOK = true);
  Tree* multiFunctionCall();
  Tree* matDef(byte basetok, byte closebracket);
  Tree* matrixDefinition();
  Tree* rowVectorDefinition();
  Tree* exp(unsigned p);
  Tree* primaryExpression();
  Tree* statementList();
public:
  MCParser(MCScanner& lex);
  CodeBlock process();
  CodeBlock processStatementList();
  string lastErr() {return lasterr;}
  unsigned lastPos() {return lastpos;}
  void dump(); 

};

#endif
