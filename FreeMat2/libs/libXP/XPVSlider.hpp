#ifndef __XPVSlider_hpp__
#define __XPVSlider_hpp__

#include "XPWidget.hpp"
#include <string>

class XPVSlider : public XPWidget {
  float val;
  int state;
  Point2D drag_start;
  float val_start;
public:
  XPVSlider(XPWidget* parent, Rect2D sze, float initval);
  virtual void OnDraw(GraphicsContext &gc, Rect2D region);
  virtual void OnMouseDown(Point2D pt);
  virtual void OnMouseDrag(Point2D pt);
  virtual void OnMouseUp(Point2D pt);
};

#endif
