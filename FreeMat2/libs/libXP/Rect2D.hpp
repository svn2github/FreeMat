#ifndef __Rect2D_hpp__
#define __Rect2D_hpp__

#include "Point2D.hpp"

class Rect2D {
 public:
  int x1;
  int y1;
  int width;
  int height;
  Rect2D(int ax1, int ay1, int awidth, int aheight);
  Rect2D();
  bool TestOutside(Point2D a);
  bool TestIntersect(Rect2D a);
};

#endif
