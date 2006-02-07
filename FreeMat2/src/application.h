#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QMainWindow>
#include "GUITerminal.hpp"
#include "Editor.hpp"
#include "ToolDock.hpp"

class ApplicationWindow : public QMainWindow {
  Q_OBJECT
    
  GUITerminal* m_term;
  QMenu *fileMenu, *editMenu, *toolsMenu, *helpMenu;
  QToolBar *editToolBar;
  QAction *saveAct, *quitAct, *copyAct, *pasteAct, *fontAct;
  QAction *aboutAct, *manualAct, *aboutQt, *editorAct;
  QAction *historyAct, *pathAct;
  FMEditor *edit;
  ToolDock *m_tool;
public:
  ApplicationWindow();
  ~ApplicationWindow();
  void SetGUITerminal(GUITerminal* term);
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void readSettings();
  //  void writeSettings();
  void initializeTools();
  void createToolBox();
protected:
  void closeEvent(QCloseEvent*);
signals:
  void startHelp();
public slots:
  void writeSettings();
private slots:
  void save();
  void copy();
  void paste();
  void font();
  void about();
  void editor();
  void history();
  void path();
  void manual();
  void tclose();
}; 

#endif
