#include "XPWidget.hpp"
#include "XPContainer.hpp"

XPContainer::XPContainer(XPWidget *parent, Rect2D rect) :
  XPWidget(parent, rect) {
  focus = -1;
}

XPContainer::~XPContainer() {
  int i;
  for (i=0;i<children.size();i++)
    delete children[i];
}

void XPContainer::AddChild(XPWidget* child) {
  child->SetParent(this);
  children.push_back(child);
}

void XPContainer::OnDraw(GraphicsContext &gc, Rect2D region) {
  // gc.SetBackGroundColor(Color("light grey"));
  //  gc.SetForeGroundColor(Color("light grey"));
  //  gc.FillRectangle(region);
  int i;
  for (i=0;i<children.size();i++) 
    if (children[i]->GetBoundingRect().TestIntersect(region)) {
      Rect2D bnd(children[i]->GetBoundingRect());
      children[i]->OnDraw(gc,region);
    }
}

void XPContainer::OnChar(char key) {
  if (focus>=0)
    children[focus]->OnChar(key);
}

void XPContainer::OnMouseDown(Point2D pt) {
  int i;
  bool hit;
  i = children.size();
  hit = false;
  focus = -1;
  while ((i>=0) && !hit) {
    i = i - 1;
    if (i<0) continue;
    hit = children[i]->HitTest(pt);
    if (hit) {
      children[i]->OnMouseDown(pt);
      focus = i;
      if (!children[i]->GetFocus())
	children[i]->SetFocus(true);
    } else if (children[i]->GetFocus())
      children[i]->SetFocus(false);
  }
}

void XPContainer::OnMouseUp(Point2D pt) {
  int i;
  bool hit;
  i = children.size();
  hit = false;
  while ((i>=0) && !hit) {
    i = i - 1;
    if (i<0) continue;
    hit = children[i]->HitTest(pt);
    if (hit)
      children[i]->OnMouseUp(pt);
  }
}

void XPContainer::OnMouseDrag(Point2D pt) {
  int i;
  bool hit;
  i = children.size();
  hit = false;
  while ((i>=0) && !hit) {
    i = i - 1;
    if (i<0) continue;
    hit = children[i]->HitTest(pt);
    if (hit)
      children[i]->OnMouseDrag(pt);
  }
}
