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

  Plot3D::Plot3D() : XWindow(VectorWindow){
    trackball(quat,0.0,0.0,0.0,0.0);
    double *xp = Array::allocateArray(FM_DOUBLE, 200);
    double *yp = Array::allocateArray(FM_DOUBLE, 200);
    double *zp = Array::allocateArray(FM_DOUBLE, 200);
    int i;
    for (i=0;i<200;i++) {
      double theta;
      zp[i] = i-100.0;
      theta = (i-100.0)/100.0*2*M_PI;
      xp[i] = 100*cos(theta);
      yp[i] = 100*sin(theta);
    }
    Dimensions dims[2];
    data = DataSet3D(Array::Array(FM_DOUBLE,dims,xp),
		     Array::Array(FM_DOUBLE,dims,yp),
		     Array::Array(FM_DOUBLE,dims,zp),
		     'b','o','-');
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
    szx = getWidth();
    szy = getHeight();
    trackball(spin_quat,
	      -(2.0*beginx - szx) / szx,
	      (szy - 2.0*beginy) / szy,
	      -(2.0*x - szx) / szx,
	      (szy - 2.0*y) / sz.y);
    add_quats( spin_quat, quat, quat );
    dirty = true;
    Refresh(FALSE);
  }

  void Plot3D::OnDraw(GraphicsContext &gc) {
    double m[4][4];
    double frm[2][4];
    int i, j;

    build_rotmatrix(m, quat);
    for (i=0;i<2;i++)
      for (j=0;j<4;j++)
	frm[i][j] = m[i][j];
    data.DrawMe(gc,xAxis,yAxis,zAxis,xform);
  }
}



