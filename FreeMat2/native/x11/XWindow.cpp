#include "XWindow.hpp"
#include "Reducer.hpp"
#include <map>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

static bool firstWindow = true;
Atom proto_atom, delete_atom;
bool stdincb_active = false;
fdcallback stdin_cb;
static Display *theDisplay;

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

XWindow::XWindow() {
  m_display = theDisplay;
  m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, 0),
				 0, 0, 300, 300, 0, 
				 BlackPixel(m_display, 0),
				 WhitePixel(m_display, 0));
  m_visual = DefaultVisual(m_display, 0);
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
}

void XWindow::Show() {
  XSelectInput ( m_display, m_window, ExposureMask | 
		 ButtonPressMask | ButtonReleaseMask | 
		 StructureNotifyMask | ButtonMotionMask);
  XMapWindow( m_display, m_window );
  XFlush(m_display);
  OnResize(300,300);
}
				 
void XWindow::Hide() {
  XUnmapWindow( m_display, m_window );
  XFlush( m_display );
}

XWindow::~XWindow() {
  XSelectInput ( m_display, m_window, NoEventMask );
  //  Hide();
  XFreeGC(m_display, m_gc);
  XDestroyWindow(m_display, m_window);
  UnregisterXWindow(this);
}

void XWindow::OnExpose(int x, int y, int w, int h) {
  if (m_pixmap != None)
    XCopyArea(m_display, m_pixmap, m_window, m_gc,
	      x, y, w, h, x, y);
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

void XWindow::OnClose() {
  printf("Window %x closed for business!\n",this);
}

void XWindow::OnResize(int w, int h) {
  m_width = w;
  m_height = h;
  // Update the contents.
  unsigned char *data;
  data = (unsigned char *) malloc(3*sizeof(char)*w*h);
  UpdateContents(data,w,h);
  SetImage(data,w,h);
  free(data);
  OnExpose(0,0,w,h);
}

void XWindow::SetTitle(std::string title) {
  char *tmp;
  tmp = (char*) title.c_str();
  if (XStringListToTextProperty(&tmp,1,&m_window_title) == 0)
    throw "Unable to set window-title!\n";
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
}

// Set the image contents of the window - source is an RGB image
// Depending on the visual, this image is converted into an XImage
// and a colormap is set for the window.
void XWindow::SetImage(unsigned char *data, int width, int height) {
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
  float rratio, gratio, bratio;
  rratio = m_image->red_mask/255.0;
  gratio = m_image->green_mask/255.0;
  bratio = m_image->blue_mask/255.0;
  unsigned long pixval;
  unsigned char red, green, blue;
  unsigned char *source_data;
  source_data = data;
  for (int y=0;y<height;y++)
    for (int x=0;x<width;x++) {
      red = *source_data++;
      green = *source_data++;
      blue = *source_data++;
      pixval = ((unsigned long) (red * rratio)) & m_image->red_mask |
	((unsigned long) (green * gratio)) & m_image->green_mask |
	((unsigned long) (blue * bratio)) & m_image->blue_mask;
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
}

void XWindow::GetClick(int &x, int &y) {
  // Set the state
  int cursor_shape = XC_crosshair;
  Cursor cursor;
  cursor = XCreateFontCursor(m_display, cursor_shape);
  XDefineCursor(m_display, m_window, cursor);
  m_state = state_click_waiting;
  while (m_state != state_normal)
    DoEvents(m_display);
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
    DoEvents(m_display);
  x1 = m_box_x1;
  x2 = m_box_x2;
  y1 = m_box_y1;
  y2 = m_box_y2;
  XUndefineCursor(m_display, m_window);
}

void XNextEventStdInCallback(Display *d, XEvent *r) {
  if (XPending(d)) {
    XNextEvent(d,r);
    return;
  }
  if (!stdincb_active)
    XNextEvent(d,r);
  else {
    bool eventCaught = false;
    while (!eventCaught) {
      // Do a select on the X connection and stdin
      int xfd;
      fd_set rmask;
      FD_ZERO(&rmask);
      xfd = ConnectionNumber(d);
      FD_SET(xfd, &rmask);
      FD_SET(STDIN_FILENO, &rmask);
      if ((select(xfd+1, &rmask, NULL, NULL, NULL) == -1) && (errno != EINTR))
	perror("select");
      if (FD_ISSET(STDIN_FILENO, &rmask))
	stdin_cb();
      if (FD_ISSET(xfd, &rmask)) {
	XNextEvent(d,r);
	eventCaught = true;
      }
    }
  }
}

void DoEvents(Display *d) {
  XEvent report;
  XWindow *p;

  // Get the event
  XNextEventStdInCallback(d, &report);
  // Lookup the window
  if (winlist.count(report.xany.window) == 0)
    fprintf(stderr,"Warning! Unknown window handle %d in event... event ignored\n",report.xany.window);
  p = winlist[report.xany.window];
  switch(report.type) {
  case Expose:
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
    while (XCheckMaskEvent(d,StructureNotifyMask,&report));
    if (report.xconfigure.width != p->getWidth() ||
	report.xconfigure.height != p->getHeight())
      p->OnResize(report.xconfigure.width, report.xconfigure.height);
    break;
  case ClientMessage:
    if (report.xclient.data.l[0] == delete_atom) {
      if (p->GetState() == state_normal) {
	p->OnClose();
	delete p;
      }
    }
    break;
  }
}

void Run(Display *d) {
  while(1) {
    DoEvents(d);
  }
}
