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

#include "wxCLI2.hpp"
#include "Command.hpp"
#include "CLIThread.hpp"
#include "Array.hpp"
#include "App.hpp"
#include <iostream>

#define TAB_WIDTH 8
/*
 * The following macro returns non-zero if a character is
 * a control character.
 */
#define IS_CTRL_CHAR(c) ((unsigned char)(c) < ' ' || (unsigned char)(c)=='\177')
/*
 * Given a binary control character, return the character that
 * had to be pressed at the same time as the control key.
 */
#define CTRL_TO_CHAR(c) (toupper((unsigned char)(c) | 0x40))

#define GL_WORD_CHARS "_*\?\\[]"

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
  SetBackgroundColour(*wxWHITE);
  m_font = wxFont(10, wxMODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  caretCol = caretRow = 0;
  CreateCaret();
  cutbuf[0] = '\0';
  linelen = 1000;
  ntotal = 0;
  buff_curpos = 0;
  term_curpos = 0;
}

// Predefined control sequence
void wxCLI::MoveDown() {
  caretRow++;
  DoMoveCaret();
}

void wxCLI::MoveUp() {
  caretRow--; 
  if (caretRow < 0) caretRow = 0;
  DoMoveCaret();
}

void wxCLI::MoveRight() {
  caretCol++;
  if (caretCol >= ncolumn) caretCol = ncolumn - 1;
  DoMoveCaret();
}

void wxCLI::MoveLeft() {
  caretCol--;
  if (caretCol < 0) caretCol = 0;
  DoMoveCaret();
}

void wxCLI::MoveBOL() {
  caretCol = 0;
  DoMoveCaret();
}

void wxCLI::OutputRawString(std::string txt) {
  
}

/*.......................................................................
 * Write a character to the terminal after expanding tabs and control
 * characters to their multi-character representations.
 *
 * Input:
 *  gl    GetLine *   The resource object of this program.
 *  c        char     The character to be output.
 *  pad      char     Many terminals have the irritating feature that
 *                    when one writes a character in the last column of
 *                    of the terminal, the cursor isn't wrapped to the
 *                    start of the next line until one more character
 *                    is written. Some terminals don't do this, so
 *                    after such a write, we don't know where the
 *                    terminal is unless we output an extra character.
 *                    This argument specifies the character to write.
 *                    If at the end of the input line send '\0' or a
 *                    space, and a space will be written. Otherwise,
 *                    pass the next character in the input line
 *                    following the one being written.
 * Output:
 *  return    int     0 - OK.
 */ 
void wxCLI::OutputChar(char c, char pad) {
  char string[TAB_WIDTH + 4]; /* A work area for composing compound strings */
  int nchar;                  /* The number of terminal characters */
  int i;
  /*
   * Check for special characters.
   */
  if(c == '\t') {
    /*
     * How many spaces do we need to represent a tab at the current terminal
     * column?
     */
    nchar = DisplayedCharWidth('\t', term_curpos);
    /*
     * Compose the tab string.
     */
    for(i=0; i<nchar; i++)
      string[i] = ' ';
  } else if(IS_CTRL_CHAR(c)) {
    string[0] = '^';
    string[1] = CTRL_TO_CHAR(c);
    nchar = 2;
  } else if(!isprint((int)(unsigned char) c)) {
    sprintf(string, "\\%o", (int)(unsigned char)c);
    nchar = strlen(string);
  } else {
    string[0] = c;
    nchar = 1;
  };
  /*
   * Terminate the string.
   */
  string[nchar] = '\0';
  /*
   * Write the string to the terminal.
   */
  OutputRawString(string);
  /*
   * Except for one exception to be described in a moment, the cursor should
   * now have been positioned after the character that was just output.
   */
  term_curpos += nchar;
  /*
   * Keep a record of the number of characters in the terminal version
   * of the input line.
   */
  if(term_curpos > term_len)
    term_len = term_curpos;
  /*
   * If the new character ended exactly at the end of a line,
   * most terminals won't move the cursor onto the next line until we
   * have written a character on the next line, so append an extra
   * space then move the cursor back.
   */
  if(term_curpos % ncolumn == 0) {
    int term_curpos = term_curpos;
    OutputChar(pad ? pad : ' ', ' ');
    SetTermCurpos(term_curpos);
  };
}

