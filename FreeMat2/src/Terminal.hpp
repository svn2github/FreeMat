#ifndef __Terminal_hpp__
#define __Terminal_hpp__

#include "Interface.hpp"
#include "KeyManager.hpp"
#include "WalkTree.hpp"
#include <termios.h>

namespace FreeMat {

  typedef struct {
    const char *sequence;
    int keycode;
  } mapping;

  typedef struct termios Termios;

  class Terminal : public KeyManager, public Interface {
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
    WalkTree *eval;
    std::vector<std::string> dirTab;
    Context *context;
    char *messageContext;
    std::vector<std::string> flist;
    std::list<std::string> enteredLines;
  public:
    Terminal();
    ~Terminal();
    void Initialize();
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
    virtual void initialize(std::string path, Context *ctxt);
    virtual void rescanPath();
    virtual void scanDirectory(std::string scdir);
    virtual void procFile(char *fname, std::string fullname);
    virtual int getTerminalWidth();
    virtual void outputMessage(const char* msg);
    virtual void errorMessage(const char* msg);
    virtual void warningMessage(const char* msg);
    virtual void setMessageContext(const char* msg);
    virtual void SetEvalEngine(WalkTree* a_eval);
    virtual void ExecuteLine(const char* line);
    virtual std::vector<std::string> GetCompletions(const char *line, int word_end, 
						    std::string &matchString);
    virtual char* getLine(const char* prompt);
  };
}

#endif
