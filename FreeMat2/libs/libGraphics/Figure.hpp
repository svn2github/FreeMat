#ifndef __Figure_hpp__
#define __Figure_hpp__

#include <FL/Fl_Double_Window.H>
#include "PrintableWidget.hpp"
#include <string>

namespace FreeMat {
  
  typedef enum {fignone, figplot, figscimg, figgui} figType;
  
  class Figure : public Fl_Double_Window {
  public:
    Figure(int fignum);
    ~Figure();
    figType getType() {return m_type;};
    void SetFigureChild(PrintableWidget *widget, figType w_type);
    PrintableWidget* GetChildWidget();
    void Print(std::string filename);
  private:
    int m_num;
    figType m_type;
    PrintableWidget *m_wid;
  };
  Figure* GetCurrentFig();
}
#endif
