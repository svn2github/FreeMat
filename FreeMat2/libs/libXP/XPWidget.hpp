#ifndef __XPWidget_hpp__
#define __XPWidget_hpp__

#include "GraphicsContext.hpp"

/**
 * This class is the basic widget class.  A widget is just an area that
 * responds to events and can draw itself.  There are two mechanisms for
 * triggering a draw of a widget.  If the parent widget requires a child
 * widget to redraw itself, it will call "OnDraw".  If the widget wants
 * to draw itself, it must get a graphics context from its parent, and
 * then call "OnDraw" directly.
 */
class XPWidget {
protected:
  Rect2D bounds;
  XPWidget *m_parent;
  bool focused;
public:
  XPWidget(XPWidget *parent, Rect2D rect);
  XPWidget() {m_parent = NULL;}
  virtual ~XPWidget() {};
  virtual void OnMouseDown(Point2D pt) {};
  virtual void OnMouseDrag(Point2D pt) {};
  virtual void OnMouseUp(Point2D pt) {};
  virtual void OnResize(Point2D pt);
  virtual void OnDraw(GraphicsContext &gc, Rect2D region) {}; 
  virtual void OnChar(char key) {};
  virtual Rect2D GetBoundingRect() {return bounds;};
  virtual int getWidth() {return bounds.width;};
  virtual int getHeight() {return bounds.height;};
  virtual void Refresh(Rect2D region) {m_parent->Refresh(region);};
  virtual XPWidget* GetParent() {return m_parent;};
  virtual GraphicsContext GetGC() {return m_parent->GetGC();};
  virtual bool HitTest(Point2D pt) {return (!bounds.TestOutside(pt));};
  virtual void SetParent(XPWidget *parent) {m_parent = parent;};
  virtual void ShiftWidget(Point2D pt) {bounds.x1 += pt.x; bounds.y1 += pt.y;};
  virtual void SetFocus(bool arg) {focused = arg; Refresh(bounds);};
  virtual bool GetFocus() {return focused;};
};

#endif
