#include "XGC.hpp"
#include "Exception.hpp"
#include <X11/X.h>
#include <X11/Xutil.h>

XGC::XGC(Display* disp, Drawable surf, GC gc, int width, int height) {
  m_display = disp;
  drawable = surf;
  m_gc = gc;
  m_width = width;
  m_height = height;
  current_fontsize = 0;
}

XGC::~XGC() {
}

Point2D XGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

Point2D XGC::GetTextExtent(std::string label) {
  return Point2D(XTextWidth(font_info, label.c_str(), label.size()),
		 font_info->ascent + font_info->descent);
}

void XGC::DrawTextString(std::string label, Point2D pos, OrientationType orient) {
  if (orient == ORIENT_0) {
    XDrawString(m_display, drawable, m_gc, pos.x, pos.y, 
		label.c_str(), label.size());
    return;
  }
  if (orient == ORIENT_90) {
    // Get the size of the string
    Point2D stringSize(GetTextExtent(label));
    // FUDGE FACTOR!! - Not sure why this is necessary, but it is...
    stringSize.x *= 1.1;
    // Create a bitmap of this size
    Pixmap bitmap, rotbitmap;
    bitmap = XCreatePixmap(m_display, drawable, stringSize.x, 
			   stringSize.y, 1);
    rotbitmap = XCreatePixmap(m_display, drawable, stringSize.y, 
			   stringSize.x, 1);
    if (!bitmap) throw 
      FreeMat::Exception("Unable to create bitmap for rotated text!\n");
    GC fontgc;
    fontgc = XCreateGC(m_display, bitmap, 0, NULL);
    XSetForeground(m_display, fontgc, WhitePixel(m_display, 0));
    XFillRectangle(m_display, bitmap, fontgc, 0, 0, 
		   stringSize.x, stringSize.y);
    XSetForeground(m_display, fontgc, BlackPixel(m_display, 0));
    XDrawString(m_display, bitmap, fontgc, font_info->max_bounds.lbearing, 
		font_info->ascent, label.c_str(), label.size());
    XImage *I1;
    I1 = XGetImage(m_display, bitmap, 0, 0, stringSize.x, 
		   stringSize.y, 1, XYPixmap);
    I1->format = XYBitmap;
    int rotw, roth;
    rotw = stringSize.y;
    roth = stringSize.x;
    int bytew;
    bytew = (rotw-1)/8 + 1;
    unsigned char *bitdata2;
    bitdata2=(unsigned char *) malloc((unsigned)(bytew * roth));
    XImage *I2;
    I2 = XCreateImage(m_display, DefaultVisual(m_display, 0),
		      1, XYBitmap, 0, (char*) bitdata2, rotw, roth, 8, 0);
    memset(bitdata2, 255, bytew*roth);
    for (int j=0;j<rotw;j++)
      for (int i=0;i<roth;i++) 
	XPutPixel(I2, j, roth-i-1, XGetPixel(I1, i, j));
    XSetForeground(m_display, fontgc, WhitePixel(m_display, 0));
    XFillRectangle(m_display, rotbitmap, fontgc, 0, 0, roth, rotw);
    XSetForeground(m_display, fontgc, BlackPixel(m_display, 0));
    XPutImage(m_display, rotbitmap, fontgc, I2, 0, 0, 0, 0, rotw, roth);
    XSetStipple(m_display, m_gc, rotbitmap);
    XSetFillStyle(m_display, m_gc, FillStippled);
    int newx, newy;
    newx = pos.x - rotw;
    newy = pos.y - roth;
    XSetTSOrigin(m_display, m_gc, newx, newy);
    XFillRectangle(m_display, drawable, m_gc, newx, newy, rotw, roth);
    XSetFillStyle(m_display, m_gc, FillSolid);
    XDestroyImage(I1);
    XDestroyImage(I2);
    XFreePixmap(m_display,bitmap);
    XFreePixmap(m_display,rotbitmap);
    XFreeGC(m_display,fontgc);
  }
}

void XGC::SetFont(int fontsize) {
  if (fontsize == current_fontsize)
    return;
  char buffer[1000];
  sprintf(buffer,"-adobe-helvetica-medium-r-normal--%d-*",fontsize);
  int cnt;
  char **flist = XListFonts(m_display, buffer, 1000, &cnt);
  if (cnt == 0) {
    printf("No match on font name\r\n");
    return;
  }
  font_info = XLoadQueryFont(m_display, flist[0]);
  XSetFont(m_display, m_gc, font_info->fid);
  XFreeFontNames(flist);
}

