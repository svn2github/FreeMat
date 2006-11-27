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
const byte TOK_DBTRACE = 140;
const byte TOK_ELSE = 141;
const byte TOK_ELSEIF = 142;
const byte TOK_END = 143;
const byte TOK_FOR = 144;
const byte TOK_FUNCTION = 145;
const byte TOK_GLOBAL = 146;
const byte TOK_IF = 147;
const byte TOK_KEYBOARD = 148;
const byte TOK_OTHERWISE = 149;
const byte TOK_PERSISTENT = 150;
const byte TOK_QUIT = 151;
const byte TOK_RETALL = 152;
const byte TOK_RETURN = 153;
const byte TOK_SWITCH = 154;
const byte TOK_TRY = 155;
const byte TOK_WHILE = 156;
// Generated (synthetic) token;
const byte TOK_MULTI = 157;
const byte TOK_SPECIAL = 158;
const byte TOK_VARIABLE = 159;
const byte TOK_DYN = 160;
const byte TOK_BLOCK = 161;
const byte TOK_EOF = 162;
const byte TOK_MATDEF = 163;
const byte TOK_CELLDEF = 164;
const byte TOK_PARENS = 165;
const byte TOK_BRACES = 166;
const byte TOK_BRACKETS = 167;
const byte TOK_ROWDEF = 168;
const byte TOK_UNARY_MINUS = 169;
const byte TOK_UNARY_PLUS = 170;
const byte TOK_EXPR = 171;
const byte TOK_DOTTIMES = 172;
const byte TOK_DOTRDIV = 173;
const byte TOK_DOTLDIV = 174;
const byte TOK_DOTPOWER = 175;
const byte TOK_DOTTRANSPOSE = 176;
const byte TOK_LE = 177;
const byte TOK_GE = 178;
const byte TOK_EQ = 179;
const byte TOK_NE = 180;
const byte TOK_SOR = 181;
const byte TOK_SAND = 182;
const byte TOK_QSTATEMENT = 183;
const byte TOK_STATEMENT = 184;
const byte TOK_INTEGER = 185;
const byte TOK_FLOAT = 186;
const byte TOK_DOUBLE = 187;
const byte TOK_COMPLEX = 188;
const byte TOK_DCOMPLEX = 189;
const byte TOK_FUNCTION_DEFS = 190;
const byte TOK_SCRIPT = 191;
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
