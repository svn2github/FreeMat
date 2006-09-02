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
#include "application.hpp"
#include <qapplication.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <iostream>
#include <qfontdialog.h>
#include <qsettings.h>
#include "Interpreter.hpp"
#include "highlighter.hpp"
#include "Editor.hpp"
#include <QtGui>
#include <QDebug>
#include "PathTool.hpp"
#include "ToolDock.hpp"

const int about_linecount = 23;
const char *about_strings[] = {"Julie Maya & Neil - My Fan Club",
			       "Bruno De Man - New icon, general suggestions, support and FreeMat advocate",
			       "Thomas Beutlich - MAT file support",
			       "M. Vogel - Matlab compatibility scripts",
			       "Brian Yanoff - Compatibility scripts",
			       "Jeff Fessler - Support and test code, help with  class support",
			       "Al Danial - Help with sparse matrix support",
			       "MT19937 - Takuji Nishimura and Makoto Matsumoto's random number generator",
			       "RANLIB - Random number generator library",
			       "ATLAS - Optimized BLAS",
			       "LAPACK - Linear algebra",
			       "UMFPACK - Sparse linear equation solver",
			       "ARPACK - Sparse eigenvalue problems",
			       "FFTW - Fast Fourier Transforms",
			       "ffcall - Foreign Function interface",
			       "Qt4 - Cross platform GUI and API",
			       "qconf - Configure script generator",
			       "libmatio - MAT file interface library",
			       "libtecla - Inspiration for console interface code",
			       "wxbasic - Inspiration for interpreter layout",
			       "kde/konsole - Inspiration for QTTerm (GUI Console)",
			       "tubeplot - Written by Anders Sandberg",
			       "nsis - Installer on Win32"};

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
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
  setWindowTitle(QString(Interpreter::getVersionString().c_str()) + " Command Window");
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  initializeTools();
  createToolBox();
  setObjectName("appwindow");
  Aboutwin = new AboutWindow;
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
  connect(keys,SIGNAL(UpdateCWD()),
	  m_tool->getFileTool(),SLOT(updateCWD()));
  connect(keys,SIGNAL(UpdateVariables()),
	  m_tool->getVariablesTool(),SLOT(refresh()));
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
  text.replace('\r',"");
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


AboutWidget::AboutWidget() : QWidget() {
  linenumber = 0;
  phase = 0;
  setMinimumSize(150,100);
}

void AboutWidget::Refresh() {
  if ((phase+3) > 255)
    linenumber = (linenumber + 1) % about_linecount;
  phase = (phase+3) % 256;
  update();
}

void AboutWidget::paintEvent(QPaintEvent *e) {
  QWidget::paintEvent(e);
  QPainter painter(this);
  QFont fnt("Helvetica",16);
  painter.setFont(fnt);
  int phaseA;
  if (phase < 128)
    phaseA = qMin(255,qMax(0,phase*2));
  else
    phaseA = qMin(255,qMax(0,2*(256-phase)+5));
  painter.setPen(QColor(0,0,0,phaseA));
  painter.drawText(0,0,width(),height(),Qt::TextWordWrap | Qt::AlignCenter,about_strings[linenumber]);
}

void AboutWindow::showEvent(QShowEvent* event) {
  refresh->start(100);
  QWidget::showEvent(event);
}

void AboutWindow::hideEvent(QHideEvent* event) {
  refresh->stop();
  QWidget::hideEvent(event);
}

AboutWindow::AboutWindow() : QWidget() {
  refresh = new QTimer;
  QVBoxLayout *layout = new QVBoxLayout;
  QLabel *txt = new QLabel("FreeMat is licensed under the GNU Public License"
			   " version 2.  The source code for FreeMat is available"
			   " at http://freemat.sf.net.  FreeMat is primarily written and"
			   " maintained by Samit Basu, but relies heavily on the following people and"
			   " projects for their contributions to FreeMat:");
  txt->setAlignment(Qt::AlignHCenter);
  QFont def(txt->font());
  def.setPointSize(12);
  txt->setFont(def);
  txt->setWordWrap(true);
  layout->addWidget(txt);
  thanks = new AboutWidget;
  layout->addWidget(thanks);
  QObject::connect(refresh,SIGNAL(timeout()),thanks,SLOT(Refresh()));
  QWidget *click = new QWidget;
  QHBoxLayout *layout2 = new QHBoxLayout;
  QPushButton *ok = new QPushButton("OK");
  layout2->addWidget(ok);
  click->setLayout(layout2);
  layout->addWidget(click);
  setLayout(layout);
  connect(ok,SIGNAL(clicked()),this,SLOT(close()));
  setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
}

void ApplicationWindow::about() {
  Aboutwin->show();
}

void ApplicationWindow::init() {
  m_tool->getVariablesTool()->setContext(m_keys->GetCompletionContext());
}
