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
#include "GraphicsServer.hpp"
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <wx/colour.h>

namespace FreeMat {

  Plot2D::Plot2D(::GraphicsServer* server, int fignum) : 
    PrintableFrame(_T("Plot Window"), wxSize(500,400)) {
    SetBackgroundColour( *wxWHITE );
    //  dataSetCount = 0;
    space = 10;
    xAxis = NULL;
    yAxis = NULL;
    holdflag = false;
    wxString title;
    title.Printf("Plot Window %d",fignum+1);
    SetTitle(title);  
    SetBackgroundColour(*wxLIGHT_GREY);
    myFigureNumber = fignum;
    m_server = server;
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
  }

  void Plot2D::SetTitleText(wxString txt) {
    title = txt;
    Refresh();
    Update();
  }

  void Plot2D::SetXLabel(wxString txt) {
    if (xAxis != NULL)
      xAxis->SetLabelText(txt);
    Refresh();
  }

  void Plot2D::SetYLabel(wxString txt) {
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

  void Plot2D::ComputeTextBounds(wxDC &dc) {
    dc.GetTextExtent( title, &titleWidth, &titleHeight );
    if (xAxis != NULL)
      xAxis->ComputeTextBounds(dc);
    if (yAxis != NULL)
      yAxis->ComputeTextBounds(dc);
  }

  void Plot2D::OnClose(wxCloseEvent &event) {
    m_server->NotifyPlotClose(myFigureNumber);
    Destroy();
  }

  void Plot2D::DrawMe(wxDC &dc) {
    if (updating || (data.size() == 0));
    if (xAxis == NULL) return;
    if (yAxis == NULL) return;

    dc.SetFont( *wxSWISS_FONT );

    int client_width;
    int client_height;
    int client_y_offset = 0;
    int client_x_offset = 0;

    GetClientSize(&client_width, &client_height);

    // Erase the background
    client_x_offset = 5;
    client_y_offset = 5;
    client_width -= 10;
    client_height -= 10;
    ComputeTextBounds(dc);
    wxCoord space = 10;
    // The title is located vspace pixels down from the
    // top, and with the left corner at the center of
    // the plot area minus half the title width
    // The width of the plot is client_width - 4*hspace 
    wxCoord plotWidth;
    wxCoord plotHeight;

    plotWidth = client_width - space - yAxis->getWidth();
    plotHeight = client_height - 2*space - titleHeight - xAxis->getHeight();

    wxCoord plotX;
    wxCoord plotY;

    plotX = yAxis->getWidth();
    plotY = 2*space + titleHeight;

    xAxis->Place(plotX, plotY + plotHeight, plotWidth, plotHeight);
    yAxis->Place(plotX, plotY, plotHeight, plotWidth);

    dc.DrawText(title, plotX + (plotWidth - titleWidth)/2, space);
  
    dc.SetBrush( *wxWHITE_BRUSH);
    dc.SetPen( *wxTRANSPARENT_PEN);
    dc.DrawRectangle(plotX, plotY, plotWidth + 1, plotHeight + 1);

    xAxis->DrawMe(dc);
    yAxis->DrawMe(dc);

    dc.DestroyClippingRegion();
    dc.SetClippingRegion(plotX, plotY, plotWidth + 1, plotHeight + 1);

    for (int i=0;i<data.size();i++)
      data[i].DrawMe(dc, xAxis, yAxis);

    dc.DestroyClippingRegion();    
  }

  void Plot2D::OnPaint(wxPaintEvent &WXUNUSED(event)) {
    wxPaintDC dc(this);
    PrepareDC(dc);
    DrawMe(dc);
  }

  
  IMPLEMENT_DYNAMIC_CLASS(Plot2D, wxFrame)

    BEGIN_EVENT_TABLE(Plot2D, wxFrame)
    EVT_PAINT(Plot2D::OnPaint)
    EVT_CLOSE(Plot2D::OnClose)
    END_EVENT_TABLE()

}
