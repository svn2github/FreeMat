#ifndef __GraphicsContext_hpp__
#define __GraphicsContext_hpp__

#include "Point2D.hpp"
#include "Rect2D.hpp"
#include "Color.hpp"
#include "GrayscaleImage.hpp"
#include "RGBImage.hpp"

#include <string>
#include <vector>

typedef enum {
  LINE_SOLID,
  LINE_DASHED,
  LINE_DOTTED,
  LINE_DASH_DOT,
  LINE_NONE
} LineStyleType;

typedef enum {
  ORIENT_0,
  ORIENT_90,
  ORIENT_180,
  ORIENT_270
} OrientationType;

// The GraphicsContext class is a purely virtual base class for the
// various graphics context types.
class GraphicsContext {
public:
  virtual Point2D GetCanvasSize() {};
  virtual Point2D GetTextExtent(std::string label) {};
  virtual void DrawTextString(std::string label, Point2D pos, OrientationType orient = ORIENT_0) {};
  virtual void SetFont(int fontsize) {};
  virtual Color SetBackGroundColor(Color col) {};
  virtual Color SetForeGroundColor(Color col) {};
  virtual LineStyleType SetLineStyle(LineStyleType style) {};
  virtual void DrawLine(Point2D pos1, Point2D pos2) {};
  virtual void DrawPoint(Point2D pos) {};
  virtual void DrawCircle(Point2D pos, int radius) {};
  virtual void DrawRectangle(Rect2D rect) {};
  virtual void FillRectangle(Rect2D rect) {};
  virtual void DrawLines(std::vector<Point2D> pts) {};
  virtual void PushClippingRegion(Rect2D rect) {};
  virtual Rect2D PopClippingRegion() {};
  virtual void BlitImage(unsigned char *data, int width, int height, int x0, int y0) {};
};

#endif
