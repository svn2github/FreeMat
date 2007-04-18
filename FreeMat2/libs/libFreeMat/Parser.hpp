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
  bool Match(byte t);
  void Consume();
  const Token& Expect(byte t);
  const Token& Next();
  unsigned Precedence(const Token&);
  bool MatchNumber();
private:
  tree TransposeFixup(tree base);
  tree ForStatement();
  tree FunctionDefinition();
  tree ForIndexExpression();
  tree SingletonStatement();
  tree DBStepOrTraceStatement();
  tree WhileStatement();
  tree IfStatement();
  tree SwitchStatement();
  tree TryStatement();
  tree Expression();
  tree Statement();
  tree DeclarationStatement();
  tree StatementSeperator();
  tree Identifier();
  tree AssignmentStatement();
  tree VariableDereference(bool blankRefOK = true);
  tree MultiFunctionCall();
  tree MatDef(byte basetok, byte closebracket);
  tree MatrixDefinition();
  tree RowVectorDefinition();
  tree SpecialFunctionCall();
  tree Keyword();
  tree Exp(unsigned p);
  tree PrimaryExpression();
  tree AnonymousFunction();
public:
  Parser(Scanner& lex);
  tree Process(); 
  string LastErr() {return lasterr;}
  unsigned LastPos() {return lastpos;}
  tree StatementList();
  void Dump(); 

  friend tree ParseExpressionString(string);
};

tree ParseString(string arg);
tree ParseExpressionString(string arg);

#endif
