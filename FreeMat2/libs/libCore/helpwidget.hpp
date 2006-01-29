#ifndef __HelpWidget_hpp__
#define __HelpWidget_hpp__

#include <QMainWindow>
#include <QDockWidget>
#include <QTextBrowser>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>
#include <QListWidget>
#include <QTreeWidget>

class HelpWindow;

class HelpWidget : public QDockWidget {
  Q_OBJECT
public:
  HelpWidget(QString url, HelpWindow *mgr);
private:
  QTabWidget *m_browser;
};

class HelpWindow : public QMainWindow {
  Q_OBJECT

public:
  HelpWindow(QString baseURL);
private slots:
  void activateModule(QListWidgetItem* item);
  void activateModule(QTreeWidgetItem* item, int);
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  QTextBrowser *tb;
  HelpWidget *m_helpwidget;
  QMenu *fileMenu, *editMenu, *goMenu, *helpMenu;
  QToolBar *editToolBar;
  QToolBar *goToolBar;
  QAction *copyAct, *exitAct;
  QAction *backAct, *forwardAct, *homeAct;
  QAction *zoominAct, *zoomoutAct;
  QString m_initial;
};

#endif
