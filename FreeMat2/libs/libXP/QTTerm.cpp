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
#include "QTTerm.hpp"
#include "KeyManager.hpp"
#include <qapplication.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qclipboard.h>
#include <math.h>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>

tagArray::tagArray(int length, tagChar* ptr) {
  if (ptr)
    m_buffer = ptr;
  else if (length> 0)
    m_buffer = new tagChar[length];
  else m_buffer = NULL;
  m_length = length;
}

tagChar* tagArray::buffer() {
  return m_buffer;
}

tagChar& tagArray::operator[](int pos) {
  if (pos < 0)
    qDebug("Negative array access\n");
  if (pos >= m_length)
    qDebug("Exceed array index\n");
  return m_buffer[pos];
}

QTTerm::QTTerm(QWidget *parent, const char *name) : 
  QWidget(parent), cursorOn(false), m_scrollback(1000), m_history_lines(0), 
  m_cursor_x(0), m_cursor_y(0), m_clearall(true), m_blink_skip(true),
  blinkEnable(true), m_scrolling(false), m_scroll_offset(0), m_mousePressed(false),
  m_firsttime(true), selectionStart(0), selectionStop(0), m_scrollmin(0), m_scrollmax(0),
  m_scrollline(1), m_scrollpage(0), m_surface_initialized(false), m_history_initialized(false)
{
  m_scrollbar = new QScrollBar(Qt::Vertical,this);
  m_scrollbar->setRange(0,0);
  QObject::connect(m_scrollbar,SIGNAL(valueChanged(int)), this, SLOT(scrollBack(int)));
  adjustScrollbarPosition();
  setAttribute(Qt::WA_OpaquePaintEvent);
  setFocusPolicy(Qt::ClickFocus);  
  InstallEventTimers();
  setFont(12);
}

QTTerm::~QTTerm() {
}

char* QTTerm::getSelectionText() {
  int start;
  int stop;
  // selection tags are relative to the start of the buffer...
  // we need to map the selection pointers to the buffer
  if (selectionStart == selectionStop) {
    return NULL;
  }
  start = selectionStart - (m_scrollback - m_height - m_history_lines)*m_width;
  stop = selectionStop - (m_scrollback - m_height - m_history_lines)*m_width;
  if (start == stop) 
    return NULL; // No-op
  if (start > stop) {
    int tmp;
    tmp = start;
    start = stop;
    stop = tmp;
  }
  int history_count;
  int width;
  char *textbuffer =  getTextSurface(history_count, width);
  // Map the selection to a row/column for start
  int startrow, startcol;
  int stoprow, stopcol;
  startrow = start/width;
  startcol = start%width;
  stoprow = stop/width;
  stopcol = stop%width;
  // Initialize the copy text buf with enough space to hold the selection
  char *copytextbuf = (char*) malloc((width+2)*(stoprow-startrow+1));
  char *cp = copytextbuf;
  for (int i=startrow;i<=stoprow;i++) {
    int jmin, jmax;
    jmin = 0;
    jmax = width-1;
    if (i==startrow)
      jmin = startcol;
    if (i==stoprow)
      jmax = stopcol;
    int j = jmax;
    while ((j>jmin) && (textbuffer[i*width+j] == ' '))
      j--;
    j++;
    for (int k=jmin;k<j;k++)
      *cp++ = textbuffer[i*width+k];
    if (i != stoprow) {
#ifdef WIN32
      *cp++ = '\r';
#endif
      *cp++ = '\n';
    }
  }
  *cp++ = 0;
  return copytextbuf;
}

char* QTTerm::getTextSurface(int &count, int &width) {
  if (m_history_lines == 0)
    count = m_cursor_y;
  else
    count = m_height + m_history_lines;
  width = m_width;
  char *outptr = new char[width*count];
  tagChar *ptr = m_history.buffer() + (m_scrollback - m_height - m_history_lines)*m_width;
  for (int i=0;i<width*count;i++)
    outptr[i] = ptr[i].v;
  return outptr;
}

void QTTerm::adjustScrollbarPosition() {
  m_scrollbar->hide();
  m_scrollbar->resize(QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent),
		      contentsRect().height());
  m_active_width = width() - 2 - m_scrollbar->width();
  m_scrollbar->move(QPoint(width()-m_scrollbar->width(),0));
  m_scrollbar->show();
}
 
 
void QTTerm::refresh() {
  DrawContent();
}

