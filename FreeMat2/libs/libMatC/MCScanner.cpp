#include "MCScanner.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Exception.hpp"

using namespace std;

extern string fm_reserved[];
extern int fm_reserved_count;

static bool isalnumus(byte a) {
  return (isalnum(a) || (a=='_'));
}

static bool isablank(byte a) {
  return (a==' ' || a=='\t' || a=='\r');
}

unsigned MCScanner::contextNum() {
  return (m_ptr << 16 | m_linenumber);
}

void MCScanner::setToken(byte tok, string text) {
  m_tok = Token(tok,m_ptr << 16 | m_linenumber,text);
}

bool MCScanner::done() {
  return (m_ptr >= (int)(m_text.size()));
}

bool MCScanner::peek(int chars, byte tok) {
  return (ahead(chars) == tok);
}

MCScanner::MCScanner(string buf, string fname) {
  m_text = buf;
  m_filename = fname;
  m_ptr = 0;
  m_linenumber = 1;
  m_tokValid = false;
  m_bracketDepth = 0;
  m_strlen = buf.size();
}


void MCScanner::fetch() {
  if (m_ptr >= m_strlen)
    setToken(TOK_EOF);
  else if (current() == '%') {
    fetchComment();
    return;
  } else if (isalpha(current()))
    fetchIdentifier();
  else if (isdigit(current()) || ((current() == '.') && isdigit(ahead(1))))
    fetchNumber();
  else if (isablank(current())) {
    fetchWhitespace();
    return;
  } else if ((current() == '\'') && !((previous() == '\'') ||
				      (previous() == ')') ||
				      (previous() == ']') ||
				      (previous() == '}') ||
				      (isalnumus(previous())))) {
    fetchString();
  } else
    fetchOther();
  m_tokValid = true;
}

bool MCScanner::tryFetchBinary(const char* op, byte tok) {
  if ((current() == op[0]) && (ahead(1) == op[1])) {
    setToken(tok);
    m_ptr += 2;
    return true;
  }
  return false;
}

void MCScanner::fetchComment() {
  while ((current() != '\n') && (m_ptr < m_strlen))
    m_ptr++;
}

void MCScanner::fetchOther() {
  if (current() == '.') {
    if (tryFetchBinary(".*",TOK_DOTTIMES)) return;
    if (tryFetchBinary("./",TOK_DOTRDIV)) return;
    if (tryFetchBinary(".\\",TOK_DOTLDIV)) return;
    if (tryFetchBinary(".^",TOK_DOTPOWER)) return;
    if (tryFetchBinary(".'",TOK_DOTTRANSPOSE)) return;
  }
  if (tryFetchBinary("<=",TOK_LE)) return;
  if (tryFetchBinary(">=",TOK_GE)) return;
  if (tryFetchBinary("==",TOK_EQ)) return;
  if (tryFetchBinary("~=",TOK_NE)) return;
  if (tryFetchBinary("&&",TOK_SAND)) return;
  if (tryFetchBinary("||",TOK_SOR)) return;
  setToken(m_text[m_ptr]);
  if (m_text[m_ptr] == '[')
    m_bracketDepth++;
  if (m_text[m_ptr] == ']')
    m_bracketDepth = min(0,m_bracketDepth-1);
  if (m_text[m_ptr] == '{')
    m_bracketDepth++;
  if (m_text[m_ptr] == '}')
    m_bracketDepth = min(0,m_bracketDepth-1);
  m_ptr++;
}

void MCScanner::fetchString() {
  int len = 0;
  // We want to advance, but skip double quotes
  //  while ((next() != ') || ((next() == ') && (next(2) == ')) && (next() != '\n')
  while (((ahead(len+1) != '\'') ||
	  ((ahead(len+1) == '\'') && (ahead(len+2) == '\''))) &&
	 (ahead(len+1) != '\n')) {
    if ((ahead(len+1) == '\'') &&
	(ahead(len+2) == '\'')) len+=2;
    else
      len++;
  }
  if (ahead(len+1) == '\n')
    throw Exception("unterminated string" + context());
  string ret(m_text,m_ptr+1,len);
  string::size_type ndx = ret.find("''");
  while (ndx != string::npos) {
    ret.erase(ndx,1);
    ndx = ret.find("''");
  }
  setToken(TOK_STRING,ret);
  m_ptr += len+2;
}

