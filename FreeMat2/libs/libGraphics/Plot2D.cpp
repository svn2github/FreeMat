// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Plot2D.hpp"
#include "RGBImage.hpp"
#include "GraphicsCore.hpp"
#include <math.h>
#include <iostream>
#include <stdio.h>

namespace FreeMat {

  Plot2D::Plot2D(int width, int height) : PrintableWidget(0,0,width,height) {
    space = 10;
    holdflag = false;
    updating = false;
    legendActive = false;
    gridflag = false;
  }

  Plot2D::~Plot2D() {
  }

  void Plot2D::DrawLegend(GraphicsContext &gc) {
    int xc, yc;
    MapPoint(legend_xc,legend_yc,xc,yc);
    int i;
    int strut;
    Point2D strutSize(gc.GetTextExtent("|"));
    strut = strutSize.y;
    double centerline;
    centerline = yc + strut/2*1.2;
    int maxwidth = 0;
    for (i=0;i<legend_data.size();i+=2) {
      Array ls(legend_data[i]);
      Array label_txt(legend_data[i+1]);
      char *linestyle = ls.getContentsAsCString();
      char *label = label_txt.getContentsAsCString();
      // Draw the line segment - set the color and line style
      // from linestyle
      gc.SetLineStyle(UtilityMapLineStyleToType(linestyle[2]));
      gc.SetForeGroundColor(UtilityMapColorSpecToColor(linestyle[0]));
      gc.DrawLine(Point2D(xc,centerline),
		  Point2D(xc+18,centerline));
      // Draw the symbol
      gc.SetLineStyle(LINE_SOLID);
      PutSymbol(gc,xc+9,centerline,linestyle[1],3);
      gc.SetForeGroundColor(Color("black"));
      gc.DrawTextString(label,Point2D(xc+22,centerline+strut/2-1));
      Point2D tmp(gc.GetTextExtent(std::string(label)));
      maxwidth = (maxwidth < tmp.x) ? tmp.x : maxwidth;
      centerline += strut*1.2;
    }
    gc.SetLineStyle(UtilityMapLineStyleToType(legend_linestyle[2]));
    gc.SetForeGroundColor(UtilityMapColorSpecToColor(legend_linestyle[0]));
    gc.DrawRectangle(Rect2D(xc-5,yc,32+maxwidth,centerline-strut/2-yc));
  }

  void Plot2D::StartSequence() {
    if (!holdflag)
      data.clear();
    holdSave = holdflag;
    holdflag = true;
    updating = true;
    legendActive = false;
  }

  void Plot2D::StopSequence() {
    holdflag = holdSave;
    updating = false;
  }

  void Plot2D::SetLegend(double xc, double yc, Array style, 
			 ArrayVector legendData) {
    legendActive = true;
    legend_xc = xc;
    legend_yc = yc;
    strcpy(legend_linestyle,style.getContentsAsCString());
    legend_data = legendData;
  }

  void Plot2D::SetTitleText(std::string txt) {
    title = txt;
  }

  void Plot2D::SetXLabel(std::string txt) {
    xlabel = txt;
  }

  void Plot2D::SetYLabel(std::string txt) {
    ylabel = txt;
  }

  void Plot2D::SetHoldFlag(bool flag) {
    holdflag = flag;
  }

  bool Plot2D::GetHoldFlag() {
    return holdflag;
  }

  void Plot2D::SetLog(bool xLog, bool yLog) {
    //    if (xAxis != NULL)
    //      xAxis->SetLogarithmic(xLog);
    //    if (yAxis != NULL)
    //      yAxis->SetLogarithmic(yLog);
  }

  void Plot2D::SetAxes(double x1, double x2, double y1, double y2) {
    xAxis.ManualSetAxis(x1,x2);
    yAxis.ManualSetAxis(y1,y2);
  }

  void Plot2D::GetAxes(double &x1, double &x2, double &y1, double &y2) {
    xAxis.GetAxisExtents(x1,x2);
    yAxis.GetAxisExtents(y1,y2);
  }

  void Plot2D::SetGrid(bool gridVal) {
    gridflag = gridVal;
  }

