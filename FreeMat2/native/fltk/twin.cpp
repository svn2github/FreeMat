#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "FL/Fl.H"
#include "FL/Fl_Double_Window.H"
#include "FL/Fl_Text_Buffer.H"
#include "FL/Fl_Text_Display.H"
#include "FL/Enumerations.H"

class TerminalWidget : public Fl_Text_Display {
  Fl_Text_Buffer *textbuf;
  const char *m_prompt;
  int promptlen;
public:
  TerminalWidget(int x, int y, int w, int h, const char *label = 0);
  ~TerminalWidget();
  int handle(int event);
  int handle_key();
  virtual void draw_cursor(int X, int Y);
  void getLine(const char* prompt);
  int backspace();
  int enter();
};


void TerminalWidget::getLine(const char*prompt) {
  m_prompt = prompt;
  promptlen = strlen(prompt);
  buffer()->append(prompt);
  insert_position(buffer()->length());
  Fl::run();
}

void TerminalWidget::draw_cursor(int X, int Y) {
  fl_color(FL_BLACK);
  fl_rectf(X,Y,5,12);
}

TerminalWidget::TerminalWidget(int x, int y, int w, int h, const char *label) :
  Fl_Text_Display(x,y,w,h,label) {
  textfont(FL_SCREEN);
  textsize(12);
  textbuf = new Fl_Text_Buffer;
  buffer(textbuf);
  cursor_style(Fl_Text_Display::CARET_CURSOR);
  show_cursor();
}

TerminalWidget::~TerminalWidget() {
  delete textbuf;
}

int TerminalWidget::backspace() {
  int line, col;
  position_to_linecol(insert_position(),&line,&col);
  if (col > promptlen)
    buffer()->remove(insert_position()-1,insert_position());
  return 1;
}

int TerminalWidget::enter() {
  buffer()->append("\n");
  insert_position(buffer()->length());
  return 1;
}

int TerminalWidget::handle_key() {
  int key;
  key = Fl::event_key();
  switch(key) {
  case FL_BackSpace:    
    return backspace();
    break;
  case FL_Enter:
    return enter();
    break;
  default:
    buffer()->insert(insert_position(),Fl::event_text());
  }
  //  buffer()->call_modify_callbacks();
  insert_position(buffer()->length());
  return 1;
}

int TerminalWidget::handle(int event) {
  if (event == FL_FOCUS)
    return 1;
  if (event == FL_UNFOCUS)
    return 1;
  if (event == FL_KEYBOARD) {
    return handle_key();
  }
  return(Fl_Text_Display::handle(event));
}

class TerminalWindow : public Fl_Double_Window {
public:
  TerminalWindow(int w, int h, const char *t);
  ~TerminalWindow();
  TerminalWidget* term;
  void addstuff();
  void getLine(const char *line);
};

TerminalWindow::TerminalWindow(int w, int h, const char* t) :
  Fl_Double_Window(w,h,t) {
  begin();
  term = new TerminalWidget(0,0,w,h);
  end();
  resizable(this);
  show();
}

TerminalWindow::~TerminalWindow() {
}

void TerminalWindow::addstuff() {
  char buffer[1000];
  int i;
  term->buffer()->append("FreeMat v 1.07\n");
  term->insert_position(term->buffer()->length());
  redraw();
}

void TerminalWindow::getLine(const char *line) {
  term->getLine(line);
}

int main(int argc, char **argv) {
  TerminalWindow* window = new TerminalWindow(600,400,"Terminal");
  window->show(1,argv);
  window->addstuff();
  window->getLine("--> ");
  return Fl::run();
}
