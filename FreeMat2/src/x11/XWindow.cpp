#include "XWindow.hpp"
#include "Reducer.hpp"
#include <map>
#include <vector>
#include <iostream>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
//#include "RGBImageGC.hpp"
#include "PostScriptGC.hpp"
#include "Exception.hpp"
#include "XGC.hpp"

static bool firstWindow = true;
Atom proto_atom, delete_atom;
bool stdincb_active = false;
fdcallback stdin_cb;
static Display *theDisplay;
std::vector<XWindow*> deleteQ;

void SetActiveDisplay(Display *d) {
  theDisplay = d;
}

void RegisterSTDINCallback(fdcallback cb) {
  stdin_cb = cb;
  stdincb_active = true;
}

#define MIN(x,y) ((x) < (y) ? (x) : (y))

enum {state_normal, state_click_waiting, state_box_start, state_box_anchored};

std::map<Window, XWindow*, std::less<Window> > winlist;

void RegisterXWindow(XWindow *p) {
  winlist[p->getWindow()] = p;
}

void UnregisterXWindow(XWindow *p) {
  winlist.erase(p->getWindow());
}

XWindow::XWindow(WindowType wtype) {
  m_display = theDisplay;
  m_type = wtype;
  m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, 0),
				 0, 0, 500, 400, 0, 
				 BlackPixel(m_display, 0),
				 WhitePixel(m_display, 0));
  m_visual = DefaultVisual(m_display, 0);
  m_width = 500;
  m_height = 400;
  RegisterXWindow(this);
  m_pixmap = None;
  m_state = state_normal;
  XGCValues values;
  unsigned long valuemask = 0;
  m_gc = XCreateGC(m_display, m_window, valuemask, &values);
  if (firstWindow) {
    proto_atom = XInternAtom(m_display, "WM_PROTOCOLS", False);
    delete_atom = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    firstWindow = false;
  }
  if ((proto_atom != None) && (delete_atom != None))
    XChangeProperty(m_display, m_window, proto_atom, XA_ATOM, 32,
		    PropModeReplace, (unsigned char *)&delete_atom, 1);
  bitmapActive = false;
  m_bitmap_contents = NULL;
}

void XWindow::Raise() {
  XRaiseWindow(m_display, m_window);
  XFlush(m_display);
}

void XWindow::Show() {
  XSelectInput ( m_display, m_window, ExposureMask | 
		 ButtonPressMask | ButtonReleaseMask | 
		 StructureNotifyMask | ButtonMotionMask);
  XMapWindow( m_display, m_window );
  XFlush(m_display);
  OnResize(m_width,m_height);
}
				 
void XWindow::Hide() {
  XUnmapWindow( m_display, m_window );
  XFlush( m_display );
}

XWindow::~XWindow() {
  Hide();
  XSelectInput ( m_display, m_window, NoEventMask );
  XFreeGC(m_display, m_gc);
  XDestroyWindow(m_display, m_window);
  UnregisterXWindow(this);
}

void XWindow::OnExpose(int x, int y, int w, int h) {
  if ((m_type == BitmapWindow)  && bitmapActive)
    XCopyArea(m_display, m_pixmap, m_window, m_gc,
  	      x, y, w, h, x, y);
  else {
    XGC xgc(m_display,m_window,m_gc,m_width, m_height);
    OnDraw(xgc);
  }
}

void XWindow::Refresh() {
  OnResize(m_width, m_height);
  XFlush(m_display);
}

void XWindow::OnMouseUp(int x, int y) {
  switch(m_state) {
  case state_box_anchored:
    m_box_x2 = x;
    m_box_y2 = y;
    m_state = state_normal;
    int cx, cy, cwidth, cheight;
    // Re-sort the coordinates for drawing the rectangle...
    cx = MIN(m_box_x1,m_box_x2);
    cy = MIN(m_box_y1,m_box_y2);
    cwidth = abs(m_box_x2 - m_box_x1)+1;
    cheight = abs(m_box_y2 - m_box_y1)+1;
    EraseRectangle(cx,cy,cwidth,cheight);
  }
}

void XWindow::EraseRectangle(int cx, int cy, int cwidth, int cheight) {
    XCopyArea(m_display, m_pixmap, m_window, m_gc,
	      cx-2, cy, 5, cheight, cx-2, cy);
    XCopyArea(m_display, m_pixmap, m_window, m_gc,
	      cx, cy-2, cwidth, 5, cx, cy-2);
    XCopyArea(m_display, m_pixmap, m_window, m_gc,
	      cx+cwidth-2, cy, 5, cheight, cx+cwidth-2, cy);
    XCopyArea(m_display, m_pixmap, m_window, m_gc,
	      cx, cy+cheight-2, cwidth, 5, cx, cy+cheight-2);
}