/*.......................................................................
 * Write a string to the terminal after expanding tabs and control
 * characters to their multi-character representations.
 *
 * Input:
 *  gl    GetLine *   The resource object of this program.
 *  string   char *   The string to be output.
 *  pad      char     Many terminals have the irritating feature that
 *                    when one writes a character in the last column of
 *                    of the terminal, the cursor isn't wrapped to the
 *                    start of the next line until one more character
 *                    is written. Some terminals don't do this, so
 *                    after such a write, we don't know where the
 *                    terminal is unless we output an extra character.
 *                    This argument specifies the character to write.
 *                    If at the end of the input line send '\0' or a
 *                    space, and a space will be written. Otherwise,
 *                    pass the next character in the input line
 *                    following the one being written.
 * Output:
 *  return    int     0 - OK.
 */
void wxCLI::OutputString(std::string st, char pad) {
  for(int i=0;i<st.size()-1;i++)
    OutputChar(st[i],st[i+1]);
  OutputChar(st[st.size()-1],pad);
}

  // Delete nc characters starting from the one under the cursor.
  // Optionally copy the deleted characters to the cut buffer.
void wxCLI::DeleteChars(int nc, int cut) {
  /*
   * If there are fewer than nc characters following the cursor, limit
   * nc to the number available.
   */
  if(buff_curpos + nc > ntotal)
    nc = ntotal - buff_curpos;
/*
 * Copy the about to be deleted region to the cut buffer.
 */
  if(cut) {
    memcpy(cutbuf, line + buff_curpos, nc);
    cutbuf[nc] = '\0';
  }
  /*
   * Nothing to delete?
   */
  if(nc <= 0)
    return;
  /*
   * Copy the remaining part of the line back over the deleted characters.
   */
  memmove(line + buff_curpos, line + buff_curpos + nc,
	  ntotal - buff_curpos - nc + 1);
  ntotal -= nc;

  /*
   * Redraw the remaining characters following the cursor.
   */
  OutputString(line + buff_curpos, '\0');
  /*
   * Clear to the end of the terminal.
   */
  TruncateDisplay();
  /*
   * Place the cursor at the start of where the deletion was performed.
   */
  PlaceCursor(buff_curpos);
}

/*.......................................................................
 * Add a character to the line buffer at the current cursor position,
 * inserting or overwriting according the current mode.
 *
 * Input:
 *  gl   GetLine *   The resource object of this library.
 *  c       char     The character to be added.
 * Output:
 *  return   int     0 - OK.
 *                   1 - Insufficient room.
 */
void wxCLI::AddCharToLine(char c) {
  /*
   * Keep a record of the current cursor position.
   */
  int buff_curpos = buff_curpos;
  int term_curpos = term_curpos;
  /*
   * Work out the displayed width of the new character.
   */
  int width = DisplayedCharWidth(c, term_curpos);
  /*
   * If we are in insert mode, or at the end of the line,
   * check that we can accomodate a new character in the buffer.
   * If not, simply return, leaving it up to the calling program
   * to check for the absence of a newline character.
   */
  if((insert || buff_curpos >= ntotal) && ntotal >= linelen)
    return;
  /*
   * Are we adding characters to the line (ie. inserting or appending)?
   */
  if(insert || buff_curpos >= ntotal) {
    /*
     * If inserting, make room for the new character.
     */
    if(buff_curpos < ntotal) {
      memmove(line + buff_curpos + 1, line + buff_curpos,
	      ntotal - buff_curpos);
    };
    /*
     * Copy the character into the buffer.
     */
    line[buff_curpos] = c;
    buff_curpos++;
    /*
     * If the line was extended, update the record of the string length
     * and terminate the extended string.
     */
    ntotal++;
    line[ntotal] = '\0';
    /*
     * Redraw the line from the cursor position to the end of the line,
     * and move the cursor to just after the added character.
     */
    OutputString(line + buff_curpos, '\0');
    SetTermCurpos(term_curpos + width);
    /*
     * Are we overwriting an existing character?
     */
  } else {
    /*
     * Get the widths of the character to be overwritten and the character
     * that is going to replace it.
     */
    int old_width = DisplayedCharWidth(line[buff_curpos],
				       term_curpos);
    /*
     * Overwrite the character in the buffer.
     */
    line[buff_curpos] = c;
    /*
     * If we are replacing with a narrower character, we need to
     * redraw the terminal string to the end of the line, then
     * overwrite the trailing old_width - width characters
     * with spaces.
     */
    if(old_width > width) {
      OutputString(line + buff_curpos, '\0');
      /*
       * Clear to the end of the terminal.
       */
      TruncateDisplay();
      /*
       * Move the cursor to the end of the new character.
       */
      SetTermCurpos(term_curpos + width);
      buff_curpos++;
      /*
       * If we are replacing with a wider character, then we will be
       * inserting new characters, and thus extending the line.
       */
    } else if(width > old_width) {
      /*
       * Redraw the line from the cursor position to the end of the line,
       * and move the cursor to just after the added character.
       */
      OutputString(line + buff_curpos, '\0');
      SetTermCurpos(term_curpos + width);
      buff_curpos++;
      /*
       * The original and replacement characters have the same width,
       * so simply overwrite.
       */
    } else {
      /*
       * Copy the character into the buffer.
       */
      line[buff_curpos] = c;
      buff_curpos++;
      /*
       * Overwrite the original character.
       */
      OutputChar(c, line[buff_curpos]);
    };
  };
}

