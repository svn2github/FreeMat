#include "XPEditLine.hpp"
#include <windows.h>

XPEditLine::XPEditLine(XPWidget* parent, Rect2D sze, std::string init) : XPWidget(parent, sze) {
  state = 0;
  ipos = 0;
  buffer = init;
  ipos = buffer.size();
}

void XPEditLine::OnMouseDown(Point2D pt) {
}

void XPEditLine::OnChar(char key) {
  if (key == 8 && ipos > 0) {
    buffer.erase(buffer.begin()+ipos-1);
    ipos--;
  } else if (key != 8) {
    buffer.insert(buffer.begin()+ipos,key);
    ipos++;
  }
  Refresh(bounds);
}

void XPEditLine::OnDraw(GraphicsContext &gc, Rect2D region) {
  gc.SetForeGroundColor(Color("white"));
  gc.FillRectangle(bounds);
  gc.SetForeGroundColor(Color("black"));
  gc.SetBackGroundColor(Color("white"));
  std::string window(buffer);
  bool fits = false;
  while (!fits) {
    Point2D fwidth(gc.GetTextExtent(window));
    fits = (fwidth.x < bounds.width*0.95);
    if (!fits)
      window.erase(window.begin());
  }
  std::string front(window.substr(0,ipos));
  std::string back;
  if (ipos < window.size() - 1)
    back = window.substr(ipos,window.size());
  Point2D fwidth = gc.GetTextExtent(front);
  gc.DrawTextString(front, 
		    Point2D(bounds.x1+5, bounds.y1+bounds.height-5), 
		    ORIENT_0);
  if (focused)
    gc.DrawLine(Point2D(bounds.x1+5+fwidth.x+1,bounds.y1+2),
		Point2D(bounds.x1+5+fwidth.x+1,bounds.y1+bounds.height-5));
  gc.DrawTextString(back, 
		    Point2D(bounds.x1+5+fwidth.x+2, 
			    bounds.y1+bounds.height-5), 
		    ORIENT_0);
}