void XWindow::SetSize(int w, int h) {
  m_width = w;
  m_height = h;
  XResizeWindow(m_display, m_window, w, h);
}

void XWindow::OnDrag(int x, int y) {
  if (m_state == state_box_anchored) {
    int cx, cy, cwidth, cheight;
    // Re-sort the coordinates for drawing the rectangle...
    cx = MIN(m_box_x1,m_box_x2);
    cy = MIN(m_box_y1,m_box_y2);
    cwidth = abs(m_box_x2 - m_box_x1)+1;
    cheight = abs(m_box_y2 - m_box_y1)+1;
    EraseRectangle(cx,cy,cwidth,cheight);
    // Draw the new rectangle
    m_box_x2 = x;
    m_box_y2 = y;
    cx = MIN(m_box_x1,m_box_x2);
    cy = MIN(m_box_y1,m_box_y2);
    cwidth = abs(m_box_x2 - m_box_x1);
    cheight = abs(m_box_y2 - m_box_y1);
    XSetForeground(m_display, m_gc, WhitePixel(m_display, 0));
    XDrawRectangle(m_display, m_window, m_gc, 
		   cx, cy, cwidth, cheight);
  }
}

void XWindow::OnMouseDown(int x, int y) {
  switch(m_state) {
  case state_normal:
    break;
  case state_click_waiting:
    m_state = state_normal; 
    m_clickx = x;
    m_clicky = y;
    break;
  case state_box_start:
    m_state = state_box_anchored;
    m_box_x1 = x;
    m_box_y1 = y;
    m_box_x2 = x;
    m_box_y2 = y;
    break;
  default:
    break;
  }
}

void XWindow::OnResize(int w, int h) {
  if (w == 0 || h == 0) return;
  m_width = w;
  m_height = h;
  if (m_type == BitmapWindow) {
    XGC xgc(m_display, m_window, m_gc, m_width, m_height);
    OnDraw(xgc);
  }
  OnSize();
  XClearArea(m_display, m_window, 0, 0, w, h, True);
  XFlush(m_display);
}

void XWindow::SetTitle(std::string title) {
  char *tmp;
  tmp = (char*) title.c_str();
  if (XStringListToTextProperty(&tmp,1,&m_window_title) == 0)
    throw FreeMat::Exception("Unable to set window-title!\n");
  XSetWMName(m_display, m_window, &m_window_title);
  XSetWMIconName(m_display, m_window, &m_window_title);
}

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

void XWindow::SetImagePseudoColor(unsigned char *data, int width, int height) {
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
  m_cmap = XCreateColormap(m_display, RootWindow(m_display, 0), 
  			   m_visual, AllocAll);
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
  XImage *m_image = XCreateImage(m_display, m_visual, ddepth, ZPixmap, 0,
				 ddata, width, height, 8, 0);
  for (int y=0; y<height; y++)
    for (int x=0; x<width; x++)
      XPutPixel(m_image,x,y,*source_data++);
  // Convert the image to a pixmap
  XSync(m_display, False);
  if (m_pixmap != None)
    XFreePixmap(m_display, m_pixmap);
  m_pixmap = XCreatePixmap(m_display, m_window, 
			   width, height, ddepth);
  // Create a graphics context
  GC gc;
  XGCValues gcv;
  gcv.function = GXcopy;
  gc = XCreateGC(m_display, m_pixmap, GCFunction, &gcv);
  XPutImage(m_display, m_pixmap, gc, m_image, 0, 0, 0, 0, width, height);
  XFreeGC(m_display, gc);
  XDestroyImage(m_image);
  XSetWindowColormap(m_display, m_window, m_cmap);
  bitmapActive = true;
}

int GetShiftFromMask(int mask) {
  if (mask == 0) return 0;
  int shift = 0;
  while (!(mask & 1)) {
    shift++;
    mask >>= 1;
  }
  return shift;
}

