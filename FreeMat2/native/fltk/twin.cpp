#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "FL/Fl.H"
#include "FL/Fl_Double_Window.H"
#include "FL/Fl_Text_Buffer.H"
#include "FL/Fl_Text_Display.H"

class TerminalWidget : public Fl_Text_Display {
  Fl_Text_Buffer *textbuf;
public:
  TerminalWidget(int x, int y, int w, int h, const char *label = 0);
  ~TerminalWidget();
  int handle(int event);
};

TerminalWidget::TerminalWidget(int x, int y, int w, int h, const char *label) :
  Fl_Text_Display(x,y,w,h,label) {
  textbuf = new Fl_Text_Buffer;
  buffer(textbuf);
  show_cursor();
}

TerminalWidget::~TerminalWidget() {
  delete textbuf;
}

int TerminalWidget::handle(int event) {
  if (event == FL_FOCUS)
    return 1;
  if (event == FL_UNFOCUS)
    return 1;
  if (event == FL_KEYDOWN) {
    insert(Fl::event_text());
    return 1; 
  }
  return(Fl_Text_Display::handle(event));
}

class TerminalWindow : public Fl_Double_Window {
public:
  TerminalWindow(int w, int h, const char *t);
  ~TerminalWindow();
  TerminalWidget* term;
  void addstuff();
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
  for (i=0;i<100;i++) {
    term->buffer()->append("Hello there people!\n");
    sprintf(buffer,"Line %d of %d of text here...\n",i,100);
    term->buffer()->append(buffer);
  }
  redraw();
}

int main(int argc, char **argv) {
  TerminalWindow* window = new TerminalWindow(600,400,"Terminal");
  window->show(1,argv);
  window->addstuff();
  return Fl::run();
}