/*.......................................................................
 * Insert/append a string to the line buffer and terminal at the current
 * cursor position.
 *
 * Input:
 *  gl   GetLine *   The resource object of this library.
 *  s       char *   The string to be added.
 * Output:
 *  return   int     0 - OK.
 *                   1 - Insufficient room.
 */
void wxCLI::AddStringToLine(std::string s) {
  int buff_slen;   /* The length of the string being added to line[] */
  int term_slen;   /* The length of the string being written to the terminal */
  int buff_curpos; /* The original value of gl->buff_curpos */
  int term_curpos; /* The original value of gl->term_curpos */
  /*
 * Keep a record of the current cursor position.
 */
  buff_curpos = buff_curpos;
  term_curpos = term_curpos;
  /*
   * How long is the string to be added?
   */
  buff_slen = s.length();
  term_slen = DisplayedStringWidth(s, buff_slen, term_curpos);
  /*
   * Check that we can accomodate the string in the buffer.
   * If not, simply return, leaving it up to the calling program
   * to check for the absence of a newline character.
   */
  if(ntotal + buff_slen > linelen)
    return;
  /*
   * Move the characters that follow the cursor in the buffer by
   * buff_slen characters to the right.
   */
  if(ntotal > buff_curpos) {
    memmove(line + buff_curpos + buff_slen, line + buff_curpos,
	    ntotal - buff_curpos);
  };
  /*
   * Copy the string into the buffer.
   */
  memcpy(line + buff_curpos, s.c_str(), buff_slen);
  ntotal += buff_slen;
  buff_curpos += buff_slen;
  /*
   * Maintain the buffer properly terminated.
   */
  line[ntotal] = '\0';
  /*
   * Write the modified part of the line to the terminal, then move
   * the terminal cursor to the end of the displayed input string.
   */
  OutputString(line + buff_curpos, '\0');
  SetTermCurpos(term_curpos + term_slen);
}

/*.......................................................................
 * Truncate the displayed input line starting from the current
 * terminal cursor position, and leave the cursor at the end of the
 * truncated line. The input-line buffer is not affected.
 *
 * Input:
 *  gl     GetLine *   The resource object of gl_get_line().
 * Output:
 *  return     int     0 - OK.
 *                     1 - Error.
 */
void wxCLI::TruncateDisplay() {
  /*
   * Keep a record of the current terminal cursor position.
   */
  int term_curpos = term_curpos;
  /*
   * First clear from the cursor to the end of the current input line.
   */
  ClearEOL();
  /*
   * If there is more than one line displayed, go to the start of the
   * next line and clear from there to the end of the display. Note that
   * we can't use clear_eod to do the whole job of clearing from the
   * current cursor position to the end of the terminal because
   * clear_eod is only defined when used at the start of a terminal line
   * (eg. with gnome terminals, clear_eod clears from the start of the
   * current terminal line, rather than from the current cursor
   * position).
   */
  if(term_len / ncolumn > term_curpos / ncolumn) {
    MoveDown();
    MoveBOL();
    ClearEOD();
    /*
     * Where is the cursor now?
     */
    term_curpos = ncolumn * (term_curpos / ncolumn + 1);
    /*
     * Restore the cursor position.
     */
    SetTermCurpos(term_curpos);
  };
  /*
   * Update the recorded position of the final character.
   */
  term_len = term_curpos;
}

