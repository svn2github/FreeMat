#ifndef __WinTerminal_hpp__
#define __WinTerminal_hpp__

#include <windows.h>
#include "TermWidget.hpp"

class WinTerminal : public TermWidget {
  HWND hwnd;
  HFONT hfnt;
  SCROLLINFO si;
  CHOOSEFONT cft;
  LOGFONT lfont;
  HBITMAP hBitmp;
  HBRUSH hWhiteBrush, hBlackBrush, hBlueBrush;
  bool bitmap_active;
  HDC hdcMem;
  int m_win_width;
  int m_win_height;
public:
  WinTerminal(HINSTANCE hInstance, int iCmdShow);
  ~WinTerminal() {};
  virtual int GetHeight();
  virtual int GetWidth();
  virtual void InstallEventTimers();
  virtual void ScrollLineUp();
  virtual void ScrollLineDown();
  virtual void SetScrollBarValue(int val);
  virtual void SetupScrollBar(int minval, int maxval, 
			      int step, int page, int val);
  virtual void BeginDraw() {}
  virtual void PutTagChar(int x, int y, tagChar g);
  virtual void EndDraw() {}
  virtual void setFont(int size);
  void OnExpose(int x, int y, int w, int h);
  HWND window() {return hwnd;};
  void OnResize(int w, int h);
  void OnScrollMsg(int v);
};

void SetupWinTerminalClass(HINSTANCE hInstance);

#endif
