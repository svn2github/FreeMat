#include "XGC.hpp"
#include "Exception.hpp"
#include "Reducer.hpp"
#include <X11/X.h>
#include <X11/Xutil.h>

static unsigned int bitsPerPixelAtDepth(Display *disp, int scrn, unsigned int depth) {
  XPixmapFormatValues *xf;
  unsigned int nxf, a;

  xf = XListPixmapFormats(disp, (int *)&nxf);
  for (a = 0; a < nxf; a++)
    if (xf[a].depth == depth)
      return(xf[a].bits_per_pixel);
  fprintf(stderr, "bitsPerPixelAtDepth: Can't find pixmap depth info!\n");
  exit(1);
}

XGC::XGC(Display* disp, Visual* vis, Drawable surf, GC gc, int width, int height) {
  m_display = disp;
  m_visual = vis;
  drawable = surf;
  m_gc = gc;
  m_width = width;
  m_height = height;
  current_fontsize = 0;
  colormapActive = false;
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
  static unsigned char line_dashed_list[2] = {4, 4};
  static unsigned char line_dotted_list[2] = {3, 1};
  static unsigned char line_dash_dot_list[4] = {3, 4, 3, 1};
  LineStyleType retsav(ls);
  ls = style;
  switch (ls) {
  case LINE_SOLID:
    XSetLineAttributes(m_display, m_gc, 1, LineSolid, CapButt, JoinMiter);
    break;
  case LINE_DASHED:
    XSetLineAttributes(m_display, m_gc, 1, LineOnOffDash, CapButt, JoinMiter);
    XSetDashes(m_display, m_gc, 0, (const char*) line_dashed_list, 2);
    break;
  case LINE_DOTTED:
    XSetLineAttributes(m_display, m_gc, 1, LineOnOffDash, CapButt, JoinMiter);
    XSetDashes(m_display, m_gc, 0, (const char*) line_dotted_list, 2);
    break;
  case LINE_DASH_DOT:
    XSetLineAttributes(m_display, m_gc, 1, LineOnOffDash, CapButt, JoinMiter);
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

bool XGC::IsColormapActive() {
  return colormapActive;
}

Colormap XGC::GetColormap() {
  return m_cmap;
}

void XGC::BlitImagePseudoColor(unsigned char *data, int width, int height, int x0, int y0) {
  // Get the number of colors in the visual...
  int ddepth = DefaultDepth(m_display, 0);
  int dbits = bitsPerPixelAtDepth(m_display,0,ddepth);
  int dpixlen = (dbits + 7)/8;
  int colorCount = 1 << m_visual->bits_per_rgb;
  colorCount = (colorCount > 32768) ? 32768 : colorCount;
  // OK, now we use the color reducer to get a colormapped image
  unsigned short *outimg = (unsigned short*) 
    malloc(width*height*sizeof(short));
  unsigned short *outcolors = (unsigned short*)
    malloc(colorCount*3*sizeof(short));
  int colorsUsed;
  colorsUsed = ColorReduce(data, width, height, colorCount, outcolors, outimg);
  // Allocate a colormap
  m_cmap = XCreateColormap(m_display, RootWindow(m_display, 0),  m_visual, AllocAll);
  XColor xcolor;
  xcolor.flags = DoRed | DoGreen | DoBlue;
  for (int b=0;b<colorsUsed;b++) {
    xcolor.pixel = b;
    xcolor.red = outcolors[3*b];
    xcolor.green = outcolors[3*b+1];
    xcolor.blue = outcolors[3*b+2];
    XStoreColor(m_display, m_cmap, &xcolor);
  }
  unsigned short *source_data;
  source_data = outimg;
  char *ddata = (char*) malloc(width*height*dpixlen);
  m_image = XCreateImage(m_display, m_visual, ddepth, ZPixmap, 0,
				 ddata, width, height, 8, 0);
  for (int y=0; y<height; y++)
    for (int x=0; x<width; x++)
      XPutPixel(m_image,x,y,*source_data++);
  colormapActive = true;
  XPutImage(m_display, drawable, m_gc, m_image, 0, 0, 0, 0, m_width, m_height);
  //  XDestroyImage(m_image);
}

void XGC::foofoo() {
  //  XPutImage(m_display, drawable, m_gc, m_image, 0, 0, 0, 0, m_width, m_height);
}

void XGC::BlitImage(unsigned char *data, int width, int height, int x0, int y0) {
  // Check for PseudoColor visual
  if ((m_visual->c_class != TrueColor) &&
      (m_visual->c_class != DirectColor)) {
    BlitImagePseudoColor(data,width,height,x0,y0);
    return;
  }
  int ddepth = DefaultDepth(m_display, 0);
  int dbits = bitsPerPixelAtDepth(m_display,0,ddepth);
  int dpixlen = (dbits + 7)/8;
  char *ddata = (char*) malloc(width*height*dpixlen);
  // Put the contents of the image into an XImage
  XImage *m_image = XCreateImage(m_display, m_visual, ddepth, ZPixmap, 0, 
				 ddata, width, height, 8, 0);
  int red_mask, green_mask, blue_mask;
  unsigned int red_shift, green_shift, blue_shift;
  float red_scale, green_scale, blue_scale;
  red_mask = m_image->red_mask;
  green_mask = m_image->green_mask;
  blue_mask = m_image->blue_mask;
  red_shift = GetShiftFromMask(red_mask);
  green_shift = GetShiftFromMask(green_mask);
  blue_shift = GetShiftFromMask(blue_mask);
  red_scale = (red_mask >> red_shift)/255.0;
  green_scale = (green_mask >> green_shift)/255.0;
  blue_scale = (blue_mask >> blue_shift)/255.0;
  unsigned long pixval;
  unsigned char red, green, blue;
  unsigned char *source_data;
  source_data = data;
  for (int y=0;y<height;y++)
    for (int x=0;x<width;x++) {
      red = *source_data++;
      green = *source_data++;
      blue = *source_data++;
      pixval = ((((unsigned long) (red * red_scale)) << red_shift) & red_mask) |
	((((unsigned long) (green * green_scale)) << green_shift) & green_mask) |
	((((unsigned long) (blue * blue_scale)) << blue_shift) & blue_mask);
      XPutPixel(m_image,x,y,pixval);
    }
  XPutImage(m_display, drawable, m_gc, m_image, 0, 0, x0, y0, width, height);
  XDestroyImage(m_image);  
}
