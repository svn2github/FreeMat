#ifndef __XPWidget_hpp__
#define __XPWidget_hpp__

class XPWidget {
protected:
  int x0, y0;
  int m_width;
  int m_height;
public:
  virtual void OnMouseDown(int x, int y) {};
  virtual void OnMouseUp(int x, int y) {};
  virtual void OnDrag(int x, int y) {};
  virtual void OnResize(int w, int h) {}; 
  virtual void OnDraw(GraphicsContext &gc) {};
  virtual int getWidth() {return m_width;};
  virtual int getHeight() {return m_height;};
};

#endif
