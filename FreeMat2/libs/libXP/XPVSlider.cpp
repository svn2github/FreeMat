#include "XPVSlider.hpp"

//
//  ----
// |    |
//  ----
//
//
XPVSlider::XPVSlider(XPWidget* parent, Rect2D sze, float initval) :
  XPWidget(parent, sze) {
  val = initval;
  state = 0;
}

void XPVSlider::OnDraw(GraphicsContext &gc, Rect2D region) {
  int n;
  n = bounds.y1+val*(bounds.height-11)+5;
  gc.SetForeGroundColor(Color("grey"));
  gc.DrawLine(Point2D(bounds.x1+bounds.width/2,bounds.y1),
	      Point2D(bounds.x1+bounds.width/2,bounds.y1+bounds.height));
  gc.SetForeGroundColor(Color("black"));
  gc.DrawLine(Point2D(bounds.x1+bounds.width/2+1,bounds.y1),
	      Point2D(bounds.x1+bounds.width/2+1,bounds.y1+bounds.height));
  gc.SetForeGroundColor(Color("light grey"));
  gc.FillRectangle(Rect2D(bounds.x1,n-5,bounds.width,11));
  // This is the center of the slider control (vertical)
  gc.SetForeGroundColor(Color("white"));
  gc.DrawLine(Point2D(bounds.x1,n-5),
	      Point2D(bounds.x1+bounds.width,n-5));
  gc.DrawLine(Point2D(bounds.x1,n-5),
	      Point2D(bounds.x1,n+5));
  gc.SetForeGroundColor(Color("grey"));
  gc.DrawLine(Point2D(bounds.x1+bounds.width-1,n-5),
	      Point2D(bounds.x1+bounds.width-1,n+5));
  gc.DrawLine(Point2D(bounds.x1,n+5),
	      Point2D(bounds.x1+bounds.width,n+5));  
}

void XPVSlider::OnMouseDown(Point2D pt) {
  Rect2D tab;
  int n;
  n = bounds.y1+val*(bounds.height-11)+5;
  tab.x1 = bounds.x1; tab.y1 = n-5;
  tab.width = bounds.width; tab.height = 11;
  if (!tab.TestOutside(pt)) {
    state = 1;
    drag_start = pt;
    val_start = val;
  }
}

void XPVSlider::OnMouseUp(Point2D pt) {
  state = 0;
}

void XPVSlider::OnMouseDrag(Point2D pt) {
  if (state == 1) {
    val = val_start + (pt.y - drag_start.y)/((float)bounds.height-15);
    val < 0.0 ? val = 0.0 : val = val;
    val > 1.0 ? val = 1.0 : val = val;
    Refresh(bounds);
  }
}
