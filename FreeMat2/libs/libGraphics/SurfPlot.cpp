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
#include <algorithm>
#include "FL/Fl.H"
extern "C" {
#include "trackball.h"
}

#undef min
#undef max
#ifndef M_PI
#define M_PI 4.0*atan(1.0)
#endif

namespace FreeMat {

  void SurfPlot::SetData(const double*x, const double*y, const double*z, int nx, int ny) {
    xcount = nx;
    ycount = ny;
    if (xvals) delete[] xvals;
    if (yvals) delete[] yvals;
    if (zvals) delete[] zvals;
    xvals = new double[nx];
    yvals = new double[ny];
    zvals = new double[nx*ny];
    // Copy the data into our local arrays
    memcpy(xvals,x,nx*sizeof(double));
    memcpy(yvals,y,ny*sizeof(double));
    memcpy(zvals,z,nx*ny*sizeof(double));
    SurfaceToQuads();
  }

  void SurfPlot::resize(int x, int y, int w, int h) {
    Fl_Widget::resize(x,y,w,h);
    m_width = w;
    m_height = h;
    redraw();    
  }

  void SurfPlot::SurfaceToQuads() {
    UpdateBounds();
    quads.clear();
    // make the surface into quads
    for (int i=0;i<(xcount-1);i++)
      for (int j=0;j<(ycount-1);j++) {
	quad3d t;
	t.pts[0].x = XAxis.Normalize(xvals[i])-0.5;
	t.pts[0].y = YAxis.Normalize(yvals[j])-0.5;
	t.pts[0].z = ZAxis.Normalize(zvals[j+i*ycount])-0.5;
	t.pts[1].x = XAxis.Normalize(xvals[i+1])-0.5;
	t.pts[1].y = YAxis.Normalize(yvals[j])-0.5;
	t.pts[1].z = ZAxis.Normalize(zvals[j+(i+1)*ycount])-0.5;
	t.pts[2].x = XAxis.Normalize(xvals[i+1])-0.5;
	t.pts[2].y = YAxis.Normalize(yvals[j+1])-0.5;
	t.pts[2].z = ZAxis.Normalize(zvals[j+1+(i+1)*ycount])-0.5;
	t.pts[3].x = XAxis.Normalize(xvals[i])-0.5;
	t.pts[3].y = YAxis.Normalize(yvals[j+1])-0.5;
	t.pts[3].z = ZAxis.Normalize(zvals[j+1+i*ycount])-0.5;
	t.meanz = ZAxis.Normalize((zvals[j+i*ycount]+zvals[j+(i+1)*ycount]+
				   zvals[j+1+(i+1)*ycount]+zvals[j+1+i*ycount])/4.0) - 0.5;
	t.color = (t.meanz-z_min)/(z_max-z_min)*255;
	quads.push_back(t);
      }
  }

  void SurfPlot::UpdateBounds() {
    x_min = xvals[0];
    x_max = xvals[0];
    for (int i=0;i<xcount;i++) {
      x_min = std::min(x_min,xvals[i]);
      x_max = std::max(x_max,xvals[i]);
    }
    x_center = (x_min+x_max)/2.0;
    XAxis.SetDataRange(x_min,x_max);

    y_min = yvals[0];
    y_max = yvals[0];
    for (int i=0;i<ycount;i++) {
      y_min = std::min(y_min,yvals[i]);
      y_max = std::max(y_max,yvals[i]);
    }
    y_center = (y_min+y_max)/2.0;
    YAxis.SetDataRange(y_min,y_max);

    z_min = zvals[0];
    z_max = zvals[0];
    for (int i=0;i<xcount*ycount;i++) {
      z_min = std::min(z_min,zvals[i]);
      z_max = std::max(z_max,zvals[i]);
    }
    z_center = (z_min+z_max)/2.0;
    ZAxis.SetDataRange(z_min,z_max);
    
    // Now, we also need the maximum radius.
    max_radius = 0;
    for (int i=0;i<xcount;i++)
      for (int j=0;j<ycount;j++) {
	double diff_x = xvals[i] - x_center;
	double diff_y = yvals[j] - y_center;
	double diff_z = zvals[i*ycount+j] - z_center;
	double diff_r = sqrt(diff_x*diff_x + diff_y*diff_y + diff_z*diff_z);
	max_radius = std::max(max_radius,diff_r);
      }

    max_radius = sqrt(3.0)/2.0;
    x_min = y_min = z_min = -0.5;
    x_max = y_max = z_max = 0.5;
    x_center = y_center = z_center = 0.0;
  }

