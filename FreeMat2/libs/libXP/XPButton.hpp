#ifndef __XPButton_hpp__
#define __XPButton_hpp__

#include "XPWidget.hpp"
#include <string>

class XPButton : public XPWidget {
  XPWidget *m_child;
  int state;
public:
  XPButton(XPWidget* parent, Rect2D sze, XPWidget* child);
  virtual void OnDraw(GraphicsContext &gc, Rect2D region);
  virtual void OnMouseDown(Point2D pt);
  virtual void OnMouseUp(Point2D pt);
};

#endif
