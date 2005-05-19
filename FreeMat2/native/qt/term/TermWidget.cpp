#include "TermWidget.h"
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


TermWidget::TermWidget(QWidget *parent, const char *name) : 
  QFrame(parent,name), pm_cursor(50,50) {
  m_surface = NULL;
  m_onscreen = NULL;
  m_history = NULL;
  m_scrollback = 1000;
  m_history_lines = 0;
  m_cursor_x = 0;
  m_cursor_y = 0;
  m_clearall = true;
  m_scrollbar = new QScrollBar(Qt::Vertical,this);
  m_scrollbar->setRange(0,100);
  m_timer_refresh = new QTimer;
  QObject::connect(m_timer_refresh, SIGNAL(timeout()), this, SLOT(refresh()));
  m_timer_blink = new QTimer;
  QObject::connect(m_timer_blink, SIGNAL(timeout()), this, SLOT(blink()));
  adjustScrollbarPosition();
  setFont(12);
  pm_cursor.fill(Qt::white);
  cursorOn = false;
  blinkEnable = true;
  setBackgroundColor(Qt::white);
  //  buffer.fill(colorGroup().base());
}

TermWidget::~TermWidget() {
}

void TermWidget::toggleCursor() {
  //  cursorOn = !cursorOn;
  //  bitBlt(this,m_cursor_x*m_char_w,m_cursor_y*m_char_h,&pm_cursor,0,0,m_char_w,m_char_h,Qt::XorROP,TRUE);
}

void TermWidget::blink() {
  if (!blinkEnable) return;
  m_surface[m_cursor_x + m_cursor_y*m_width] ^= 128;
  //  toggleCursor();
//   if (!cursorOn)
//     m_onscreen[m_cursor_x + m_cursor_y*m_width] = -1;
//   repaint(cursorRect,true);
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
  blinkEnable = false;
  m_timer_refresh->start(30,false);
  m_timer_blink->start(1000);
  int new_width = m_active_width/m_char_w;
  int new_height = height()/m_char_h;
  char *new_surface = new char[new_width*new_height];
  memset(new_surface,' ',new_width*new_height*sizeof(char));
  char *new_history = new char[new_width*m_scrollback];
  memset(new_history,' ',new_width*m_scrollback*sizeof(char));
  // Copy old surface to new surface here
  // Copy old history to new history here
  delete[] m_onscreen;
  delete[] m_surface;
  delete[] m_history;
  m_onscreen = new char[new_width*new_height];
  memset(m_onscreen,' ',new_width*new_height*sizeof(char));
  m_history = new_history;
  m_surface = new_surface;
  m_width = new_width;
  m_height = new_height;
  m_clearall = true;
  blinkEnable = true;
  // only do this the first time
  setCursor(0,0);
}

void TermWidget::OutputString(std::string txt) {
  for (int i=0;i<txt.size();i++) {
    if (txt[i] == '\n' || txt[i] == '\r') {
      setCursor(0,m_cursor_y+1);
    } else {
      m_surface[m_cursor_x + m_cursor_y*m_width] = txt[i];
      setCursor(m_cursor_x+1,m_cursor_y);
    }
  }
  m_timer_blink->start(1000);
}

void TermWidget::ProcessChar(char c) {
  if (c == 'q')
    exit(0);
  if (c == 'l') {
    setCursor(QMAX(m_cursor_x-1,0),m_cursor_y);
    return;
  }
  if (c != 'd') {
    char buffer[2];
    buffer[0] = c;
    buffer[1] = 0;
    OutputString(buffer);
  } else 
    OutputString("Now is the time for all men to come to the aid of their country, and by their aid, assist those who need it, or something like that....");
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
	if ((m_surface[i*m_width+j] & 128) == 0) {
	  todraw.append(m_surface[i*m_width+j]);
	  paint.eraseRect(txtrect);
	  paint.drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
	} else {
	  todraw.append(m_surface[i*m_width+j] ^ 128);
	  paint.setPen(Qt::white);
	  paint.setBackgroundColor(Qt::black);
	  paint.eraseRect(txtrect);
	  paint.drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
	  //	  paint.setPen(Qt::black);
	  //	  paint.setBackgroundColor(Qt::white);
	  paint.setPen(Qt::black);
	  paint.setBackgroundColor(Qt::white);
	}
	m_onscreen[i*m_width+j] = m_surface[i*m_width+j];
	j++;
      }
    }
  }
}
#if 0
      // We have found a difference
      QString todraw;
      bool emptystring = true;
      int x0 = j*m_char_w;
      int y0 = i*m_char_h;
      while ((j < m_width) && 
	     (m_onscreen[i*m_width+j] != m_surface[i*m_width+j])) {
	todraw.append(m_surface[i*m_width+j]);
	m_onscreen[i*m_width+j] = m_surface[i*m_width+j];
	j++;
	emptystring = false;
      }
      if (!emptystring) {
	QRect txtrect(x0,y0,todraw.length()*m_char_w,m_char_h);
	erase(txtrect);
	paint.drawText(txtrect,Qt::AlignLeft | Qt::AlignTop, todraw);
      }
    }
//     if (cursorOn && cursorEnable) {
//       QBrush brush(black);
//       paint.fillRect(cursorRect,brush);
//     }
  }
#endif


void TermWidget::setCursor(int x, int y) {
//   cursorOn = false;
//   m_onscreen[m_cursor_x + m_cursor_y*m_width] = -1;
//   repaint(cursorRect,true);
//   cursorEnable = false;
  if ((m_surface[m_cursor_y*m_width+m_cursor_x] & 128) != 0)
    m_surface[m_cursor_y*m_width+m_cursor_x] ^= 128;
  m_cursor_x = x;
  m_cursor_y = y;
  m_cursor_y += m_cursor_x/m_width;
  m_cursor_x %= m_width;
  if (m_cursor_y >= m_height) {
    // scroll up
    int toscroll = m_cursor_y - m_height + 1;
    memcpy(m_history+m_history_lines*m_width,
	   m_surface,toscroll*m_width*sizeof(char));
    m_history_lines += toscroll;
    memmove(m_surface,m_surface+toscroll*m_width*sizeof(char),
	    (m_height-toscroll)*m_width*sizeof(char));
    memset(m_surface+(m_height-toscroll)*m_width*sizeof(char),
	   ' ',toscroll*m_width*sizeof(char));
    m_cursor_y -= toscroll;
  }
  m_surface[m_cursor_y*m_width+m_cursor_x] |= 128;
}

void TermWidget::keyPressEvent(QKeyEvent *e) {
  ProcessChar(e->ascii());
}

void TermWidget::setFont(int size) {
  QFont afont("Courier",size);
  QFrame::setFont(afont);
  QFontMetrics fmi(afont);
  m_char_w = fmi.width("w");
  m_char_h = fmi.height();
}


int main(int argc, char **argv) {
  QApplication app(argc, argv, TRUE);
  TermWidget *win = new TermWidget();
  win->resize(400,300);
  win->show();
  return app.exec();
}
