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

#ifndef __wxCLI_hpp__
#define __wxCLI_hpp__

#include "wx/wxprec.h"
#include <string>
#include <vector>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/caret.h"

class App;
class wxCLI : public wxScrolledWindow
{
public:
  wxCLI() {}
  wxCLI(App *tMain, wxWindow *parent);
  ~wxCLI();

  // Predefined control sequence
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void Home();
  void MoveBOL();
  // Raw output of a string
  void OutputRawString(std::string txt);
  // Output a character or string to the terminal after converting
  // tabs to spaces and control characters to a caret followed by
  // the modified character
  void OutputChar(char c, char pad);
  void OutputString(std::string string, char pad);
  // Delete nc characters starting from the one under the cursor.
  // Optionally copy the deleted characters to the cut buffer.
  void DeleteChars(int nc, int cut);
  // Add a character to the line buffer at the current cursor position,
  // inserting or overwriting according the current mode.
  void AddCharToLine(char c);
  // Insert/append a string to the line buffer and terminal at the
  // current cursor position
  void AddStringToLine(std::string s);
  // Delete the displayed part of the input line that follows the current
  // terminal cursor position.
  void TruncateDisplay();
  // Move the terminal cursor n positions to the left or right.
  void TerminalMoveCursor(int n);
  // Move the terminal cursor to a given position.
  void SetTermCurpos(int term_curpos);
  // Set the position of the cursor both in the line input buffer and on the
  // terminal.
  void PlaceCursor(int buff_curpos);
  // Return the terminal cursor position that corresponds to a given
  // line buffer cursor position.
  int BuffCurposToTermCurpos(int buff_curpos);
  // Return the number of terminal characters needed to display a
  // given raw character.
  int DisplayedCharWidth(char c, int term_curpos);
  // Return the number of terminal characters needed to display a
  // given substring.
  int DisplayedStringWidth(std::string s, int nc, int term_curpos);
  // Return non-zero if 'c' is to be considered part of a word.
  int IsWordChar(int c);
  // Display the prompt regardless of the current visibility mode.
  int DisplayPrompt();
  // Return the number of characters used by the prompt on the terminal.
  int DisplayedPromptWidth();
  void EndOfLine();
  void ReplacePrompt(std::string prmpt);
  // operations
  void CreateCaret();
  void MoveCaret(int x, int y);
  void outputMessage(const char *);
  void warningMessage(const char *);
  void errorMessage(const char *);
  void IssueGetLineRequest(const char*);
  void IssueGetWidthRequest();
  // event handlers
  void OnSize( wxSizeEvent &event );
  void OnChar( wxKeyEvent &event );
  void PutMessage(const char *);
  virtual void OnDraw(wxDC& dc);
  void SetFont(wxFont aFont);
  void CursorLeft();
  void CursorRight();
  void BeginningOfLine();
  void BackwardDeleteChar();
  void ForwardDeleteChar();
  void HistoryFindBackwards();
  void HistoryFindForwards();
  void SearchPrefix(const char*, int);
  void HistorySearchBackward();
  void HistorySearchForward();
  void Redisplay();
  void AddHistory(std::string);
  void KillLine();
  void Yank();
  wxChar& CharAt(int row, int column);
  int ScrollRowAdjust(int row);
private:
  // move the caret to m_xCaret, m_yCaret
  void DoMoveCaret();
  void DoResizeBuffer(int xsize, int ysize);
  void UpdateLineCount();
  App *mainApp;
  wxFont   m_font;
  // size (in pixels) of one character
  long charWidth, charHeight;
  // position (in text coords) of the caret
  int  caretCol, caretRow;
  // the size (in text coords) of the window
  int nline;
  int ncolumn;
  // the text
  std::vector<std::string> history;
  // The line buffer
  char line[1002];
  // The maximum allowed line length
  int linelen;
  // The cut buffer
  char cutbuf[1002];
  // The prompt
  std::string prompt;
  // length of the prompt string
  int prompt_len;
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
  // Number of scroll-back buffer lines
  int scrollback;
  // Which row of the scroll-buffer represents
  // the last line of producable text
  int scrolltail;
  // This indicates which row the top of the 
  // scroll buffer represents
  int nlinecount;
  wxChar *m_text;

  DECLARE_DYNAMIC_CLASS(wxCLI)

  DECLARE_EVENT_TABLE()

};

  

#endif
