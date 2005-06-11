#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>
#include <string>
#include <iostream>

#define QMIN(a,b) (((a) < (b)) ? (a) : (b))
#define QMAX(a,b) (((a) < (b)) ? (b) : (a))

#define REFRESHTIME 0.03
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

class XPWindow {
 public:
  WindowRef win;
  ControlHandle vScrollBar;
  int m_client_width, m_client_height;
  int m_scrollmin, m_scrollmax, m_scrollline, m_scrollpage, m_scrollval;
  bool m_scrollinit;
 public:
  XPWindow(int width, int height, std::string title);
  ~XPWindow();
  void CreateVerticalScrollBar(int scrollmin, int scrollmax, int scrollline, int scrollpage, int scrollval);
  virtual void OnDraw();
  virtual void OnResize();
  void Show();
  void Hide();
  void Erase();
  void Raise();
  void SetTitle(std::string title);
  virtual void OnMouseDown(int x, int y) {}
  virtual void OnMouseDrag(int x, int y) {}
  virtual void OnMouseUp(int x, int y) {}
  virtual void OnKeyPress(int key) {}
  virtual void OnScroll(int val) {};
  int GetHeight();
  int GetWidth();
  // Scroll commands
  void ScrollLineUp();
  void ScrollLineDown();
  void ScrollPageUp();
  void ScrollPageDown();
  void SetScrollValue(int val);
};

XPWindow::~XPWindow() {
  DisposeWindow(win);
}

int XPWindow::GetHeight() {
  return m_client_height;
}

int XPWindow::GetWidth() {
  return m_client_width;
}

static void refresh_cb(EventLoopTimerRef ref, void *data) {
  XPWindow *win = (XPWindow*) data;
  win->OnDraw();
  InstallEventLoopTimer(GetMainEventLoop(),REFRESHTIME,0,
			NewEventLoopTimerUPP(refresh_cb),
			win,NULL);
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
  OnScroll(val);
  OnDraw();
}

static void vscroll_f(ControlRef ref, ControlPartCode part) {
  XPWindow *winptr;
  UInt32 sze;
  GetControlProperty(ref, 0, 1001, sizeof(XPWindow*), &sze, &winptr);
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
  XPWindow *ptr = this;
  SetControlProperty(vScrollBar, 0, 1001, sizeof(XPWindow*), &ptr);
  DrawControls(win);
}

void XPWindow::Raise() {
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
  r.right = m_client_width;
  r.bottom = m_client_height;
  ClipRect(&r);
  EraseRect(&r);
  SetClip(rgn);
  DisposeRgn(rgn);
}

void XPWindow::OnDraw() {
  DrawControls(win);
}

void XPWindow::OnResize() {
  Rect r;
  GetWindowBounds(win, kWindowContentRgn, &r);
  m_client_width = r.right - r.left - 15;
  m_client_height = r.bottom - r.top;
  DisposeControl(vScrollBar);
  Erase();
  CreateVerticalScrollBar(m_scrollmin,m_scrollmax,m_scrollline,m_scrollpage,m_scrollval);
  DrawControls(win);
  OnDraw();
}

