#ifndef __XPGC_hpp__
#define __XPGC_hpp__

#include <string>
#include "GraphicsContext.hpp"
#include <stdio.h>
#include <stdlib.h>

class XPGC : public GraphicsContext {  
  int m_width;
  int m_height;
  Color m_bg;
  Color m_fg;
  LineStyleType m_ls;
  std::vector<Rect2D> clips;
  int m_size;
public:
  XPGC(int width, int height);
  virtual ~XPGC();
  virtual Point2D GetCanvasSize();
  virtual Point2D GetTextExtent(std::string label);
  virtual void DrawTextString(std::string label, Point2D pos, OrientationType orient);
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
