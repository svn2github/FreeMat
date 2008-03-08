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

#include "helpwidget.hpp"
#include "Interpreter.hpp"
#include <QtGui>
#include <QDebug>

HelpSearcher::HelpSearcher(QWidget *parent, QString basepath, HelpWindow *mgr) : QWidget(parent) {
  QVBoxLayout *vlayout = new QVBoxLayout;
  QWidget *pane = new QWidget;
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget(new QLabel("Search Text:"));
  m_search_word = new QLineEdit;
  hlayout->addWidget(m_search_word);
  pane->setLayout(hlayout);
  vlayout->addWidget(pane);
  m_results_list = new QListWidget;
  vlayout->addWidget(m_results_list);
  setLayout(vlayout);
  connect(m_search_word,SIGNAL(returnPressed()),this,SLOT(updateSearch()));
  connect(m_results_list,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	  mgr,SLOT(activateModule(QListWidgetItem*)));
  connect(m_results_list,SIGNAL(itemActivated(QListWidgetItem*)),
	  mgr,SLOT(activateModule(QListWidgetItem*)));
  m_basepath = basepath;
}

void HelpSearcher::updateSearch() {
  m_results_list->clear();
  QRegExp search_pattern(m_search_word->text());
  // Get a list of all .html files
  QDir dir(m_basepath);
  dir.setNameFilters(QStringList() << "*.html");
  QFileInfoList list = dir.entryInfoList();
  for (int i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    QFile f(fileInfo.absoluteFilePath());
    if (f.open(QIODevice::ReadOnly)) {
      QTextStream str(&f);
      QString helpText(str.readAll());
      if (helpText.count(search_pattern) > 0)  {
	QString entry(fileInfo.fileName());
	QRegExp reg("(\\w*)_(\\w*)");
	if (reg.indexIn(entry) > -1) {
	  if (reg.cap(1) != "sec")
	    new QListWidgetItem(reg.cap(2) + " (" + reg.cap(1) + ")",m_results_list);
	}
      }
    }
  }
}

void HelpWindow::activateModule(QListWidgetItem* item) {
  QString name_and_section(item->text());
  QRegExp modname_pattern("^\\s*(\\b\\w+\\b)\\s*\\((\\b\\w+\\b)\\)");
  if (modname_pattern.indexIn(name_and_section) < 0)
    return;
  tb->setSource(QUrl::fromLocalFile(m_initial+"/"+modname_pattern.cap(2) + "_" + modname_pattern.cap(1)+".html"));
}

void HelpWindow::activateModule(QTreeWidgetItem* item, int) {
  QString fulltext(item->text(0));
  QRegExp modname("^\\s*(\\b[\\w]+\\b)");
  if (modname.indexIn(fulltext) < 0)
    return;
  QString module(modname.cap(1).toLower());
  tb->setSource(QUrl::fromLocalFile(m_initial+"/"+item->text(1)+"_"+module+".html"));
}

void HelpWindow::helpText(QString fulltext) {
  QList<QListWidgetItem *> items = m_helpwidget->m_flist->findItems(fulltext.toLower(),Qt::MatchStartsWith);
  if (items.isEmpty()) {
    QMessageBox::warning(this, "helpwin", 
                   "Cannot find help document for '" + fulltext + "'\n",
                   QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);    
    return;
  }
  QListWidgetItem* item = items.at(0);
  QString name_and_section(item->text());
  QRegExp modname_pattern("^\\s*(\\b\\w+\\b)\\s*\\((\\b\\w+\\b)\\)");
  if (modname_pattern.indexIn(name_and_section) < 0)
    return;
  tb->setSource(QUrl::fromLocalFile(m_initial+"/"+modname_pattern.cap(2) + "_" + modname_pattern.cap(1)+".html"));
}

