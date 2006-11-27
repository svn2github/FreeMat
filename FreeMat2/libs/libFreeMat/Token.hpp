#ifndef __Token_hpp__
#define __Token_hpp__

#include <string>
#include "Array.hpp"

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
const byte TOK_DBSTEP = 139;
const byte TOK_ELSE = 140;
const byte TOK_ELSEIF = 141;
const byte TOK_END = 142;
const byte TOK_FOR = 143;
const byte TOK_FUNCTION = 144;
const byte TOK_GLOBAL = 145;
const byte TOK_IF = 146;
const byte TOK_KEYBOARD = 147;
const byte TOK_OTHERWISE = 148;
const byte TOK_PERSISTENT = 149;
const byte TOK_QUIT = 150;
const byte TOK_RETALL = 151;
const byte TOK_RETURN = 152;
const byte TOK_SWITCH = 153;
const byte TOK_TRY = 154;
const byte TOK_WHILE = 155;
// Generated (synthetic) token;
const byte TOK_MULTI = 156;
const byte TOK_SPECIAL = 157;
const byte TOK_VARIABLE = 158;
const byte TOK_DYN = 159;
const byte TOK_BLOCK = 160;
const byte TOK_EOF = 161;
const byte TOK_MATDEF = 162;
const byte TOK_CELLDEF = 163;
const byte TOK_PARENS = 164;
const byte TOK_BRACES = 165;
const byte TOK_BRACKETS = 166;
const byte TOK_ROWDEF = 167;
const byte TOK_UNARY_MINUS = 168;
const byte TOK_UNARY_PLUS = 169;
const byte TOK_EXPR = 170;
const byte TOK_DOTTIMES = 171;
const byte TOK_DOTRDIV = 172;
const byte TOK_DOTLDIV = 173;
const byte TOK_DOTPOWER = 174;
const byte TOK_DOTTRANSPOSE = 175;
const byte TOK_LE = 176;
const byte TOK_GE = 177;
const byte TOK_EQ = 178;
const byte TOK_NE = 179;
const byte TOK_SOR = 180;
const byte TOK_SAND = 181;
const byte TOK_QSTATEMENT = 182;
const byte TOK_STATEMENT = 183;
const byte TOK_INTEGER = 184;
const byte TOK_FLOAT = 185;
const byte TOK_DOUBLE = 186;
const byte TOK_COMPLEX = 187;
const byte TOK_DCOMPLEX = 188;
const byte TOK_FUNCTION_DEFS = 189;
const byte TOK_SCRIPT = 190;
const byte TOK_DBTRACE = 191;
typedef byte tok;
class Serialize;

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
  bool m_bpflag;
  Array m_array;
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
  Array GetArray() const {return m_array;}
  void FillArray();
  void SetBPFlag(bool flags) {m_bpflag = flags;}
  bool BPFlag() const {return m_bpflag;}
  void Print(ostream& o) const;

  friend void FreezeToken(const Token& a, Serialize *s);
  friend Token ThawToken(Serialize *s);
};

string TokenToString(const Token& b);

ostream& operator<<(ostream& o, const Token& b);

void FreezeToken(const Token& a, Serialize *s);

Token ThawToken(Serialize *s);

#endif
