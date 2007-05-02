#include "term2.hpp"
#include <QtDebug>
#include <QtGui>
#include <QFontDialog>

QScreen::QScreen() {
  setMinimumSize(50,50);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  buffer << tagLine();
  cursor_x = 0;
  cursor_y = 0;
  blinkEnable = true;
  m_blink_skip = false;
  m_timer_blink = new QTimer;
  QObject::connect(m_timer_blink, SIGNAL(timeout()), this, SLOT(blink()));
  m_timer_blink->start(1000);
}

void QScreen::blink() {
  if (!blinkEnable) return;
  if (m_blink_skip) {
    m_blink_skip = false;
    return;
  }
  buffer[cursor_y].data[cursor_x].toggleCursor();
  viewport()->update();
}

void QScreen::ensureCursorVisible() {
  // For the cursor to be visible
  // the scroll bar must be at 
  // cursor_y - m_term_height + 1
  int cscroll = verticalScrollBar()->value();
  if ((cscroll < cursor_y) && 
      (cursor_y < (cscroll+m_term_height-1))) return;
  int minval = cursor_y-m_term_height+1;
  verticalScrollBar()->setValue(minval);
}

void QScreen::focusOutEvent(QFocusEvent *e) {
  QWidget::focusOutEvent(e);
  blinkEnable = false;
  buffer[cursor_y].data[cursor_x].clearCursor();  
  viewport()->update();
  m_timer_blink->stop();
}

void QScreen::focusInEvent(QFocusEvent *e) {
  QWidget::focusInEvent(e);
  buffer[cursor_y].data[cursor_x].setCursor();
  viewport()->update();
  m_timer_blink->start(1000);
  blinkEnable = true;
}

void QScreen::setChar(char t) {
  blinkEnable = false;
  buffer[cursor_y].data[cursor_x].clearCursor();
  buffer[cursor_y].data[cursor_x++].v = t;
  buffer[cursor_y].data[cursor_x].setCursor();
  if (cursor_x >= m_term_width) {
    nextLine(); 
  } else {
    ensureCursorVisible();
    viewport()->update();
  }
  blinkEnable = true;
  m_blink_skip = true;
}

void QScreen::setFont(int size) {
  fnt = QFont("Courier",size);
//   bool ok;
//   fnt = QFontDialog::getFont(&ok,fnt,this);
  calcGeometry();
}

void QScreen::nextLine() {
  blinkEnable = false;
  buffer[cursor_y].data[cursor_x].clearCursor();
  cursor_x = 0;
  cursor_y++;
  if (cursor_y >= buffer.size())
    buffer << tagLine();
  if (buffer.size() > 500) {
    buffer.pop_front();
    cursor_y--;
  } else {
    int cval = verticalScrollBar()->value();
    verticalScrollBar()->setRange(0,qMax(0,qMax(verticalScrollBar()->maximum(),buffer.size()-m_term_height)));
    verticalScrollBar()->setValue(cval);
  }
  ensureCursorVisible();
  buffer[cursor_y].data[cursor_x].setCursor();
  viewport()->update();
  blinkEnable = true;
  m_blink_skip = true;  
}

void QScreen::drawLine(int linenum, QPainter *e, int yval) {
  QString outd;
  tagLine todraw(buffer[linenum]);
  char gflags = 0;
  int frag_start = 0;
  for (int col=0;col<m_term_width;col++) {
    tagChar g(todraw.data[col]);
    if (g.mflags() != gflags) {
      drawFragment(e,outd,gflags,yval,frag_start);
      gflags = g.mflags();
      frag_start = col;
      outd.clear();
      outd.append(g.v);
    } else
      outd.append(g.v);
  }
  drawFragment(e,outd,gflags,yval,frag_start);
}

void QScreen::mousePressEvent( QMouseEvent *e ) {
  // Get the x and y coordinates of the mouse click - map that
  // to a row and column
  int clickcol = e->x()/m_char_w;
  int clickrow = e->y()/m_char_h;
  selectionStart = verticalScrollBar()->value()*m_term_width + clickcol + clickrow*m_term_width;
  selectionStart = qMax(0,selectionStart);
  selectionStop = selectionStart;
}

void QScreen::clearSelection() {
  // clear the selection bits
  for (int i=0;i<buffer.size();i++) {
    for (int j=0;j<maxlen;j++) {
      buffer[i].data[j].clearSelection();
    }
  }
}

