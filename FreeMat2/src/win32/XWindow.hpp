#ifndef __XWindow_hpp__
#define __XWindow_hpp__

#include <windows.h>
#include <string>
#include "GraphicsContext.hpp"
#include "XPWidget.hpp"

class XWindow : public XPWidget {
 public:
  XWindow(Rect2D sze);
  virtual ~XWindow();
  HWND getWindow() {return m_window;}
  void Raise();
  void Show();
  void Hide();
  void Close();
  void SetTitle(std::string title);
  virtual GraphicsContext GetGC();
  void SetChildWidget(XPWidget* child) {m_child = child;};
  XPWidget* GetChildWidget() {return m_child;};
  virtual void OnMouseDown(Point2D pt);
  virtual void OnMouseUp(Point2D pt);
  virtual void OnMouseDrag(Point2D pt);
  virtual void OnResize(Point2D pt);
  virtual void OnChar(char key);
  virtual void OnDraw(GraphicsContext& gc, Rect2D rect);
  virtual void Refresh(Rect2D rect);
 protected:
  HWND m_window;
  XPWidget* m_child;
};

void CloseXWindow(XWindow*);
void InitializeXWindowSystem(HINSTANCE hInstance);
void FlushWindowEvents();
bool DoEvents();
void Run();
#endif
