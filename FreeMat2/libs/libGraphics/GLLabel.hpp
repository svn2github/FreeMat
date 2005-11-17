#ifndef __GLLabel_hpp__
#define __GLLabel_hpp__

// A low-level interface to describe a text label in OpenGL-ish.
#include <qfont.h>
#include <qgl.h>
#include <qimage.h>

namespace FreeMat {

  class GLLabel {
    GLubyte *bits;
    int width; 
    int height;
    int x0;
    int y0;
    GLubyte red, green, blue;
    std::string text;
    QImage pic;
  public:
    std::string Text();
    enum AlignmentFlag {Min, Mean, Max};
    GLLabel(QFont fnt, std::string txt, GLubyte r, GLubyte g, GLubyte b);
    GLLabel();
    ~GLLabel();
    void DrawMe(QGLWidget *widget, int x, int y, AlignmentFlag xflag, AlignmentFlag yflag);
    int twidth();
    int theight();
    int xoffset(AlignmentFlag);
    int yoffset(AlignmentFlag);
  };

}

#endif
