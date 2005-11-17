
#include "GLLabel.hpp"
#include "qimage.h"
#include "qpainter.h"

namespace FreeMat {
  
  std::string GLLabel::Text() {
    return text;
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
    width = sze.width()+fm.width("W");
    height = sze.height();
    QImage img(width,height,QImage::Format_RGB32);
    QPainter pnt(&img);
    pnt.setRenderHint(QPainter::TextAntialiasing);
    pnt.setRenderHint(QPainter::Antialiasing);
    pnt.setBackground(QColor(255,255,255));
    pnt.eraseRect(0,0,width,height);
    pnt.setFont(fnt);
    pnt.setPen(QColor(0,0,0));
    pnt.drawText(x0,height-y0-1,text.c_str());
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
    pic = QImage(width,height,QImage::Format_ARGB32);
    // Set the color bits to all be the same color as specified
    // in the argument list, and use the grey scale to modulate
    // the transparency
    for (int i=0;i<height;i++) {
      QRgb* ibits = (QRgb*) img.scanLine(i);
      QRgb* obits = (QRgb*) pic.scanLine(i);
      for (int j=0;j<width;j++) 
  	obits[j] = qRgba(red,green,blue,255-qRed(ibits[j]));
    }
  }

  int GLLabel::twidth() {
    return width;
  }
  
  int GLLabel::theight() {
    return height;
  }

  int GLLabel::xoffset(AlignmentFlag xflag) {
    if (xflag == Mean)
      return -width/2;
    else if (xflag == Max)
      return -width;
    else
      return 0;
  }

  int GLLabel::yoffset(AlignmentFlag yflag) {
    if (yflag == Mean)
      return -height/2;
    else if (yflag == Max)
      return -height;
    else
      return 0;
  }

  void GLLabel::DrawMe(QGLWidget *widget, int x, int y, 
		       AlignmentFlag xflag, AlignmentFlag yflag) {
    y -= y0;
    if (xflag == Mean)
      x -= width/2;
    else if (xflag == Max)
      x -= width;
    if (yflag == Mean)
      y -= height/2;
    else if (yflag == Max)
      y -= height;
    glRasterPos2i(x,y);
    widget->bindTexture(pic);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0);
    glVertex2d(x,y);
    glTexCoord2d(1,0);
    glVertex2d(x+width-1,y);
    glTexCoord2d(1,1);
    glVertex2d(x+width-1,y+height-1);
    glTexCoord2d(0,1);
    glVertex2d(x,y+height-1);
    glEnd();
    //    glDrawPixels(width,height,GL_RGBA,GL_UNSIGNED_BYTE,bits);
  }

  GLLabel::~GLLabel() {
    delete bits;
  }
}
