#ifndef __Terminal_hpp__
#define __Terminal_hpp__

#include "KeyManager.hpp"
#include <termios.h>

typedef struct {
  const char *sequence;
  int keycode;
} mapping;

typedef struct termios Termios;

class Terminal : public KeyManager {
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
 public:
  Terminal();
  ~Terminal();
  void SetRawMode();
  void RestoreOriginalMode();
  void RetrieveTerminalName();
  const char* LookupControlString(const char *name);
  void SetupControlStrings();
  void ProcessChar(char c);
  void ResizeEvent();
  virtual void MoveDown();
  virtual void MoveUp();
  virtual void MoveRight();
  virtual void MoveLeft();
  virtual void ClearEOL();
  virtual void ClearEOD();
  virtual void MoveBOL();
  virtual void OutputRawString(std::string txt);
};


#endif
