#include "FLTKTerminal.hpp"
#include "FL/Fl.H"
#include "FL/Enumerations.H"
#include "WalkTree.hpp"
#include "File.hpp"
#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#endif

#ifdef WIN32
#define DELIM "\\"
#else
#define DELIM "/"
#endif

//The scrollback buffer length - later to be put in the prefs file
#define SCROLLBACK 5000
// CTRL+D doesn't work yet.

void FLTKTerminalWidget::blinkCB(void* data) {
  ((FLTKTerminalWidget*) data)->blink();
  Fl::add_timeout(1, FLTKTerminalWidget::blinkCB, data);
}

void FLTKTerminalWidget::blink() {
  if (blinkactive) {
    blinkon = !blinkon;
    int X, Y;
    position_to_xy(mCursorPos, &X, &Y);
    damage(1, X, Y, fl_width('W'), mMaxsize);
  }
  blinkactive = true;
}

FLTKTerminalWidget::FLTKTerminalWidget(int x, int y, int w, int h, const char *label) : textInitialized(false),
  Fl_Text_Display(x,y,w,h,label) {
  textfont(FL_SCREEN);
  textsize(12);
  textbuf = new Fl_Text_Buffer;
  buffer(textbuf);
  cursor_style(Fl_Text_Display::CARET_CURSOR);
  //  show_cursor();
  m_context = NULL;
  linecount = 0;
  m_width = 80;
  m_height = 25;
  Fl::add_timeout(1, FLTKTerminalWidget::blinkCB, this);
  blinkon = true;
  blinkactive = false;
}

#define LEFT_MARGIN 3
#define RIGHT_MARGIN 3

void FLTKTerminalWidget::draw() {
  int X, Y;
  position_to_xy(mCursorPos, &X, &Y);
  Fl_Text_Display::draw();
  fl_push_clip(text_area.x-LEFT_MARGIN,
	       text_area.y,
	       text_area.w+LEFT_MARGIN+RIGHT_MARGIN,
	       text_area.h);
  if (blinkon) {
    fl_color(textcolor());
    fl_rectf(X, Y, fl_width('W'), mMaxsize);
  } else {
    fl_color(color());
    fl_rectf(X, Y, fl_width('W'), mMaxsize);
    fl_color(textcolor());
    if (mCursorPos < buffer()->length())
      fl_draw(buffer()->text_range(mCursorPos,mCursorPos+1),
	      1, X, Y + mMaxsize - fl_descent());
  }
  fl_pop_clip();
}

FLTKTerminalWidget::~FLTKTerminalWidget() {
}

void FLTKTerminalWidget::setContext(Context *ctxt) {
  if (m_context) delete m_context;
  m_context = ctxt;
}

void FLTKTerminalWidget::setPath(std::string path) {
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
    m_path = path;
    rescanPath();
}

std::string FLTKTerminalWidget::getPath() {
  return m_path;
}

void FLTKTerminalWidget::rescanPath() {
  int i;
  for (i=0;i<dirTab.size();i++)
    scanDirectory(dirTab[i]);
  // Scan the current working directory.
  char cwd[1024];
  getcwd(cwd,1024);
  scanDirectory(std::string(cwd));
}

char* FLTKTerminalWidget::getLine(const char*prompt) {
  m_prompt = prompt;
  promptlen = strlen(prompt);
  buffer()->append(prompt);
  insert_position(buffer()->length());
  history_ptr = -1;
  while(enteredLines.empty())
    Fl::wait(0);
  std::string theline(enteredLines.front());
  enteredLines.pop_front();
  char *cp = strdup(theline.c_str());
  return cp;
}

void FLTKTerminalWidget::RegisterInterrupt() {
  sigInterrupt(0);
}

int FLTKTerminalWidget::getTerminalWidth() {
  return m_width;
}

