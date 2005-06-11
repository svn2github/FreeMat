#include "GTKGC.hpp"

GTKGC::GTKGC(GdkPixmap* pixmp, int width, int height, PangoLayout* pngo) {
  pixmap = pixmp;
  gc = gdk_gc_new(pixmap);
  m_width = width;
  m_height = height;
  pango = pngo;
  myFont = NULL;
}

GTKGC::~GTKGC() {
  g_object_unref(gc);
}

Point2D GTKGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

void GTKGC::DrawLine(Point2D pos1, Point2D pos2) {
  gdk_draw_line(pixmap,gc,pos1.x,pos1.y,pos2.x,pos2.y);
}

void GTKGC::DrawPoint(Point2D pos) {
  gdk_draw_point(pixmap,gc,pos.x,pos.y);
}

void GTKGC::DrawCircle(Point2D pos, int radius) {
  gdk_draw_arc(pixmap,gc,FALSE,pos.x-radius,pos.y-radius,radius*2,radius*2,0,360*64);
}

void GTKGC::DrawRectangle(Rect2D rect) {
  gdk_draw_rectangle(pixmap,gc,FALSE,rect.x1,rect.y1,rect.width,rect.height);
}

void GTKGC::FillRectangle(Rect2D rect) {
  gdk_draw_rectangle(pixmap,gc,TRUE,rect.x1,rect.y1,rect.width,rect.height);  
}

void GTKGC::FillQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  GdkPoint pt[4];
  pt[0].x = p1.x; pt[0].y = p1.y;
  pt[1].x = p2.x; pt[1].y = p2.y;
  pt[2].x = p3.x; pt[2].y = p3.y;
  pt[3].x = p4.x; pt[3].y = p4.y;
  gdk_draw_polygon(pixmap,gc,TRUE,pt,4);
}

void GTKGC::DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  GdkPoint pt[4];
  pt[0].x = p1.x; pt[0].y = p1.y;
  pt[1].x = p2.x; pt[1].y = p2.y;
  pt[2].x = p3.x; pt[2].y = p3.y;
  pt[3].x = p4.x; pt[3].y = p4.y;
  gdk_draw_polygon(pixmap,gc,FALSE,pt,4);
}

void GTKGC::DrawLines(std::vector<Point2D> pts) {
  GdkPoint *gpts = new GdkPoint[pts.size()];
  for (int i=0;i<pts.size();i++) {
    gpts[i].x = pts[i].x;
    gpts[i].y = pts[i].y;
  }
  gdk_draw_lines(pixmap,gc,gpts,pts.size());
}

void GTKGC::PushClippingRegion(Rect2D rect) {
  clips.push_back(rect);
  GdkRectangle grect;
  grect.x = rect.x1;
  grect.y = rect.y1;
  grect.width = rect.width;
  grect.height = rect.height;
  gdk_gc_set_clip_rectangle(gc,&grect);
}

Rect2D GTKGC::PopClippingRegion() {
  Rect2D ret(clips.back());
  clips.pop_back();
  if (!clips.empty()) {
    GdkRectangle grect;
    grect.x = clips.back().x1;
    grect.y = clips.back().y1;
    grect.width = clips.back().width;
    grect.height = clips.back().height;
    gdk_gc_set_clip_rectangle(gc,&grect);
  }
  return ret; 
}

Color GTKGC::SetBackGroundColor(Color col) {
  Color ret(m_bg);
  m_bg = col;
  GdkColor gcol;
  gcol.red = col.red << 8;
  gcol.green = col.green << 8;
  gcol.blue = col.blue << 8;
  gdk_gc_set_rgb_bg_color(gc,&gcol);
  return ret;
}

Color GTKGC::SetForeGroundColor(Color col) {
  Color ret(m_fg);
  m_fg = col;
  GdkColor gcol;
  gcol.red = col.red << 8;
  gcol.green = col.green << 8;
  gcol.blue = col.blue << 8;
  gdk_gc_set_rgb_fg_color(gc,&gcol);
  return ret;
}

void GTKGC::SetFont(int fontsize) {
  if (myFont)
    pango_font_description_free(myFont);
  myFont = pango_font_description_new();
  pango_font_description_set_family( myFont, "sans" );
  pango_font_description_set_size( myFont, fontsize*PANGO_SCALE);
  pango_layout_set_font_description( pango, myFont);
}

Point2D GTKGC::GetTextExtent(std::string label) {
  pango_layout_set_text(pango, label.c_str(), label.size());
  int w, h;
  pango_layout_get_pixel_size(pango, &w, &h);
  return Point2D(w,h);
}

void GTKGC::DrawTextString(std::string label, Point2D pos, OrientationType orient) {
  pango_layout_set_text(pango, label.c_str(), label.size());
  gdk_draw_layout(pixmap, gc, pos.x, pos.y, pango);
}

LineStyleType GTKGC::SetLineStyle(LineStyleType style) {
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

void GTKGC::BlitImage(unsigned char *data, int width, int height, int x0, int y0) {
  gdk_draw_rgb_image(pixmap,gc,x0,y0,width,height,GDK_RGB_DITHER_NORMAL,data,width*3);
}
