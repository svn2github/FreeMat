#include "FLTKGC.hpp"
#include "FL/x.H"


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
  fl_measure(label.c_str(),w,h);
  return Point2D(w,h);
}

void FLTKGC::DrawTextString(std::string label, Point2D pos, 
			    OrientationType orient) {
  fl_draw(label.c_str(),pos.x,pos.y);
}

void FLTKGC::SetFont(int fontsize) {
  fl_font(FL_HELVETICA,fontsize);
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
}

void FLTKGC::BlitImage(unsigned char *data, int width, 
		       int height, int x0, int y0) {
  fl_draw_image(data,x0,y0,width,height);
}

void CaptureWidget(Fl_Widget *a, unsigned char *data, int width, int height) {
#if defined(__APPLE__) || defined(WIN32)
  void *id; 
#else
  unsigned id; 
#endif // __APPLE__ || WIN32
  id = fl_create_offscreen(width, height);
  fl_begin_offscreen((Fl_Offscreen)id);
  a->draw();
  fl_end_offscreen();
  // Ask the server to capture the contents of the pixmap into
  // an XImage and send it back to us...
  XImage *img = XGetImage(fl_display, id, 0, 0, width, height,
			  ~0, ZPixmap);
  // What we do now depends on the visual type.  For pseudocolor
  // visuals, we retrieve the current colormap
  if (fl_visual->c_class == PseudoColor) {
    XColor *cvals = (XColor*) malloc(sizeof(XColor)*fl_visual->colormap_size);
    for (int m=0;m<fl_visual->colormap_size;m++)
      cvals[m].pixel = m;
    XQueryColors(fl_display, DefaultColormap(fl_display, 0), cvals, 
		 fl_visual->colormap_size);
    // Then we directly convert the image data...	
    for (int y=0;y<height;y++)
      for (int x=0;x<width;x++) {
	unsigned long pixel = XGetPixel(img, x, y);
	data[3*(y*width+x)] = cvals[pixel].red >> 8;
	data[3*(y*width+x)+1] = cvals[pixel].green >> 8;
	data[3*(y*width+x)+2] = cvals[pixel].blue >> 8;
      }
    free(cvals);
  } else {
    // For TrueColor and DirectColor, we do the pixel conversion
    // manually - we assume that there are no more than 8 bits
    // per primary...
    unsigned int red_mask, green_mask, blue_mask;
    unsigned int red_shift, green_shift, blue_shift;
    float red_scale, green_scale, blue_scale;
    red_mask = fl_visual->red_mask;
    green_mask = fl_visual->green_mask;
    blue_mask = fl_visual->blue_mask;
    red_shift = GetShiftFromMask(red_mask);
    green_shift = GetShiftFromMask(green_mask);
    blue_shift = GetShiftFromMask(blue_mask);
    red_scale = 255.0/(red_mask >> red_shift);
    green_scale = 255.0/(green_mask >> green_shift);
    blue_scale = 255.0/(blue_mask >> blue_shift);
    for (int y=0;y<height;y++)
      for (int x=0;x<width;x++) {
	unsigned long pixel = XGetPixel(img, x, y);
	data[3*(y*width+x)] = red_scale*((pixel & red_mask) >> red_shift);
	data[3*(y*width+x)+1] = green_scale*((pixel & green_mask) >> 
					       green_shift);
	data[3*(y*width+x)+2] = blue_scale*((pixel & blue_mask) >>
					      blue_shift);
      }
  }
}
