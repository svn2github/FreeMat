#ifndef __GraphicsContext_hpp__
#define __GraphicsContext_hpp__

#include "Point2D.hpp"
#include "Rect2D.hpp"
#include "Color.hpp"
#include "GrayscaleImage.hpp"
#include "RGBImage.hpp"

#include <string>
#include <vector>

// The GraphicsContext class is a purely virtual base class for the
// various graphics context types.
class GraphicsContext {
  virtual Point2D GetCanvasSize() = 0;
  virtual Point2D GetTextExtent(std::string label) = 0;
  virtual void DrawText(std::string label, Point2D pos) = 0;
  virtual void DrawRotatedText(std::string label, Point2D pos, OrientationType orient) = 0;
  virtual void SetFont(string fontname, int fontsize) = 0;
  virtual void SetBackGroundColor(Color col);
  virtual void SetForeGroundColor(Color col);
  virtual void SetLineStyle(LineStyleType style) = 0;
  virtual void DrawLine(Point2D pos1, Point2D pos2) = 0;
  virtual void DrawPoint(Point2D pos) = 0;
  virtual void DrawCircle(Point2D pos, int radius) = 0;
  virtual void DrawRectangle(Rect2D rect) = 0;
  virtual void FillRectangle(Rect2D rect) = 0;
  virtual void DrawLines(std::vector<Point2D> pts) = 0;
  virtual void SetClippingRegion(Rect2D rect) = 0;
  virtual void ClearClippingRegion() = 0;
  virtual void BlitGrayscaleImage(Point2D pos, GrayscaleImage img) = 0;
  virtual void BlitRGBImage(Point2D pos, RGBImage img) = 0;
};

#endif
