#ifndef __HandleWindow_hpp__
#define __HandleWindow_hpp__

#include <QWidget>
#include "HandleFigure.hpp"
#include <QStackedWidget>
#include <QTabWidget>
#include <QGLWidget>
#include <QEventLoop>

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
  QEventLoop m_loop;
  int click_x, click_y;
 public:
  HandleWindow(unsigned ahandle);
  ~HandleWindow() {delete hfig;}
  unsigned Handle();
  HandleFigure *HFig();
  void UpdateState();
  void closeEvent(QCloseEvent* e);
  void GetClick(int &x, int &y);
  void mousePressEvent(QMouseEvent* e);
};

}

#endif