HelpWidget::HelpWidget(QString url, HelpWindow *mgr) {
  setObjectName("helpwidget");
  m_browser = new QTabWidget(this);
  setWidget(m_browser);
  m_flist = new QListWidget;
  // Populate the list widget
  QFile *file = new QFile(url + "/modules.txt");
  if (!file->open(QFile::ReadOnly | QIODevice::Text))
    QMessageBox::warning(this,"Cannot Find Module List","The file modules.txt is missing from the directory "+url+" where I think help files should be.  The Topic List widget will not function properly.",QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  else {
    QTextStream t(file);
    while (!t.atEnd()) {
      QString line(t.readLine());
      new QListWidgetItem(line,m_flist);
    }
  }
  delete file;
  connect(m_flist,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	  mgr,SLOT(activateModule(QListWidgetItem*)));
  connect(m_flist,SIGNAL(itemActivated(QListWidgetItem*)),
	  mgr,SLOT(activateModule(QListWidgetItem*)));
  
  m_browser->addTab(m_flist,"Topic List");


  QTreeWidget *m_tindex = new QTreeWidget;

  connect(m_tindex,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
	  mgr,SLOT(activateModule(QTreeWidgetItem*,int)));
  connect(m_tindex,SIGNAL(itemActivated(QTreeWidgetItem*,int)),
	  mgr,SLOT(activateModule(QTreeWidgetItem*,int)));

  m_tindex->setColumnCount(1);
  m_tindex->setHeaderLabels(QStringList() << Interpreter::getVersionString().c_str());
  file = new QFile(url + "/sectable.txt");
  QRegExp reg("\\+\\s*\\((\\b\\w+\\b)\\)\\s*(\\b.*)");
  if (!file->open(QFile::ReadOnly | QIODevice::Text))
    QMessageBox::warning(this,"Cannot Find Section Index","The file sectable.txt is missing from the directory "+url+" where I think help files should be.  The Index widget will not function properly.",QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  else {
    QTextStream t(file);
    QTreeWidgetItem *prev = NULL;
    while (!t.atEnd()) {
      QString line(t.readLine());
      if (reg.indexIn(line) < 0)
	prev = new QTreeWidgetItem(m_tindex,QStringList() << line);
      else {
	new QTreeWidgetItem(prev,QStringList() << reg.cap(2) << reg.cap(1));
      }
    }
  }
  delete file;
  m_browser->addTab(m_tindex,"Index");
  m_browser->addTab(new HelpSearcher(m_browser,url,mgr),"Search");
}

HelpWindow::HelpWindow(QString url) {
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
  setWindowTitle(QString(Interpreter::getVersionString().c_str()) + " Online Help");
  m_initial = url;
  tb = new QTextBrowser(this);
  tb->setSource(QUrl::fromLocalFile(m_initial+"/index.html"));
  setCentralWidget(tb);
  m_helpwidget = new HelpWidget(url,this);
  addDockWidget(Qt::LeftDockWidgetArea,m_helpwidget);
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  readSettings();
}

void HelpWindow::closeEvent(QCloseEvent* ce) {
  writeSettings();
  ce->accept();
}

void HelpWindow::writeSettings() {
  QSettings settings("FreeMat", "FreeMat");
  settings.setValue("helpwindow/state",saveState());
  settings.setValue("helpwindow/pos", pos());
  settings.setValue("helpwindow/size", size());
}

void HelpWindow::readSettings() {
  QSettings settings("FreeMat", "FreeMat");
  QPoint pos = settings.value("helpwindow/pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("helpwindow/size", QSize(500, 300)).toSize();
  resize(size);
  move(pos);
  QByteArray state = settings.value("helpwindow/state").toByteArray();
  restoreState(state);  
}

void HelpWindow::createActions() {
  zoominAct = new QAction(QIcon(":/images/zoomin.png"),"Zoom In",this);
  connect(zoominAct,SIGNAL(triggered()),tb,SLOT(zoomIn()));
  zoomoutAct = new QAction(QIcon(":/images/zoomout.png"),"Zoom Out",this);
  connect(zoomoutAct,SIGNAL(triggered()),tb,SLOT(zoomOut()));
  copyAct = new QAction(QIcon(":/images/copy.png"),"&Copy Text",this);
  connect(copyAct,SIGNAL(triggered()),tb,SLOT(copy()));
  exitAct = new QAction(QIcon(":/images/quit.png"),"&Exit Help",this);
  connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));
  forwardAct = new QAction(QIcon(":/images/next.png"),"Next",this);
  connect(forwardAct,SIGNAL(triggered()),tb,SLOT(forward()));
  backAct = new QAction(QIcon(":/images/previous.png"),"Previous",this);
  connect(backAct,SIGNAL(triggered()),tb,SLOT(backward()));
  homeAct = new QAction(QIcon(":/images/home.png"),"Home",this);
  connect(homeAct,SIGNAL(triggered()),tb,SLOT(home()));
  connect(tb,SIGNAL(forwardAvailable(bool)),forwardAct,SLOT(setEnabled(bool)));
  connect(tb,SIGNAL(backwardAvailable(bool)),backAct,SLOT(setEnabled(bool)));
}

void HelpWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(exitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(zoominAct);
  editMenu->addAction(zoomoutAct);
  goMenu = menuBar()->addMenu("&Go");
  goMenu->addAction(forwardAct);
  goMenu->addAction(backAct);
  goMenu->addAction(homeAct);
}

void HelpWindow::createToolBars() {
  goToolBar = addToolBar("Go");
  goToolBar->setObjectName("GoToolBar");
  goToolBar->addAction(backAct);
  goToolBar->addAction(forwardAct);
  goToolBar->addAction(homeAct); 
  editToolBar = addToolBar("Edit");
  editToolBar->setObjectName("EditToolBar");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(zoominAct);
  editToolBar->addAction(zoomoutAct);
}

void HelpWindow::createStatusBar() {
  statusBar()->showMessage("Ready");
}

