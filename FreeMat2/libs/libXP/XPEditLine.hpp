#ifndef __XPEditLine_hpp__
#define __XPEditLine_hpp__

#include "XPWidget.hpp"
#include <string>

class XPEditLine : public XPWidget {
  std::string buffer;
  int state;
  int ipos;
public:
  XPEditLine(XPWidget* parent, Rect2D sze, std::string init);
  virtual void OnMouseDown(Point2D pt);
  virtual void OnChar(char key);
  virtual void OnDraw(GraphicsContext &gc, Rect2D region);
};

#endif
