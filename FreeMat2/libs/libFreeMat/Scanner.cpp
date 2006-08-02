#include "Scanner.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern string fm_reserved[21];


bool isalnumus(byte a) {
  return (isalnum(a) || (a=='_'));
}

bool isblank(byte a) {
  return (a==' ' || a=='\t');
}

unsigned Scanner::ContextNum() {
  return (m_ptr << 16 | m_linenumber);
}

void Scanner::SetToken(byte tok, string text) {
  m_tok = Token(tok,m_ptr << 16 | m_linenumber,text);
}

bool Scanner::Done() {
  return (m_ptr >= m_text.size());
}

bool Scanner::Peek(int chars, byte tok) {
  return (ahead(chars) == tok);
}

Scanner::Scanner(string buf, string fname) {
  m_text = buf;
  m_filename = fname;
  m_ptr = 0;
  m_ptr_save = 0;
  m_linenumber = 1;
  m_tokValid = false;
  m_strlen = buf.size();
  m_ignorews.push(true);
  m_prevws = false;
  m_debugFlag = false;
}

void Scanner::FetchContinuation() {
  m_ptr += 3;
  while ((current() != '\n') && (m_ptr < m_strlen))
    m_ptr++;
  if (current() == '\n') {
    m_linenumber++;
    m_ptr++;
  }
}

void Scanner::Fetch() {
  if (m_ptr >= m_strlen)
    SetToken(TOK_EOF);
  else if (current() == '%') {
    FetchComment();
    return;
  } else if ((current() == '.') && 
	     (ahead(1) == '.') &&
	     (ahead(2) == '.')) {
    FetchContinuation();
    return;
  } else if (isalpha(current()))
    FetchIdentifier();
  else if (isdigit(current()) || ((current() == '.') && isdigit(ahead(1))))
    FetchNumber();
  else if (isblank(current())) {
    FetchWhitespace();
    if (m_ignorews.top()) return;
  } else if ((current() == '\'') && !((previous() == '\'') ||
				      (previous() == ')') ||
				      (previous() == ']') ||
				      (previous() == '}') ||
				      (isalnumus(previous())))) {
    FetchString();
  } else
    FetchOther();
  m_tokValid = true;
}

bool Scanner::TryFetchBinary(const char* op, byte tok) {
  if ((current() == op[0]) && (ahead(1) == op[1])) {
    SetToken(tok);
    m_ptr += 2;
    return true;
  }
  return false;
}

void Scanner::FetchComment() {
  while ((current() != '\n') && (m_ptr < m_strlen))
    m_ptr++;
}

void Scanner::FetchOther() {
  if (current() == '.') {
    if (TryFetchBinary(".*",TOK_DOTTIMES)) return;
    if (TryFetchBinary("./",TOK_DOTRDIV)) return;
    if (TryFetchBinary(".\\",TOK_DOTLDIV)) return;
    if (TryFetchBinary(".^",TOK_DOTPOWER)) return;
    if (TryFetchBinary(".'",TOK_DOTTRANSPOSE)) return;
  }
  if (TryFetchBinary("<=",TOK_LE)) return;
  if (TryFetchBinary(">=",TOK_GE)) return;
  if (TryFetchBinary("==",TOK_EQ)) return;
  if (TryFetchBinary("~=",TOK_NE)) return;
  if (TryFetchBinary("&&",TOK_SAND)) return;
  if (TryFetchBinary("||",TOK_SOR)) return;
  SetToken(m_text[m_ptr]);
  m_ptr++;
}

void Scanner::FetchString() {
  int len = 0;
  while (!(((ahead(len+1) == '\'') &&
	    (ahead(len+2) != '\'')) || 
	   (ahead(len+1) == '\n'))) {
    if ((ahead(len+1) == '\'') &&
	(ahead(len+2) == '\'')) len+=2;
    else
      len++;
  }
  if (ahead(len+1) == '\n')
    throw ParseException(m_ptr,"unterminated string");
  string ret(m_text,m_ptr+1,len);
  string::size_type ndx = ret.find("''");
  while (ndx != string::npos) {
    ret.erase(ndx,1);
    ndx = ret.find("''");
  }
  SetToken(TOK_STRING,ret);
  m_ptr += len+2;
}

void Scanner::FetchWhitespace() {
  int len = 0;
  //   while (isblank(ahead(len)) ||
  // 	 (ahead(len) == '\r') || 
  // 	 (ahead(len) == '\n')) len++;
  while (isblank(ahead(len))) len++;
  SetToken(TOK_SPACE);
  m_ptr += len;
}


//A number consists of something like:
//{integer}.{integer}E{sign}{integer}
//   s1   s2   s3   s4  s5    s6
// .{integer}E{sign}{integer}
//
// <Float><Exponent>
// <Float>
// <Integer>
//
// flags - int, float, double, complex
//
typedef enum {
  integer_class,
  float_class,
  double_class,
  complex_class,
  dcomplex_class
} number_class;

