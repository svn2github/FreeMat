#include "TermWidget.hpp"
#include <string.h>
#include <iostream>
#include <qpainter.h>
#include <qapplication.h>
#include <qstyle.h>
#include <math.h>

// Need: a scroll bar, and a cursor...
// To be perfectly consistent, the cursor must be drawn by the
// paintContents code.  What does the cursor represent?  It represents
// a single character whose attribute is different from all other
// characters.... one way to handle this is to set a bit in the 


// History - suppose we start with a long rectangle of text
//      
//
//  Need copy & paste & select

TermWidget::TermWidget(QWidget *parent, const char *name) : 
  QFrame(parent,name) {
  m_surface = NULL;
  m_onscreen = NULL;
  m_history = NULL;
  m_scrollback = 1000;
  m_history_lines = 0;
  m_cursor_x = 0;
  m_cursor_y = 0;
  m_clearall = true;
  m_scrollbar = new QScrollBar(Qt::Vertical,this);
  m_scrollbar->setRange(0,0);
  m_timer_refresh = new QTimer;
  QObject::connect(m_timer_refresh, SIGNAL(timeout()), this, SLOT(refresh()));
  m_timer_blink = new QTimer;
  QObject::connect(m_timer_blink, SIGNAL(timeout()), this, SLOT(blink()));
  QObject::connect(m_scrollbar,SIGNAL(valueChanged(int)), this, SLOT(scrollBack(int)));
  adjustScrollbarPosition();
  setFont(12);
  cursorOn = false;
  blinkEnable = true;
  setBackgroundColor(Qt::white);
  m_scrolling = false;
  m_scroll_offset = 0;
  m_mousePressed = false;
  setFocusPolicy(QWidget::ClickFocus);
  selectionStart = 0;
  selectionStop = 0;
}

TermWidget::~TermWidget() {
}

void TermWidget::getSelection(int& start, int& stop) {
  // selection tags are relative to the start of the buffer...
  // we need to map the selection pointers to the buffer
  if (selectionStart == selectionStop) {
    start = stop = 0;
    return;
  }
  start = selectionStart - (m_scrollback - m_height - m_history_lines)*m_width;
  stop = selectionStop - (m_scrollback - m_height - m_history_lines)*m_width;
}

char* TermWidget::getTextSurface(int &count, int &width) {
  if (m_history_lines == 0)
    count = m_cursor_y;
  else
    count = m_height + m_history_lines;
  width = m_width;
  char *outptr = new char[width*count];
  tagChar *ptr = m_history + (m_scrollback - m_height - m_history_lines)*m_width;
  for (int i=0;i<width*count;i++)
    outptr[i] = ptr[i].v;
  return outptr;
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
  m_surface[m_cursor_x + m_cursor_y*m_width].toggleCursor();
}

void TermWidget::adjustScrollbarPosition() {
  m_scrollbar->resize(QApplication::style().pixelMetric(QStyle::PM_ScrollBarExtent),
		      contentsRect().height());
  m_active_width = contentsRect().width() - 2 - m_scrollbar->width();
  m_scrollbar->move(contentsRect().topRight() - QPoint(m_scrollbar->width()-1,0));
  m_scrollbar->show();
}

void TermWidget::refresh() {
  QPainter paint;
  paint.begin(this);
  paintContents(paint);
  paint.end();
}

void TermWidget::resizeTextSurface() {
  bool firsttime = (m_surface == NULL);
  if (m_surface)
    m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();  
  int cursor_offset = m_height - 1 - m_cursor_y;
  m_timer_refresh->start(30,false);
  m_timer_blink->start(1000);
  int new_width = m_active_width/m_char_w;
  int new_height = height()/m_char_h;
  tagChar *new_history = new tagChar[new_width*m_scrollback];
  
  if (m_history) {
    int minwidth = QMIN(new_width,m_width);
    for (int i=0;i<m_scrollback;i++)
      for (int j=0;j<minwidth;j++)
	new_history[i*new_width+j] = m_history[i*m_width+j];
  }
  // Copy old history to new history here
  
  delete[] m_onscreen;
  delete[] m_history;
  m_onscreen = new tagChar[new_width*new_height];
  m_history = new_history;
  m_surface = m_history + (m_scrollback - new_height)*new_width;
  
  if (!firsttime) {
    m_history_lines -= (new_height-m_height);
    m_history_lines = QMAX(0,m_history_lines);
  }

  m_width = new_width;
  m_height = new_height;
  m_clearall = true;
  // only do this the first time
  if (firsttime) {
    setCursor(0,0);
    setScrollbar(0);
  } else {
    m_cursor_y = m_height - 1 - cursor_offset;
    setScrollbar(m_history_lines);
  }
  m_scroll_offset = (m_scrollback - m_height)*m_width;
}

void TermWidget::setScrollbar(int val) {
  m_scrollbar->setRange(0,m_history_lines);
  m_scrollbar->setSteps(1,m_height);
  m_scrollbar->setValue(val);
}

void TermWidget::PutString(std::string txt) {
  if (m_scrolling) {
    setScrollbar(m_history_lines);
  }
  for (int i=0;i<txt.size();i++) {
    if (txt[i] == '\n')
      setCursor(m_cursor_x,m_cursor_y+1);
    else if (txt[i] == '\r')
      setCursor(0,m_cursor_y);
    else {
      m_surface[m_cursor_x + m_cursor_y*m_width] = tagChar(txt[i]);
      setCursor(m_cursor_x+1,m_cursor_y);
    }
  }
  m_timer_blink->start(1000);
}

