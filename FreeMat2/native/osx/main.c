#include <Carbon/Carbon.h>

int main(int argc, char* argv[])
{
  WindowRef win;
  Rect winSize;
  
  winSize.top = 50;
  winSize.left = 50;
  winSize.bottom = 400;
  winSize.right = 400;

  CreateNewWindow(kDocumentWindowClass,
		  kWindowStandardDocumentAttributes,
		  &winSize,
		  &win);
  ShowWindow(win);
  RunApplicationEventLoop();
}