/*.......................................................................
 * Move the terminal cursor n characters to the left or right.
 *
 * Input:
 *  gl     GetLine *   The resource object of this program.
 *  n          int     number of positions to the right (> 0) or left (< 0).
 * Output:
 *  return     int     0 - OK.
 *                     1 - Error.
 */
void wxCLI::TerminalMoveCursor(int n) {
  int cur_row, cur_col; /* The current terminal row and column index of */
                        /*  the cursor wrt the start of the input line. */
  int new_row, new_col; /* The target terminal row and column index of */
                        /*  the cursor wrt the start of the input line. */
  /*
   * How far can we move left?
   */
  if(term_curpos + n < 0)
    n = term_curpos;
  /*
   * Break down the current and target cursor locations into rows and columns.
   */
  cur_row = term_curpos / ncolumn;
  cur_col = term_curpos % ncolumn;
  new_row = (term_curpos + n) / ncolumn;
  new_col = (term_curpos + n) % ncolumn;
  /*
   * Move down to the next line.
   */
  for(; cur_row < new_row; cur_row++) 
    MoveDown();
  /*
   * Move up to the previous line.
   */
  for(; cur_row > new_row; cur_row--)
    MoveUp();
  /*
   * Move to the right within the target line?
   */
  if(cur_col < new_col) {
    {
      for(; cur_col < new_col; cur_col++)
	MoveRight();
    };
    /*
     * Move to the left within the target line?
     */
  } else if(cur_col > new_col) {
    {
      for(; cur_col > new_col; cur_col--)
	MoveLeft();
    };
  }
  /*
   * Update the recorded position of the terminal cursor.
   */
  term_curpos += n;
}

// Move the terminal cursor to a given position.
void wxCLI::SetTermCurpos(int aterm_curpos) {
  TerminalMoveCursor(term_curpos - aterm_curpos);
}

// Set the position of the cursor both in the line input buffer and on the
// terminal.
void wxCLI::PlaceCursor(int abuff_curpos) {
  /*
   * Don't allow the cursor position to go out of the bounds of the input
   * line.
   */
  if(abuff_curpos >= ntotal)
    abuff_curpos = ntotal;
  if(abuff_curpos < 0)
    abuff_curpos = 0;
  /*
   * Record the new buffer position.
   */
  buff_curpos = abuff_curpos;
  /*
   * Move the terminal cursor to the corresponding character.
   */
  SetTermCurpos(BuffCurposToTermCurpos(abuff_curpos));
}

// Return the terminal cursor position that corresponds to a given
// line buffer cursor position.
int wxCLI::BuffCurposToTermCurpos(int abuff_curpos) {
  return prompt_len + DisplayedStringWidth(line, abuff_curpos,
					   prompt_len);
}

// Return the number of terminal characters needed to display a
// given raw character.
int wxCLI::DisplayedCharWidth(char c, int aterm_curpos) {
  if(c=='\t')
    return TAB_WIDTH - ((aterm_curpos % ncolumn) % TAB_WIDTH);
  if(IS_CTRL_CHAR(c))
    return 2;
  if(!isprint((int)(unsigned char) c)) {
    char string[TAB_WIDTH + 4];
    sprintf(string, "\\%o", (int)(unsigned char)c);
    return strlen(string);
  };
  return 1;
}

// Return the number of terminal characters needed to display a
// given substring.
int wxCLI::DisplayedStringWidth(std::string s, int nc, int aterm_curpos) {
  int slen=0;   /* The displayed number of characters */
  int i;
  /*
   * How many characters are to be measured?
   */
  if(nc < 0)
    nc = s.length();
  /*
   * Add up the length of the displayed string.
   */
  for(i=0; i<nc; i++)
    slen += DisplayedCharWidth(s[i], aterm_curpos + slen);
  return slen;
}

// Return non-zero if 'c' is to be consIdered part of a word.
int wxCLI::IsWordChar(int c) {
  return isalnum((int)(unsigned char)c) || strchr(GL_WORD_CHARS, c) != NULL;
}

// Display the prompt regardless of the current visibility mode.
int wxCLI::DisplayPrompt() {
  const char *pptr;       /* A pointer into gl->prompt[] */
  unsigned old_attr=0;    /* The current text display attributes */
  unsigned new_attr=0;    /* The requested text display attributes */
  /*
   * In case the screen got messed up, send a carriage return to
   * put the cursor at the beginning of the current terminal line.
   */
  MoveBOL();
  /*
   * Write the prompt, using the currently selected prompt style.
   */
  OutputString(prompt, '\0');
  return 0;
}

