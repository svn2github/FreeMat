#include <FL/Fl.H>
#include "XWindow.hpp"
#include "Reducer.hpp"
#include <map>
#include <vector>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "PostScriptGC.hpp"
#include "Exception.hpp"
#include "BitmapPrinterGC.hpp"
#include "XGC.hpp"

static bool firstWindow = true;
Atom proto_atom, delete_atom;
bool stdincb_active = false;
fdcallback stdin_cb;
static Display *theDisplay = NULL;
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

XWindow::XWindow(Rect2D sze) : XPWidget(NULL, sze) { 
  if (!theDisplay)
    throw FreeMat::Exception("Graphics not available!");
  m_display = theDisplay;
  XColor t;
  t.red = 211 * 257;
  t.green = 211 * 257;
  t.blue = 211 * 257;
  XAllocColor(m_display, DefaultColormap(m_display, 0), &t);
  m_window = XCreateSimpleWindow(m_display, RootWindow(m_display, 0),
				 0, 0, sze.width, sze.height, 0, 
				 BlackPixel(m_display, 0),
				 t.pixel);
  m_visual = DefaultVisual(m_display, 0);
  RegisterXWindow(this);
  XGCValues values;
  values.function = GXcopy;
  unsigned long valuemask = GCFunction;
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

void XWindow::Raise() {
  XRaiseWindow(m_display, m_window);
  XFlush(m_display);
}

void XWindow::Show() {
  XSelectInput ( m_display, m_window, ExposureMask | 
		 ButtonPressMask | ButtonReleaseMask | 
		 StructureNotifyMask | ButtonMotionMask | 
		 KeyPressMask);
  XMapWindow( m_display, m_window );
  XClearArea(m_display, m_window, 0, 0, bounds.width, bounds.height, True);
  XFlush(m_display);
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

void XWindow::OnChar(char key) {
  if (m_child)
    m_child->OnChar(key);
}

void XWindow::Refresh(Rect2D rct) {
  XClearArea(m_display, m_window, rct.x1, rct.y1, rct.width, rct.height, True);
  XFlush(m_display);
}

void XWindow::OnMouseDown(Point2D pt) {
  if (m_child)
    m_child->OnMouseDown(pt);
}

void XWindow::OnMouseUp(Point2D pt) {
  if (m_child)
    m_child->OnMouseUp(pt);
}

void XWindow::OnMouseDrag(Point2D pt) {
  if (m_child)
    m_child->OnMouseDrag(pt);
}

void XWindow::OnDraw(GraphicsContext& gc, Rect2D rect) {
  printf("Draw %d %d %d %d\r\n",
	 rect.x1, rect.y1, rect.width, rect.height);
  if (m_child)
    m_child->OnDraw(gc, rect);
}

void XWindow::OnResize(Point2D pt) {
  if (m_child)
    m_child->OnResize(pt);
}

void XWindow::SetTitle(std::string title) {
  char *tmp;
  tmp = (char*) title.c_str();
  if (XStringListToTextProperty(&tmp,1,&m_window_title) == 0)
    throw FreeMat::Exception("Unable to set window-title!\n");
  XSetWMName(m_display, m_window, &m_window_title);
  XSetWMIconName(m_display, m_window, &m_window_title);
}

void XWindow::OnExpose(int x1, int y1, int width, int height) {
  
  XGC xgc(m_display, m_visual, m_window, m_gc, bounds.width, bounds.height);
  Rect2D rect;
  rect.x1 = x1; rect.y1 = y1; rect.width = width; rect.height = height;
  OnDraw(xgc,rect);
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
  Fl::wait(0);
  return;
  if (theDisplay) {
    XEvent report;
    XWindow *p, *q;
    
    // Get the event
    if (XNextEventStdInCallback(theDisplay, &report)) {
      // Lookup the window
      if (winlist.count(report.xany.window) == 0)
      return;
      p = winlist[report.xany.window];
      switch(report.type) {
      case Expose: {
	p->OnExpose(report.xexpose.x,report.xexpose.y,
		    report.xexpose.width,report.xexpose.height);
      }
	break;
      case KeyPress: {
	KeySym key_symbol = XKeycodeToKeysym(theDisplay, 
					     report.xkey.keycode,0);
	if (key_symbol >= XK_A && key_symbol <= XK_Z) 
	  p->OnChar(key_symbol - XK_A + 'A');
	else if (key_symbol >= XK_a && key_symbol <= XK_z)
	  p->OnChar(key_symbol - XK_a + 'a');
	else if (key_symbol >= XK_0 && key_symbol <= XK_9)
	  p->OnChar(key_symbol - XK_0 + '0');
	break;
      }
      case ButtonPress:
      p->OnMouseDown(Point2D(report.xbutton.x, report.xbutton.y));
      break;
      case ButtonRelease:
	p->OnMouseUp(Point2D(report.xbutton.x, report.xbutton.y));
	break;
      case MotionNotify:
	p->OnMouseDrag(Point2D(report.xmotion.x, report.xmotion.y));
	break;
      case ConfigureNotify: 
	XEvent repcheck;
	while (XCheckMaskEvent(theDisplay,StructureNotifyMask,&repcheck))
	  if (repcheck.type == ConfigureNotify) report = repcheck;
	if (report.xconfigure.width != p->getWidth() ||
	    report.xconfigure.height != p->getHeight())
	  p->OnResize(Point2D(report.xconfigure.width, report.xconfigure.height));
	break;
      case ClientMessage:
	if (report.xclient.data.l[0] == delete_atom) {
	  delete p;
	}
	break;
      }
    }
  } else {
    fd_set rmask, emask;
    FD_ZERO(&rmask);
    FD_SET(STDIN_FILENO, &rmask);
    FD_ZERO(&emask);
    FD_SET(STDIN_FILENO, &emask);
    int ret;
    ret = select(STDIN_FILENO+1, &rmask, NULL, &emask, NULL);
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
    }
  }
}

void Run() {
  Fl::wait(0);
}

void FlushWindowEvents() {
  Fl::wait(0);
}

void CloseXWindow(XWindow *p) {
  delete p;
}
