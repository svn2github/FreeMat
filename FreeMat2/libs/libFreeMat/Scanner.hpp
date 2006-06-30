#ifndef __Scanner_hpp__
#define __Scanner_hpp__

#include <string>
#include "Token.hpp"
#include <stack>

using namespace std;

class ScannerState {
public:
  int m_strlen;
  int m_ignorews_len;
  ScannerState(int strlen, int ignorews) {
    m_strlen = strlen;
    m_ignorews_len = ignorews;
  }
};

class Scanner {
  string m_text;
  int m_ptr, m_ptr_save;
  int m_strlen;
  stack<ScannerState> m_checkpoints;
  stack<bool> m_ignorews;
  Token m_tok;
  bool m_tokValid;
  bool m_debugFlag;
  bool m_prevws;
  byte current();
  byte previous();
  byte ahead(int n);
  void Fetch();
  void FetchWhitespace();
  void FetchIdentifier();
  void FetchComment();
  void FetchContinuation();
  void FetchNumber();
  void FetchString();
  void FetchOther();
  bool TryFetchBinary(const char* op, byte tok);
  void SetToken(byte tok, string text = string());
public:
  Scanner(string buf);
  // Methods accessed by the parser
  const Token& Next();
  void Consume();
  bool Match(byte tok);
  void SetDebug(bool debugFlag) {m_debugFlag = debugFlag;}
  void PushWSFlag(bool ignoreWS);
  void PopWSFlag();
  void Save();
  void Restore();
  void Continue();
  bool Done();
  bool Peek(int chars, byte tok);
  unsigned Position() {return m_ptr;}
  string Context(unsigned pos);
};

#endif
