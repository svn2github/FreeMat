#include "Terminal.hpp"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <curses.h>
#include <string>
#include <sys/ioctl.h>
#include "Exception.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#include "XWindow.hpp"

#define KM_ESC       0x1b

#ifdef WIN32
#define DELIM "\\"
#else
#define DELIM "/"
#endif


namespace FreeMat {
  // Set up the terminal in raw mode, and initialize the control
  // strings.
  Terminal::Terminal() {
  }

  void Terminal::Initialize() {
    RetrieveTerminalName();
    SetRawMode();
    SetupControlStrings();
    ResizeEvent();
    state = 0;
  }

  Terminal::~Terminal() {
  }

  void Terminal::SetRawMode() {
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    newattr.c_iflag &= ~(ICRNL | INPCK | ISTRIP);
    newattr.c_cflag &= ~(CSIZE | PARENB);
    newattr.c_cflag |= CS8;
    newattr.c_oflag &= ~(OPOST);  
    newattr.c_cc[VMIN] = 1;
    newattr.c_cc[VTIME] = 0;
    while (tcsetattr(STDIN_FILENO, TCSADRAIN, &newattr)) {
      if (errno != EINTR) 
	throw Exception(std::string("Unable to set up terminal attributes: tcsetattr error:") + strerror(errno));
    }
  }
  
  void Terminal::RestoreOriginalMode() {
    // Restore the original terminal setttings
    while (tcsetattr(STDIN_FILENO, TCSADRAIN, &oldattr)) {
      if (errno != EINTR)
	throw Exception(std::string("Unable to set up terminal attributes: tcsetattr error:") + strerror(errno));
    }
  }

  void Terminal::RetrieveTerminalName() {
    term = getenv("TERM");
    if (!term)
      throw Exception("Unable to retrieve terminal name!");
    if (setupterm((char*) term, STDIN_FILENO, NULL) == ERR)
      throw Exception(std::string("Unable to retrieve terminal info for ") + term);
  }

  const char* Terminal::LookupControlString(const char *name) {
    const char* value = tigetstr((char*)name);
    if (!value || value == (char*) -1)
      return NULL;
    else
      return strdup(value);
  }

  void Terminal::SetupControlStrings() {
    left = LookupControlString("cub1");
    right = LookupControlString("cuf1");
    up = LookupControlString("cuu1");
    down = LookupControlString("cud1");
    home = LookupControlString("home");
    clear_eol = LookupControlString("el");
    clear_eod = LookupControlString("ed");
    u_arrow = LookupControlString("kcuu1");
    d_arrow = LookupControlString("kcud1");
    l_arrow = LookupControlString("kcub1");
    r_arrow = LookupControlString("kcuf1");
    const char* delstr = LookupControlString("kdch1");
    // Store these in the esc_seq_array for later lookup
    esc_seq_array = (mapping*) malloc(13*sizeof(mapping));
    esc_seq_count = 13;
    // These are the ones provided by terminfo
    esc_seq_array[0].sequence = l_arrow;
    esc_seq_array[0].keycode = 0x101;
    esc_seq_array[1].sequence = r_arrow;
    esc_seq_array[1].keycode = 0x102;
    esc_seq_array[2].sequence = u_arrow;
    esc_seq_array[2].keycode = 0x103;
    esc_seq_array[3].sequence = d_arrow;
    esc_seq_array[3].keycode = 0x104;
    // These are defaults...
    esc_seq_array[4].sequence = "\033[D";
    esc_seq_array[4].keycode = 0x101;
    esc_seq_array[5].sequence = "\033[C";
    esc_seq_array[5].keycode = 0x102;
    esc_seq_array[6].sequence = "\033[A";
    esc_seq_array[6].keycode = 0x103;
    esc_seq_array[7].sequence = "\033[B";
    esc_seq_array[7].keycode = 0x104;
    // These are defaults...
    esc_seq_array[8].sequence = "\033OD";
    esc_seq_array[8].keycode = 0x101;
    esc_seq_array[9].sequence = "\033OC";
    esc_seq_array[9].keycode = 0x102;
    esc_seq_array[10].sequence = "\033OA";
    esc_seq_array[10].keycode = 0x103;
    esc_seq_array[11].sequence = "\033OB";
    esc_seq_array[11].keycode = 0x104;
    // The delete character...
    esc_seq_array[12].sequence = delstr;
    esc_seq_array[12].keycode = 0x108;
  }

  // Translate the given character (which is a raw
  // character) - this traps escape sequences and
  // maps them to integer constants (as expected
  // by, e.g. KeyManager).
  void Terminal::ProcessChar(char c) {
    // Check our current state, if it is zero, we are not
    // in the middle of an escape sequence.  
    if (state == 0) {
      if (c != KM_ESC) {
	OnChar(c);
	return;
      } else {
	escseq[0] = KM_ESC;
	state = 1;
	return;
      }
    }
    // We are in the middle of an escape sequence.
    // Append the current character to the escseq
    // buffer, increase the state number.
    escseq[state] = c; escseq[state+1] = 0;
    state++;
    // Now we check against the contents of esc-seq-array for
    // a match
    int matchnum = 0;
    bool isprefix = false;
    bool matchfound = false;
    while (!matchfound && (matchnum < esc_seq_count)) {
      matchfound = (strcmp(escseq,esc_seq_array[matchnum].sequence)==0);
      isprefix = isprefix | (strncmp(escseq,esc_seq_array[matchnum].sequence,state)==0);
      if (!matchfound) matchnum++;
    }
    // Did we find a match? If so, convert to an extended key
    // code (by adding 0x101), reset state to 0, and call the callback.
    if (matchfound) {
      state = 0;
      OnChar(esc_seq_array[matchnum].keycode);
      return;
    }
    // Not a prefix of any known codes... ignore it
    if (!isprefix) {
      state = 0;
    }
  }

