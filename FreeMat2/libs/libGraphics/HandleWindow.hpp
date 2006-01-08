#ifndef __HandleWindow_hpp__
#define __HandleWindow_hpp__

#include <QWidget>

class HandleWindow : QWidget {
 protected:
  unsigned handle;
  bool GLActive;
  QWidget *child;
  HandleFigure *hfig;
 public:
  HandleFigure(unsigned ahandle);
  unsigned Handle();
  HandleFigure *HFig();
  void UpdateState();
};



#endif