  void Plot2D::SetAxesTight() {
    if (data.size() == 0) return;
    // Adjust the axes...
    double xMin, xMax, yMin, yMax;
    DataSet2D &t = data[0];
    t.GetDataRange(xMin, xMax, yMin, yMax);
    for (int i=1;i<data.size();i++) {
      DataSet2D &t = data[i];
      double txMin, txMax, tyMin, tyMax;
      t.GetDataRange(txMin, txMax, tyMin, tyMax);
      xMin = (xMin < txMin) ? xMin : txMin;
      yMin = (yMin < tyMin) ? yMin : tyMin;
      xMax = (xMax > txMax) ? xMax : txMax;
      yMax = (yMax > tyMax) ? yMax : tyMax;
    }
    xAxis.ManualSetAxis(xMin, xMax);
    yAxis.ManualSetAxis(yMin, yMax);
  }
  
  void Plot2D::SetAxesAuto() {
    if (data.size() == 0) return;
    // Adjust the axes...
    double xMin, xMax, yMin, yMax;
    DataSet2D &t = data[0];
    t.GetDataRange(xMin, xMax, yMin, yMax);
    for (int i=1;i<data.size();i++) {
      DataSet2D &t = data[i];
      double txMin, txMax, tyMin, tyMax;
      t.GetDataRange(txMin, txMax, tyMin, tyMax);
      xMin = (xMin < txMin) ? xMin : txMin;
      yMin = (yMin < tyMin) ? yMin : tyMin;
      xMax = (xMax > txMax) ? xMax : txMax;
      yMax = (yMax > tyMax) ? yMax : tyMax;
    }
    xAxis.SetDataRange(xMin, xMax);
    yAxis.SetDataRange(yMin, yMax);
  }

  void Plot2D::AddPlot(DataSet2D dp) {
    if (!holdflag)
      data.clear();
    data.push_back(dp);
    SetAxesAuto();
  }

  void Plot2D::draw() {
    FLTKGC gc(w(),h());
    OnDraw(gc);
  }

  void Plot2D::resize(int x, int y, int w, int h) {
    Fl_Widget::resize(x,y,w,h);
    redraw();
  }

  void Plot2D::DrawAxes(GraphicsContext &gc, Rect2D viewport) {
  }

  void Plot2D::MapPoint(double x, double y, int &xc, int &yc) {
    double xn = xAxis.Normalize(x);
    double yn = yAxis.Normalize(y);
    double u, v;
    u = viewport.x1 + xn*viewport.width;
    v = viewport.y1 + yn*viewport.height;
    u = std::min(4096.0,std::max(-4096.0,u));
    v = std::min(4096.0,std::max(-4096.0,v));
    xc = (int) u;
    yc = (int) v;
  }

  void Plot2D::OnDraw(GraphicsContext &gc) {
    Point2D sze(gc.GetCanvasSize());
    int width = sze.x;
    int height = sze.y;
    gc.SetBackGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("light grey"));
    gc.FillRectangle(Rect2D(0, 0, sze.x, sze.y));

    if (updating || (data.size() == 0))
      return;
    gc.SetFont(12);

    int client_y_offset = 0;
    int client_x_offset = 0;

    client_x_offset = 5;
    client_y_offset = 5;
    width -= 10;
    height -= 10;

    int space = 10;
    // A generic length for text
    Point2D t(gc.GetTextExtent("|"));
    int sze_textheight = t.y;

    // The title is located space pixels down from the
    // top, and with the left corner at the center of
    // the plot area minus half the title width
    // The width of the plot is width - 4*hspace 
    int plotWidth;
    int plotHeight;
    int plotX;
    int plotY;

    // Need space for the tic, text, and a spacer
    plotWidth = width - 2*space - sze_textheight;
    plotX = 2*space+sze_textheight;
    // If the label is active, subtract another text and spacer
    if (!ylabel.empty()) {
      plotWidth -= (space+sze_textheight);
      plotX += space+sze_textheight;
    }

    // Need space for the tic, text and a spacer
    plotHeight = height-2*space-sze_textheight;
    plotY = 2*space+sze_textheight;
    // If the label is active, subtract another text and spacer
    if (!xlabel.empty()) 
      plotHeight -= (space+sze_textheight);

    // If the title is active, subtract another text and spacer
    if (!title.empty()) {
      plotHeight -= (space+sze_textheight);
      plotY += (space+sze_textheight);
    }

    gc.SetForeGroundColor(Color("black"));
    gc.DrawTextStringAligned(title, Point2D(plotX + plotWidth/2,space),
			     LR_CENTER, TB_TOP);
    gc.SetForeGroundColor(Color("white"));
    gc.FillRectangle(Rect2D(plotX, plotY, plotWidth + 1, plotHeight + 1));
    

    viewport = Rect2D(plotX, plotY, plotWidth + 1, plotHeight + 1);
    DrawAxes(gc,viewport);
    gc.PushClippingRegion(viewport);

