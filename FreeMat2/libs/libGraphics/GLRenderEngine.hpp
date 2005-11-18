#ifndef __GLRenderEngine_hpp__
#define __GLRenderEngine_hpp__

#include "RenderEngine.hpp"
#include <qgl.h>

namespace FreeMat {
  class GLRenderEngine : public RenderEngine {
    double model[16];
    double proj[16];
    int viewport[4];
    double m_x1, m_y1, m_width, m_height;
  public:
    GLRenderEngine(QGLWidget *widget, double x1, double y1, 
		   double width, double height);
    ~GLRenderEngine();
    virtual void initialize();
    virtual void toPixels(double x, double y, double z, int &a, int &b);
    virtual void lookAt(double px, double py, double pz,
			double tx, double ty, double tz,
			double ux, double uy, double uz);
    virtual void mapPoint(double x, double y, double z,
			  double &a, double &b, double &c);
    virtual void project(double xmin, double xmax, double ymin, double ymax,
			 double zmin, double zmax);
    virtual void viewport(double x0, double y0, double width, double height);
    virtual void quad(double x1, double y1, double z1,
		      double x2, double y2, double z2,
		      double x3, double y3, double z3,
		      double x4, double y4, double z4);
    virtual void color(std::vector<double>);
    virtual void setLineStyle(std::string style);
    virtual void lineWidth(double n);
    virtual void line(double x1, double y1, double z1,
		      double x2, double y2, double z2);
    virtual void line(double x1, double y1,
		      double x2, double y2);
    virtual void setupDirectDraw();
    virtual void releaseDirectDraw();
    virtual void getModelviewMatrix(double amodel[16]);
    virtual void setFont(QFont fnt);
  };
};

#endif
