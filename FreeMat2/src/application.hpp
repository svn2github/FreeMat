/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QMainWindow>
#include "QTTerm.hpp"
#include "Editor.hpp"
#include "ToolDock.hpp"
#include "KeyManager.hpp"
#include <QLabel>


class AboutWidget : public QWidget {
  Q_OBJECT
private:
  int linenumber;
  int phase;
public:
  AboutWidget();
  void paintEvent(QPaintEvent *e);
public slots:
  void Refresh();
};

class AboutWindow : public QWidget {
  Q_OBJECT
private:
  int linenumber;
  int phase;
  QTimer* refresh;
  AboutWidget* thanks;
public:
  AboutWindow();
  void showEvent(QShowEvent* event);
  void hideEvent(QHideEvent* event);
};

class ApplicationWindow : public QMainWindow {
  Q_OBJECT
    
  QTTerm* m_term;
  KeyManager* m_keys;
  QMenu *fileMenu, *editMenu, *toolsMenu, *historyMenu, *helpMenu, *debugMenu;
  QToolBar *editToolBar, *debugToolBar;
  QAction *saveAct, *quitAct, *copyAct, *pasteAct, *fontAct;
  QAction *aboutAct, *manualAct, *aboutQt, *editorAct;
  QAction *pathAct, *filetoolAct, *historyAct, *cleanHistoryAct;
  QAction *pauseAct, *continueAct, *stopAct;
  QAction *dbStepAct, *dbTraceAct;
  ToolDock *m_tool;
  AboutWindow *Aboutwin;
public:
  ApplicationWindow();
  ~ApplicationWindow();
  void SetGUITerminal(QTTerm* term);
  void SetKeyManager(KeyManager* keys);
  void readSettings();
  bool event(QEvent* e);
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void initializeTools();
  void createToolBox();
protected:
  void closeEvent(QCloseEvent*);
signals:
  void startHelp();
  void startEditor();
  void startPathTool();
public slots:
  void writeSettings();
private slots:
  void save();
  void copy();
  void paste();
  void font();
  void about();
  void editor();
  void path();
  void manual();
  void tclose();
  void filetool();
  void history();
  void cleanhistory();
  void init();
}; 

#endif
