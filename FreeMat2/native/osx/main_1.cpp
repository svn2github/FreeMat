#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>
#include <string>
#include <iostream>

#define NEVENT 10
EventTypeSpec commSpec[NEVENT] = {
  {kEventClassCommand, kEventProcessCommand},
  {kEventClassKeyboard, kEventRawKeyDown},
  {kEventClassKeyboard, 1234},
  {kEventClassWindow, kEventWindowResizeCompleted},
  {kEventClassWindow, kEventWindowZoomed},
  {kEventClassWindow, kEventWindowClosed},
  {kEventClassWindow, kEventWindowDrawContent},
  {kEventClassWindow, kEventWindowClickContentRgn },
  {kEventClassMouse, kEventMouseUp },
  {kEventClassMouse, kEventMouseDragged }
};

class XPWindow {
 public:
  WindowRef win;
  ControlHandle vScrollBar;
  int m_width, m_height;
  int m_scrollmin, m_scrollmax, m_scrollline, m_scrollpage, m_scrollval;
 public:
  XPWindow(int width, int height, std::string title);
  ~XPWindow();
  void CreateVerticalScrollBar(int scrollmin, int scrollmax, int scrollline, int scrollpage, int scrollval);
  void OnDraw();
  void OnResize();
  void Show();
  void Hide();
  void Erase();
  void Raise();
  void SetTitle(std::string title);
  void OnMouseDown(int x, int y);
  void OnMouseDrag(int x, int y);
  void OnMouseUp(int x, int y);
  // Scroll commands
  void ScrollLineUp();
  void ScrollLineDown();
  void ScrollPageUp();
  void ScrollPageDown();
  void SetScrollValue(int val);
};

static void refresh_cb(EventLookTimerRef ref, void *data) {
  XPWindow *win = (XPWindow*) data;
  data->Refresh();
  InstallEventLoopTimer(GetMainEventLoop(),.03,0,
			NewEventLoopTimerUPP(refresh_cb),
			NULL,NULL);
}


void XPWindow::OnMouseDown(int x, int y) {
  std::cout << "down " << x << "," << y << "\n";
}

void XPWindow::OnMouseDrag(int x, int y) {
  std::cout << "drag " << x << "," << y << "\n";
}

void XPWindow::OnMouseUp(int x, int y) {
  std::cout << "up " << x << "," << y << "\n";
}

pascal OSStatus XPWndProc(EventHandlerCallRef handlerRef,
			  EventRef event,
			  void *userData);

void XPWindow::Show() {
  ShowWindow(win);
}

void XPWindow::Hide() {
  HideWindow(win);
}

void XPWindow::SetTitle(std::string title) {
  CFStringRef str = CFStringCreateWithCString(NULL,title.c_str(),kCFStringEncodingMacRoman);
  SetWindowTitleWithCFString(win, str);
  CFRelease(str);
}

void XPWindow::ScrollLineUp() {
  m_scrollval -= m_scrollline;
  SetScrollValue(m_scrollval);
}

void XPWindow::ScrollLineDown() {
  m_scrollval += m_scrollline;
  SetScrollValue(m_scrollval);
}

void XPWindow::ScrollPageUp() {
  std::cout << "scroll up " << m_scrollpage << "\n";
  m_scrollval -= m_scrollpage;
  SetScrollValue(m_scrollval);
}

void XPWindow::ScrollPageDown() {
  m_scrollval += m_scrollpage;
  SetScrollValue(m_scrollval);
}

void XPWindow::SetScrollValue(int val) {
  val = (val < m_scrollmin) ? m_scrollmin : val;
  val = (val > m_scrollmax) ? m_scrollmax : val;
  m_scrollval = val;
  SetControl32BitValue(vScrollBar,m_scrollval);
  OnDraw();
  std::cout << "Scroll value " << val << "\n";
}

static void vscroll_f(ControlRef ref, ControlPartCode part) {
  XPWindow *winptr;
  UInt32 sze;
  GetControlProperty(ref, 0, 1001, sizeof(XPWindow*), &sze, &winptr);
  std::cout << "click part = " << part << "\n";
  switch(part) {
  case kControlUpButtonPart:
    winptr->ScrollLineUp();
    break;
  case kControlDownButtonPart:
    winptr->ScrollLineDown();
    break;
  case kControlPageUpPart:
    winptr->ScrollPageUp();
    break;
  case kControlPageDownPart:
    winptr->ScrollPageDown();
    break;
  case 129: // thumb
    winptr->SetScrollValue(GetControl32BitValue(ref));
    break;
  }
}

