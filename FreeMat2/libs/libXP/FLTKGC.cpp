#include "FLTKGC.hpp"
#include "FL/x.H"
#include "FL/Fl.H"


FLTKGC::FLTKGC(int width, int height) {
  m_width = width;
  m_height = height;
}

FLTKGC::~FLTKGC() {
  fl_line_style(0);
}

Point2D FLTKGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

Point2D FLTKGC::GetTextExtent(std::string label) {
  int w, h;
  w = fl_width(label.c_str());
  h = fl_height();
  return Point2D(w,h);
}

void FLTKGC::DrawTextString(std::string label, Point2D pos, 
			    OrientationType orient) {
  if (orient == ORIENT_0) 
    fl_draw(label.c_str(),pos.x,pos.y);
  else if (orient == ORIENT_90) {
    int w, h;
    unsigned char *data;
    w = fl_width(label.c_str());
    h = fl_height();
    Fl_Offscreen id;
    id = fl_create_offscreen(w,h);
    fl_begin_offscreen((Fl_Offscreen) id);
    fl_font(FL_HELVETICA,m_size);
    fl_color(m_bg.red,m_bg.green,m_bg.blue);;
    fl_rectf(0,0,w,h);
    fl_color(FL_BLACK);
    fl_draw(label.c_str(),0,11);
    data = new unsigned char[w*h*3];
    fl_read_image(data,0,0,w,h);
    fl_end_offscreen();
    fl_delete_offscreen(id);
    unsigned char *rotdata;
    rotdata = new unsigned char[w*h*3];
    // The (i,j)th pixel of the original bitmap
    // is data[i*w+j]
    // The (i,j)th pixel of the transposed bitmap
    // is rotdata[i*h+j]
    // rotdata[i,j] = data[j,i]
    int i, j;
    for (i=0;i<h;i++)
      for (j=0;j<w;j++) {
	rotdata[3*((w-1-j)*h+i)] = data[3*(i*w+j)];
	rotdata[3*((w-1-j)*h+i)+1] = data[3*(i*w+j)+1];
	rotdata[3*((w-1-j)*h+i)+2] = data[3*(i*w+j)+2];
      }
    fl_draw_image(rotdata,pos.x-h,pos.y-w,h,w);
    delete rotdata;
    delete data;
  }
}

void FLTKGC::SetFont(int fontsize) {
  fl_font(FL_HELVETICA,fontsize);
  m_size = fontsize;
}

Color FLTKGC::SetBackGroundColor(Color col) {
  Color ret;
  ret = m_bg;
  m_bg = col;
  return ret;
}

Color FLTKGC::SetForeGroundColor(Color col) {
  Color ret;
  ret = m_fg;
  m_fg = col;
  fl_color(col.red,col.green,col.blue);
  return ret;
}

LineStyleType FLTKGC::SetLineStyle(LineStyleType style) {
  LineStyleType ret;
  ret = m_ls;
  m_ls = style;
  switch(style) {
  case LINE_SOLID:
    fl_line_style(FL_SOLID);
    break;
  case LINE_DASHED:
    fl_line_style(FL_DASH);
    break;
  case LINE_DOTTED:
    fl_line_style(FL_DOT);
    break;
  case LINE_DASH_DOT:
    fl_line_style(FL_DASHDOT);
  }
  return ret;
}

void FLTKGC::DrawLine(Point2D pos1, Point2D pos2) {
  if (m_ls != LINE_NONE)
    fl_line(pos1.x,pos1.y,pos2.x,pos2.y);
}

void FLTKGC::DrawPoint(Point2D pos) {
  if (m_ls != LINE_NONE)
    fl_point(pos.x,pos.y);
}

void FLTKGC::DrawCircle(Point2D pos, int radius) {
  if (m_ls != LINE_NONE)
    fl_arc(pos.x-radius,pos.y-radius,2*radius,2*radius,0,360);
}

void FLTKGC::DrawRectangle(Rect2D rect) {
  if (m_ls != LINE_NONE)
    fl_rect(rect.x1,rect.y1,rect.width,rect.height);
}

void FLTKGC::FillRectangle(Rect2D rect) {
  if (m_ls != LINE_NONE)
    fl_rectf(rect.x1,rect.y1,rect.width,rect.height);
}

void FLTKGC::DrawLines(std::vector<Point2D> pts) {
  if (m_ls == LINE_NONE) 
    return;
  fl_begin_line();
  for (int i=0;i<pts.size();i++)
    fl_vertex(pts[i].x,pts[i].y);
  fl_end_line();
}

void FLTKGC::PushClippingRegion(Rect2D rect) {
  clips.push_back(rect);
  fl_push_clip(rect.x1,rect.y1,rect.width,rect.height);
}

Rect2D FLTKGC::PopClippingRegion() {
  Rect2D ret;
  ret = clips.back();
  clips.pop_back();
  fl_pop_clip();
  return ret;
}

void FLTKGC::BlitImage(unsigned char *data, int width, 
		       int height, int x0, int y0) {
  fl_draw_image(data,x0,y0,width,height);
}

