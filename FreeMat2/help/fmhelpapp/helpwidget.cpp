#include "helpwidget.hpp"
#include <QtGui>
#include <QDebug>

HelpWinManager::HelpWinManager(QWidget *parent) : QTabWidget(parent) {
  connect(this,SIGNAL(currentChanged(int)),this,SLOT(activeChanged(int)));
}

void HelpWinManager::forward() {
  QTextBrowser *tb = qobject_cast<QTextBrowser*>(currentWidget());
  tb->forward();
}

void HelpWinManager::back() {
  QTextBrowser *tb = qobject_cast<QTextBrowser*>(currentWidget());
  tb->backward();
}

void HelpWinManager::home() {
  QTextBrowser *tb = qobject_cast<QTextBrowser*>(currentWidget());
  tb->home();
}

void HelpWinManager::forwardFlag(bool avail) {
  qDebug("forward flag is %d",avail);
//   if (m_forwardFlag != avail) {
//     m_forwardFlag = avail;
    emit forwardAvailable(avail);
//   }
}

void HelpWinManager::backwardFlag(bool avail) {
  qDebug("backward flag is %d",avail);
//   if (m_backwardFlag != avail) {
//     m_backwardFlag = avail;
    emit backwardAvailable(avail);
//   }
}

void HelpWinManager::activeChanged(int num) {
  qDebug("active changed %d",num);
  QTextBrowser *tb = qobject_cast<QTextBrowser*>(currentWidget());
  disconnect(tb,SIGNAL(forwardAvailable(bool)),0,0);
  disconnect(tb,SIGNAL(backwardAvailable(bool)),0,0);
  connect(tb,SIGNAL(forwardAvailable(bool)),this,SLOT(forwardFlag(bool)));
  connect(tb,SIGNAL(backwardAvailable(bool)),this,SLOT(backwardFlag(bool)));
  emit forwardAvailable(m_forwardFlag);
  emit backwardAvailable(m_backwardFlag);
}

HelpWidget::HelpWidget(QString url) {
  m_browser = new QTabWidget(this);
  setWidget(m_browser);
  QListWidget *m_flist = new QListWidget;
  // Populate the list widget
  QFile *file = new QFile(url + "\\modules.txt");
  if (!file->open(QFile::ReadOnly | QIODevice::Text))
    QMessageBox::warning(this,"Cannot Find Module List","The file modules.txt is missing from the directory "+url+" where I think help files should be.  The Topic List widget will not function properly.",QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  else {
    QTextStream t(file);
    while (!file->atEnd()) {
      QString line(t.readLine());
      qDebug() << line;
      new QListWidgetItem(line,m_flist);
    }
  }
  delete file;
  new QListWidgetItem("atan",m_flist);
  new QListWidgetItem("cos",m_flist);
//   new QListWidgetItem("sin",m_flist);
//   new QListWidgetItem("asin",m_flist);
  m_browser->addTab(m_flist,"Topic List");
  QTreeWidget *m_tindex = new QTreeWidget;
  m_tindex->setColumnCount(1);
  m_tindex->setHeaderLabels(QStringList() << "FreeMat 2.0");
  QTreeWidgetItem *cities = new QTreeWidgetItem(m_tindex);
  cities->setText(0, tr("Cities"));
  QTreeWidgetItem *osloItem = new QTreeWidgetItem(cities);
  osloItem->setText(0, tr("Oslo"));
  QTreeWidgetItem *yorkItem = new QTreeWidgetItem(cities);
  yorkItem->setText(0, "New York");
  QTreeWidgetItem *planb = new QTreeWidgetItem(cities);
  //  planb->setText(0, "Hello");
  m_browser->addTab(m_tindex,"Index");
  m_tindex->setItemExpanded(cities,true);
  //  m_browser = new QTextBrowser(this);
  //  m_browser->setSource(QUrl("file:///qt/4.1.0/doc/html/index.html"));
  //  setWidget(m_browser);
}

HelpWindow::HelpWindow(QString url) {
  m_initial = url;
  m_tabs = new HelpWinManager(this);
  setCentralWidget(m_tabs);
  m_helpwidget = new HelpWidget(url);
  addDockWidget(Qt::LeftDockWidgetArea,m_helpwidget);
  //  setCentralWidget(m_helpwidget);
  //  m_helpwidget->show();
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  newTab();
}

void HelpWindow::newTab() {
  QTextBrowser *tb = new QTextBrowser;
  tb->setLineWrapMode(QTextEdit::WidgetWidth);
  m_tabs->addTab(tb,"Help Browser");
  tb->setSource("file://" + m_initial+"/index.html");
}

void HelpWindow::closeTab() {
  if (m_tabs->count() == 0) return;
  QWidget *widget = m_tabs->currentWidget();
  m_tabs->removeTab(m_tabs->currentIndex());
  delete widget;
}

void HelpWindow::createActions() {
  newAct = new QAction("&New Tab",this);
  connect(newAct,SIGNAL(triggered()),this,SLOT(newTab()));
  closeAct = new QAction("&Close Tab",this); 
  connect(closeAct,SIGNAL(triggered()),this,SLOT(closeTab()));
  copyAct = new QAction("&Copy Text",this);
  pasteAct = new QAction("&Paste Text",this);  
  exitAct = new QAction("&Exit Help",this);
  forwardAct = new QAction("Next",this);
  connect(forwardAct,SIGNAL(triggered()),m_tabs,SLOT(forward()));
  backAct = new QAction("Previous",this);
  connect(backAct,SIGNAL(triggered()),m_tabs,SLOT(back()));
  homeAct = new QAction("Home",this);
  connect(homeAct,SIGNAL(triggered()),m_tabs,SLOT(home()));
  connect(m_tabs,SIGNAL(forwardAvailable(bool)),forwardAct,SLOT(setEnabled(bool)));
  connect(m_tabs,SIGNAL(backwardAvailable(bool)),backAct,SLOT(setEnabled(bool)));
}

void HelpWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(newAct);
  fileMenu->addAction(closeAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
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
  fileToolBar = addToolBar("File");
  fileToolBar->addAction(newAct);
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
}

void HelpWindow::createStatusBar() {
  statusBar()->showMessage("Ready");
}

