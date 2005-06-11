#include "mt.hpp"

#define REFRESHTIME .03
#define BLINKTIME 1.0

#define NEVENT 8
EventTypeSpec commSpec[NEVENT] = {
  {kEventClassKeyboard, kEventRawKeyDown},
  {kEventClassWindow, kEventWindowResizeCompleted},
  {kEventClassWindow, kEventWindowZoomed},
  {kEventClassWindow, kEventWindowClosed},
  {kEventClassWindow, kEventWindowDrawContent},
  {kEventClassWindow, kEventWindowClickContentRgn },
  {kEventClassMouse, kEventMouseUp },
  {kEventClassMouse, kEventMouseDragged }
};

pascal OSStatus OSXTerminalWndProc(EventHandlerCallRef handlerRef,
				   EventRef event,
				   void *userData);

OSXTerminal::~OSXTerminal() {
  DisposeWindow(win);
}

OSXTerminal::OSXTerminal(int width, int height, std::string title) {
  Rect winSize;
  m_client_width = width - 15;
  m_client_height = height;
  winSize.top = 50;
  winSize.left = 50;
  winSize.bottom = height+50;
  winSize.right = width+50;
  CreateNewWindow(kDocumentWindowClass,
		  kWindowStandardDocumentAttributes | 
		  kWindowStandardHandlerAttribute,
		  &winSize,
		  &win);
  InstallWindowEventHandler(win, NewEventHandlerUPP(OSXTerminalWndProc),
  			    NEVENT,commSpec, this, NULL);
  Initialize();
  ShowWindow(win);
  m_scrollinit = false;
  OnResize(m_client_width,m_client_height);
}

int OSXTerminal::GetHeight() {
  return m_client_height;
}

int OSXTerminal::GetWidth() {
  return m_client_width;
}

static void refresh_cb(EventLoopTimerRef ref, void *data) {
  OSXTerminal *win = (OSXTerminal*) data;
  win->DrawContent();
  InstallEventLoopTimer(GetMainEventLoop(),REFRESHTIME,0,
			NewEventLoopTimerUPP(refresh_cb),
			win,NULL);
}

static void blink_cb(EventLoopTimerRef ref, void *data) {
  OSXTerminal *win = (OSXTerminal*) data;
  win->blink();
  InstallEventLoopTimer(GetMainEventLoop(),BLINKTIME,0,
			NewEventLoopTimerUPP(blink_cb),
			win,NULL);
}

void OSXTerminal::InstallEventTimers() {
  std::cout << "installing event timers\n";
  InstallEventLoopTimer(GetMainEventLoop(),REFRESHTIME,0,
			NewEventLoopTimerUPP(refresh_cb),
			this,NULL);
  InstallEventLoopTimer(GetMainEventLoop(),BLINKTIME,0,
			NewEventLoopTimerUPP(blink_cb),
			this,NULL);
}

void OSXTerminal::ScrollLineUp() {
  SetScrollBarValue(m_scrollval-1);
}

void OSXTerminal::ScrollLineDown() {
  SetScrollBarValue(m_scrollval+1);
}

void OSXTerminal::SetScrollBarValue(int val) {
  val = (val < m_scrollmin) ? m_scrollmin : val;
  val = (val > m_scrollmax) ? m_scrollmax : val;
  m_scrollval = val;
  SetControl32BitValue(vScrollBar,m_scrollval);
  OnScroll(val);
}

void OSXTerminal::ScrollPageUp() {
  m_scrollval -= m_scrollpage;
  SetScrollBarValue(m_scrollval);  
}

void OSXTerminal::ScrollPageDown() {
  m_scrollval += m_scrollpage;
  SetScrollBarValue(m_scrollval);    
}

static void vscroll_f(ControlRef ref, ControlPartCode part) {
  OSXTerminal *winptr;
  UInt32 sze;
  GetControlProperty(ref, 0, 1001, sizeof(OSXTerminal*), &sze, &winptr);
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
    winptr->SetScrollBarValue(GetControl32BitValue(ref));
    break;
  }
}

void OSXTerminal::SetupScrollBar(int scrollmin, int scrollmax, 
				 int scrollline, int scrollpage, 
				 int scrollval) {
  Rect r;
  r.left = m_client_width;
  r.right = m_client_width + 15;
  r.top = 0;
  r.bottom = m_client_height - 15;
  if (m_scrollinit)
    DisposeControl(vScrollBar);
  CreateScrollBarControl(win, &r, scrollval, scrollmin, scrollmax, 
			 scrollline, TRUE, vscroll_f, &vScrollBar);
  m_scrollinit = true;
  m_scrollval = scrollval;
  m_scrollmin = scrollmin;
  m_scrollmax = scrollmax;
  m_scrollline = scrollline;
  m_scrollpage = scrollpage;
  OSXTerminal *ptr = this;
  SetControlProperty(vScrollBar, 0, 1001, sizeof(OSXTerminal*), &ptr);
  DrawControls(win);
}

void OSXTerminal::BeginDraw() {
  CreateCGContextForPort(GetWindowPort(win),&gh);
  CGContextSelectFont(gh,"Monaco",12,kCGEncodingMacRoman);
}

void OSXTerminal::EndDraw() {
  CGContextFlush(gh);
  CGContextRelease(gh);
  DrawControls(win);
}

