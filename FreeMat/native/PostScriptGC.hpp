#ifndef __PostScriptGC_hpp__
#define __PostScriptGC_hpp__

#include <string>
#include "GraphicsContext.hpp"
#include <stdio.h>
#include <stdlib.h>

class PostScriptGC : public GraphicsContext {  
  FILE *fp;
  int m_width;
  int m_height;
  Color m_bg;
  Color m_fg;
  LineStyleType m_lst;
  std::vector<Rect2D> clips;
  std::string m_fontname;
  int m_fontsize;

  Point2D ToPS(Point2D p);
  void DoRect(Rect2D p);
  void RefreshGS();
public:
  PostScriptGC(std::string filename, int width, int height);
  virtual ~PostScriptGC();

  virtual Point2D GetCanvasSize();
  virtual Point2D GetTextExtent(std::string label);
  virtual void DrawText(std::string label, Point2D pos);
  virtual void DrawRotatedText(std::string label, Point2D pos, OrientationType orient);
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