  SurfPlot::SurfPlot(int width, int height) : PrintableWidget(0,0,width,height){
    m_width = width;
    m_height = height;
    //    trackball(quat,0.0,0.0,0.0,0.0);
    xvals = NULL;
    yvals = NULL;
    zvals = NULL;
    azim = 35;
    elev = -20;
    xlabel = "x-axis";
    ylabel = "y-axis";
    zlabel = "z-axis";
    dragging = false;
    grid = true;
  }

  SurfPlot::~SurfPlot() {
  }

  void SurfPlot::OnMouseDown(int x, int y) {
    beginx = x;
    beginy = y;
    dragging = true;
  }

  void SurfPlot::OnMouseUp(int x, int y) {
    dragging = false;
    redraw();
  }

  void SurfPlot::OnDrag(int x, int y) {
    elev -= (y - beginy);
    azim += (x - beginx);
    elev = (elev + 360) % 360;
    azim = (azim + 360) % 360;
    beginx = x;
    beginy = y;    
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
    } else if (event == FL_RELEASE) {
      OnMouseUp(Fl::event_x(),Fl::event_y());
    }
    return 0;
  }

  void transformPoint(pt3d& src, pt3d& dst, float m[4][4]) {
    dst.x = m[0][0]*src.x + m[1][0]*src.y + m[2][0]*src.z + m[3][0];
    dst.y = m[0][1]*src.x + m[1][1]*src.y + m[2][1]*src.z + m[3][1];
    dst.z = m[0][2]*src.x + m[1][2]*src.y + m[2][2]*src.z + m[3][2]; 
 }

  Point2D SurfPlot::MapPoint(pt3d a) {
    Point2D ret;
    return Point2D((int) (a.x*scalex+offsetx),
		   (int) (a.y*scaley+offsety));
  }

  bool operator<(const quad3d& a, const quad3d& b) {
    return (a.meanz < b.meanz);
  }

  void SurfPlot::SetColormap(double *cmap) {
    colormap = (double*) malloc(sizeof(double)*256*3);
    memcpy(colormap,cmap,sizeof(double)*256*3);
  }


  void SurfPlot::DrawAxis(GraphicsContext &gc, std::string label,
			  pt3d a_start, pt3d a_stop,
			  NewAxis& ref, pt3d unit,
			  bool isZ) {
    gc.SetForeGroundColor(Color("black"));
    gc.DrawLine(MapPoint(a_start),MapPoint(a_stop));
    double tmin, tdelta;
    int tcount;
    int axlen;
    axlen = sqrt(pow(scaley*(a_stop.y-a_start.y),2.0) + 
		 pow(scalex*(a_stop.x-a_start.x),2.0));
    ref.SetAxisLength(axlen); 
    std::vector<double> tics = ref.GetTickLocations();
    std::vector<std::string> labs = ref.GetTickLabels();
    float deltx, delty;
    deltx = unit.x*scalex;
    delty = unit.y*scaley;
    float rad;
	rad = sqrt(deltx*deltx+delty*delty);
    rad = std::max(1e-6f,rad);
    deltx /= rad;
    delty /= rad;
    int maxlabellength = 0;
    for (int i=0;i<tics.size();i++) {
      double t;
      t = ref.Normalize(tics[i]);
      Point2D ex = gc.GetTextExtent(labs[i]);
      maxlabellength = std::max(maxlabellength,ex.x);
      pt3d tmp;
      tmp.x = a_start.x + t*(a_stop.x-a_start.x);
      tmp.y = a_start.y + t*(a_stop.y-a_start.y);
      Point2D h_start = MapPoint(tmp);
      Point2D h_stop, h_text;
      if ((deltx != 0) || (delty != 0)) {
	h_stop.x = h_start.x + deltx*10.0;
	h_stop.y = h_start.y + delty*10.0;
	gc.DrawLine(h_start,h_stop);
	h_text.x = h_start.x + deltx*13.0;
	h_text.y = h_start.y + delty*13.0;
	if (delty == 0) {
	  gc.DrawTextStringAligned(labs[i],h_text,
				   LR_RIGHT,
				   TB_BOTTOM);
	} else {
	  if (deltx>0)
	    gc.DrawTextStringAligned(labs[i],h_text,
				     LR_LEFT,
				     TB_CENTER);
	  else
	    gc.DrawTextStringAligned(labs[i],h_text,
				     LR_RIGHT,
				     TB_CENTER);
	}
      }
    }
    if (!label.empty()) {
      pt3d lab;
      lab.x = (a_start.x + a_stop.x)/2;
      lab.y = (a_start.y + a_stop.y)/2;
      Point2D h_mid = MapPoint(lab);
      h_mid.x += deltx*(maxlabellength+25);
      h_mid.y += delty*(maxlabellength+25);
      if (isZ)
	gc.DrawTextStringAligned(label,h_mid,
				 LR_CENTER,
				 TB_CENTER,
				 ORIENT_90);
      else {
	if (deltx>0)
	  gc.DrawTextStringAligned(label,h_mid,
				   LR_LEFT,
				   TB_CENTER);
	else
	  gc.DrawTextStringAligned(label,h_mid,
				   LR_RIGHT,
				   TB_CENTER);
      }
    }
  }

  void SurfPlot::DrawAxisTest(GraphicsContext &gc, std::string label,
			      pt3d q[8], int a_start[4], int a_stop[4],
			      int m_height, NewAxis& ref, pt3d units[4],
			      bool Zaxis) {
    float centers_x[4];
    float centers_y[4];
    
    // Calculate the axis centers
    for (int i=0;i<4;i++) {
      centers_x[i] = (q[a_start[i]].x+q[a_stop[i]].x)/2;
      centers_y[i] = (q[a_start[i]].y+q[a_stop[i]].y)/2;
    }
    if (Zaxis) {
      // The Z axis should be leftmost
      float cost = 1e100;
      int optindex = 0;
      for (int i=0;i<4;i++)
	if (cost > centers_x[i]) {
	  cost = centers_x[i];
	  optindex = i;
	}
      DrawAxis(gc,label,q[a_start[optindex]],q[a_stop[optindex]],
	       ref,units[optindex],true);
    } else {
      // Other axes should be bottom most
      float cost = -1e100;
      int optindex = 0;
      for (int i=0;i<4;i++)
	if (cost < centers_y[i]) {
	  cost = centers_y[i];
	  optindex = i;
	}
      DrawAxis(gc,label,q[a_start[optindex]],q[a_stop[optindex]],
	       ref,units[optindex],false);
    }
  }

  static pt3d crossprod(pt3d v1, pt3d v2) {
    pt3d temp;
    temp.x = (v1.y * v2.z) - (v1.z * v2.y);
    temp.y = (v1.z * v2.x) - (v1.x * v2.z);
    temp.z = (v1.x * v2.y) - (v1.y * v2.x);
    return temp;
  }
  
  void SurfPlot::DrawGridLines(GraphicsContext &gc, float m[4][4],
			       pt3d delta, pt3d start, pt3d stop,
			       NewAxis &ref) {
    std::vector<double> tics = ref.GetTickLocations();
    gc.SetForeGroundColor(Color("light grey"));
    gc.SetLineStyle(LINE_DOTTED);
    for (int i=0;i<tics.size();i++) {
      pt3d tstart, tstop;
      double tloc;
      tloc = ref.Normalize(tics[i])-0.5;
      tstart = start;
      tstop = stop;
      tstart.x += delta.x*tloc; tstop.x += delta.x*tloc;
      tstart.y += delta.y*tloc; tstop.y += delta.y*tloc;
      tstart.z += delta.z*tloc; tstop.z += delta.z*tloc;
      gc.DrawLine(XformPoint(tstart,m),XformPoint(tstop,m));
    }
  }

  void SurfPlot::DrawGrid(GraphicsContext &gc, float m[4][4]) {
    pt3d delta, start, stop;
    // XY plane, Zneg - x
    delta.x = 1; delta.y = 0; delta.z = 0;
    start.x = 0; start.y = -0.5; start.z = -0.5;
    stop.x = 0; stop.y = 0.5; stop.z = -0.5;
    if (m[2][2] > 0)
      DrawGridLines(gc,m,delta,start,stop,XAxis);
    // XY plane, Zpos - x
    delta.x = 1; delta.y = 0; delta.z = 0;
    start.x = 0; start.y = -0.5; start.z = 0.5;
    stop.x = 0; stop.y = 0.5; stop.z = 0.5;
    if (m[2][2] < 0)
        DrawGridLines(gc,m,delta,start,stop,XAxis);
    // XZ plane, Yneg - x
    delta.x = 1; delta.y = 0; delta.z = 0;
    start.x = 0; start.y = -0.5; start.z = -0.5;
    stop.x = 0; stop.y = -0.5; stop.z = 0.5;
    if (m[1][2] > 0)
      DrawGridLines(gc,m,delta,start,stop,XAxis);
    // XZ plane, Ypos - x
    delta.x = 1; delta.y = 0; delta.z = 0;
    start.x = 0; start.y = 0.5; start.z = -0.5;
    stop.x = 0; stop.y = 0.5; stop.z = 0.5;
    if (m[1][2] < 0)
      DrawGridLines(gc,m,delta,start,stop,XAxis);
    // YZ plane, Xneg - y
    delta.x = 0; delta.y = 1; delta.z = 0;
    start.x = -0.5; start.y = 0; start.z = -0.5;
    stop.x = -0.5; stop.y = 0; stop.z = 0.5;
    if (m[0][2] > 0)
      DrawGridLines(gc,m,delta,start,stop,YAxis);
    // YZ plane, Xpos - y
    delta.x = 0; delta.y = 1; delta.z = 0;
    start.x = 0.5; start.y = 0; start.z = -0.5;
    stop.x = 0.5; stop.y = 0; stop.z = 0.5;
    if (m[0][2] < 0)
      DrawGridLines(gc,m,delta,start,stop,YAxis);
    // YX plane, Zneg - y
    delta.x = 0; delta.y = 1; delta.z = 0;
    start.x = -0.5; start.y = 0; start.z = -0.5;
    stop.x = 0.5; stop.y = 0; stop.z = -0.5;
    if (m[2][2] > 0)
      DrawGridLines(gc,m,delta,start,stop,YAxis);
    // YX plane, Zpos - y
    delta.x = 0; delta.y = 1; delta.z = 0;
    start.x = -0.5; start.y = 0; start.z = 0.5;
    stop.x = 0.5; stop.y = 0; stop.z = 0.5;
    if (m[2][2] < 0)
      DrawGridLines(gc,m,delta,start,stop,YAxis);
    // ZX plane, Yneg - z
    delta.x = 0; delta.y = 0; delta.z = 1;
    start.x = -0.5; start.y = -0.5; start.z = 0;
    stop.x = 0.5; stop.y = -0.5; stop.z = 0;
    if (m[1][2] > 0)
      DrawGridLines(gc,m,delta,start,stop,ZAxis);
    // XZ plane, Ypos - x
    delta.x = 0; delta.y = 0; delta.z = 1;
    start.x = -0.5; start.y = 0.5; start.z = 0;
    stop.x = 0.5; stop.y = 0.5; stop.z = 0;
    if (m[1][2] < 0)
      DrawGridLines(gc,m,delta,start,stop,ZAxis);
    // YZ plane, Xneg - y
    delta.x = 0; delta.y = 0; delta.z = 1;
    start.x = -0.5; start.y = -0.5; start.z = 0;
    stop.x = -0.5; stop.y = 0.5; stop.z = 0;
    if (m[0][2] > 0)
      DrawGridLines(gc,m,delta,start,stop,ZAxis);
    // YZ plane, Xpos - y
    delta.x = 0; delta.y = 0; delta.z = 1;
    start.x = 0.5; start.y = -0.5; start.z = 0;
    stop.x = 0.5; stop.y = 0.5; stop.z = 0;
    if (m[0][2] < 0)
      DrawGridLines(gc,m,delta,start,stop,ZAxis);
  }

  void SurfPlot::DrawAxes(GraphicsContext &gc, float m[4][4]) {
    // The 8 corners of the cube
    int a_start_x[4] = {0,2,6,4};
    int a_stop_x[4] = {1,3,7,5};
    pt3d tmp;
    pt3d units_x[4];
    tmp.z = 0; tmp.x = 0; tmp.y = -1;
    transformPoint(tmp,units_x[0],m);
    transformPoint(tmp,units_x[2],m);
    tmp.z = 0; tmp.x = 0; tmp.y = 1;
    transformPoint(tmp,units_x[1],m);
    transformPoint(tmp,units_x[3],m);
    int a_start_y[4] = {1,5,0,4};
    int a_stop_y[4] = {2,6,3,7};
    pt3d units_y[4];
    tmp.z = 0; tmp.x = 1; tmp.y = 0;
    transformPoint(tmp,units_y[0],m);
    transformPoint(tmp,units_y[1],m);
    tmp.z = 0; tmp.x = -1; tmp.y = 0;
    transformPoint(tmp,units_y[2],m);
    transformPoint(tmp,units_y[3],m);
    int a_start_z[4] = {0,1,2,3};
    int a_stop_z[4] = {7,6,5,4};
    pt3d units_z[4];
    units_z[0].x = -1; units_z[0].y = 0; units_z[0].z = 0;
    units_z[1].x = -1; units_z[1].y = 0; units_z[1].z = 0;
    units_z[2].x = -1; units_z[2].y = 0; units_z[2].z = 0;
    units_z[3].x = -1; units_z[3].y = 0; units_z[3].z = 0;

    pt3d p[8], q[8];
    p[0].x = x_min-x_center; p[0].y = y_min-y_center; p[0].z = z_min-z_center;
    p[1].x = x_max-x_center; p[1].y = y_min-y_center; p[1].z = z_min-z_center;
    p[2].x = x_max-x_center; p[2].y = y_max-y_center; p[2].z = z_min-z_center;
    p[3].x = x_min-x_center; p[3].y = y_max-y_center; p[3].z = z_min-z_center;
    p[4].x = x_min-x_center; p[4].y = y_max-y_center; p[4].z = z_max-z_center;
    p[5].x = x_max-x_center; p[5].y = y_max-y_center; p[5].z = z_max-z_center;
    p[6].x = x_max-x_center; p[6].y = y_min-y_center; p[6].z = z_max-z_center;
    p[7].x = x_min-x_center; p[7].y = y_min-y_center; p[7].z = z_max-z_center;
    // Transform the corners by the rotation matrix
    for (int i=0;i<8;i++)
      transformPoint(p[i],q[i],m);

    // Draw the box behind the data
    gc.SetForeGroundColor(Color("black"));
    gc.SetLineStyle(LINE_SOLID);

    // To do this, we test each axis case,
    DrawAxisTest(gc,xlabel,q,a_start_x,a_stop_x,m_height,XAxis,units_x,false);
    DrawAxisTest(gc,ylabel,q,a_start_y,a_stop_y,m_height,YAxis,units_y,false);
    DrawAxisTest(gc,zlabel,q,a_start_z,a_stop_z,m_height,ZAxis,units_z,true);
  }

  void SurfPlot::DrawPanel(GraphicsContext &gc, pt3d p[4], float m[4][4], bool fill) {
    Point2D x[4];
    for (int i=0;i<4;i++) 
      x[i] = XformPoint(p[i],m);
    if (fill)
      gc.FillQuad(x[0],x[1],x[2],x[3]);
    else
      gc.DrawQuad(x[0],x[1],x[2],x[3]);
  }

  void SurfPlot::DrawCube(GraphicsContext &gc, float m[4][4], bool fill) {
    pt3d p[4];
    if (fill)
      gc.SetForeGroundColor(Color("white"));
    else
      gc.SetForeGroundColor(Color("black"));
    // zmin panel
    p[0].x = -0.5; p[0].y = -0.5; p[0].z = -0.5;
    p[1].x = -0.5; p[1].y = 0.5;  p[1].z = -0.5;
    p[2].x = 0.5;  p[2].y = 0.5;  p[2].z = -0.5;
    p[3].x = 0.5;  p[3].y = -0.5; p[3].z = -0.5;
    DrawPanel(gc,p,m,fill);
    // zmax panel
    p[0].x = -0.5; p[0].y = -0.5; p[0].z = 0.5;
    p[1].x = -0.5; p[1].y = 0.5;  p[1].z = 0.5;
    p[2].x = 0.5;  p[2].y = 0.5;  p[2].z = 0.5;
    p[3].x = 0.5;  p[3].y = -0.5; p[3].z = 0.5;
    DrawPanel(gc,p,m,fill);
    // xmin panel
    p[0].x = -0.5; p[0].y = -0.5; p[0].z = -0.5;
    p[1].x = -0.5; p[1].y = 0.5;  p[1].z = -0.5;
    p[2].x = -0.5; p[2].y = 0.5;  p[2].z = 0.5;
    p[3].x = -0.5; p[3].y = -0.5;  p[3].z = 0.5;
    DrawPanel(gc,p,m,fill);
    // xmax panel
    p[0].x = 0.5; p[0].y = -0.5; p[0].z = -0.5;
    p[1].x = 0.5; p[1].y = 0.5;  p[1].z = -0.5;
    p[2].x = 0.5; p[2].y = 0.5;  p[2].z = 0.5;
    p[3].x = 0.5; p[3].y = -0.5;  p[3].z = 0.5;
    DrawPanel(gc,p,m,fill);
    // ymin panel
    p[0].x = -0.5; p[0].y = -0.5; p[0].z = -0.5;
    p[1].x = 0.5;  p[1].y = -0.5; p[1].z = -0.5;
    p[2].x = 0.5;  p[2].y = -0.5; p[2].z = 0.5;
    p[3].x = -0.5; p[3].y = -0.5; p[3].z = 0.5;
    DrawPanel(gc,p,m,fill);
    // ymax panel
    p[0].x = -0.5; p[0].y = 0.5; p[0].z = -0.5;
    p[1].x = 0.5;  p[1].y = 0.5; p[1].z = -0.5;
    p[2].x = 0.5;  p[2].y = 0.5; p[2].z = 0.5;
    p[3].x = -0.5; p[3].y = 0.5; p[3].z = 0.5;
    DrawPanel(gc,p,m,fill);
  }
  
  Point2D SurfPlot::XformPoint(pt3d x, float m[4][4]) {
    pt3d y;
    transformPoint(x,y,m);
    return MapPoint(y);
  }
  
  void SurfPlot::OnDraw(GraphicsContext &gc) {
    float m[4][4];
    double frm[4][4];
    int i, j;

    gc.SetFont(12);
    gc.SetBackGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("light grey"));
    gc.FillRectangle(Rect2D(0, 0, m_width, m_height));

    double cosaz, sinaz;
    double cosel, sinel;
    cosaz = cos((90+azim)*M_PI/180.0);
    sinaz = sin((90+azim)*M_PI/180.0);
    cosel = cos((elev-180)*M_PI/180.0);
    sinel = sin((elev-180)*M_PI/180.0);
    m[0][0] = cosaz;
    m[0][1] = sinaz;
    m[0][2] = 0;
    m[1][0] = -sinel*sinaz;
    m[1][1] = sinel*cosaz;
    m[1][2] = cosel;
    m[2][0] = cosel*sinaz;
    m[2][1] = -cosel*cosaz;
    m[2][2] = sinel;
    for (i=0;i<4;i++) {
      m[i][3] = 0;
      m[3][i] = 0;
    }
    m[3][3] = 1;

    for (i=0;i<4;i++)
      for (j=0;j<4;j++)
	frm[i][j] = m[j][i];

    for (i=0;i<4;i++)
      for (j=0;j<4;j++)
	m[i][j] = frm[i][j];

    std::vector<quad3d> tquads;
    // Transform the quads
    for (i=0;i<quads.size();i++) {
      quad3d s;
      transformPoint(quads[i].pts[0],s.pts[0],m);
      transformPoint(quads[i].pts[1],s.pts[1],m);
      transformPoint(quads[i].pts[2],s.pts[2],m);
      transformPoint(quads[i].pts[3],s.pts[3],m);
      s.meanz = (s.pts[0].z+s.pts[1].z+s.pts[2].z+s.pts[3].z)/4.0;
      s.color = quads[i].color;
      tquads.push_back(s);
    }
    
    // Sort the transformed quads...
    std::sort(tquads.begin(),tquads.end());
    // Calculate the scale and translate parameters
    // Calculate the largest axis label (x & y)
    Point2D ex, ey;
    ex = gc.GetTextExtent(xlabel);
    ey = gc.GetTextExtent(ylabel);
    int maxlen = std::max(ex.x,ey.x);
    scalex = (m_width-50-maxlen)/(2*max_radius);
    scaley = (m_height-50-maxlen)/(2*max_radius);
    offsetx = m_width/2;
    offsety = m_height/2;

    // White out the box
    if (dragging) {
      DrawCube(gc,m,true);
      DrawCube(gc,m,false);
      return;
    } else {
      DrawCube(gc,m,true);
    }
    
    DrawGrid(gc,m);
    DrawAxes(gc,m);
    
    // Draw them to the screen...
    for (i=0;i<tquads.size();i++) {
      gc.SetForeGroundColor(Color("white"));
      gc.SetForeGroundColor(Color(255*colormap[tquads[i].color],
				  255*colormap[tquads[i].color+256],
				  255*colormap[tquads[i].color+512]));
      gc.FillQuad(MapPoint(tquads[i].pts[0]),
 		  MapPoint(tquads[i].pts[1]),
 		  MapPoint(tquads[i].pts[2]),
 		  MapPoint(tquads[i].pts[3]));
      //      gc.SetForeGroundColor(Color("black"));
      gc.DrawQuad(MapPoint(tquads[i].pts[0]),
 		  MapPoint(tquads[i].pts[1]),
 		  MapPoint(tquads[i].pts[2]),
 		  MapPoint(tquads[i].pts[3]));
    }

  }
}



