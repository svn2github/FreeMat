#ifndef __XPLabel_hpp__
#define __XPLabel_hpp__

#include "XPWidget.hpp"
#include <string>

class XPLabel : public XPWidget {
  std::string label;
public:
  XPLabel(XPWidget* parent, Rect2D size, std::string alabel);
  virtual void OnDraw(GraphicsContext &gc, Rect2D region);
};

#endif
