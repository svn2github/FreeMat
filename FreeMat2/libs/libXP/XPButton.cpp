#include "XPButton.hpp"

XPButton::XPButton(XPWidget* parent, Rect2D sze, XPWidget* child) :
  XPWidget(parent, sze) {
  m_child = child;
  m_child->SetParent(this);
  state = 0;
}

void XPButton::OnDraw(GraphicsContext &gc, Rect2D region) {
  if (state == 0) {
    gc.SetForeGroundColor(Color("white"));
    gc.DrawLine(Point2D(bounds.x1,bounds.y1),Point2D(bounds.x1+bounds.width-2,bounds.y1));
    gc.DrawLine(Point2D(bounds.x1,bounds.y1),Point2D(bounds.x1,bounds.y1+bounds.height-2));
    gc.SetForeGroundColor(Color("grey"));
    gc.DrawLine(Point2D(bounds.x1,bounds.y1+bounds.height-1),
		Point2D(bounds.x1+bounds.width-1,bounds.y1+bounds.height-1));
    gc.DrawLine(Point2D(bounds.x1+bounds.width-1,bounds.y1),
		Point2D(bounds.x1+bounds.width-1,bounds.y1+bounds.height-1));
    m_child->OnDraw(gc, region);
  } else {
    gc.SetForeGroundColor(Color("grey"));
    gc.DrawLine(Point2D(bounds.x1,bounds.y1),Point2D(bounds.x1+bounds.width-2,bounds.y1));
    gc.DrawLine(Point2D(bounds.x1,bounds.y1),Point2D(bounds.x1,bounds.y1+bounds.height-2));
    gc.SetForeGroundColor(Color("white"));
    gc.DrawLine(Point2D(bounds.x1,bounds.y1+bounds.height-1),
		Point2D(bounds.x1+bounds.width-1,bounds.y1+bounds.height-1));
    gc.DrawLine(Point2D(bounds.x1+bounds.width-1,bounds.y1),
		Point2D(bounds.x1+bounds.width-1,bounds.y1+bounds.height-1));
    m_child->ShiftWidget(Point2D(2,2));
    m_child->OnDraw(gc, region);
    m_child->ShiftWidget(Point2D(-2,-2));
  }
}

void XPButton::OnMouseDown(Point2D pt) {
  state = 1;
  Refresh(bounds);
}

void XPButton::OnMouseUp(Point2D pt) {
  state = 0;
  Refresh(bounds);
}