  // The terminal has been resized - calculate the 
  void Terminal::ResizeEvent() {
    int lines_used;       /* The number of lines currently in use */
    struct winsize size;  /* The new size information */
    int i;
    /*
     * Query the new terminal window size. Ignore invalid responses.
     */
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 &&
       size.ws_row > 0 && size.ws_col > 0) {
      /*
       * How many lines are currently displayed.
       */
      lines_used = (DisplayedStringWidth(line,-1,prompt_len) +
		    prompt_len + ncolumn - 1) / ncolumn;
      /*
       * Move to the cursor to the start of the line.
       */
      for(i=1; i<lines_used; i++) 
	MoveUp();
      MoveBOL();
    
      /*
       * Clear to the end of the terminal.
       */
      ClearEOD();
      //       if(gl_output_control_sequence(gl, size.ws_row, gl->clear_eod))
      // 	return 1;
      /*
       * Record the fact that the cursor is now at the beginning of the line.
       */
      term_curpos = 0;
      /*
       * Update the recorded window size.
       */
      nline = size.ws_row;
      ncolumn = size.ws_col;
    };
    Redisplay();
  }

  void Terminal::MoveDown() {
    tputs(down,1,putchar);
    fflush(stdout);
  }

  void Terminal::MoveUp() {
    tputs(up,1,putchar);
    fflush(stdout);
  }

  void Terminal::MoveRight() {
    tputs(right,1,putchar);
    fflush(stdout);
  }

  void Terminal::MoveLeft() {
    tputs(left,1,putchar);
    fflush(stdout);
  }

  void Terminal::ClearEOL() {
    tputs(clear_eol,1,putchar);
    fflush(stdout);
  }

  void Terminal::ClearEOD() {
    tputs(clear_eod,nline,putchar);
    fflush(stdout);
  }

  void Terminal::MoveBOL() {
    //  tputs(home,1,putchar);
    putchar('\r');
    fflush(stdout);
  }

  void Terminal::OutputRawString(std::string txt) {
    int ndone = 0;   /* The number of characters written so far */
    /*
     * How long is the string to be written?
     */
    int slen = txt.size();
    /*
     * Attempt to write the string to the terminal, restarting the
     * write if a signal is caught.
     */
    while(ndone < slen) {
      int nnew = write(STDOUT_FILENO, txt.c_str() + ndone, 
		       sizeof(char)*(slen-ndone));
      if(nnew > 0)
	ndone += nnew;
      else if(errno != EINTR && errno != EAGAIN) {
 	perror("write");
 	throw Exception("stop");
      }
    }
    //    fflush(stdout);
  }

  void Terminal::setContext(Context *contxt) {
    context = contxt;
  }

  void Terminal::setPath(std::string path) {
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

  std::string Terminal::getPath() {
    return m_path;
  }

  void Terminal::rescanPath() {
    int i;
    for (i=0;i<dirTab.size();i++)
      scanDirectory(dirTab[i]);
    // Scan the current working directory.
    char cwd[1024];
    getcwd(cwd,1024);
    scanDirectory(std::string(cwd));
  }

  void Terminal::scanDirectory(std::string scdir) {
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

  void Terminal::procFile(char *fname, 
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

  int Terminal::getTerminalWidth() {
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
  
  void Terminal::outputMessage(const char* msg) {
    //     FILE *fp;
    //     fp = fopen("log.txt","a");
    //     fwrite(msg,strlen(msg),1,fp);
    //     fclose(fp);
    std::string msg2(TranslateString(msg));
    OutputRawString(msg2);
  }

  void Terminal::errorMessage(const char* msg) {
    std::string msg2(TranslateString(msg));
    OutputRawString("Error: " + msg2 + "\r\n");
    OutputRawString("   at " + TranslateString(messageContext) + "\r\n");
  }

  void Terminal::warningMessage(const char* msg) {
    std::string msg2(TranslateString(msg));
    OutputRawString("Warning: " + msg2 + "\r\n");
    OutputRawString("   at " + TranslateString(messageContext) + "\r\n");
  }

  void Terminal::SetEvalEngine(WalkTree* a_eval) {
    eval = a_eval;
  }

  void Terminal::ExecuteLine(const char * line) {
//     if (eval->evaluateString((char*) line)) 
//       exit(1);
    enteredLines.push_back(line);
    ReplacePrompt("");
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

  char* Terminal::getLine(const char* prompt) {
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

  std::vector<std::string> Terminal::GetCompletions(const char *line, int word_end, 
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

}