void QTTerm::SetScrollBarValue(int val) {
  m_scrollbar->setValue(val);
}

void QTTerm::SetupScrollBar(int minval, int maxval, int step, int page, int val) {
  m_scrollbar->setRange(minval,maxval);
  if (maxval == minval)
    m_scrollbar->setPageStep(0);
  else
    m_scrollbar->setPageStep(page);
  m_scrollbar->setValue(val);
}

void QTTerm::ScrollLineUp() {
  m_scrollbar->setValue(qMax(0,m_scrollbar->value()-1));
}

void QTTerm::ScrollLineDown() {
  m_scrollbar->setValue(m_scrollbar->value()+1);
}

int QTTerm::GetWidth() {
  return width()-m_scrollbar->width();
}

int QTTerm::GetHeight() {
  return height();
}

void QTTerm::InstallEventTimers() {
  m_timer_refresh = new QTimer;
  QObject::connect(m_timer_refresh, SIGNAL(timeout()), this, SLOT(refresh()));
  m_timer_refresh->start(30);
  m_timer_blink = new QTimer;
  QObject::connect(m_timer_blink, SIGNAL(timeout()), this, SLOT(blink()));
  m_timer_blink->start(1000);
}


void QTTerm::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);
  m_active_width = GetWidth();
  resizeTextSurface();
  adjustScrollbarPosition();
  update();
}


// Paint the given rectangle... We do this by erasing it
void QTTerm::PaintRectangle(QPainter *paint, QRect rect) {
  paint->setBackground(QBrush(Qt::white));
  paint->eraseRect(rect);
  int col_start, col_stop;
  int row_start, row_stop;
  col_start = (int)floor(rect.left()/m_char_w);
  col_stop = (int)ceil(rect.right()/m_char_w);
  col_stop = qMin(col_stop,m_width-1);
  row_start = (int)ceil(rect.top()/m_char_h);
  row_stop = (int)floor(rect.bottom()/m_char_h);
  row_stop = qMin(row_stop,m_height-1);


  // Need to optimize this code to draw faster...

  // Drawing text is _painfully_ slow especially on the Mac.
  // The easiest way to accellerate is line by line.  But for
  // the _best_ performance, I think rectangular blocks are
  // better.  Still, my tests with the row-at-a-time
  // version show it is pretty good (and harder to get wrong).
  // DrawFragment could provide some caching or block building
  // later on...
  for (int row=row_start;row<=row_stop;row++) {
    QString todraw;
    int col = col_start;
    char gflags = 0;
    int frag_start = col;
    for (int col=col_start;col<=col_stop;col++) {
      tagChar g(m_onscreen[row*m_width+col]);
      if (g.mflags() != gflags) {
	DrawFragment(paint,todraw,gflags,row,frag_start);
	gflags = g.mflags();
	frag_start = col;
	todraw.clear();
	todraw.append(g.v);
      } else
	todraw.append(g.v);
    }
    DrawFragment(paint,todraw,gflags,row,frag_start);
  }
}

void QTTerm::DrawFragment(QPainter *paint, QString todraw, char flags, int row, int col) {
  if (todraw.size() == 0) return;
  QRect txtrect(col*m_char_w,row*m_char_h,(col+todraw.size())*m_char_w,(row+1)*m_char_h);
  if (flags == 0) {
    paint->setPen(Qt::black);
    paint->setBackground(QBrush(Qt::white));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  } else if (flags & CURSORBIT) {
    paint->setPen(Qt::white);
    paint->setBackground(QBrush(Qt::black));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  } else {
    paint->setPen(Qt::white);
    paint->setBackground(QBrush(Qt::blue));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  }
}

void QTTerm::paintEvent(QPaintEvent *e) {
  QVector<QRect> rects = e->region().rects();
  QPainter painter(this);
  painter.setFont(QWidget::font());
  painter.setPen(Qt::black);
  //  painter.setRenderHint(QPainter::TextAntialiasing);
  //  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBackground(QBrush(Qt::white));
  // Loop over the regions in the paint event
  //  for ( uint i = 0; i < rects.size(); i++ ) 
  //    PaintRectangle(&painter,rects[i]);
  PaintRectangle(&painter,e->region().boundingRect());
}

