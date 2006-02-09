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
#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <string>
#define CURSORBIT 1
#define SELECTBIT 2
class tagChar
{
 public:
  char v;
  char flags;
  tagChar(): v(' '), flags(0) {};
  tagChar(char t): v(t), flags(0) {};
  bool noflags() {return(flags == 0);};
  bool cursor() {return(flags & CURSORBIT);};
  void toggleCursor() {flags ^= CURSORBIT;};
  void setCursor() {flags |= CURSORBIT;};
  void clearCursor() {flags &= ~CURSORBIT;};
  void clearSelection() {flags &= ~SELECTBIT;};
  void setSelection() {flags |= SELECTBIT;};
  bool operator == (const tagChar& b) {return (v==b.v) && (flags==b.flags);};
};

class TermWidget {
protected:
  tagChar* m_surface;  // width x height
  int m_height;        // height of terminal in characters
  int m_width;         // width of terminal in characters
  int m_cursor_x;      // column position of the cursor
  int m_cursor_y;      // row position of the cursor
  tagChar* m_onscreen; // width x height - contains contents of the screen
  tagChar* m_history;  // width x scrollheight - contains history of terminal
  int m_char_w;        // width of a character
  int m_char_h;        // height of a character
  int m_char_decent;   // descent of a character
  int m_active_width;  // width of the active text region.
  bool m_blink_skip;
  bool m_clearall;
  bool cursorOn;
  bool blinkEnable;
  int m_scrollback;
  int m_history_lines;
  bool m_scrolling;
  bool m_mousePressed;
  int selectionStart;
  int selectionStop;
  int m_scroll_offset;
  int m_scrollmin, m_scrollmax, m_scrollline, m_scrollpage;
  bool m_firsttime;
 public:
  void scrollBack(int val);
  TermWidget();
  virtual ~TermWidget();
  void resizeTextSurface();
  //  virtual void ProcessChar(int c) = 0;
  void PutString(std::string txt);
  void blink();
  void SetCursor(int x, int y);
  int  getTextWidth() {return m_width;};
  void Initialize();
  //TK dependant functions
public:
  void setScrollbar(int val);
  void OnResize();
  void OnKeyPress(int key);
  void OnMouseDown(int x, int y);
  void OnMouseDrag(int x, int y);
  void OnMouseUp(int x, int y);
  void OnScroll(int val);
  void DrawContent();
  void EnableBlink();
  void DisableBlink();
  
  virtual int GetHeight() = 0;
  virtual int GetWidth() = 0;
  virtual void InstallEventTimers() = 0;
  virtual void ScrollLineUp() = 0;
  virtual void ScrollLineDown() = 0;
  virtual void SetScrollBarValue(int val) = 0;
  virtual void SetupScrollBar(int minval, int maxval, int step, int page, int val) = 0;
  virtual void BeginDraw() = 0;
  virtual void PutTagChar(int x, int y, tagChar g) = 0;
  virtual void EndDraw() = 0;
  virtual void setFont(int size) = 0;
  virtual void Erase() = 0;
};

#endif
