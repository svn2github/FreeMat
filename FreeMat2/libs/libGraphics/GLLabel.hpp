#ifndef __GLLabel_hpp__
#define __GLLabel_hpp__

// A low-level interface to describe a text label in OpenGL-ish.
#include <qfont.h>
#include <qgl.h>

namespace FreeMat {

  class GLLabel {
    GLubyte *bits;
    int width; 
    int height;
    int x0;
    int y0;
    GLubyte red, green, blue;
    std::string text;
  public:
    std::string Text();
    enum AlignmentFlag {Min, Mean, Max};
    GLLabel(const GLLabel& copy);
    GLLabel(QFont fnt, std::string txt, GLubyte r, GLubyte g, GLubyte b);
    GLLabel();
    ~GLLabel();
    void DrawMe(int x, int y, AlignmentFlag xflag, AlignmentFlag yflag);
    const GLLabel& operator=(const GLLabel& arg);
    int twidth();
    int theight();
    int xoffset(AlignmentFlag);
    int yoffset(AlignmentFlag);
  };

}

#endif
