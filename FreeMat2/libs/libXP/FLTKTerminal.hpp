#ifndef __FLTKTerminal_hpp__
#define __FLTKTerminal_hpp__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "FL/Fl_Double_Window.H"
#include "FL/Fl_Text_Buffer.H"
#include "FL/Fl_Text_Display.H"
#include "FL/Fl_Menu_Bar.H"
#include "Interface.hpp"
#include "WalkTree.hpp"
#include "Context.hpp"

using namespace FreeMat;

class FLTKTerminalWidget : public Fl_Text_Display, public Interface {
  Fl_Text_Buffer *textbuf;
  const char *m_prompt;
  int promptlen;
  Context *m_context; 
  std::vector<std::string> dirTab;
  std::list<std::string> enteredLines;
  bool enteredLinesChanged;
  std::string m_path;
  int linecount;
  int m_width, m_height;
  bool textInitialized;
  std::vector<std::string> history;
  int history_ptr;
  std::string cutbuff;
  bool blinkon;
  bool blinkactive;
  bool crflag;
  int sizetext;
public:
  FLTKTerminalWidget(int x, int y, int w, int h, const char *label = 0);
  virtual ~FLTKTerminalWidget();
  void setContext(Context *ctxt);
  std::string getPath();
  void setPath(std::string);
  void rescanPath();
  char* getLine(const char *prompt);
  int getTerminalWidth();
  void outputMessage(const char* msg);
  void errorMessage(const char* msg);
  void warningMessage(const char* msg);
  void outputText(const char *txt);
  void fontsize(int sze);
  int handle(int event);
  int handle_key();
  int backspace();
  int enter();
  int leftkey();
  int rightkey();
  int upkey();
  int downkey();
  int ctrla();
  int ctrlc();
  int ctrld();
  int ctrle();
  int ctrlk();
  int ctrly();
  int tab();
  int handleascii(int key);
  void adjustInsertPosition();
  void adjustScrollPosition();
  void scanDirectory(std::string scdir, bool);
  void procFile(std::string fname, std::string fullname, bool);
  void resize(int X, int Y, int W, int H);
  void RegisterInterrupt();
  std::vector<std::string> GetCompletions(const char *line, int word_end, 
					  std::string &matchString);
  void CompleteWord();
  void ListCompletions(std::vector<std::string> completions);
  void draw(void);
  void blink();
  void docopy();
  void dopaste();
  static void blinkCB(void* data);
};

class FLTKTerminalWindow : public Fl_Double_Window {
  FLTKTerminalWidget *m_term;
  Fl_Menu_Bar *m_menu;
public:
  FLTKTerminalWindow(int w, int h, const char *t, const char *helppath);
  ~FLTKTerminalWindow();
  FLTKTerminalWidget* term();
};

#endif

