#include "term2.hpp"
#include <QtDebug>
#include <QtGui>

QScreen::QScreen() {
  setMinimumSize(50,50);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  buffer << tagLine();
  cursor_x = 0;
  cursor_y = 0;
}

void QScreen::ensureCursorVisible() {
  // For the cursor to be visible
  // the scroll bar must be at 
  // cursor_y - m_term_height + 1
  int minval = qMax(verticalScrollBar()->value(),cursor_y-m_term_height+1);
  verticalScrollBar()->setValue(minval);
}

void QScreen::setChar(char t) {
  buffer[cursor_y].data[cursor_x++].v = t;
  if (cursor_x >= m_term_width) {
    nextLine(); 
  } else {
    ensureCursorVisible();
    viewport()->update();
  }
}

void QScreen::setFont(int size) {
  fnt = QFont("Monaco",size);
  calcGeometry();
}

void QScreen::nextLine() {
  cursor_x = 0;
  cursor_y++;
  if (cursor_y >= buffer.size())
    buffer << tagLine();
  int cval = verticalScrollBar()->value();
  verticalScrollBar()->setRange(0,qMax(0,buffer.size()-m_term_height));
  verticalScrollBar()->setValue(cval);
  ensureCursorVisible();
}

void QScreen::drawLine(int linenum, QPainter *e, int yval) {
  QString outd;
  tagLine todraw(buffer[linenum]);
  for (int i=0;i<m_term_width;i++)
    outd += todraw.data[i].v;
  //  qDebug() << ":" << outd;
  e->drawText(0,yval+m_char_h,outd);
}

void QScreen::keyPressEvent(QKeyEvent *e) {
  if (e->key() != Qt::Key_Return) {
    QString qtxt(e->text());
    if (qtxt.isEmpty()) return;
    setChar(qtxt.toStdString().c_str()[0]);
    if (qtxt == "P") {
      for (int i=0;i<1000;i++) 
	outputString(QString("bunch line %1").arg(i));
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
  ensureCursorVisible();
}

void QScreen::paintEvent(QPaintEvent *e) {
  QPainter p(viewport());
  p.setFont(fnt);
  p.setBackground(qApp->palette().brush(QPalette::Highlight));
  p.setPen(qApp->palette().color(QPalette::HighlightedText));
  int offset = verticalScrollBar()->value();
  for (int i=0;i<m_term_height;i++) 
    if ((i+offset) < buffer.size())
      drawLine(i+offset,&p,i*m_char_h);
}

int main(int argc, char *argv[]) {
  QApplication app(argc,argv);
  QScreen *scrollArea = new QScreen;
  scrollArea->setFont(12);
  scrollArea->show();
  return app.exec();
}
