#include "RGBImageGC.hpp"
#include "PostScriptGC.hpp"
#include <math.h>
#include <iostream>

void TestDrawStuff(GraphicsContext &gc) {
  gc.PushClippingRegion(Rect2D(30,30,150,150));
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
  gc.PopClippingRegion();
  gc.DrawCircle(Point2D(150,150),101);
  gc.SetLineStyle(LINE_DASH_DOT);
  gc.DrawLine(Point2D(40,200),Point2D(80,280));
  gc.DrawText("Coolio!",Point2D(40,200));
  RGBImage stamp(30,30);
  for (int i=0;i<30;i++)
    for (int j=0;j<30;j++)
      stamp.SetPixel(i,j,Color(255-8*i,50,8*j));
  gc.SetLineStyle(LINE_SOLID);
  gc.SetForeGroundColor(Color(0,0,0));
  gc.DrawLine(Point2D(10,180),Point2D(290,180));
  gc.BlitRGBImage(Point2D(230,180),stamp);
  GrayscaleImage stamp2(50,25,NULL);
  for (int i=0;i<25;i++)
    for (int j=0;j<50;j++)
      stamp2.SetPixel(j,i,i*j/5);
  gc.BlitGrayscaleImage(Point2D(130,50),stamp2);
  gc.DrawText("Coolio!",Point2D(200,200));
  gc.DrawLine(Point2D(350,0),Point2D(350,300));
  gc.DrawLine(Point2D(300,50),Point2D(400,50));
  // 350, 50
  Point2D textbox(gc.GetTextExtent("CenterMe!"));
  gc.DrawText("CenterMe!",Point2D(350-textbox.x/2,50+textbox.y/2));
  gc.DrawCircle(Point2D(450,150),10);
  gc.DrawText("Welcome",Point2D(450,150),ORIENT_0);
  gc.DrawText("Welcome 90",Point2D(450,150),ORIENT_90);
  gc.DrawText("Welcome 180",Point2D(450,150),ORIENT_180);
  gc.DrawText("Welcome 270",Point2D(450,150),ORIENT_270);
}

int main() {
  RGBImage canvas(500,500);
  canvas.SetAllPixels(Color(255,255,255));
  RGBImageGC gc(canvas);
  TestDrawStuff(gc);
  Point2D extentTest;
  extentTest = gc.GetTextExtent("Hello !! This is fun$\n");
  std::cout << extentTest.x << " x " << extentTest.y << "\n";
  extentTest = gc.GetTextExtent("1");
  std::cout << extentTest.x << " x " << extentTest.y << "\n";
  extentTest = gc.GetTextExtent("Hello !! This is fun$\n");
  std::cout << extentTest.x << " x " << extentTest.y << "\n";
  extentTest = gc.GetTextExtent("x");
  std::cout << extentTest.x << " x " << extentTest.y << "\n";
  extentTest = gc.GetTextExtent("Hello !! This is fun$\n");
  std::cout << extentTest.x << " x " << extentTest.y << "\n";
  extentTest = gc.GetTextExtent("Title is a Long thingy");
  std::cout << extentTest.x << " x " << extentTest.y << "\n";
  canvas.WritePPM("diag.ppm");
  PostScriptGC ec("diag.eps",500,500);
  TestDrawStuff(ec);
}
