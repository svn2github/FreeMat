#ifndef __HelpWidget_hpp__
#define __HelpWidget_hpp__

#include <QMainWindow>
#include <QDockWidget>
#include <QTextBrowser>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>


class HelpWidget : public QDockWidget {
  Q_OBJECT

public:
  HelpWidget(QString url);
private:
  QTabWidget *m_browser;
};

class HelpWinManager : public QTabWidget {
  Q_OBJECT
  bool m_forwardFlag, m_backwardFlag;
public:
  HelpWinManager(QWidget *parent);
signals:
  void forwardAvailable(bool avail);
  void backwardAvailable(bool avail);
private slots:
  void forward();
  void back();
  void home();
  void forwardFlag(bool avail);
  void backwardFlag(bool avail);
  void activeChanged(int nim);
};

class HelpWindow : public QMainWindow {
  Q_OBJECT

public:
  HelpWindow(QString baseURL);
private slots:
  void newTab();
  void closeTab();
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  HelpWinManager *m_tabs;
  HelpWidget *m_helpwidget;
  QMenu *fileMenu, *editMenu, *goMenu, *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *editToolBar;
  QToolBar *goToolBar;
  QAction *newAct, *closeAct, *copyAct, *pasteAct, *exitAct;
  QAction *backAct, *forwardAct, *homeAct;
  QString m_initial;
};

#endif
