#include "XPButton.hpp"

XPButton::XPButton(int a_x0, int a_y0,
		   int a_width, int a_height,
		   std::string a_label) {
  label = a_label;
  x0 = a_x0;
  y0 = a_y0;
  m_width = a_width;
  m_height = a_height;
  state = 0;
}

void XPButton::OnDraw(GraphicsContext &gc) {
  if (state == 0) {
    gc.SetForeGroundColor(Color("white"));
    gc.DrawLine(Point2D(x0,y0),Point2D(x0+m_width-2,y0));
    gc.DrawLine(Point2D(x0,y0),Point2D(x0,y0+m_height-2));
    gc.SetForeGroundColor(Color("black"));
    gc.DrawLine(Point2D(x0,y0+m_height-1),
		Point2D(x0+m_width-1,y0+m_height-1));
    gc.DrawLine(Point2D(x0+m_width-1,y0),
		Point2D(x0+m_width-1,y0+m_height-1));
    gc.SetForeGroundColor(Color("grey"));
    gc.DrawLine(Point2D(x0+1,y0+m_height-2),
		Point2D(x0+m_width-2,y0+m_height-2));
    gc.DrawLine(Point2D(x0+m_width-2,y0+1),
		Point2D(x0+m_width-2,y0+m_height-2));
    gc.SetForeGroundColor(Color("black"));
    gc.SetFont(12);
    Point2D txtsze(gc.GetTextExtent(label));
    gc.DrawTextString(label,
		      Point2D(x0+m_width/2-txtsze.x/2,
			      y0+m_height/2+txtsze.y/2),
		      ORIENT_0);
  } else {
    gc.SetForeGroundColor(Color("black"));
    gc.DrawLine(Point2D(x0,y0),Point2D(x0+m_width-2,y0));
    gc.DrawLine(Point2D(x0,y0),Point2D(x0,y0+m_height-2));
    gc.SetForeGroundColor(Color("white"));
    gc.DrawLine(Point2D(x0,y0+m_height-1),
		Point2D(x0+m_width-1,y0+m_height-1));
    gc.DrawLine(Point2D(x0+m_width-1,y0),
		Point2D(x0+m_width-1,y0+m_height-1));
    gc.SetForeGroundColor(Color("grey"));
    gc.DrawLine(Point2D(x0+1,y0+1),
		Point2D(x0+1,y0+m_height-2));
    gc.DrawLine(Point2D(x0+1,y0+1),
		Point2D(x0+m_width-3,y0+1));
    gc.SetForeGroundColor(Color("black"));
    gc.SetFont(12);
    Point2D txtsze(gc.GetTextExtent(label));
    gc.DrawTextString(label,
		      Point2D(x0+m_width/2-txtsze.x/2+1,
			      y0+m_height/2+txtsze.y/2+1),
		      ORIENT_0);
  }
}

void XPButton::OnMouseDown(int x, int y) {
  state = 1;
  Refresh();
}

void XPButton::OnMouseUp(int x, int y) {
  state = 0;
  Refresh();
}