// Just marks the area as needing a repaint
void QTTerm::PutTagChar(int x, int y, tagChar g) {
  QRect txtrect(x,y,m_char_w+1,m_char_h);
  update(txtrect);
}

#ifndef __APPLE__
#define CTRLKEY(x)  else if ((keycode == x) && (e->modifiers() & Qt::ControlModifier))
#else
#define CTRLKEY(x)  else if ((keycode == x) && (e->modifiers() & Qt::MetaModifier))
#endif

bool QTTerm::event(QEvent *e) {
  if (e->type() == QEvent::KeyPress) {
    QKeyEvent* ke = static_cast<QKeyEvent *>(e);
    if (ke->key() == Qt::Key_Tab) {
      emit OnChar(KM_TAB);
      return true;
    }
  }
  return QWidget::event(e);
}

void QTTerm::keyPressEvent(QKeyEvent *e) {
  int keycode = e->key(); 
  if (!keycode) return;
  if (keycode == Qt::Key_Left)
    emit OnChar(KM_LEFT);
  CTRLKEY('Z')
    emit OnChar(KM_CTRLC);
  CTRLKEY('A')
    emit OnChar(KM_CTRLA);
  CTRLKEY('D')
    emit OnChar(KM_CTRLD);
  CTRLKEY('E')
    emit OnChar(KM_CTRLE);
  CTRLKEY('K')
    emit OnChar(KM_CTRLK);
  CTRLKEY('Y')
    emit OnChar(KM_CTRLY);
  else if (keycode == Qt::Key_Right)
    emit OnChar(KM_RIGHT);
  else if (keycode == Qt::Key_Up)
    emit OnChar(KM_UP);
  else if (keycode == Qt::Key_Down)
    emit OnChar(KM_DOWN);
  else if (keycode == Qt::Key_Delete)
    emit OnChar(KM_DELETE);
  else if (keycode == Qt::Key_Insert)
    emit OnChar(KM_INSERT);
  else if (keycode == Qt::Key_Home)
    emit OnChar(KM_HOME);
  else if (keycode == Qt::Key_End)
    emit OnChar(KM_END);
  else if (keycode == Qt::Key_Return)
    emit OnChar(KM_NEWLINE);
  else if (keycode == Qt::Key_Backspace)
    emit OnChar(KM_BACKSPACE);
  else {
    QByteArray p(e->text().toAscii());
    char key;
    if (!e->text().isEmpty())
      key = p[0];
    else
      key = 0;
    if (key) {
      emit OnChar(key);
      e->accept();
    } else
      e->ignore();
  }
}

void QTTerm::setFont(QFont font) {
  QWidget::setFont(font);
  QFontMetrics fmi(font);
  m_char_w = fmi.width("w");
  m_char_h = fmi.height();
  resizeTextSurface();
}

QFont QTTerm::getFont() {
  return QWidget::font();
}

void QTTerm::setFont(int size) {
#ifdef __APPLE__
  QFont afont("Monaco",size);
#elif WIN32
  QFont afont("Lucida Console",size);
#else
  QFont afont("Monospace",size);
#endif
  setFont(afont);
}


void QTTerm::mousePressEvent( QMouseEvent *e ) {
  // Get the x and y coordinates of the mouse click - map that
  // to a row and column
  int clickcol = e->x()/m_char_w;
  int clickrow = e->y()/m_char_h;
  selectionStart = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStart = qMin(qMax(0,selectionStart),m_width*m_scrollback-1);
  selectionStop = selectionStart;
}

