#include "XPWidget.hpp"
#include "XPContainer.hpp"

void XPContainer::AddChild(XPWidget* child) {
  child->SetParent(this);
  children.push_back(child);
}

void XPContainer::OnDraw(GraphicsContext &gc) {
  gc.SetBackGroundColor(Color("light grey"));
  gc.SetForeGroundColor(Color("light grey"));
  gc.FillRectangle(Rect2D(0, 0, m_width, m_height));
  int i;
  for (i=0;i<children.size();i++)
    children[i]->OnDraw(gc);
}

void XPContainer::OnMouseDown(int x, int y) {
  int i;
  bool hit;
  XPWidget *w;
  i = children.size();
  hit = false;
  while ((i>=0) && !hit) {
    i = i - 1;
    if (i<0) continue;
    w = children[i];
    hit = (x>w->x0) && (x<(w->x0+w->m_width)) &&
      (y>w->y0) && (y<(w->y0+w->m_height));
    if (hit)
      w->OnMouseDown(x,y);
  }
}

void XPContainer::OnMouseUp(int x, int y) {
  int i;
  bool hit;
  XPWidget *w;
  i = children.size();
  hit = false;
  while ((i>=0) && !hit) {
    i = i - 1;
    if (i<0) continue;
    w = children[i];
    hit = (x>w->x0) && (x<(w->x0+w->m_width)) &&
      (y>w->y0) && (y<(w->y0+w->m_height));
    if (hit)
      w->OnMouseUp(x,y);
  }
}
