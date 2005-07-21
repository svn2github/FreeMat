#include "TermWidget.hpp"
#include <iostream>

#include <qglobal.h>

#define TMIN(a,b) ((a) < (b) ? (a) : (b))
#define TMAX(a,b) ((a) > (b) ? (a) : (b))

TermWidget::TermWidget() {
  m_surface = NULL;
  m_onscreen = NULL;
  m_history = NULL;
  m_scrollback = 1000;
  m_history_lines = 0;
  m_cursor_x = 0;
  m_cursor_y = 0;
  m_clearall = true;
  cursorOn = false;
  m_blink_skip = true;
  blinkEnable = true;
  m_scrolling = false;
  m_scroll_offset = 0;
  m_mousePressed = false;
  m_firsttime = true;
  selectionStart = 0;
  selectionStop = 0;
  m_scrollmin = 0;
  m_scrollmax = 0;
  m_scrollline = 1;
  m_scrollpage = 0;
}

TermWidget::~TermWidget() {
}

void TermWidget::Initialize() {
  setFont(10);
  m_active_width = 400;
  resizeTextSurface();
}

void TermWidget::scrollBack(int val) {
  if (m_history_lines == 0) return;
  m_scrolling =  (val != m_history_lines);
  m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();
  m_surface = m_history + (m_scrollback - m_height - (m_history_lines - val))*m_width;
  m_scroll_offset = (m_scrollback - m_height - (m_history_lines - val))*m_width;
}

void TermWidget::blink() {
  if (!blinkEnable) return;
  if (m_blink_skip) {
    m_blink_skip = false;
    return;
  }
  m_surface[m_cursor_x + m_cursor_y*m_width].toggleCursor();
}

void TermWidget::resizeTextSurface() {
  if (!m_char_w || !m_char_h) return;
  if (m_surface)
    m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();  
  int cursor_offset = m_height - 1 - m_cursor_y;
  //  m_timer_refresh->start(30,false);
  //  m_timer_blink->start(1000);
  m_blink_skip = true;
  int new_width = m_active_width/m_char_w;
  int new_height = GetHeight()/m_char_h;
  tagChar *new_history = new tagChar[new_width*m_scrollback];
  
  if (m_history) {
    int minwidth = TMIN(new_width,m_width);
    int offset = 0;
    if (!m_history_lines && !m_firsttime)
      offset = m_height - new_height;
    if (offset < 0) {
      for (int i=-offset;i<m_scrollback;i++)
	for (int j=0;j<minwidth;j++)
	  new_history[(i+offset)*new_width+j] = m_history[i*m_width+j];
    } else {
      for (int i=offset;i<m_scrollback;i++)
	for (int j=0;j<minwidth;j++)
	  new_history[i*new_width+j] = m_history[(i-offset)*m_width+j];
    }
  }
  // Copy old history to new history here
  
  delete[] m_onscreen;
  delete[] m_history;
  m_onscreen = new tagChar[new_width*new_height];
  m_history = new_history;
  m_surface = m_history + (m_scrollback - new_height)*new_width;
  
  if (!m_firsttime && m_history_lines) {
    m_history_lines -= (new_height-m_height);
    m_history_lines = TMAX(0,m_history_lines);
  }

  m_width = new_width;
  m_height = new_height;
  m_clearall = true;
  // only do this the first time
  //  if (firsttime) {
  if (!m_history_lines) {
    if (m_firsttime) {
      SetCursor(0,0);
      setScrollbar(0);
    } else {
      // Do nothing?
    }
  } else {
    m_cursor_y = m_height - 1 - cursor_offset;
    setScrollbar(m_history_lines);
  }
  m_scroll_offset = (m_scrollback - m_height)*m_width;
}

void TermWidget::setScrollbar(int val) {
  if ((m_scrollmin != 0) ||
      (m_scrollmax != m_history_lines) ||
      (m_scrollline != 1) ||
	  (m_scrollpage != m_height)) {
    SetupScrollBar(0,m_history_lines,1,m_height,val);
	m_scrollmin = 0;
	m_scrollmax = m_history_lines;
	m_scrollline = 1;
	m_scrollpage = m_height;
	  }
  else
    SetScrollBarValue(val);
}

void TermWidget::PutString(std::string txt) {
  m_firsttime = false;
  if (m_scrolling) {
    setScrollbar(m_history_lines);
  }
  for (int i=0;i<txt.size();i++) {
    if (txt[i] == '\n')
      SetCursor(m_cursor_x,m_cursor_y+1);
    else if (txt[i] == '\r')
      SetCursor(0,m_cursor_y);
    else {
      m_surface[m_cursor_x + m_cursor_y*m_width] = tagChar(txt[i]);
      SetCursor(m_cursor_x+1,m_cursor_y);
    }
  }
  m_blink_skip = true;
}

void TermWidget::OnResize() {
  //  XPWindow::OnResize();
  m_active_width = GetWidth();
  Erase();
  resizeTextSurface();
}

