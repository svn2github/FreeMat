#ifndef __WinGC_hpp__
#define __WinGC_hpp__

#include <windows.h>
#include "GraphicsContext.hpp"

class WinGC : public GraphicsContext {
public:
  WinGC(HDC dc, int width, int height);
  ~WinGC();
  virtual Point2D GetCanvasSize();
  virtual Point2D GetTextExtent(std::string label);
  virtual void DrawTextString(std::string label, Point2D pos, OrientationType orient = ORIENT_0);
  virtual void SetFont(std::string fontname, int fontsize);
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
  virtual void BlitGrayscaleImage(Point2D pos, GrayscaleImage &img);
  virtual void BlitRGBImage(Point2D pos, RGBImage &img);
private:
  HDC hdc;
  int m_width;
  int m_height;
  std::string m_fontname;
  int m_fontsize;
  std::vector<Rect2D> clipstack;
  HFONT m_hfont;
  HFONT m_vfont;

  HRGN clipwin;
  Color bgcol, fgcol;
  LineStyleType m_style;
};

#endif