void FLTKTerminalWidget::adjustScrollPosition() {
  if (textInitialized) {
    int top;
    if (linecount > SCROLLBACK) {
      int startpos = 0;
      int stoppos = 0;
      for (int i=0;i<linecount-SCROLLBACK;i++)
	stoppos += buffer()->line_end(stoppos)+1;
      buffer()->remove(startpos,stoppos);
      linecount = SCROLLBACK;
    }
    top = linecount - m_height + 2;
    top = (top < 0) ? 0 : top;
    scroll(top,0);
    Fl::flush();
  }
}

void FLTKTerminalWidget::outputText(const char *txt) {
  bool linecount_changed;
  
  linecount_changed = false;
  const char *cp = txt;
  while (*cp)
    if (*cp++ == '\n') {
      linecount++;
      linecount_changed = true;
    }
  buffer()->append(txt);
  insert_position(buffer()->length());
  if (linecount_changed) 
    adjustScrollPosition();
  textInitialized = true;
  blinkactive = false;
  blinkon = true;
}

void FLTKTerminalWidget::resize(int X, int Y, int W, int H) {
  Fl_Text_Display::resize(X,Y,W,H);
  if (textInitialized) {
    m_width = w()/fl_width('W');
    m_height = h()/fl_height();
  }
}

void FLTKTerminalWidget::outputMessage(const char*msg) {
  outputText(msg);
}

void FLTKTerminalWidget::errorMessage(const char*msg) {
  outputText("Error: ");
  outputText(msg);
  outputText("\n");
  outputText("  at ");
  outputText(messageContext.c_str());
  outputText("\n");
}

void FLTKTerminalWidget::warningMessage(const char*msg) {
  outputText("Warning: ");
  outputText(msg);
  outputText("\n");
  outputText("  at ");
  outputText(messageContext.c_str());
  outputText("\n");
}

void FLTKTerminalWidget::adjustInsertPosition() {
  int line, col;
  position_to_linecol(insert_position(),&line,&col);
  if ((linecount+1) != line)
    insert_position(buffer()->length());
  adjustScrollPosition();
}

int FLTKTerminalWidget::leftkey() {
  int line, col;
  adjustInsertPosition();
  position_to_linecol(insert_position(),&line,&col);
  if (col > promptlen)
    insert_position(insert_position()-1);
  return 1;
}

int FLTKTerminalWidget::rightkey() {
  adjustInsertPosition();
  if (insert_position() < buffer()->length())
    insert_position(insert_position()+1);
  return 1;
}

int FLTKTerminalWidget::ctrla() {
  adjustInsertPosition();
  insert_position(buffer()->line_start(insert_position()) + promptlen);
  return 1;
}

int FLTKTerminalWidget::ctrle() {
  adjustInsertPosition();
  insert_position(buffer()->length());
  return 1;
}

int FLTKTerminalWidget::ctrlk() {
  adjustInsertPosition();
  int cutin, cutout;
  cutin = insert_position();
  cutout = buffer()->line_end(insert_position());
  cutbuff = buffer()->text_range(cutin,cutout);
  buffer()->remove(cutin,cutout);
  return 1;
}

int FLTKTerminalWidget::ctrly() {
  adjustInsertPosition();
  buffer()->insert(insert_position(),cutbuff.c_str());
  insert_position(insert_position()+cutbuff.size());
  return 1;
}

int FLTKTerminalWidget::tab() {
  int line, col;
  position_to_linecol(insert_position(),&line,&col);
  if ((col > promptlen) && (buffer()->character(insert_position()-1) != ' ') &&
      (buffer()->character(insert_position()-1) != '\t'))
    CompleteWord();
  else {
    buffer()->insert(insert_position(),"\t");
    insert_position(insert_position()+1);
  }
  return 1;
}

