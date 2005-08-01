// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#ifndef __KeyManager_hpp__
#define __KeyManager_hpp__

#include <string>
#include <vector>
#include <list>
#include "Interface.hpp"
#include <qeventloop.h>

#define KM_CTRLA     0x01
#define KM_CTRLC     0x03
#define KM_CTRLD     0x04
#define KM_CTRLE     0x05
#define KM_TAB       0x09
#define KM_CTRLK     0x0b
#define KM_CTRLY     0x19
#define KM_BACKSPACE 0x08
#define KM_BACKSPACEALT 0x7f
#define KM_ESC       0x1b
#define KM_NEWLINE   0x0d
#define KM_LEFT      0x101
#define KM_RIGHT     0x102
#define KM_UP        0x103
#define KM_DOWN      0x104
#define KM_DELETE    0x108
#define KM_INSERT    0x105
#define KM_HOME      0x106
#define KM_END       0x107

using namespace FreeMat;

class KeyManager : public Interface
{
  std::list<std::string> enteredLines;  
  bool enteredLinesEmpty;  
public:
  KeyManager();
  virtual ~KeyManager();

  // Predefined control sequence
  virtual void MoveDown() = 0;
  virtual void MoveUp() = 0;
  virtual void MoveRight() = 0;
  virtual void MoveLeft() = 0;
  virtual void ClearEOL() = 0;
  virtual void ClearEOD() = 0;
  virtual void MoveBOL() = 0;
  virtual void OutputRawString(std::string txt) = 0;

  virtual void ExecuteLine(std::string line);
  virtual char* getLine(std::string aprompt);
  virtual void outputMessage(std::string msg);
  virtual void errorMessage(std::string msg);
  virtual void warningMessage(std::string msg);

  virtual void RestoreOriginalMode() {};
  virtual void SetRawMode() {};
  virtual void Initialize() {};
  virtual void ResizeEvent() {};
  virtual void RegisterInterrupt();

  void Redisplay();
  void setTerminalWidth(int w);
  void AddCharToLine(char c);
  void ReplacePrompt(std::string prmt);
  int DisplayPrompt();
  void OnChar( int c );
  void OutputChar(char c, char pad);
  void OutputString(std::string msg, char c);
  void TerminalMove(int n);
  void SetTermCurpos(int n);
  void PlaceCursor(int n);
  int DisplayedCharWidth(char c, int aterm_curpos);
  int DisplayedStringWidth(std::string s, int nc, int offset);
  int BuffCurposToTermCurpos(int n);
  void DeleteChars(int nc, int cut);
  void TruncateDisplay();
  void SetChar(unsigned int p, char c);
  void EndOfLine();
  void KillLine();
 private:
  void CursorLeft();
  void CursorRight();
  void BeginningOfLine();
  void BackwardDeleteChar();
  void ForwardDeleteChar();

  void HistorySearchBackward();
  void HistorySearchForward();
  void AddHistory(std::string line);
  void HistoryFindForwards();
  void HistoryFindBackwards();
  void AddStringToLine(std::string s);
  void SearchPrefix(std::string aline, int alen);
  void Yank();
  void ListCompletions(std::vector<std::string> completions);
  void CompleteWord();
 protected:
  void NewLine();
  void ResetLineBuffer();
  // the size (in text coords) of the window
  int nline;
  int ncolumn;
  // the text
  std::vector<std::string> history;
  // The line buffer
  std::string line;
  // The maximum allowed line length
  int linelen;
  // The cut buffer
  std::string cutbuf;
  // number of characters in line
  int ntotal;
  // Current position in the buffer
  int buff_curpos;
  // Current position on the terminal
  int term_curpos;
  // Number of characters used to display the current input line
  int term_len;
  // Marker location in the buffer
  int buff_mark;
  // Cursor position at start of insert
  int insert_curpos;
  int keyseq_count;
  int last_search;
  std::string prefix;
  int prefix_len;
  // True in insert mode
  bool insert;
  int startsearch;
  // The prompt
  std::string prompt;
  // length of the prompt string
  int prompt_len;
  // The event loops used by keymanager
  QEventLoop *m_loop;
};

  

#endif
