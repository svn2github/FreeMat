#ifndef __OSXTerminal_hpp__
#define __OSXTerminal_hpp__

#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>
#include <string>
#include <iostream>

#include "../win32/TermWidget.hpp"

class OSXTerminal : public TermWidget {
  WindowRef win;
  ControlHandle vScrollBar;
  int m_client_width, m_client_height, m_scrollval;
  bool m_scrollinit;
  CGContextRef gh;
 public:
  OSXTerminal(int width, int height, std::string title);
  ~OSXTerminal();
  WindowRef Win();
  virtual int GetHeight();
  virtual int GetWidth();
  virtual void InstallEventTimers();
  virtual void ScrollLineUp();
  virtual void ScrollLineDown();
  virtual void SetScrollBarValue(int val);
  virtual void SetupScrollBar(int minval, int maxval, 
			      int step, int page, int val);
  virtual void BeginDraw();
  virtual void PutTagChar(int x, int y, tagChar g);
  virtual void EndDraw();
  virtual void setFont(int size);
  void OnResize(int w, int h);
  void Erase();
  void ScrollPageUp();
  void ScrollPageDown();
  void CreateVerticalScrollBar(int scrollmin, int scrollmax, 
			       int scrollline, int scrollpage, 
			       int scrollval);
};

#endif