void TermWidget::SetCursor(int x, int y) {
  //   cursorOn = false;
  //   m_onscreen[m_cursor_x + m_cursor_y*m_width] = -1;
  //   repaint(cursorRect,true);
  //   cursorEnable = false;
  //  if (m_scrolling) 
  //    setScrollbar(0);
  if (m_surface[m_cursor_y*m_width+m_cursor_x].cursor())
    m_surface[m_cursor_y*m_width+m_cursor_x].toggleCursor();
  m_cursor_x = x;
  m_cursor_y = y;
  m_cursor_y += m_cursor_x/m_width;
  m_cursor_x %= m_width;
  if (m_cursor_y >= m_height) {
    // scroll up - which we do by a single memmove op
    int toscroll = m_cursor_y - m_height + 1;
    for (int i=0;i<(m_scrollback - toscroll)*m_width;i++)
      m_history[i] = m_history[i+toscroll*m_width];
    for (int i=0;i<toscroll*m_width;i++)
      m_history[(m_scrollback - toscroll)*m_width+i] = tagChar();
    m_history_lines = TMIN(m_history_lines+toscroll,m_scrollback-m_height);
    m_cursor_y -= toscroll;
    setScrollbar(m_history_lines);
  }
  m_surface[m_cursor_y*m_width+m_cursor_x].setCursor();
}

void TermWidget::OnScroll(int val) {
  scrollBack(val);
}

// Want scrollbar to be such that the cursor is visible
void TermWidget::OnKeyPress(int c) {
  if (m_scrolling) 
    setScrollbar(m_history_lines);
  if (c == 'q')
    exit(0);
  if (c == 'l') {
    SetCursor(TMAX(m_cursor_x-1,0),m_cursor_y);
    return;
  }
  if (c != 'd' && c != 'x') {
    char buffer[2];
    buffer[0] = c;
    buffer[1] = 0;
    PutString(buffer);
  } else if (c == 'd')
    PutString("Now is the time for all men to come to the aid of their country, and by their aid, assist those who need it, or something like that....");
  else if (c == 'x') {
    for (int i=0;i<50;i++) {
      char buffer[1000];
      sprintf(buffer,"line %d\r\n",i);
      PutString(buffer);
    }
  }
#if 0
  if (m_scrolling) 
    setScrollbar(0);
  int keycode = e->key();
  if (!keycode) return;
  if (keycode == Qt::Key_Left)
    ProcessChar(KM_LEFT);
  else if (keycode == Qt::Key_Right)
    ProcessChar(KM_RIGHT);
  else if (keycode == Qt::Key_Up)
    ProcessChar(KM_UP);
  else if (keycode == Qt::Key_Down)
    ProcessChar(KM_DOWN);
  else if (keycode == Qt::Key_Delete)
    ProcessChar(KM_DELETE);
  else if (keycode == Qt::Key_Insert)
    ProcessChar(KM_INSERT);
  else if (keycode == Qt::Key_Home)
    ProcessChar(KM_HOME);
  else if (keycode == Qt::Key_End)
    ProcessChar(KM_END);
  else {
    char key = e->ascii();
    if (key) ProcessChar(key);
  }
#endif
}


void TermWidget::OnMouseDown(int x, int y) {
  // Get the x and y coordinates of the mouse click - map that
  // to a row and column
  int clickcol = x/m_char_w;
  int clickrow = y/m_char_h;
  selectionStart = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStart = TMIN(TMAX(0,selectionStart),m_width*m_scrollback-1);
  selectionStop = selectionStart;
}

void TermWidget::DrawContent() {
  BeginDraw();
  if (m_width == 0) return;
  for (int i=0;i<m_height;i++) {
    int j=0;
    while (j<m_width) {
      // skip over characters that do not need to be redrawn
      while ((j < m_width) && 
	     (m_onscreen[i*m_width+j] == m_surface[i*m_width+j])) j++;
      if (j < m_width) {
	tagChar g = m_surface[i*m_width+j];
	if (m_scrolling) 
	  g.flags &= ~CURSORBIT;
	PutTagChar(j*m_char_w,i*m_char_h,g);
	m_onscreen[i*m_width+j] = g;
	j++;
      }
    }
  }
  EndDraw();
}

void TermWidget::OnMouseDrag(int x, int y) {
  if (y < 0) 
    ScrollLineUp();
  if (y > GetHeight())
    ScrollLineDown();
  // Get the position of the click
  // to a row and column
  int clickcol = x/m_char_w;
  int clickrow = y/m_char_h;
  selectionStop = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStop = TMIN(TMAX(0,selectionStop),m_width*m_scrollback-1);
  // clear the selection bits
  for (int i=0;i<m_width*m_scrollback;i++)
    m_history[i].clearSelection();
  // set the selection bits
  if (selectionStart < selectionStop)
    for (int i=selectionStart;i<selectionStop;i++)
      m_history[i].setSelection();
  else
    for (int i=selectionStop;i<selectionStart;i++)
      m_history[i].setSelection();
}

void TermWidget::OnMouseUp( int, int ) {
}