XPWindow::XPWindow(int width, int height, std::string title) {
  Rect winSize;
  m_client_width = width - 15;
  m_client_height = height;
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
  m_scrollinit = false;
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
    Rect r;
    GetWindowBounds(winptr->win, kWindowContentRgn, &r);
    winptr->OnMouseDown(pt[1] - r.left,pt[0] - r.top);
  }
  if EVMATCH(kEventClassMouse,kEventMouseDragged) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    Rect r;
    GetWindowBounds(winptr->win, kWindowContentRgn, &r);
    winptr->OnMouseDrag(pt[1] - r.left,pt[0] - r.top);
  }
  if EVMATCH(kEventClassMouse,kEventMouseUp) {
    GetEventParameter(event, kEventParamMouseLocation, typeQDPoint, 
		      NULL, sizeof(pt), &sz, pt);
    Rect r;
    GetWindowBounds(winptr->win, kWindowContentRgn, &r);
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

#define CURSORBIT 1
#define SELECTBIT 2

class tagChar
{
 public:
  char v;
  char flags;
  tagChar(): v(' '), flags(0) {};
  tagChar(char t): v(t), flags(0) {};
  bool noflags() {return(flags == 0);};
  bool cursor() {return(flags & CURSORBIT);};
  void toggleCursor() {flags ^= CURSORBIT;};
  void setCursor() {flags |= CURSORBIT;};
  void clearCursor() {flags &= ~CURSORBIT;};
  void clearSelection() {flags &= ~SELECTBIT;};
  void setSelection() {flags |= SELECTBIT;};
  bool operator == (const tagChar& b) {return (v==b.v) && (flags==b.flags);};
};

class TermWidget : public XPWindow
{
protected:
  tagChar* m_surface;  // width x height
  int m_height;        // height of terminal in characters
  int m_width;         // width of terminal in characters
  int m_cursor_x;      // column position of the cursor
  int m_cursor_y;      // row position of the cursor
  tagChar* m_onscreen; // width x height - contains contents of the screen
  tagChar* m_history;  // width x scrollheight - contains history of terminal
  int m_char_w;        // width of a character
  int m_char_h;        // height of a character
  int m_char_decent;   // descent of a character
  int m_active_width;  // width of the active text region.
  bool m_blink_skip;
  bool m_clearall;
//   QScrollBar *m_scrollbar;
//   QTimer *m_timer_refresh;
//   QTimer *m_timer_blink;
  bool cursorOn;
  bool blinkEnable;
  int m_scrollback;
  int m_history_lines;
  bool m_scrolling;
  bool m_mousePressed;
  int selectionStart;
  int selectionStop;
  int m_scroll_offset;
 public:
  void scrollBack(int val);
  TermWidget(int width, int height, char *title);
  virtual ~TermWidget();
  void resizeTextSurface();
  //  virtual void ProcessChar(int c) = 0;
  void PutString(std::string txt);
  void refresh();
  void blink();
  void setCursor(int x, int y);
  int  getTextWidth() {return m_width;};
  //TK dependant functions
 protected:
  void setScrollbar(int val);
  void OnResize();
  void OnKeyPress(int key);
  void OnDraw();
  void setFont(int size);
  void OnMouseDown(int x, int y);
  void OnMouseDrag(int x, int y);
  void OnMouseUp(int x, int y);
  void OnScroll(int val);
};

static void blink_cb(EventLoopTimerRef ref, void *data) {
  TermWidget *win = (TermWidget*) data;
  win->blink();
  InstallEventLoopTimer(GetMainEventLoop(),BLINKTIME,0,
			NewEventLoopTimerUPP(blink_cb),
			win,NULL);
}


TermWidget::TermWidget(int width, int height, char *title) 
  : XPWindow(width, height, title) {
    m_surface = NULL;
    m_onscreen = NULL;
    m_history = NULL;
    m_scrollback = 1000;
    m_history_lines = 0;
    m_cursor_x = 0;
    m_cursor_y = 0;
    m_clearall = true;
    InstallEventLoopTimer(GetMainEventLoop(),REFRESHTIME,0,
			  NewEventLoopTimerUPP(refresh_cb),
			  this,NULL);
    InstallEventLoopTimer(GetMainEventLoop(),BLINKTIME,0,
			  NewEventLoopTimerUPP(blink_cb),
			  this,NULL);
    setFont(12);
    cursorOn = false;
    m_blink_skip = true;
    blinkEnable = true;
    m_scrolling = false;
    m_scroll_offset = 0;
    m_mousePressed = false;
    m_active_width = GetWidth();
    resizeTextSurface();
}

TermWidget::~TermWidget() {
}

void TermWidget::scrollBack(int val) {
  if (m_history_lines == 0) return;
  m_scrolling =  (val != m_history_lines);
  m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();
  m_surface = m_history + (m_scrollback - m_height - (m_history_lines - val))*m_width;
  m_scroll_offset = (m_scrollback - m_height - (m_history_lines - val))*m_width;
}

void TermWidget::blink() {
  if (!blinkEnable) return;
  if (m_blink_skip) {
    m_blink_skip = false;
    return;
  }
  m_surface[m_cursor_x + m_cursor_y*m_width].toggleCursor();
}

void TermWidget::refresh() {
  OnDraw();
}

void TermWidget::resizeTextSurface() {
  bool firsttime = (m_surface == NULL);
  if (m_surface)
    m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();  
  int cursor_offset = m_height - 1 - m_cursor_y;
  //  m_timer_refresh->start(30,false);
  //  m_timer_blink->start(1000);
  m_blink_skip = true;
  int new_width = m_active_width/m_char_w;
  int new_height = GetHeight()/m_char_h;
  tagChar *new_history = new tagChar[new_width*m_scrollback];
  
  if (m_history) {
    int minwidth = QMIN(new_width,m_width);
    for (int i=0;i<m_scrollback;i++)
      for (int j=0;j<minwidth;j++)
	new_history[i*new_width+j] = m_history[i*m_width+j];
  }
  // Copy old history to new history here
  
  delete[] m_onscreen;
  delete[] m_history;
  m_onscreen = new tagChar[new_width*new_height];
  m_history = new_history;
  m_surface = m_history + (m_scrollback - new_height)*new_width;
  
  if (!firsttime) {
    m_history_lines -= (new_height-m_height);
    m_history_lines = QMAX(0,m_history_lines);
  }

  m_width = new_width;
  m_height = new_height;
  m_clearall = true;
  // only do this the first time
  if (firsttime) {
    setCursor(0,0);
    setScrollbar(0);
  } else {
    m_cursor_y = m_height - 1 - cursor_offset;
    setScrollbar(m_history_lines);
  }
  m_scroll_offset = (m_scrollback - m_height)*m_width;
}

void TermWidget::setScrollbar(int val) {
  if ((m_scrollmin != 0) ||
      (m_scrollmax != m_history_lines) ||
      (m_scrollline != 1) ||
      (m_scrollpage != m_height))
    CreateVerticalScrollBar(0,m_history_lines,1,m_height,val);
  else
    SetScrollValue(val);
}

void TermWidget::PutString(std::string txt) {
  if (m_scrolling) {
    setScrollbar(m_history_lines);
  }
  for (int i=0;i<txt.size();i++) {
    if (txt[i] == '\n')
      setCursor(m_cursor_x,m_cursor_y+1);
    else if (txt[i] == '\r')
      setCursor(0,m_cursor_y);
    else {
      m_surface[m_cursor_x + m_cursor_y*m_width] = tagChar(txt[i]);
      setCursor(m_cursor_x+1,m_cursor_y);
    }
  }
  //  m_timer_blink->start(1000);
  m_blink_skip = true;
}

void TermWidget::OnResize() {
  XPWindow::OnResize();
  m_active_width = GetWidth();
  resizeTextSurface();
}


// Update the onscreen buffer to match the text buffer surface.
void TermWidget::OnDraw() {
  XPWindow::OnDraw();
  if (m_width == 0) return;
  CGContextRef gh;
  CreateCGContextForPort(GetWindowPort(win),&gh);
  CGContextSelectFont(gh,"Monaco",12,kCGEncodingMacRoman);
  for (int i=0;i<m_height;i++) {
    int j=0;
    while (j<m_width) {
      // skip over characters that do not need to be redrawn
      while ((j < m_width) && 
	     (m_onscreen[i*m_width+j] == m_surface[i*m_width+j])) j++;
      if (j < m_width) {
	CGContextSetTextPosition(gh,j*m_char_w,(m_height-1-i)*m_char_h + 2);
	CGRect fill = CGRectMake(j*m_char_w,(m_height-1-i)*m_char_h,
				 m_char_w,m_char_h);
	tagChar g = m_surface[i*m_width+j];
	if (m_scrolling) 
	  g.flags &= ~CURSORBIT;
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
	m_onscreen[i*m_width+j] = g;
	j++;
      }
    }
  }
  CGContextFlush(gh);
  CGContextRelease(gh);
}

void TermWidget::setCursor(int x, int y) {
//   cursorOn = false;
//   m_onscreen[m_cursor_x + m_cursor_y*m_width] = -1;
//   repaint(cursorRect,true);
//   cursorEnable = false;
//  if (m_scrolling) 
//    setScrollbar(0);
  if (m_surface[m_cursor_y*m_width+m_cursor_x].cursor())
    m_surface[m_cursor_y*m_width+m_cursor_x].toggleCursor();
  m_cursor_x = x;
  m_cursor_y = y;
  m_cursor_y += m_cursor_x/m_width;
  m_cursor_x %= m_width;
  if (m_cursor_y >= m_height) {
    // scroll up - which we do by a single memmove op
    int toscroll = m_cursor_y - m_height + 1;
    for (int i=0;i<(m_scrollback - toscroll)*m_width;i++)
      m_history[i] = m_history[i+toscroll*m_width];
    for (int i=0;i<toscroll*m_width;i++)
      m_history[(m_scrollback - toscroll)*m_width+i] = tagChar();
    m_history_lines = QMIN(m_history_lines+toscroll,m_scrollback-m_height);
    m_cursor_y -= toscroll;
    setScrollbar(m_history_lines);
  }
  m_surface[m_cursor_y*m_width+m_cursor_x].setCursor();
}

void TermWidget::OnScroll(int val) {
  scrollBack(val);
}

// Want scrollbar to be such that the cursor is visible
void TermWidget::OnKeyPress(int c) {
  if (m_scrolling) 
    setScrollbar(m_history_lines);
  if (c == 'q')
    exit(0);
  if (c == 'l') {
    setCursor(QMAX(m_cursor_x-1,0),m_cursor_y);
    return;
  }
  if (c != 'd' && c != 'x') {
    char buffer[2];
    buffer[0] = c;
    buffer[1] = 0;
    PutString(buffer);
  } else if (c == 'd')
    PutString("Now is the time for all men to come to the aid of their country, and by their aid, assist those who need it, or something like that....");
  else if (c == 'x') {
    for (int i=0;i<100;i++) {
      char buffer[1000];
      sprintf(buffer,"line %d\r\n",i);
      PutString(buffer);
    }
  }
#if 0
  if (m_scrolling) 
    setScrollbar(0);
  int keycode = e->key();
  if (!keycode) return;
  if (keycode == Qt::Key_Left)
    ProcessChar(KM_LEFT);
  else if (keycode == Qt::Key_Right)
    ProcessChar(KM_RIGHT);
  else if (keycode == Qt::Key_Up)
    ProcessChar(KM_UP);
  else if (keycode == Qt::Key_Down)
    ProcessChar(KM_DOWN);
  else if (keycode == Qt::Key_Delete)
    ProcessChar(KM_DELETE);
  else if (keycode == Qt::Key_Insert)
    ProcessChar(KM_INSERT);
  else if (keycode == Qt::Key_Home)
    ProcessChar(KM_HOME);
  else if (keycode == Qt::Key_End)
    ProcessChar(KM_END);
  else {
    char key = e->ascii();
    if (key) ProcessChar(key);
  }
#endif
}

void TermWidget::setFont(int size) {
  //  FPShowHideFontPanel();
  //   // Get a platform font reference
  //   CGFontRef myFont;
  //   myFont = CGFontCreateWithPlatformFont(&platformFontReference);
  CGContextRef gh;
  char tst = 'w';
  CreateCGContextForPort(GetWindowPort(win),&gh);
  CGContextSelectFont(gh,"Monaco",size,kCGEncodingMacRoman);
  CGContextSetTextPosition(gh,0,0);
  CGContextSetRGBFillColor(gh,0,0,0,0);
  CGContextShowText(gh,&tst,1);
  CGPoint pt(CGContextGetTextPosition(gh));
  CGContextRelease(gh);
  m_char_w = pt.x;
  m_char_h = size;
}


void TermWidget::OnMouseDown(int x, int y) {
  std::cout << "mouse down at " << x << "," << y << "\n";
  // Get the x and y coordinates of the mouse click - map that
  // to a row and column
  int clickcol = x/m_char_w;
  int clickrow = y/m_char_h;
  selectionStart = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStart = QMIN(QMAX(0,selectionStart),m_width*m_scrollback-1);
  selectionStop = selectionStart;
}

void TermWidget::OnMouseDrag(int x, int y) {
  if (y < 0) 
    ScrollLineUp();
  if (y > GetHeight())
    ScrollLineDown();
  // Get the position of the click
    // to a row and column
    int clickcol = x/m_char_w;
    int clickrow = y/m_char_h;
    selectionStop = m_scroll_offset + clickcol + clickrow*m_width;
    selectionStop = QMIN(QMAX(0,selectionStop),m_width*m_scrollback-1);
    // clear the selection bits
    for (int i=0;i<m_width*m_scrollback;i++)
      m_history[i].clearSelection();
    // set the selection bits
    if (selectionStart < selectionStop)
      for (int i=selectionStart;i<selectionStop;i++)
	m_history[i].setSelection();
    else
      for (int i=selectionStop;i<selectionStart;i++)
	m_history[i].setSelection();
}

void TermWidget::OnMouseUp( int, int ) {
}


int main(int arc, char* argv[]) {
  XPWindow *win = new XPWindow(500,300,"main window");
  win->SetTitle("Main Window");
  win->CreateVerticalScrollBar(0,100,1,10,0);
  win->Raise();
  win->Show();
  TermWidget *term = new TermWidget(500, 300, "Interaction");
  term->Show();
  RunApplicationEventLoop();
}
