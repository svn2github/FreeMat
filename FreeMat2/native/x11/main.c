#include <X11/Xlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  Display *d = XOpenDisplay(0);
  if (d) {
    Window w = XCreateWindow(d, DefaultRootWindow(d), 0, 0, 200,
			     100, 0, CopyFromParent, CopyFromParent,
			     CopyFromParent, 0, 0);
    XMapWindow(d, w);
    XFlush(d);
    sleep(10);
  } else {
    printf("Oy!\n");
  }
  return 0;
}
