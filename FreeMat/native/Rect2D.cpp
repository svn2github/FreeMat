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

Rect2D::Rect2D() {
  x1 = 0;
  y1 = 0;
  width = 0;
  height = 0;
}
