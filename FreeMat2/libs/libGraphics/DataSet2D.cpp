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

#include "DataSet2D.hpp"
#include <math.h>
#include "Malloc.hpp"

#include <iostream>

namespace FreeMat {

  DataSet2D::DataSet2D(Array xarg, Array yarg, char a_color, 
		       char a_symbol, char a_line) {
    color = a_color;
    symbol = a_symbol;
    line = a_line;
    x = xarg;
    y = yarg;
    symbolLength = 3;
  }

  DataSet2D::~DataSet2D() {
  }

  LineStyleType DataSet2D::GetPenStyle() {
    if (line == '-')
      return LINE_SOLID;
    if (line == ':')
      return LINE_DOTTED;
    if (line == ';')
      return LINE_DASH_DOT;
    if (line == '|')
      return LINE_DASHED;
    return LINE_SOLID;
  }

  void DataSet2D::SetPenColor(GraphicsContext& dc, bool useStyle) {
    LineStyleType penStyle;

    if (useStyle)
      penStyle = GetPenStyle();
    else
      penStyle = LINE_SOLID;

    dc.SetLineStyle(penStyle);

    switch(color) {
    case 'y':
      dc.SetForeGroundColor(Color("yellow"));
      break;
    case 'm':
      dc.SetForeGroundColor(Color("magenta"));
      break;
    case 'c':
      dc.SetForeGroundColor(Color("cyan"));
      break;
    case 'r':
      dc.SetForeGroundColor(Color("red"));
      break;
    case 'g':
      dc.SetForeGroundColor(Color("green"));
      break;
    case 'b':
      dc.SetForeGroundColor(Color("blue"));
      break;
    case 'w':
      dc.SetForeGroundColor(Color("white"));
      break;
    case 'k':
      dc.SetForeGroundColor(Color("black"));
      break;
    }
    if (useStyle && (line == ' '))
      dc.SetLineStyle( LINE_NONE );
  }

  void DataSet2D::GetDataRange(double& xMin, double &xMax, double &yMin, double & yMax) {
    const double *xVals;
    const double *yVals;
    int ptCount;
	int i;

    xVals = (const double *) x.getDataPointer();
    yVals = (const double *) y.getDataPointer();
    ptCount = x.getLength();

    // Calculate the min and max values in x
    xMin = xVals[0];  
    xMax = xMin;
    for (i=0;i<ptCount;i++) {
      xMin = (xVals[i] < xMin) ? xVals[i] : xMin;
      xMax = (xVals[i] > xMax) ? xVals[i] : xMax;
    }
    // Calculate the min and max values in y
    yMin = yVals[0];
    yMax = yMin;
    for (i=0;i<ptCount;i++) {
      yMin = (yVals[i] < yMin) ? yVals[i] : yMin;
      yMax = (yVals[i] > yMax) ? yVals[i] : yMax;
    }
    // If the y values are less than eps apart, set the y range to eps
    if (fabs(yMax - yMin) < 1e-5) {
      yMin = (yMax+yMin)/2 - 1e-5;
      yMax = (yMax+yMin)/2 + 1e-5;
    }
      
  }

  void DataSet2D::PutSymbol(GraphicsContext &dc, int xp, int yp, 
			    char symbol, int len) {
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

  void DataSet2D::DrawMe(GraphicsContext &dc, Axis *xAxis, Axis *yAxis) {
    const double *xVals;
    const double *yVals;
    int ptCount;
	int i;

    xVals = (const double *) x.getDataPointer();
    yVals = (const double *) y.getDataPointer();
    ptCount = x.getLength();

    SetPenColor(dc, false);
    // Draw the symbols
    for (i=0;i<ptCount;i++) {
      // Map the data point to a coordinate
      int xp, yp;
      xp = xAxis->MapPoint(xVals[i]);
      yp = yAxis->MapPoint(yVals[i]);
      PutSymbol(dc, xp, yp, symbol, symbolLength);
    }
    // Plot the lines
    SetPenColor(dc, true);
    std::vector<Point2D> pts;
    for (i=0;i<ptCount;i++)
      pts.push_back(Point2D(xAxis->MapPoint(xVals[i]),
			    yAxis->MapPoint(yVals[i])));
    dc.DrawLines(pts);
  }
}
