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
#include "RGBImageGC.hpp"
#include "RGBImage.hpp"
#include "GraphicsCore.hpp"
#include <math.h>
#include <iostream>
#include <stdio.h>

namespace FreeMat {

  Plot2D::Plot2D(int fignum) : XWindow(VectorWindow){
    space = 10;
    xAxis = NULL;
    yAxis = NULL;
    holdflag = false;
    char buffer[1000];
    sprintf(buffer,"Plot Window %d",fignum+1);
    SetTitle(buffer);  
    myFigureNumber = fignum;
    updating = false;
  }

  Plot2D::~Plot2D() {
    if (xAxis != NULL) delete xAxis;
    if (yAxis != NULL) delete yAxis;
  }

  void Plot2D::StartSequence() {
    if (!holdflag)
      data.clear();
    holdSave = holdflag;
    holdflag = true;
    updating = true;
  }

  void Plot2D::StopSequence() {
    holdflag = holdSave;
    updating = false;
	Refresh();
  }

  void Plot2D::SetTitleText(std::string txt) {
    title = txt;
    Refresh();
  }

  void Plot2D::SetXLabel(std::string txt) {
    if (xAxis != NULL)
      xAxis->SetLabelText(txt);
    Refresh();
  }

  void Plot2D::SetYLabel(std::string txt) {
    if (yAxis != NULL)
      yAxis->SetLabelText(txt);
    Refresh();
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
    Refresh();
  }

  void Plot2D::SetAxes(double x1, double x2, double y1, double y2) {
    if (xAxis != NULL)
      xAxis->ManualSetAxis(x1, x2);
    if (yAxis != NULL)
      yAxis->ManualSetAxis(y1, y2);
    Refresh();
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
    Refresh();
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
    Refresh();
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
    Refresh();
  }

  void Plot2D::AddPlot(DataSet2D dp) {
    if (!holdflag)
      data.clear();
    data.push_back(dp);
    SetAxesAuto();
    Refresh();
  }

  void Plot2D::ComputeTextBounds(GraphicsContext &dc) {
    Point2D t(dc.GetTextExtent(title));
    titleWidth = t.x; titleHeight = t.y;
    if (xAxis != NULL)
      xAxis->ComputeTextBounds(dc);
    if (yAxis != NULL)
      yAxis->ComputeTextBounds(dc);
  }

   void Plot2D::OnClose() {
     NotifyPlotClose(myFigureNumber);
   }

  void Plot2D::OnDraw(GraphicsContext &gc) {
    if (updating || (data.size() == 0))
      return;
    if (xAxis == NULL) return;
    if (yAxis == NULL) return;
    int width = getWidth();
    int height = getHeight();

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

    gc.SetBackGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("light grey"));
    gc.FillRectangle(Rect2D(0, 0, getWidth(), getHeight()));
    gc.SetForeGroundColor(Color("black"));
    gc.DrawTextString(title, Point2D(plotX + (plotWidth - titleWidth)/2, space + titleHeight));
    gc.SetForeGroundColor(Color("white"));
    gc.FillRectangle(Rect2D(plotX, plotY, plotWidth + 1, plotHeight + 1));
    xAxis->DrawMe(gc);
    yAxis->DrawMe(gc);

    gc.PushClippingRegion(Rect2D(plotX, plotY, plotWidth + 1, plotHeight + 1));

    for (int i=0;i<data.size();i++)
      data[i].DrawMe(gc, xAxis, yAxis);

    gc.PopClippingRegion();
  }
}
