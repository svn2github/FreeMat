#include "RGBImageGC.hpp"
#include "PostScriptGC.hpp"
#include <math.h>

void TestDrawStuff(GraphicsContext &gc) {
  //  gc.PushClippingRegion(Rect2D(30,30,150,150));
  for (int t=0;t<360;t+=10) {
    float delx = ceil(100*cos(t*M_PI/180)+0.5);
    float dely = ceil(100*sin(t*M_PI/180)+0.5);
    gc.DrawLine(Point2D(150,150),Point2D(150+delx,150+dely));
  }
  gc.SetForeGroundColor(Color(255,125,32));
  gc.SetFont("swiss",12);
  gc.DrawText("Hello World!",Point2D(30,70));
  gc.FillRectangle(Rect2D(80,80,30,5));
  gc.DrawRectangle(Rect2D(80,90,30,5));
  //  gc.PopClippingRegion();
  gc.DrawCircle(Point2D(150,150),101);
}

int main() {
  RGBImage canvas(300,300);
  Color ltgray(Color(200,200,200));
  canvas.SetAllPixels(ltgray);
  RGBImageGC gc(canvas);
  gc.SetBackGroundColor(ltgray);
  TestDrawStuff(gc);
  canvas.WritePPM("diag.ppm");
  PostScriptGC ec("diag.eps",300,300);
  TestDrawStuff(ec);
}
