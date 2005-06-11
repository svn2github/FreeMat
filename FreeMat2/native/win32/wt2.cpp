#include "wt2.hpp"
#include <string.h>
#include <algorithm>
#include <iostream>

#include <direct.h>
#define getcwd _getcwd
#define S_ISREG(x) (x & _S_IFREG)

void WinTerminal::OnExpose(int x, int y, int w, int h) {
  if (!bitmap_active) return;
  HDC hdc = GetDC(hwnd);
  BitBlt(hdc, x, y, w, h, hdcMem, x, y, SRCCOPY);
  ReleaseDC(hwnd, hdc);
}

void WinTerminal::OnResize(int w, int h) {
  if (w == 0 || h == 0) return;
  m_win_width = w;
  m_win_height = h;
  HDC hdc = GetDC(hwnd);
  DeleteDC(hdcMem);
  hdcMem = CreateCompatibleDC(hdc);
  if (bitmap_active)
    DeleteObject(hBitmp);
  hBitmp = CreateCompatibleBitmap(hdc, m_win_width, m_win_height);
  RECT rct;
  rct.top = 0;
  rct.bottom = m_win_height;
  rct.left = 0;
  rct.right = m_win_width;
  SelectObject(hdcMem, hBitmp);
  FillRect(hdcMem, &rct, (HBRUSH) GetStockObject(WHITE_BRUSH));
  TermWidget::OnResize();
  InvalidateRect(hwnd,NULL,TRUE);
  UpdateWindow(hwnd);
  bitmap_active = true;
  ReleaseDC(hwnd, hdc);
}

void WinTerminal::OnScrollMsg(int scrollType) {
  std::cout << "OnScroll Message received " << scrollType << "\n";
  int iVertpos;
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
  GetScrollInfo(hwnd, SB_VERT, &si);
  iVertpos = si.nPos;
  switch(scrollType) {
  case SB_TOP:
    si.nPos = si.nMin;
    break;
  case SB_BOTTOM:
    si.nPos = si.nMax;
    break;
  case SB_LINEUP:
    si.nPos--;
    break;
  case SB_LINEDOWN:
    si.nPos++;
    break;
  case SB_PAGEUP:
    si.nPos -= si.nPage;
    break;
  case SB_PAGEDOWN:
    si.nPos += si.nPage;
    break;
  case SB_THUMBTRACK:
    si.nPos = si.nTrackPos;
    break;
  default:
    break;
  }
  si.fMask = SIF_POS;
  SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
  GetScrollInfo(hwnd, SB_VERT, &si);
  TermWidget::OnScroll(si.nPos);
}

int WinTerminal::GetHeight() {
  return m_win_height;
}

int WinTerminal::GetWidth() {
  return m_win_width;
}

void WinTerminal::InstallEventTimers() {
  SetTimer(hwnd, 1, 100, NULL);
  SetTimer(hwnd, 2, 1000, NULL);
}

void WinTerminal::ScrollLineUp() {
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
  GetScrollInfo(hwnd, SB_VERT, &si);
  int newval = si.nPos - 1;
  newval = (newval < si.nMin) ? si.nMin : newval;
  SetScrollBarValue(newval);
}

void WinTerminal::ScrollLineDown() {
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
  GetScrollInfo(hwnd, SB_VERT, &si);
  int newval = si.nPos + 1;
  newval = (newval > (si.nMax - si.nPage + 1)) ? (si.nMax-si.nPage+1):newval;
  SetScrollBarValue(newval);
}

void WinTerminal::SetScrollBarValue(int val) {
  si.cbSize = sizeof(si);
  si.fMask = SIF_POS;
  si.nPos = val;
  int p;
  p = SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
  char buffer[1024];
  sprintf(buffer,"scroll at  %d %d\n",p,val);
  //  OutputDebugString(buffer);
  std::cout << buffer;
  OnScroll(val);
}

// We want the scrollbar to take values between minval
// and maxval (inclusive).  The win32 scrollbar takes 
// values between nMin and nMax-nPage+1, so nMax = maxVal+nPage-1
void WinTerminal::SetupScrollBar(int minval, int maxval,
				 int step, int page, int val) {
  std::cout << "scroll inputs " << minval << " " << maxval << " " << step << " " << page << " " << val << "\n";
  si.cbSize = sizeof(si);
  si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
  si.nMin = minval;
  si.nMax = maxval+page-1;
  si.nPage = page;
  si.nPos = val;
  SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
  char buffer[1024];
  sprintf(buffer,"scroll setup %d %d %d %d %d\n",si.nMin,si.nMax,si.nPage,step,val);
  OutputDebugString(buffer);
}

