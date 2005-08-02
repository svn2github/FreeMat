#include "XPWindow.hpp"
#include <qpixmap.h>
#include "freemat-2.xpm"
#ifndef QT3
#include <QIcon>
#endif

XPWindow::XPWindow(int width, int height) :
#ifdef QT3
  QWidget(NULL,NULL) {
#else
  QWidget(NULL,NULL) {
#endif
  QPixmap myIcon = QPixmap(freemat_2);
#ifdef QT3
  setIcon(myIcon);
#else
  setWindowIcon(QIcon(myIcon));
#endif
  resize(width,height);
}

void XPWindow::Title(std::string title) {
#ifdef QT3
  setCaption(title);
#else
  setWindowTitle(title.c_str());
#endif
}

