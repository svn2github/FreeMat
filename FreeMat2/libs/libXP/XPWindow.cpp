#include "XPWindow.hpp"

XPWindow::XPWindow(int width, int height) :
  QMainWindow() {
  resize(width,height);
  child = NULL;
}

void XPWindow::Title(std::string title) {
  setCaption(title);
}

void XPWindow::AddWidget(XPWidget *widget) {
  if (child) {
    child->reparent(NULL,0,QPoint(width(),height()),true);
    child->hide();
  }
  widget->reparent(this,0,QPoint(width(),height()),true);
  setCentralWidget(widget);
  child = widget;
  child->show();
  repaint();
}
