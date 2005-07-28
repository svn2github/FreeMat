#ifndef __Terminal_hpp__
#define __Terminal_hpp__

#include "KeyManager.hpp"
#include <termios.h>

namespace FreeMat {

  typedef struct {
    const char *sequence;
    int keycode;
  } mapping;

  typedef struct termios Termios;

  class Terminal : public KeyManager {
  protected:
    int state;
    Termios oldattr;
    Termios newattr;
    const char *term;
    const char *left, *right, *up, *down, *home, *bol;
    const char *clear_eol, *clear_eod;
    const char *u_arrow, *d_arrow, *l_arrow, *r_arrow;
    char *tgetent_buf, *tgetstr_buf;
    char escseq[50];
    mapping *esc_seq_array;
    int esc_seq_count;
    std::vector<std::string> flist;
    std::list<std::string> enteredLines;
  public:
    Terminal();
    virtual ~Terminal();
    virtual void Initialize();
    virtual void SetRawMode();
    virtual void RestoreOriginalMode();
    virtual void RetrieveTerminalName();
    const char* LookupControlString(const char *name);
    virtual void SetupControlStrings();
    virtual void ProcessChar(char c);
    virtual void ResizeEvent();
    virtual void MoveDown();
    virtual void MoveUp();
    virtual void MoveRight();
    virtual void MoveLeft();
    virtual void ClearEOL();
    virtual void ClearEOD();
    virtual void MoveBOL();
    virtual void OutputRawString(std::string txt);
    virtual int getTerminalWidth();
  };
}

#endif