int FLTKTerminalWidget::upkey() {
  char tbuf[4096];
  adjustInsertPosition();
  if (history.size() == 0) return 1;
  if (history_ptr == -1)
    history_ptr = history.size() - 2;
  else {
    if (history_ptr == 0)
      history_ptr = history.size();
    history_ptr--;
  }
  sprintf(tbuf,"%s%s",m_prompt,history[history_ptr].c_str());
  tbuf[strlen(tbuf)-1] = 0;
  buffer()->replace(line_start(insert_position()),
		    line_start(insert_position())+strlen(tbuf),
		    tbuf);
  buffer()->remove(line_start(insert_position())+strlen(tbuf),
		   buffer()->line_end(insert_position()));
  return 1;
}

int FLTKTerminalWidget::downkey() {
  char tbuf[4096];
  adjustInsertPosition();
  if (history.size() == 0) return 1;
  history_ptr++;
  if (history_ptr >= history.size())
    history_ptr = 0;
  sprintf(tbuf,"%s%s",m_prompt,history[history_ptr].c_str());
  tbuf[strlen(tbuf)-1] = 0;
  buffer()->replace(line_start(insert_position()),
		    line_start(insert_position())+strlen(tbuf),
		    tbuf);
  buffer()->remove(line_start(insert_position())+strlen(tbuf),
		   buffer()->line_end(insert_position()));
  return 1;
}

int FLTKTerminalWidget::backspace() {
  int line, col;
  adjustInsertPosition();
  position_to_linecol(insert_position(),&line,&col);
  if (col > promptlen)
    buffer()->remove(insert_position()-1,insert_position());
  return 1;
}

int FLTKTerminalWidget::enter() {
  char tbuf[4096];
  adjustInsertPosition();
  strcpy(tbuf,buffer()->line_text(insert_position()));
  strcat(tbuf,"\n");
  enteredLines.push_back(tbuf+promptlen);
  outputText("\n");
  if (strlen(tbuf+promptlen) > 1) {
    if (history.size() > 0)
      history.pop_back();
    history.push_back(tbuf+promptlen);
    history.push_back(" ");
  }
  return 1;
}

int FLTKTerminalWidget::handleascii(int key) {
  adjustInsertPosition();
  if ((key>=32) && (key <129)) {
    buffer()->insert(insert_position(),Fl::event_text());
    insert_position(insert_position()+1);
  } 
  blinkactive = false;
  blinkon = true;
}

int FLTKTerminalWidget::handle_key() {
  int key;
  key = Fl::event_key();
  if ((key == 'a') && (Fl::event_state() & FL_CTRL))
    return ctrla();
  if ((key == 'e') && (Fl::event_state() & FL_CTRL))
    return ctrle();
  if ((key == 'k') && (Fl::event_state() & FL_CTRL))
    return ctrlk();
  if ((key == 'y') && (Fl::event_state() & FL_CTRL))
    return ctrly();
  switch(key) {
  case FL_Tab:
    return tab();
    break;
  case FL_BackSpace:    
    return backspace();
    break;
  case FL_Enter:
    return enter();
    break;
  case FL_Left:
    return leftkey();
    break;
  case FL_Right:
    return rightkey();
    break;
  case FL_Up:
    return upkey();
    break;
  case FL_Down:
    return downkey();
    break;
  default: 
    handleascii(key);
  }
  return 1;
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
FLTKTerminalWidget::GetCompletions(const char *line, int word_end, 
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
    local_completions = m_context->getCurrentScope()->getCompletions(std::string(start));
    global_completions = m_context->getGlobalScope()->getCompletions(std::string(start));
    for (i=0;i<local_completions.size();i++)
      completions.push_back(local_completions[i]);
    for (i=0;i<global_completions.size();i++)
      completions.push_back(global_completions[i]);
    std::sort(completions.begin(),completions.end());
    return completions;
  } else {
#ifdef WIN32
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
#else
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
#endif
  }
}

