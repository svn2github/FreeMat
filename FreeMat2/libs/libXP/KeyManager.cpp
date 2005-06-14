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

/*
 * To-add: copy/paste/select - typeahead
 */

#include <iostream>
#include <algorithm>
#include "KeyManager.hpp"
#include "Interface.hpp"

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

// Maximum allowed number of columns in the text window
#define MAXCOLS 256


// ----------------------------------------------------------------------------
// KeyManager
// ----------------------------------------------------------------------------

KeyManager::KeyManager() {
  cutbuf[0] = '\0';
  linelen = 1000;
  ntotal = 0;
  buff_curpos = 0;
  term_curpos = 0;
  keyseq_count = 0;
  last_search = -1;
  ncolumn = 80;
  nline = 24;
  insert = true;
  ReplacePrompt("");
  ResetLineBuffer();
  history.push_back("");
}


void KeyManager::EndOfLine() {
  PlaceCursor(ntotal);
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
void KeyManager::OutputChar(char c, char pad) {
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
    int sterm_curpos = term_curpos;
    OutputChar(pad ? pad : ' ', ' ');
    SetTermCurpos(sterm_curpos);
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
void KeyManager::OutputString(std::string st, char pad) {
  if (st.size() == 0) return;
  for(int i=0;i<st.size()-1;i++)
    OutputChar(st[i],st[i+1]);
  OutputChar(st[st.size()-1],pad);
}

void KeyManager::ReplacePrompt(std::string aprompt) {
  prompt = aprompt;
  prompt_len = DisplayedPromptWidth();
}

  // Delete nc characters starting from the one under the cursor.
  // Optionally copy the deleted characters to the cut buffer.
void KeyManager::DeleteChars(int nc, int cut) {
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
void KeyManager::AddCharToLine(char c) {
  /*
   * Keep a record of the current cursor position.
   */
  int sbuff_curpos = buff_curpos;
  int sterm_curpos = term_curpos;
  /*
   * Work out the displayed width of the new character.
   */
  int width = DisplayedCharWidth(c, sterm_curpos);
  /*
   * If we are in insert mode, or at the end of the line,
   * check that we can accomodate a new character in the buffer.
   * If not, simply return, leaving it up to the calling program
   * to check for the absence of a newline character.
   */
  if((insert || sbuff_curpos >= ntotal) && ntotal >= linelen)
    return;
  /*
   * Are we adding characters to the line (ie. inserting or appending)?
   */
  if(insert || sbuff_curpos >= ntotal) {
    /*
     * If inserting, make room for the new character.
     */
    if(sbuff_curpos < ntotal) {
      memmove(line + sbuff_curpos + 1, line + sbuff_curpos,
	      ntotal - sbuff_curpos);
    };
    /*
     * Copy the character into the buffer.
     */
    line[sbuff_curpos] = c;
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
    OutputString(line + sbuff_curpos, '\0');
    SetTermCurpos(sterm_curpos + width);
    /*
     * Are we overwriting an existing character?
     */
  } else {
    /*
     * Get the widths of the character to be overwritten and the character
     * that is going to replace it.
     */
    int old_width = DisplayedCharWidth(line[sbuff_curpos],
				       sterm_curpos);
    /*
     * Overwrite the character in the buffer.
     */
    line[sbuff_curpos] = c;
    /*
     * If we are replacing with a narrower character, we need to
     * redraw the terminal string to the end of the line, then
     * overwrite the trailing old_width - width characters
     * with spaces.
     */
    if(old_width > width) {
      OutputString(line + sbuff_curpos, '\0');
      /*
       * Clear to the end of the terminal.
       */
      TruncateDisplay();
      /*
       * Move the cursor to the end of the new character.
       */
      SetTermCurpos(sterm_curpos + width);
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
      OutputString(line + sbuff_curpos, '\0');
      SetTermCurpos(sterm_curpos + width);
      buff_curpos++;
      /*
       * The original and replacement characters have the same width,
       * so simply overwrite.
       */
    } else {
      /*
       * Copy the character into the buffer.
       */
      line[sbuff_curpos] = c;
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
void KeyManager::AddStringToLine(std::string s) {
  int buff_slen;   /* The length of the string being added to line[] */
  int term_slen;   /* The length of the string being written to the terminal */
  int sbuff_curpos; /* The original value of gl->buff_curpos */
  int sterm_curpos; /* The original value of gl->term_curpos */
  /*
 * Keep a record of the current cursor position.
 */
  sbuff_curpos = buff_curpos;
  sterm_curpos = term_curpos;
  /*
   * How long is the string to be added?
   */
  buff_slen = s.length();
  term_slen = DisplayedStringWidth(s, buff_slen, sterm_curpos);
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
  OutputString(line + sbuff_curpos, '\0');
  SetTermCurpos(sterm_curpos + term_slen);
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
void KeyManager::TruncateDisplay() {
  /*
   * Keep a record of the current terminal cursor position.
   */
  int aterm_curpos = term_curpos;
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
    term_curpos = ncolumn * (aterm_curpos / ncolumn + 1);
    /*
     * Restore the cursor position.
     */
    SetTermCurpos(aterm_curpos);
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
void KeyManager::TerminalMoveCursor(int n) {
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
void KeyManager::SetTermCurpos(int aterm_curpos) {
  TerminalMoveCursor(aterm_curpos - term_curpos);
}

// Set the position of the cursor both in the line input buffer and on the
// terminal.
void KeyManager::PlaceCursor(int abuff_curpos) {
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
  int tmpi = BuffCurposToTermCurpos(abuff_curpos);
  SetTermCurpos(tmpi);
}

// Return the terminal cursor position that corresponds to a given
// line buffer cursor position.
int KeyManager::BuffCurposToTermCurpos(int abuff_curpos) {
  return prompt_len + DisplayedStringWidth(line, abuff_curpos,
					   prompt_len);
}

// Return the number of terminal characters needed to display a
// given raw character.
int KeyManager::DisplayedCharWidth(char c, int aterm_curpos) {
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
int KeyManager::DisplayedStringWidth(std::string s, int nc, int aterm_curpos) {
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
int KeyManager::IsWordChar(int c) {
  return isalnum((int)(unsigned char)c) || strchr(GL_WORD_CHARS, c) != NULL;
}

// Display the prompt regardless of the current visibility mode.
int KeyManager::DisplayPrompt() {
  const char *pptr;       /* A pointer into gl->prompt[] */
  unsigned old_attr=0;    /* The current text display attributes */
  unsigned new_attr=0;    /* The requested text display attributes */
  /*
   * In case the screen got messed up, send a carriage return to
   * put the cursor at the beginning of the current terminal line.
   */
  MoveBOL();
  ClearEOL();
  /*
   * Write the prompt, using the currently selected prompt style.
   */
  OutputString(prompt, '\0');
  return 0;
}

// Return the number of characters used by the prompt on the terminal.
int KeyManager::DisplayedPromptWidth() {
  int slen=0;         /* The displayed number of characters */
  const char *pptr;   /* A pointer into prompt[] */
  /*
   * The length differs according to the prompt display style.
   */
  return DisplayedStringWidth(prompt, -1, 0);
}

KeyManager::~KeyManager() {
}


void KeyManager::DoResizeBuffer(int xsize, int ysize) {
  int i;
  int cols_used = DisplayedStringWidth(line,-1,prompt_len) + prompt_len + ncolumn - 1;
  int lines_used = (DisplayedStringWidth(line,-1,prompt_len) +
		    prompt_len + ncolumn - 2) / (ncolumn-1);
  /*
   * Move to the cursor to the start of the line.
   */
  for(i=1; i<lines_used; i++) {
    MoveUp();
  };
  MoveBOL();
  /*
   * Clear to the end of the terminal.
   */
  ClearEOD();
  /*
   * Record the fact that the cursor is now at the beginning of the line.
   */
  term_curpos = 0;
  /*
   * Update the recorded window size.
   */
  ncolumn = (xsize>20) ? xsize : 20;
  nline = (ysize>5) ? ysize : 5;
  /*
   * Redisplay the line?
   */
  Redisplay();
}


void KeyManager::ResetLineBuffer() {
  memset(line,0,1002);
  ntotal = 0;
  buff_curpos = 0;
  term_curpos = 0;
  term_len = 0;
  insert_curpos = 0;
}

void KeyManager::NewLine() {
  line[ntotal++] = '\n';
  line[ntotal] = 0;
  //  if (waitingForInput) {
    //    FreeMat::PostGUIReply(new FreeMat::Command(FreeMat::CMD_GUIGetLineAcq,
    //					       FreeMat::Array::stringConstructor(line)));
  //    waitingForInput = false;
  //  } else {
  //    pendinglines.push_back(line);
  //  }
  //  printf("Execute command %s\r\n",line);
  EndOfLine();
  OutputRawString("\r\n");
  ExecuteLine(line);
  AddHistory(line);
  ResetLineBuffer();
  DisplayPrompt();
}

void KeyManager::CursorLeft() {
  PlaceCursor(buff_curpos-1);
}

void KeyManager::CursorRight() {
  PlaceCursor(buff_curpos+1);
}

void KeyManager::BeginningOfLine() {
  PlaceCursor(0);
}

void KeyManager::BackwardDeleteChar() {
  if (1 > buff_curpos - insert_curpos)
    return;
  CursorLeft();
  DeleteChars(1,0);
}

void KeyManager::ForwardDeleteChar() {
  DeleteChars(1,0);
}

void KeyManager::AddHistory(std::string line) {
  prefix = "";
  prefix_len = 0;
  history.push_back(line);
  return;
//  if (line.size() <= 1) return;
//  if (history.size() >= 1)
//    history.pop_back();
//  if ((history.size() > 0) && (history.back() == line)) {
//    history.push_back("");    
//    return;
//  }
//  history.push_back(line);    
//  history.push_back("");
//  return;
}

void KeyManager::Redisplay() {
  /*
   * Keep a record of the current cursor position.
   */
  int sbuff_curpos = buff_curpos;
  /*
   * Move the cursor to the start of the terminal line, and clear from there
   * to the end of the display.
   */
  SetTermCurpos(0);
  ClearEOD();
  /*
   * Nothing is displayed yet.
   */
  term_len = 0;
  /*
   * Display the current prompt.
   */
  DisplayPrompt();
  /*
   * Render the part of the line that the user has typed in so far.
   */
  OutputString(line,'\0');
  /*
   * Restore the cursor position.
   */
  PlaceCursor(sbuff_curpos);
}

void KeyManager::HistoryFindBackwards() {
  int i;
  bool found;
  if (startsearch == 0) return;
  i = startsearch-1;
  found = false;
  while (i>=0 && !found) {
    found = (history[i].compare(0,prefix_len,prefix) == 0);
    if (!found) i--;
  }
  if (!found) return;
  strcpy(line,history[i].c_str());
  if (strlen(line)>=1)
    line[strlen(line)-1] = 0;
  startsearch = i;
}

void KeyManager::HistoryFindForwards() {
  int i;
  bool found;
  if (startsearch == 0) return;
  i = startsearch+1;
  found = false;
  while (i<history.size() && !found) {
    found = (prefix_len == 0) || 
      (history[i].compare(0,prefix_len,prefix) == 0);
    if (!found) i++;
  }
  if (!found && (i >= history.size())) {
    line[0] = 0;
    return;
  }
  strcpy(line,history[i].c_str());
  if (strlen(line)>=1)
    line[strlen(line)-1] = 0;
  startsearch = i;
}

void KeyManager::SearchPrefix(const char* aline, int aprefix_len) {
  char *tbuf = (char*) malloc(linelen+2);
  // Set the prefix string
  memcpy(tbuf,aline,aprefix_len);
  tbuf[aprefix_len] = 0;
  prefix_len = aprefix_len;
  prefix = std::string(tbuf);
  startsearch = history.size();
  free(tbuf);
}

void KeyManager::HistorySearchBackward() {
  if (last_search != keyseq_count-1)
    SearchPrefix(line,buff_curpos);
  last_search = keyseq_count;
  HistoryFindBackwards();
  ntotal = strlen(line);
  buff_curpos = strlen(line);
  Redisplay();
}

void KeyManager::HistorySearchForward() {
  if (last_search != keyseq_count-1)
    SearchPrefix(line,buff_curpos);
  last_search = keyseq_count;
  HistoryFindForwards();
  ntotal = strlen(line);
  buff_curpos = strlen(line);
  Redisplay();
}

void KeyManager::KillLine() {
  strcpy(cutbuf,line+buff_curpos);
  ntotal = buff_curpos;
  line[ntotal] = '\0';
  TruncateDisplay();
  PlaceCursor(buff_curpos);
}

void KeyManager::Yank() {
  buff_mark = buff_curpos;
  if (cutbuf[0] == '\0')
    return;
  AddStringToLine(cutbuf);
}

void KeyManager::ListCompletions(std::vector<std::string> completions) {
  int maxlen;    /* The length of the longest matching string */
  int width;     /* The width of a column */
  int ncol;      /* The number of columns to list */
  int nrow;      /* The number of rows needed to list all of the matches */
  int row,col;   /* The row and column being written to */
  int i;
  /*
   * Not enough space to list anything?
   */
  if(ncolumn < 1)
    return;
  /*
   * Work out the maximum length of the matching strings.
   */
  maxlen = 0;
  for(i=0; i<completions.size(); i++) {
    int len = completions[i].length();
    if(len > maxlen)
      maxlen = len;
  };
  /*
   * Nothing to list?
   */
  if(maxlen == 0)
    return;
  /*
   * Split the available terminal width into columns of maxlen + 2 characters.
   */
  width = maxlen + 2;
  ncol = ncolumn / width;
  /*
   * If the column width is greater than the terminal width, the matches will
   * just have to overlap onto the next line.
   */
  if(ncol < 1)
    ncol = 1;
  /*
   * How many rows will be needed?
   */
  nrow = (completions.size() + ncol - 1) / ncol;
  /*
   * Print the matches out in ncol columns, sorted in row order within each
   * column.
   */
  for(row=0; row < nrow; row++) {
    for(col=0; col < ncol; col++) {
      int m = col*nrow + row;
      if(m < completions.size()) {
	char buffer[4096];
	sprintf(buffer, "%s%-*s%s", completions[m].c_str(),
		(int) (ncol > 1 ? maxlen - completions[m].length():0),
		"", col<ncol-1 ? "  " : "\r\n");
	OutputRawString(buffer);
      } else {
	OutputRawString("\r\n");
	break;
      };
    };
  };
}

std::string GetCommonPrefix(std::vector<std::string> matches,
			    std::string tempstring) {
  int minlength;
  int prefixlength;
  bool allmatch;
  std::string templ;
  int i, j;

  minlength = matches[0].size();
  for (i=0;i<matches.size();i++)
    minlength = (minlength < matches[i].size()) ? 
      minlength : matches[i].size();
  prefixlength = minlength;
  templ = matches[0];
  for (i=0;i<matches.size();i++) {
    j = 0;
    allmatch = true;
    while (allmatch && (j<prefixlength)) {
      std::string mtch(matches[i]);
      allmatch = (mtch[j] == templ[j]);
      if (allmatch) j++;
    }
    prefixlength = (j < prefixlength) ? j : prefixlength;
  }
  if (prefixlength <= tempstring.length())
    return (std::string(""));
  else
    return(templ.substr(tempstring.length(),prefixlength-tempstring.length()));
}

void KeyManager::CompleteWord() {
  int redisplay=0;        /* True if the whole line needs to be redrawn */
  int suffix_len;         /* The length of the completion extension */
  int cont_len;           /* The length of any continuation suffix */
  int nextra;             /* The number of characters being added to the */
                          /*  total length of the line. */
  int buff_pos;           /* The buffer index at which the completion is */
                          /*  to be inserted. */
  std::vector<std::string> matches;
  redisplay = 1;
  /*
   * Get the cursor position at which the completion is to be inserted.
   */
  buff_pos = buff_curpos;
  /*
   * Perform the completion.
   */
  std::string tempstring;
  matches = GetCompletionList(line, buff_curpos, tempstring);
  if(matches.size() == 0) {
    OutputRawString("\r\n");
    term_curpos = 0;
    redisplay = 1;
    /*
     * Are there any completions?
     */
  } else if(matches.size() >= 1) {
    /*
     * If there any ambiguous matches, report them, starting on a new line.
     */
    if(matches.size() > 1) {
      OutputRawString("\r\n");
      ListCompletions(matches);
      redisplay = 1;
    };
    /*
     * Find the common prefix
     */
    std::string prefix;
    prefix = GetCommonPrefix(matches, tempstring);
    /*
     * Get the length of the suffix and any continuation suffix to add to it.
     */
    suffix_len = prefix.length(); // This is supposed to be the length of the filename extension...
    cont_len = 0;
    /*
     * Work out the number of characters that are to be added.
     */
    nextra = suffix_len + cont_len;
    /*
     * Is there anything to be added?
     */
    if(nextra) {
      /*
       * Will there be space for the expansion in the line buffer?
       */
      if(ntotal + nextra < linelen) {
	/*
	 * Make room to insert the filename extension.
	 */
	memmove(line + buff_curpos + nextra, line + buff_curpos,
		ntotal - buff_curpos);
	/*
	 * Insert the filename extension.
	 */
	memcpy(line + buff_curpos, prefix.c_str(), suffix_len);
	/*
	 * Record the increased length of the line.
	 */
	ntotal += nextra;
	/*
	 * Place the cursor position at the end of the completion.
	 */
	buff_curpos += nextra;
	/*
	 * Terminate the extended line.
	 */
	line[ntotal] = '\0';
	/*
	 * If we don't have to redisplay the whole line, redisplay the part
	 * of the line which follows the original cursor position, and place
	 * the cursor at the end of the completion.
	 */
	if(!redisplay) {
	  TruncateDisplay();
	  OutputString(line + buff_pos, '\0');
	  PlaceCursor(buff_curpos);
	  return;
	};
      } else {
	redisplay = 1;
      };
    };
  };
  /*
   * Redisplay the whole line?
   */
  if(redisplay) {
    term_curpos = 0;
    Redisplay();
    return;
  };
  return;
}

extern bool InterruptPending;

void KeyManager::RegisterInterrupt() {
  InterruptPending  = true;
}

void KeyManager::OnChar( int c ) {
  keyseq_count++;
  switch(c) {
  case KM_LEFT:
    CursorLeft();
    break;
  case KM_RIGHT:
    CursorRight();
    break;
  case KM_DELETE:
    ForwardDeleteChar();
    break;
  case KM_INSERT:
    insert = !insert;
    break;
  case KM_HOME:
    BeginningOfLine();
    break;
  case KM_END:
    EndOfLine();
    break;
  case KM_UP:
    HistorySearchBackward();
    break;
  case KM_DOWN:
    HistorySearchForward();
    break;
  case KM_CTRLA:
    BeginningOfLine();
    break;
  case KM_CTRLC:
    RegisterInterrupt();
    break;
  case KM_CTRLE:
    EndOfLine();
    break;
  case KM_CTRLD:
    ForwardDeleteChar();
    break;
  case KM_TAB:
    if ((buff_curpos != 0) && (line[buff_curpos-1] != ' ') &&
	(line[buff_curpos-1] != '\t'))
      CompleteWord();
    else
      AddCharToLine(c);
    break;
  case KM_CTRLK:
    KillLine();
    break;
  case KM_NEWLINE:
    NewLine();
    break;
  case KM_CTRLY:
    Yank();
    break;
  case KM_BACKSPACE:
  case KM_BACKSPACEALT:
    BackwardDeleteChar();
    break;
  default:
    AddCharToLine(c);
  }
}

