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

#define KM_ESC       0x1b

#define Exception(x) (x)

// Set up the terminal in raw mode, and initialize the control
// strings.
Terminal::Terminal() {
  RetrieveTerminalName();
  SetRawMode();
  SetupControlStrings();
  state = 0;
}

Terminal::~Terminal() {
  RestoreOriginalMode();
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
  // Store these in the esc_seq_array for later lookup
  esc_seq_array = (mapping*) malloc(12*sizeof(mapping));
  esc_seq_count = 12;
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
  printf("%s",txt.c_str());
  fflush(stdout);
}