void XWindow::PrintMe(std::string filename) {
  XSync(m_display, False);  
  if (m_type == BitmapWindow && m_bitmap_contents == NULL)
    throw FreeMat::Exception("Cannot print empty image window!\n");
  // Logic to detect print mode..
  int np;
  unsigned char *data;
  np = filename.find_last_of(".");
  if (np > 0) {
    std::string extension(filename.substr(np));
    transform (extension.begin(), extension.end(), 
	       extension.begin(), tolower);
    if (extension == ".eps" || extension == ".ps") {
      if (m_type == VectorWindow) {
	PostScriptGC gc(filename, m_width, m_height);
	OnDraw(gc);
      } else
	WriteEPSFile(filename, m_bitmap_contents, m_width, m_height);
    } else {
      if (m_type == VectorWindow) {
	Pixmap pmap;
	pmap = XCreatePixmap(m_display, m_window, m_width, m_height, 
			     DefaultDepth(m_display, 0));
	GC gc;
	XGCValues gcv;
	gcv.function = GXcopy;
	gc = XCreateGC(m_display, pmap, GCFunction, &gcv);
	XGC xgc(m_display, pmap, gc, m_width, m_height);
	OnDraw(xgc);
	// Ask the server to capture the contents of the pixmap into
	// an XImage and send it back to us...
	XImage *img = XGetImage(m_display, pmap, 0, 0, m_width, m_height,
				~0, ZPixmap);
	data = (unsigned char*) malloc(3*sizeof(char)*m_width*m_height);
	// What we do now depends on the visual type.  For pseudocolor
	// visuals, we retrieve the current colormap
	if (m_visual->c_class == PseudoColor) {
	  XColor *cvals = (XColor*) malloc(sizeof(XColor)*m_visual->map_entries);
	  for (int m=0;m<m_visual->map_entries;m++)
	    cvals[m].pixel = m;
	  XQueryColors(m_display, DefaultColormap(m_display, 0), cvals, 
		       m_visual->map_entries);
	  // Then we directly convert the image data...	
	  for (int y=0;y<m_height;y++)
	    for (int x=0;x<m_width;x++) {
	      unsigned long pixel = XGetPixel(img, x, y);
	      data[3*(y*m_width+x)] = cvals[pixel].red >> 8;
	      data[3*(y*m_width+x)+1] = cvals[pixel].green >> 8;
	      data[3*(y*m_width+x)+2] = cvals[pixel].blue >> 8;
	    }
	  free(cvals);
	} else {
	  // For TrueColor and DirectColor, we do the pixel conversion
	  // manually - we assume that there are no more than 8 bits
	  // per primary...
	  unsigned int red_mask, green_mask, blue_mask;
	  unsigned int red_shift, green_shift, blue_shift;
	  float red_scale, green_scale, blue_scale;
	  red_mask = m_visual->red_mask;
	  green_mask = m_visual->green_mask;
	  blue_mask = m_visual->blue_mask;
	  red_shift = GetShiftFromMask(red_mask);
	  green_shift = GetShiftFromMask(green_mask);
	  blue_shift = GetShiftFromMask(blue_mask);
	  red_scale = 255.0/(red_mask >> red_shift);
	  green_scale = 255.0/(green_mask >> green_shift);
	  blue_scale = 255.0/(blue_mask >> blue_shift);
	  for (int y=0;y<m_height;y++)
	    for (int x=0;x<m_width;x++) {
	      unsigned long pixel = XGetPixel(img, x, y);
	      data[3*(y*m_width+x)] = red_scale*((pixel & red_mask) >> red_shift);
	      data[3*(y*m_width+x)+1] = green_scale*((pixel & green_mask) >> 
						     green_shift);
	      data[3*(y*m_width+x)+2] = blue_scale*((pixel & blue_mask) >>
						    blue_shift);
	    }
	}
	XDestroyImage(img);
	XFreeGC(m_display, gc);
      } else {
	data = m_bitmap_contents;
      }
      if (extension == ".jpeg" || extension == ".jpg") 
	WriteJPEGFile(filename, data, m_width, m_height);
      else if (extension == ".png")
	WritePNGFile(filename, data, m_width, m_height);
      else if (extension == ".tiff" || extension == ".tif")
	WriteTIFFFile(filename, data, m_width, m_height);      
      if (m_type == VectorWindow)
	free(data);
    }
  } else
    throw FreeMat::Exception(std::string("Unable to determine format of output from filename"));
}

// Set the image contents of the window - source is an RGB image
// Depending on the visual, this image is converted into an XImage
// and a colormap is set for the window.
void XWindow::SetImage(unsigned char *data, int width, int height) {
  m_bitmap_contents = data;
  // Check for PseudoColor visual
  if ((m_visual->c_class != TrueColor) &&
      (m_visual->c_class != DirectColor)) {
    SetImagePseudoColor(data,width,height);
    return;
  }
  int ddepth = DefaultDepth(m_display, 0);
  int dbits = bitsPerPixelAtDepth(m_display,0,ddepth);
  int dpixlen = (dbits + 7)/8;
  char *ddata = (char*) malloc(width*height*dpixlen);
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
  // Convert the image to a pixmap
  XSync(m_display, False);
  if (m_pixmap != None)
    XFreePixmap(m_display, m_pixmap);
  m_pixmap = XCreatePixmap(m_display, m_window, 
			   width, height, ddepth);
  // Create a graphics context
  GC gc;
  XGCValues gcv;
  gcv.function = GXcopy;
  gc = XCreateGC(m_display, m_pixmap, GCFunction, &gcv);
  XPutImage(m_display, m_pixmap, gc, m_image, 0, 0, 0, 0, width, height);
  XFreeGC(m_display, gc);
  XDestroyImage(m_image);  
  bitmapActive = true;
}

