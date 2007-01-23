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
#include <QtGui>
#include "Common.hpp"
#include "MainApp.hpp"

#include "KeyManager.hpp"
#include "File.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "HandleCommands.hpp"
#include "Core.hpp"
#include "HandleList.hpp"
#include "Interpreter.hpp"

HandleList<Interpreter*> m_threadHandles;

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
  GUIHack = false;
  skipGreeting = false;
  m_keys = new KeyManager;
  m_global = new Scope("global");
}

MainApp::~MainApp() {
}

void MainApp::HelpWin() {
  ArrayVector dummy;
  HelpWinFunction(0,dummy,m_eval);
}

void MainApp::SetupGUICase() {
  m_win = new ApplicationWindow;
  QTTerm *gui = new QTTerm(NULL);
  m_keys->RegisterTerm(gui);
  gui->resizeTextSurface();
  //  gui->show();
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
  GUIHack = true;
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
  GUIHack = true;
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
  PathToolFunction(0,dummy,m_eval);
}

void MainApp::Editor() {
  ArrayVector dummy;
  EditorFunction(0,dummy,m_eval);
}

void MainApp::SetGUIMode(bool mode) {
  guimode = mode;
}

void MainApp::SetSkipGreeting(bool skip) {
  skipGreeting = skip;
}

void MainApp::Crashed() {
  TerminalReset();
  if (guimode)
    QMessageBox::critical(NULL,"FreeMat Crash","Interpreter thread crashed unexpectedly!\n  This is likely a FreeMat bug of some kind.  \nPlease file a bug report at http://freemat.sf.net.",QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
  else
    cout << "Interpreter thread crashed unexpectedly!  This is likely a FreeMat bug of some kind.  Please file a bug report at http://freemat.sf.net.";
  qApp->quit();
}

void MainApp::Quit() {
  TerminalReset();
  qApp->closeAllWindows();
  qApp->quit();
}

void MainApp::TerminalReset() {
#ifdef Q_WS_X11
  Terminal *tptr = dynamic_cast<Terminal*>(gterm);
  if (tptr)
    tptr->RestoreOriginalMode();
#endif  
}

void MainApp::UpdateTermWidth(int w) {
  m_eval->setTerminalWidth(w);
}

void MainApp::ExecuteLine(string txt) {
  m_eval->ExecuteLine(txt);
}

static bool NonGUIModeHack = false;

class NonClosable : public QWidget {
public:
  NonClosable() : QWidget(0,Qt::FramelessWindowHint) {};
  void closeEvent(QCloseEvent *ce) {ce->ignore();}
};

static NonClosable *wid = NULL;

void MainApp::CheckNonClosable() {
  if (GUIHack && !wid) {
    wid = new NonClosable;
    wid->setGeometry(0,0,1,1);
    wid->setWindowIcon(QIcon(":/images/freemat_small_mod_64.png"));
    wid->show();
  }
}

void MainApp::DoGraphicsCall(FuncPtr f, ArrayVector m, int narg) { 
  CheckNonClosable();
  try {
    ArrayVector n(f->evaluateFunction(m_eval,m,narg));
    m_eval->RegisterGfxResults(n);
  } catch (Exception& e) {
    m_eval->RegisterGfxError(e.getMessageCopy());
  }
}

//!
//@Module THREADID Get Current Thread Handle
//@@Section THREAD
//@@Usage
//The @|threadid| function in FreeMat tells you which thread
//is executing the context you are in.  Normally, this is thread
//1, the main thread.  However, if you start a new thread using
//@|newthread|, you will be operating in a new thread, and functions
//that call @|threadid| from the new thread will return their 
//handles.
//@@Example
//From the main thread, we have
//@<
//threadid
//@>
//But from a launched auxilliary thread, we have
//@<
//t_id = newthread('threadid'); waitthread(t_id);
//@>
//!
ArrayVector ThreadIDFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector() << Array::uint32Constructor(eval->getThreadID());
}

extern MainApp *m_app;

ArrayVector NewThreadFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) throw Exception("newthread requires at least one argument (the function to spawn in a thread)");
  const char *fnc = ArrayToString(arg[0]);
  // Lookup this function in base interpreter to see if it is defined
  FuncPtr val;
  if (!eval->lookupFunction(fnc, val))
    throw Exception(string("Unable to map ") + fnc + " to a defined function ");
  val->updateCode();
  if (val->scriptFlag)
    throw Exception(string("Cannot use a script as the main function in a thread."));
  // Create a new thread
  int threadID = m_app->StartNewInterpreterThread();
  // Set the thread function
  Interpreter *p_eval = m_threadHandles.lookupHandle(threadID);
  ArrayVector args(arg);
  args.pop_front();
  p_eval->setThreadFunc(val,args);
  p_eval->start();
  return ArrayVector() << Array::uint32Constructor(threadID);
}

