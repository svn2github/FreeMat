#include "XPLabel.hpp"

XPLabel::XPLabel(XPWidget* parent, Rect2D size, std::string alabel) :
  XPWidget(parent, size) {
  label = alabel;
}

void XPLabel::OnDraw(GraphicsContext &gc, Rect2D region) {
  Point2D txtsze(gc.GetTextExtent(label));
  gc.SetForeGroundColor(Color("black"));
  gc.DrawTextString(label,
		    Point2D(bounds.x1+bounds.width/2-txtsze.x/2,
			    bounds.y1+bounds.height/2+txtsze.y/2),
		    ORIENT_0);
}
