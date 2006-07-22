#ifndef __Token_hpp__
#define __Token_hpp__

#include <string>

using namespace std;

typedef unsigned char byte;

const byte TOK_IDENT = 130;
const byte TOK_NUMBER = 131;
const byte TOK_SPACE = 132;
const byte TOK_STRING = 133;
const byte TOK_KEYWORD = 134;
const byte TOK_BREAK = 135;
const byte TOK_CASE = 136;
const byte TOK_CATCH = 137;
const byte TOK_CONTINUE = 138;
const byte TOK_ELSE = 139;
const byte TOK_ELSEIF = 140;
const byte TOK_END = 141;
const byte TOK_FOR = 142;
const byte TOK_FUNCTION = 143;
const byte TOK_GLOBAL = 144;
const byte TOK_IF = 145;
const byte TOK_KEYBOARD = 146;
const byte TOK_OTHERWISE = 147;
const byte TOK_PERSISTENT = 148;
const byte TOK_QUIT = 149;
const byte TOK_RETALL = 150;
const byte TOK_RETURN = 151;
const byte TOK_SWITCH = 152;
const byte TOK_TRY = 153;
const byte TOK_WHILE = 154;
// Generated (synthetic) token;
const byte TOK_MULTI = 155;
const byte TOK_SPECIAL = 156;
const byte TOK_VARIABLE = 157;
const byte TOK_DYN = 158;
const byte TOK_BLOCK = 159;
const byte TOK_EOF = 160;
const byte TOK_MATDEF = 161;
const byte TOK_CELLDEF = 162;
const byte TOK_PARENS = 163;
const byte TOK_BRACES = 164;
const byte TOK_BRACKETS = 165;
const byte TOK_ROWDEF = 166;
const byte TOK_UNARY_MINUS = 167;
const byte TOK_UNARY_PLUS = 168;
const byte TOK_EXPR = 169;
const byte TOK_DOTTIMES = 170;
const byte TOK_DOTRDIV = 171;
const byte TOK_DOTLDIV = 172;
const byte TOK_DOTPOWER = 173;
const byte TOK_DOTTRANSPOSE = 174;
const byte TOK_LE = 175;
const byte TOK_GE = 176;
const byte TOK_EQ = 177;
const byte TOK_NE = 178;
const byte TOK_SOR = 179;
const byte TOK_SAND = 180;
const byte TOK_QSTATEMENT = 181;
const byte TOK_STATEMENT = 182;
const byte TOK_INTEGER = 183;
const byte TOK_FLOAT = 184;
const byte TOK_DOUBLE = 185;
const byte TOK_COMPLEX = 186;
const byte TOK_DCOMPLEX = 187;
const byte TOK_FUNCTION_DEFS = 188;
const byte TOK_SCRIPT = 189;

const byte FLAG_DEBUG_STATEMENT = 1;
const byte FLAG_STEPTRAP = 2;

typedef byte tok;

class ParseException {
  unsigned m_pos;
  string m_text;
public:
  ParseException(unsigned pos = 0, string text = string()) : m_pos(pos), m_text(text) {}
  unsigned Position() {return m_pos;}
  string Text() {return m_text;}
};

class Token {
  byte m_tok;
  unsigned m_pos;
  string m_text;
  byte m_flags;
public:
  Token();
  Token(byte tok, unsigned pos = 0, string text = string());
  bool Is(byte tok) const {return m_tok == tok;}
  bool IsBinaryOperator() const;
  bool IsUnaryOperator() const;
  unsigned Precedence() const;
  bool IsRightAssociative() const;
  byte Value() const {return m_tok;}
  void SetValue(byte a) {m_tok = a;}
  unsigned Position()  const {return m_pos;}
  string Text()  const {return m_text;}
  void SetFlags(byte flags) {m_flags = flags;}
  byte Flags() const {return m_flags;}
  void Print(ostream& o) const;
};

string TokenToString(const Token& b);

ostream& operator<<(ostream& o, const Token& b);
#endif
