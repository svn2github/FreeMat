#include "Point2D.hpp"

bool Point2D::isFinite() {
  return (x<4096) & (x>-4096) & (y<4096) & (y>-4096);
}

Point2D::Point2D(int xa, int ya) {
  x = xa;
  y = ya;
}

Point2D::Point2D() {
  x = 0; 
  y = 0;
}
