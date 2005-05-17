#include "TermWidget.h"
#include <string.h>
#include <iostream>
#include <qpainter.h>
#include <qapplication.h>
#include <qstyle.h>

// Need: a scroll bar, and a cursor...

TermWidget::TermWidget(QWidget *parent, const char *name) : 
  QFrame(parent,name){
  m_surface = NULL;
  m_onscreen = NULL;
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
  setFont(11);
  cursorOn = true;
  //  buffer.fill(colorGroup().base());
}

TermWidget::~TermWidget() {
}

void TermWidget::blink() {
  cursorOn = !cursorOn;
  repaint(cursorRect,true);
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
  paintContents(paint,contentsRect(),false);
  paint.end();
}

void TermWidget::resizeTextSurface() {
  m_timer_refresh->start(30,false);
  m_timer_blink->start(1000);
  int new_width = m_active_width/m_char_w;
  int new_height = height()/m_char_h;
  char *new_surface = new char[new_width*new_height];
  memset(new_surface,' ',new_width*new_height*sizeof(char));
  // Copy old surface to new surface here
  delete[] m_onscreen;
  delete[] m_surface;
  m_onscreen = new char[new_width*new_height];
  memset(m_onscreen,' ',new_width*new_height*sizeof(char));
  m_surface = new_surface;
  m_width = new_width;
  m_height = new_height;
  m_clearall = true;
}

void TermWidget::OutputString(std::string txt) {
  m_timer_blink->start(1000);
  cursorOn = true;
  for (int i=0;i<txt.size();i++) {
    m_surface[m_cursor_x + m_cursor_y*m_width] = txt[i];
    setCursor(m_cursor_x+1,m_cursor_y);
  }
}

void TermWidget::ProcessChar(char c) {
  m_timer_blink->start(1000);
  if (c == 'q')
    exit(0);
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
  adjustScrollbarPosition();
  resizeTextSurface();
}

void TermWidget::paintEvent(QPaintEvent *e) {
  QPainter paint;
  paint.begin(this);
  QFrame::paintEvent( e );
  QMemArray<QRect> rects = e->region().rects();
  for ( uint i = 0; i < rects.count(); i++ ) {
    paintContents(paint,rects[(int) i],false);
  }
  paint.end();
}

void TermWidget::paintContents(QPainter &paint, const QRect &rect, bool pm) {
  QPoint tL  = contentsRect().topLeft();
  int    tLx = tL.x();
  int    tLy = tL.y();

  int lux = QMIN(m_width-1, QMAX(0,(rect.left()   - tLx - 1 ) / m_char_w));
  int luy = QMIN(m_height-1,   QMAX(0,(rect.top()    - tLy - 1  ) / m_char_h));
  int rlx = QMIN(m_width-1, QMAX(0,(rect.right()  - tLx - 1 ) / m_char_w));
  int rly = QMIN(m_height-1,   QMAX(0,(rect.bottom() - tLy - 1  ) / m_char_h));
  
  for (int i=luy;i<=rly;i++) {
    int j=lux;
    while (j<=rlx) {
      // skip over characters that do not need to be redrawn
      while ((j <= rlx) && 
	     (m_onscreen[i*m_width+j] == m_surface[i*m_width+j])) j++;
      // We have found a difference
      QString todraw;
      bool emptystring = true;
      int x0 = j*m_char_w;
      int y0 = i*m_char_h;
      while ((j <= rlx) && 
	     (m_onscreen[i*m_width+j] != m_surface[i*m_width+j])) {
	todraw.append(m_surface[i*m_width+j]);
	m_onscreen[i*m_width+j] = m_surface[i*m_width+j];
	j++;
	emptystring = false;
      }
      if (!emptystring) {
	erase(QRect(x0,y0,todraw.length()*m_char_w,m_char_h));
	paint.drawText(x0,y0+m_char_h,todraw);
      }
    }
    m_onscreen[m_cursor_x + m_cursor_y*m_width] = -1;
    if (cursorOn) {
      QBrush brush(black);
      paint.fillRect(cursorRect,brush);
    }
  }
}

void TermWidget::setCursor(int x, int y) {
  cursorOn = false;
  repaint(cursorRect,true);
  m_cursor_x = x;
  m_cursor_y = y;
  m_cursor_y += m_cursor_x/m_width;
  m_cursor_x %= m_width;
  m_cursor_y %= m_height;
  cursorRect = QRect(x*m_char_w,y*m_char_h,m_char_w,m_char_h);
  cursorOn = true;
  repaint(cursorRect,true);
}

void TermWidget::keyPressEvent(QKeyEvent *e) {
  ProcessChar(e->ascii());
}

void TermWidget::setFont(int size) {
  QFont afont("Courier",size);
  QFrame::setFont(afont);
  QFontMetrics fmi(afont);
  m_char_w = fmi.maxWidth();
  m_char_h = size;
}


int main(int argc, char **argv) {
  QApplication app(argc, argv, TRUE);
  TermWidget *win = new TermWidget();
  win->resize(400,300);
  win->show();
  return app.exec();
}
