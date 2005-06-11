#include "GraphicsContext.hpp"
#include <qpainter.h>
#include <qpen.h>

// Wrapper class that makes a GraphicsContext interface
// for a QTPainter class
class QTGC : public GraphicsContext {
  int m_width, m_height;
  QPainter& m_qt;
  Color m_bg;
  Color m_fg;
  LineStyleType m_ls;
  Qt::PenStyle m_ps;
  std::vector<Rect2D> m_clip_stack;
public:
  QTGC(QPainter& qt, int width, int height);
  virtual ~QTGC() {};
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
