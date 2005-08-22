#include "QPainterDrawEngine.hpp"

QPainterDrawEngine::QPainterDrawEngine(QPainter& gc) : m_gc(gc) {
}

void QPainterDrawEngine::setPen(const QPen &pen) {
  m_gc.setPen(pen);
}

void QPainterDrawEngine::setPen(const QColor &color) {
  m_gc.setPen(color);
}

void QPainterDrawEngine::setPen(Qt::PenStyle ps) {
  m_gc.setPen(ps);
}

void QPainterDrawEngine::save() {
  m_gc.save();
}

void QPainterDrawEngine::translate(float dx, float dy) {
  m_gc.translate(dx, dy);
}

void QPainterDrawEngine::restore() {
  m_gc.restore();
}

QFontMetrics QPainterDrawEngine::fontMetrics() {
  return m_gc.fontMetrics();
}

void QPainterDrawEngine::rotate(float a) {
  m_gc.rotate(a);
}

void QPainterDrawEngine::drawLine(int x1, int y1, int x2, int y2) {
  m_gc.drawLine(x1,y1,x2,y2);
}

void QPainterDrawEngine::setBrush(const QBrush &brush) {
  m_gc.setBrush(brush);
}

void QPainterDrawEngine::drawRect(int x1, int y1, int w, int h) {
  m_gc.drawRect(x1,y1,w,h);
}

void QPainterDrawEngine::fillRect(int x, int y, int w, int h, const QBrush &b) {
  m_gc.fillRect(x,y,w,h,b);
}

QPoint QPainterDrawEngine::xForm(const QPoint &p) {
  return m_gc.xForm(p);
}

void QPainterDrawEngine::setClipRect(int x, int y, int w, int h) {
  m_gc.setClipRect(x,y,w,h);
}

void QPainterDrawEngine::drawPoint(int x, int y) {
  m_gc.drawPoint(x,y);
}

void QPainterDrawEngine::drawEllipse(int x, int y, int w, int h) {
  m_gc.drawEllipse(x,y,w,h);
}

void QPainterDrawEngine::drawPolyline(const std::vector<QPoint> &pts) {
#ifdef QT3
  QPointArray a(pts.size());
  for (int i=0;i<pts.size();i++)
    a.setPoint(i,pts[i].x(),pts[i].y());
#else
  QPolygon a;
  for (int i=0;i<pts.size();i++)
    a.push_back(pts[i]);  
#endif
  m_gc.drawPolyline(a);
}

void QPainterDrawEngine::setFont(const QFont &f) {
  m_gc.setFont(f);
}

void QPainterDrawEngine::drawText(int x, int y, const QString &s) {
  m_gc.drawText(x,y,s);
}

void QPainterDrawEngine::drawImage(int x, int y, const QImage &image) {
  m_gc.drawImage(x,y,image);
}


