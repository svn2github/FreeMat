#include "WinTerminal.hpp"
#include "XWindow.hpp"
#include <string.h>
#include <algorithm>
#include "resource.h"

#include <direct.h>
#define getcwd _getcwd
#define S_ISREG(x) (x & _S_IFREG)


#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define MAXCOLS 256
namespace FreeMat {

  void WinTerminal::UpdateLineCount() {
    RECT winsze;
    GetClientRect(hwnd, &winsze);
    si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hwnd,SB_VERT,&si);
	int cpos = si.nPos;
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = max(0, nlinecount - scrollback);
    si.nMax = nlinecount-1;
    si.nPage = winsze.bottom / charHeight;
    si.nPos = si.nMax;
    SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
    GetScrollInfo(hwnd,SB_VERT,&si);
    if (cpos != si.nPos) {
      InvalidateRect(hwnd,NULL,TRUE);
      UpdateWindow(hwnd);
    }
  }
  
  char& WinTerminal::CharAt(int row, int column) {
    return textbuf[(row%scrollback)*MAXCOLS+column];
  }
  
  void WinTerminal::MakeCaret() {
    CreateCaret(hwnd, (HBITMAP) NULL, charWidth, charHeight);
	ShowCaret(hwnd);
  }
  
  void WinTerminal::DoMoveCaret() {
//    if (caret_y >= nlinecount) {
//      nlinecount = caret_y+1;
//     nlinecount = caret_y+1;
//    }
	nlinecount = max(nlinecount,caret_y + 1);
	si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    UpdateLineCount();
    int scrtop = si.nPos;
    if (scrtop < 0) scrtop = 0;
    int screen_x = caret_x * charWidth;
    int screen_y = (caret_y - scrtop) * charHeight;
    //int screen_y = caret_y * charHeight;
    HideCaret(hwnd);
    SetCaretPos(screen_x, screen_y);
    ShowCaret(hwnd);
  }

  LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

  void SetupWinTerminalClass(HINSTANCE hInstance) {
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 4;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wndclass.lpszClassName = "WinTerminal";
    if (!RegisterClass(&wndclass))
      {
	MessageBox(NULL, TEXT("This program requires Windows NT!"),
		   "WinTerm",MB_ICONERROR);
	return;
      }
  }

  WinTerminal::WinTerminal(HINSTANCE hInstance, int iCmdShow) {
    static TCHAR szFilter[] = "Text Files (*.TXT)\0*.txt\0\0";
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
    SetWindowLong(hwnd,GWL_USERDATA,(LONG) this);
    caret_x = 0;
    caret_y = 0;
    TEXTMETRIC tm;
    HDC hdc;
    hdc = GetDC(hwnd);
    hfnt = (HFONT) GetStockObject(ANSI_FIXED_FONT);
    SelectObject(hdc,hfnt);
    GetTextMetrics(hdc,&tm);
    ReleaseDC(hwnd,hdc);
    charWidth = tm.tmAveCharWidth;
    charHeight = tm.tmHeight;
    scrollback = 1000;
    textbuf = (char*) calloc(MAXCOLS*scrollback,sizeof(char));
    UpdateLineCount();
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);
    messageContext = NULL;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = NULL;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = 0;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "txt";
    ofn.lCustData = 0L;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    cft.lStructSize = sizeof(CHOOSEFONT);
    cft.hwndOwner = hwnd;
    cft.lpLogFont = &lfont;
    cft.Flags = CF_FIXEDPITCHONLY | CF_FORCEFONTEXIST | 
      CF_SCREENFONTS;
    cft.iPointSize = 12;
    selstart_col = 0;
    selstart_row = 0;
    selstop_col = 0;
    selstop_row = 0;
  }

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
    //    InvalidateRect(hwnd,NULL,TRUE);
    UpdateWindow(hwnd);
  }

  void WinTerminal::MoveBOL() {
    caret_x = 0;
    DoMoveCaret();
  }

  void WinTerminal::OutputRawString(std::string txt) {
    int startRow, startCol;
	bool redrawall;
    const char *cp;
    cp = txt.c_str();
    startRow = caret_y;
	redrawall=false;
    while (*cp) {
      if (*cp == '\n') {
	caret_y++;
	caret_x = 0;
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
  }

  void WinTerminal::OnScroll(int scrollType) {
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

  void WinTerminal::initialize(std::string path, Context *contxt) {
    char* pathdata = strdup(path.c_str());
    char* token;
    token = strtok(pathdata,";");
    while (token != NULL) {
      if (strcmp(token,".") != 0)
		dirTab.push_back(std::string(token));
      token = strtok(NULL,";");
    }
    context = contxt;
    rescanPath();
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
    if (messageContext != NULL)
      OutputRawString("   at " + TranslateString(messageContext) + "\r\n");
  }

  void WinTerminal::warningMessage(const char* msg) {
    std::string msg2(TranslateString(msg));
    OutputRawString("Warning: " + msg2 + "\r\n");
    if (messageContext != NULL)
      OutputRawString("   at " + TranslateString(messageContext) + "\r\n");
  }

  void WinTerminal::setMessageContext(const char* msg) {
    if (messageContext != NULL)
      free(messageContext);
    if (msg != NULL) 
      messageContext = strdup(msg);
    else
      messageContext = NULL;
  }

  void WinTerminal::SetEvalEngine(WalkTree* a_eval) {
    eval = a_eval;
  }
  
  void WinTerminal::ExecuteLine(const char * line) {
    enteredLines.push_back(line);
//	OutputRawString("\r\n");
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
	completions.push_back(FileData.cFileName);
	while (FindNextFile(hSearch, &FileData))
	  completions.push_back(FileData.cFileName);
	OutputDebugString("completion :");
	OutputDebugString(FileData.cFileName);
	OutputDebugString("\n");
      }
      FindClose(hSearch);
      return completions;
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
	wptr->OnChar(wParam);
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
	}
      case WM_KEYDOWN:{
	switch (wParam) {
	case VK_LEFT:
	  wptr->OnChar(KM_LEFT);
	  break;
	case VK_RIGHT:
	  wptr->OnChar(KM_RIGHT);
	  break;
	case VK_UP:
	  wptr->OnChar(KM_UP);
	  break;
	case VK_DOWN:
	  wptr->OnChar(KM_DOWN);
	  break;
	case VK_INSERT:
	  wptr->OnChar(KM_INSERT);
	  break;
	case VK_HOME:
	  wptr->OnChar(KM_HOME);
	  break;
	case VK_END:
	  wptr->OnChar(KM_END);
	  break;
	}
      }
      }
    return DefWindowProc(hwnd, message, wParam, lParam);
  }
}
		
