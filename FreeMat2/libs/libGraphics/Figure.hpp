#ifndef __Figure_hpp__
#define __Figure_hpp__

#include "XPWindow.hpp"
#include "XPWidget.hpp"
#include <string>
#ifdef QT3
#include <qlayout.h>
#else
#include <qgridlayout.h>
#endif

namespace FreeMat {
  
  typedef enum {fignone, figplot, figscimg, figgui, fig3plot} figType;
  
  class Figure : public XPWindow {
  public:
    Figure(int fignum);
    ~Figure();
    figType getType();
    void SetFigureChild(XPWidget *widget, figType w_type);
    XPWidget* GetChildWidget();
    void ReconfigurePlotMatrix(int rows, int cols);
    void ActivateMatrixEntry(int slot);
    void GetPlotMatrix(int& rows, int& cols);
    void Copy();
  private:
    int m_num;
    figType *m_type;
    XPWidget **m_wid;
    QGridLayout *m_layout;
    int m_rows;
    int m_cols;
    int m_active_slot;
  };

  Figure* GetCurrentFig();
}
#endif
