#include "Parser.hpp"
// This one is still a question:
//    [1 3' + 5]
// Need: 
//       Error reporting
//
// Done:
//       Reference counting for the tree?
//       strings (embedded quotes)
//       Comments
//       ...
//       Number recognition
//       Function handles
//       Keywords
//       Scripts
//
// [1 -2] --> [1 -2] 
// [1 - 2] --> [-1]
// [1 - - 2] --> [3]
// [1 - 2 - 3 -4] --> [-4 -4]
// [1 - 2 - 3 - 4] --> -8
// [1 -2 - 3 -4] --> [1 -5 -4]
// [- 3] --> [-3]
// [2 -(3)] --> [2 -3]
// [(2 -(3))] --> [-1]
// [1 --2] --> [1 2]
//
// An additional set of cases to consider:
// [A (3)]  [A {4}] 
// both of which are incorrectly parsed as
// [A(3)] and [A{4}] 
//
// Question:
//
//   if a +b print
//
// Suggestion.. if have unary operator followed by a nonwhitespace
// tag it as a possible unary operator.  
//
// Conclusion - if we have tight binding between the unary operators 
// 
// Conclusion - do not allow spaces after unary operators (unless you have to)
// [a' 1 b'] --> [a',1,b']
// [a ' 1 b'] --> [a,' 1 b']
// [a ' 1 b] --> error
// [a .' 1 b] --> error
// Conclusion - do not allow spaces before transpose operators
// [1 -3, 4] --> [1,-3,4]
// Conclusion - spaces and commas are equivalent!
// [a (3)] --> [a,3], not [a(3)]
// [(a (3))] --> [a(3)]
// Outside
// fprintf('%d\n',a (3)) --> works as a(3)


tree Parser::StatementSeperator() {
  tree root;
  if (Match(';')) {
    Consume();
    if (Match('\n')) 
      Consume();
    root = mkLeaf(TOK_QSTATEMENT);
  } else if (Match('\n')) {
    Consume();
    root = mkLeaf(TOK_STATEMENT);
  } else if (Match(',')) {
    Consume();
    root = mkLeaf(TOK_STATEMENT);
  }
  return root;
}

tree Parser::SingletonStatement() {
  tree root(mkLeaf(Next()));
  Consume();
  return root;
}

tree Parser::MultiFunctionCall() {
  tree root(mkLeaf(Expect('[')));
  root.Rename(TOK_MULTI);
  tree lhs = mkLeaf(TOK_BRACKETS);
  while (!Match(']')) {
    addChild(lhs,VariableDereference());
    if (Match(',')) Consume();
  }
  Expect(']');
  addChild(root,lhs);
  Expect('=');
  addChild(root,Identifier());
  if (Match('(')) {
    Consume();
    tree sub = mkLeaf(TOK_PARENS);
    while (!Match(')')) {
      addChild(sub,Expression());
      if (Match(',')) Consume();
    }
    Expect(')');
    addChild(root,sub);
  }
  return root;
}

tree Parser::FunctionDefinition() {
  tree root(mkLeaf(Expect(TOK_FUNCTION)));
  if (Match('[')) {
    Consume();
    tree lhs = mkLeaf(TOK_BRACKETS);
    while (!Match(']')) {
      addChild(lhs,Identifier());
      if (Match(',')) Consume();
    }
    Expect(']');
    addChild(root,lhs);
    Expect('=');
    addChild(root,Identifier());
  } else {
    // Two possible parses here
    tree save = Identifier();
    if (Match('=')) {
      tree lhs = mkLeaf(TOK_BRACKETS);
      addChild(lhs,save);
      addChild(root,lhs);
      Expect('=');
      addChild(root,Identifier());
    } else {
      addChild(root,mkLeaf(TOK_BRACKETS));
      addChild(root,save);
    }
  }
  // Process (optional) args
  if (Match('(')) {
    Consume();
    tree args = mkLeaf(TOK_PARENS);
    while (!Match(')')) {
      tree ident;
      if (Match('&')) {
	ident = mkLeaf(Expect('&'));
	addChild(ident,Identifier());
      } else 
	ident = Identifier();
      addChild(args,ident);
      if (Match(',')) Consume();
    }
    Expect(')');
    addChild(root,args);
  } else {
    addChild(root,mkLeaf(TOK_PARENS));
  }
  StatementSeperator();
  addChild(root,StatementList());
  return root;
}

bool Parser::MatchNumber() {
  return (Match(TOK_INTEGER) || Match(TOK_FLOAT) ||
	  Match(TOK_DOUBLE) || Match(TOK_COMPLEX) || 
	  Match(TOK_DCOMPLEX));
}

