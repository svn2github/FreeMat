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
  int m_ptr;
  int m_strlen;
  int m_linenumber;
  stack<bool> m_ignorews;
  Token m_tok;
  bool m_tokValid;
  bool m_debugFlag;
  bool m_inContinuationState;
  int  m_bracketDepth;
  bool m_blobFlag;
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
  void FetchBlob();
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
  bool Match(byte tok);
  void SetDebug(bool debugFlag) {m_debugFlag = debugFlag;}
  // Warning: Ugly Hack.  When in Special Call mode, the
  // rules for what constitutes a string change completely.
  void SetBlobMode(bool blobFlag) {m_blobFlag = blobFlag;}
  void PushWSFlag(bool ignoreWS);
  void PopWSFlag();
  bool Done();
  bool Peek(int chars, byte tok);
  void SetBreakpoints(vector<int> bp);
  unsigned Position() {return m_ptr;}
  unsigned ContextNum();
  string Context(unsigned pos);
  string Context();
  bool InContinuationState();
  bool InBracket();
};

#endif
