#ifndef __RGBImageGC_hpp__
#define __RGBImageGC_hpp__

#include "GraphicsContext.hpp"
#include "BitmapFont.hpp"
#include <string>

class RGBImageGC : public GraphicsContext {  
  Color c_fg;
  Color c_bg;
  LineStyleType c_linestyle;
  RGBImage& canvas;
  std::vector<Rect2D> clipregions;
  int penstate;
  BitmapFont c_font;

  bool InsideClippingRegion(int x, int y);
  void SetClippedPixel(int, int, Color);
  void BlendPixel(int, int, int);
  bool PenDraws();
  void DrawAALineStyle(int X0, int Y0, int X1, int Y1, bool endPt);
  void Plot8CirclePoints(int cx, int cy, int x, int y, int c);
public:
  // Methods specific to RGBImageGC
  RGBImageGC(RGBImage& surface);
  virtual ~RGBImageGC();
  // Methods for generic GCs
  virtual Point2D GetCanvasSize();
  Point2D GetTextExtent(std::string label);
  virtual void DrawText(std::string label, Point2D pos, OrientationType orient = ORIENT_0);
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
};

#endif