// Return the number of characters used by the prompt on the terminal.
int wxCLI::DisplayedPromptWidth() {
  int slen=0;         /* The displayed number of characters */
  const char *pptr;   /* A pointer into prompt[] */
  /*
   * The length differs according to the prompt display style.
   */
  return DisplayedStringWidth(prompt, -1, 0);
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
  charHeight = dc.GetCharHeight();
  charWidth = dc.GetCharWidth();
  m_hLine = charHeight;

  wxCaret *caret = new wxCaret(this, charWidth, charHeight);
  SetCaret(caret);

  caret->Move(0,0);
  caret->Show();
}

void wxCLI::MoveCaret(int x, int y) {
  caretCol = x;
  caretRow = y;

  //    DoMoveCaret();
}

void wxCLI::DoMoveCaret() {
  int screen_x, screen_y;
  
  CalcScrolledPosition(m_xMargin + caretCol * charWidth,
		       m_yMargin + caretRow * charHeight,
		       &screen_x, &screen_y);
  GetCaret()->Move(screen_x, screen_y);
}

void wxCLI::DoResizeBuffer(int xsize, int ysize) {
  if (m_text == NULL) {
    m_text = (wxChar *) calloc(xsize*ysize*m_scrollback,sizeof(wxChar));
    ncolumn = xsize;
    m_yChars = ysize;
    return;
  }
//   std::cout << "Got size event " << xsize << " x " << ysize << "\n";
//   std::cout.flush();
  if ((ncolumn == xsize) && (m_yChars == ysize)) return;
  wxChar *newBuf;

  newBuf = (wxChar *) calloc(xsize*ysize*m_scrollback,sizeof(wxChar));
  int copyx;
  copyx = xsize;
  if (copyx > ncolumn) copyx = ncolumn;
  for (int k=0;k<m_nLines;k++) {
    for (int l=0;l<copyx;l++)
      newBuf[k*xsize+l] = m_text[k*ncolumn+l];
  }
  free(m_text);
  m_text = newBuf;
  ncolumn = xsize;
  m_yChars = ysize;
}

void wxCLI::OnSize( wxSizeEvent &event ) {

  int ncolumn_new = (event.GetSize().x - 2*m_xMargin) / charWidth;
  int m_yChars_new = (event.GetSize().y - 2*m_yMargin) / charHeight;
  if ( !ncolumn_new )
    ncolumn_new = 1;
  if ( !m_yChars_new )
    m_yChars_new = 1;

  DoResizeBuffer(ncolumn_new,m_yChars_new);

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
    for (int x=0; x < ncolumn; x++) {
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
					     int32Constructor(ncolumn)));
}

void wxCLI::IssueGetLineRequest(const char *prompt) {
  PutMessage(prompt);
  promptLength = strlen(prompt);
}

void wxCLI::Backspace() {
  int i;
  if (caretCol <= promptLength) return;
  /* Delete the character pointed to by the type-ahead buffer*/
  for (i=typeAheadPtr;i<typeAheadTail;i++) {
    typeAhead[i-1] = typeAhead[i];
  }
  typeAheadTail--;
  typeAheadPtr--;
  /* Change the contents of the screen buffer also */
  for (i=caretCol;i<ncolumn;i++)
    CharAt(i-1,caretRow) = CharAt(i,caretRow);
  CharAt(ncolumn-1,caretRow) = 0;
  caretCol--;
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
    for (i=ncolumn-1;i>=caretCol;i--)
      CharAt(i+1,caretRow) = CharAt(i,caretRow);
    typeAheadTail++;
    Refresh();
  }
  typeAhead[typeAheadPtr++] = (char) ch;
  if (typeAheadPtr > typeAheadTail)
    typeAheadTail = typeAheadPtr;
  CharAt(caretCol, caretRow) = ch;
  
  wxCaretSuspend cs(this);
  wxClientDC dc(this);
  dc.SetFont(m_font);
  dc.SetBackgroundMode(wxSOLID); // overwrite old value
  int scroll_x, scroll_y;
  CalcScrolledPosition(caretCol * charWidth,
		       caretRow * charHeight,
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
      if (caretCol > promptLength) {
	PrevChar();
	typeAheadPtr--;
      }
      break;
    case WXK_RIGHT:
      if (caretCol  < ncolumn) {
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