void Scanner::FetchNumber() {
  int len = 0;
  int lookahead = 0;
  number_class numclass;

  numclass = integer_class;
  while (isdigit(ahead(len))) len++;
  lookahead = len;
  if (ahead(lookahead) == '.') {
    numclass = double_class;
    lookahead++;
    len = 0;
    while (isdigit(ahead(len+lookahead))) len++;
    lookahead+=len;
  }
  if ((ahead(lookahead) == 'E') ||
      (ahead(lookahead) == 'e')) {
    numclass = double_class;
    lookahead++;
    if ((ahead(lookahead) == '+') ||
	(ahead(lookahead) == '-')) {
      lookahead++;
    }
    len = 0;
    while (isdigit(ahead(len+lookahead))) len++;
    lookahead+=len;
  }
  if ((ahead(lookahead) == 'f') ||
      (ahead(lookahead) == 'F')) {
    numclass = float_class;
    lookahead++;
  }
  if ((ahead(lookahead) == 'd') ||
      (ahead(lookahead) == 'D')) {
    numclass = double_class;
    lookahead++;
  }
  // Recognize the complex constants, but strip the "i" off
  if ((ahead(lookahead) == 'i') ||
      (ahead(lookahead) == 'I') ||
      (ahead(lookahead) == 'j') ||
      (ahead(lookahead) == 'J')) {
    numclass = (numclass == float_class) ? complex_class : dcomplex_class;
  }
  // Back off if we aggregated a "." from "..." into the number
  if ((ahead(lookahead-1) == '.') &&
      (ahead(lookahead) == '.') &&
      (ahead(lookahead+1) == '.') &&
      (ahead(lookahead+2) != '.')) lookahead--;
  string numtext(string(m_text,m_ptr,lookahead));
  m_ptr += lookahead;
  if ((numclass == complex_class) ||
      (numclass == dcomplex_class))
    m_ptr++;
  switch (numclass) {
  case integer_class:
    SetToken(TOK_INTEGER,numtext);
    return;
  case float_class:
    SetToken(TOK_FLOAT,numtext);
    return;
  case double_class:
    SetToken(TOK_DOUBLE,numtext);
    return;
  case complex_class:
    SetToken(TOK_COMPLEX,numtext);
    return;
  case dcomplex_class:
    SetToken(TOK_DCOMPLEX,numtext);
    return;
  }
}

void Scanner::FetchIdentifier() {
  int len = 0;
  while (isalnumus(ahead(len))) len++;
  // Collect the identifier into a string
  string ident(string(m_text,m_ptr,len));
  string *p = lower_bound(fm_reserved,fm_reserved+21,ident);
  if ((p!= fm_reserved+21) && (*p == ident))
    SetToken(TOK_KEYWORD+(p-fm_reserved)+1);
  else
    SetToken(TOK_IDENT,string(m_text,m_ptr,len));
  m_ptr += len;
}

void Scanner::Gobble() {
  int len = 0;
  while (isblank(current())) m_ptr++;
  if (current() == '\'')
    FetchString();
  else {
    while ((ahead(len) != '\n') && (ahead(len) != '%')) len++;
    SetToken(TOK_STRING,string(m_text,m_ptr,len));
    m_ptr += len;
  }
  m_tokValid = true;
}


const Token& Scanner::Next() {
  while (!m_tokValid) {
    m_ptr_save = m_ptr;
    Fetch();
    if (m_tokValid && m_debugFlag)
      cout << m_tok;
    if ((m_ptr < m_strlen) && (current() == '\n'))
      m_linenumber++;
  }
  return m_tok;
}

void Scanner::Consume() {
  m_tokValid = false;
}

byte Scanner::current() {
  return m_text.at(m_ptr);
}

byte Scanner::previous() {
  if (m_ptr)
    return m_text.at(m_ptr-1);
  else
    return 0;
}

void Scanner::PushWSFlag(bool ignoreWS) {
  m_ignorews.push(ignoreWS);
}

void Scanner::PopWSFlag() {
  m_ignorews.pop();
}

byte Scanner::ahead(int n) {
  if ((m_ptr+n) >= m_text.size()) 
    return 0;
  else
    return m_text.at(m_ptr+n);
}

string Scanner::Context() {
  return Context(m_ptr);
}

string stringFromNumber(unsigned line) {
  char buffer[1000];
  sprintf(buffer,"%d",line);
  return string(buffer);
}

string Scanner::Context(unsigned pos) {
  pos = pos >> 16;
  string::size_type line_start = 0;
  int linenumber = 1;
  string::size_type line_stop = m_text.find("\n");
  string prevline;
  while (pos > line_stop) {
    prevline = string(m_text,line_start,line_stop-line_start);
    line_start = line_stop+1;
    line_stop = m_text.find("\n",line_start);
    linenumber++;
  }
  string retstring;
  if (m_filename.size() > 0) {
    retstring = " at line number: " + stringFromNumber(linenumber);
    retstring += " of file " + m_filename + "\n";
  }  else
    retstring += "\n";
  retstring += "     " + prevline + "\n";
  retstring += "     " + string(m_text,line_start,line_stop-line_start);
  int offset = pos-line_start-1;
  if (offset < 0) offset = 0;
  retstring += "\n     " + string(offset,' ') + "^";
  return(retstring);
}