void OSXTerminal::PutTagChar(int x, int y, tagChar g) {
  CGRect fill = CGRectMake(x,m_height*m_char_h-m_char_h-y,m_char_w+1,m_char_h);
  CGContextSetTextPosition(gh,x,m_height*m_char_h-m_char_h-y);
  if (g.noflags()) {
    CGContextSetRGBFillColor(gh,1,1,1,1);
    CGContextFillRect(gh,fill);
    CGContextSetRGBFillColor(gh,0,0,0,1);
    CGContextShowText(gh,&g.v,1);
  } else if (g.cursor()) {
    CGContextSetRGBFillColor(gh,0,0,0,1);
    CGContextFillRect(gh,fill);
    CGContextSetRGBFillColor(gh,1,1,1,1);
    CGContextShowText(gh,&g.v,1);
  } else {
    CGContextSetRGBFillColor(gh,0,0,1,1);
    CGContextFillRect(gh,fill);
    CGContextSetRGBFillColor(gh,1,1,1,1);
    CGContextShowText(gh,&g.v,1);
  }
}

// Maybe get some help from:
// http://cvs.sourceforge.net/viewcvs.py/tom7misc/vstplugins/dfxgui_mac/dfxguidisplay.cpp?rev=1.29
void OSXTerminal::setFont(int size) {
  CGContextRef gh;
  char tst = 'w';
  CreateCGContextForPort(GetWindowPort(win),&gh);
  CGContextSelectFont(gh,"Monaco",size,kCGEncodingMacRoman);
  CGContextSetTextPosition(gh,0,0);
  CGContextSetRGBFillColor(gh,0,0,0,0);
  CGContextShowText(gh,&tst,1);
  CGPoint pt(CGContextGetTextPosition(gh));
  CGContextRelease(gh);
  m_char_w = pt.x+2;
  m_char_h = size+2;
}

void OSXTerminal::Erase() {
  RgnHandle rgn;
  SetPortWindowPort(win);
  rgn = NewRgn();
  GetClip(rgn);
  Rect r;
  r.left = 0;
  r.top = 0;
  r.right = m_client_width;
  r.bottom = m_client_height;
  ClipRect(&r);
  EraseRect(&r);
  SetClip(rgn);
  DisposeRgn(rgn);
}

void OSXTerminal::CreateVerticalScrollBar(int scrollmin, int scrollmax, 
				       int scrollline, int scrollpage, 
				       int scrollval) {
  Rect r;
  r.left = m_client_width;
  r.right = m_client_width + 15;
  r.top = 0;
  r.bottom = m_client_height - 15;
  if (m_scrollinit)
    DisposeControl(vScrollBar);
  CreateScrollBarControl(win, &r, scrollval, scrollmin, scrollmax, 
			 scrollline, TRUE, vscroll_f, &vScrollBar);
  m_scrollinit = true;
  m_scrollval = scrollval;
  m_scrollmin = scrollmin;
  m_scrollmax = scrollmax;
  m_scrollline = scrollline;
  m_scrollpage = scrollpage;
  OSXTerminal *ptr = this;
  SetControlProperty(vScrollBar, 0, 1001, sizeof(OSXTerminal*), &ptr);
  DrawControls(win);
}


void OSXTerminal::OnResize(int w, int h) {
  m_client_width = w;
  m_client_height = h;
  DisposeControl(vScrollBar);
  Erase();
  CreateVerticalScrollBar(m_scrollmin,m_scrollmax,m_scrollline,m_scrollpage,m_scrollval);
  TermWidget::OnResize();
  DrawControls(win);
  DrawContent();
}

WindowRef OSXTerminal::Win() {
  return win;
}

#define EVMATCH(a,b) ((yclass == a) && (kind == b))

pascal OSStatus OSXTerminalWndProc(EventHandlerCallRef handlerRef,
				   EventRef event,
				   void *userData) {
  int yclass, kind;
  OSXTerminal *winptr;
  short pt[2];
  UInt32 sz;
  
  yclass = GetEventClass(event);
  kind = GetEventKind(event);
  winptr = (OSXTerminal*) userData;
  
  if EVMATCH(kEventClassWindow,kEventWindowResizeCompleted) {
    Rect r;
    GetWindowBounds(winptr->Win(), kWindowContentRgn, &r);
    winptr->OnResize(r.right - r.left - 15, r.bottom - r.top);
  }
  if EVMATCH(kEventClassWindow,kEventWindowDrawContent)
    winptr->DrawContent();
  if EVMATCH(kEventClassWindow,kEventWindowZoomed) {
    Rect r;
    GetWindowBounds(winptr->Win(), kWindowContentRgn, &r);
    winptr->OnResize(r.right - r.left - 15, r.bottom - r.top);
  }
  if EVMATCH(kEventClassWindow,kEventWindowClosed)
    exit(0);
  if EVMATCH(kEventClassWindow,kEventWindowClickContentRgn) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    Rect r;
    GetWindowBounds(winptr->Win(), kWindowContentRgn, &r);
    winptr->OnMouseDown(pt[1] - r.left,pt[0] - r.top);
  }
  if EVMATCH(kEventClassMouse,kEventMouseDragged) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    Rect r;
    GetWindowBounds(winptr->Win(), kWindowContentRgn, &r);
    winptr->OnMouseDrag(pt[1] - r.left,pt[0] - r.top);
  }
  if EVMATCH(kEventClassMouse,kEventMouseUp) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    Rect r;
    GetWindowBounds(winptr->Win(), kWindowContentRgn, &r);
    winptr->OnMouseUp(pt[1] - r.left,pt[0] - r.top);
  }
  if (yclass == kEventClassKeyboard) {
    char keycode;
    GetEventParameter(event, kEventParamKeyMacCharCodes, typeChar, NULL,
		      sizeof(char), NULL, &keycode);
    winptr->OnKeyPress(keycode);
  }
  return eventNotHandledErr;
}

int main(int arc, char* argv[]) {
  OSXTerminal *term = new OSXTerminal(500, 300, "Interaction");
  ShowWindow(term->Win());
  RunApplicationEventLoop();
}
