#ifndef __XWindow_hpp__
#define __XWindow_hpp__

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <string>
#include "GraphicsContext.hpp"

class XWindow {
 public:
  XWindow();
  virtual ~XWindow();
  Window getWindow() {return m_window;}
  int getWidth() {return m_width;}
  int getHeight() {return m_height;}
  void Raise();
  void Show();
  void Hide();
  void Close();
  void SetImagePseudoColor(unsigned char *data, int width, int height);
  void SetImage(unsigned char *data, int width, int height);
  void OnExpose(int x, int y, int w, int h);
  void OnMouseDown(int x, int y);
  void OnMouseUp(int x, int y);
  void OnDrag(int x, int y);
  void OnResize(int w, int h);
  virtual void OnSize() {};
  virtual void OnClose();
  virtual void OnDraw(GraphicsContext &gc) = 0;
  void Print(std::string filename);
  void SetTitle(std::string title);
  void GetClick(int &x, int &y);
  void GetBox(int &x1, int &y1, int &x2, int &y2);
  void EraseRectangle(int cx, int cy, int cwidth, int cheight);
  void SetSize(int w, int h);
  int GetState();
  void Refresh();
  void UpdateContents(unsigned char *data, int width, int height);
 private:
  Window m_window;
  Display *m_display;
  Visual *m_visual;
  Colormap m_cmap;
  Pixmap m_pixmap;
  GC m_gc;
  XTextProperty m_window_title;
  int m_width;
  int m_height;
  int m_state;
  int m_clickx, m_clicky;
  int m_box_x1, m_box_x2, m_box_y1, m_box_y2;
};

typedef void (*fdcallback) (void);
void RegisterSTDINCallback(fdcallback cb);
void SetActiveDisplay(Display* d);
void DoEvents();
void Run();
#endif
