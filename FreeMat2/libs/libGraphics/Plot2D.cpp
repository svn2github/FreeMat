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

  Plot2D::Plot2D(int width, int height) : Fl_Widget(0,0,width,height) {
    space = 10;
    xAxis = NULL;
    yAxis = NULL;
    holdflag = false;
    updating = false;
    legendActive = false;
  }

  Plot2D::~Plot2D() {
    if (xAxis != NULL) delete xAxis;
    if (yAxis != NULL) delete yAxis;
  }

  void Plot2D::DrawLegend(GraphicsContext &gc) {
    double xc, yc;
    xc = xAxis->MapPoint(legend_xc);
    yc = yAxis->MapPoint(legend_yc);
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
      Point2D tmp(gc.GetTextExtent(label));
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
    if (xAxis != NULL)
      xAxis->SetLabelText(txt);
  }

  void Plot2D::SetYLabel(std::string txt) {
    if (yAxis != NULL)
      yAxis->SetLabelText(txt);
  }

  void Plot2D::SetHoldFlag(bool flag) {
    holdflag = flag;
  }

  bool Plot2D::GetHoldFlag() {
    return holdflag;
  }

  void Plot2D::SetLog(bool xLog, bool yLog) {
    if (xAxis != NULL)
      xAxis->SetLogarithmic(xLog);
    if (yAxis != NULL)
      yAxis->SetLogarithmic(yLog);
  }

  void Plot2D::SetAxes(double x1, double x2, double y1, double y2) {
    if (xAxis != NULL)
      xAxis->ManualSetAxis(x1, x2);
    if (yAxis != NULL)
      yAxis->ManualSetAxis(y1, y2);
  }

  void Plot2D::GetAxes(double &x1, double &x2, double &y1, double &y2) {
    if (xAxis != NULL)
      xAxis->GetAxisExtents(x1, x2);
    else {
      x1 = 0;
      x2 = 1;
    }
    if (yAxis != NULL)
      yAxis->GetAxisExtents(y1, y2);
    else {
      y1 = 0;
      y2 = 1;
    }
  }

  void Plot2D::SetGrid(bool gridVal) {
    if (xAxis != NULL)
      xAxis->SetGrid(gridVal);
    if (yAxis != NULL)
      yAxis->SetGrid(gridVal);
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
    if (xAxis == NULL)
      xAxis = new Axis(xMin, xMax, false, Axis_X);
    xAxis->ManualSetAxis(xMin, xMax);
    if (yAxis == NULL)
      yAxis = new Axis(yMin, yMax, false, Axis_Y);
    yAxis->ManualSetAxis(yMin, yMax);
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
    if (xAxis == NULL)
      xAxis = new Axis(xMin, xMax, false, Axis_X);
    else
      xAxis->SetExtent(xMin, xMax);
    if (yAxis == NULL)
      yAxis = new Axis(yMin, yMax, false, Axis_Y);
    else
      yAxis->SetExtent(yMin, yMax);
  }

  void Plot2D::AddPlot(DataSet2D dp) {
    if (!holdflag)
      data.clear();
    data.push_back(dp);
    SetAxesAuto();
  }

  void Plot2D::ComputeTextBounds(GraphicsContext &dc) {
    Point2D t(dc.GetTextExtent(title));
    titleWidth = t.x; titleHeight = t.y;
    if (xAxis != NULL)
      xAxis->ComputeTextBounds(dc);
    if (yAxis != NULL)
      yAxis->ComputeTextBounds(dc);
  }

  void Plot2D::draw() {
    FLTKGC gc(w(),h());
    OnDraw(gc);
  }

  void Plot2D::resize(int x, int y, int w, int h) {
    Fl_Widget::resize(x,y,w,h);
    redraw();
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
    if (xAxis == NULL) return;
    if (yAxis == NULL) return;

    gc.SetFont(12);

    int client_y_offset = 0;
    int client_x_offset = 0;

    client_x_offset = 5;
    client_y_offset = 5;
    width -= 10;
    height -= 10;
    ComputeTextBounds(gc);
    int space = 10;
    // The title is located vspace pixels down from the
    // top, and with the left corner at the center of
    // the plot area minus half the title width
    // The width of the plot is width - 4*hspace 
    int plotWidth;
    int plotHeight;

    plotWidth = width - space - yAxis->getWidth();
    plotHeight = height - 2*space - titleHeight - xAxis->getHeight();

    int plotX;
    int plotY;

    plotX = yAxis->getWidth();
    plotY = 2*space + titleHeight;

    xAxis->Place(plotX, plotY + plotHeight, plotWidth, plotHeight);
    yAxis->Place(plotX, plotY, plotHeight, plotWidth);

    gc.SetForeGroundColor(Color("black"));
    gc.DrawTextString(title, Point2D(plotX + (plotWidth - titleWidth)/2, space + titleHeight));
    gc.SetForeGroundColor(Color("white"));
    gc.FillRectangle(Rect2D(plotX, plotY, plotWidth + 1, plotHeight + 1));
    xAxis->DrawMe(gc);
    yAxis->DrawMe(gc);

    gc.PushClippingRegion(Rect2D(plotX, plotY, plotWidth + 1, plotHeight + 1));

    for (int i=0;i<data.size();i++)
      data[i].DrawMe(gc, xAxis, yAxis);

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
