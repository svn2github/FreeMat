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

namespace FreeMat {

  typedef struct {
    float x;
    float y;
    int ptnum;
  } Point;

  bool operator<(const Point& a, const Point& b) {
    return ((a.x < b.x) || ((a.x == b.x) && (a.y < b.y)));
  }
  
  // Copyright 2001, softSurfer (www.softsurfer.com)
  // This code may be freely used and modified for any purpose
  // providing that this copyright notice is included with it.
  // SoftSurfer makes no warranty for this code, and cannot be held
  // liable for any real or imagined damage resulting from its use.
  // Users of this code must verify correctness for their application.
  // Assume that a class is already given for the object:
  // Point with coordinates {float x, y;}
  //===================================================================
  // isLeft(): tests if a point is Left|On|Right of an infinite line.
  // Input: three points P0, P1, and P2
  // Return: >0 for P2 left of the line through P0 and P1
  // =0 for P2 on the line
  // <0 for P2 right of the line
  // See: the January 2001 Algorithm on Area of Triangles
  inline float isLeft(Point P0, Point P1, Point P2 ) {
    return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
  }
  //===================================================================
  // chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
  // Input: P[] = an array of 2D points 
  // presorted by increasing x- and y-coordinates
  // n = the number of points in P[]
  // Output: H[] = an array of the convex hull vertices (max is n)
  // Return: the number of points in H[]
  int chainHull_2D( Point* P, int n, Point* H ) {
    // the output array H[] will be used as the stack
    int bot=0, top=(-1); // indices for bottom and top of the stack
    int i; // array scan index
    
    // Get the indices of points with min x-coord and min|max y-coord
    int minmin = 0, minmax;
    float xmin = P[0].x;
    for (i=1; i<n; i++)
      if (P[i].x != xmin) break;
    minmax = i-1;
    if (minmax == n-1) { // degenerate case: all x-coords == xmin
      H[++top] = P[minmin];
      if (P[minmax].y != P[minmin].y) // a nontrivial segment
	H[++top] = P[minmax];
      H[++top] = P[minmin]; // add polygon endpoint
      return top+1;
    }

    // Get the indices of points with max x-coord and min|max y-coord
    int maxmin, maxmax = n-1;
    float xmax = P[n-1].x;
    for (i=n-2; i>=0; i--)
      if (P[i].x != xmax) break;
    maxmin = i+1;
    
    // Compute the lower hull on the stack H
    H[++top] = P[minmin]; // push minmin point onto stack
    i = minmax;
    while (++i <= maxmin) {
      // the lower line joins P[minmin] with P[maxmin]
      if (isLeft( P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin)
	continue; // ignore P[i] above or on the lower line
      while (top > 0) { // there are at least 2 points on the stack
	// test if P[i] is left of the line at the stack top
	if (isLeft( H[top-1], H[top], P[i]) > 0)
	  break; // P[i] is a new hull vertex
	else
	  top--; // pop top point off stack
      }
      H[++top] = P[i]; // push P[i] onto stack
    }
    // Next, compute the upper hull on the stack H above the bottom hull
    if (maxmax != maxmin) // if distinct xmax points
      H[++top] = P[maxmax]; // push maxmax point onto stack
    bot = top; // the bottom point of the upper hull stack
    i = maxmin;
    while (--i >= minmax) {
      // the upper line joins P[maxmax] with P[minmax]
      if (isLeft( P[maxmax], P[minmax], P[i]) >= 0 && i > minmax)
	continue; // ignore P[i] below or on the upper line
      while (top > bot) { // at least 2 points on the upper stack
	// test if P[i] is left of the line at the stack top
	if (isLeft( H[top-1], H[top], P[i]) > 0)
	  break; // P[i] is a new hull vertex
	else
	  top--; // pop top point off stack
      }
      H[++top] = P[i]; // push P[i] onto stack
    }
    if (minmax != minmin)
      H[++top] = P[minmin]; // push joining endpoint onto stack
    return top+1;
  }

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
	t.pts[0].x = xvals[i]-x_center;
	t.pts[0].y = yvals[j]-y_center;
	t.pts[0].z = zvals[j+i*ycount]-z_center;
	t.pts[1].x = xvals[i+1]-x_center;
	t.pts[1].y = yvals[j]-y_center;
	t.pts[1].z = zvals[j+(i+1)*ycount]-z_center;
	t.pts[2].x = xvals[i+1]-x_center;
	t.pts[2].y = yvals[j+1]-y_center;
	t.pts[2].z = zvals[j+1+(i+1)*ycount]-z_center;
	t.pts[3].x = xvals[i]-x_center;
	t.pts[3].y = yvals[j+1]-y_center;
	t.pts[3].z = zvals[j+1+i*ycount]-z_center;
	t.meanz = (zvals[j+i*ycount]+zvals[j+(i+1)*ycount]+zvals[j+1+(i+1)*ycount]+zvals[j+1+i*ycount])/4.0;
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
    YAxis.SetDataRange(x_min,x_max);

    z_min = zvals[0];
    z_max = zvals[0];
    for (int i=0;i<xcount*ycount;i++) {
      z_min = std::min(z_min,zvals[i]);
      z_max = std::max(z_max,zvals[i]);
    }
    z_center = (z_min+z_max)/2.0;
    ZAxis.SetDataRange(x_min,x_max);
    
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
  }

