#include "Rect2D.hpp"

Rect2D::Rect2D(int ax1, int ay1, int awidth, int aheight) {
  x1 = ax1;
  y1 = ay1;
  width = awidth;
  height = aheight;
}

bool Rect2D::TestOutside(Point2D a) {
  return (a.x < x1 || a.x >= (x1+width) || a.y < y1 || a.y > (y1+height));
}

bool Rect2D::TestIntersect(Rect2D a) {
  return ((!TestOutside(Point2D(a.x1,a.y1)) ||
	   !TestOutside(Point2D(a.x1+a.width,a.y1)) ||
	   !TestOutside(Point2D(a.x1,a.y1+a.height)) ||
	   !TestOutside(Point2D(a.x1+a.width,a.y1+a.height))) ||
	  (!a.TestOutside(Point2D(x1,y1)) ||
	   !a.TestOutside(Point2D(x1+width,y1)) ||
	   !a.TestOutside(Point2D(x1,y1+height)) ||
	   !a.TestOutside(Point2D(x1+width,y1+height))));
}

Rect2D::Rect2D() {
  x1 = 0;
  y1 = 0;
  width = 0;
  height = 0;
}
