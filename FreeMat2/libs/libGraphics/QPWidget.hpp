#ifndef __QPWidget_hpp__
#define __QPWidget_hpp__

#include <qwidget.h>
#include <qpainter.h>

namespace FreeMat {
  /**
   * Printable QWidget - supports a hook to the drawing
   * code through the DrawMe method (which is automatically
   * called in expose events).  Anything we want to be able
   * to print to EPS/PS/Printer needs to derive from this 
   * class.
   */
  class QPWidget : public QWidget {
  public:
    QPWidget(QWidget* parent, const char *Name);
    virtual void DrawMe(QPainter& gc) = 0;
    void paintEvent(QPaintEvent* e);
  };
}

#endif