    for (int i=0;i<data.size();i++)
      data[i].DrawMe(gc, *this);

    if (legendActive)
      DrawLegend(gc);

    gc.PopClippingRegion();
  }

  LineStyleType UtilityMapLineStyleToType(char line) {
    if (line == '-')
      return LINE_SOLID;
    if (line == ':')
      return LINE_DOTTED;
    if (line == ';')
      return LINE_DASH_DOT;
    if (line == '|')
      return LINE_DASHED;
    if (line == ' ')
      return LINE_NONE;
    return LINE_SOLID;
  }

  Color UtilityMapColorSpecToColor(char color) {
    switch(color) {
    case 'y':
      return(Color("yellow"));
    case 'm':
      return(Color("magenta"));
    case 'c':
      return(Color("cyan"));
    case 'r':
      return(Color("red"));
    case 'g':
      return(Color("green"));
    case 'b':
      return(Color("blue"));
    case 'w':
      return(Color("white"));
    case 'k':
      return(Color("black"));
    }
    return(Color("black"));
  }

  void PutSymbol(GraphicsContext &dc, int xp, int yp, char symbol, int len) {
    int len2 = (int) (len / sqrt(2.0));
    switch (symbol) {
    case '.':
      dc.DrawPoint(Point2D(xp, yp));
      break;
    case 'o':
      dc.DrawCircle(Point2D(xp, yp), len);
      break;
    case 'x':
      dc.DrawLine(Point2D(xp - len2, yp - len2), 
		  Point2D(xp + len2 + 1, yp + len2 + 1));
      dc.DrawLine(Point2D(xp + len2, yp - len2), 
		  Point2D(xp - len2 - 1, yp + len2 + 1));
      break;
    case '+':
      dc.DrawLine(Point2D(xp - len, yp), 
		  Point2D(xp + len + 1, yp));
      dc.DrawLine(Point2D(xp, yp - len), 
		  Point2D(xp, yp + len + 1));
      break;
    case '*':
      dc.DrawLine(Point2D(xp - len, yp), 
		  Point2D(xp + len + 1, yp));
      dc.DrawLine(Point2D(xp, yp - len), 
		  Point2D(xp, yp + len + 1));
      dc.DrawLine(Point2D(xp - len2, yp - len2), 
		  Point2D(xp + len2 + 1, yp + len2 + 1));
      dc.DrawLine(Point2D(xp + len2, yp - len2), 
		  Point2D(xp - len2 - 1, yp + len2 + 1));
      break;
    case 's':
      dc.DrawRectangle(Rect2D(xp - len/2, yp - len/2, len + 1, len + 1));
      break;
    case 'd':
      dc.DrawLine(Point2D(xp - len, yp), Point2D(xp, yp - len));
      dc.DrawLine(Point2D(xp, yp - len), Point2D(xp + len, yp));
      dc.DrawLine(Point2D(xp + len, yp), Point2D(xp, yp + len));
      dc.DrawLine(Point2D(xp, yp + len), Point2D(xp - len, yp));
      break;
    case 'v':
      dc.DrawLine(Point2D(xp - len, yp - len), 
		  Point2D(xp + len, yp - len));
      dc.DrawLine(Point2D(xp + len, yp - len), 
		  Point2D(xp, yp + len));
      dc.DrawLine(Point2D(xp, yp + len), 
		  Point2D(xp - len, yp - len));
      break;
    case '^':
      dc.DrawLine(Point2D(xp - len, yp + len), 
		  Point2D(xp + len, yp + len));
      dc.DrawLine(Point2D(xp + len, yp + len), 
		  Point2D(xp, yp - len));
      dc.DrawLine(Point2D(xp, yp - len), 
		  Point2D(xp - len, yp + len));
      break;
    case '<':
      dc.DrawLine(Point2D(xp + len, yp - len), 
		  Point2D(xp - len, yp));
      dc.DrawLine(Point2D(xp - len, yp), 
		  Point2D(xp + len, yp + len));
      dc.DrawLine(Point2D(xp + len, yp + len), 
		  Point2D(xp + len, yp - len));
      break;
    case '>':
      dc.DrawLine(Point2D(xp - len, yp - len), 
		  Point2D(xp + len, yp));
      dc.DrawLine(Point2D(xp + len, yp), 
		  Point2D(xp - len, yp + len));
      dc.DrawLine(Point2D(xp - len, yp + len), 
		  Point2D(xp - len, yp - len));
      break;
    }
  }
}
