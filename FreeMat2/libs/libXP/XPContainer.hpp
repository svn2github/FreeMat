#ifndef __XPContainer_hpp__
#define __XPContainer_hpp__

#include "XPWidget.hpp"
#include <vector>

class XPContainer : public XPWidget {
  std::vector<XPWidget*> children;
public:
  virtual void OnDraw(GraphicsContext &gc);
  void AddChild(XPWidget* child);
  virtual void OnMouseDown(int x, int y);
  virtual void OnMouseUp(int x, int y);
};

#endif