void XWindow::GetClick(int &x, int &y) {
  // Set the state
  int cursor_shape = XC_crosshair;
  Cursor cursor;
  cursor = XCreateFontCursor(m_display, cursor_shape);
  XDefineCursor(m_display, m_window, cursor);
  m_state = state_click_waiting;
  while (m_state != state_normal)
    DoEvents();
  x = m_clickx;
  y = m_clicky;
  XUndefineCursor(m_display, m_window);
}

int XWindow::GetState() {
  return m_state;
}

void XWindow::GetBox(int &x1, int &y1, int &x2, int &y2) {
  int cursor_shape = XC_crosshair;
  Cursor cursor;
  cursor = XCreateFontCursor(m_display, cursor_shape);
  XDefineCursor(m_display, m_window, cursor);
  m_state = state_box_start;
  while (m_state != state_normal) 
    DoEvents();
  x1 = m_box_x1;
  x2 = m_box_x2;
  y1 = m_box_y1;
  y2 = m_box_y2;
  XUndefineCursor(m_display, m_window);
}

void CheckDeleteQ() {
#if 0
  // Check for windows to delete...
  for (int i=0;i<deleteQ.size();i++) {
    deleteQ[i]->OnClose();
    delete deleteQ[i];
  }
  deleteQ.clear();
#endif
}

bool XNextEventStdInCallback(Display *d, XEvent *r) {
  CheckDeleteQ();
  if (XPending(d)) {
    XNextEvent(d,r);
    return true;
  }
  if (!stdincb_active) {
    XNextEvent(d,r);
    return true;
  } else {
    // Do a select on the X connection and stdin
    bool XEventCaught = false;
    int xfd;
    fd_set rmask, emask;
    FD_ZERO(&rmask);
    xfd = ConnectionNumber(d);
    FD_SET(xfd, &rmask);
    FD_SET(STDIN_FILENO, &rmask);
    FD_ZERO(&emask);
    FD_SET(STDIN_FILENO, &emask);
    int ret;
    ret = select(xfd+1, &rmask, NULL, &emask, NULL);
    if (ret == -1) {
      if (errno != EINTR)
	perror("select");
      else
	/* Do nothing */
	;
    } else {
      if (FD_ISSET(STDIN_FILENO, &emask)) {
	perror("stdin");
	printf("\r\nException!\r\n");
	fflush(stdout);
      }
      if (FD_ISSET(STDIN_FILENO, &rmask))
	stdin_cb();
      if (FD_ISSET(xfd, &rmask)) {
	XNextEvent(d,r);
	XEventCaught = true;
      }
    }
    return XEventCaught;
  }
}

void DoEvents() {
  XEvent report;
  XWindow *p, *q;

  // Get the event
  if (XNextEventStdInCallback(theDisplay, &report)) {
    // Lookup the window
    if (winlist.count(report.xany.window) == 0)
      return;
    p = winlist[report.xany.window];
    switch(report.type) {
    case Expose:
      if (p->GetWindowType() == VectorWindow) {
	XEvent repcheck;
	while (XCheckMaskEvent(theDisplay,ExposureMask,&repcheck)) {
	  if (report.xany.window != repcheck.xany.window) {
	    if (winlist.count(report.xany.window) != 0) {
	      q = winlist[repcheck.xany.window];
	      q->OnExpose(repcheck.xexpose.x,repcheck.xexpose.y,
			  repcheck.xexpose.width,repcheck.xexpose.height);
	    }
	  }
	}
      }
      p->OnExpose(report.xexpose.x,report.xexpose.y,
		  report.xexpose.width,report.xexpose.height);
      break;
    case ButtonPress:
      p->OnMouseDown(report.xbutton.x, report.xbutton.y);
      break;
    case ButtonRelease:
      p->OnMouseUp(report.xbutton.x, report.xbutton.y);
      break;
    case MotionNotify:
    p->OnDrag(report.xmotion.x, report.xmotion.y);
    break;
    case ConfigureNotify: 
      XEvent repcheck;
      while (XCheckMaskEvent(theDisplay,StructureNotifyMask,&repcheck))
	if (repcheck.type == ConfigureNotify) report = repcheck;
      if (report.xconfigure.width != p->getWidth() ||
	  report.xconfigure.height != p->getHeight())
	p->OnResize(report.xconfigure.width, report.xconfigure.height);
      break;
    case ClientMessage:
      if (report.xclient.data.l[0] == delete_atom) {
	if (p->GetState() == state_normal) {
	  delete p;
	}
      }
      break;
    }
  }
}

void Run() {
  while(XPending(theDisplay)) {
    DoEvents();
  }
}

void FlushWindowEvents() {
  Run();
}

void CloseXWindow(XWindow *p) {
  delete p;
}