void XPWindow::CreateVerticalScrollBar(int scrollmin, int scrollmax, 
				       int scrollline, int scrollpage, 
				       int scrollval) {
  Rect r;
  r.left = m_width - 15;
  r.right = m_width;
  r.top = 0;
  r.bottom = m_height - 15;
  CreateScrollBarControl(win, &r, scrollval, scrollmin, scrollmax, 
			 scrollline, TRUE, vscroll_f, &vScrollBar);
  m_scrollval = scrollval;
  m_scrollmin = scrollmin;
  m_scrollmax = scrollmax;
  m_scrollline = scrollline;
  m_scrollpage = scrollpage;
  XPWindow *ptr = this;
  SetControlProperty(vScrollBar, 0, 1001, sizeof(XPWindow*), &ptr);
  DrawControls(win);
}

void XPWindow::Raise() {
  std::cout << "raising " << this << "\n";
  SelectWindow(win);
}

void XPWindow::Erase() {
  RgnHandle rgn;
  SetPortWindowPort(win);
  rgn = NewRgn();
  GetClip(rgn);
  Rect r;
  r.left = 0;
  r.top = 0;
  r.right = m_width-15;
  r.bottom = m_height;
  ClipRect(&r);
  EraseRect(&r);
  SetClip(rgn);
  DisposeRgn(rgn);
}

void XPWindow::OnDraw() {
  Erase();
  CGContextRef gh;
  CGRect r2 = CGRectMake(0, 0, m_width-15, m_height);
  CreateCGContextForPort(GetWindowPort(win),&gh);
  CGContextBeginPath(gh);
  CGContextMoveToPoint(gh,0,0);
  CGContextAddLineToPoint(gh,m_width-15,m_height-1);
  CGContextClosePath(gh);
  CGContextStrokePath(gh);
  CGContextBeginPath(gh);
  CGContextMoveToPoint(gh,0,m_height-1);
  CGContextAddLineToPoint(gh,m_width-15,0);
  CGContextClosePath(gh);
  CGContextStrokePath(gh);
  CGContextSelectFont(gh,"Monaco",12,kCGEncodingMacRoman);
  CGContextSetTextPosition(gh, m_width/2, m_height/2);
  CGContextShowText(gh,"Hello World!",12);
  for (int i=0;i<50;i++) {
    char buffer[100];
    CGContextSetTextPosition(gh,0,i*12);
    sprintf(buffer,"line %d",i+m_scrollval);
    CGContextShowText(gh,buffer,strlen(buffer));
  }
  CGContextFlush(gh);
  CGContextRelease(gh);
  DrawControls(win);
}

void XPWindow::OnResize() {
  Rect r;
  GetWindowBounds(win, kWindowContentRgn, &r);
  m_width = r.right - r.left;
  m_height = r.bottom - r.top;
  DisposeControl(vScrollBar);
  Erase();
  CreateVerticalScrollBar(m_scrollmin,m_scrollmax,m_scrollline,m_scrollpage,m_scrollval);
  DrawControls(win);
  OnDraw();
}

XPWindow::XPWindow(int width, int height, std::string title) {
  Rect winSize;
  m_width = width;
  m_height = height;
  winSize.top = 50;
  winSize.left = 50;
  winSize.bottom = height+50;
  winSize.right = width+50;
  CreateNewWindow(kDocumentWindowClass,
		  kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute,
		  &winSize,
		  &win);
  InstallWindowEventHandler(win, NewEventHandlerUPP(XPWndProc),
  			    NEVENT,commSpec, this, NULL);
}

#define EVMATCH(a,b) ((yclass == a) && (kind == b))

pascal OSStatus XPWndProc(EventHandlerCallRef handlerRef,
			EventRef event,
			void *userData) {
  int yclass, kind;
  XPWindow *winptr;
  short pt[2];
  UInt32 sz;

  yclass = GetEventClass(event);
  kind = GetEventKind(event);
  winptr = (XPWindow*) userData;

  if EVMATCH(kEventClassWindow,kEventWindowResizeCompleted)
    winptr->OnResize();
  if EVMATCH(kEventClassWindow,kEventWindowDrawContent)
    winptr->OnDraw();
  if EVMATCH(kEventClassWindow,kEventWindowZoomed)
    winptr->OnResize();
  if EVMATCH(kEventClassWindow,kEventWindowClosed)
    exit(0);
  if EVMATCH(kEventClassWindow,kEventWindowClickContentRgn) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    winptr->OnMouseDown(pt[0],pt[1]);
  }
  if EVMATCH(kEventClassMouse,kEventMouseDragged) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    winptr->OnMouseDrag(pt[0],pt[1]);
  }
  if EVMATCH(kEventClassMouse,kEventMouseUp) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    winptr->OnMouseUp(pt[0],pt[1]);
  }
  return eventNotHandledErr;
}


int main(int arc, char* argv[]) {
  XPWindow *win = new XPWindow(500,300,"main window");
  win->SetTitle("Main Window");
  win->CreateVerticalScrollBar(0,100,1,10,0);
  win->Raise();
  win->Show();
  RunApplicationEventLoop();
}
