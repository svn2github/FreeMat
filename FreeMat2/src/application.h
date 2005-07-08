#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#ifdef QT3
#include <qmainwindow.h>
#else
#include <q3mainwindow.h>
#endif

#include "GUITerminal.hpp"

#ifdef QT3
class ApplicationWindow: public QMainWindow
#else
class ApplicationWindow: public Q3MainWindow
#endif
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
