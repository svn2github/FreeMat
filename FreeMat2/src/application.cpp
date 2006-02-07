#include "application.h"
#include <qapplication.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <iostream>
#include <qfontdialog.h>
#include <qsettings.h>
#include "WalkTree.hpp"
#include "highlighter.hpp"
#include "Editor.hpp"
#include <QtGui>
#include <QDebug>
#include "PathTool.hpp"
#include "ToolDock.hpp"

#define MAKEASCII(x) x.toAscii().constData()
#include <QtGui>

ApplicationWindow::~ApplicationWindow() {
}

void ApplicationWindow::createActions() {
  editorAct = new QAction("&Editor",this);
  connect(editorAct,SIGNAL(triggered()),this,SLOT(editor()));
  historyAct = new QAction("&History",this);
  connect(historyAct,SIGNAL(triggered()),this,SLOT(history()));
  pathAct = new QAction("&Path Tool",this);
  connect(pathAct,SIGNAL(triggered()),this,SLOT(path()));
  saveAct = new QAction(QIcon(":/images/save.png"),"&Save Transcript",this);
  connect(saveAct,SIGNAL(triggered()),this,SLOT(save()));
  quitAct = new QAction(QIcon(":/images/quit.png"),"&Quit",this);
  connect(quitAct,SIGNAL(triggered()),this,SLOT(close()));
  copyAct = new QAction(QIcon(":/images/copy.png"),"&Copy",this);
  connect(copyAct,SIGNAL(triggered()),this,SLOT(copy()));
  pasteAct = new QAction(QIcon(":/images/paste.png"),"&Paste",this);
  connect(pasteAct,SIGNAL(triggered()),this,SLOT(paste()));
  fontAct = new QAction("&Font",this);
  connect(fontAct,SIGNAL(triggered()),this,SLOT(font()));
  aboutAct = new QAction("&About",this);
  connect(aboutAct,SIGNAL(triggered()),this,SLOT(about()));
  manualAct = new QAction("Online &Manual",this);
  connect(manualAct,SIGNAL(triggered()),this,SLOT(manual()));
  aboutQt = new QAction("About &Qt",this);
  connect(aboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
}

void ApplicationWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  fileMenu->addAction(quitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
  editMenu->addAction(fontAct);
  toolsMenu = menuBar()->addMenu("&Tools");
  toolsMenu->addAction(editorAct);
  toolsMenu->addAction(historyAct);
  toolsMenu->addAction(pathAct);
  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(manualAct);
  helpMenu->addAction(aboutQt);
}

void ApplicationWindow::createToolBars() {
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
}

void ApplicationWindow::createStatusBar() {
  statusBar()->showMessage("Ready");
}

ApplicationWindow::ApplicationWindow() : QMainWindow() {
  setWindowIcon(QPixmap(":/images/freemat-2.xpm"));
  setWindowTitle(QString(WalkTree::getVersionString().c_str()) + " Command Window");
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  readSettings();
  initializeTools();
  createToolBox();
}

void ApplicationWindow::createToolBox() {
  m_tool = new ToolDock(this);
  addDockWidget(Qt::LeftDockWidgetArea, m_tool);
}

void ApplicationWindow::initializeTools() {
  edit = new FMEditor;
}

void ApplicationWindow::closeEvent(QCloseEvent* ce) {
  writeSettings();
  delete m_tool;
  ce->accept();
  exit(0);
}

void ApplicationWindow::readSettings() {
  QSettings settings("FreeMat", "FreeMat");
  QPoint gpos = settings.value("mainwindow/pos", QPoint(200, 200)).toPoint();
  QSize gsize = settings.value("mainwindow/size", QSize(600, 400)).toSize();
  resize(gsize);
  move(gpos);
}

void ApplicationWindow::writeSettings() {
  QSettings settings("FreeMat", "FreeMat");
  settings.setValue("mainwindow/pos", pos());
  settings.setValue("mainwindow/size", size());
  settings.sync();
}

void ApplicationWindow::tclose() {
  close();
}

void ApplicationWindow::SetGUITerminal(GUITerminal* term) {
  m_term = term;
  setCentralWidget(term);
  QSettings settings("FreeMat","FreeMat");
  QString font = settings.value("terminal/font").toString();
  if (!font.isNull()) {
    QFont new_font;
    new_font.fromString(font);
    m_term->setFont(new_font);
  }
  term->show();
  connect(term,SIGNAL(CommandLine(QString)),
	  m_tool->getHistoryWidget(),SLOT(addCommand(QString)));
}

void ApplicationWindow::save() {
  QString fn = QFileDialog::getSaveFileName();
  if (!fn.isEmpty()) {
    FILE *fp;
    fp = fopen(MAKEASCII(fn),"w");
    if (!fp) {
      char buffer[1000];
      sprintf(buffer,"Unable to save transcript to file %s!",
	      MAKEASCII(fn));
      QMessageBox::information(this,"Error",buffer,
			       QMessageBox::Ok);
      return;
    }
    int history_count;
    int width;
    char *textbuffer;
    textbuffer = m_term->getTextSurface(history_count, width);
    char *linebuf = new char[width+1];
    for (int i=0;i<history_count;i++) {
      // scan backwards for last non ' ' char
      int j=width-1;
      while ((j>0) && (textbuffer[i*width+j] == ' '))
	j--;
      j++;
      memcpy(linebuf,textbuffer+i*width,j*sizeof(char));
      linebuf[j] = 0;
#ifdef WIN32
      fprintf(fp,"%s\r\n",linebuf);
#else
      fprintf(fp,"%s\n",linebuf);
#endif
    }
    fclose(fp);
  }
}

void ApplicationWindow::copy() {
  char *copytextbuf = m_term->getSelectionText();
  if (!copytextbuf) return;
  QClipboard *cb = QApplication::clipboard();
  cb->setText(copytextbuf, QClipboard::Clipboard);
  free(copytextbuf);
}

void ApplicationWindow::paste() {
  QClipboard *cb = QApplication::clipboard();
  QString text;
  if (cb->supportsSelection())
    text = cb->text(QClipboard::Selection);
  if (text.isNull())
    text = cb->text(QClipboard::Clipboard);
  if (!text.isNull()) {
    const char *cp = MAKEASCII(text);
    while (*cp) 
      m_term->ProcessChar(*cp++);
  }
}

void ApplicationWindow::font() {
  QFont old_font = m_term->getFont();
  bool ok;
  QFont new_font = QFontDialog::getFont(&ok, old_font, this);
  if (ok) {
    QSettings settings("FreeMat","FreeMat");
    settings.setValue("terminal/font",new_font.toString());
    m_term->setFont(new_font);
  }
}

void ApplicationWindow::manual() {
  emit startHelp();
}

void ApplicationWindow::editor() {
  edit->showNormal();
  edit->raise();
}

void ApplicationWindow::history() {
}

void ApplicationWindow::path() {
  PathTool *p = new PathTool;
  p->exec();
}

void ApplicationWindow::about() {
}