void FLTKTerminalWidget::ListCompletions(std::vector<std::string> completions) {
  int maxlen;    /* The length of the longest matching string */
  int width;     /* The width of a column */
  int ncol;      /* The number of columns to list */
  int nrow;      /* The number of rows needed to list all of the matches */
  int row,col;   /* The row and column being written to */
  int i;
  /*
   * Not enough space to list anything?
   */
  if(m_width < 1)
    return;
  /*
   * Work out the maximum length of the matching strings.
   */
  maxlen = 0;
  for(i=0; i<completions.size(); i++) {
    int len = completions[i].length();
    if(len > maxlen)
      maxlen = len;
  };
  /*
   * Nothing to list?
   */
  if(maxlen == 0)
    return;
  /*
   * Split the available terminal width into columns of maxlen + 2 characters.
   */
  width = maxlen + 2;
  ncol = m_width / width;
  /*
   * If the column width is greater than the terminal width, the matches will
   * just have to overlap onto the next line.
   */
  if(ncol < 1)
    ncol = 1;
  /*
   * How many rows will be needed?
   */
  nrow = (completions.size() + ncol - 1) / ncol;
  /*
   * Print the matches out in ncol columns, sorted in row order within each
   * column.
   */
  for(row=0; row < nrow; row++) {
    for(col=0; col < ncol; col++) {
      int m = col*nrow + row;
      if(m < completions.size()) {
	char buffer[4096];
	sprintf(buffer, "%s%-*s%s", completions[m].c_str(),
		(int) (ncol > 1 ? maxlen - completions[m].length():0),
		"", col<ncol-1 ? "  " : "\n");
	outputText(buffer);
      } else {
	outputText("\n");
	break;
      };
    };
  };
}

std::string GetCommonPrefix(std::vector<std::string> matches,
			    std::string tempstring) {
  int minlength;
  int prefixlength;
  bool allmatch;
  std::string templ;
  int i, j;

  minlength = matches[0].size();
  for (i=0;i<matches.size();i++)
    minlength = (minlength < matches[i].size()) ? 
      minlength : matches[i].size();
  prefixlength = minlength;
  templ = matches[0];
  for (i=0;i<matches.size();i++) {
    j = 0;
    allmatch = true;
    while (allmatch && (j<prefixlength)) {
      std::string mtch(matches[i]);
      allmatch = (mtch[j] == templ[j]);
      if (allmatch) j++;
    }
    prefixlength = (j < prefixlength) ? j : prefixlength;
  }
  if (prefixlength <= tempstring.length())
    return (std::string(""));
  else
    return(templ.substr(tempstring.length(),prefixlength-tempstring.length()));
}

void FLTKTerminalWidget::CompleteWord() {
  int redisplay = 0;
  int suffix_len;         /* The length of the completion extension */
  int cont_len;           /* The length of any continuation suffix */
  char *line_start;
  int relative_pos;
  int nextra;

  line_start = buffer()->line_text(insert_position());
  relative_pos = insert_position() - buffer()->line_start(insert_position());

  std::vector<std::string> matches;
  /*
   * Perform the completion.
   */
  std::string tempstring;
  matches = GetCompletions(line_start,relative_pos,tempstring);
  if(matches.size() >= 1) {
    /*
     * If there any ambiguous matches, report them, starting on a new line.
     */
    if(matches.size() > 1) {
      outputText("\n");
      ListCompletions(matches);
      buffer()->append(line_start);
      insert_position(relative_pos+buffer()->line_start(buffer()->length()));
    };
    /*
     * Find the common prefix
     */
    std::string prefix;
    prefix = GetCommonPrefix(matches, tempstring);
    /*
     * Get the length of the suffix and any continuation suffix to add to it.
     */
    suffix_len = prefix.length(); // This is supposed to be the length of the filename extension...
    cont_len = 0;
    /*
     * Work out the number of characters that are to be added.
     */
    nextra = suffix_len + cont_len;
    /*
     * Is there anything to be added?
     */
    buffer()->insert(insert_position(),prefix.c_str());
    insert_position(insert_position()+suffix_len);
  }
  return;
}


