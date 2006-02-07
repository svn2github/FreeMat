#include <QDir>
#include <QtGui>
#include <QDebug>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "GUITerminal.hpp"
#include "MainApp.hpp"
#include <qapplication.h>
#include "Exception.hpp"
#include "application.h"
#include "KeyManager.hpp"

#include "LinearEqSolver.hpp"
#include "helpgen.hpp"

using namespace FreeMat;

KeyManager *term;

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
  term->RestoreOriginalMode();
  printf("Suspending FreeMat...\n");
  fflush(stdout);
  signal(SIGTSTP,signal_suspend_default);
  raise(SIGTSTP);
}

void signal_resume(int a) {
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  printf("Resuming FreeMat...\n");
  term->SetRawMode();
  term->Redisplay();
}

void signal_resize(int a) {
  term->ResizeEvent();
}

void stdincb() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1)
    ((Terminal*)term)->ProcessChar(c);
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


int main(int argc, char *argv[]) {
  QCoreApplication *app;
  
  int nogui = parseFlagArg(argc,argv,"-nogui",false);
  int scriptMode = parseFlagArg(argc,argv,"-e",false); 
  int helpgen = parseFlagArg(argc,argv,"-helpgen",false);
  int noX = parseFlagArg(argc,argv,"-noX",false);
  int help = parseFlagArg(argc,argv,"-help",false);
  int funcMode = parseFlagArg(argc,argv,"-f",true);
  
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
  
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  dir.cd("Plugins");
  QString dummy(dir.absolutePath());
  QApplication::setLibraryPaths(QStringList(dir.absolutePath()));

  if (funcMode) {
    scriptMode = 0;
    nogui = 1;
    helpgen = 0;
  }
  
  if (scriptMode) nogui = 1;
  
  MainApp m_app;
  ApplicationWindow *m_win = NULL;
  
  if (!nogui) {
    m_win = new ApplicationWindow;
    QObject::connect(qApp,SIGNAL(lastWindowClosed()),qApp,SLOT(quit()));
    term = new GUITerminal(m_win);
    m_win->SetGUITerminal((GUITerminal*)term);
    m_win->show();
    ((GUITerminal*)term)->resizeTextSurface();
    ((GUITerminal*)term)->setFocus();
  } else {
#ifdef Q_WS_X11
    if (!scriptMode && !funcMode) {
      QWidget *wid = new QWidget(0,Qt::FramelessWindowHint);
      wid->setGeometry(2000,2000,1,1);
      wid->setWindowIcon(QIcon(":/images/freemat-2.xpm"));
      wid->show();
      term = new Terminal;
      fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
      try {
	term->Initialize();
      } catch(Exception &e) {
	fprintf(stderr,"Unable to initialize terminal.  Try to start FreeMat with the '-e' option.");
	exit(1);
      }
      QSocketNotifier *notify = new QSocketNotifier(STDIN_FILENO,QSocketNotifier::Read);
      SocketCB *socketcb = new SocketCB(stdincb);
      QObject::connect(notify, SIGNAL(activated(int)), socketcb, SLOT(activated(int)));
    } else if (!funcMode)
      term = new DumbTerminal;
    else
      term = new FuncTerminal(argv,argc,funcMode);
    signal_suspend_default = signal(SIGTSTP,signal_suspend);
    signal_resume_default = signal(SIGCONT,signal_resume);
    signal(SIGWINCH, signal_resize);
#else
    fprintf(stderr,"-nogui flag is unsupported on win32/macosx\n");
    exit(1);
#endif
  }
  m_app.SetTerminal(term);
  m_app.SetGUIMode(!noX);
  m_app.SetSkipGreeting(funcMode);
  QTimer::singleShot(0,&m_app,SLOT(Run()));
  if (m_win != NULL)
    QObject::connect(m_win,SIGNAL(startHelp()),&m_app,SLOT(HelpWin()));
  return app->exec();
}
