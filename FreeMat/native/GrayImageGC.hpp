#ifndef __GrayImageGC_hpp__
#define __GrayImageGC_hpp__

class GrayImageDC : public GraphicsContext {  
  Font cFont;
  
 public:
  GrayImageDC(GrayScaleImage &img);
  GrayImageDC();

  virtual Point2D GetCanvasSize();
  virtual Point2D GetTextExtent(std::string label);
  virtual void DrawText(std::string label, Point2D pos);
  virtual void DrawRotatedText(std::string label, Point2D pos, OrientationType orient);
  virtual void SetFont(string fontname, int fontsize);
  virtual void SetColor(string colorName);
  virtual void SetColor(int red, int green, int blue);
  virtual void SetLineStyle(LineStyleType style);
  virtual void DrawLine(Point2D pos1, Point2D pos2);
  virtual void DrawPoint(Point2D pos);
  virtual void DrawCircle(Point2D pos, int radius);
  virtual void DrawRectangle(Rect2D rect);
  virtual void FillRectangle(Rect2D rect);
  virtual void DrawLines(std::vector<Point2D> pts);
  virtual void SetClippingRegion(Rect2D rect);
  virtual void ClearClippingRegion();
  virtual void BlitGrayscaleImage(Point2D pos, GrayscaleImage img);
  virtual void BlitRGBImage(Point2D pos, RGBImage img);
};

#endif
