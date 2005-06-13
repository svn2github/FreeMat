#ifndef __XPWidget_hpp__
#define __XPWidget_hpp__

#include "GraphicsContext.hpp"
#include <qwidget.h>
#include <iostream>

// In its phoenix-like reincarnation, xpwidget is the base class that
// represents a generic (cross platform) widget - it is a subregion
// of another widget that can draw itself, receive mouse clicks, etc.
// 
// <soap>
// If/when libXP is ported to a new platform/toolkit, get XPWidget
// to work first (along with XPGC).  Then XPWindow.
// </soap>
class XPWidget : public QWidget {

  Q_OBJECT

  XPWidget* m_parent;
  bool m_mousepressed;
  Point2D m_size;
public:
  // These are the interface methods to libXP - they should be toolkit neutral
  XPWidget(XPWidget *parent, Point2D sze);
  XPWidget();
  virtual ~XPWidget() {};
  virtual void OnMouseDown(Point2D pt) {};
  virtual void OnMouseDrag(Point2D pt) {};
  virtual void OnMouseUp(Point2D pt) {};
  virtual void OnResize() {};
  virtual void OnDraw(GraphicsContext &gc) {};
  int GetWidth();
  int GetHeight();
  void Redraw();
  XPWidget* GetParent();
  void GetClick(int &x, int &y);
  void Resize(Point2D pt);
  void Hide();
  void Show();
  bool Print(std::string filename,std::string type);
  // These are toolkit dependant routines that map the toolkit stuff to the above events
private:
  void paintEvent(QPaintEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void resizeEvent(QResizeEvent* e);
  QSize sizeHint();
};

void SaveFocus();
void RestoreFocus();

std::string NormalizeImageExtension(std::string ext);
std::string FormatListAsString();
#endif
