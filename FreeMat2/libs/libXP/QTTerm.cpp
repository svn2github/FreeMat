#include "QTTerm.hpp"
#include "KeyManager.hpp"
#include <qapplication.h>
#include <qstyle.h>
#include <qpainter.h>
#include <qclipboard.h>
#include <math.h>
#ifndef QT3
#include <QKeyEvent>
#include <Q3MemArray>
#endif

QTTerm::QTTerm(QWidget *parent, const char *name) : 
#ifdef QT3
  QWidget(parent,name,WRepaintNoErase) {
#else
  QWidget(parent,name,Qt::WNoAutoErase) {
#endif
  m_scrollbar = new QScrollBar(Qt::Vertical,this);
  m_scrollbar->setRange(0,0);
  QObject::connect(m_scrollbar,SIGNAL(valueChanged(int)), this, SLOT(scrollBack(int)));
  adjustScrollbarPosition();
  setFont(12);
  cursorOn = false;
  blinkEnable = true;
#ifdef QT3
  setBackgroundColor(Qt::white);
#else
  setBackgroundRole(QPalette::Base);
  //  setAttribute(Qt::WA_NoBackground);
#endif
#ifdef QT3
  setFocusPolicy(QWidget::ClickFocus);
#else
  setFocusPolicy(Qt::ClickFocus);  
#endif
  InstallEventTimers();
}
#if 0
}
#endif

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
#ifdef QT3
#ifdef __APPLE__
  m_scrollbar->resize(QApplication::style().pixelMetric(QStyle::PM_ScrollBarExtent),
		      contentsRect().height() - 
		      QApplication::style().pixelMetric(QStyle::PM_ScrollBarExtent));
#else
  m_scrollbar->resize(QApplication::style().pixelMetric(QStyle::PM_ScrollBarExtent),
		      height());
#endif
#else
#ifdef __APPLE__
  m_scrollbar->resize(QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent),
		      contentsRect().height() - 
		      QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent));
#else
  m_scrollbar->resize(QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent),
		      contentsRect().height());
#endif
#endif
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
#ifdef QT3
  if (maxval == minval)
    m_scrollbar->setSteps(0,0);
  else
    m_scrollbar->setSteps(step,page);
#else
  if (maxval == minval)
    m_scrollbar->setPageStep(0);
  else
    m_scrollbar->setPageStep(page);
#endif
  m_scrollbar->setValue(val);
}

void QTTerm::ScrollLineUp() {
  m_scrollbar->setValue(QMAX(0,m_scrollbar->value()-1));
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
  m_timer_refresh->start(30,false);
  m_timer_blink = new QTimer;
  QObject::connect(m_timer_blink, SIGNAL(timeout()), this, SLOT(blink()));
  m_timer_blink->start(1000);
}


void QTTerm::resizeEvent(QResizeEvent *e) {
  surface = QPixmap(width(),height());
  QPainter *paint = new QPainter(&surface);
  paint->fillRect(0,0,width(),height(),QBrush(Qt::white));
  delete paint;
  QWidget::resizeEvent(e);
  OnResize();
  adjustScrollbarPosition();
}


void QTTerm::paintEvent(QPaintEvent *e) {
  // QWidget::paintEvent(e);
#ifndef QT3
  Q3MemArray<QRect> rects = e->region().rects();
#else
  QMemArray<QRect> rects = e->region().rects();
#endif
  QPainter painter(this);
  for ( uint i = 0; i < rects.count(); i++ ) {
#ifndef QT3
    painter.drawPixmap(rects[(int) i],surface,rects[(int) i]);
#else
    painter.drawPixmap(QPoint(rects[(int) i].left(),rects[(int) i].top()),surface,rects[(int) i]);
#endif
  }
}

void QTTerm::BeginDraw() {
  paint = new QPainter(&surface);
  paint->setFont(QWidget::font());
  paint->setPen(Qt::black);
#ifndef QT3
  paint->setBackground(QBrush(Qt::white));
#endif
  paint->setBackgroundMode(Qt::OpaqueMode);
}

void QTTerm::EndDraw() {
  delete paint;
}

void QTTerm::PutTagChar(int x, int y, tagChar g) {
  char buffer[1000];
  QString todraw;
  todraw.append(g.v);
  QRect txtrect(x,y,m_char_w,m_char_h);
  update(txtrect);
  if (g.noflags()) {
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  } else if (g.cursor()) {
    paint->setPen(Qt::white);
#ifdef QT3
    paint->setBackgroundColor(Qt::black);
#else
    paint->setBackground(QBrush(Qt::black));
#endif
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
    paint->setPen(Qt::black);
#ifdef QT3
    paint->setBackgroundColor(Qt::white);
#else
    paint->setBackground(QBrush(Qt::white));
#endif
  } else {
    paint->setPen(Qt::white);
#ifdef QT3
    paint->setBackgroundColor(Qt::blue);
#else
    paint->setBackground(QBrush(Qt::blue));
#endif
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
    paint->setPen(Qt::black);
#ifdef QT3
    paint->setBackgroundColor(Qt::white);
#else
    paint->setBackground(QBrush(Qt::white));
#endif
  }
}

void QTTerm::keyPressEvent(QKeyEvent *e) {
  //  if (m_scrolling) 
  //    TermWidget::setScrollbar(m_history_lines);
  int keycode = e->key();
  if (!keycode) return;
  if (keycode == Qt::Key_Left)
    ProcessChar(KM_LEFT);
#ifdef QT3
  else if ((keycode == 'Z') && (e->state() && Qt::ControlButton))
#else
  else if ((keycode == 'Z') && (e->modifiers() && Qt::ControlModifier))
#endif
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
  else if (keycode == Qt::Key_Return)
    ProcessChar(KM_NEWLINE);
  else if (keycode == Qt::Key_Backspace)
    ProcessChar(KM_BACKSPACE);
  else {
#ifdef QT3
    char key = e->ascii();
#else
    QByteArray p(e->text().toAscii());
    char key;
    if (!p.isEmpty())
      key = p.at(0);
    else
      key = 0;
#endif
    if (key) ProcessChar(key);
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
#ifndef QT3
  if (e->buttons())
#else
    if (e->state() & (LeftButton | RightButton | MidButton))
#endif
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