tree Parser::SpecialFunctionCall() {
  tree root = mkLeaf(TOK_SPECIAL);
  addChild(root,Identifier());
  if (!(Match(TOK_IDENT) || MatchNumber() || Match(TOK_STRING)))
    serror("Not special call");
  while (Match(TOK_IDENT) || MatchNumber() || Match(TOK_STRING)) {
    addChild(root,mkLeaf(Next()));
    Consume();
  }
  return root;
}

tree Parser::ForIndexExpression() {
  if (Match('(')) {
    Consume();
    tree ret = ForIndexExpression();
    Expect(')');
    return ret;
  }
  tree ident = Identifier();
  if (Match('=')) {
    tree root(mkLeaf(Next()));
    Consume();
    tree expr = Expression();
    addChild(root,ident,expr);
    return root;
  } else
    return ident;
}

tree Parser::ForStatement() {
  tree root(mkLeaf(Expect(TOK_FOR)));
  tree index = ForIndexExpression();
  tree term = StatementSeperator();
  tree block = StatementList();
  Expect(TOK_END);
  addChild(root,index,block);
  return root;
}

tree Parser::WhileStatement() {
  tree root(mkLeaf(Expect(TOK_WHILE)));
  tree warg = Expression();
  StatementSeperator();
  tree block = StatementList();
  Expect(TOK_END);
  addChild(root,warg,block);
  return root;
}

tree Parser::IfStatement() {
  tree root(mkLeaf(Expect(TOK_IF)));
  tree test = Expression();
  StatementSeperator();
  tree trueblock = StatementList();
  addChild(root,test,trueblock);
  while (Match(TOK_ELSEIF)) {
    tree elseif(mkLeaf(Next()));
    Consume();
    tree test = Expression();
    tree block = StatementList();
    addChild(elseif,test,block);
    addChild(root,elseif);
  }
  if (Match(TOK_ELSE)) {
    tree elseblk(mkLeaf(Next()));
    Consume();
    tree block = StatementList();
    addChild(elseblk,block);
    addChild(root,elseblk);
  }
  Expect(TOK_END);
  return root;
}

tree Parser::Identifier() {
  if (!Match(TOK_IDENT))
    serror("expecting identifier");
  tree ret = mkLeaf(Next());
  Consume();
  return ret;
}

tree Parser::DeclarationStatement() {
  tree root(mkLeaf(Next()));
  Consume();
  while (Match(TOK_IDENT))
    addChild(root,Identifier());
  return root;
}

tree Parser::TryStatement() {
  tree root(mkLeaf(Expect(TOK_TRY)));
  StatementSeperator();
  tree block = StatementList();
  addChild(root,block);
  if (Match(TOK_CATCH)) {
    tree catchblock(mkLeaf(Next()));
    Consume();
    StatementSeperator();
    tree block = StatementList();
    addChild(catchblock,block);
    addChild(root,catchblock);
  }
  Expect(TOK_END);
  return root;
}

tree Parser::Keyword() {
  tree root(mkLeaf(Expect('/')));
  addChild(root,Identifier());
  if (Match('=')) {
    Consume();
    addChild(root,Expression());
  }
  return root;
}

// Parse A(foo).goo{1:3}... etc
tree Parser::VariableDereference() {
  tree ident = Identifier();
  tree root = mkLeaf(TOK_VARIABLE);
  addChild(root,ident);
  bool deref = true;
  while (deref) {
    if (Match('(')) {
      Consume();
      tree sub = mkLeaf(TOK_PARENS);
      while (!Match(')')) {
	if (Match(':'))
	  addChild(sub,mkLeaf(Expect(':')));
	else if (Match('/'))
	  addChild(sub,Keyword());
	else
	  addChild(sub,Expression());
	if (Match(',')) Consume();
      }
      Expect(')');
      addChild(root,sub);
    } else if (Match('{')) {
      Consume();
      tree sub = mkLeaf(TOK_BRACES);
      while (!Match('}')) {
	addChild(sub,Expression());
	if (Match(',')) Consume();
      }
      Expect('}');
      addChild(root,sub);
    } else if (Match('.')) {
      tree dynroot(mkLeaf(Next()));
      Consume();
      if (Match('(')) {
	Consume();
	dynroot.Rename(TOK_DYN);
	addChild(dynroot,Identifier());
	addChild(root,dynroot);
	Expect(')');
      } else {
	addChild(dynroot,Identifier());
	addChild(root,dynroot);
      }
    } else
      deref = false;
  }
  return root;
}

