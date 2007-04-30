#include "term2.hpp"
#include <QtDebug>

QScreen::QScreen() {
  setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
}

void QScreen::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key_L) {
    qDebug() << "Keypress\n";
    for (int i=0;i<200;i++) {
      m_text << QString("line %1").arg(i);
    }
    updateGeometry();
  }
}

 QSize QScreen::sizeHint() const {
   qDebug() << "width " << 80*5 << " height " << m_text.size()*5;
   return QSize(80*5,m_text.size()*5);
 }

void QScreen::paintEvent(QPaintEvent *e) {
  
}

int main(int argc, char *argv[]) {
  QApplication app(argc,argv);
  qDebug() << "Initialize\n";
  QScreen *imageLabel = new QScreen;
  //  QImage image("happyguy.png");
  //  imageLabel->setPixmap(QPixmap::fromImage(image));
//   QScrollArea *scrollArea = new QScrollArea;
//    scrollArea->setBackgroundRole(QPalette::Base);
//    scrollArea->setWidget(imageLabel);
//    scrollArea->show();
  imageLabel->show();
  return app.exec();
}
