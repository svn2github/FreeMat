#ifndef __DrawEngine_hpp__
#define __DrawEngine_hpp__

#include <qpen.h>
#include <qfontmetrics.h>
#include <qpoint.h>
#include <qbrush.h>
#include <qfont.h>
#include <qimage.h>
#include <qstring.h>
#include <vector>

class DrawEngine {
 public:
  virtual void setPen(const QPen &pen) = 0;
  virtual void setPen(const QColor &color) = 0;
  virtual void setPen(Qt::PenStyle) = 0;
  virtual void save() = 0;
  virtual void translate(float dx, float dy) = 0;
  virtual void restore() = 0;
  virtual QFontMetrics fontMetrics() = 0;
  virtual void rotate(float a) = 0;
  virtual void drawLine(int x1, int y1, int x2, int y2) = 0;
  virtual void setBrush(const QBrush &brush) = 0;
  virtual void drawRect(int x1, int y1, int w, int h) = 0;
  virtual void fillRect(int x, int y, int w, int h, const QBrush &) = 0;
  virtual QPoint xForm(const QPoint &) = 0;
  virtual void setClipRect(int x, int y, int w, int h) = 0;
  virtual void drawPoint(int x, int y) = 0;
  virtual void drawEllipse(int x, int y, int w, int h) = 0;
  virtual void drawPolyline(const std::vector<QPoint> &pa) = 0;
  virtual void setFont(const QFont &f) = 0;
  virtual void drawText(int x, int y, const QString &s) = 0;
  virtual void drawImage(int x, int y, const QImage &image) = 0;
};

#endif
