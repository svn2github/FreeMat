#include "QTGC.hpp"
#include <qimage.h>

QTGC::QTGC(QPainter& qt, int width, int height) : m_qt(qt){
  m_width = width;
  m_height = height;
  m_ls = LINE_SOLID;
  m_ps = Qt::SolidLine;
  m_fg = Color("black");
}

Point2D QTGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

Point2D QTGC::GetTextExtent(std::string label) {
  QRect sze(m_qt.fontMetrics().boundingRect(label));
  return Point2D(sze.width(),sze.height());
}
//FIXME - rotated text never show up?
void QTGC::DrawTextString(std::string txt, Point2D pos, OrientationType orient) {
  switch (orient) {
  case ORIENT_0:
    m_qt.drawText(pos.x,pos.y,txt);
    break;
  case ORIENT_90:
    m_qt.rotate(-90);
    m_qt.drawText(pos.x,pos.y,txt);
    m_qt.rotate(90);
    break;
  case ORIENT_180:
    m_qt.rotate(-180);
    m_qt.drawText(pos.x,pos.y,txt);
    m_qt.rotate(180);
    break;
  case ORIENT_270:
    m_qt.rotate(-270);
    m_qt.drawText(pos.x,pos.y,txt);
    m_qt.rotate(270);
    break;
  }
}

void QTGC::SetFont(int fontsize) {
  m_qt.setFont(QFont("Helvetica",12));
}

Color QTGC::SetBackGroundColor(Color col) {
  Color ret(m_bg);
  m_qt.setBackgroundColor(QColor(col.red,col.green,col.blue));
  m_bg = col;
  return ret;
}

Color QTGC::SetForeGroundColor(Color col) {
  Color ret(m_fg);
  m_qt.setBrush(QColor(col.red,col.green,col.blue));
  m_qt.setPen(QPen(QColor(col.red,col.green,col.blue),0,m_ps));
  m_fg = col;
  return ret;
}

LineStyleType QTGC::SetLineStyle(LineStyleType style) {
  LineStyleType ret(m_ls);
  m_ls = style;
  switch(m_ls) {
  case LINE_SOLID:
    m_ps = Qt::SolidLine;
    break;
  case LINE_DASHED:
    m_ps = Qt::DashLine;
    break;
  case LINE_DOTTED:
    m_ps = Qt::DotLine;
    break;
  case LINE_DASH_DOT:
    m_ps = Qt::DashDotLine;
    break;
  case LINE_NONE:
    m_ps = Qt::NoPen;
    break;
  }
  m_qt.setPen(QPen(QColor(m_fg.red,m_fg.green,m_fg.blue),0,m_ps));
  return ret;
}

void QTGC::DrawLine(Point2D pos1, Point2D pos2) {
  m_qt.drawLine(pos1.x,pos1.y,pos2.x,pos2.y);
}

void QTGC::DrawPoint(Point2D pos1) {
  m_qt.drawPoint(pos1.x,pos1.y);
}

void QTGC::DrawCircle(Point2D pos, int radius) {
  m_qt.setBrush(Qt::NoBrush);
  m_qt.drawEllipse(pos.x-radius,pos.y-radius,2*radius,2*radius);
}

void QTGC::DrawRectangle(Rect2D rect) {
  m_qt.setBrush(Qt::NoBrush);
  m_qt.drawRect(rect.x1,rect.y1,rect.width,rect.height);
}

void QTGC::FillRectangle(Rect2D rect) {
  m_qt.fillRect(rect.x1,rect.y1,rect.width,rect.height,
		QBrush(QColor(m_fg.red,m_fg.green,m_fg.blue)));
}

void QTGC::FillQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  QPointArray a(4);
  a.putPoints(0,4,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,p4.x,p4.y);
  m_qt.drawPolygon(a);
}

void QTGC::DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  QPointArray a(4);
  a.putPoints(0,4,p1.x,p1.y,p2.x,p2.y,p3.x,p3.y,p4.x,p4.y,p1.x,p1.y);
  m_qt.drawPolyline(a);
}

void QTGC::DrawLines(std::vector<Point2D> pts) {
  QPointArray a(pts.size());
  for (int i=0;i<pts.size();i++)
    a.setPoint(i,pts[i].x,pts[i].y);
  m_qt.drawPolyline(a);
}

void QTGC::PushClippingRegion(Rect2D rect) {
  m_clip_stack.push_back(rect);
  m_qt.setClipRect(rect.x1,rect.y1,rect.width,rect.height);
}

Rect2D QTGC::PopClippingRegion() {
  Rect2D ret;
  ret = m_clip_stack.back();
  m_clip_stack.pop_back();
  if (m_clip_stack.size() > 0) {
    Rect2D rect(m_clip_stack.back());
    m_qt.setClipRect(rect.x1,rect.y1,rect.width,rect.height);
  } else {
    m_qt.setClipping(FALSE);
  }
  return ret;
}

void QTGC::BlitImage(unsigned char *data, int width, int height, int x0, int y0) {
  QImage qimg(width, height, 32);
  for (int i=0;i<height;i++) {
    uint *p = (uint*) qimg.scanLine(i);
    for (int j=0;j<width;j++)
      p[j] = qRgb(data[i*width*3 + j*3],data[i*width*3 + j*3 + 1],data[i*width*3 + j*3 + 2]);
  }
  m_qt.drawImage(x0,y0,qimg);
}
