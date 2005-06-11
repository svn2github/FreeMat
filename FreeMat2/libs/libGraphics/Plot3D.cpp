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

#include "Plot3D.hpp"
#include "GraphicsCore.hpp"
#include <math.h>
#include <iostream>
#include <stdio.h>
extern "C" {
#include "trackball.h"
}

namespace FreeMat {

  Plot3D::Plot3D(int width, int height) : XPWidget(NULL,Point2D(width,height)){
    m_width = width;
    m_height = height;
    trackball(quat,0.0,0.0,0.0,0.0);
    double *xp = (double*) Array::allocateArray(FM_DOUBLE, 200);
    double *yp = (double*) Array::allocateArray(FM_DOUBLE, 200);
    double *zp = (double*) Array::allocateArray(FM_DOUBLE, 200);
    int i;
    for (i=0;i<200;i++) {
      double theta;
      double radius;
      zp[i] = i-100.0;
      theta = (i-100.0)/100.0*2*M_PI;
      radius = i;
      xp[i] = radius*cos(theta);
      yp[i] = radius*sin(theta);
    }
    Dimensions dims(2);
    dims[0] = 200;
    dims[1] = 1;
    data.push_back(DataSet3D(Array::Array(FM_DOUBLE,dims,xp),
			     Array::Array(FM_DOUBLE,dims,yp),
			     Array::Array(FM_DOUBLE,dims,zp),
			     'b','o','-'));
    xAxis = new Axis(-100,100,false,Axis_X);
    yAxis = new Axis(-100,100,false,Axis_Y);
    zAxis = new Axis(-100,100,false,Axis_X);
  }

  Plot3D::~Plot3D() {
  }

  void Plot3D::OnMouseDown(int x, int y) {
    beginx = x;
    beginy = y;
  }

  void Plot3D::OnMouseUp(int x, int y) {
  }

  void Plot3D::OnDrag(int x, int y) {
    float spin_quat[4];
    int szx, szy;
    szx = m_width;
    szy = m_height;
    trackball(spin_quat,
	      -(2.0*beginx - szx) / szx / 10,
	      (szy - 2.0*beginy) / szy / 10,
	      -(2.0*x - szx) / szx / 10,
	      (szy - 2.0*y) / szy / 10);
    add_quats( spin_quat, quat, quat );
    Redraw();
  }


  void Plot3D::OnDraw(GraphicsContext &gc) {
    float m[4][4];
    double frm[2][4];
    int i, j;

    gc.SetBackGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("light grey"));
    gc.FillRectangle(Rect2D(0, 0, m_width, m_height));
    build_rotmatrix(m, quat);
    for (i=0;i<2;i++) 
      for (j=0;j<4;j++) 
	frm[i][j] = m[j][i];
    data[0].DrawMe(gc,xAxis,yAxis,zAxis,frm);
  }
}



