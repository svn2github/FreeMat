#include "GLRenderEngine.hpp"

namespace FreeMat {
  GLRenderEngine::GLRenderEngine(QGLWidget *widget, double x1, double y1,
				 double width, double height) {
    m_x1 = x1;
    m_y1 = y1;
    m_width = width;
    m_height = height;
  }

  GLRenderEngine::~GLRenderEngine() {
  }

  void GLRenderEngine::initialize() {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
  }

  void GLRenderEngine::toPixels(double x, double y, double z, 
				int &a, int &b) {
    double c1, c2, c3;
    gluProject(x,y,z,model,proj,viewp,&c1,&c2,&c3);
    a = c1;
    b = c2;
  }

  void GLRenderEngine::lookAt(double px, double py, double pz,
			      double tx, double ty, double tz,
			      double ux, double uy, double uz) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(px,py,pz,tx,ty,tz,ux,uy,uz);
    glGetDoublev(GL_MODELVIEW_MATRIX,model);
  }

  
  static void gluMultMatrixVecd(const double matrix[16], const double in[4], double out[4])
  {
    int i;
    
    for (i=0; i<4; i++) {
      out[i] = 
	in[0] * matrix[0*4+i] +
	in[1] * matrix[1*4+i] +
	in[2] * matrix[2*4+i] +
	in[3] * matrix[3*4+i];
    }
  }
  
  void GLRenderEngine::mapPoint(double x, double y, double z,
				double &a, double &b, double &c) {
    double out[4];
    double in[4];
    in[0] = x;
    in[1] = y;
    in[2] = z;
    in[3] = 1.0;
    gluMultMatrixVecd(model,in,out);
    a = out[0];
    b = out[1];
    c = out[2];
  }

  void GLRenderEngine::project(double xmin, double xmax, double ymin, 
			       double ymax, double zmin, double zmax) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xmin,xmax,ymin,ymax,zmin,zmax);
    glGetDoublev(GL_PROJECTION_MATRIX,proj);
  }

  void GLRenderEngine::viewport(double x0, double y0, 
				double width, double height) {
    glViewport(x0,y0,width,height);
    glGetIntegerv(GL_VIEWPORT,viewport);
  }

  void GLRenderEngine::quad(double x1, double y1, double z1,
			    double x2, double y2, double z2,
			    double x3, double y3, double z3,
			    double x4, double y4, double z4) {
    glBegin(GL_QUADS);
    glVertex3f(x1,y1,z1);
    glVertex3f(x2,y2,z2);
    glVertex3f(x3,y3,z3);
    glVertex3f(x4,y4,z4);
    glEnd();
  }

  void GLRenderEngine::color(std::vector<double> col) {
    glColor3f(col[0],col[1],col[2]);
  }
  
  void GLRenderEngine::setLineStyle(std::string style) {
    if (style == "-") {
      glDisable(GL_LINE_STIPPLE);
      return;
    }
    if (style == "--") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0x00FF);
      return;
    }
    if (style == ":") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0xDDDD);
      return;
    }
    if (style == "-.") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0xF0D0);
      return;
    }
    if (style == "none") {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1,0x0000);
      return;
    }
  }

  void GLRenderEngine::lineWidth(double n) {
    glLineWidth(n);
  }

  void GLRenderEngine::line(double x1, double y1, double z1,
			    double x2, double y2, double z2) {
    glBegin(GL_LINES);
    glVertex3f(x1,y1,z1);
    glVertex3f(x2,y2,z2);
    glEnd();
  }

  void GLRenderEngine::line(double x1, double y1, double x2, double y2) {
    glBegin(GL_LINES);
    glVertex3f(x1,y1);
    glVertex3f(x2,y2);
    glEnd();
  }

  void GLRenderEngine::setupDirectDraw() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    std::vector<double> outerpos(GetPropertyVectorAsPixels("outerposition"));
    glViewport(x1,y1,width,height);
    glOrtho(x1,x1+width,y1,y1+height,-1,1);
    glDisable(GL_DEPTH_TEST);
  }

  void GLRenderEngine::releaseDirectDraw() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();    
    glViewport(viewp[0],viewp[1],viewp[2],viewp[3]);
  }

  void GLRenderEngine::getModelviewMatrix(double amodel[16]) {
    for (int i=0;i<16;i++)
      amodel[i] = model[i];
  }
  
  
  
}
