#include <X11/Xlib.h>
#include <unistd.h>
#include "XWindow.hpp"
#include "Terminal.hpp"
#include <stdio.h>
#include <stdlib.h>

Display *d;
int screen_num = 0;

#define MAX_COLORS 3
/* This is just so we can print the visual class intelligibly */
static char *visual_class[] = {
   "StaticGray",
   "GrayScale",
   "StaticColor",
   "PseudoColor",
   "TrueColor",
   "DirectColor"
};

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

unsigned char rgbbuf1[IMAGE_WIDTH * IMAGE_HEIGHT * 3];
unsigned char rgbbuf2[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

class gradientWindow : public XWindow {
 public:
  gradientWindow(Display *d) : XWindow(d) {};
  ~gradientWindow() {};
  void UpdateContents(unsigned char * dat, int width, int height) {
    int x, y;
    for (y=0;y<height;y++) 
      for (x=0;x<width;x++) {
	*dat++ = 235.0*x/width;
	*dat++ = 255.0*x*y/(width*height);
	*dat++ = 235.0*y/height;
      }
  }
};

class gradientWindow2 : public XWindow {
 public:
  gradientWindow2(Display *d) : XWindow(d) {};
  ~gradientWindow2() {};
  void UpdateContents(unsigned char * dat, int width, int height) {
    int x, y;
    for (y=0;y<height;y++) 
      for (x=0;x<width;x++) {
	*dat++ = 235.0*x*y/(width*height);
	*dat++ = 255.0*y/height;
	*dat++ = 235.0*x/width;
      }
  }
};

Terminal term;

void stdincb() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1) {
    term.ProcessChar(c);
  }
}


int main(int argc, char *argv[]) {
  d = XOpenDisplay(0);
  if (!d) {
    fprintf(stderr,"Error - unable to open X display\n");
    exit(1);
  }
  gradientWindow *t = new gradientWindow(d);
  t->Show();
  gradientWindow2 *z= new gradientWindow2(d);
  z->Show();
  int x, y; 
  int x2, y2;
  z->SetTitle("Blue Green Window");
  t->SetTitle("Red Blue Window");
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  RegisterSTDINCallback(stdincb);
  std::vector<std::string> fnames;
  fnames.push_back("ls");
  fnames.push_back("list");
  fnames.push_back("anotherfunc");
  fnames.push_back("coolio");
  fnames.push_back("sin");
  fnames.push_back("cos");
  term.SetFunctionList(fnames);
  term.ReplacePrompt("--> ");
  term.DisplayPrompt();
  fflush(stdout);
  z->GetClick(x,y);
  t->GetBox(x,y,x2,y2);
  printf("%d %d %d %d\n",x,y,x2,y2);
  fflush(stdout);
  Run(d);
}
