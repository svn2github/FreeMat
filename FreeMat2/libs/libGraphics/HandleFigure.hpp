#ifndef __HandleFigure_hpp__
#define __HandleFigure_hpp__
#include "HandleObject.hpp"

namespace FreeMat {
  //Figure
  //   contains one or more axes
  //   to redraw the figure, we proxy our draws to the axes
  //   Axes
  //   contains one or more children
  //     to redraw the axes, we clear it with bkcolor
  //     then draw it
  //     set the transformation & clipping
  //     then draw the children
  class HandleFigure : public HandleObject {
    int m_width, m_height;
    bool resized;
  public:
    HandleFigure();
    virtual ~HandleFigure() {}
    virtual void ConstructProperties();
    bool Resized();
    int GetWidth() {return m_width;}
    int GetHeight() {return m_height;}
    //    virtual void UpdateState();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    void SetupDefaults();
  };
}

#endif

