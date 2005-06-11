#include "OSXGC.hpp"

OSXGC::OSXGC(CGContextRef gh, int width, int height) {
  m_width = width;
  m_height = height;
  gc = gh;
}

OSXGC::~OSXGC() {
  CGContextFlush(gc);
  CGContextRelease(gc);
}

Point2D OSXGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

void OSXGC::DrawLine(Point2D pos1, Point2D pos2) {
  CGContextBegin(gc);
  CGContextMoveToPoint(gc,pos1.x,pos1.y);
  CGAddLineToPoint(gc,pos2.x,pos2.y);
  CGContextClosePath(gc);
  CGContextStrokePath(gc);
}

void OSXGC::DrawPoint(Point2D pos) {
  //  gdk_draw_point(pixmap,gc,pos.x,pos.y);
}

void OSXGC::DrawCircle(Point2D pos, int radius) {
  CGContextBegin(gc);
  CGContextAddArc(gc,pos.x,pos.y,radius,0,2*M_PI,1);
  CGContextClosePath(gc);
  CGContextStrokePath(gc);
}

void OSXGC::DrawRectangle(Rect2D rect) {
  CGContextBegin(gc);
  CGContextAddRect(gc,CGRectMake(rect.x1,rect.y1,rect.width,rect.height));
  CGContextClosePath(gc);
  CGContextStrokePath(gc);
}

void OSXGC::FillRectangle(Rect2D rect) {
  CGContextBegin(gc);
  CGContextAddRect(gc,CGRectMake(rect.x1,rect.y1,rect.width,rect.height));
  CGContextClosePath(gc);
  CGContextFillPath(gc);
}

void OSXGC::FillQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  CGContextBegin(gc);
  CGContextMoveToPoint(gc,p1.x,p1.y);
  CGAddLineToPoint(gc,p2.x,p2.y);
  CGAddLineToPoint(gc,p3.x,p3.y);
  CGAddLineToPoint(gc,p4.x,p4.y);
  CGContextClosePath(gc);
  CGContextFillPath(gc);
}

void OSXGC::DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  CGContextBegin(gc);
  CGContextMoveToPoint(gc,p1.x,p1.y);
  CGAddLineToPoint(gc,p2.x,p2.y);
  CGAddLineToPoint(gc,p3.x,p3.y);
  CGAddLineToPoint(gc,p4.x,p4.y);
  CGContextClosePath(gc);
  CGContextStrokePath(gc);
}

void OSXGC::DrawLines(std::vector<Point2D> pts) {
  CGContextBegin(gc);
  CGContextMoveToPoint(gc,pts[0].x,pts[0].y);
  for (int i=1:i<pts.size();i++)
    CGContextAddLineToPoint(gc,pts[i].x,pts[i].y);
  CGContextStrokePath(gc);
}

void OSXGC::PushClippingRegion(Rect2D rect) {
  clips.push_back(rect);
  CGContextClipToRect(gc,CGRectMake(rect.x1,rect.y1,rect.width,rect.height));
}

Rect2D OSXGC::PopClippingRegion() {
  Rect2D ret(clips.back());
  clips.pop_back();
  if (!clips.empty()) {
    CGContextClipToRect(gc,CGRectMake(clips.back().x1,
				      clips.back().y1,
				      clips.back().width,
				      clips.back().height));
  }
  return ret; 
}

Color OSXGC::SetBackGroundColor(Color col) {
  Color ret(m_bg);
  m_bg = col;
  return ret;
}

Color OSXGC::SetForeGroundColor(Color col) {
  Color ret(m_fg);
  m_fg = col;
  CGContextSetRGBFillColor(gc,col.red/255.0,
			   col.green/255.0,
			   col.blue/255.0,
			   1.0);
  CGContextSetRGBStrokeColor(gc,col.red/255.0,
			     col.green/255.0,
			     col.blue/255.0,
			     1.0);
  return ret;
}

void OSXGC::SetFont(int fontsize) {
  CGContextSelectFont(gc,"Monaco",fontsize,kCGEncodingMacRoman);
}

Point2D OSXGC::GetTextExtent(std::string label) {
  //FIXME
  return Point2D(label.size()*10,12);
}

void OSXGC::DrawTextString(std::string label, Point2D pos, OrientationType orient) {
  CGContextSetTextPosition(gc, pos.x, pos.y);
  CGContextShowText(gc,label,label.size());
}

LineStyleType OSXGC::SetLineStyle(LineStyleType style) {
  LineStyleType ret(m_lst);
  m_lst = style;
  static gint8 line_dashed_list[2] = {4, 4};
  static gint8 line_dotted_list[2] = {3, 1};
  static gint8 line_dash_dot_list[4] = {3, 4, 3, 1};
  switch (m_lst) {
  case LINE_SOLID:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
    break;
  case LINE_DASHED:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
    gdk_gc_set_dashes(gc, 0,  line_dashed_list, 2);
    break;
  case LINE_DOTTED:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
    gdk_gc_set_dashes(gc, 0,  line_dotted_list, 2);
    break;
  case LINE_DASH_DOT:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
    gdk_gc_set_dashes(gc, 0,  line_dash_dot_list, 4);
    break;
  }
  return ret;
}

void OSXGC::BlitImage(unsigned char *data, int width, int height, int x0, int y0) {
  gdk_draw_rgb_image(pixmap,gc,x0,y0,width,height,GDK_RGB_DITHER_NORMAL,data,width*3);
}