tree Parser::AssignmentStatement() {
  tree ident = VariableDereference();
  tree root(mkLeaf(Expect('=')));
  tree expr = Expression();
  addChild(root,ident,expr);
  return root;
}

tree Parser::SwitchStatement() {
  tree root(mkLeaf(Expect(TOK_SWITCH)));
  tree swexpr = Expression();
  addChild(root,swexpr);
  StatementSeperator();
  while (Match(TOK_CASE)) {
    tree caseblock(mkLeaf(Next()));
    Consume();
    tree csexpr = Expression();
    StatementSeperator();
    tree block = StatementList();
    addChild(caseblock,csexpr,block);
    addChild(root,caseblock);
  }
  if (Match(TOK_OTHERWISE)) {
    tree otherwise(mkLeaf(Next()));
    Consume();
    StatementSeperator();
    tree block = StatementList();
    addChild(otherwise,block);
    addChild(root,otherwise);
  }
  Expect(TOK_END);
  return root;
}

tree Parser::Statement() {
  if (Match(TOK_EOF))
    return NULL;
  if (Match(TOK_END))
    return NULL;
  if (Match(TOK_FOR))
    return ForStatement();
  if (Match(TOK_BREAK))
    return SingletonStatement();
  if (Match(TOK_CONTINUE))
    return SingletonStatement();
  if (Match(TOK_WHILE))
    return WhileStatement();
  if (Match(TOK_IF))
    return IfStatement();
  if (Match(TOK_SWITCH))
    return SwitchStatement();
  if (Match(TOK_TRY))
    return TryStatement();
  if (Match(TOK_KEYBOARD))
    return SingletonStatement();
  if (Match(TOK_RETURN))
    return SingletonStatement();
  if (Match(TOK_RETALL))
    return SingletonStatement();
  if (Match(TOK_QUIT))
    return SingletonStatement();
  if (Match(TOK_GLOBAL))
    return DeclarationStatement();
  if (Match(TOK_PERSISTENT))
    return DeclarationStatement();
  // Now come the tentative parses
  if (Match(TOK_IDENT)) {
    try {
      m_lex.Save();
      tree retval = AssignmentStatement();
      m_lex.Continue();
      return retval;
    } catch (ParseException &e) {
      m_lex.Restore();
    } 
  }
  if (Match('[')) {
    try {
      m_lex.Save();
      tree retval = MultiFunctionCall();
      m_lex.Continue();
      return retval;
    } catch (ParseException &e) {
      m_lex.Restore();
    }
  }
  if (Match(TOK_IDENT)) {
    try {
      m_lex.Save();
      tree retval = SpecialFunctionCall();
      m_lex.Continue();
      return retval;
    } catch (ParseException &e) {
      m_lex.Restore();
    } 
  }
  try {
    m_lex.Save();
    tree retval = Expression();
    m_lex.Continue();
    return retval;
  } catch (ParseException &e) {
    m_lex.Restore();
  }
  return NULL;
}

tree Parser::StatementList() {
  tree stlist = mkLeaf(TOK_BLOCK);
  while (StatementSeperator().valid());
  tree statement = Statement();
  while (statement.valid()) {
    tree sep = StatementSeperator();
    if (!sep.valid()) return stlist;
    addChild(sep,statement);
    addChild(stlist,sep);
    while (StatementSeperator().valid());
    statement = Statement();
  }
  return stlist;
}

tree Parser::Expression() {
  if (Match(TOK_SPACE)) Consume();
  tree root(mkLeaf(TOK_EXPR));
  addChild(root,Exp(0));
  return root;
}

Parser::Parser(Scanner& lex) : m_lex(lex) {
}

const Token& Parser::Next() {
  return m_lex.Next();
}

void Parser::serror(string errmsg) {
  if (m_lex.Position() > lastpos) {
    lasterr = errmsg;
    lastpos = m_lex.Position();
  }
  throw ParseException(m_lex.Position(),errmsg);
}

const Token & Parser::Expect(byte a) {
  const Token & ret(Next());
  if (!m_lex.Next().Is(a)) {
    serror(string("Expecting ") + TokenToString(Token(a,0)));
  }  else {
    Consume();
  }
  return ret;
}