Color XGC::SetBackGroundColor(Color col) {
  Color retsave(bg);
  bg = col;
  XColor t;
  t.red = col.red * 257;
  t.green = col.green * 257;
  t.blue = col.blue * 257;
  XAllocColor(m_display, DefaultColormap(m_display, 0), &t);
  XSetBackground(m_display, m_gc, t.pixel);
  return retsave;
}

Color XGC::SetForeGroundColor(Color col) {
  Color retsave(fg);
  fg = col;
  XColor t;
  t.red = col.red * 257;
  t.green = col.green * 257;
  t.blue = col.blue * 257;
  XAllocColor(m_display, DefaultColormap(m_display, 0), &t);
  XSetForeground(m_display, m_gc, t.pixel);
  return retsave;
}

LineStyleType XGC::SetLineStyle(LineStyleType style) {
  unsigned char line_dashed_list[2] = {4, 4};
  unsigned char line_dotted_list[2] = {3, 1};
  unsigned char line_dash_dot_list[4] = {3, 4, 3, 1};
  LineStyleType retsav(ls);
  ls = style;
  switch (ls) {
  case LINE_SOLID:
    XSetLineAttributes(m_display, m_gc, 1, LineSolid, CapButt, JoinMiter);
    break;
  case LINE_DASHED:
    XSetDashes(m_display, m_gc, 0, (const char*) line_dashed_list, 2);
    break;
  case LINE_DOTTED:
    XSetDashes(m_display, m_gc, 0, (const char*) line_dotted_list, 2);
    break;
  case LINE_DASH_DOT:
    XSetDashes(m_display, m_gc, 0, (const char*) line_dash_dot_list, 4);
    break;
  }
  return retsav;
}

void XGC::DrawLine(Point2D pos1, Point2D pos2) {
  XDrawLine(m_display, drawable, m_gc, pos1.x, pos1.y, pos2.x, pos2.y);
}

void XGC::DrawPoint(Point2D pos) {
  XDrawPoint(m_display, drawable, m_gc, pos.x, pos.y);
}

void XGC::DrawCircle(Point2D pos, int radius) {
  XDrawArc(m_display, drawable, m_gc, pos.x - radius, pos.y - radius, radius*2, 
	   radius*2 , 0, 64*360);
}

void XGC::DrawRectangle(Rect2D rect) {
  XDrawRectangle(m_display, drawable, m_gc, rect.x1, rect.y1, rect.width, rect.height);
}

void XGC::FillRectangle(Rect2D rect) {
  XFillRectangle(m_display, drawable, m_gc, rect.x1, rect.y1, rect.width, rect.height);
}

void XGC::DrawLines(std::vector<Point2D> pts) {
  XPoint *t;
  t = (XPoint*) malloc(sizeof(XPoint)*pts.size());
  int i;
  for (i=0;i<pts.size();i++) {
    t[i].x = pts[i].x;
    t[i].y = pts[i].y;
  }
  XDrawLines(m_display, drawable, m_gc, t, pts.size(), CoordModeOrigin);
  free(t);
}

void XGC::PushClippingRegion(Rect2D rect) {
  XRectangle clipwin;
  clipwin.x = rect.x1;
  clipwin.y = rect.y1;
  clipwin.width = rect.width;
  clipwin.height = rect.height;
  XSetClipRectangles(m_display, m_gc, 0, 0, &clipwin, 1, Unsorted);
  clipstack.push_back(rect);
}

Rect2D XGC::PopClippingRegion() {
  clipstack.pop_back();
  Rect2D rect;
  if (clipstack.empty()) {
    rect.x1 = 0;
    rect.y1 = 0;
    rect.width = m_width;
    rect.height = m_height;
  } else {
    rect = clipstack.back();
  }
  XRectangle clipwin;
  clipwin.x = rect.x1;
  clipwin.y = rect.y1;
  clipwin.width = rect.width;
  clipwin.height = rect.height;
  XSetClipRectangles(m_display, m_gc, 0, 0, &clipwin, 1, Unsorted);  
  return rect;
}