ArrayVector ThreadValueFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) throw Exception("threadvalue requires at least one argument (thread id to retrieve value from)");
  int32 handle = ArrayToInt32(arg[0]);
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  if (!thread->wait()) throw Exception("error waiting for thread to complete");
  return thread->getThreadFuncReturn();
}

ArrayVector ThreadWaitFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 1) throw Exception("threadwait requires at least one argument (thread id to wait on)");
#error FINISHME
  int32 handle = ArrayToInt32(arg[0]);
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  if (!thread->wait()) throw Exception("error waiting for thread to complete");
  return thread->getThreadFuncReturn();
}

void LoadThreadFunctions(Context *context) {
  context->addSpecialFunction("threadid",ThreadIDFunction,0,1,NULL);
  context->addSpecialFunction("newthread",NewThreadFunction,-1,1,NULL);
  context->addSpecialFunction("threadvalue",ThreadValueFunction,1,-1,"id",NULL);
}
			 
Context *MainApp::NewContext() {
  Context *context = new Context(m_global);
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  if (guimode) {
    LoadGUICoreFunctions(context);
    LoadHandleGraphicsFunctions(context);  
  }
  LoadThreadFunctions(context);
  return context;
}

void MainApp::UpdatePaths() {
  static bool paths_set = false;
  if (!paths_set) {
    if (inBundleMode()) {
      QDir dir(QApplication::applicationDirPath());
      dir.cdUp();
      dir.cd("Plugins");
      QString dummy(dir.absolutePath());
      QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
      QDir dir1(qApp->applicationDirPath() + "/../Resources/toolbox");
      if (dir1.exists()) {
	QString path1(dir1.canonicalPath());
	basePath += GetRecursiveDirList(path1);
      }
      QDir dir2(qApp->applicationDirPath() + "/../Resources/help/text");
      if (dir2.exists()) {
	QString path2(dir2.canonicalPath());
	basePath += GetRecursiveDirList(path2);
      }
    } else {
      QSettings settings("FreeMat","FreeMat");
      QDir dir1(QString(settings.value("root").toString())+"/toolbox");   
      if (dir1.exists()) {
	QString path1(dir1.canonicalPath());
	basePath += GetRecursiveDirList(path1);
      }
      QDir dir2(QString(settings.value("root").toString())+"/help/text");
      if (dir2.exists()) {
	QString path2(dir2.canonicalPath());
	basePath += GetRecursiveDirList(path2);
      }
    }
    QSettings settings("FreeMat","FreeMat");
    userPath = settings.value("interpreter/path").toStringList();
    paths_set = true;
  }
}

int MainApp::StartNewInterpreterThread() {
  Interpreter *p_eval = new Interpreter(NewContext());
  p_eval->setBasePath(basePath);
  p_eval->setUserPath(userPath);
  p_eval->rescanPath();
  connect(p_eval,SIGNAL(outputRawText(string)),m_term,SLOT(OutputRawString(string)));
  connect(p_eval,SIGNAL(SetPrompt(string)),m_keys,SLOT(SetPrompt(string)));
  connect(p_eval,SIGNAL(doGraphicsCall(FuncPtr,ArrayVector,int)),
	  this,SLOT(DoGraphicsCall(FuncPtr,ArrayVector,int)));
  connect(p_eval,SIGNAL(CWDChanged()),m_keys,SIGNAL(UpdateCWD()));
  connect(p_eval,SIGNAL(QuitSignal()),this,SLOT(Quit()));
  connect(p_eval,SIGNAL(CrashedSignal()),this,SLOT(Crashed()));
  p_eval->setTerminalWidth(m_keys->getTerminalWidth());
  p_eval->setGreetingFlag(skipGreeting);
  int threadID = m_threadHandles.assignHandle(p_eval);
  p_eval->setThreadID(threadID);
  return threadID;
}

int MainApp::Run() {
  UpdatePaths();
  qRegisterMetaType<string>("string");
  qRegisterMetaType<FuncPtr>("FuncPtr");
  qRegisterMetaType<ArrayVector>("ArrayVector");
  connect(m_keys,SIGNAL(ExecuteLine(string)),this,SLOT(ExecuteLine(string)));
  connect(m_keys,SIGNAL(UpdateTermWidth(int)),this,SLOT(UpdateTermWidth(int)));
  // Get a new thread
  int m_mainID = StartNewInterpreterThread();
  // Assign this to the main thread
  m_eval = m_threadHandles.lookupHandle(m_mainID);
  m_keys->SetCompletionContext(m_eval->getContext());
  FunctionDef *doCLI;
  if (!m_eval->lookupFunction("docli",doCLI))
    return 0;
  m_eval->setThreadFunc(doCLI,ArrayVector());
  m_eval->start();
  emit Initialize();
  return 0;
}
