#include <windows.h>
#include "XWindow.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  XWindow *ptr;

  InitializeXWindowSystem(hInstance);
  ptr = new XWindow;
  ptr->SetTitle("Figure 1");
  ptr = new XWindow;
  ptr->SetTitle("Image 1");
  Run();
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
