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

#define KM_CTRLA     0x01
#define KM_CTRLD     0x04
#define KM_CTRLE     0x05
#define KM_TAB       0x09
#define KM_CTRLK     0x0b
#define KM_CTRLY     0x19
#define KM_BACKSPACE 0x7f
#define KM_ESC       0x1b
#define KM_NEWLINE   0x0d
#define KM_LEFT      0x101
#define KM_RIGHT     0x102
#define KM_UP        0x103
#define KM_DOWN      0x104
#define KM_DELETE    0x80
#define KM_INSERT    0x105
#define KM_HOME      0x106
#define KM_END       0x107
class KeyManager
{
public:
  KeyManager();
  ~KeyManager();

  // Predefined control sequence
  virtual void MoveDown() = 0;
  virtual void MoveUp() = 0;
  virtual void MoveRight() = 0;
  virtual void MoveLeft() = 0;
  virtual void ClearEOL() = 0;
  virtual void ClearEOD() = 0;
  virtual void MoveBOL() = 0;
  virtual void OutputRawString(std::string txt) = 0;

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
  // event handlers
  //  void OnSize( wxSizeEvent &event );
  void OnChar( int c );
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
  virtual std::vector<std::string> GetCompletions(const char *line, int word_end,
						  std::string &matchString) {};
  void CompleteWord();
  int ScrollRowAdjust(int row);
  virtual void ExecuteLine(const char * line) {};
protected:
  // move the caret to m_xCaret, m_yCaret
  void DoResizeBuffer(int xsize, int ysize);
  void ListCompletions(std::vector<std::string> completions);
  void NewLine();
  void ResetLineBuffer();

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
};

  

#endif
