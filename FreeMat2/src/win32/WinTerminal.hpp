#ifndef __WinTerminal_hpp__
#define __WinTerminal_hpp__

#include <windows.h>
#include "KeyManager.hpp"
#include "Interface.hpp"
#include "WalkTree.hpp"

namespace FreeMat {
  class WinTerminal : public KeyManager, public Interface {
    int selstart_row, selstart_col;
    int selstop_row, selstop_col;
    int caret_x, caret_y;
    int charWidth, charHeight;
    int scrollback;
    char *textbuf;
    int nlinecount;
    HWND hwnd;
    HFONT hfnt;
    SCROLLINFO si;
    WalkTree *eval;
    std::vector<std::string> dirTab;
    Context *context;
    char *messageContext;
    std::vector<std::string> flist;
    std::list<std::string> enteredLines;
  public:
    WinTerminal(HINSTANCE hInstance, int iCmdShow);
    ~WinTerminal() {};
    int getCaretX() {return caret_x;}
    int getCaretY() {return caret_y;}
    void MoveDown();
    void MoveUp();
    void MoveRight();
    void MoveLeft();
    void ClearEOL();
    void ClearEOD();
    void MoveBOL();
    void OutputRawString(std::string txt);
    char& CharAt(int row, int column);
    void DoMoveCaret();
    void OnDraw(HDC hdc, PAINTSTRUCT ps);
    void MakeCaret();
    void UpdateLineCount();
    void Resize(int w, int h);
    void OnScroll(int scrollType);
    void initialize(std::string path, Context *contxt);
    void OnMouseDown(int x, int y);
    void OnDrag(int x, int y);
    virtual void rescanPath();
    virtual void scanDirectory(std::string scdir);
    virtual void procFile(std::string fname, std::string fullname);
    virtual int getTerminalWidth();
    virtual void outputMessage(const char* msg);
    virtual void errorMessage(const char* msg);
    virtual void warningMessage(const char* msg);
    virtual void setMessageContext(const char* msg);
    virtual void SetEvalEngine(WalkTree* a_eval);
    virtual void ExecuteLine(const char* line);
    virtual void RegisterInterrupt();
    virtual std::vector<std::string> GetCompletions(const char *line, int word_end, 
						    std::string &matchString);
    virtual char* getLine(const char* prompt);
	void Copy();
	void Paste();
  };

  void SetupWinTerminalClass(HINSTANCE hInstance);
};
#endif
