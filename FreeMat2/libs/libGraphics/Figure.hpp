#ifndef __Figure_hpp__
#define __Figure_hpp__

#include <string>
#include <qwidget.h>
#ifdef QT3
#include <qlayout.h>
#else
#include <qgridlayout.h>
#endif

namespace FreeMat {
  
  typedef enum {fignone, figblank, figplot, figscimg, figcbar, figgui, fig3plot} figType;
  
  class Figure : public QWidget {
  public:
    Figure(int fignum);
    ~Figure();
    figType getType();
    void SetFigureChild(QWidget *widget, figType w_type);
    QWidget* GetChildWidget();
    void ReconfigurePlotMatrix(int rows, int cols);
    void ActivateMatrixEntry(int slot);
    void GetPlotMatrix(int& rows, int& cols);
    void Figure::closeEvent(QCloseEvent* e);
    void Copy();
  private:
    int m_num;
    figType *m_type;
    QWidget **m_wid;
    QGridLayout *m_layout;
    int m_rows;
    int m_cols;
    int m_active_slot;
  };

  Figure* GetCurrentFig();
}
#endif