void QTTerm::mouseMoveEvent( QMouseEvent *e ) {
  int x = e->x();
  int y = e->y();
  if (y < 0) 
    ScrollLineUp();
  if (y > GetHeight())
    ScrollLineDown();
  // Get the position of the click
  // to a row and column
  int clickcol = x/m_char_w;
  int clickrow = y/m_char_h;
  selectionStop = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStop = qMin(qMax(0,selectionStop),m_width*m_scrollback-1);
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

void QTTerm::mouseReleaseEvent( QMouseEvent *e ) {
  QClipboard *cb = QApplication::clipboard();
  if (!cb->supportsSelection())
    return;
  char *copytextbuf = getSelectionText();
  if (!copytextbuf) return;
  cb->setText(copytextbuf, QClipboard::Selection);
  free(copytextbuf);  
}

void QTTerm::focusOutEvent(QFocusEvent *e) {
  QWidget::focusOutEvent(e);
  DisableBlink();
  m_timer_blink->stop();
  m_timer_refresh->setInterval(1000);
}

void QTTerm::focusInEvent(QFocusEvent *e) {
  QWidget::focusInEvent(e);
  EnableBlink();
  m_timer_blink->start(1000);
  m_timer_refresh->setInterval(30);
}

void QTTerm::MoveDown() {
  SetCursor(m_cursor_x,m_cursor_y+1);
}

void QTTerm::MoveUp() {
  SetCursor(m_cursor_x,m_cursor_y-1);
}

void QTTerm::MoveLeft() {
  SetCursor(m_cursor_x-1,m_cursor_y);
}

void QTTerm::MoveRight() {
  SetCursor(m_cursor_x+1,m_cursor_y);
}

void QTTerm::MoveBOL() {
  SetCursor(0,m_cursor_y);
}

void QTTerm::ClearEOL() {
  for (int i=m_cursor_x;i<m_width;i++)
    m_surface[i + m_cursor_y*m_width] = tagChar(' ');
}

void QTTerm::ClearEOD() {
  for (int i=m_cursor_x + m_cursor_y*m_width;i<m_width*m_height;i++)
    m_surface[i] = tagChar(' ');
}

void QTTerm::OutputRawString(std::string txt) {
  PutString(txt);
}

void QTTerm::EnableBlink() {
  blinkEnable = true;
}

void QTTerm::DisableBlink() {
  if (m_surface[m_cursor_y*m_width+m_cursor_x].cursor())
    m_surface[m_cursor_y*m_width+m_cursor_x].toggleCursor();
  blinkEnable = false;
}

void QTTerm::Initialize() {
  setFont(12);
  m_active_width = 400;
  resizeTextSurface();
}

void QTTerm::scrollBack(int val) {
  if (m_history_lines == 0) return;
  m_scrolling =  (val != m_history_lines);
  m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();
  m_surface = tagArray(m_width*m_height,
		       m_history.buffer()+(m_scrollback-m_height-(m_history_lines - val))*m_width);
  m_scroll_offset = (m_scrollback - m_height - (m_history_lines - val))*m_width;
}

void QTTerm::blink() {
  if (!blinkEnable) return;
  if (m_blink_skip) {
    m_blink_skip = false;
    return;
  }
  m_surface[m_cursor_x + m_cursor_y*m_width].toggleCursor();
}

void QTTerm::resizeTextSurface() {
  if (!m_char_w || !m_char_h) return;
  if (m_surface_initialized)
    m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();  
  m_surface_initialized = true;
  int cursor_offset = m_height - 1 - m_cursor_y;
  //  m_timer_refresh->start(30,false);
  //  m_timer_blink->start(1000);
  m_blink_skip = true;
  int new_width = m_active_width/m_char_w;
  int new_height = GetHeight()/m_char_h;
  if (new_width < 5) new_width = 5;
  if (new_height < 5) new_height = 5;
  tagArray new_history(new_width*m_scrollback);
  
  if (m_history_initialized) {
    int minwidth = qMin(new_width,m_width);
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
  
  //  delete[] m_onscreen;
  //  delete[] m_history;
  m_onscreen = tagArray(new_width*new_height);
  m_history = new_history;
  m_history_initialized = true;
  m_surface = tagArray(new_height*new_width,
		       m_history.buffer()+(m_scrollback - new_height)*new_width);
  
  if (!m_firsttime && m_history_lines) {
    m_history_lines -= (new_height-m_height);
    m_history_lines = qMax(0,m_history_lines);
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
  emit SetTextWidth(m_width);
}

void QTTerm::setScrollbar(int val) {
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

void QTTerm::PutString(std::string txt) {
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

void QTTerm::SetCursor(int x, int y) {
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
    m_history_lines = qMin(m_history_lines+toscroll,m_scrollback-m_height);
    m_cursor_y -= toscroll;
    setScrollbar(m_history_lines);
  }
  m_surface[m_cursor_y*m_width+m_cursor_x].setCursor();
}

void QTTerm::DrawContent() {
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
}


