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
  filetoolAct = new QAction("&File Browser",this);
  connect(filetoolAct,SIGNAL(triggered()),this,SLOT(filetool()));
  historyAct = new QAction("&History",this);
  connect(historyAct,SIGNAL(triggered()),this,SLOT(history()));
  editorAct = new QAction("&Editor",this);
  connect(editorAct,SIGNAL(triggered()),this,SLOT(editor()));
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
  pauseAct = new QAction(QIcon(":/images/player_pause.png"),"&Pause",this);
  continueAct = new QAction(QIcon(":/images/player_play.png"),"&Continue",this);
  stopAct = new QAction(QIcon(":/images/player_stop.png"),"&Stop",this);
}

void ApplicationWindow::createMenus() {
  fileMenu = menuBar()->addMenu("&File");
  fileMenu->addAction(saveAct);
  fileMenu->addAction(quitAct);
  editMenu = menuBar()->addMenu("&Edit");
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
  editMenu->addAction(fontAct);
  debugMenu = menuBar()->addMenu("&Debug");
  debugMenu->addAction(pauseAct);
  debugMenu->addAction(continueAct);
  debugMenu->addAction(stopAct);
  toolsMenu = menuBar()->addMenu("&Tools");
  toolsMenu->addAction(editorAct);
  toolsMenu->addAction(pathAct);
  toolsMenu->addAction(filetoolAct);
  toolsMenu->addAction(historyAct);
  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(manualAct);
  helpMenu->addAction(aboutQt);
}

void ApplicationWindow::createToolBars() {
  editToolBar = addToolBar("Edit");
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
  editToolBar->setObjectName("edittoolbar");
  debugToolBar = addToolBar("Debug");
  debugToolBar->addAction(pauseAct);
  debugToolBar->addAction(continueAct);
  debugToolBar->addAction(stopAct);
  debugToolBar->setObjectName("debugtoolbar");
}

void ApplicationWindow::createStatusBar() {
  statusBar()->showMessage("Ready");
}

ApplicationWindow::ApplicationWindow() : QMainWindow() {
  setWindowIcon(QPixmap(":/images/freemat-2.xpm"));
  setWindowTitle(QString(FreeMat::WalkTree::getVersionString().c_str()) + " Command Window");
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  initializeTools();
  createToolBox();
  setObjectName("appwindow");
}

void ApplicationWindow::createToolBox() {
  m_tool = new ToolDock(this);
  addDockWidget(Qt::RightDockWidgetArea, m_tool);
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
  QPoint pos = settings.value("mainwindow/pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("mainwindow/size", QSize(600, 400)).toSize();
  resize(size);
  move(pos);
  QByteArray state = settings.value("mainwindow/state").toByteArray();
  restoreState(state);
}

void ApplicationWindow::writeSettings() {
  QSettings settings("FreeMat", "FreeMat");
  settings.setValue("mainwindow/state",saveState());
  settings.setValue("mainwindow/pos", pos());
  settings.setValue("mainwindow/size", size());
  settings.sync();
}

void ApplicationWindow::tclose() {
  close();
}

void ApplicationWindow::SetGUITerminal(QTTerm* term) {
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
}

void ApplicationWindow::SetKeyManager(KeyManager *keys) {
  m_keys = keys;
  connect(keys,SIGNAL(SendCommand(QString)),
 	  m_tool->getHistoryWidget(),SLOT(addCommand(QString)));
  connect(m_tool->getHistoryWidget(),SIGNAL(sendCommand(QString)),
 	  keys,SLOT(QueueCommand(QString)));
  connect(m_tool->getFileTool(),SIGNAL(sendCommand(QString)),
 	  keys,SLOT(QueueString(QString)));
  connect(pauseAct,SIGNAL(triggered()),m_keys,SLOT(RegisterInterrupt()));
  connect(continueAct,SIGNAL(triggered()),m_keys,SLOT(ContinueAction()));
  connect(stopAct,SIGNAL(triggered()),m_keys,SLOT(StopAction()));
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
  if (!text.isNull())
    m_keys->QueueMultiString(text);
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
  emit startEditor();
}

void ApplicationWindow::path() {
  emit startPathTool();
}

void ApplicationWindow::filetool() {
  m_tool->show();
  m_tool->getFileTool()->show();
}

void ApplicationWindow::history() {
  m_tool->show();
  m_tool->getHistoryWidget()->show();
}

class AboutWindow : public QWidget {
 public:
  AboutWindow() : QWidget() {}
  void paintEvent(QPaintEvent *e);
};

void AboutWindow::paintEvent(QPaintEvent * e) {
  QWidget::paintEvent(e);
  QPainter painter(this);
  QLinearGradient grad1(0,0,100,100);
  grad1.setColorAt(0, QColor(255,0,0,127));
  grad1.setColorAt(1, QColor(0,0,255,0));
  painter.setPen(QPen(grad1,0));
  for (int y=12;y < 100; y += 12)
    painter.drawText(0, y, "Thanks to you!!!");
}

void ApplicationWindow::about() {
  AboutWindow *win = new AboutWindow;
  win->show();
  QMessageBox mb(this);
  mb.setWindowTitle(QString("About FreeMat"));
  mb.setText(QString("<h3>About FreeMat</h3>"
		     "<p>This is %1.</p>"
		     "<p>FreeMat is licensed under the GNU Public License"
		     " version 2.  Type <code>license</code> at the prompt"
		     " to get details.  The source code for FreeMat is"
		     " downloadable at <tt>http://freemat.sf.net</tt>."
		     "<p>FreeMat is written and maintained by Samit Basu"
		     " and uses contributions from the following people"
		     " and projects:"
		     " <ul>"
		     " <li> Bruno De Man - General suggestions and support </li>"
		     " <li> Brian Yanoff - Scripts </li>"
		     " <li> Jeff Fessler - Support and test code </li>"
		     " <li> ATLAS - For optimized BLAS "
		     " <tt>http://math-atlas.sourceforge.net</tt></li>"
		     " <li> LAPACK - For linear algebra  "
		     " <tt>http://www.netlib.org/lapack</tt></li>"
		     " <li> Umfpack - Sparse linear equations "
		     " <tt>http://www.cise.ufl.edu/research/sparse/umfpack</tt></li>"
		     " <li> ARPACK - Sparse eigenvalue problems  "
		     " <tt>http://www.caam.rice.edu/software/ARPACK</tt></li>"
		     " <li> FFTW - Fast Fourier Transforms  "
		     " <tt>http://www.fftw.org</tt></li>"
		     " <li> ffcall - Foreign Function interface "
		     " <tt>http://www.haible.de/bruno/packages-ffcall.html</tt></li>"
		     " <li> Qt4 - Cross platform GUI and API "
		     " <tt>http://www.trolltech.com</tt></li>"
		     " <li> qconf - Configure script generator"
		     " <tt>http://delta.affinix.com/qconf</tt></li>"
		     " <li> libtecla - The console interface uses code"
		     " from this library <tt>http://www.astro.caltech.edu/"
		     "~mcs/tecla/</tt></li>"
		     " <li> wxbasic - Inspiration for the layout of the"
		     " interpreter <tt>http://wxbasic.sourceforge.net</tt></li>"
		     " <li> nsis - Installer on Win32"
		     " </ul>").arg(QString::fromStdString(FreeMat::WalkTree::getVersionString())));
  mb.setIconPixmap(QPixmap(":/images/freemat-2.xpm"));
  mb.setButtonText(0, "OK");
  mb.exec();
}


