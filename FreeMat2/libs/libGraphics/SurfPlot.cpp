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

#include "SurfPlot.hpp"
#include "RGBImage.hpp"
#include "GraphicsCore.hpp"
#include "FLTKGC.hpp"
#include <math.h>
#include <iostream>
#include <stdio.h>
#include "FL/Fl.H"
extern "C" {
#include "trackball.h"
}

namespace FreeMat {

  SurfPlot::SurfPlot(int width, int height) : PrintableWidget(0,0,width,height){
    m_width = width;
    m_height = height;
    trackball(quat,0.0,0.0,0.0,0.0);
    int i, j, n, P;
    P = 21;
    xvals = (double*) malloc(sizeof(double)*P);
    yvals = (double*) malloc(sizeof(double)*P);
    zvals = (double*) malloc(sizeof(double)*P*P);
    for (n=0;n<P;n++) {
      xvals[n] = (n-(P-1.0)/2)/2.0;
      yvals[n] = (n-(P-1.0)/2)/2.0;
    }
    for (i=0;i<P;i++) 
      for (j=0;j<P;j++) {
	double r;
	r = sqrt(xvals[i]*xvals[i]+yvals[j]*yvals[j]);
	zvals[i+j*P] = sin(r);
      }
    // make the surface into quads
    for (i=0;i<(P-1);i++)
      for (j=0;j<(P-1);j++) {
	quad3d t;
	t.pts[0].x = xvals[i];
	t.pts[0].y = yvals[j];
	t.pts[0].z = zvals[i+j*P];
	t.pts[1].x = xvals[i+1];
	t.pts[1].y = yvals[j];
	t.pts[1].z = zvals[i+1+j*P];
	t.pts[2].x = xvals[i+1];
	t.pts[2].y = yvals[j+1];
	t.pts[2].z = zvals[i+1+(j+1)*P];
	t.pts[3].x = xvals[i];
	t.pts[3].y = yvals[j+1];
	t.pts[3].z = zvals[i+(j+1)*P];
	quads.push_back(t);
      }
  }

  SurfPlot::~SurfPlot() {
  }

  void SurfPlot::OnMouseDown(int x, int y) {
    beginx = x;
    beginy = y;
  }

  void SurfPlot::OnMouseUp(int x, int y) {
  }

  void SurfPlot::OnDrag(int x, int y) {
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
    redraw();
  }

  void SurfPlot::draw() {
    FLTKGC gc(w(),h());
    OnDraw(gc);
  }

  int SurfPlot::handle(int event) {
    if (event == FL_PUSH) {
      OnMouseDown(Fl::event_x(),Fl::event_y());
      return 1;
    } else if (event == FL_DRAG) {
      OnDrag(Fl::event_x(),Fl::event_y());
      return 1;
    }
    return 0;
  }

  void transformPoint(pt3d& src, pt3d& dst, float m[4][4]) {
    dst.x = m[0][0]*src.x + m[0][1]*src.y + m[0][2]*src.z + m[0][3];
    dst.y = m[1][0]*src.x + m[1][1]*src.y + m[1][2]*src.z + m[1][3];
    dst.z = m[2][0]*src.x + m[2][1]*src.y + m[2][2]*src.z + m[2][3];
  }

  bool operator<(const quad3d& a, const quad3d& b) {
    return (a.meanz < b.meanz);
  }
  
  void SurfPlot::OnDraw(GraphicsContext &gc) {
    float m[4][4];
    double frm[2][4];
    int i, j;

    gc.SetBackGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("white"));
    gc.FillRectangle(Rect2D(0, 0, m_width, m_height));
    build_rotmatrix(m, quat);
    std::vector<quad3d> tquads;
    // Transform the quads
    for (i=0;i<quads.size();i++) {
      quad3d s;
      transformPoint(quads[i].pts[0],s.pts[0],m);
      transformPoint(quads[i].pts[1],s.pts[1],m);
      transformPoint(quads[i].pts[2],s.pts[2],m);
      transformPoint(quads[i].pts[3],s.pts[3],m);
      s.meanz = (s.pts[0].z+s.pts[1].z+s.pts[2].z+s.pts[3].z)/4.0;
      tquads.push_back(s);
    }
    // Sort the transformed quads...
    std::sort(tquads.begin(),tquads.end());
    // Draw them to the screen...
    for (i=0;i<tquads.size();i++) {
      gc.SetForeGroundColor(Color("white"));
      gc.FillQuad(Point2D(tquads[i].pts[0].x*20+300,tquads[i].pts[0].y*20+300),
		  Point2D(tquads[i].pts[1].x*20+300,tquads[i].pts[1].y*20+300),
		  Point2D(tquads[i].pts[2].x*20+300,tquads[i].pts[2].y*20+300),
		  Point2D(tquads[i].pts[3].x*20+300,tquads[i].pts[3].y*20+300));
      gc.SetForeGroundColor(Color("black"));
      gc.DrawQuad(Point2D(tquads[i].pts[0].x*20+300,tquads[i].pts[0].y*20+300),
		  Point2D(tquads[i].pts[1].x*20+300,tquads[i].pts[1].y*20+300),
		  Point2D(tquads[i].pts[2].x*20+300,tquads[i].pts[2].y*20+300),
		  Point2D(tquads[i].pts[3].x*20+300,tquads[i].pts[3].y*20+300));
    }
//     for (i=0;i<2;i++) 
//       for (j=0;j<4;j++) 
// 	frm[i][j] = m[j][i];
//     data[0].DrawMe(gc,xAxis,yAxis,zAxis,frm);
  }
}