unsigned Parser::Precedence(const Token& t) {
  switch(t.Value()) {
  case TOK_SOR: return 1;
  case TOK_SAND: return 2;
  case '|': return 3;
  case '&': return 4;
  case '<': return 5;
  case '>': return 5;
  case TOK_LE : return 5;
  case TOK_GE: return 5;
  case TOK_EQ: return 5;
  case TOK_NE: return 5;
  case ':': return 6;
  case '+': return 7;
  case '-': return 7;
  case '*': return 8;
  case '/': return 8;
  case '\\': return 8;
  case TOK_DOTTIMES: return 8;
  case TOK_DOTRDIV: return 8;
  case TOK_DOTLDIV: return 8;
  case TOK_UNARY_PLUS: return 9;
  case TOK_UNARY_MINUS: return 9;
  case '^': return 10;
  case TOK_DOTPOWER: return 10;
  }
  return 1;
}

tree Parser::MatDef(byte basetok, byte closebracket) {
  m_lex.PushWSFlag(false);
  tree matdef(mkLeaf(basetok));
  while (!Match(closebracket)) {
    tree rowdef(mkLeaf(TOK_ROWDEF));
    while (!Match(';') && !Match(closebracket)) {
      addChild(rowdef,Expression());
      if (Match(',') || Match(TOK_SPACE))
	Consume();
    }
    if (Match(';'))
      Consume();
    addChild(matdef,rowdef);
  }
  m_lex.PopWSFlag();
  return matdef;
}

tree Parser::TransposeFixup(tree base) {
  while ((Next().Value() == '\'') || (Next().Value() == TOK_DOTTRANSPOSE)) {
    base = mkNode(Next(),base);
    Consume();
  }
  if (Match(TOK_SPACE))
    if (!((m_lex.Peek(0,'-') || m_lex.Peek(0,'+')) && !m_lex.Peek(1,' ')))
      Consume();
  return base;
}

tree Parser::PrimaryExpression() {
  if (Next().IsUnaryOperator()) {
    Token opr(Next());
    Consume();
    if (Match(TOK_SPACE)) Consume();
    if (opr.Is('+')) opr.SetValue(TOK_UNARY_PLUS);
    if (opr.Is('-')) opr.SetValue(TOK_UNARY_MINUS);
    unsigned q = Precedence(opr);
    tree child = Exp(q);
    tree root(mkNode(opr,child));
    return root;
  } else if (Match('(')) {
    Consume();
    m_lex.PushWSFlag(true);
    tree t = Exp(0);
    m_lex.PopWSFlag();
    Expect(')');
    return TransposeFixup(t);
  } else if (Match('@')) {
    tree root(mkLeaf(Next()));
    Consume();
    addChild(root,Identifier());
    return TransposeFixup(root);
  } else if (MatchNumber() || Match(TOK_STRING)) {
    tree t = mkLeaf(Next());
    Consume();
    return TransposeFixup(t);
  } else if (Match(TOK_END)) {
    return TransposeFixup(mkLeaf(Expect(TOK_END)));
  } else if (Match(TOK_IDENT)) {
    tree t = VariableDereference();
    return TransposeFixup(t);
  } else if (Match('[')) {
    Consume();
    tree t = MatDef(TOK_MATDEF,']');
    Expect(']');
    return TransposeFixup(t);
  } else if (Match('{')) {
    Consume();
    tree t = MatDef(TOK_CELLDEF,'}');
    Expect('}');
    return TransposeFixup(t);
  } else
    serror("unrecognized token");
}

tree Parser::Exp(unsigned p) {
  tree t = PrimaryExpression();
  while (Next().IsBinaryOperator() && (Precedence(Next()) >= p)) {
    Token opr_save(Next());
    Consume();
    if (Match(TOK_SPACE)) Consume();
    unsigned q;
    if (opr_save.IsRightAssociative())
      q = Precedence(opr_save);
    else
      q = 1+Precedence(opr_save);
    tree t1 = Exp(q);
    t = mkNode(opr_save,t,t1);
  }
  return t;
}

bool Parser::Match(byte a) {
  return m_lex.Next().Is(a);
}

void Parser::Consume() {
  m_lex.Consume();
}

tree Parser::Process() {
  lastpos = 0;
  tree root;
  while (Match('\n'))
    Consume();
  if (Match(TOK_FUNCTION)) {
    root = mkLeaf(TOK_FUNCTION_DEFS);
    while (Match(TOK_FUNCTION))
      addChild(root,FunctionDefinition());
    if (!Match(TOK_EOF))
      serror("expected a valid statement or expression");
  } else {
    root = mkLeaf(TOK_SCRIPT);
    addChild(root,StatementList());
    if (!Match(TOK_EOF))
      serror("expected a valid statement or expression");
  }
  return root;
}
