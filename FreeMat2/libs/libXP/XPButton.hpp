#ifndef __XPButton_hpp__
#define __XPButton_hpp__

#include "XPWidget.hpp"
#include <string>

class XPButton : public XPWidget {
  std::string label;
  int state;
public:
  XPButton(int a_x0, int a_y0, 
	   int a_width, int a_height,
	   std::string a_label);
  virtual void OnDraw(GraphicsContext &gc);
  virtual void OnMouseDown(int x, int y);
  virtual void OnMouseUp(int x, int y);
};

#endif
