#ifndef __HandleWindow_hpp__
#define __HandleWindow_hpp__

#include <QWidget>
#include "HandleFigure.hpp"
#include <QStackedWidget>
#include <QTabWidget>
#include <QGLWidget>

namespace FreeMat {

class HandleWindow : public QWidget {
 protected:
  unsigned handle;
  bool glActive;
  QGLWidget *glchild;
  QWidget *qtchild;
  HandleFigure *hfig;
  QStackedWidget *layout;
  //  QTabWidget *layout;
 public:
  HandleWindow(unsigned ahandle);
  ~HandleWindow() {delete hfig;}
  unsigned Handle();
  HandleFigure *HFig();
  void UpdateState();
  void closeEvent(QCloseEvent* e);
};

}

#endif
