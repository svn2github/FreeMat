#ifndef __RGBImageGC_hpp__
#define __RGBImageGC_hpp__

#include "GraphicsContext.hpp"

class RGBImageGC : public GraphicsContext {  
  Font c_font;
  Color c_fg;
  Color c_bg;
  int c_linestyle;
  RGBImage& canvas;
  std::vector<Rect2D> clipregions;
 public:
  // Methods specific to RGBImageGC
  RGBImageGC(RGBImage& surface);
  ~RGBImageGC();
  void WritePPM(std::string filename);
  // Methods for generic GCs
  virtual Point2D GetCanvasSize();
  virtual Point2D GetTextExtent(std::string label);
  virtual void DrawText(std::string label, Point2D pos);
  virtual void DrawRotatedText(std::string label, Point2D pos, OrientationType orient);
  virtual void SetFont(string fontname, int fontsize);
  virtual void SetBackGroundColor(Color col);
  virtual void SetForeGroundColor(Color col);
  virtual void SetLineStyle(LineStyleType style);
  virtual void DrawLine(Point2D pos1, Point2D pos2);
  virtual void DrawPoint(Point2D pos);
  virtual void DrawCircle(Point2D pos, int radius);
  virtual void DrawRectangle(Rect2D rect);
  virtual void FillRectangle(Rect2D rect);
  virtual void DrawLines(std::vector<Point2D> pts);
  virtual void PushClippingRegion(Rect2D rect);
  virtual Rect2D PopClippingRegion();
  virtual void BlitGrayscaleImage(Point2D pos, GrayscaleImage img);
  virtual void BlitRGBImage(Point2D pos, RGBImage img);
};

#endif
