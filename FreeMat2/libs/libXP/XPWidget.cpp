#include "XPWidget.hpp"
#include <algorithm>
#include <ctype.h>
#include "PostScriptGC.hpp"
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qprinter.h>
#include "QTGC.hpp"
#include <iostream>
#include <qapplication.h>

XPWidget::XPWidget(XPWidget *parent, Point2D size) 
  : QWidget(parent), m_size(size) { 
}

XPWidget::XPWidget() : QWidget(NULL), m_size(Point2D(1,1)) {
}

void XPWidget::paintEvent(QPaintEvent* e) {
  QWidget::paintEvent(e);

  QPainter paint(this);
  QTGC gc(paint,width(),height());
  OnDraw(gc);
}

void XPWidget::mousePressEvent(QMouseEvent* e) {
  m_mousepressed = true;
  OnMouseDown(Point2D(e->x(),e->y()));
}

void XPWidget::mouseReleaseEvent(QMouseEvent* e) {
  m_mousepressed = false;
  OnMouseUp(Point2D(e->x(),e->y()));
}

void XPWidget::mouseMoveEvent(QMouseEvent* e) {
  if (m_mousepressed)
    OnMouseDrag(Point2D(e->x(),e->y()));
}

int XPWidget::GetWidth() {
  return width();
}

int XPWidget::GetHeight() {
  return height();
}

void XPWidget::Hide() {
  hide();
}

void XPWidget::Show() {
  show();
}

void XPWidget::Resize(Point2D pt) {
  resize(pt.x,pt.y);
}

bool XPWidget::Print(std::string filename, std::string type) {
  if (type == "EPS" || type == "PS") {
    QPrinter mprnt;
    mprnt.setOutputToFile(TRUE);
    mprnt.setOutputFileName(filename);
    mprnt.setColorMode(QPrinter::Color);
    QPainter paint(&mprnt);
    paint.setClipRect(0,0,width(),height());
    QTGC gc(paint,width(),height());
    OnDraw(gc);
    return true;
  } else {
    QPixmap pxmap(width(),height());
    QPainter paint(&pxmap);
    QTGC gc(paint,width(),height());
    OnDraw(gc);
    QImage img(pxmap.convertToImage());
    return img.save(filename,type.c_str());
  }
}

void XPWidget::GetClick(int &x, int &y) {
  //FIXME
}

XPWidget* XPWidget::GetParent() {
  return m_parent;
}

void XPWidget::Redraw() {
  repaint();
}

void XPWidget::resizeEvent(QResizeEvent* e) {
  m_size = Point2D(width(),height());
  OnResize();
}


QSize XPWidget::sizeHint() {
  return QSize(m_size.x,m_size.y);
}

QWidget *save;

void SaveFocus() {
  save = qApp->focusWidget();
}

void RestoreFocus() {
  if (save)
    save->setFocus();
}

std::string NormalizeImageExtension(std::string ext) {
  std::transform(ext.begin(),ext.end(),ext.begin(),toupper);
  if (ext == "JPG") return std::string("JPEG");
  if ((ext == "PS") || (ext == "EPS")) return ext;
  QStrList formats(QImage::outputFormats());
  for (int i=0;i<formats.count();i++)
    if (formats.at(i) == ext) return ext;
  return std::string();
}

std::string FormatListAsString() {
  std::string ret_text = "Supported Formats: ";
  QStrList formats(QImage::outputFormats());
  for (int i=0;i<formats.count();i++)
    ret_text = ret_text + formats.at(i) + " ";
  return ret_text;
}
