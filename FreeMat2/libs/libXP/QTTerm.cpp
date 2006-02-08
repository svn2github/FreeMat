#include "QTTerm.hpp"
#include "KeyManager.hpp"
#include <qapplication.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qclipboard.h>
#include <math.h>
#include <QKeyEvent>
#include <QDebug>

QTTerm::QTTerm(QWidget *parent, const char *name) : 
  QWidget(parent) {
  m_scrollbar = new QScrollBar(Qt::Vertical,this);
  m_scrollbar->setRange(0,0);
  QObject::connect(m_scrollbar,SIGNAL(valueChanged(int)), this, SLOT(scrollBack(int)));
  adjustScrollbarPosition();
  setFont(12);
  cursorOn = false;
  blinkEnable = true;
  //  setBackgroundRole(QPalette::Base);
  setAttribute(Qt::WA_OpaquePaintEvent);
  //  setAttribute(Qt::WA_NoBackground);
  setFocusPolicy(Qt::ClickFocus);  
  InstallEventTimers();
}

QTTerm::~QTTerm() {
}

void QTTerm::Erase() {
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
  tagChar *ptr = m_history + (m_scrollback - m_height - m_history_lines)*m_width;
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
 
 
void QTTerm::blink() {
  TermWidget::blink();
}

void QTTerm::refresh() {
  DrawContent();
}

void QTTerm::scrollBack(int val) {
  TermWidget::scrollBack(val);
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
  OnResize();
  adjustScrollbarPosition();
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

//   QString todraw;
//   for (int row=row_start;row<=row_stop;row++) {
//     for (int col=col_start;col<=col_stop;col++) {
//       tagChar g(m_onscreen[row*m_width+col]);
//       todraw.append(g.v);
//     }
//     todraw.append('\n');
//   }
//   QRect txtrect(col_start*m_char_w,row_start*m_char_h,(col_stop-col_start+1)*m_char_w,
// 		(row_stop-row_start+1)*m_char_h);
//   paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
    
  for (int row=row_start;row<=row_stop;row++) {
    for (int col=col_start;col<col_stop;col++) {
      tagChar g(m_onscreen[row*m_width+col]);
      int x = col*m_char_w;
      int y = row*m_char_h;
      QString todraw;
      todraw.append(g.v);
      QRect txtrect(x,y,m_char_w+1,m_char_h);
      if (g.noflags()) {
	paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
      } else if (g.cursor()) {
	paint->setPen(Qt::white);
	paint->setBackground(QBrush(Qt::black));
	paint->eraseRect(txtrect);
	paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
	paint->setPen(Qt::black);
	paint->setBackground(QBrush(Qt::white));
      } else {
	paint->setPen(Qt::white);
	paint->setBackground(QBrush(Qt::blue));
	paint->eraseRect(txtrect);
	paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
	paint->setPen(Qt::black);
	paint->setBackground(QBrush(Qt::white));
      }
    }
  }
      //      DrawTagChar(paint,col*m_char_w,row*m_char_h,m_onscreen[row*m_width+col]);
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

void QTTerm::BeginDraw() {
//   paint = new QPainter(&surface);
//   paint->setFont(QWidget::font());
//   paint->setPen(Qt::black);
//   paint->setRenderHint(QPainter::TextAntialiasing);
//   paint->setRenderHint(QPainter::Antialiasing);
//   paint->setBackground(QBrush(Qt::white));
}

void QTTerm::EndDraw() {
//   delete paint;
}

void QTTerm::DrawTagChar(QPainter *paint, int x, int y, tagChar g) {
  QString todraw;
  todraw.append(g.v);
  QRect txtrect(x,y,m_char_w+1,m_char_h);
  if (g.noflags()) {
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  } else if (g.cursor()) {
    paint->setPen(Qt::white);
    paint->setBackground(QBrush(Qt::black));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
    paint->setPen(Qt::black);
    paint->setBackground(QBrush(Qt::white));
  } else {
    paint->setPen(Qt::white);
    paint->setBackground(QBrush(Qt::blue));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
    paint->setPen(Qt::black);
    paint->setBackground(QBrush(Qt::white));
  }
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

void QTTerm::keyPressEvent(QKeyEvent *e) {
  int keycode = e->key(); 
  if (!keycode) return;
  if (keycode == Qt::Key_Left)
    ProcessChar(KM_LEFT);
  CTRLKEY('Z')
    ProcessChar(KM_CTRLC);
  CTRLKEY('A')
    ProcessChar(KM_CTRLA);
  CTRLKEY('D')
    ProcessChar(KM_CTRLD);
  CTRLKEY('E')
    ProcessChar(KM_CTRLE);
  CTRLKEY('K')
    ProcessChar(KM_CTRLK);
  CTRLKEY('Y')
    ProcessChar(KM_CTRLY);
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
  else if (keycode == Qt::Key_Return)
    ProcessChar(KM_NEWLINE);
  else if (keycode == Qt::Key_Backspace)
    ProcessChar(KM_BACKSPACE);
  else {
    QByteArray p(e->text().toAscii());
    char key;
    if (!e->text().isEmpty())
      key = p[0];
    else
      key = 0;
    if (key) {
      ProcessChar(key);
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
  OnMouseDown(e->x(),e->y());
}

void QTTerm::mouseMoveEvent( QMouseEvent *e ) {
  if (e->buttons())
    OnMouseDrag(e->x(),e->y());
}

void QTTerm::mouseReleaseEvent( QMouseEvent *e ) {
  OnMouseUp(e->x(),e->y());
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
