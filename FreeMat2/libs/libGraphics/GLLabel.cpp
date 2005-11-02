
#include "GLLabel.hpp"
#include "qimage.h"
#include "qpainter.h"

namespace FreeMat {
  
  const GLLabel& GLLabel::operator=(const GLLabel& arg) {
    if (this != &arg) {
      delete bits;
    }
    width = arg.width;
    height = arg.height;
    x0 = arg.x0;
    y0 = arg.y0;
    red = arg.red;
    green = arg.green;
    blue = arg.blue;
    bits = new GLubyte[width*height*4];
    memcpy(bits,arg.bits,width*height*4);
    return *this;
  }

  GLLabel::GLLabel(const GLLabel& copy) {
    width = copy.width;
    height = copy.height;
    x0 = copy.x0;
    y0 = copy.y0;
    red = copy.red;
    green = copy.green;
    blue = copy.blue;
    bits = new GLubyte[width*height*4];
    memcpy(bits,copy.bits,width*height*4);
  }

  GLLabel::GLLabel() {
    bits = NULL;
  }

  GLLabel::GLLabel(QFont fnt, std::string txt, GLubyte r, GLubyte g, GLubyte b) {
    text = txt;
    red = r;
    green = g;
    blue = b;
    QFontMetrics fm(fnt);
    QRect sze(fm.boundingRect(txt.c_str()));
    x0 = sze.left();
    y0 = sze.bottom();
    width = sze.width();
    height = sze.height();
    QImage img(width,height,QImage::Format_RGB32);
    QPainter pnt(&img);
    pnt.setBackground(QColor(255,255,255));
    pnt.eraseRect(0,0,width,height);
    pnt.setFont(fnt);
    QFontInfo fntinfo(pnt.fontInfo());
    pnt.setPen(QColor(0,0,0));
    pnt.drawText(x0,height-y0-1,text.c_str());
    pnt.end();
    // Now, we generate a synthetic image that is of the same size
    bits = new GLubyte[width*height*4];
    GLubyte *ibits = img.bits();
    // Set the color bits to all be the same color as specified
    // in the argument list, and use the grey scale to modulate
    // the transparency
    for (int i=0;i<height;i++) {
      QRgb* ibits = (QRgb*) img.scanLine(height-1-i);
      for (int j=0;j<width;j++) {
	int dptr = 4*(i*width+j);
  	bits[dptr] = red;
  	bits[dptr+1] = green;
  	bits[dptr+2] = blue;
  	bits[dptr+3] = 255-qRed(ibits[j]);
      }
    }
  }

  void GLLabel::DrawMe(int x, int y, AlignmentFlag xflag, AlignmentFlag yflag) {
    y -= y0;
    if (xflag == Mean)
      x -= width/2;
    else if (xflag == Max)
      x -= width;
    if (yflag == Mean)
      y -= height/2;
    else if (yflag == Max)
      y -= height;
    qDebug("raster pos %d %d",x,y);
    glRasterPos2i(x,y);
    glDrawPixels(width,height,GL_RGBA,GL_UNSIGNED_BYTE,bits);
  }

  GLLabel::~GLLabel() {
    delete bits;
  }
}
