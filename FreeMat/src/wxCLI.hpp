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

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/caret.h"

class wxCLI : public wxScrolledWindow
{
public:
  wxCLI() {}
  wxCLI(wxWindow *parent);
  ~wxCLI();
  wxChar& CharAt(int x, int y) { return *(m_text + x + m_xChars * y); }
  
  void SetLineCount(int);

  // operations
  void CreateCaret();
  void MoveCaret(int x, int y);

  void outputMessage(const char *);
  void warningMessage(const char *);
  void errorMessage(const char *);
  void IssueGetLineRequest(const char*);
  void IssueGetWidthRequest();
  void NewLine();
  void DoKeyPress(wxChar ch);
  
  // caret movement
  void Backspace();
  void Home() { m_xCaret = 0; }
  void End() { m_xCaret = strlen(m_text+m_yCaret*m_xChars); }
  void FirstLine() { m_yCaret = 0; }
  void LastLine() { m_yCaret = m_nLines - 1; }
  void PrevChar() { m_xCaret--;}
  void NextChar() { m_xCaret++;}
  void PrevLine() { if ( !m_yCaret-- ) LastLine(); }
  void NextLine() { m_yCaret++;}

  // event handlers
  void OnSize( wxSizeEvent &event );
  void OnChar( wxKeyEvent &event );

  void PutMessage(const char *);
  virtual void OnDraw(wxDC& dc);
private:
  // move the caret to m_xCaret, m_yCaret
  void DoMoveCaret();
  void DoResizeBuffer(int xsize, int ysize);
  
  wxFont   m_font;
  
  // the margin around the text (looks nicer)
  int      m_xMargin, m_yMargin;
  
  // size (in pixels) of one character
  long     m_widthChar, m_heightChar;
  
  // position (in text coords) of the caret
  int      m_xCaret, m_yCaret;
  
  // the size (in text coords) of the window
  int      m_xChars, m_yChars;
  
  // the text
  wxChar  *m_text;

  // the head pointer
  int m_head, m_tail, m_scrollback;
  // the position on the line
  int linepos;
  
  // The height of one line
  wxCoord m_hLine;

  // The number of lines we draw
  size_t m_nLines;

  // The type-ahead buffer
  char typeAhead[1000];
  int typeAheadTail;
  int typeAheadPtr;
  int promptLength;
  
  DECLARE_DYNAMIC_CLASS(wxCLI)
  DECLARE_EVENT_TABLE()
};

  

#endif
