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
    XAxis.UpdateIntervals(x_min,x_max);
    XAxis.AutoSetAxis();

    y_min = yvals[0];
    y_max = yvals[0];
    for (int i=0;i<ycount;i++) {
      y_min = std::min(y_min,yvals[i]);
      y_max = std::max(y_max,yvals[i]);
    }
    y_center = (y_min+y_max)/2.0;
    YAxis.UpdateIntervals(y_min,y_max);
    YAxis.AutoSetAxis();

    z_min = zvals[0];
    z_max = zvals[0];
    for (int i=0;i<xcount*ycount;i++) {
      z_min = std::min(z_min,zvals[i]);
      z_max = std::max(z_max,zvals[i]);
    }
    z_center = (z_min+z_max)/2.0;
    ZAxis.UpdateIntervals(z_min,z_max);
    ZAxis.AutoSetAxis();
    
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
    trackball(quat,0.0,0.0,0.0,0.0);
    xvals = NULL;
    yvals = NULL;
    zvals = NULL;
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
	      (2.0*beginx - szx) / szx / 10,
	      -(szy - 2.0*beginy) / szy / 10,
	      (2.0*x - szx) / szx / 10,
	      -(szy - 2.0*y) / szy / 10);
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

  static void DrawAxis(GraphicsContext &gc, const char *label,
		       int hull_count, Point* chull,
		       int startndx, int stopndx,
		       NewAxis& ref) {
    Point start, stop;
    start = FindCHullPoint(hull_count,chull,startndx);
    stop = FindCHullPoint(hull_count,chull,stopndx);
    gc.SetForeGroundColor(Color("black"));
    gc.DrawLine(Point2D((int)start.x,(int)start.y),
		Point2D((int)stop.x,(int)stop.y));
    // Compute the unit vector for this axis
    double delta_x, delta_y;
    delta_x = stop.x - start.x;
    delta_y = stop.y - start.y;
    double delta_mag;
    delta_mag = sqrt(delta_x*delta_x+delta_y*delta_y);
    delta_x /= delta_mag;
    delta_y /= delta_mag;
    // Now, compute the normal vector - there are
    // two choices (-dy,dx) and (dy,-dx)
    double norm1_x, norm1_y;
    norm1_x = -delta_y;
    norm1_y = delta_x;
    double norm2_x, norm2_y;
    norm2_x = delta_y;
    norm2_y = -delta_x;
    // We have to determine which normal vector to use...
    // To do this, we use the convex hull test again.  
    Point pts[10];
    for (int i=0;i<hull_count;i++)
      pts[i] = chull[i];
    // Add the two extreme points
    pts[hull_count].x = (start.x+stop.x)/2 + 10*norm1_x;
    pts[hull_count].y = (start.y+stop.y)/2 + 10*norm1_y;
    pts[hull_count].ptnum = -1;
    pts[hull_count+1].x = (start.x+stop.x)/2 + 10*norm2_x;
    pts[hull_count+1].y = (start.y+stop.y)/2 + 10*norm2_y;
    pts[hull_count+1].ptnum = -2;
    // Sort and build the convex hull
    std::sort(pts,pts+hull_count+2);
    int new_hull_count;
    Point new_chull[12];
    new_hull_count = chainHull_2D(pts,hull_count+2,new_chull);
    // Whichever normal survives on the C-Hull is the
    // one we are going to keep
    double norm_x, norm_y;
    if (OnHull(new_hull_count,new_chull,-1)) {
      norm_x = norm1_x;
      norm_y = norm1_y;
    } else {
      norm_x = norm2_x;
      norm_y = norm2_y;
    }
    gc.SetFont(12);
    double tmin, tdelta;
    int tcount;
    ref.GetIntervals(tmin,tdelta,tcount);
    for (int i=0;i<tcount;i++) {
      Point2D t(start.x + i/((double) tcount)*(stop.x-start.x),
		start.y + i/((double) tcount)*(stop.y-start.y));
      gc.DrawLine(t,Point2D(t.x+10*norm_x,t.y+10*norm_y));
    }
//    
//    if (fabs(norm_y) < fabs(norm_x))
//      gc.DrawTextString(label,Point2D((int)(start.x+stop.x)/2 + 10*norm_x,
//				      (int)(start.y+stop.y)/2 + 10*norm_y),
//			ORIENT_90);
//    else
//      gc.DrawTextString(label,Point2D((int)(start.x+stop.x)/2 + 10*norm_x,
//				      (int)(start.y+stop.y)/2 + 10*norm_y),
//			ORIENT_0);
  }

  static void DrawAxisTest(GraphicsContext &gc, const char *label,
			   int hull_count, Point* chull, 
			   int a1, int a2, int a3, int a4,
			   int a5, int a6, int a7, int a8,
			   int m_height, NewAxis& ref) {
    float centers_x[4];
    float centers_y[4];
    int axis_index[4];
    int active_count;
    Point start, stop;

    // To draw an axis, we test the four possibilities against
    // the convex hull, and label (and draw) those axes that
    // are on the hull
    active_count = 0;
    if (OnHull(hull_count,chull,a1) && OnHull(hull_count,chull,a2)) {
      start = FindCHullPoint(hull_count,chull,a1);
      stop = FindCHullPoint(hull_count,chull,a2);
      centers_x[active_count] = (start.x + stop.x)/2;
      centers_y[active_count] = (start.y + stop.y)/2;
      axis_index[active_count++] = 1;
    }
    if (OnHull(hull_count,chull,a3) && OnHull(hull_count,chull,a4)) {
      start = FindCHullPoint(hull_count,chull,a3);
      stop = FindCHullPoint(hull_count,chull,a4);
      centers_x[active_count] = (start.x + stop.x)/2;
      centers_y[active_count] = (start.y + stop.y)/2;
      axis_index[active_count++] = 2;
    }
    if (OnHull(hull_count,chull,a5) && OnHull(hull_count,chull,a6))  {
      start = FindCHullPoint(hull_count,chull,a5);
      stop = FindCHullPoint(hull_count,chull,a6);
      centers_x[active_count] = (start.x + stop.x)/2;
      centers_y[active_count] = (start.y + stop.y)/2;
      axis_index[active_count++] = 3;
    }
    if (OnHull(hull_count,chull,a7) && OnHull(hull_count,chull,a8))  {
      start = FindCHullPoint(hull_count,chull,a7);
      stop = FindCHullPoint(hull_count,chull,a8);
      centers_x[active_count] = (start.x + stop.x)/2;
      centers_y[active_count] = (start.y + stop.y)/2;
      axis_index[active_count++] = 4;
    }
    // Find the axis projection with the smallest radius to the
    // lower left corner;
    if (!active_count) return;
    double best_radius = 1e100;
    int best_ndx = 0;
    for (int i=0;i<active_count;i++) {
      double radius;
      radius = (m_height-centers_y[i])*1000 + centers_x[i];
      if (radius < best_radius) {
	best_radius = radius;
	best_ndx = i;
      }
    }
    switch (axis_index[best_ndx]) {
    case 1:
      DrawAxis(gc,label,hull_count,chull,a1,a2,ref);
      break;
    case 2:
      DrawAxis(gc,label,hull_count,chull,a3,a4,ref);
      break;
    case 3:
      DrawAxis(gc,label,hull_count,chull,a5,a6,ref);
      break;
    case 4:
      DrawAxis(gc,label,hull_count,chull,a7,a8,ref);
      break;
    }
  }

  void SurfPlot::DrawAxes(GraphicsContext &gc, float m[4][4]) {
    // The 8 corners of the cube
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
    // Map the corners to the screen
    Point corners[8];
    for (int i=0;i<8;i++) {
      corners[i].x = q[i].x*scalex+offsetx;
      corners[i].y = q[i].y*scaley+offsety;
      corners[i].ptnum = i;
    }
    // Sort the points (required by the convex hull code)
    std::sort(corners,corners+8);
    // Compute the convex hull...
    Point chull[8];
    int hull_count;
    hull_count = chainHull_2D(corners,8,chull);
    // Now, we have to draw the axes in...
    // To do this, we test each axis case,
    // and find all cases that are on the convex hull
    DrawAxisTest(gc,"xaxis",hull_count,chull,0,1,2,3,6,7,4,5,m_height,XAxis);
    DrawAxisTest(gc,"yaxis",hull_count,chull,1,2,5,6,0,3,4,7,m_height,YAxis);
    DrawAxisTest(gc,"zaxis",hull_count,chull,0,7,1,6,2,5,3,4,m_height,ZAxis);
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

    // Draw the box behind the data
#if 0
    gc.SetForeGroundColor(Color("black"));
    if ((q1.z+q2.z) < 0) gc.DrawLine(MapPoint(q1),MapPoint(q2));
    if ((q2.z+q3.z) < 0) gc.DrawLine(MapPoint(q2),MapPoint(q3));
    if ((q3.z+q4.z) < 0) gc.DrawLine(MapPoint(q3),MapPoint(q4));
    if ((q4.z+q1.z) < 0) gc.DrawLine(MapPoint(q4),MapPoint(q1));
    if ((q4.z+q5.z) < 0) gc.DrawLine(MapPoint(q4),MapPoint(q5));
    if ((q5.z+q6.z) < 0) gc.DrawLine(MapPoint(q5),MapPoint(q6));
    if ((q6.z+q7.z) < 0) gc.DrawLine(MapPoint(q6),MapPoint(q7));
    if ((q7.z+q8.z) < 0) gc.DrawLine(MapPoint(q7),MapPoint(q8));
    if ((q8.z+q5.z) < 0) gc.DrawLine(MapPoint(q8),MapPoint(q5));
    if ((q8.z+q1.z) < 0) gc.DrawLine(MapPoint(q8),MapPoint(q1));
    if ((q2.z+q7.z) < 0) gc.DrawLine(MapPoint(q2),MapPoint(q7));
    if ((q6.z+q3.z) < 0) gc.DrawLine(MapPoint(q6),MapPoint(q3));
#endif

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



