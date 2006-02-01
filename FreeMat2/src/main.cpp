#include <QDir>
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

using namespace FreeMat;

KeyManager *term;

#ifdef Q_WS_X11
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
  QApplication app(argc, argv);
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  dir.cd("Plugins");
  QString dummy(dir.absolutePath());
  QApplication::setLibraryPaths(QStringList(dir.absolutePath()));


  int nogui = parseFlagArg(argc,argv,"-nogui",false);
  int scriptMode = parseFlagArg(argc,argv,"-e",false); 

  if (scriptMode) nogui = 1;

  MainApp m_app;
  ApplicationWindow *m_win = NULL;

  if (!nogui) {
    m_win = new ApplicationWindow;
    QObject::connect(qApp,SIGNAL(lastWindowClosed()),qApp,SLOT(quit()));
    term = new GUITerminal(m_win);
    m_win->SetGUITerminal((GUITerminal*)term);
    m_win->resize(400,300);
    m_win->show();
    ((GUITerminal*)term)->resizeTextSurface();
    ((GUITerminal*)term)->setFocus();
  } else {
#ifdef Q_WS_X11
    if (!scriptMode) {
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
    } else
      term = new DumbTerminal;

    double *a, *b, *c;
    int m, n;
    m = 20; n = 20;
    b = new double[20*20];
    a = new double[20*20];
    for (int i=0;i<20*20;i++) {
      a[i] = drand48();
      b[i] = drand48();
    }
    c = new double[20*20];
    doubleSolveLinEq(term,m,n,c,a,b);

    signal_suspend_default = signal(SIGTSTP,signal_suspend);
    signal_resume_default = signal(SIGCONT,signal_resume);
    signal(SIGWINCH, signal_resize);
#else
    fprintf(stderr,"-nogui flag is unsupported on win32/macosx\n");
    exit(1);
#endif
  }
  m_app.SetTerminal(term);
  QTimer::singleShot(0,&m_app,SLOT(Run()));
  if (m_win != NULL)
    QObject::connect(m_win,SIGNAL(startHelp()),&m_app,SLOT(HelpWin()));
  return app.exec();
}
