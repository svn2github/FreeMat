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

#include "wxCLI.hpp"
#include "Command.hpp"
#include "CLIThread.hpp"
#include "Array.hpp"
#include "App.hpp"
#include <iostream>

// ----------------------------------------------------------------------------
// wxCLI
// ----------------------------------------------------------------------------
/**
 * OK - here goes - how does this work?  For a given size, we can allocate
 * a buffer that contains a given number of scroll-back lines.  We make
 * this a circular buffer with a start and end pointer for the line number.
 * 
 */

IMPLEMENT_DYNAMIC_CLASS(wxCLI, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxCLI, wxScrolledWindow)
    EVT_SIZE(wxCLI::OnSize)
    EVT_CHAR(wxCLI::OnChar)
END_EVENT_TABLE()

  wxCLI::wxCLI( App* tMain, wxWindow *parent )
    : wxScrolledWindow( parent, -1,
			wxDefaultPosition, wxDefaultSize,
			wxSUNKEN_BORDER ) {
  mainApp = tMain;
  m_text = (wxChar *)NULL;
    
  m_head = 0;
  m_tail = 0;
  m_scrollback = 1000;

  SetBackgroundColour(*wxWHITE);

  m_font = wxFont(10, wxMODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

  m_xCaret = m_yCaret =
    m_xChars = m_yChars = 0;

  m_xMargin = m_yMargin = 0;

  CreateCaret();

  SetLineCount(50);
  linepos = 0;
  typeAheadTail = 0;
  typeAheadPtr = 0;
}

void wxCLI::warningMessage(const char * msg) {
  PutMessage(msg);
  PutMessage("\n");
}

void wxCLI::errorMessage(const char * msg) {
  PutMessage(msg);
  PutMessage("\n");
}

void wxCLI::outputMessage(const char * msg) {
  PutMessage(msg);
}

wxScrollWinEvent gScroll(wxEVT_SCROLLWIN_BOTTOM);

// Add a message to the output
void wxCLI::PutMessage(const char * msg) {
  const char *cp;

//   std::cout << "Got putmessage:" << msg << "\n";
//   std::cout.flush();
  cp = msg;
  while (*cp) {
    if (*cp == '\n') {
      m_tail++;
      linepos = 0;
    }
    else
      CharAt(linepos++,m_tail) = wxChar(*cp);
    cp++;
  }
  SetLineCount(m_tail+1);
  LastLine(); End();
  wxPostEvent(this,gScroll);
  Refresh();
  //  DoMoveCaret();
}

wxCLI::~wxCLI() {
  free(m_text);
}

void wxCLI::SetLineCount(int lineCount) {
  m_nLines = lineCount;
  SetScrollRate(0, m_hLine);
  SetVirtualSize(-1, (m_nLines + 1) * m_hLine);
}

void wxCLI::CreateCaret() {
  wxClientDC dc(this);
  dc.SetFont(m_font);
  m_heightChar = dc.GetCharHeight();
  m_widthChar = dc.GetCharWidth();
  m_hLine = m_heightChar;

  wxCaret *caret = new wxCaret(this, m_widthChar, m_heightChar);
  SetCaret(caret);

  caret->Move(m_xMargin, m_yMargin);
  caret->Show();
}

void wxCLI::MoveCaret(int x, int y) {
  m_xCaret = x;
  m_yCaret = y;

  //    DoMoveCaret();
}

void wxCLI::DoMoveCaret() {
  int screen_x, screen_y;
  
  CalcScrolledPosition(m_xMargin + m_xCaret * m_widthChar,
		       m_yMargin + m_yCaret * m_heightChar,
		       &screen_x, &screen_y);
  GetCaret()->Move(screen_x, screen_y);
}

void wxCLI::DoResizeBuffer(int xsize, int ysize) {
  if (m_text == NULL) {
    m_text = (wxChar *) calloc(xsize*ysize*m_scrollback,sizeof(wxChar));
    m_xChars = xsize;
    m_yChars = ysize;
    return;
  }
//   std::cout << "Got size event " << xsize << " x " << ysize << "\n";
//   std::cout.flush();
  if ((m_xChars == xsize) && (m_yChars == ysize)) return;
  wxChar *newBuf;

  newBuf = (wxChar *) calloc(xsize*ysize*m_scrollback,sizeof(wxChar));
  int copyx;
  copyx = xsize;
  if (copyx > m_xChars) copyx = m_xChars;
  for (int k=0;k<m_nLines;k++) {
    for (int l=0;l<copyx;l++)
      newBuf[k*xsize+l] = m_text[k*m_xChars+l];
  }
  free(m_text);
  m_text = newBuf;
  m_xChars = xsize;
  m_yChars = ysize;
}

void wxCLI::OnSize( wxSizeEvent &event ) {

  int m_xChars_new = (event.GetSize().x - 2*m_xMargin) / m_widthChar;
  int m_yChars_new = (event.GetSize().y - 2*m_yMargin) / m_heightChar;
  if ( !m_xChars_new )
    m_xChars_new = 1;
  if ( !m_yChars_new )
    m_yChars_new = 1;

  DoResizeBuffer(m_xChars_new,m_yChars_new);

  //    PutMessage("Ready\n");
  //     for (int k=0;k<50;k++)
  //       PutMessage("Hello!\n  Welcome to Grace Brothers!\n-->");

  event.Skip();
}

void wxCLI::SetFont(wxFont aFont) {
  m_font = aFont;
  CreateCaret();
}

void wxCLI::OnDraw(wxDC& dc) {
  wxCaretSuspend cs(this);
  dc.SetFont(m_font);
  // update region is always in device coords, translate to logical ones
  wxRect rectUpdate = GetUpdateRegion().GetBox();
  CalcUnscrolledPosition(rectUpdate.x, rectUpdate.y,
			 &rectUpdate.x, &rectUpdate.y);
  
  size_t lineFrom = rectUpdate.y / m_hLine,
    lineTo = rectUpdate.GetBottom() / m_hLine;
  
  if ( lineTo > m_nLines - 1)
    lineTo = m_nLines - 1;
  
  wxCoord y = lineFrom*m_hLine;
  for ( size_t line = lineFrom; line <= lineTo; line++ )  {
    wxCoord yPhys;
    CalcScrolledPosition(0, y, NULL, &yPhys);

    wxString oline;
    for (int x=0; x < m_xChars; x++) {
      wxChar ch = CharAt(x,line);
      if (!ch) break;
      oline += ch;
    }
    dc.DrawText(oline,0,y);
    //    dc.DrawText(wxString::Format(_T("Line %u (logical %d, physical %d)"),
    //				 line, y, yPhys), 0, y);
    y += m_hLine;
  }
  DoMoveCaret();
}

void wxCLI::NewLine() {
  typeAhead[typeAheadTail++] = '\n';
  typeAhead[typeAheadTail] = 0;
  linepos = 0;
  m_tail++;
//   std::cout << "Posting command :" << typeAhead << ":\n";
  FreeMat::PostGUIReply(new FreeMat::Command(FreeMat::CMD_GUIGetLineAcq,
					     FreeMat::Array::
					     stringConstructor(typeAhead)));
  typeAheadTail = 0;
  typeAheadPtr = 0;
}

void wxCLI::IssueGetWidthRequest() {
  FreeMat::PostGUIReply(new FreeMat::Command(FreeMat::CMD_GUIGetWidth,
					     FreeMat::Array::
					     int32Constructor(m_xChars)));
}

void wxCLI::IssueGetLineRequest(const char *prompt) {
  PutMessage(prompt);
  promptLength = strlen(prompt);
}

void wxCLI::Backspace() {
  int i;
  if (m_xCaret <= promptLength) return;
  /* Delete the character pointed to by the type-ahead buffer*/
  for (i=typeAheadPtr;i<typeAheadTail;i++) {
    typeAhead[i-1] = typeAhead[i];
  }
  typeAheadTail--;
  typeAheadPtr--;
  /* Change the contents of the screen buffer also */
  for (i=m_xCaret;i<m_xChars;i++)
    CharAt(i-1,m_yCaret) = CharAt(i,m_yCaret);
  CharAt(m_xChars-1,m_yCaret) = 0;
  m_xCaret--;
  /* Force a refresh */
  Refresh();
}

void wxCLI::DoKeyPress(wxChar ch) {
  LastLine();// End();
  if (typeAheadPtr < typeAheadTail) {
    int i;
    for (i=typeAheadTail;i>=typeAheadPtr;i--)
      typeAhead[i+1] = typeAhead[i];
    // Adjust the contents of the screen buffer
    for (i=m_xChars-1;i>=m_xCaret;i--)
      CharAt(i+1,m_yCaret) = CharAt(i,m_yCaret);
    typeAheadTail++;
    Refresh();
  }
  typeAhead[typeAheadPtr++] = (char) ch;
  if (typeAheadPtr > typeAheadTail)
    typeAheadTail = typeAheadPtr;
  CharAt(m_xCaret, m_yCaret) = ch;
  
  wxCaretSuspend cs(this);
  wxClientDC dc(this);
  dc.SetFont(m_font);
  dc.SetBackgroundMode(wxSOLID); // overwrite old value
  int scroll_x, scroll_y;
  CalcScrolledPosition(m_xMargin + m_xCaret * m_widthChar,
		       m_yMargin + m_yCaret * m_heightChar,
		       &scroll_x,&scroll_y);
  dc.DrawText(ch, scroll_x, scroll_y );
  NextChar();
}

void wxCLI::OnChar( wxKeyEvent &event ) {
  bool ctrlDown = event.ControlDown();

  switch ( event.KeyCode() )
    {
    case WXK_BACK:
      Backspace();
      break;
    case WXK_LEFT:
      if (m_xCaret > promptLength) {
	PrevChar();
	typeAheadPtr--;
      }
      break;
    case WXK_RIGHT:
      if (m_xCaret < m_xChars) {
	NextChar();
	typeAheadPtr++;
      }
      break;
    case WXK_UP:
      PrevLine();
      break;
    case WXK_DOWN:
      NextLine();
      break;
    case WXK_HOME:
      Home();
      break;
    case WXK_END:
      End();
      break;
    case WXK_RETURN:
      Home();
      //            NextLine();
      NewLine();
      break;
    default:
      if ( !event.AltDown() && wxIsprint(event.KeyCode()) )
	{
	  wxChar ch = (wxChar)event.KeyCode();
	  DoKeyPress(ch);
	}
      else
	{
	  event.Skip();
	}
    }
  DoMoveCaret();
}

