#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <qmainwindow.h>
#include "GUITerminal.hpp"

class ApplicationWindow: public QMainWindow
{
  Q_OBJECT

  GUITerminal* m_term;
public:
  ApplicationWindow();
  ~ApplicationWindow();
  void SetGUITerminal(GUITerminal* term);

protected:
  void closeEvent(QCloseEvent*);

private slots:
 void save();
 void copy();
 void paste();
 void font();
 void about();
 void manual();
 void aboutQt();
}; 


#endif
