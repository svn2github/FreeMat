#include "RGBImageGC.hpp"
#include <math.h>

int main() {
  RGBImage canvas(300,300);
  Color ltgray(Color(200,200,200));
  canvas.SetAllPixels(ltgray);
  RGBImageGC gc(canvas);
  gc.SetBackGroundColor(ltgray);
  gc.PushClippingRegion(Rect2D(30,30,150,150));
  for (int t=0;t<360;t+=10) {
    float delx = ceil(100*cos(t*M_PI/180)+0.5);
    float dely = ceil(100*sin(t*M_PI/180)+0.5);
    gc.DrawLine(Point2D(150,150),Point2D(150+delx,150+dely));
  }
  gc.SetForeGroundColor(Color(0,0,0));
  gc.DrawText("Hello World!",Point2D(30,70));
  gc.FillRectangle(Rect2D(80,80,30,5));
  gc.DrawRectangle(Rect2D(80,90,30,5));
  gc.DrawCircle(Point2D(150,150),101);
  //  gc.DrawLine(Point2D(20,40),Point2D(250,280));
  canvas.WritePPM("diag.ppm");
}
