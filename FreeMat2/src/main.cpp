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
#include <QDir>
#include <QtGui>
#include <QDebug>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "QTTerm.hpp"
#include "MainApp.hpp"
#include <qapplication.h>
#include "Exception.hpp"
#include "application.h"
#include "KeyManager.hpp"

#include "LinearEqSolver.hpp"
#include "helpgen.hpp"

using namespace FreeMat;

KeyManager *keys;
QObject *term;
QCoreApplication *app;
ApplicationWindow *m_win;
MainApp *m_app;

#ifdef Q_WS_X11
#include "FuncTerminal.hpp"
#include "DumbTerminal.hpp"
#include "Terminal.hpp"
#include "SocketCB.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <qsocketnotifier.h>

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

void usage() {
  printf("%s\n  Command Line Help\n",WalkTree::getVersionString().c_str());
  printf(" You can invoke FreeMat with the following command line options:\n");
  printf("     -f <command>  Runs FreeMat in command mode.  FreeMat will \n");
  printf("                   startup, run the given command, and then quit.\n");
  printf("                   Note that this option uses the remainder of the\n");
  printf("                   command line, so use it last.\n");
#ifdef Q_WS_X11
  printf("     -nogui        Suppress the GUI for FreeMat.\n");
#endif
  printf("     -noX          Disables the graphics subsystem.\n");
  printf("     -e            uses a dumb terminal interface \n");
  printf("                   (no command line editing, etc.)\n");
  printf("                   This flag is primarily used when \n");
  printf("                   you want to capture input/output\n");
  printf("                   to FreeMat from another application.\n");
  printf("     -help         Get this help text\n");
  exit(0);
}


// Search through the arguments to freemat... look for the given
// flag.  if the flagarg variable is true, then an argument must
// be provided to the flag.  If the flag is not found, then a 
// 0 is returned.  Otherwise, the index into argv of the flag is
// returned.
int parseFlagArg(int argc, char *argv[], const char* flagstring, bool flagarg) {
  bool flagFound = false;
  int ndx;
  ndx = 1;
  while (!flagFound && ndx < argc) {
    flagFound = strcmp(argv[ndx],flagstring) == 0;
    if (!flagFound) ndx++;
  }
  if (flagFound && flagarg && (ndx == argc-1)) {
    fprintf(stderr,"Error: flag %s requires an argument!\n",flagstring);
    exit(1);
  }
  if (!flagFound)
    ndx = 0;
  return ndx;
}

void SetupGUICase() {
  m_win = new ApplicationWindow;
  QTTerm *gui = new QTTerm(m_win,"terminal");
  keys->RegisterTerm(gui);
  m_win->SetGUITerminal(gui);
  m_win->SetKeyManager(keys);
  m_win->show();
  gui->resizeTextSurface();
  gui->setFocus();
  QObject::connect(m_win,SIGNAL(startHelp()),m_app,SLOT(HelpWin()));
  QObject::connect(m_win,SIGNAL(startEditor()),m_app,SLOT(Editor()));
  QObject::connect(m_win,SIGNAL(startPathTool()),m_app,SLOT(PathTool()));
  QObject::connect(app,SIGNAL(aboutToQuit()),m_win,SLOT(writeSettings()));
  QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
  term = gui;
}

void SetupInteractiveTerminalCase() {
#ifdef Q_WS_X11
  Terminal *myterm = new Terminal;
  keys->RegisterTerm(myterm);
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
  term = myterm;
#endif
}

void SetupDumbTerminalCase() {
#ifdef Q_WS_X11
  DumbTerminal *myterm = new DumbTerminal;
  keys->RegisterTerm(myterm);
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  QSocketNotifier *notify = new QSocketNotifier(STDIN_FILENO,QSocketNotifier::Read);
  QObject::connect(notify, SIGNAL(activated(int)), myterm, SLOT(DoRead()));
  signal_suspend_default = signal(SIGTSTP,signal_suspend);
  signal_resume_default = signal(SIGCONT,signal_resume);
  signal(SIGWINCH, signal_resize);
  term = myterm;  
#endif
}

int main(int argc, char *argv[]) {  
  int nogui = parseFlagArg(argc,argv,"-nogui",false);
  int scriptMode = parseFlagArg(argc,argv,"-e",false); 
  int helpgen = parseFlagArg(argc,argv,"-helpgen",false);
  int noX = parseFlagArg(argc,argv,"-noX",false);
  int help = parseFlagArg(argc,argv,"-help",false);
  int funcMode = parseFlagArg(argc,argv,"-f",true);
  int nogreet = parseFlagArg(argc,argv,"-nogreet",false);
  
  if (help) usage();
  if (!noX)
    app = new QApplication(argc, argv);
  else {
    app = new QCoreApplication(argc, argv);
    nogui = true;
    helpgen = false;
  }
  
  if (helpgen) {
    DoHelpGen();
    return 0;
  }
  
  if (funcMode) {
    scriptMode = 0;
    nogui = 1;
    helpgen = 0;
  }

  if (scriptMode) nogui = 1;

  keys = new KeyManager;
  m_app = new MainApp;

  if (!nogui)
    SetupGUICase();
  else if (!scriptMode) 
    SetupInteractiveTerminalCase();
  else
    SetupDumbTerminalCase();

  m_app->SetKeyManager(keys);
  m_app->SetGUIMode(!noX);
  m_app->SetSkipGreeting(nogreet);
  QTimer::singleShot(0,m_app,SLOT(Run()));
  return app->exec();
}
