#include "WinTerminal.hpp"
#include <string.h>
#include <unistd.h>

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define MAXCOLS 256
namespace FreeMat {
  
  void WinTerminal::UpdateLineCount() {
    RECT winsze;
    GetClientRect(hwnd, &winsze);
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    //  si.nMin = 0;
    //  si.nMax = nlinecount - 1;
    si.nMin = max(0, nlinecount - scrollback);
    si.nMax = nlinecount - 1;
    si.nPage = winsze.bottom / charHeight;
    si.nPos = si.nMax;
    SetScrollInfo(hwnd,SB_VERT,&si,TRUE);
  }
  
  char& WinTerminal::CharAt(int row, int column) {
    return textbuf[(row%scrollback)*MAXCOLS+column];
  }
  
  void WinTerminal::MakeCaret() {
    CreateCaret(hwnd, (HBITMAP) NULL, charWidth, charHeight);
  }
  
  void WinTerminal::DoMoveCaret() {
    if (caret_y >= nlinecount) {
      nlinecount = caret_y+1;
    }
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    UpdateLineCount();
    int scrtop = (int)si.nPos - si.nPage + 1;
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
    wndclass.lpszMenuName = NULL;
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
			TEXT("The Hello Program"),
			WS_OVERLAPPEDWINDOW | WS_VSCROLL,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
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
    InvalidateRect(hwnd,NULL,TRUE);
    UpdateWindow(hwnd);
  }

  void WinTerminal::ClearEOD() {
    int i,  j;
    ClearEOL();
    for (i=caret_y+1;i<nlinecount;i++) {
      for (j=0;j<MAXCOLS;j++)
	CharAt(i,j) = 0;
    }
    InvalidateRect(hwnd,NULL,TRUE);
    UpdateWindow(hwnd);
  }

  void WinTerminal::MoveBOL() {
    caret_x = 0;
    DoMoveCaret();
  }

  void WinTerminal::OutputRawString(std::string txt) {
    int startRow, startCol;

    const char *cp;
    cp = txt.c_str();
    startRow = caret_y;
    while (*cp) {
      if (*cp == '\n') {
	caret_y++;
	caret_x = 0;
	cp++;
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
    //   // Set the scroll
    //   int scrollstart;
    //   scrollstart = (nlinecount < scrollback) ? nlineccount : scrollback;
    //   scrollstart -= nline;
    //   if (scrollstart < 0) scrollstart = 0;
    //   Scroll(0,scrollstart);
    //   DoMoveCaret();
    //   // Setup a refresh region that starts at startRow,startCol --> caretRow, caretCol
    //   int rectx1, recty1, rectx2, recty2, adjrow;
    //   // First map the start row to screen coordinates
    //   adjrow = startRow;
    //   if (adjrow >= scrollback)
    //     adjrow = scrollback-1;
    //   CalcScrolledPosition(0,adjrow*charHeight,&rectx1,&recty1);
    //   wxRect rect(rectx1, recty1, ncolumn*charWidth, (caretRow - startRow + 1)*charHeight);
    //   RefreshRect(rect);
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hwnd, SB_VERT, &si);
    RECT refresh;
    refresh.left = 0;
    refresh.right = ncolumn*charWidth;
    refresh.top = (startRow-si.nPos)*charHeight;
    refresh.bottom = (caret_y+1-si.nPos)*charHeight;
    InvalidateRect(hwnd,&refresh,TRUE);
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
    for (int y=paintBeg; y <= paintEnd; y++) {
      char buffer[1000];
      int x;
      for (x=0;x<ncolumn;x++) {
	char ch = CharAt(y,x);
	buffer[x] = ch;
	if (!ch) break;
      }
      buffer[x] = 0;
      if (strlen(buffer) != 0)
	TextOut(hdc,0,(y-iVertpos)*charHeight,buffer,strlen(buffer));
    }
    ShowCaret(hwnd);
  }

  void WinTerminal::Resize(int xdim, int ydim) {
    UpdateLineCount();
    DoResizeBuffer(xdim/charWidth,ydim/charHeight);
  }

  void WinTerminal::initialize(std::string path, Context *contxt) {
    char* pathdata = strdup(path.c_str());
    // Search through the path
    char *saveptr = (char*) malloc(sizeof(char)*1024);
    char* token;
    token = strtok_r(pathdata,":",&saveptr);
    while (token != NULL) {
      if (strcmp(token,".") != 0)
	dirTab.push_back(std::string(token));
      token = strtok_r(NULL,":",&saveptr);
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

  void WinTerminal::scanDirectory(std::string scdir) {
    // Open the directory
    DIR *dir;

    dir = opendir(scdir.c_str());
    if (dir == NULL) return;
    // Scan through the directory..
    struct dirent *fspec;
    char *fname;
    std::string fullname;
    while (fspec = readdir(dir)) {
      // Get the name of the entry
      fname = fspec->d_name;
      // Check for '.' and '..'
      if ((strcmp(fname,".") == 0) || (strcmp(fname,"..") == 0)) 
	continue;
      // Stat the file...
      fullname = std::string(scdir + std::string(DELIM) + fname);
      procFile(fname,fullname);
    }
    closedir(dir);
  }

  void WinTerminal::procFile(char *fname, 
			     std::string fullname) {
    struct stat filestat;
    char buffer[1024];
  
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
    } else if (S_ISLNK(filestat.st_mode)) {
      int lncnt = readlink(fullname.c_str(),buffer,1024);
      buffer[lncnt] = 0;
      procFile(fname, std::string(buffer));
    }
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
    ReplacePrompt("");
  }
  
  char* WinTerminal::getLine(const char* prompt) {
    fflush(stdout);
    ReplacePrompt(prompt);
    DisplayPrompt();
    while(enteredLines.empty())
      DoEvents();
    std::string theline(enteredLines.front());
    enteredLines.pop_front();
    char *cp;
    cp = strdup(theline.c_str());
    return cp;
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
      glob_t names;
      std::string pattern(tmp);
      pattern.append("*");
      glob(pattern.c_str(), GLOB_MARK, NULL, &names);
      int i;
      for (i=0;i<names.gl_pathc;i++) 
	completions.push_back(names.gl_pathv[i]);
      globfree(&names);
      free(tmp);
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
	// Adjust the caret position, in client coordinates. 
	SetCaretPos(wptr->getCaretX(), wptr->getCaretY()); 
	// Display the caret. 
	ShowCaret(hwnd); 
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
		
