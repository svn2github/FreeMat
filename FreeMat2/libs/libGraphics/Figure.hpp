#ifndef __Figure_hpp__
#define __Figure_hpp__

#include <FL/Fl_Double_Window.H>
#include "XPWidget.hpp"
#include <string>

namespace FreeMat {
  
  typedef enum {fignone, figplot, figscimg, figgui} figType;
  
  class Figure : public Fl_Double_Window {
  public:
    Figure(int fignum);
    ~Figure();
    figType getType() {return m_type;};
    void SetFigureChild(Fl_Widget *widget, figType w_type);
    Fl_Widget* GetChildWidget();
  private:
    int m_num;
    figType m_type;
    Fl_Widget *m_wid;
  };
  Figure* GetCurrentFig();
}
#endif
