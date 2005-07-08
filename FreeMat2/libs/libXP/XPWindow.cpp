#include "XPWindow.hpp"
#include <qpixmap.h>
#include "freemat-2.xpm"
#ifndef QT3
#include <QIcon>
#endif

XPWindow::XPWindow(int width, int height) :
#ifdef QT3
  QMainWindow(NULL,NULL,WRepaintNoErase) {
#else
  QMainWindow(NULL,NULL) {
#endif
  QPixmap myIcon = QPixmap(freemat_2);
#ifdef QT3
  setIcon(myIcon);
#else
  setWindowIcon(QIcon(myIcon));
#endif
  resize(width,height);
  child = NULL;
}

void XPWindow::Title(std::string title) {
#ifdef QT3
  setCaption(title);
#else
  setWindowTitle(title.c_str());
#endif
}

void XPWindow::AddWidget(XPWidget *widget) {
  if (child) {
#ifdef QT3
    child->reparent(NULL,0,QPoint(width(),height()),true);
#else
    child->setParent(NULL);
#endif
    child->hide();
  }
#ifdef QT3
  widget->reparent(this,0,QPoint(width(),height()),true);
#else
  widget->setParent(this);
#endif
  setCentralWidget(widget);
  child = widget;
  child->show();
  repaint();
}
