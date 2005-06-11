#ifndef __GTKGC_hpp__
#define __GTKGC_hpp__

#include "GraphicsContext.hpp"
#include <gtk/gtk.h>

// The GraphicsContext class is a purely virtual base class for the
// various graphics context types.
class GTKGC : public GraphicsContext {
  GdkGC* gc;
  GdkPixmap* pixmap;
  int m_width, m_height;
  Color m_bg, m_fg;
  LineStyleType m_lst;
  std::vector<Rect2D> clips;
  int m_fontsize;
  PangoFontDescription *myFont;
  PangoLayout *pango;  
public:
  GTKGC(GdkPixmap* pixmp, int width, int height, PangoLayout *pngo);
  ~GTKGC();
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
  virtual void FillQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4);
  virtual void DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4);
  virtual void DrawLines(std::vector<Point2D> pts);
  virtual void PushClippingRegion(Rect2D rect);
  virtual Rect2D PopClippingRegion();
  virtual void BlitImage(unsigned char *data, int width, int height, int x0, int y0);
};

#endif
