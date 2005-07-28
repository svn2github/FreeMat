#ifndef __Point2D_hpp__
#define __Point2D_hpp__

class Point2D {
 public:
  int x;
  int y;
  Point2D(int xarg, int yarg);
  Point2D();
  bool isFinite();
};

#endif
