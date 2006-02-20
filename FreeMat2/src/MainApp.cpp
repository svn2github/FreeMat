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
#include <qapplication.h>
#include <QDir>
#include <QDebug>
#include <QtCore>
#include "Common.hpp"
#include "MainApp.hpp"
using namespace FreeMat;

#include "KeyManager.hpp"
#include "File.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "HandleCommands.hpp"
#include "Core.hpp"
#include "DumbTerminal.hpp"
#include "Terminal.hpp"
#include <fcntl.h>
#include <qsocketnotifier.h>
#include "SocketCB.hpp"
#include "HandleCommands.hpp"
#include "QTTerm.hpp"

QObject *term;
KeyManager *keys;

#ifdef Q_WS_X11
#include "FuncTerminal.hpp"
#include "DumbTerminal.hpp"
#include "Terminal.hpp"
#include "SocketCB.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <qsocketnotifier.h>
#include <signal.h>

sig_t signal_suspend_default;
sig_t signal_resume_default;

void signal_suspend(int a) {
  Terminal *tptr = dynamic_cast<Terminal*>(term);
  if (tptr)
    tptr->RestoreOriginalMode();
  printf("Suspending FreeMat...\n");
  fflush(stdout);
  signal(SIGTSTP,signal_suspend_default);
  raise(SIGTSTP);
}

void signal_resume(int a) {
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  printf("Resuming FreeMat...\n");
  Terminal *tptr = dynamic_cast<Terminal*>(term);
  if (tptr) {
    tptr->SetRawMode();
    keys->Redisplay();
  }
}

void signal_resize(int a) {
  Terminal *tptr = dynamic_cast<Terminal*>(term);
  if (tptr) {
    tptr->ResizeEvent();
  }
}
#endif

void MainApp::TerminalReset() {
  Terminal *tptr = dynamic_cast<Terminal*>(term);
  if (tptr) {
    tptr->RestoreOriginalMode();
  }
}

MainApp::MainApp() {
  guimode = true;
  skipGreeting = false;
  m_keys = new KeyManager;
}

MainApp::~MainApp() {
}

void MainApp::HelpWin() {
  ArrayVector dummy;
  HelpWinFunction(0,dummy,eval);
}

void MainApp::SetupGUICase() {
  m_win = new ApplicationWindow;
  QTTerm *gui = new QTTerm(NULL);
  m_keys->RegisterTerm(gui);
  gui->resizeTextSurface();
  gui->show();
  m_win->SetGUITerminal(gui);
  m_win->SetKeyManager(m_keys);
  m_win->readSettings();
  m_win->show();
  gui->setFocus();
  QObject::connect(m_win,SIGNAL(startHelp()),this,SLOT(HelpWin()));
  QObject::connect(m_win,SIGNAL(startEditor()),this,SLOT(Editor()));
  QObject::connect(m_win,SIGNAL(startPathTool()),this,SLOT(PathTool()));
  QObject::connect(qApp,SIGNAL(aboutToQuit()),m_win,SLOT(writeSettings()));
  QObject::connect(qApp,SIGNAL(lastWindowClosed()),qApp,SLOT(quit()));
  QObject::connect(this,SIGNAL(Shutdown()),m_win,SLOT(close()));
  QObject::connect(this,SIGNAL(Initialize()),m_win,SLOT(init()));
  m_term = gui;
}

void MainApp::SetupInteractiveTerminalCase() {
#ifdef Q_WS_X11
  FreeMat::SetNonGUIHack();
  Terminal *myterm = new Terminal;
  m_keys->RegisterTerm(myterm);
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  try {
    myterm->Initialize();
  } catch(Exception &e) {
    fprintf(stderr,"Unable to initialize terminal.  Try to start FreeMat with the '-e' option.");
    exit(1);
  }
  QSocketNotifier *notify = new QSocketNotifier(STDIN_FILENO,QSocketNotifier::Read);
  QObject::connect(notify, SIGNAL(activated(int)), myterm, SLOT(DoRead()));
  myterm->ResizeEvent();
  signal_suspend_default = signal(SIGTSTP,signal_suspend);
  signal_resume_default = signal(SIGCONT,signal_resume);
  signal(SIGWINCH, signal_resize);
  m_term = myterm;
  QObject::connect(this,SIGNAL(Shutdown()),qApp,SLOT(quit()));
#endif
}

KeyManager* MainApp::GetKeyManager() {
  return m_keys;
}

void MainApp::SetupDumbTerminalCase() {
#ifdef Q_WS_X11
  FreeMat::SetNonGUIHack();
  DumbTerminal *myterm = new DumbTerminal;
  m_keys->RegisterTerm(myterm);
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  QSocketNotifier *notify = new QSocketNotifier(STDIN_FILENO,QSocketNotifier::Read);
  QObject::connect(notify, SIGNAL(activated(int)), myterm, SLOT(DoRead()));
  signal_suspend_default = signal(SIGTSTP,signal_suspend);
  signal_resume_default = signal(SIGCONT,signal_resume);
  signal(SIGWINCH, signal_resize);
  m_term = term;
  QObject::connect(this,SIGNAL(Shutdown()),qApp,SLOT(quit()));
#endif
}

void MainApp::PathTool() {
  ArrayVector dummy;
  PathToolFunction(0,dummy,eval);
}

void MainApp::Editor() {
  ArrayVector dummy;
  EditorFunction(0,dummy,eval);
}

void MainApp::SetGUIMode(bool mode) {
  guimode = mode;
}

void MainApp::SetSkipGreeting(bool skip) {
  skipGreeting = skip;
}

int MainApp::Run() {
  Context *context = new Context;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  if (guimode) {
    LoadGUICoreFunctions(context);
    LoadHandleGraphicsFunctions(context);  
  }
  m_keys->setContext(context);
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  dir.cd("Plugins");
  QString dummy(dir.absolutePath());
  QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
  QDir dir1(qApp->applicationDirPath() + "/../Resources/mfiles");
  QStringList basePath;
  if (dir1.exists()) {
    QString path1(dir1.canonicalPath());
    basePath += GetRecursiveDirList(path1);
  }
  QDir dir2(qApp->applicationDirPath() + "/../Resources/help/text");
  if (dir2.exists()) {
    QString path2(dir2.canonicalPath());
    basePath += GetRecursiveDirList(path2);
  }
  m_keys->setBasePath(basePath);
  QSettings settings("FreeMat","FreeMat");
  QStringList userPath = settings.value("interpreter/path").toStringList();
  m_keys->setUserPath(userPath);
  m_keys->setAppPath(qApp->applicationDirPath().toStdString());
  m_keys->rescanPath();
  eval = new WalkTree(context,m_keys);
  emit Initialize();
  if (!skipGreeting)
    eval->sendGreeting();
  eval->run();
  TerminalReset();
  emit Shutdown();
  return 0;
}
