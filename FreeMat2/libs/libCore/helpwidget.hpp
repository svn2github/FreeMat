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
  void writeSettings();
  void readSettings();
  void closeEvent(QCloseEvent* ce);

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
