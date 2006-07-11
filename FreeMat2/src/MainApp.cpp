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

#ifdef Q_WS_X11 
#include "FuncTerminal.hpp"
#include "DumbTerminal.hpp"
#include "Terminal.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <qsocketnotifier.h>
#include <signal.h>
#include <unistd.h>

sig_t signal_suspend_default;
sig_t signal_resume_default;

Terminal* gterm;

void signal_suspend(int a) {
  Terminal *tptr = dynamic_cast<Terminal*>(gterm);
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
  Terminal *tptr = dynamic_cast<Terminal*>(gterm);
  if (tptr) {
    tptr->SetRawMode();
  }
}

void signal_resize(int a) {
  Terminal *tptr = dynamic_cast<Terminal*>(gterm);
  if (tptr) {
    tptr->ResizeEvent();
  }
}

#endif

MainApp::MainApp() {
  guimode = true;
  skipGreeting = false;
  m_keys = new KeyManager;
}

MainApp::~MainApp() {
}

void MainApp::HelpWin() {
  ArrayVector dummy;
  HelpWinFunction(0,dummy,irun->GetInterpreter());
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
  gterm = myterm;
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
  m_term = myterm;
  QObject::connect(this,SIGNAL(Shutdown()),qApp,SLOT(quit()));
#endif
}

void MainApp::PathTool() {
  ArrayVector dummy;
  PathToolFunction(0,dummy,irun->GetInterpreter());
}

void MainApp::Editor() {
  ArrayVector dummy;
  EditorFunction(0,dummy,irun->GetInterpreter());
}

void MainApp::SetGUIMode(bool mode) {
  guimode = mode;
}

void MainApp::SetSkipGreeting(bool skip) {
  skipGreeting = skip;
}

void MainApp::TerminalReset() {
#ifdef Q_WS_X11
  Terminal *tptr = dynamic_cast<Terminal*>(gterm);
  if (tptr)
    tptr->RestoreOriginalMode();
#endif  
}

void MainApp::ExecuteLine(std::string txt) {
  irun->ExecuteLine(txt);
}

int MainApp::Run() {
  qDebug("Starting interpreter...\n");
  irun = new InterpreterThread;
  irun->Setup();
  qRegisterMetaType<std::string>("std::string");
  connect(m_keys,SIGNAL(ExecuteLine(std::string)),this,SLOT(ExecuteLine(std::string)));
  connect(irun->GetInterpreter(),SIGNAL(outputRawText(std::string)),m_term,SLOT(OutputRawString(std::string)));
  connect(irun,SIGNAL(Ready()),m_keys,SLOT(Ready()));
  irun->start();
  return 0;
}