void QScreen::mouseMoveEvent( QMouseEvent *e ) {
  if (!e->buttons())
    return;
  int x = e->x();
  int y = e->y();
  if (y < 0) 
     verticalScrollBar()->setValue(verticalScrollBar()->value()-1);
   if (y > height())
     verticalScrollBar()->setValue(verticalScrollBar()->value()+1);
  // Get the position of the click
  // to a row and column
  int clickcol = x/m_char_w;
  int clickrow = y/m_char_h;
  selectionStop = verticalScrollBar()->value()*m_term_width + clickcol + clickrow*m_term_width;
  selectionStop = qMax(0,selectionStop);

  clearSelection();

  int lSelectionStart = selectionStart;
  int lSelectionStop = selectionStop;
  if (lSelectionStart > lSelectionStop) 
    qSwap(lSelectionStop,lSelectionStart);

  int sel_row_start = lSelectionStart/m_term_width;
  int sel_col_start = lSelectionStart % m_term_width;
  int sel_row_stop = lSelectionStop/m_term_width;
  int sel_col_stop = lSelectionStop % m_term_width;

  sel_row_start = qMin(sel_row_start,buffer.size()-1);
  sel_row_stop = qMin(sel_row_stop,buffer.size()-1);

  if (sel_row_stop == sel_row_start) {
    for (int j=sel_col_start;j<sel_col_stop;j++)
      buffer[sel_row_start].data[j].setSelection();
  } else {
    for (int j=sel_col_start;j<m_term_width;j++) {
      buffer[sel_row_start].data[j].setSelection();
    }
    for (int i=sel_row_start+1;i<sel_row_stop;i++) 
      for (int j=0;j<m_term_width;j++) {
	buffer[i].data[j].setSelection();
      }
    for (int j=0;j<sel_col_stop;j++)
      buffer[sel_row_stop].data[j].setSelection();
  }
  viewport()->update();
}

void QScreen::mouseReleaseEvent( QMouseEvent *e ) {
  QClipboard *cb = QApplication::clipboard();
  if (!cb->supportsSelection())
    return;
  cb->setText(selectedText(), QClipboard::Selection);
}

void QScreen::drawFragment(QPainter *paint, QString todraw, char flags, int row, int col) {
  if (todraw.size() == 0) return;
  QRect txtrect(col*m_char_w,row*m_char_h,(col+todraw.size())*m_char_w,m_char_h);
  QPalette qp(qApp->palette());
  if (flags == 0) {
    paint->setPen(qp.color(QPalette::WindowText));
    paint->setBackground(qp.brush(QPalette::Base));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  } else if (flags & CURSORBIT) {
    paint->setPen(qp.color(QPalette::Base));
    paint->setBackground(qp.brush(QPalette::Shadow));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  } else {
    paint->setPen(qp.color(QPalette::HighlightedText));
    paint->setBackground(qp.brush(QPalette::Highlight));
    paint->eraseRect(txtrect);
    paint->drawText(txtrect,Qt::AlignLeft|Qt::AlignTop,todraw);
  }
}

void QScreen::keyPressEvent(QKeyEvent *e) {
  if (e->key() != Qt::Key_Return) {
    QString qtxt(e->text());
    if (qtxt.isEmpty()) return;
    setChar(qtxt.toStdString().c_str()[0]);
    if (qtxt == "P") {
      for (int i=0;i<1000;i++) 
	outputString(QString("bunch o text on line %1").arg(i));
    }
  } else {
    nextLine();
  }
}

void QScreen::outputString(QString txt) {
  QStringList frags(txt.split('\n'));
  for (int i=0;i<frags.size();i++) {
    std::string out(frags[i].toStdString());
    for (int j=0;j<out.size();j++)
      setChar(out[j]);
    nextLine();
  }
}

void QScreen::calcGeometry() {
  QFontMetrics fmi(fnt);
  m_char_w = fmi.width("w");
  m_char_h = fmi.height();
  m_term_width = viewport()->width()/m_char_w;
  m_term_height = viewport()->height()/m_char_h;
}

void QScreen::resizeEvent(QResizeEvent *e) {
  QAbstractScrollArea::resizeEvent(e);
  calcGeometry();
  clearSelection();
  ensureCursorVisible();
  // If we are in a full buffer situation, put the scroller in the right spot
  if (buffer.size() >= 500) {
    verticalScrollBar()->setRange(0,buffer.size()-m_term_height);
    verticalScrollBar()->setValue(cursor_y-m_term_height+1);
  } else {
    int cval = verticalScrollBar()->value();
    verticalScrollBar()->setRange(0,qMax(cval,buffer.size()-m_term_height));
    verticalScrollBar()->setValue(cval);
  }
}

void QScreen::paintEvent(QPaintEvent *e) {
  QPainter p(viewport());
  p.setFont(fnt);
  //  qDebug() << "Current font: " << p.font().toString();
//   p.setBackground(qApp->palette().brush(QPalette::Highlight));
//   p.setPen(qApp->palette().color(QPalette::HighlightedText));
  int offset = verticalScrollBar()->value();
  //  qDebug() << "offset = " << offset;
  for (int i=0;i<m_term_height;i++) 
    if ((i+offset) < buffer.size())
      drawLine(i+offset,&p,i);
}

QString QScreen::selectedText() {
  QString ret;
  for (int i=0;i<buffer.size();i++) {
    bool lineHasSelectedText = false;
    for (int j=0;j<maxlen;j++)
      if (buffer[i].data[j].selected()) {
	ret += buffer[i].data[j].v;
	lineHasSelectedText = true;
      }
    if (lineHasSelectedText)
      ret += '\n';
  }
  ret.replace(QRegExp(" +\\n"),"\n");
  return ret;
}

int main(int argc, char *argv[]) {
  QApplication app(argc,argv);
  QScreen *scrollArea = new QScreen;
  scrollArea->setFont(12);
  scrollArea->show();
  return app.exec();
}
