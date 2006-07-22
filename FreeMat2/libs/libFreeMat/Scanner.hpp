#ifndef __Scanner_hpp__
#define __Scanner_hpp__

#include <string>
#include "Token.hpp"
#include <stack>
#include <vector>

using namespace std;

class Scanner {
  string m_filename;
  string m_text;
  int m_ptr, m_ptr_save;
  int m_strlen;
  int m_linenumber;
  stack<bool> m_ignorews;
  vector<int> m_breakpoints;
  vector<int> m_current_breakpoints;
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
  bool isBreakpointLine(int num);
  void deleteBreakpoint(int num);
public:
  Scanner(string buf, string fname);
  // Methods accessed by the parser
  const Token& Next();
  void Consume();
  void Gobble();
  bool Match(byte tok);
  void SetDebug(bool debugFlag) {m_debugFlag = debugFlag;}
  void PushWSFlag(bool ignoreWS);
  void PopWSFlag();
  bool Done();
  bool Peek(int chars, byte tok);
  void SetBreakpoints(vector<int> bp);
  unsigned Position() {return m_ptr;}
  unsigned ContextNum();
  string Context(unsigned pos);
  string Context();
};

#endif
