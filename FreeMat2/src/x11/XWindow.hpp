#ifndef __XWindow_hpp__
#define __XWindow_hpp__

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <string>
#include "GraphicsContext.hpp"
#include "XPWidget.hpp"

class XWindow : public XPWidget {
 public:
  XWindow(Rect2D sze);
  virtual ~XWindow();
  Window getWindow() {return m_window;}
  void Raise();
  void Show();
  void Hide();
  void Close();
  void SetTitle(std::string title);
  void SetChildWidget(XPWidget* child) {m_child = child;};
  XPWidget* GetChildWidget() {return m_child;};
  virtual void OnMouseDown(Point2D pt);
  virtual void OnMouseUp(Point2D pt);
  virtual void OnMouseDrag(Point2D pt);
  virtual void OnResize(Point2D pt);
  virtual void OnChar(char key);
  virtual void OnDraw(GraphicsContext& gc, Rect2D rect);
  virtual void OnExpose(int x1, int y1, int width, int height);
  virtual void Refresh(Rect2D rect);
 private:
  Window m_window;
  Display *m_display;
  Visual *m_visual;
  GC m_gc;
  XTextProperty m_window_title;
  XPWidget* m_child;
};

typedef void (*fdcallback) (void);
void CloseXWindow(XWindow*);
void RegisterSTDINCallback(fdcallback cb);
void SetActiveDisplay(Display* d);
void DoEvents();
void Run();
void FlushWindowEvents();
#endif
