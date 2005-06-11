#include <qapplication.h>
#include <iostream>
#include "XPWidget.hpp"
#include "XPWindow.hpp"

bool switchme;

class testwidget : public XPWidget {
public:
  testwidget(XPWidget *parent, int w, int h) : XPWidget(parent, Point2D(w, h)) {};
  void OnMouseUp(Point2D pt) { switchme = true; }
  void OnDraw(GraphicsContext &gc) {
    gc.SetForeGroundColor(Color(255,0,0));
    gc.DrawLine(Point2D(0,0),Point2D(GetWidth(),GetHeight()));
    gc.DrawLine(Point2D(GetWidth(),0),Point2D(0,GetHeight()));
    gc.DrawCircle(Point2D(GetWidth()/2,GetHeight()/2),10);
  }
};

class testwidget2 : public XPWidget {
public:
  testwidget2(XPWidget *parent, int w, int h) : XPWidget(parent, Point2D(w, h)) {};
  void OnMouseUp(Point2D pt) { switchme = true; }
  void OnDraw(GraphicsContext &gc) {
    gc.SetForeGroundColor(Color(0,255,0));
    gc.DrawLine(Point2D(0,0),Point2D(GetWidth(),GetHeight()));
    gc.DrawLine(Point2D(GetWidth(),0),Point2D(0,GetHeight()));
    gc.DrawCircle(Point2D(GetWidth()/2,GetHeight()/2),10);
    gc.FillRectangle(Rect2D(0,0,20,30));
  }
};

int main(int argc, char* argv[]) {
  QApplication myapp(argc, argv);
  XPWindow *main = new XPWindow(400,500);
  main->Title("Figure 1");
  testwidget *roo = new testwidget(NULL,300,300);
  testwidget2 *goo = new testwidget2(NULL,300,300);
  main->AddWidget(roo);
  main->Show();
  switchme = false;
  while (!switchme) 
    myapp.processEvents();
  std::cout << "switch\n";
  main->AddWidget(goo);
  switchme = false;
  while (!switchme) 
    myapp.processEvents();
  std::cout << "switch\n";
  main->AddWidget(roo);
  switchme = false;
  while (!switchme) 
    myapp.processEvents();
  roo->Print("test.eps","eps");
  return(0);
}
