#include "Token.hpp"
#include "Serialize.hpp"
#include <iostream>
#include <errno.h>

// These must appear as sequential token numbers
string fm_reserved[22] = {
  "break",
  "case",
  "catch",
  "continue",
  "dbstep",
  "dbtrace",
  "else",
  "elseif",
  "end",
  "for",
  "function",
  "global",
  "if",
  "keyboard",
  "otherwise",
  "persistent",
  "quit",
  "retall",
  "return",
  "switch",
  "try",
  "while"
};

int fm_reserved_count = sizeof(fm_reserved)/sizeof(fm_reserved[0]);

Token::Token(byte tok, unsigned pos, string text) :
  m_tok(tok), m_pos(pos), m_text(text), m_bpflag(false) {
}

Token::Token() {
}

bool Token::IsBinaryOperator() const {
  return ((m_tok == '+') || (m_tok == '-') ||
	  (m_tok == '*') || (m_tok == '/') ||
	  (m_tok == '\\') || (m_tok == '^') ||
	  (m_tok == '>') || (m_tok == '<') || 
	  (m_tok == ':') || (m_tok == TOK_LE) ||
	  (m_tok == TOK_GE) || (m_tok == TOK_EQ) ||
	  (m_tok == TOK_NE) || (m_tok == TOK_SOR) ||
	  (m_tok == TOK_SAND) || (m_tok == TOK_DOTTIMES) ||
	  (m_tok == TOK_DOTRDIV) || (m_tok == TOK_DOTLDIV) ||
	  (m_tok == TOK_DOTPOWER) || (m_tok == '|') ||
	  (m_tok == '&'));
}

bool Token::IsUnaryOperator() const {
  return ((m_tok == '+') || (m_tok == '-') || (m_tok == '~')
	  || (m_tok == TOK_UNARY_MINUS) || 
	  (m_tok == TOK_UNARY_PLUS));
}


bool Token::IsRightAssociative() const {
  return (m_tok == '^');
}

ostream& operator<<(ostream& o, const Token& b) {
  o << TokenToString(b) << " (" << (b.Position() >> 16)
    << "," << (b.Position() & 0xffff);
  if (b.BPFlag())
    o << "," << "BP)\r\n";
  else
    o << ")\r\n";
  return o;
}

string TokenToString(const Token& b) {
  switch(b.Value()) {
  case TOK_IDENT: return "(ident)"+b.Text();
  case TOK_NUMBER: return b.Text();
  case TOK_SPACE: return "space";
  case TOK_STRING: return "(string)"+b.Text();
  case TOK_KEYWORD: return "keyword";
  case TOK_BREAK: return "break";
  case TOK_CASE: return "case";
  case TOK_CATCH: return "catch";
  case TOK_CONTINUE: return "continue";
  case TOK_DBSTEP: return "dbstep";
  case TOK_ELSE: return "else";
  case TOK_ELSEIF: return "elseif";
  case TOK_END: return "end";
  case TOK_FOR: return "for";
  case TOK_FUNCTION: return "function";
  case TOK_GLOBAL: return "global";
  case TOK_IF: return "if";
  case TOK_KEYBOARD: return "keyboard";
  case TOK_OTHERWISE: return "otherwise";
  case TOK_PERSISTENT: return "persistent";
  case TOK_QUIT: return "quit";
  case TOK_RETALL: return "retall";
  case TOK_RETURN: return "return";
  case TOK_SWITCH: return "switch";
  case TOK_TRY: return "try";
  case TOK_WHILE: return "while";
    // Generated (synthetic) token;
  case TOK_MULTI: return "multi";
  case TOK_SPECIAL: return "special";
  case TOK_VARIABLE: return "variable";
  case TOK_DYN: return "dyn";
  case TOK_BLOCK: return "block";
  case TOK_EOF: return "eof";
  case TOK_MATDEF: return "matdef";
  case TOK_CELLDEF: return "celldef";
  case TOK_PARENS: return "()";
  case TOK_BRACES: return "{}";
  case TOK_BRACKETS: return "[]";
  case TOK_ROWDEF: return "row";
  case TOK_UNARY_MINUS: return "u-";
  case TOK_UNARY_PLUS: return "u+";
  case TOK_EXPR: return "expr";
  case TOK_DOTTIMES: return ".*";
  case TOK_DOTRDIV: return "./";
  case TOK_DOTLDIV: return ".\\";
  case TOK_DOTPOWER: return ".^";
  case TOK_DOTTRANSPOSE: return ".'";
  case TOK_LE: return "<=";
  case TOK_GE: return ">=";
  case TOK_EQ: return "==";
  case TOK_NE: return "~=";
  case TOK_SOR: return "||";
  case TOK_SAND: return "&&";
  case TOK_QSTATEMENT: return "qstmnt";
  case TOK_STATEMENT: return "stmnt";
  case TOK_INTEGER: return "(int)" + b.Text();
  case TOK_FLOAT: return "(float)" + b.Text();
  case TOK_DOUBLE: return "(double)" + b.Text();
  case TOK_COMPLEX: return "(complex)" + b.Text();
  case TOK_DCOMPLEX: return "(dcomplex)" + b.Text();
  case TOK_FUNCTION_DEFS: return "functions:";
  case TOK_SCRIPT: return "script:";
  case TOK_DBTRACE: return "dbtrace";
  }
  return string(1,(char) b.Value());
}

void FreezeToken(const Token& a, Serialize *s) {
  s->putByte(a.m_tok);
  s->putInt(a.m_pos);
  s->putString(a.m_text.c_str());
  s->putBool(a.m_bpflag);
}

Token ThawToken(Serialize *s) {
  Token a;
  a.m_tok = s->getByte();
  a.m_pos = s->getInt();
  a.m_text = s->getString();
  a.m_bpflag = s->getBool();
  a.FillArray();
  return a;
}

void Token::FillArray() {
  Array retval;
  switch(m_tok) {
  case TOK_INTEGER:
    int iv;
    double fv;
    iv = strtol(m_text.c_str(),NULL,10);
    if ((errno == ERANGE) && ((iv == LONG_MAX) || (iv == LONG_MIN))) {
      fv = strtod(m_text.c_str(),NULL);
      retval = Array::doubleConstructor(fv);
    } else {
      retval = Array::int32Constructor(iv);
    }
    break;
  case TOK_FLOAT:
    retval = Array::floatConstructor(atof(m_text.c_str()));
    break;
  case TOK_DOUBLE:
    retval = Array::doubleConstructor(atof(m_text.c_str()));
    break;
  case TOK_COMPLEX:
    retval = Array::complexConstructor(0,atof(m_text.c_str()));
    break;
  case TOK_DCOMPLEX:
    retval = Array::dcomplexConstructor(0,atof(m_text.c_str()));
    break;
  case TOK_STRING:
    retval = Array::stringConstructor(m_text);
    break;
  }
  m_array = retval;
}
