#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QMainWindow>
#include "GUITerminal.hpp"

class ApplicationWindow : public QMainWindow {
  Q_OBJECT
    
    GUITerminal* m_term;
  QMenu *fileMenu, *editMenu, *helpMenu;
  QToolBar *editToolBar;
  QAction *saveAct, *quitAct, *copyAct, *pasteAct, *fontAct;
  QAction *aboutAct, *manualAct, *aboutQt;
public:
  ApplicationWindow();
  ~ApplicationWindow();
  void SetGUITerminal(GUITerminal* term);
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
protected:
  void closeEvent(QCloseEvent*);
signals:
  void startHelp();
private slots:
  void save();
  void copy();
  void paste();
  void font();
  void about();
  void editor();
  void manual();
}; 

#endif
