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
    WalkTree *eval;
    std::vector<std::string> dirTab;
    Context *context;
    std::vector<std::string> flist;
    std::list<std::string> enteredLines;
    std::string m_path;
  public:
    Terminal();
    ~Terminal();
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
    virtual void setContext(Context *ctxt);
    virtual std::string getPath();
    virtual void setPath(std::string path);
    virtual void rescanPath();
    virtual void scanDirectory(std::string scdir, bool tempfunc);
    virtual void procFile(char *fname, std::string fullname, bool tempfunc);
    virtual int getTerminalWidth();
    virtual void outputMessage(const char* msg);
    virtual void errorMessage(const char* msg);
    virtual void warningMessage(const char* msg);
    virtual void SetEvalEngine(WalkTree* a_eval);
    virtual void ExecuteLine(const char* line);
    virtual std::vector<std::string> GetCompletions(const char *line, int word_end, 
						    std::string &matchString);
    virtual char* getLine(const char* prompt);
  };
}

#endif
