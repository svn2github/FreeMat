#include "XPWidget.hpp"

XPWidget::XPWidget(XPWidget *parent, Rect2D rect) {
  m_parent = parent;
  bounds = rect;
  focused = false;
}

void XPWidget::OnResize(Point2D pt) {
  bounds.width = pt.x;
  bounds.height = pt.y;
  Refresh(bounds);
}
