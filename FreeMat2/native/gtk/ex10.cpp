#include <X11/Xlib.h>
#include <unistd.h>
#include <string>

#define Exception(x) (x)

main() {
  Display *d = XOpenDisplay(0);
  if (!d) throw Exception(std::string("Unable to open display"));
  Window w = XCreateWindow(d, DefaultRootWindow(d), 0, 0, 200,
			   100, 0, CopyFromParent, CopyFromParent,
			   CopyFromParent, 0, 0);
  XMapWindow(d, w);
  XFlush(d);
  sleep(10);
  return 0;
}
