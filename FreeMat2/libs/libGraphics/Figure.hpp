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
    void SetWidget(XPWidget *xp, figType typ);
    XPWidget* GetWidget();
    virtual void Print(std::string fname);
    virtual void OnDraw(GraphicsContext& dc);
    figType getType() {return m_type;};
    virtual void OnMouseDown(int x, int y);
    virtual void OnMouseUp(int x, int y);
  private:
    int m_num;
    figType m_type;
    XPWidget *m_payload;
  };
  Figure* GetCurrentFig();
  void ForceRefresh();
}
#endif
