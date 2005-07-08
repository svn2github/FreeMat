#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#ifdef QT3
#include <qmainwindow.h>
#else
#include <q3mainwindow.h>
#endif

#include "GUITerminal.hpp"

#ifdef QT3
#define MainWin QMainWindow
#else
#define MainWin Q3MainWindow
#endif

class ApplicationWindow: public MainWin
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
