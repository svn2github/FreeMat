/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __KeyManager_hpp__
#define __KeyManager_hpp__

#include <string>
#include <vector>
#include <list>
#include "Interface.hpp"
#include <QEventLoop>

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
  Q_OBJECT
public:
  KeyManager();
  virtual ~KeyManager();

  virtual void ExecuteLine(std::string line);
  virtual char* getLine(std::string aprompt);
  virtual void outputMessage(std::string msg);
  virtual void errorMessage(std::string msg);
  virtual void warningMessage(std::string msg);

  void Redisplay();
  void setTerminalWidth(int w);
  void AddCharToLine(char c);
  void ReplacePrompt(std::string prmt);
  int DisplayPrompt();
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
  void EndOfLine();
  void KillLine();
  int getTerminalWidth();
  void RegisterTerm(QObject* term);
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
  void RegisterInterrupt();
 protected:
  void EraseCharacters(int pos, int count);
  void InsertCharacter(int pos, char c);
  void SetCharacter(int pos, char c);
  void InsertString(int pos, std::string s);
  
  void NewLine();
  void ResetLineBuffer();
  std::list<std::string> enteredLines;  
  bool enteredLinesEmpty;  
  // the size (in text coords) of the window
  int nline;
  int ncolumn;
  // the text
  std::vector<std::string> history;
  // The new line buffer
  char *lineData;
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
  // Are we waiting for input?
  int loopactive;
  QEventLoop *m_loop;
signals:
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  void OutputRawString(std::string txt);
  void SendCommand(QString);
  void Interrupt();
protected slots:
  void OnChar( int c );
  void SetTermWidth(int w);
  void QueueString(QString);
  void QueueCommand(QString);
};

  

#endif
