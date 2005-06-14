#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "GUITerminal.hpp"
#include "MainApp.hpp"
#include <qapplication.h>
#include "Exception.hpp"
#include "DumbTerminal.hpp"
#include <qmainwindow.h>

using namespace FreeMat;

#ifndef WIN32
#include "Terminal.hpp"
#include "SocketCB.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <qsocketnotifier.h>

sig_t signal_suspend_default;
sig_t signal_resume_default;

BaseTerminal *term;

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
  QApplication app(argc, argv, TRUE);
  int nogui = parseFlagArg(argc,argv,"-nogui",false);
  int scriptMode = parseFlagArg(argc,argv,"-e",false); 

  if (scriptMode) nogui = 1;

  MainApp m_app;
  QTimer *m_start;
  QMainWindow *m_win;

  if (!nogui) {
    m_win = new QMainWindow;
    term = new GUITerminal(m_win);
    ((GUITerminal*)term)->show();
    //    ((GUITerminal*)term)->reparent(m_win,QPoint(0,0));
    m_win->setCentralWidget((GUITerminal*)term);
    m_win->setMinimumSize(400,300);
    m_win->show();
    ((GUITerminal*)term)->resizeTextSurface();
    ((GUITerminal*)term)->setFocus();
//     m_win->setFocus();
  } else {
#ifndef WIN32
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
    signal_suspend_default = signal(SIGTSTP,signal_suspend);
    signal_resume_default = signal(SIGCONT,signal_resume);
    signal(SIGWINCH, signal_resize);
#else
    fprintf(stderr,"-nogui flag is unsupported on win32\n");
    exit(1);
#endif
  }
  m_app.SetTerminal(term);
  m_start = new QTimer;
  QObject::connect(m_start,SIGNAL(timeout()),&m_app,SLOT(Run()));
  m_start->start(10,TRUE);
  return app.exec();
}