void FLTKTerminalWidget::scanDirectory(std::string scdir) {
#ifdef WIN32
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
#else
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
#endif
}

void FLTKTerminalWidget::procFile(std::string fname, std::string fullname) {
#ifdef WIN32
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
      if (!m_context->lookupFunctionGlobally(std::string(fname),fdef)) {
	MFunctionDef *adef;
	adef = new MFunctionDef();
	adef->name = std::string(fname);
	adef->fileName = fullname;
	m_context->insertFunctionGlobally(adef);
      }
    }else if (fname[namelen-2] == '.' && 
	      (fname[namelen-1] == 'p' ||
	       fname[namelen-1] == 'P')) {
      fname[namelen-2] = 0;
      // Look for the function in the context - only insert it
      // if it is not already defined.
      FunctionDef *fdef;
      if (!m_context->lookupFunctionGlobally(std::string(fname),fdef)) {
	MFunctionDef *adef;
	// Open the file
	File *f = new File(fullname.c_str(),"rb");
	Serialize *s = new Serialize(f);
	s->handshakeClient();
	s->checkSignature('p',1);
	adef = ThawMFunction(s);
	adef->pcodeFunction = true;
	m_context->insertFunctionGlobally(adef);
      }
    }
  }
  free(fname);
#else
  struct stat filestat;
  char buffer[1024];
  char fnamec[1024];
  strcpy(fnamec,fname.c_str());
  stat(fullname.c_str(),&filestat);
  if (S_ISREG(filestat.st_mode)) {
    int namelen;
    namelen = strlen(fnamec);
    if (fnamec[namelen-2] == '.' && 
	(fnamec[namelen-1] == 'm' ||
	 fnamec[namelen-1] == 'M')) {
      fnamec[namelen-2] = 0;
      // Look for the function in the context - only insert it
      // if it is not already defined.
      FunctionDef *fdef;
      if (!m_context->lookupFunctionGlobally(std::string(fnamec),fdef)) {
	MFunctionDef *adef;
	adef = new MFunctionDef();
	adef->name = std::string(fnamec);
	adef->fileName = fullname;
	m_context->insertFunctionGlobally(adef);
      }
    } else if (fnamec[namelen-2] == '.' && 
	       (fnamec[namelen-1] == 'p' ||
		fnamec[namelen-1] == 'P')) {
      fnamec[namelen-2] = 0;
      // Look for the function in the context - only insert it
      // if it is not already defined.
      FunctionDef *fdef;
      if (!m_context->lookupFunctionGlobally(std::string(fnamec),fdef)) {
	MFunctionDef *adef;
	// Open the file
	File *f = new File(fullname.c_str(),"rb");
	Serialize *s = new Serialize(f);
	s->handshakeClient();
	s->checkSignature('p',1);
	adef = ThawMFunction(s);
	adef->pcodeFunction = true;
	m_context->insertFunctionGlobally(adef);
      }
    }
  } else if (S_ISLNK(filestat.st_mode)) {
    int lncnt = readlink(fullname.c_str(),buffer,1024);
    buffer[lncnt] = 0;
    procFile(fnamec, std::string(buffer));
  }
#endif
}

int FLTKTerminalWidget::handle(int event) {
  if (event == FL_KEYBOARD) {
    return handle_key();
  }
  return(Fl_Text_Display::handle(event));
}


FLTKTerminalWindow::FLTKTerminalWindow(int w, int h, const char* t) :
  Fl_Double_Window(w,h,t) {
  begin();
  m_term = new FLTKTerminalWidget(0,0,w,h);
  end();
  resizable(this);
  show();
}

FLTKTerminalWindow::~FLTKTerminalWindow() {
}

FLTKTerminalWidget* FLTKTerminalWindow::term() {
  return m_term;
}
