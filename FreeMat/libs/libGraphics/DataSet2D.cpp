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

  int DataSet2D::GetPenStyle() {
    if (line == '-')
      return wxSOLID;
    if (line == ':')
      return wxDOT;
    if (line == ';')
      return wxDOT_DASH;
    if (line == '|')
      return wxSHORT_DASH;
    return wxSOLID;
  }

  void DataSet2D::SetPenColor(wxDC& dc, bool useStyle) {
    int penStyle;

    if (useStyle)
      penStyle = GetPenStyle();
    else
      penStyle = wxSOLID;

    switch(color) {
    case 'y':
      dc.SetPen( wxPen( wxT("yellow"), 1, penStyle) );
      break;
    case 'm':
      dc.SetPen( wxPen( wxT("magenta"), 1, penStyle) );
      break;
    case 'c':
      dc.SetPen( wxPen( wxT("cyan"), 1, penStyle) );
      break;
    case 'r':
      dc.SetPen( wxPen( wxT("red"), 1, penStyle) );
      break;
    case 'g':
      dc.SetPen( wxPen( wxT("green"), 1, penStyle) );
      break;
    case 'b':
      dc.SetPen( wxPen( wxT("blue"), 1, penStyle) );
      break;
    case 'w':
      dc.SetPen( wxPen( wxT("white"), 1, penStyle) );
      break;
    case 'k':
      dc.SetPen( wxPen( wxT("black"), 1, penStyle) );
      break;
    }
    if (useStyle && (line == ' '))
      dc.SetPen( *wxTRANSPARENT_PEN );
  }

  void DataSet2D::GetDataRange(double& xMin, double &xMax, double &yMin, double & yMax) {
    const double *xVals;
    const double *yVals;
    int ptCount;

    xVals = (const double *) x.getDataPointer();
    yVals = (const double *) y.getDataPointer();
    ptCount = x.getLength();

    // Calculate the min and max values in x
    xMin = xVals[0];  
    xMax = xMin;
    for (int i=0;i<ptCount;i++) {
      xMin = (xVals[i] < xMin) ? xVals[i] : xMin;
      xMax = (xVals[i] > xMax) ? xVals[i] : xMax;
    }
    // Calculate the min and max values in y
    yMin = yVals[0];
    yMax = yMin;
    for (int i=0;i<ptCount;i++) {
      yMin = (yVals[i] < yMin) ? yVals[i] : yMin;
      yMax = (yVals[i] > yMax) ? yVals[i] : yMax;
    }
  }

  void DataSet2D::PutSymbol(wxDC &dc, wxCoord xp, wxCoord yp, 
			    char symbol, wxCoord len) {
    wxCoord len2 = (wxCoord) (len / sqrt(2.0));
    switch (symbol) {
    case '.':
      dc.DrawPoint(xp, yp);
      break;
    case 'o':
      dc.DrawCircle(xp, yp, len);
      break;
    case 'x':
      dc.DrawLine(xp - len2, yp - len2, xp + len2 + 1, yp + len2 + 1);
      dc.DrawLine(xp + len2, yp - len2, xp - len2 - 1, yp + len2 + 1);
      break;
    case '+':
      dc.DrawLine(xp - len, yp, xp + len + 1, yp);
      dc.DrawLine(xp, yp - len, xp, yp + len + 1);
      break;
    case '*':
      dc.DrawLine(xp - len, yp, xp + len + 1, yp);
      dc.DrawLine(xp, yp - len, xp, yp + len + 1);
      dc.DrawLine(xp - len2, yp - len2, xp + len2 + 1, yp + len2 + 1);
      dc.DrawLine(xp + len2, yp - len2, xp - len2 - 1, yp + len2 + 1);
      break;
    case 's':
      dc.DrawRectangle(xp - len/2, yp - len/2, len + 1, len + 1);
      break;
    case 'd':
      dc.DrawLine(xp - len, yp, xp, yp - len);
      dc.DrawLine(xp, yp - len, xp + len, yp);
      dc.DrawLine(xp + len, yp, xp, yp + len);
      dc.DrawLine(xp, yp + len, xp - len, yp);
      break;
    case 'v':
      dc.DrawLine(xp - len, yp - len, xp + len, yp - len);
      dc.DrawLine(xp + len, yp - len, xp, yp + len);
      dc.DrawLine(xp, yp + len, xp - len, yp - len);
      break;
    case '^':
      dc.DrawLine(xp - len, yp + len, xp + len, yp + len);
      dc.DrawLine(xp + len, yp + len, xp, yp - len);
      dc.DrawLine(xp, yp - len, xp - len, yp + len);
      break;
    case '<':
      dc.DrawLine(xp + len, yp - len, xp - len, yp);
      dc.DrawLine(xp - len, yp, xp + len, yp + len);
      dc.DrawLine(xp + len, yp + len, xp + len, yp - len);
      break;
    case '>':
      dc.DrawLine(xp - len, yp - len, xp + len, yp);
      dc.DrawLine(xp + len, yp, xp - len, yp + len);
      dc.DrawLine(xp - len, yp + len, xp - len, yp - len);
      break;
    }
  }

  void DataSet2D::DrawMe(wxDC &dc, Axis *xAxis, Axis *yAxis) {
    const double *xVals;
    const double *yVals;
    int ptCount;

    xVals = (const double *) x.getDataPointer();
    yVals = (const double *) y.getDataPointer();
    ptCount = x.getLength();

    SetPenColor(dc, false);
    // Draw the symbols
    for (int i=0;i<ptCount;i++) {
      // Map the data point to a coordinate
      wxCoord xp, yp;
      xp = xAxis->MapPoint(xVals[i]);
      yp = yAxis->MapPoint(yVals[i]);
      PutSymbol(dc, xp, yp, symbol, symbolLength);
    }
    // Plot the lines
    SetPenColor(dc, true);
    wxPoint *ptArray;
    ptArray = new wxPoint[ptCount];
    for (int i=0;i<ptCount;i++)
      ptArray[i] = wxPoint(xAxis->MapPoint(xVals[i]),
			   yAxis->MapPoint(yVals[i]));
    // Clip the line
    dc.DrawLines(ptCount,ptArray);
    delete[] ptArray;
  }
}
