#include "XPWidget.hpp"
#include <algorithm>
#include <ctype.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qprinter.h>
#include "QTGC.hpp"
#include <iostream>
#include <qapplication.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <qeventloop.h>

#ifndef QT3
#include <QMouseEvent>
#include <QImageWriter>
#endif

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
  click_x = e->x();
  click_y = e->y();
}

void XPWidget::mouseReleaseEvent(QMouseEvent* e) {
  m_mousepressed = false;
  OnMouseUp(Point2D(e->x(),e->y()));
  click_mode = false;
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

QPixmap *cpxmap = NULL;

void XPWidget::Copy() {
  if (cpxmap)
    delete cpxmap;
  cpxmap = new QPixmap(width(),height());
  QPainter paint(cpxmap);
  QTGC gc(paint,width(),height());
  OnDraw(gc);
  QClipboard *cb = QApplication::clipboard();
  cb->setPixmap(*cpxmap);
}



// To add PS capability to win32...  how can this work?  The problem
// is that without FreeType, you can't get font metric information...
// Perhaps its best to drop PS/EPS output on Win/Mac?  Anyway, the
// way the PS/EPS output works is by creating a QPrinter object.
//
// Now QPrinter <-- QPaintDevice,  so is this enough?

bool XPWidget::Print(std::string filename, std::string type) {
  if (type == "EPS" || type == "PS") {
    QPrinter mprnt;
#ifdef QT3
    mprnt.setOutputToFile(TRUE);
    mprnt.setOutputFileName(filename);
#else
    mprnt.setOutputFileName(filename.c_str());
#endif
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
#ifdef QT3
    QImage img(pxmap.convertToImage());
#else
    QImage img(pxmap.toImage());
#endif
    return img.save(filename.c_str(),type.c_str());
  }
}

void XPWidget::GetClick(int &x, int &y) {
  // Set the cross cursor
#ifdef QT3
  setCursor(QCursor(QCursor::CrossCursor));
#else
  setCursor(QCursor(Qt::CrossCursor));
#endif
  click_mode = true;
  while (click_mode)
#ifdef QT3
    qApp->eventLoop()->processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMore);
#else
    qApp->processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
#endif
  x = click_x;
  y = click_y;
#ifdef QT3
  setCursor(QCursor(Qt::ArrowCursor));
#else
  setCursor(QCursor(Qt::ArrowCursor));
#endif
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
#ifdef QT3
  QStrList formats(QImage::outputFormats());
  for (int i=0;i<formats.count();i++)
    if (formats.at(i) == ext) return ext;
#else
  QList<QByteArray> formats(QImageWriter::supportedImageFormats());
  for (int i=0;i<formats.count();i++)
    if (formats.at(i).data() == ext) return ext;
#endif
  return std::string();
}

std::string FormatListAsString() {
  std::string ret_text = "Supported Formats: ";
#ifdef QT3
  QStrList formats(QImage::outputFormats());
  for (int i=0;i<formats.count();i++)
    ret_text = ret_text + formats.at(i) + " ";
#else
  QList<QByteArray> formats(QImageWriter::supportedImageFormats());
  for (int i=0;i<formats.count();i++)
    ret_text = ret_text + formats.at(i).data() + " ";
#endif
  return ret_text;
}
