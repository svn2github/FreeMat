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
#include "Util.hpp"
#include <math.h>
#include <iostream>
#include <stdio.h>

#undef min
#undef max

namespace FreeMat {

  Plot2D::Plot2D(QWidget *parent) : 
    QPWidget(parent,"plot2d") {
    space = 10;
    holdflag = false;
    updating = false;
    legendActive = false;
    gridflag = false;
  }

  Plot2D::~Plot2D() {
  }

  void Plot2D::DrawTextLabels(QPainter &gc) {
    int xc, yc;
    gc.setPen(QPen(Qt::black));
    for (int i=0;i<textx.size();i++) {
      MapPoint(textx[i],texty[i],xc,yc);
      DrawTextStringAligned(gc,textlabel[i],Point2D(xc,yc),LR_LEFT,TB_BOTTOM);
    }
  }
  
  void Plot2D::DrawLegend(QPainter &gc) {
    int xc, yc;
    MapPoint(legend_xc,legend_yc,xc,yc);
    int i;
    int strut;
    Point2D strutSize(GetTextExtent(gc,"|"));
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
      gc.setPen(QPen(UtilityMapColorSpecToColor(linestyle[0]),0,
		     UtilityMapLineStyleToType(linestyle[2])));
      gc.drawLine(xc,centerline,xc+18,centerline);
      // Draw the symbol
      gc.setPen(Qt::SolidLine);
      PutSymbol(gc,xc+9,centerline,linestyle[1],3);
      gc.setPen(QPen(Qt::black));
      DrawTextString(gc,label,Point2D(xc+22,centerline+strut/2-1));
      Point2D tmp(GetTextExtent(gc,std::string(label)));
      maxwidth = (maxwidth < tmp.x) ? tmp.x : maxwidth;
      centerline += strut*1.2;
    }
    gc.setPen(QPen(UtilityMapColorSpecToColor(legend_linestyle[0]),0,
		   UtilityMapLineStyleToType(legend_linestyle[2])));
    gc.setBrush(Qt::NoBrush);
    gc.drawRect(xc-5,yc,32+maxwidth,centerline-strut/2-yc);
  }

  void Plot2D::StartSequence() {
    if (!holdflag)
      data.clear();
    holdSave = holdflag;
    holdflag = true;
    updating = true;
    legendActive = false;
    textx.clear();
    texty.clear();
    textlabel.clear();
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

  void Plot2D::AddText(double x, double y, std::string val) {
    textx.push_back(x);
    texty.push_back(y);
    textlabel.push_back(val);
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
    setMinimumSize(200,150);
  }

  void Plot2D::DrawAxes(QPainter &gc) {
    double xmin, xmax, ymin, ymax;
    int xc_min, xc_max, yc_min, yc_max;
    xAxis.GetAxisExtents(xmin,xmax);
    yAxis.GetAxisExtents(ymin,ymax);
    MapPoint(xmin,ymin,xc_min,yc_min);
    MapPoint(xmax,ymax,xc_max,yc_max);
    gc.setPen(QPen(Qt::black));
    gc.drawLine(xc_min,yc_min,xc_max,yc_min);
    gc.drawLine(xc_min,yc_min,xc_min,yc_max);
    if (!xlabel.empty()) {
      DrawTextStringAligned(gc,xlabel,
			    Point2D((xc_min+xc_max)/2,
				    yc_min+space+sze_textheight+space),
			    LR_CENTER, TB_TOP);
    }
    if (!ylabel.empty()) {
      DrawTextStringAligned(gc,ylabel,
			    Point2D(space+sze_textheight,
				    (yc_min+yc_max)/2),
			    LR_CENTER, TB_TOP, 90);
    }
    std::vector<double> xtics;
    xtics = xAxis.GetTickLocations();
    std::vector<std::string> xlabels;
    xlabels = xAxis.GetTickLabels();
    for (int i=0;i<xtics.size();i++) {
      double xp, yp;
      int xn, yn;
      xp = xtics[i]; yp = ymin;
      MapPoint(xp,yp,xn,yn);
      DrawTextStringAligned(gc,xlabels[i],Point2D(xn,yn+ticlen),
			    LR_CENTER, TB_TOP);
      int xn2, yn2;
      MapPoint(xp,ymax,xn2,yn2);
      if (gridflag && (xn != xc_min) && (xn != xc_max)) {
	gc.setPen(QPen(Qt::lightGray,0,Qt::DotLine));
	gc.drawLine(xn,yn,xn,yn2);
      }
      gc.setPen(QPen(Qt::black));
      gc.drawLine(xn,yn,xn,yn-ticlen);
    }
    std::vector<double> ytics;
    ytics = yAxis.GetTickLocations();
    std::vector<std::string> ylabels;
    ylabels = yAxis.GetTickLabels();
    for (int i=0;i<ytics.size();i++) {
      double xp, yp;
      int xn, yn;
      xp = xmin; yp = ytics[i];
      MapPoint(xp,yp,xn,yn);
      DrawTextStringAligned(gc,ylabels[i],Point2D(xn-5,yn),
			    LR_RIGHT, TB_CENTER);
      int xn2, yn2;
      MapPoint(xmax,yp,xn2,yn2);
      if (gridflag && (yn != yc_min) && (yn != yc_max)) {
	gc.setPen(QPen(Qt::lightGray,0,Qt::DotLine));
	gc.drawLine(xn,yn,xn2,yn);
      }
      gc.setPen(QPen(Qt::black));
      gc.drawLine(xn,yn,xn+ticlen,yn);
    }
  }

  void Plot2D::MapPoint(double x, double y, int &xc, int &yc) {
    double xn = xAxis.Normalize(x);
    double yn = yAxis.Normalize(y);
    double u, v;
    u = viewport.x1 + xn*viewport.width;
    v = viewport.y1 + (1-yn)*viewport.height;
    u = std::min(4096.0,std::max(-4096.0,u));
    v = std::min(4096.0,std::max(-4096.0,v));
    xc = (int) u;
    yc = (int) v;
  }

  void Plot2D::DrawMe(QPainter &gc) {
    Point2D sze(width(),height());
    int width = sze.x;
    int height = sze.y;
#if 0
    gc.SetBackGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("light grey"));
    gc.FillRectangle(Rect2D(0, 0, sze.x, sze.y));
#endif
    if (updating || (data.size() == 0))
      return;
    SetFontSize(gc,12);

    int client_y_offset = 0;
    int client_x_offset = 0;

    client_x_offset = 5;
    client_y_offset = 5;
    width -= 10;
    height -= 10;

    space = 10;
    ticlen = 5;
    // A generic length for text
    Point2D t(GetTextExtent(gc,"|"));
    sze_textheight = t.y;

    // The title is located space pixels down from the
    // top, and with the left corner at the center of
    // the plot area minus half the title width
    // The width of the plot is width - 4*hspace 
    int plotWidth;
    int plotHeight;
    int plotX;
    int plotY;

    std::vector<std::string> xlabels = xAxis.GetTickLabels();
    std::vector<std::string> ylabels = xAxis.GetTickLabels();

    // Need space for the text, and a spacer
    plotWidth = width - space - sze_textheight;
    plotX = space+sze_textheight;
    // If the label is active, subtract another text and spacer
    if (!ylabel.empty()) {
      plotWidth -= (space+sze_textheight);
      plotX += space+sze_textheight;
    }

    // Adjust the width to handle the length of the last x-label
    t = GetTextExtent(gc,xlabels.back());
    plotWidth -= t.x/2;

    // Adjust the plotX for the width of the labels on the y-axis
    int maxwidth;
    maxwidth = 0;
    for (int i=0;i<ylabels.size();i++){ 
      t = GetTextExtent(gc,ylabels[i]);
      maxwidth = std::max(maxwidth,t.x);
    }
    plotX += maxwidth+space/2;
    plotWidth -= (maxwidth+space/2);

    // Need space for the tic, text and a spacer
    plotHeight = height-2*space-sze_textheight;
    plotY = space;
    // If the label is active, subtract another text and spacer
    if (!xlabel.empty()) 
      plotHeight -= (space+sze_textheight);

    // If the title is active, subtract another text and 2 spacers
    if (!title.empty()) {
      plotHeight -= (space+sze_textheight);
      plotY += (space+sze_textheight);
    }

    gc.setPen(QPen(Qt::black));
    if (!title.empty())
      DrawTextStringAligned(gc, title, Point2D(plotX + plotWidth/2,space),
			    LR_CENTER, TB_TOP);
    gc.fillRect(plotX, plotY, plotWidth + 1, plotHeight + 1,
		QBrush(Qt::white));

    viewport = Rect2D(plotX, plotY, plotWidth + 1, plotHeight + 1);
    xAxis.SetAxisLength(plotWidth);
    yAxis.SetAxisLength(plotHeight);
    DrawAxes(gc);
    gc.save();
    QPoint origin(gc.xForm(QPoint(0,0)));
    gc.setClipRect(viewport.x1+origin.x(),viewport.y1+origin.y(),viewport.width,viewport.height);

    for (int i=0;i<data.size();i++)
      data[i].DrawMe(gc, *this);

    if (legendActive)
      DrawLegend(gc);

    if (!textx.empty())
      DrawTextLabels(gc);
    gc.restore();

  }

  Qt::PenStyle UtilityMapLineStyleToType(char line) {
    if (line == '-')
      return Qt::SolidLine;
    if (line == ':')
      return Qt::DotLine;
    if (line == ';')
      return Qt::DashDotLine;
    if (line == '|')
      return Qt::DashLine;
    if (line == ' ')
      return Qt::NoPen;
    return Qt::SolidLine;
  }

  QColor UtilityMapColorSpecToColor(char color) {
    switch(color) {
    case 'y':
      return(Qt::yellow);
    case 'm':
      return(Qt::magenta);
    case 'c':
      return(Qt::cyan);
    case 'r':
      return(Qt::red);
    case 'g':
      return(Qt::green);
    case 'b':
      return(Qt::blue);
    case 'w':
      return(Qt::white);
    case 'k':
      return(Qt::black);
    }
    return(Qt::black);
  }

  void PutSymbol(QPainter &dc, int xp, int yp, char symbol, int len) {
    int len2 = (int) (len / sqrt(2.0));
    switch (symbol) {
    case '.':
      dc.drawPoint(xp, yp);
      break;
    case 'o':
      dc.setBrush(Qt::NoBrush);
      dc.drawEllipse(xp-len,yp-len,2*len,2*len);
      break;
    case 'x':
      dc.drawLine(xp - len2, yp - len2, xp + len2 + 1, yp + len2 + 1);
      dc.drawLine(xp + len2, yp - len2, xp - len2 - 1, yp + len2 + 1);
      break;
    case '+':
      dc.drawLine(xp - len, yp, xp + len + 1, yp);
      dc.drawLine(xp, yp - len, xp, yp + len + 1);
      break;
    case '*':
      dc.drawLine(xp - len, yp, xp + len + 1, yp);
      dc.drawLine(xp, yp - len, xp, yp + len + 1);
      dc.drawLine(xp - len2, yp - len2, xp + len2 + 1, yp + len2 + 1);
      dc.drawLine(xp + len2, yp - len2, xp - len2 - 1, yp + len2 + 1);
      break;
    case 's':
      dc.setBrush(Qt::NoBrush);
      dc.drawRect(xp - len/2, yp - len/2, len + 1, len + 1);
      break;
    case 'd':
      dc.drawLine(xp - len, yp, xp, yp - len);
      dc.drawLine(xp, yp - len, xp + len, yp);
      dc.drawLine(xp + len, yp, xp, yp + len);
      dc.drawLine(xp, yp + len, xp - len, yp);
      break;
    case 'v':
      dc.drawLine(xp - len, yp - len, xp + len, yp - len);
      dc.drawLine(xp + len, yp - len, xp, yp + len);
      dc.drawLine(xp, yp + len, xp - len, yp - len);
      break;
    case '^':
      dc.drawLine(xp - len, yp + len, xp + len, yp + len);
      dc.drawLine(xp + len, yp + len, xp, yp - len);
      dc.drawLine(xp, yp - len, xp - len, yp + len);
      break;
    case '<':
      dc.drawLine(xp + len, yp - len, xp - len, yp);
      dc.drawLine(xp - len, yp, xp + len, yp + len);
      dc.drawLine(xp + len, yp + len, xp + len, yp - len);
      break;
    case '>':
      dc.drawLine(xp - len, yp - len, xp + len, yp);
      dc.drawLine(xp + len, yp, xp - len, yp + len);
      dc.drawLine(xp - len, yp + len, xp - len, yp - len);
      break;
    }
  }
}