  SurfPlot::SurfPlot(int width, int height) : PrintableWidget(0,0,width,height){
    m_width = width;
    m_height = height;
    //    trackball(quat,0.0,0.0,0.0,0.0);
    xvals = NULL;
    yvals = NULL;
    zvals = NULL;
    azim = 0;
    elev = 0;
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
//     float spin_quat[4];
//     int szx, szy;
//     szx = m_width;
//     szy = m_height;
//     trackball(spin_quat,
// 	      (2.0*beginx - szx) / szx / 10,
// 	      -(szy - 2.0*beginy) / szy / 10,
// 	      (2.0*x - szx) / szx / 10,
// 	      -(szy - 2.0*y) / szy / 10);
//     add_quats( spin_quat, quat, quat );
    elev += (y - beginy)/10;
    azim += (x - beginx)/10;
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

  bool OnHull(int hull_count, Point* chull, int ndx) {
    bool found;
    int i;
    i = 0;
    found = false;
    while (!found && i < hull_count) {
      found = chull[i].ptnum == ndx;
      i++;
    }
    return found;
  }
  
  Point FindCHullPoint(int hull_count, Point* chull, int ndx) {
    for (int i=0;i<hull_count;i++)
      if (chull[i].ptnum == ndx)
	return chull[i];
    throw Exception("Unexpected error processing convex hull!");
  }

  void SurfPlot::DrawAxis(GraphicsContext &gc, const char *label,
			  pt3d a_start, pt3d a_stop,
			  NewAxis& ref, pt3d unit) {
    gc.SetForeGroundColor(Color("black"));
    gc.DrawLine(MapPoint(a_start),MapPoint(a_stop));
    gc.SetFont(12);
    double tmin, tdelta;
    int tcount;
    int axlen;
    axlen = sqrt(pow(scaley*(a_stop.y-a_start.y),2.0) + 
		 pow(scalex*(a_stop.x-a_start.x),2.0));
    ref.SetAxisLength(axlen); 
    ref.GetIntervals(tmin,tdelta,tcount);
    for (int i=0;i<tcount;i++) {
      Point2D h_start, h_stop;
      pt3d tmp;
      tmp.x = a_start.x + i/((double) tcount-1)*(a_stop.x-a_start.x);
      tmp.y = a_start.y + i/((double) tcount-1)*(a_stop.y-a_start.y);
      h_start = MapPoint(tmp);
      float deltx, delty;
      deltx = unit.x*scalex;
      delty = unit.y*scaley;
      if ((deltx != 0) || (delty != 0)) {
	h_stop.x = h_start.x + deltx*10.0/sqrt(deltx*deltx+delty*delty);
	h_stop.y = h_start.y + delty*10.0/sqrt(deltx*deltx+delty*delty);
	gc.DrawLine(h_start,h_stop);
      }
    }
  }

  void SurfPlot::DrawAxisTest(GraphicsContext &gc, const char *label,
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
	       ref,units[optindex]);
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
	       ref,units[optindex]);
    }
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
    if (0) {
      if ((q[0].z+q[1].z) < 0) gc.DrawLine(MapPoint(q[0]),MapPoint(q[1]));
      if ((q[1].z+q[2].z) < 0) gc.DrawLine(MapPoint(q[1]),MapPoint(q[2]));
      if ((q[2].z+q[3].z) < 0) gc.DrawLine(MapPoint(q[2]),MapPoint(q[3]));
      if ((q[3].z+q[0].z) < 0) gc.DrawLine(MapPoint(q[3]),MapPoint(q[0]));
      if ((q[3].z+q[4].z) < 0) gc.DrawLine(MapPoint(q[3]),MapPoint(q[4]));
      if ((q[4].z+q[5].z) < 0) gc.DrawLine(MapPoint(q[4]),MapPoint(q[5]));
      if ((q[5].z+q[6].z) < 0) gc.DrawLine(MapPoint(q[5]),MapPoint(q[6]));
      if ((q[6].z+q[7].z) < 0) gc.DrawLine(MapPoint(q[6]),MapPoint(q[7]));
      if ((q[7].z+q[4].z) < 0) gc.DrawLine(MapPoint(q[7]),MapPoint(q[4]));
      if ((q[7].z+q[0].z) < 0) gc.DrawLine(MapPoint(q[7]),MapPoint(q[0]));
      if ((q[1].z+q[6].z) < 0) gc.DrawLine(MapPoint(q[1]),MapPoint(q[6]));
      if ((q[5].z+q[2].z) < 0) gc.DrawLine(MapPoint(q[5]),MapPoint(q[2]));
    }
    // To do this, we test each axis case,
    DrawAxisTest(gc,"xaxis",q,a_start_x,a_stop_x,m_height,XAxis,units_x,false);
    DrawAxisTest(gc,"yaxis",q,a_start_y,a_stop_y,m_height,YAxis,units_y,false);
    DrawAxisTest(gc,"zaxis",q,a_start_z,a_stop_z,m_height,ZAxis,units_z,true);
  }
  
  void SurfPlot::OnDraw(GraphicsContext &gc) {
    float m[4][4];
    double frm[4][4];
    int i, j;

    gc.SetBackGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("light grey"));
    gc.SetForeGroundColor(Color("white"));
    gc.FillRectangle(Rect2D(0, 0, m_width, m_height));

    double cosaz, sinaz;
    double cosel, sinel;
    cosaz = cos(azim*M_PI/180.0);
    sinaz = sin(azim*M_PI/180.0);
    cosel = cos(elev*M_PI/180.0);
    sinel = sin(elev*M_PI/180.0);
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
    //    build_rotmatrix(m, quat);

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
    scalex = (m_width-20)/(2*max_radius);
    scaley = (m_height-20)/(2*max_radius);
    offsetx = m_width/2;
    offsety = m_height/2;

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

    // Draw the box in front of the data
    #if 0
    gc.SetForeGroundColor(Color("black"));
    if ((q1.z+q2.z) > 0) gc.DrawLine(MapPoint(q1),MapPoint(q2));
    if ((q2.z+q3.z) > 0) gc.DrawLine(MapPoint(q2),MapPoint(q3));
    if ((q3.z+q4.z) > 0) gc.DrawLine(MapPoint(q3),MapPoint(q4));
    if ((q4.z+q1.z) > 0) gc.DrawLine(MapPoint(q4),MapPoint(q1));
    if ((q4.z+q5.z) > 0) gc.DrawLine(MapPoint(q4),MapPoint(q5));
    if ((q5.z+q6.z) > 0) gc.DrawLine(MapPoint(q5),MapPoint(q6));
    if ((q6.z+q7.z) > 0) gc.DrawLine(MapPoint(q6),MapPoint(q7));
    if ((q7.z+q8.z) > 0) gc.DrawLine(MapPoint(q7),MapPoint(q8));
    if ((q8.z+q5.z) > 0) gc.DrawLine(MapPoint(q8),MapPoint(q5));
    if ((q8.z+q1.z) > 0) gc.DrawLine(MapPoint(q8),MapPoint(q1));
    if ((q2.z+q7.z) > 0) gc.DrawLine(MapPoint(q2),MapPoint(q7));
    if ((q6.z+q3.z) > 0) gc.DrawLine(MapPoint(q6),MapPoint(q3));

    // Check the x-axis options - there are 4 of these
    // p1-p2 p3-p4 p7-p8 p5-p6
    // We want the ones in front
    gc.SetForeGroundColor(Color("black"));
    
    if ((q1.z+q2.z) > 0) gc.DrawLine(MapPoint(q1),MapPoint(q2));
    if ((q3.z+q4.z) > 0) gc.DrawLine(MapPoint(q3),MapPoint(q4));
    if ((q7.z+q8.z) > 0) gc.DrawLine(MapPoint(q7),MapPoint(q8));
    if ((q5.z+q6.z) > 0) gc.DrawLine(MapPoint(q5),MapPoint(q6));
    
    // Check the y-axis options - there are 4 of these
    // p2-p3 p6-p7 p1-p4 p5-p8
    // Check the z-axis options - there are 4 of these
    // p1-p8 p2-p7 p3-p6 p4-p5
    #endif
    
  }
}



