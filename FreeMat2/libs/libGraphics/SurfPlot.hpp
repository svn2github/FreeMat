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

#ifndef __SurfPlot_hpp__
#define __SurfPlot_hpp__

#include "DataSet2D.hpp"
#include "Axis.hpp"
#include "FLTKGC.hpp"
#include "PrintableWidget.hpp"
#include "NewAxis.hpp"

namespace FreeMat {

  typedef std::vector<DataSet3D> DataSet3DVector;

  typedef struct {
    double x;
    double y;
    double z;
  } pt3d;
  
  typedef struct {
    pt3d pts[4];
    double meanz;
    int color;
  } quad3d;


  class SurfPlot: public PrintableWidget {
  public:
    /**
     * Default constructor.
     */
    SurfPlot(int width, int height);
    /**
     * Default destructor.
     */
    ~SurfPlot();
    void OnMouseDown(int x, int y);
    void OnMouseUp(int x, int y);
    void OnDrag(int x, int y);
    void OnDraw(GraphicsContext &gc);
    void draw();
    void resize(int x, int y, int w, int h);
    int handle(int event);
    // Set the colormap
    void SetColormap(double *cmap);
    // Set the data to render
    void SetData(const double*x, const double*y, const double*z, int nx, int ny);
    // Calculate the mapping factors
    void UpdateBounds();
    // Convert the surface into quads
    void SurfaceToQuads();
    // Map a point to the viewport
    Point2D MapPoint(pt3d a);
    // Draw the axes
    void DrawAxes(GraphicsContext &gc, float m[4][4]);
    void DrawAxisTest(GraphicsContext &gc, const char *label,
		      pt3d q[8], int a_start[4], int a_stop[4],
		      int m_height, NewAxis& ref, pt3d units[4],
		      bool Zaxis);
    void DrawAxis(GraphicsContext &gc, const char *label,
		  pt3d a_start, pt3d a_stop,
		  NewAxis& ref, pt3d unit);
  private:
    /*
     * The data for the surf model.
     */
    double *xvals;
    double *yvals;
    double *zvals;
    int xcount;
    int ycount;
    NewAxis XAxis;
    NewAxis YAxis;
    NewAxis ZAxis;
    // Describe the transformation for the data.
    double x_center;
    double y_center;
    double z_center;
    double max_radius;
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    double z_min;
    double z_max;
    double scalex;
    double scaley;
    double offsetx;
    double offsety;
    std::vector<quad3d> quads;

    double *colormap;

    double azim;
    double elev;
    //    float quat[4];
    int beginx;
    int beginy;
    Axis *xAxis;
    Axis *yAxis;
    Axis *zAxis;
    int m_width;
    int m_height;
  };
}

#endif  
