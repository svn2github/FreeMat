#include "GLRenderEngine.hpp"
#include <qimage.h>
#include <qpainter.h>

namespace FreeMat {
  GLRenderEngine::GLRenderEngine(QGLWidget *widget, double x1, double y1,
				 double width, double height) {
    m_x1 = x1;
    m_y1 = y1;
    m_width = width;
    m_height = height;
    m_widget = widget;
  }

  GLRenderEngine::~GLRenderEngine() {
  }

  void GLRenderEngine::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void GLRenderEngine::toPixels(double x, double y, double z, 
				double &a, double &b) {
    double c1, c2, c3;
    gluProject(x,y,z,model,proj,viewp,&c1,&c2,&c3);
    a = c1;
    b = c2;
  }

  void GLRenderEngine::toPixels(double x, double y, double z, 
				int &a, int &b) {
    double c1, c2;
    toPixels(x,y,z,c1,c2);
    a = (int) c1;
    b = (int) c2;
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
    glGetIntegerv(GL_VIEWPORT,viewp);
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
    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glEnd();
  }

  void GLRenderEngine::setupDirectDraw() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glViewport(m_x1,m_y1,m_width,m_height);
    glOrtho(m_x1,m_x1+m_width,m_y1,m_y1+m_height,-1,1);
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
  
  void GLRenderEngine::putText(double x, double y, std::string txt, 
			       std::vector<double> color, 
			       AlignmentFlag xflag, AlignmentFlag yflag,
			       QFont fnt, double rotation) {
    QFontMetrics fm(fnt);
    QRect sze(fm.boundingRect(txt.c_str()));
    int x0 = sze.left();
    int y0 = sze.bottom();
    int width = sze.width()+fm.width("W");
    int height = sze.height();
    QImage img(width,height,QImage::Format_RGB32);
    QPainter pnt(&img);
    pnt.setRenderHint(QPainter::TextAntialiasing);
    pnt.setRenderHint(QPainter::Antialiasing);
    pnt.setBackground(QColor(255,255,255));
    pnt.eraseRect(0,0,width,height);
    pnt.setFont(fnt);
    pnt.setPen(QColor(0,0,0));
    pnt.drawText(x0,height-y0-1,txt.c_str());
    pnt.end();
    // Figure out what the minimum bounding box is...
    int newwidth = 0;
    for (int j=0;j<height;j++) {
      QRgb* ibits = (QRgb*) img.scanLine(j);
      int k=width-1;
      while ((qRed(ibits[k])==255) && (k>=0)) 
	k--;
      newwidth = (newwidth < k) ? k : newwidth;
    }
    width = newwidth+1;
    // Now, we generate a synthetic image that is of the same size
    QImage pic = QImage(width,height,QImage::Format_ARGB32);
    // Set the color bits to all be the same color as specified
    // in the argument list, and use the grey scale to modulate
    // the transparency
    for (int i=0;i<height;i++) {
      QRgb* ibits = (QRgb*) img.scanLine(i);
      QRgb* obits = (QRgb*) pic.scanLine(height-1-i);
      for (int j=0;j<width;j++) 
  	obits[j] = qRgba(color[0]*255,color[1]*255,color[2]*255,255-qRed(ibits[j]));
    }
    pic = QGLWidget::convertToGLFormat(pic);
    y -= y0;
    if (xflag == Mean)
      x -= width/2;
    else if (xflag == Max)
      x -= width;
    if (yflag == Mean)
      y -= height/2;
    else if (yflag == Max)
      y -= height;
    int txid = m_widget->bindTexture(pic);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex2d(x,y);
    glTexCoord2d(1,0);
    glVertex2d(x+width+1,y);
    glTexCoord2d(1,1);
    glVertex2d(x+width+1,y+height+1);
    glTexCoord2d(0,1);
    glVertex2d(x,y+height+1);
    glEnd();
    m_widget->deleteTexture(txid);
  }

  void GLRenderEngine::depth(bool flag) {
    if (flag)
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);
  }

  void GLRenderEngine::measureText(std::string txt, QFont fnt, AlignmentFlag xflag, 
				   AlignmentFlag yflag,int &width, int &height,
				   int &xoffset, int &yoffset) {
  }
}
