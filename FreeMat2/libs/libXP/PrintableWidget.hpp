#ifndef __PrintableWidget_hpp__
#define __PrintableWidget_hpp__

#include <FL/Fl_Widget.H>
#include "GraphicsContext.hpp"

class PrintableWidget : public Fl_Widget {
public:
  PrintableWidget(int x, int y, int w, int h);
  virtual void OnDraw(GraphicsContext &gc) = 0;
};

#endif
