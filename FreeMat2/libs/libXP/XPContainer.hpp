#ifndef __XPContainer_hpp__
#define __XPContainer_hpp__

#include "XPWidget.hpp"
#include <vector>

class XPContainer : public XPWidget {
  std::vector<XPWidget*> children;
  int focus;
public:
  XPContainer(XPWidget *parent, Rect2D rect);
  virtual ~XPContainer();
  virtual void OnDraw(GraphicsContext &gc, Rect2D region);
  void AddChild(XPWidget* child);
  virtual void OnMouseDown(Point2D pt);
  virtual void OnMouseDrag(Point2D pt);
  virtual void OnMouseUp(Point2D pt);
  virtual void OnChar(char key);
};

#endif
