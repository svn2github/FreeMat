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
#include "IEEEFP.hpp"
#include "Plot2D.hpp"
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

  void DataSet2D::SetPenColor(GraphicsContext& dc, bool useStyle) {
    LineStyleType penStyle;

    if (useStyle)
      penStyle = UtilityMapLineStyleToType(line);
    else
      penStyle = LINE_SOLID;

    dc.SetLineStyle(penStyle);

    dc.SetForeGroundColor(UtilityMapColorSpecToColor(color));

    if (useStyle && (line == ' '))
      dc.SetLineStyle( LINE_NONE );
  }

  void DataSet2D::GetDataRange(double& xMin, double &xMax, double &yMin, double & yMax) {
    const double *xVals;
    const double *yVals;
    int ptCount;
    int i, j;
    bool firstval;

    xVals = (const double *) x.getDataPointer();
    yVals = (const double *) y.getDataPointer();
    ptCount = x.getLength();

    // Calculate the min and max values in x
    firstval = true;
    for (i=0;i<ptCount;i++) {
      if (IsFinite(xVals[i])) {
	if (firstval) {
	  xMin = xVals[i];
	  xMax = xVals[i];
	  firstval = false;
	} else {
	  xMin = (xVals[i] < xMin) ? xVals[i] : xMin;
	  xMax = (xVals[i] > xMax) ? xVals[i] : xMax;
	}
      }
    }
    if (firstval) {
      xMin = -1;
      xMax = 1;
    }

    // Calculate the min and max values in y
    firstval = true;
    for (i=0;i<ptCount;i++) {
      if (IsFinite(yVals[i])) {
	if (firstval) {
	  yMin = yVals[i];
	  yMax = yVals[i];
	  firstval = false;
	} else {
	  yMin = (yVals[i] < yMin) ? yVals[i] : yMin;
	  yMax = (yVals[i] > yMax) ? yVals[i] : yMax;
	}
      }
    }
    if (firstval) {
      yMin = -1;
      yMax = 1;
    }

    // If the y values are less than eps apart, set the y range to eps
    if (fabs(yMax - yMin) < 1e-16) {
      yMin = (yMax+yMin)/2 - 1e-16;
      yMax = (yMax+yMin)/2 + 1e-16;
    }
    if (ptCount == 1) {
      xMin = xMin - 1;
      xMax = xMax + 1;
    }
    if (fabs(xMax - xMin) < 1e-16) {
      xMin = (xMax+xMin)/2 - 1e-16;
      xMax = (xMax+xMin)/2 + 1e-16;
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
      if (IsFinite(xVals[i]) && IsFinite(yVals[i])) {
	int xp, yp;
	xp = xAxis->MapPoint(xVals[i]);
	yp = yAxis->MapPoint(yVals[i]);
	PutSymbol(dc, xp, yp, symbol, symbolLength);
      }
    }
    // Plot the lines
    SetPenColor(dc, true);
    std::vector<Point2D> pts;
    for (i=0;i<ptCount;i++) {
      if (IsFinite(xVals[i]) && (IsFinite(yVals[i]))) 
	pts.push_back(Point2D(xAxis->MapPoint(xVals[i]),
			      yAxis->MapPoint(yVals[i])));
      else {
	dc.DrawLines(pts);
	pts.clear();
      }
    }
    dc.DrawLines(pts);
  }

  DataSet3D::DataSet3D(Array xarg, Array yarg, Array zarg,
		       char a_color, char a_symbol, char a_line) {
    color = a_color;
    symbol = a_symbol;
    line = a_line;
    x = xarg;
    y = yarg;
    z = zarg;
    symbolLength = 3;
  }

  DataSet3D::~DataSet3D() {
  }

  void DataSet3D::SetPenColor(GraphicsContext& dc, bool useStyle) {
    LineStyleType penStyle;

    if (useStyle)
      penStyle = UtilityMapLineStyleToType(line);
    else
      penStyle = LINE_SOLID;

    dc.SetLineStyle(penStyle);

    dc.SetForeGroundColor(UtilityMapColorSpecToColor(color));

    if (useStyle && (line == ' '))
      dc.SetLineStyle( LINE_NONE );
  }

  void DataSet3D::GetDataRange(double &xMin, double &xMax, 
			       double &yMin, double &yMax,
			       double &zMin, double &zMax) {
    const double *xVals;
    const double *yVals;
    const double *zVals;
    int ptCount;
    int i, j;
    bool firstval;

    xVals = (const double *) x.getDataPointer();
    yVals = (const double *) y.getDataPointer();
    zVals = (const double *) z.getDataPointer();
    ptCount = x.getLength();

    // Calculate the min and max values in x
    firstval = true;
    for (i=0;i<ptCount;i++) {
      if (IsFinite(xVals[i])) {
	if (firstval) {
	  xMin = xVals[i];
	  xMax = xVals[i];
	  firstval = false;
	} else {
	  xMin = (xVals[i] < xMin) ? xVals[i] : xMin;
	  xMax = (xVals[i] > xMax) ? xVals[i] : xMax;
	}
      }
    }
    if (firstval) {
      xMin = -1;
      xMax = 1;
    }

    // Calculate the min and max values in y
    firstval = true;
    for (i=0;i<ptCount;i++) {
      if (IsFinite(yVals[i])) {
	if (firstval) {
	  yMin = yVals[i];
	  yMax = yVals[i];
	  firstval = false;
	} else {
	  yMin = (yVals[i] < yMin) ? yVals[i] : yMin;
	  yMax = (yVals[i] > yMax) ? yVals[i] : yMax;
	}
      }
    }
    if (firstval) {
      yMin = -1;
      yMax = 1;
    }

    // Calculate the min and max values in z
    firstval = true;
    for (i=0;i<ptCount;i++) {
      if (IsFinite(zVals[i])) {
	if (firstval) {
	  zMin = zVals[i];
	  zMax = zVals[i];
	  firstval = false;
	} else {
	  zMin = (zVals[i] < zMin) ? zVals[i] : zMin;
	  zMax = (zVals[i] > zMax) ? zVals[i] : zMax;
	}
      }
    }
    if (firstval) {
      zMin = -1;
      zMax = 1;
    }

    // If the y values are less than eps apart, set the y range to eps
    if (fabs(zMax - zMin) < 1e-16) {
      zMin = (zMax+zMin)/2 - 1e-16;
      zMax = (zMax+zMin)/2 + 1e-16;
    }
    if (fabs(yMax - yMin) < 1e-16) {
      yMin = (yMax+yMin)/2 - 1e-16;
      yMax = (yMax+yMin)/2 + 1e-16;
    }
    if (ptCount == 1) {
      xMin = xMin - 1;
      xMax = xMax + 1;
    }
    if (fabs(xMax - xMin) < 1e-16) {
      xMin = (xMax+xMin)/2 - 1e-16;
      xMax = (xMax+xMin)/2 + 1e-16;
    }
      
  }


  void DataSet3D::DrawMe(GraphicsContext &dc, Axis *xAxis, 
			 Axis *yAxis, Axis *zAxis,
			 double xform[2][4]) {
    const double *xVals;
    const double *yVals;
    const double *zVals;
    int ptCount;
    int i;

    xVals = (const double *) x.getDataPointer();
    yVals = (const double *) y.getDataPointer();
    zVals = (const double *) z.getDataPointer();
    ptCount = x.getLength();

    SetPenColor(dc, false);
    // Draw the symbols
    for (i=0;i<ptCount;i++) {
      // Map the data point to a coordinate
      if (IsFinite(xVals[i]) && IsFinite(yVals[i]) && IsFinite(zVals[i])) {
	int xp, yp, zp;
	xp = xAxis->MapPoint(xVals[i]);
	yp = yAxis->MapPoint(yVals[i]);
	zp = zAxis->MapPoint(zVals[i]);
	int xxp, yyp;
	xxp = (int)(xform[0][0]*xp + xform[0][1]*yp + 
		    xform[0][2]*zp + xform[0][3]);
	yyp = (int)(xform[1][0]*xp + xform[1][1]*yp + 
		    xform[1][2]*zp + xform[1][3]);
	PutSymbol(dc, xp, yp, symbol, symbolLength);
      }
    }
    // Plot the lines
    SetPenColor(dc, true);
    std::vector<Point2D> pts;
    for (i=0;i<ptCount;i++) {
      if (IsFinite(xVals[i]) && IsFinite(yVals[i]) &&
	  IsFinite(yVals[i])) {
	int xp, yp, zp;
	xp = xAxis->MapPoint(xVals[i]);
	yp = yAxis->MapPoint(yVals[i]);
	zp = zAxis->MapPoint(zVals[i]);
	int xxp, yyp;
	xxp = (int)(xform[0][0]*xp + xform[0][1]*yp + 
		    xform[0][2]*zp + xform[0][3]);
	yyp = (int)(xform[1][0]*xp + xform[1][1]*yp + 
		    xform[1][2]*zp + xform[1][3]);
	pts.push_back(Point2D(xxp,yyp));
      }
      else {
	dc.DrawLines(pts);
	pts.clear();
      }
    }
    dc.DrawLines(pts);
  }

}
