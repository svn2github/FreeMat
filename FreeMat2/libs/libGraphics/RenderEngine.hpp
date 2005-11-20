#ifndef __RenderEngine_hpp__
#define __RenderEngine_hpp__

#include <vector>
#include <string>
#include <qfont.h>

namespace FreeMat {
  class RenderEngine {
  public:
    enum AlignmentFlag {Min, Mean, Max};
    RenderEngine() {};
    virtual ~RenderEngine() {};
    virtual void clear() = 0;
    virtual void toPixels(double x, double y, double z, int &a, int &b) = 0;
    virtual void toPixels(double x, double y, double z, double &a, double &b) = 0;
    virtual void lookAt(double px, double py, double pz,
			double tx, double ty, double tz,
			double ux, double uy, double uz) = 0;
    virtual void mapPoint(double x, double y, double z,
			  double &a, double &b, double &c) = 0;
    virtual void project(double xmin, double xmax, double ymin, double ymax,
			 double zmin, double zmax) = 0;
    virtual void viewport(double x0, double y0, double width, double height) = 0;
    virtual void quad(double x1, double y1, double z1,
		      double x2, double y2, double z2,
		      double x3, double y3, double z3,
		      double x4, double y4, double z4) = 0;
    virtual void color(std::vector<double>) = 0;
    virtual void setLineStyle(std::string style) = 0;
    virtual void lineWidth(double n) = 0;
    virtual void line(double x1, double y1, double z1,
		      double x2, double y2, double z2) = 0;
    virtual void line(double x1, double y1,
		      double x2, double y2) = 0;
    virtual void setupDirectDraw() = 0;
    virtual void releaseDirectDraw() = 0;
    virtual void getModelviewMatrix(double model[16]) = 0;
    virtual void putText(double x, double y, std::string txt, 
			 std::vector<double> color, 
			 AlignmentFlag xflag, AlignmentFlag yflag,
			 QFont fnt, double rotation) = 0;
    virtual void measureText(std::string txt, QFont fnt, AlignmentFlag xflag, 
			     AlignmentFlag yflag,int &width, int &height,
			     int &xoffset, int &yoffset) = 0;
    virtual void depth(bool) = 0;
    virtual void rect(double x1, double y1, double x2, double y2) = 0;
    virtual void rectFill(double x1, double y1, double x2, double y2) = 0;
  };
}

#endif