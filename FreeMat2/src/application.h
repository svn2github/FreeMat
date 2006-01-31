#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QMainWindow>
#include "GUITerminal.hpp"
#include <QTextDocument>
#include <QTextEdit>

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

class FMTextEdit : public QTextEdit {
  Q_OBJECT
public:
  FMTextEdit();
  virtual ~FMTextEdit();
  void keyPressEvent(QKeyEvent*e);
signals:
  void indent();
};

class FMIndent : public QObject {
  Q_OBJECT
  FMTextEdit *m_te;
public:
  FMIndent();
  virtual ~FMIndent();
  void setDocument(FMTextEdit *te);
  FMTextEdit *document() const;
private slots:
  void update();
};

#endif
