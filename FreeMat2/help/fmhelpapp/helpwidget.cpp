#include "helpwidget.hpp"
#include <QtGui>
#include <QDebug>


void HelpWindow::activateModule(QListWidgetItem* item) {
  tb->setSource("file://"+m_initial+"/"+item->text()+".html");
}

void HelpWindow::activateModule(QTreeWidgetItem* item, int) {
  QString fulltext(item->text(0));
  QRegExp modname("^\\s*(\\b[\\w]+\\b)");
  if (modname.indexIn(fulltext) < 0)
    return;
  QString module(modname.cap(1).toLower());
  tb->setSource("file://"+m_initial+"/"+module+".html");
}

HelpWidget::HelpWidget(QString url, HelpWindow *mgr) {
  m_browser = new QTabWidget(this);
  setWidget(m_browser);
  QWidget *m_listdoc = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;
  
  QListWidget *m_flist = new QListWidget;
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
  m_browser->addTab(m_flist,"Topic List");
  QTreeWidget *m_tindex = new QTreeWidget;
  connect(m_tindex,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
	  mgr,SLOT(activateModule(QTreeWidgetItem*,int)));
  m_tindex->setColumnCount(1);
  m_tindex->setHeaderLabels(QStringList() << "FreeMat 2.0");
  file = new QFile(url + "/sectable.txt");
  if (!file->open(QFile::ReadOnly | QIODevice::Text))
    QMessageBox::warning(this,"Cannot Find Section Index","The file sectable.txt is missing from the directory "+url+" where I think help files should be.  The Index widget will not function properly.",QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  else {
    QTextStream t(file);
    QTreeWidgetItem *prev;
    while (!t.atEnd()) {
      QString line(t.readLine());
      if (line[0] != QChar('+')) {
	prev = new QTreeWidgetItem(m_tindex,QStringList() << line);
      } else {
	new QTreeWidgetItem(prev,QStringList() << line.remove(0,1));
      }
    }
  }
  delete file;
  //  planb->setText(0, "Hello");
  m_browser->addTab(m_tindex,"Index");
  //  m_tindex->setItemExpanded(cities,true);
  //  m_browser = new QTextBrowser(this);
  //  m_browser->setSource(QUrl("file:///qt/4.1.0/doc/html/index.html"));
  //  setWidget(m_browser);
}

HelpWindow::HelpWindow(QString url) {
  m_initial = url;
  tb = new QTextBrowser(this);
  tb->setSource("file://" + m_initial+"/index.html");
  setCentralWidget(tb);
  m_helpwidget = new HelpWidget(url,this);
  addDockWidget(Qt::LeftDockWidgetArea,m_helpwidget);
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
}


void HelpWindow::createActions() {
  zoominAct = new QAction("Zoom In",this);
  connect(zoominAct,SIGNAL(triggered()),tb,SLOT(zoomIn()));
  zoomoutAct = new QAction("Zoom Out",this);
  connect(zoomoutAct,SIGNAL(triggered()),tb,SLOT(zoomOut()));
  copyAct = new QAction("&Copy Text",this);
  connect(copyAct,SIGNAL(triggered()),tb,SLOT(copy()));
  exitAct = new QAction("&Exit Help",this);
  connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));
  forwardAct = new QAction("Next",this);
  connect(forwardAct,SIGNAL(triggered()),tb,SLOT(forward()));
  backAct = new QAction("Previous",this);
  connect(backAct,SIGNAL(triggered()),tb,SLOT(backward()));
  homeAct = new QAction("Home",this);
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
  goToolBar->addAction(forwardAct);
  goToolBar->addAction(backAct);
  goToolBar->addAction(homeAct); 
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(zoominAct);
  editToolBar->addAction(zoomoutAct);
}

void HelpWindow::createStatusBar() {
  statusBar()->showMessage("Ready");
}