void MCScanner::fetchWhitespace() {
  int len = 0;
  while (isablank(ahead(len))) len++;
  setToken(TOK_SPACE);
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

void MCScanner::fetchNumber() {
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
  if (((ahead(lookahead-1) == '.') &&
       (ahead(lookahead) == '.') &&
       (ahead(lookahead+1) == '.') &&
       (ahead(lookahead+2) != '.')) ||
      ((ahead(lookahead-1) == '.') &&
       ((ahead(lookahead) == '*') ||
	(ahead(lookahead) == '/') ||
	(ahead(lookahead) == '\\') ||
	(ahead(lookahead) == '^') ||
	(ahead(lookahead) == '\'')))) lookahead--;
  string numtext(string(m_text,m_ptr,lookahead));
  m_ptr += lookahead;
  if ((numclass == complex_class) ||
      (numclass == dcomplex_class))
    m_ptr++;
  switch (numclass) {
  case integer_class:
    setToken(TOK_DOUBLE,numtext);
    return;
  case float_class:
    setToken(TOK_FLOAT,numtext);
    return;
  case double_class:
    setToken(TOK_DOUBLE,numtext);
    return;
  case complex_class:
    setToken(TOK_COMPLEX,numtext);
    return;
  case dcomplex_class:
    setToken(TOK_DCOMPLEX,numtext);
    return;
  }
}

void MCScanner::fetchIdentifier() {
  int len = 0;
  while (isalnumus(ahead(len))) len++;
  // Collect the identifier into a string
  string ident(string(m_text,m_ptr,len));
  string *p = lower_bound(fm_reserved,fm_reserved+fm_reserved_count,ident);
  if ((p!= fm_reserved+fm_reserved_count) && (*p == ident))
    setToken(TOK_KEYWORD+(p-fm_reserved)+1);
  else
    setToken(TOK_IDENT,string(m_text,m_ptr,len));
  m_ptr += len;
}

// A Blob is either:
//   1.  A regular string (with quote delimiters)
//   2.  A sequence of characters with either a whitespace
//       a comma or a colon.
void MCScanner::fetchBlob() {
  if (current() == '\'') {
    fetchString();
    m_tokValid = true;
  } else {
    int len = 0;
    while ((ahead(len) != '\n') && (!isablank(ahead(len))) && 
	   (ahead(len) != '%') && (ahead(len) != ',') &&
	   (ahead(len) != ';')) len++;
    if (len > 0) {
      setToken(TOK_STRING,string(m_text,m_ptr,len));
      m_ptr += len;
      m_tokValid = true;    
    } 
  }
}

const Token& MCScanner::next() {
  while (!m_tokValid) {
    fetch();
    if ((m_ptr < m_strlen) && (current() == '\n'))
      m_linenumber++;
  }
  return m_tok;
}

void MCScanner::consume() {
  m_tokValid = false;
}

byte MCScanner::current() {
  if (m_ptr < m_strlen)
    return m_text.at(m_ptr);
  else
    return 0;
}

byte MCScanner::previous() {
  if (m_ptr)
    return m_text.at(m_ptr-1);
  else
    return 0;
}

byte MCScanner::ahead(int n) {
  if ((m_ptr+n) >= (int)(m_text.size()))
    return 0;
  else
    return m_text.at(m_ptr+n);
}

string MCScanner::context() {
  return context(contextNum());
}

static string stringFromNumber(unsigned line) {
  char buffer[1000];
  sprintf(buffer,"%d",line);
  return string(buffer);
}

string MCScanner::context(unsigned pos) {
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
