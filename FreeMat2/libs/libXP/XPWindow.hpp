#ifndef __XPWindow_hpp__
#define __XPWindow_hpp__

#include "XPWidget.hpp"
#include <string>
#include <qmainwindow.h>

// A window is a widget with a title, frame, etc
// It has one child widget
class XPWindow : public QMainWindow {
  XPWidget *child;
public:
  virtual ~XPWindow() {};
  XPWindow(int width, int height);
  void Title(std::string title);
  void AddWidget(XPWidget *widget);
  void Show() {show();}
  int GetHeight() {return height();}
  int GetWidth() {return width();}
  void Redraw() {repaint();}
  void Resize(int w, int h) {resize(w,h);}
  void Hide() {hide();}
};

#endif