void TermWidget::resizeEvent(QResizeEvent *e) {
  QFrame::resizeEvent(e);
  QPainter paint;
  adjustScrollbarPosition();
  resizeTextSurface();
}

void TermWidget::markDirty(QRect &rect) {
  int xmin, xmax, ymin, ymax;
  xmin = (int) floor(rect.left()/((float) m_char_w));
  xmax = (int) ceil(rect.right()/((float) m_char_w));
  ymin = (int) floor(rect.top()/((float) m_char_h));
  ymax = (int) ceil(rect.bottom()/((float) m_char_h));
  xmin = QMIN(m_width-1,QMAX(0,xmin));
  xmax = QMIN(m_width-1,QMAX(0,xmax));
  ymin = QMIN(m_height-1,QMAX(0,ymin));
  ymax = QMIN(m_height-1,QMAX(0,ymax));
  for (int i=ymin;i<=ymax;i++)
    for (int j=xmin;j<=xmax;j++)
      m_onscreen[i*m_width+j] = -1;
}

void TermWidget::paintEvent(QPaintEvent *e) {
  QMemArray<QRect> rects = e->region().rects();
  for ( uint i = 0; i < rects.count(); i++ ) 
    markDirty(rects[(int) i]);
  QPainter paint;
  paint.begin(this);
  paintContents(paint);
  paint.end();
}

// Update the onscreen buffer to match the text buffer surface.
void TermWidget::paintContents(QPainter &paint) {
  for (int i=0;i<m_height;i++) {
    int j=0;
    while (j<m_width) {
      // skip over characters that do not need to be redrawn
      while ((j < m_width) && 
	     (m_onscreen[i*m_width+j] == m_surface[i*m_width+j])) j++;
      if (j < m_width) {
	QString todraw;
	QRect txtrect(j*m_char_w,i*m_char_h,m_char_w,m_char_h);
	tagChar g = m_surface[i*m_width+j];
	if (m_scrolling) 
	  g.flags &= ~CURSORBIT;
	todraw.append(g.v);
	if (g.noflags()) {
	  paint.eraseRect(txtrect);
	  paint.drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
	} else if (g.cursor()) {
	  paint.setPen(Qt::white);
	  paint.setBackgroundColor(Qt::black);
	  paint.eraseRect(txtrect);
	  paint.drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
	  paint.setPen(Qt::black);
	  paint.setBackgroundColor(Qt::white);
	} else {
	  paint.setPen(Qt::white);
	  paint.setBackgroundColor(Qt::blue);
	  paint.eraseRect(txtrect);
	  paint.drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
	  paint.setPen(Qt::black);
	  paint.setBackgroundColor(Qt::white);
	}
	m_onscreen[i*m_width+j] = g;
	j++;
      }
    }
  }
}

void TermWidget::setCursor(int x, int y) {
//   cursorOn = false;
//   m_onscreen[m_cursor_x + m_cursor_y*m_width] = -1;
//   repaint(cursorRect,true);
//   cursorEnable = false;
  if (m_scrolling) 
    setScrollbar(0);
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
    m_history_lines = QMIN(m_history_lines+toscroll,m_scrollback-m_height);
    m_cursor_y -= toscroll;
    setScrollbar(m_history_lines);
  }
  m_surface[m_cursor_y*m_width+m_cursor_x].setCursor();
}

void TermWidget::keyPressEvent(QKeyEvent *e) {
  if (m_scrolling) 
    setScrollbar(m_history_lines);
  int keycode = e->key();
  if (!keycode) return;
  if (keycode == Qt::Key_Left)
    ProcessChar(KM_LEFT);
  else if ((keycode == 'Z') && (e->state() && Qt::ControlButton))
    ProcessChar(KM_CTRLC);
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
}

void TermWidget::setFont(int size) {
#ifdef __APPLE__
  QFont afont("Monaco",size);
#else
  QFont afont("Monospace",size);
#endif
  QFrame::setFont(afont);
  QFontMetrics fmi(afont);
  m_char_w = fmi.width("w");
  m_char_h = fmi.height();
}


void TermWidget::mousePressEvent( QMouseEvent *e ) {
  m_mousePressed = true;
  // Get the x and y coordinates of the mouse click - map that
  // to a row and column
  int clickcol = e->x()/m_char_w;
  int clickrow = e->y()/m_char_h;
  selectionStart = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStart = QMIN(QMAX(0,selectionStart),m_width*m_scrollback-1);
  selectionStop = selectionStart;
}

void TermWidget::mouseMoveEvent( QMouseEvent *e ) {
  if (m_mousePressed) {
    if (e->y() < 0) 
      m_scrollbar->setValue(m_scrollbar->value()-1);
    if (e->y() > height())
      m_scrollbar->setValue(m_scrollbar->value()+1);
    // Get the position of the click
    // to a row and column
    int clickcol = e->x()/m_char_w;
    int clickrow = e->y()/m_char_h;
    selectionStop = m_scroll_offset + clickcol + clickrow*m_width;
    selectionStop = QMIN(QMAX(0,selectionStop),m_width*m_scrollback-1);
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
}

void TermWidget::mouseReleaseEvent( QMouseEvent *e ) {
  m_mousePressed = false;
}

