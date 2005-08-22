#include "DrawEngine.hpp"
#include <qtextstream.h>
#include <qfile.h>
#include <qstring.h>
#include <qfont.h>

class PSDrawEngine : public DrawEngine {
  QFile *outDevice;
  QTextStream pageStream;
  unsigned m_width;
  unsigned m_height;
  QString m_title;
  QFont m_font;
  void emitHeader();
 public:
  PSDrawEngine(std::string filename, int width, int height);
  virtual ~PSDrawEngine();
  virtual void setPen(const QPen &pen);
  virtual void setPen(const QColor &color);
  virtual void setPen(Qt::PenStyle);
  virtual void save();
  virtual void translate(float dx, float dy);
  virtual void restore();
  virtual QFontMetrics fontMetrics();
  virtual void rotate(float a);
  virtual void drawLine(int x1, int y1, int x2, int y2);
  virtual void setBrush(const QBrush &brush);
  virtual void drawRect(int x1, int y1, int w, int h);
  virtual void fillRect(int x, int y, int w, int h, const QBrush &);
  virtual QPoint xForm(const QPoint &);
  virtual void setClipRect(int x, int y, int w, int h);
  virtual void drawPoint(int x, int y);
  virtual void drawEllipse(int x, int y, int w, int h);
  virtual void drawPolyline(const std::vector<QPoint> &pa);
  virtual void setFont(const QFont &f);
  virtual void drawText(int x, int y, const QString &s);
  virtual void drawImage(int x, int y, const QImage &image);  
};
