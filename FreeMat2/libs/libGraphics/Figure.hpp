#ifndef __Figure_hpp__
#define __Figure_hpp__

#include "XWindow.hpp"
#include "XPWidget.hpp"
#include <string>

namespace FreeMat {
  
  typedef enum {fignone, figplot, figscimg, figgui} figType;
  
  class Figure : public XWindow {
  public:
    Figure(int fignum);
    ~Figure();
    void SetFigureChild(XPWidget *xp, figType typ);
    figType getType() {return m_type;};
  private:
    int m_num;
    figType m_type;
  };
  Figure* GetCurrentFig();
  void ForceRefresh();
}
#endif