void WinTerminal::PutTagChar(int x, int y, tagChar g) {
  RECT rct;
  rct.left = x;
  rct.right = x + m_char_w;
  rct.top = y;
  rct.bottom = y + m_char_h;
  SelectObject(hdcMem, hfnt);
  if (g.noflags()) {
    SetTextColor(hdcMem, RGB(0, 0, 0));
    SetBkColor(hdcMem, RGB(255,255,255));
    TextOut(hdcMem, x, y, &g.v, 1);
  } else if (g.cursor()) {
    SetBkColor(hdcMem, RGB(0,0,0));
    SetTextColor(hdcMem, RGB(255, 255, 255));
    TextOut(hdcMem, x, y, &g.v, 1);
  } else {
    SetBkColor(hdcMem, RGB(0,0,255));
    SetTextColor(hdcMem, RGB(255, 255, 255));
    TextOut(hdcMem, x, y, &g.v, 1);
  }
  InvalidateRect(hwnd,&rct,FALSE);
}

void WinTerminal::setFont(int size) {
  hfnt = (HFONT) GetStockObject(ANSI_FIXED_FONT);
  TEXTMETRIC tm;
  HDC hdc;
  hdc = GetDC(hwnd);
  SelectObject(hdc,hfnt);
  GetTextMetrics(hdc,&tm);
  ReleaseDC(hwnd,hdc);
  m_char_w = tm.tmAveCharWidth;
  m_char_h = tm.tmHeight;
}


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void SetupWinTerminalClass(HINSTANCE hInstance) {
  WNDCLASS wndclass;
  wndclass.style = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = WndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 4;
  wndclass.hInstance = hInstance;
  //wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  //wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
  wndclass.lpszMenuName = NULL;
  wndclass.hIcon = 0;
  wndclass.lpszClassName = "WinTerminal";
  if (!RegisterClass(&wndclass))
    {
      MessageBox(NULL, TEXT("This program requires Windows NT!"),
		 "WinTerm",MB_ICONERROR);
      return;
    }
}


