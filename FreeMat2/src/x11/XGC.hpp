#ifndef __XGC_hpp__
#define __XGC_hpp__

#include "GraphicsContext.hpp"
#include <X11/Xlib.h>

class XGC : public GraphicsContext {
  Display *m_display;
  Drawable drawable;
  GC m_gc;
  int m_width, m_height;
  Color bg, fg;
  LineStyleType ls;
  int current_fontsize;
  XFontStruct *font_info;
  std::vector<Rect2D> clipstack;
 public:
  XGC(Display*, Drawable, GC, int width, int height);
  ~XGC();
  virtual Point2D GetCanvasSize();
  virtual Point2D GetTextExtent(std::string label);
  virtual void DrawTextString(std::string label, Point2D pos, OrientationType orient = ORIENT_0);
  virtual void SetFont(int fontsize);
  virtual Color SetBackGroundColor(Color col);
  virtual Color SetForeGroundColor(Color col);
  virtual LineStyleType SetLineStyle(LineStyleType style);
  virtual void DrawLine(Point2D pos1, Point2D pos2);
  virtual void DrawPoint(Point2D pos);
  virtual void DrawCircle(Point2D pos, int radius);
  virtual void DrawRectangle(Rect2D rect);
  virtual void FillRectangle(Rect2D rect);
  virtual void DrawLines(std::vector<Point2D> pts);
  virtual void PushClippingRegion(Rect2D rect);
  virtual Rect2D PopClippingRegion();
 private:
};
#endif
