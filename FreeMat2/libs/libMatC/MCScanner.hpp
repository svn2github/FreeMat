#ifndef __MCScanner_hpp__
#define __MCScanner_hpp__

#include <string>
#include "Token.hpp"

class MCScanner {
  std::string m_filename;
  std::string m_text;
  int m_ptr;
  int m_strlen;
  int m_linenumber;
  Token m_tok;
  bool m_tokValid;
  int  m_bracketDepth;
  byte current();
  byte previous();
  byte ahead(int n);
  void fetch();
  void fetchWhitespace();
  void fetchIdentifier();
  void fetchComment();
  void fetchContinuation();
  void fetchNumber();
  void fetchString();
  void fetchBlob();
  void fetchOther();
  bool tryFetchBinary(const char* op, byte tok);
  void setToken(byte tok, string text = string());
public:
  MCScanner(string buf, string fname);
  // Methods accessed by the parser
  const Token& next();
  void consume();
  bool match(byte tok);
  bool done();
  bool peek(int chars, byte tok);
  unsigned position() {return m_ptr;}
  unsigned contextNum();
  std::string context(unsigned pos);
  std::string context();
};

#endif
