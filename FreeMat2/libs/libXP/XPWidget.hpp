#ifndef __XPWidget_hpp__
#define __XPWidget_hpp__

#include "GraphicsContext.hpp"

class XPWidget {
public:
  int x0, y0;
  int m_width;
  int m_height;
  XPWidget *parent;
  virtual void OnMouseDown(int x, int y) {};
  virtual void OnMouseUp(int x, int y) {};
  virtual void OnDrag(int x, int y) {};
  virtual void OnResize(int w, int h) {m_width = w; m_height = h;}; 
  virtual void OnDraw(GraphicsContext &gc) {};
  virtual int getWidth() {return m_width;};
  virtual int getHeight() {return m_height;};
  virtual void Refresh() {parent->Refresh();};
  virtual XPWidget* GetParent() {return parent;};
  virtual void SetParent(XPWidget* p) {parent = p;};
  virtual void SetX0(int x) {x0 = x;};
  virtual void SetY0(int y) {y0 = y;};
};

#endif