WinTerminal::WinTerminal(HINSTANCE hInstance, int iCmdShow) {
  hwnd = CreateWindow("WinTerminal",
		      TEXT("FreeMat"),
		      WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      500,
		      400,
		      NULL,
		      NULL,
		      hInstance,
		      NULL);
  m_win_width = 500;
  m_win_height = 400;
  SetWindowLong(hwnd,GWL_USERDATA,(LONG) this);
  bitmap_active = false;
  TermWidget::Initialize();
  ShowWindow(hwnd, iCmdShow);
  InstallEventTimers();
//   hWhiteBrush = (HBRUSH) GetStockObject(WHITE_BRUSH);
//   hBlackBrush = (HBRUSH) GetStockObject(BLACK_BRUSH);
//   hBlueBrush =  (HBRUSH) CreateSolidBrush(RGB(0,0,255));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  WinTerminal* wptr;
  HDC hdc;
  PAINTSTRUCT ps;
  RECT rect;
  
  wptr = (WinTerminal*) GetWindowLong(hwnd, GWL_USERDATA);
  
  switch (message)
    {
    case WM_SIZE:
      wptr->OnResize(LOWORD(lParam), HIWORD(lParam));
      return 0;
    case WM_CREATE:
      return 0;
    case WM_LBUTTONDOWN:
      wptr->OnMouseDown((short)LOWORD(lParam),(short)HIWORD(lParam));
	  SetCapture(hwnd);
      return 0;
    case WM_MOUSEMOVE:
      if (wParam & MK_LBUTTON)
	    wptr->OnMouseDrag((short)LOWORD(lParam),(short)HIWORD(lParam));
      return 0;
    case WM_LBUTTONUP:
	  wptr->OnMouseUp((short)LOWORD(lParam),(short)HIWORD(lParam));
	  ReleaseCapture();
      return 0;
    case WM_PAINT:
      HDC hdc;
      PAINTSTRUCT ps;
      hdc = BeginPaint(wptr->window(),&ps);
      wptr->OnExpose(ps.rcPaint.left,ps.rcPaint.top,
		     ps.rcPaint.right-ps.rcPaint.left+1,
		     ps.rcPaint.bottom-ps.rcPaint.top+1);
      EndPaint(wptr->window(), &ps);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_SETFOCUS:  
      break;
    case WM_KILLFOCUS:  
      break; 
    case WM_CHAR: {
      wptr->OnKeyPress(wParam);
      break;
    }
    case WM_VSCROLL: {
      wptr->OnScrollMsg(LOWORD(wParam));
      break;
    }    
    case WM_TIMER: {
      if (LOWORD(wParam) == 1)
	wptr->DrawContent();
      else
	wptr->blink();
      return 0;
      break;
    }
    }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine, int nCmdShow) {
  MSG        Msg;
  SetupWinTerminalClass(hInstance);
  WinTerminal term(hInstance, nCmdShow);
  
  while( GetMessage(&Msg, NULL, 0, 0) )
    {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
  return Msg.wParam;
}


#if 0
  void WinTerminal::MoveDown() {
    caret_y++;
    DoMoveCaret();
  }

  void WinTerminal::MoveUp() {
    caret_y--;
    if (caret_y < 0) caret_y  = 0;
    DoMoveCaret();
  }

  void WinTerminal::MoveRight(){
    caret_x++;
    if (caret_x >= ncolumn) caret_x = ncolumn-1;
    DoMoveCaret();
  }

  void WinTerminal::MoveLeft() {
    caret_x--;
    if (caret_x < 0) caret_x = 0;
    DoMoveCaret();
  }

  void WinTerminal::ClearEOL() {
    int i;
    for (i=caret_x;i<MAXCOLS;i++)
      CharAt(caret_y,i) = ' ';
    CharAt(caret_y,caret_x) = 0;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    RECT refresh;
    refresh.left = caret_x * charWidth;
    refresh.right = ncolumn * charWidth;
    refresh.top = (caret_y-si.nPos)*charHeight;
    refresh.bottom = (caret_y+1-si.nPos)*charHeight;
    InvalidateRect(hwnd,&refresh,TRUE);
    UpdateWindow(hwnd);
  }

  void WinTerminal::ClearEOD() {
    int i,  j;
    ClearEOL();
    for (i=caret_y+1;i<nlinecount;i++) {
      for (j=0;j<MAXCOLS;j++)
        CharAt(i,j) = 0;
    }
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    RECT refresh;
    refresh.left = 0;
    refresh.right = ncolumn * charWidth;
    refresh.top = (caret_y+1-si.nPos)*charHeight;
    refresh.bottom = (nlinecount-si.nPos)*charHeight;
    InvalidateRect(hwnd,NULL,TRUE);
    UpdateWindow(hwnd);
  }

  void WinTerminal::MoveBOL() {
    caret_x = 0;
    DoMoveCaret();
  }

  void WinTerminal::OutputRawString(std::string txt) {
    HideCaret(hwnd);
    int startRow, startCol;
        int i;
        bool redrawall;
    const char *cp;
    cp = txt.c_str();
    startRow = caret_y;
        redrawall=false;
    while (*cp) {
      if (*cp == '\n') {
        caret_y++;
        caret_x = 0;
    for (i=0;i<MAXCOLS;i++)
      CharAt(caret_y,i) = 0;
          cp++;
          redrawall = true;
      } else if (*cp == '\r') {
        caret_x = 0;
        cp++;
      } else if (caret_x == ncolumn) {
        caret_y++;
        caret_x = 0;
      } else {
        CharAt(caret_y,caret_x) = *cp;
        caret_x++;
        cp++;
      }
    }
    UpdateLineCount();
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    RECT refresh;
    refresh.left = 0;
    refresh.right = ncolumn*charWidth;
    refresh.top = (startRow-si.nPos)*charHeight;
    refresh.bottom = (caret_y+1-si.nPos)*charHeight;
        if (!redrawall)
      InvalidateRect(hwnd,&refresh,TRUE);
        else
      InvalidateRect(hwnd,NULL,TRUE);
    UpdateWindow(hwnd);
    DoMoveCaret();
        ShowCaret(hwnd);
  }

  void WinTerminal::OnScroll(int scrollType) {
    char buffer[1024];
    sprintf(buffer,"OnScroll %d\n",scrollType);
    OutputDebugString(buffer);
    int iVertpos;
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    GetScrollInfo(hwnd, SB_VERT, &si);
    iVertpos = si.nPos;
    switch(scrollType) {
    case SB_TOP:
      si.nPos = si.nMin;
      break;
    case SB_BOTTOM:
      si.nPos = si.nMax;
      break;
    case SB_LINEUP:
      si.nPos--;
      break;
    case SB_LINEDOWN:
      si.nPos++;
      break;
    case SB_PAGEUP:
      si.nPos -= si.nPage;
      break;
    case SB_PAGEDOWN:
      si.nPos += si.nPage;
      break;
    case SB_THUMBTRACK:
      si.nPos = si.nTrackPos;
      break;
    default:
      break;
    }
    si.fMask = SIF_POS;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
    GetScrollInfo(hwnd, SB_VERT, &si);
    if (si.nPos != iVertpos) {
      ScrollWindow(hwnd, 0, charHeight*(iVertpos - si.nPos), NULL, NULL);
      UpdateWindow(hwnd);
    }
  }
  
  void WinTerminal::OnDraw(HDC hdc, PAINTSTRUCT ps) {
        if (nlinecount == 0) return;
    HideCaret(hwnd);
    SelectObject(hdc,hfnt);
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    int iVertpos = si.nPos;
    int paintBeg = max(0, iVertpos + ps.rcPaint.top/charHeight);
    int paintEnd = min(nlinecount-1,
                       iVertpos + ps.rcPaint.bottom/charHeight);
    bool selectActive;
    int sel_y1, sel_y2, sel_x1, sel_x2;
    sel_y1 = selstart_row;
    sel_y2 = selstop_row;
    sel_x1 = selstart_col;
    sel_x2 = selstop_col;
    if (sel_y1 > sel_y2) {
      sel_y2 = selstart_row;
      sel_y1 = selstop_row;
      sel_x2 = selstart_col;
      sel_x1 = selstop_col;
    }
    for (int y=paintBeg; y <= paintEnd; y++) {
      selectActive = ((y >= sel_y1) && (y <= sel_y2));
      char buffer[1000];
      int x;
      for (x=0;x<ncolumn;x++) {
        char ch = CharAt(y,x);
        buffer[x] = ch;
        if (!ch) break;
      }
      buffer[x] = 0;
      if (strlen(buffer) != 0) {
                  int textlen;
                  textlen = strlen(buffer);
        if (selectActive) {
          // Special case if start and stop row are the same
          if ((sel_y1 == sel_y2) && (y == sel_y1)) {
            if (sel_x1 >= sel_x2) {
              int t = sel_x1;
              sel_x1 = sel_x2;
              sel_x2 = t;
            }
            // We output three parts of text - start in black
            TextOut(hdc,0,(y-iVertpos)*charHeight,buffer,
                    min(sel_x1,textlen));
            // The second part is in red (requires textlen > sel_x1)
            if (textlen > sel_x1) {
              SetTextColor(hdc, RGB(255, 0, 0));
              TextOut(hdc,sel_x1*charWidth,
                      (y-iVertpos)*charHeight,buffer+sel_x1,
                      min(sel_x2,textlen)-sel_x1);
              SetTextColor(hdc, RGB(0, 0, 0));
            }
            // The third part is in black (requires textlen > sel_x2)
            if (textlen > sel_x2) 
              TextOut(hdc,sel_x2*charWidth,(y-iVertpos)*charHeight,
                        buffer+sel_x2,
                      textlen - sel_x2);
          } else if (y == sel_y1) {
            TextOut(hdc,0,(y-iVertpos)*charHeight,buffer,
                    min(sel_x1,textlen));
            // The second part is in red (requires textlen > sel_x1)
            if (textlen > sel_x1) {
              SetTextColor(hdc, RGB(255, 0, 0));
              TextOut(hdc,sel_x1*charWidth,
                      (y-iVertpos)*charHeight,buffer+sel_x1,
                      textlen-sel_x1);
              SetTextColor(hdc, RGB(0, 0, 0));
            }
          } else if (y == sel_y2) {
                // Output enough text in red to cover sel_x2
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOut(hdc,0,(y-iVertpos)*charHeight,buffer,min(sel_x2,textlen));
            SetTextColor(hdc, RGB(0, 0, 0));
                if (textlen > sel_x2)
              TextOut(hdc,sel_x2*charWidth,
                    (y-iVertpos)*charHeight,buffer+sel_x2,
                    textlen-sel_x2);
          } else {
            SetTextColor(hdc, RGB(255, 0, 0));
            TextOut(hdc,0,(y-iVertpos)*charHeight,buffer,
                    strlen(buffer));
            SetTextColor(hdc, RGB(0, 0, 0));
          }
        } else {
          SetTextColor(hdc, RGB(0, 0, 0));
          TextOut(hdc,0,(y-iVertpos)*charHeight,buffer,strlen(buffer));
        }
      }
    }
    ShowCaret(hwnd);
  }

  void WinTerminal::Resize(int xdim, int ydim) {
    UpdateLineCount();
    DoResizeBuffer(xdim/charWidth,ydim/charHeight);
  }

  void WinTerminal::setContext(Context *contxt) {
    context = contxt;
  }

  void WinTerminal::setPath(std::string path) {
    char* pathdata = strdup(path.c_str());
    // Search through the path
    char* token;
    token = strtok(pathdata,";");
    while (token != NULL) {
      if (strcmp(token,".") != 0)
            dirTab.push_back(std::string(token));
      token = strtok(NULL,";");
    }
    m_path = path;
    rescanPath();
  }

  std::string WinTerminal::getPath() {
    return m_path;
  }

  void WinTerminal::rescanPath() {
    int i;
    for (i=0;i<dirTab.size();i++)
      scanDirectory(dirTab[i]);
    // Scan the current working directory.
    char cwd[1024];
    getcwd(cwd,1024);
    scanDirectory(std::string(cwd));
  }

  std::string getFileNameOnly(char* fullname) {
          char *fcopy, *cp;
          std::string retval;
          fcopy = strdup(fullname);
          cp = fcopy + strlen(fcopy);
          while (*cp != '\\')
                  cp--;
          retval = std::string(cp+1);
          free(fcopy);
          return retval;
  }

  void WinTerminal::scanDirectory(std::string scdir) {
     HANDLE hSearch;
         WIN32_FIND_DATA FileData;
         std::string searchpat(scdir + "\\*.m");
         hSearch = FindFirstFile(searchpat.c_str(), &FileData);
         if (hSearch != INVALID_HANDLE_VALUE) {
                 procFile(std::string(FileData.cFileName),
                        scdir + "\\" + std::string(FileData.cFileName));
                while (FindNextFile(hSearch, &FileData)) {
                        procFile(std::string(FileData.cFileName),
                                scdir + "\\" + std::string(FileData.cFileName));
                }
                FindClose(hSearch);
         }
  }

  void WinTerminal::procFile(std::string sfname, 
                             std::string fullname) {
    struct stat filestat;
    char buffer[1024];
        char *fname;
  
        fname = strdup(sfname.c_str());
    stat(fullname.c_str(),&filestat);
    if (S_ISREG(filestat.st_mode)) {
      int namelen;
      namelen = strlen(fname);
      if (fname[namelen-2] == '.' && 
          (fname[namelen-1] == 'm' ||
           fname[namelen-1] == 'M')) {
        fname[namelen-2] = 0;
        // Look for the function in the context - only insert it
        // if it is not already defined.
        FunctionDef *fdef;
        if (!context->lookupFunctionGlobally(std::string(fname),fdef)) {
          MFunctionDef *adef;
          adef = new MFunctionDef();
          adef->name = std::string(fname);
          adef->fileName = fullname;
          context->insertFunctionGlobally(adef);
        }
      }else if (fname[namelen-2] == '.' && 
                (fname[namelen-1] == 'p' ||
                 fname[namelen-1] == 'P')) {
        fname[namelen-2] = 0;
        // Look for the function in the context - only insert it
        // if it is not already defined.
        FunctionDef *fdef;
        if (!context->lookupFunctionGlobally(std::string(fname),fdef)) {
          MFunctionDef *adef;
          // Open the file
          File *f = new File(fullname.c_str(),"rb");
          Serialize *s = new Serialize(f);
          s->handshakeClient();
          s->checkSignature('p',1);
          adef = ThawMFunction(s);
          adef->pcodeFunction = true;
          context->insertFunctionGlobally(adef);
        }
      }
    }
    free(fname);
  }

  int WinTerminal::getTerminalWidth() {
    return ncolumn;
  }
  
    std::string TranslateString(std::string x) {
    std::string y(x);
    int n;
    n = 0;
    while (n<y.size()) {
      if (y[n] == '\n') 
        y.insert(n++,"\r");
      n++;
    }
    return y;
  }
  
  void WinTerminal::outputMessage(const char* msg) {
    //     FILE *fp;
    //     fp = fopen("log.txt","a");
    //     fwrite(msg,strlen(msg),1,fp);
    //     fclose(fp);
    std::string msg2(TranslateString(msg));
    OutputRawString(msg2);
  }

  void WinTerminal::errorMessage(const char* msg) {
    std::string msg2(TranslateString(msg));
    OutputRawString("Error: " + msg2 + "\r\n");
    OutputRawString("   at " + TranslateString(messageContext) + "\r\n");
  }

  void WinTerminal::warningMessage(const char* msg) {
    std::string msg2(TranslateString(msg));
    OutputRawString("Warning: " + msg2 + "\r\n");
    OutputRawString("   at " + TranslateString(messageContext) + "\r\n");
  }

  void WinTerminal::SetEvalEngine(WalkTree* a_eval) {
    eval = a_eval;
  }

  void WinTerminal::ExecuteLine(const char * line) {
    enteredLines.push_back(line);
//      OutputRawString("\r\n");
    ReplacePrompt("");
  }
  
  void WinTerminal::RegisterInterrupt() {
        sigInterrupt(0);
  }
  
  void WinTerminal::OnMouseDown(int x, int y) {
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    selstart_row = si.nPos + y/charHeight;
    selstart_col = x/charWidth;
  }

  void WinTerminal::OnDrag(int x, int y) {
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS | SIF_PAGE;
    GetScrollInfo(hwnd, SB_VERT, &si);
    selstop_row = si.nPos + y/charHeight;
    selstop_col = x/charWidth;
    InvalidateRect(hwnd,NULL,FALSE);
    if (selstop_row >= si.nPos + si.nPage - 1) {
      si.nPos ++;
      SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
      InvalidateRect(hwnd,NULL,TRUE);
    } else if (selstop_row <= si.nPos + 1) {
      si.nPos --;
      SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
      InvalidateRect(hwnd,NULL,TRUE);
    } else
      InvalidateRect(hwnd,NULL,FALSE);
  }
  
  char* WinTerminal::getLine(const char* prompt) {
    fflush(stdout);
    ReplacePrompt(prompt);
    DisplayPrompt();
    char *cp;
        bool quitflag = false;
    while(enteredLines.empty()) {
                if (!DoEvents()) {
                  quitflag = true;
                  break;
                }
        }
        if (!quitflag) {
            std::string theline(enteredLines.front());
                enteredLines.pop_front();
                cp = strdup(theline.c_str());
        } else {
                cp = strdup("quit\n");
        }
    return cp;
  }

  void WinTerminal::Save() {
    char fname[MAX_PATH];
        char ftitle[MAX_PATH];
    ofn.Flags = OFN_OVERWRITEPROMPT;
        fname[0] = 0;
        ftitle[0] = 0;
    ofn.lpstrFile = fname;
        ofn.lpstrFileTitle = ftitle;
    if (!GetSaveFileName(&ofn)) {
                DWORD tmp;
                tmp = CommDlgExtendedError();
                return;
        }
    FILE *fp;
    fp = fopen(fname,"w");
    if (!fp) {
      char buffer[1000];
      sprintf(buffer,"Unable to open file %s for writing.",ofn.lpstrFile);
      MessageBox(hwnd, buffer, "Error on save", MB_OK);
    }   
    // Write the output
        int i;
    for (i=max(0,nlinecount-scrollback);i<nlinecount;i++) {
      fprintf(fp,"%s\n",&textbuf[(i%scrollback)*MAXCOLS]);
    }
    fclose(fp);
  }

  void WinTerminal::Copy() {
    int sel_y1, sel_y2, sel_x1, sel_x2;
    sel_y1 = selstart_row;
    sel_y2 = selstop_row;
    sel_x1 = selstart_col;
    sel_x2 = selstop_col;
    if (sel_y1 > sel_y2) {
      sel_y2 = selstart_row;
      sel_y1 = selstop_row;
      sel_x2 = selstart_col;
      sel_x1 = selstop_col;
    }
    HGLOBAL hGlobal;
    hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (sel_y2 - sel_y1 + 1)*ncolumn);
    LPVOID pGlobal;
    pGlobal = GlobalLock(hGlobal);
    char* ptr = (char*) pGlobal;
    for (int y=sel_y1;y <= sel_y2;y++) {
      char buffer[1000];
      int x;
      for (x=0;x<ncolumn;x++) {
        char ch = CharAt(y,x);
        buffer[x] = ch;
        if (!ch) break;
      }
      buffer[x] = 0;
          int textlen;
          textlen = strlen(buffer);
      if ((sel_y1 == sel_y2) && (y == sel_y1)) {
        if (sel_x1 >= sel_x2) {
          int t = sel_x1;
          sel_x1 = sel_x2;
          sel_x2 = t;
        }
        if (textlen > sel_x1) {
          memcpy(ptr, buffer+sel_x1, min(sel_x2,textlen)-sel_x1);
          ptr += min(sel_x2,textlen)-sel_x1;
          *ptr++ = '\r'; *ptr++ = '\n';
        }
      } else if (y == sel_y1) {
        if (textlen > sel_x1) {
          memcpy(ptr, buffer+sel_x1, textlen-sel_x1);
          ptr += textlen - sel_x1;
          *ptr++ = '\r'; *ptr++ = '\n';
        }
      } else if (y == sel_y2) {
        memcpy(ptr, buffer, min(sel_x2,textlen));
        ptr += min(sel_x2,textlen);
        *ptr++ = '\r'; *ptr++ = '\n';
      } else {
        memcpy(ptr, buffer, strlen(buffer));
        ptr += strlen(buffer);
        *ptr++ = '\r'; *ptr++ = '\n';
      }
    }
    GlobalUnlock(hGlobal);
    OpenClipboard(hwnd);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hGlobal);
    CloseClipboard();
  }

  void WinTerminal::Paste() {
    if (IsClipboardFormatAvailable(CF_TEXT)) {
      OpenClipboard(hwnd);
      HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
      char *ptext = (char*) malloc(GlobalSize(hGlobal));
      LPVOID pGlobal = GlobalLock(hGlobal);
      strcpy(ptext, (const char *)pGlobal);
      GlobalUnlock(hGlobal);
      CloseClipboard();
      char *cp = ptext;
      while (*cp) {
        OnChar(*cp++);
      }
      free(ptext);
    }
  }

  void WinTerminal::Font() {
    if (ChooseFont(&cft)) {
      hfnt = CreateFontIndirect(&lfont);
      HDC hdc;
      hdc = GetDC(hwnd);
      SelectObject(hdc, hfnt);
      TEXTMETRIC tm;
      GetTextMetrics(hdc, &tm);
      ReleaseDC(hwnd,hdc);
      charWidth = tm.tmAveCharWidth;
      charHeight = tm.tmHeight;
      RECT t;
      GetClientRect(hwnd, &t);
      Resize(t.right-t.left, t.bottom-t.top);
      InvalidateRect(hwnd,NULL,TRUE);
      UpdateWindow(hwnd);
      cft.Flags |= CF_INITTOLOGFONTSTRUCT;
    }
  }
  
  /*.......................................................................
   * Search backwards for the potential start of a filename. This
   * looks backwards from the specified index in a given string,
   * stopping at the first unescaped space or the start of the line.
   *
   * Input:
   *  string  const char *  The string to search backwards in.
   *  back_from      int    The index of the first character in string[]
   *                        that follows the pathname.
   * Output:
   *  return        char *  The pointer to the first character of
   *                        the potential pathname, or NULL on error.
   */
  static char *start_of_path(const char *string, int back_from)
  {
    int i, j;
    /*
     * Search backwards from the specified index.
     */
    for(i=back_from-1; i>=0; i--) {
      int c = string[i];
      /*
       * Stop on unescaped spaces.
       */
      if(isspace((int)(unsigned char)c)) {
        /*
         * The space can't be escaped if we are at the start of the line.
         */
        if(i==0)
          break;
        /*
         * Find the extent of the escape characters which precedes the space.
         */
        for(j=i-1; j>=0 && string[j]=='\\'; j--)
          ;
        /*
         * If there isn't an odd number of escape characters before the space,
         * then the space isn't escaped.
         */
        if((i - 1 - j) % 2 == 0)
          break;
      } 
      else if (!isalpha(c) && !isdigit(c) && (c != '_') && (c != '.') && (c != '\\') && (c != '/'))
        break;
    };
    return (char *)string + i + 1;
  }

  
  std::vector<std::string> 
  WinTerminal::GetCompletions(const char *line, int word_end, 
                              std::string &matchString) {
    std::vector<std::string> completions;
    /*
     * Find the start of the filename prefix to be completed, searching
     * backwards for the first unescaped space, or the start of the line.
     */
    char *start = start_of_path(line, word_end);
    char *tmp;
    int mtchlen;
    mtchlen = word_end - (start-line);
    tmp = (char*) malloc(mtchlen+1);
    memcpy(tmp,start,mtchlen);
    tmp[mtchlen] = 0;
    matchString = std::string(tmp);
    
    /*
     *  the preceeding character was not a ' (quote), then
     * do a command expansion, otherwise, do a filename expansion.
     */
    if (start[-1] != '\'') {
      std::vector<std::string> local_completions;
      std::vector<std::string> global_completions;
      int i;
      local_completions = context->getCurrentScope()->getCompletions(std::string(start));
      global_completions = context->getGlobalScope()->getCompletions(std::string(start));
      for (i=0;i<local_completions.size();i++)
        completions.push_back(local_completions[i]);
      for (i=0;i<global_completions.size();i++)
        completions.push_back(global_completions[i]);
      std::sort(completions.begin(),completions.end());
      return completions;
    } else {
      HANDLE hSearch;
      WIN32_FIND_DATA FileData;
      std::string pattern(tmp);
      pattern.append("*");
      OutputDebugString("Searching ");
      OutputDebugString(pattern.c_str());
      OutputDebugString("\n");
      hSearch = FindFirstFile(pattern.c_str(),&FileData);
      if (hSearch != INVALID_HANDLE_VALUE) {
        // Windows does not return any part of the path in the completion,
        // So we need to find the base part of the pattern.
        int lastslash;
        std::string prefix;
        lastslash = pattern.find_last_of("/");
        if (lastslash == -1) {
          lastslash = pattern.find_last_of("\\");
        }
        if (lastslash != -1)
          prefix = pattern.substr(0,lastslash+1);
        completions.push_back(prefix + FileData.cFileName);
        while (FindNextFile(hSearch, &FileData))
          completions.push_back(prefix + FileData.cFileName);
      }
      FindClose(hSearch);
      return completions;
    }
  }

  BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
  { 
    switch (message) 
    { 
        case WM_COMMAND:
                EndDialog(hwndDlg,0);
                break; 
        // Place message cases here. 
        
        default: 
            return FALSE; 
    } 
  }


  LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    WinTerminal* wptr;
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    wptr = (WinTerminal*) GetWindowLong(hwnd, GWL_USERDATA);
    switch (message)
      {
      case WM_SIZE:
        wptr->Resize(LOWORD(lParam), HIWORD(lParam));
        return 0;
      case WM_CREATE:
        return 0;
      case WM_LBUTTONDOWN:
        wptr->OnMouseDown(LOWORD(lParam),HIWORD(lParam));
        return 0;
      case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON)
          wptr->OnDrag(LOWORD(lParam),HIWORD(lParam));
        return 0;
      case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        wptr->OnDraw(hdc, ps);
        EndPaint(hwnd, &ps);
        return 0;
      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
      case WM_SETFOCUS:  
        // Create a solid black caret. 
        wptr->MakeCaret();
        wptr->DoMoveCaret();
        break;
      case WM_KILLFOCUS:  
        // The window is losing the keyboard focus, so destroy the caret. 
        DestroyCaret(); 
        break; 
      case WM_CHAR: {
                HideCaret(hwnd);
                wptr->OnChar(wParam);
                ShowCaret(hwnd);
        break;
      }
      case WM_VSCROLL: {
        wptr->OnScroll(LOWORD(wParam));
        break;
      }
      case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_EDIT_COPY:
          wptr->Copy();
          break;
        case IDM_EDIT_PASTE:
          wptr->Paste();
          break;
        case IDM_EDIT_FONT:
          wptr->Font();
          break;
        case IDM_FILE_SAVE:
          wptr->Save();
          break;
        case IDM_FILE_EXIT:
          PostQuitMessage(0);
          break;
        case ID_HELP_CONTENTS: 
          HtmlHelp(GetDesktopWindow(),filepath,HH_DISPLAY_TOC,NULL);
          break;
        case ID_HELP_ABOUTFREEMAT:
          DialogBox(NULL,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DlgProc);
          break;
        }
      case WM_KEYDOWN:{
        switch (wParam) {
        case VK_LEFT:
                HideCaret(hwnd);
          wptr->OnChar(KM_LEFT);
                ShowCaret(hwnd);
          break;
        case VK_RIGHT:
                HideCaret(hwnd);
          wptr->OnChar(KM_RIGHT);
                ShowCaret(hwnd);
          break;
        case VK_UP:
                HideCaret(hwnd);
          wptr->OnChar(KM_UP);
                ShowCaret(hwnd);
          break;
        case VK_DOWN:
                HideCaret(hwnd);
          wptr->OnChar(KM_DOWN);
                ShowCaret(hwnd);
          break;
        case VK_INSERT:
                HideCaret(hwnd);
          wptr->OnChar(KM_INSERT);
                ShowCaret(hwnd);
          break;
        case VK_HOME:
                HideCaret(hwnd);
          wptr->OnChar(KM_HOME);
                ShowCaret(hwnd);
          break;
        case VK_END:
                HideCaret(hwnd);
          wptr->OnChar(KM_END);
                ShowCaret(hwnd);
          break;
        }
      }
      }
    return DefWindowProc(hwnd, message, wParam, lParam);
  }
}
#endif
