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
#include "LoadFN.hpp"
#include "HandleCommands.hpp"
#include "HandleList.hpp"
#include "Interpreter.hpp"
#include "HandleWindow.hpp"
#include "PathSearch.hpp"
#include "DebugWin.hpp"
#include "DebugStream.hpp"
#include "Platform.hpp"
#include "MemPtr.hpp"

HandleList<Interpreter*> m_threadHandles;

extern MainApp *m_app;
static FMEditor *edit = NULL;

#ifdef Q_WS_X11 
#include "FuncTerminal.hpp"
#include "DumbTerminal.hpp"
#include "Terminal.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <qsocketnotifier.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

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
  m_global = new Scope("global",false);
  // The global scope is special
  m_global->mutexSetup();
}

MainApp::~MainApp() {
}

ArrayVector HelpWinFunction(int, const ArrayVector &, Interpreter *);

void MainApp::HelpWin() {
  ArrayVector dummy;
  HelpWinFunction(0,dummy,m_eval);
}

bool inBundleMode()  {
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  return (dir.dirName() == "Contents");
}

void MainApp::SetupGUICase() {
  if (inBundleMode()) {
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    QString dummy(dir.absolutePath());
    QApplication::setLibraryPaths(QStringList() << dir.absolutePath());
  }
  m_win = new ApplicationWindow;
  QTTerm *gui = new QTTerm;
  m_keys->RegisterTerm(gui);
  m_win->SetGUITerminal(gui);
  m_win->SetKeyManager(m_keys);
  m_win->readSettings();
  m_win->show();
  gui->setFocus();
  QObject::connect(m_win,SIGNAL(newfile()),this,SLOT(NewFile()));
  QObject::connect(m_win,SIGNAL(openfile()),this,SLOT(OpenFile()));
  QObject::connect(m_win,SIGNAL(startHelp()),this,SLOT(HelpWin()));
  QObject::connect(m_win,SIGNAL(startEditor()),this,SLOT(Editor()));
  QObject::connect(m_win,SIGNAL(startPathTool()),this,SLOT(PathTool()));
  QObject::connect(qApp,SIGNAL(lastWindowClosed()),qApp,SLOT(quit()));
  QObject::connect(this,SIGNAL(Shutdown()),m_win,SLOT(close()));
  QObject::connect(this,SIGNAL(Initialize()),m_win,SLOT(init()));
  m_term = gui;
  QObject::connect(m_term,SIGNAL(showFileAtLine(QString,int)),this,SLOT(showFileAtLine(QString,int)));
}

void createEditor(Interpreter* eval){
  if (edit == NULL) {
    edit = new FMEditor(eval);
    QObject::connect(eval, SIGNAL(RefreshBPLists()), edit, SLOT(RefreshBPLists()));
    QObject::connect(eval, SIGNAL(ShowActiveLine(QString,int)),
		     edit, SLOT(ShowActiveLine(QString,int)));
    ApplicationWindow *m_win = m_app->getApplicationWindow();
    QObject::connect(m_win,SIGNAL(shutdown()),edit,SLOT(close()));
    QObject::connect(edit,SIGNAL(checkEditorExist(bool)),m_win,SLOT(checkEditorExist(bool)));
    // Because of the threading setup, we need the keymanager to relay commands
    // from the editor to the interpreter.  
    QObject::connect(edit, SIGNAL(EvaluateText(QString)),
		     m_app->GetKeyManager(), SLOT(QueueMultiString(QString)));
    // Connect the editor to the variable update signal from the interpreter
    QObject::connect(eval, SIGNAL(updateVarView(QVariant)), edit, SLOT(updateVarView(QVariant)));
    //Ask to change current path when setting breakpoint
    QObject::connect(eval, SIGNAL(IllegalLineOrCurrentPath(QString, int)), edit,
		     SLOT(IllegalLineOrCurrentPath(QString, int)));
  }
}

void MainApp::showFileAtLine(QString fileName, int lineNumber) {
  createEditor(m_eval);
  edit->ShowActiveLine(fileName, lineNumber);
  edit->showNormal();
  edit->raise();
}

void MainApp::NewFile() {
  createEditor(m_eval);
  edit->addTabUntitled();
  edit->showNormal();
  edit->raise();
}

void MainApp::OpenFile() {
  QString currentPath = QDir::currentPath();
  QStringList fileNames = QFileDialog::getOpenFileNames(
                         m_win,
                         "Select one or more files to open",
                         currentPath,
                         "M files (*.m);;Text files (*.txt);;All files (*)");
  QStringList::Iterator it = fileNames.begin();
  if (!fileNames.isEmpty()) {
    createEditor(m_eval);
    while(it != fileNames.end()) {
      QString fileName = *it;
      edit->loadFile(fileName);
      ++it;
    }
    edit->showNormal();
    edit->raise();
  }
}


void MainApp::SetupInteractiveTerminalCase() {
#ifdef Q_WS_X11
  GUIHack = true;
  Terminal *myterm = new Terminal;
  m_win = NULL;
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

ApplicationWindow* MainApp::getApplicationWindow() {
  return m_win;
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

ArrayVector PathToolFunction(int, const ArrayVector&, Interpreter*);

void MainApp::PathTool() {
  ArrayVector dummy;
  PathToolFunction(0,dummy,m_eval);
}

//!
//@Module EDITOR Open Editor Window
//@@Section FREEMAT
//@@Usage
//Brings up the editor window.  The @|editor| function takes no
//arguments:
//@[
//  editor
//@]
//@@Signature
//sgfunction editor EditorFunction
//inputs none
//outputs none
//!
ArrayVector EditorFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  createEditor(eval);
  edit->loadLastSession();
  edit->addTabIfEmpty();
  edit->showNormal();
  edit->raise();
  return ArrayVector();
}

//!
//@Module EDIT Open Editor Window
//@@Section FREEMAT
//@@Usage
//Brings up the editor window.  The arguments of @|edit| function  
//are names of files for editing:
//@[
//  edit file1 file2 file3
//@]
//@@Signature
//sgfunction edit EditFunction
//inputs varargin
//outputs none
//!
ArrayVector EditFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  //Open the editor
  createEditor(eval);
  edit->loadLastSession();
  if (arg.size() == 0)
    //Create untitled.m if without argument
    edit->addTabUntitled();
  else {
    //Load files listed in the command argument
    for (int i=0; i<arg.size(); ++i ) {
      if (arg[i].isString()) {
        QString fname = arg[i].asString();
        edit->loadOrCreateFile(fname);
      }
      else {
      throw Exception("Illegal file name");
      }
    }
  }
  edit->showNormal();
  edit->raise();
  return ArrayVector();
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
  //  else
    dbout << "Interpreter thread crashed unexpectedly!  This is likely a FreeMat bug of some kind.  Please file a bug report at http://freemat.sf.net.";
  qApp->quit();
}

void MainApp::Quit() {
  TerminalReset();
  m_keys->WriteHistory();
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

void MainApp::ExecuteLine(QString txt) {
  m_eval->ExecuteLine(txt);
}

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
    wid->setWindowTitle("FreeMat v" VERSION);
    wid->show();
  }
}

void MainApp::DoGraphicsCall(Interpreter* interp, FuncPtr f, ArrayVector m, int narg) { 
  CheckNonClosable();
  try {
    ArrayVector n(f->evaluateFunc(interp,m,narg));
    interp->RegisterGfxResults(n);
  } catch (Exception& e) {
    interp->RegisterGfxError(e.msg());
  }
}

//!
//@Module THREADID Get Current Thread Handle
//@@Section THREAD
//@@Usage
//The @|threadid| function in FreeMat tells you which thread
//is executing the context you are in.  Normally, this is thread
//1, the main thread.  However, if you start a new thread using
//@|threadnew|, you will be operating in a new thread, and functions
//that call @|threadid| from the new thread will return their 
//handles.
//@@Example
//From the main thread, we have
//@<
//threadid
//@>
//But from a launched auxilliary thread, we have
//@<
//t_id = threadnew
//id = threadcall(t_id,1000,'threadid')
//threadfree(t_id);
//@>
//@@Signature
//sfunction threadid ThreadIDFunction
//inputs none
//outputs id
//!
ArrayVector ThreadIDFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(double(eval->getThreadID())));
}

//!
//@Module PAUSE Pause Script Execution
//@@Section IO
//@@Usage
//The @|pause| function can be used to pause execution of FreeMat
//scripts.  There are several syntaxes for its use.  The first form
//is
//@[
//   pause
//@]
//This form of the @|pause| function pauses FreeMat until you press
//any key.  The second form of the @|pause| function takes an argument
//@[
//   pause(p)
//@]
//where @|p| is the number of seconds to pause FreeMat for.  The pause
//argument should be accurate to a millisecond on all supported platforms.
//Alternately, you can control all @|pause| statements using:
//@[
//   pause on
//@]
//which enables pauses and
//@[
//   pause off
//@]
//which disables all @|pause| statements, both with and without arguments.
//@@Signature
//sgfunction pause PauseFunction
//inputs flag
//outputs none
//!
static bool pause_active = true;

ArrayVector PauseFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 1) {
    // Check for the first argument being a string
    if (arg[0].isString()) {
      QString parg(arg[0].asString().toUpper());
      if (parg == "ON")
	pause_active = true;
      else if (parg == "OFF")
	pause_active = false;
      else
	throw Exception("Unrecognized argument to pause function - must be either 'on' or 'off'");
    }
    if (pause_active)
      eval->sleepMilliseconds((unsigned long)(arg[0].asDouble()*1000));
  } else {
    // Do something...
    if (pause_active)
      m_app->GetKeyManager()->getKeyPress();
  }
  return ArrayVector();
}

//!
//@Module SLEEP Sleep For Specified Number of Seconds
//@@Section FREEMAT
//@@Usage
//Suspends execution of FreeMat for the specified number
//of seconds.  The general syntax for its use is
//@[
//  sleep(n),
//@]
//where @|n| is the number of seconds to wait.
//@@Signature
//sgfunction sleep SleepFunction
//inputs n
//outputs none
//!
ArrayVector SleepFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("sleep function requires 1 argument");
  int sleeptime;
  sleeptime = arg[0].asInteger();
  eval->sleepMilliseconds(1000*sleeptime);
  return ArrayVector();
}


//!
//@Module THREADNEW Create a New Thread
//@@Section THREAD
//@@Usage
//The @|threadnew| function creates a new FreeMat thread, and
//returns a handle to the resulting thread.   The @|threadnew|
//function takes no arguments.  They general syntax for the
//@|threadnew| function is
//@[
//   handle = threadnew
//@]
//Once you have a handle to a thread, you can start the thread
//on a computation using the @|threadstart| function.  The
//threads returned by @|threadnew| are in a dormant state (i.e.,
//not running).  Once you are finished with the thread you
//must call @|threadfree| to free the resources associated with
//that thread.
//
//Some additional important information.  Thread functions operate
//in their own context or workspace, which means that data cannot
//be shared between threads.  The exception is @|global| variables,
//which provide a thread-safe way for multiple threads to share data.
//Accesses to global variables are serialized so that they can 
//be used to share data.  Threads and FreeMat are a new feature, so
//there is room for improvement in the API and behavior.  The best
//way to improve threads is to experiment with them, and send feedback.
//@@Signature
//sfunction threadnew ThreadNewFunction
//inputs none
//outputs handle
//!
ArrayVector ThreadNewFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  // Create a new thread
  int threadID = m_app->StartNewInterpreterThread();
  // Translate the path from the starter thread to the new thread
  Interpreter* thread = m_threadHandles.lookupHandle(threadID);
  // Create a context for it
  thread->getContext()->pushScope("scratch");
  thread->setPath(eval->getPath());
  return ArrayVector(Array(double(threadID)));
}

//!
//@Module THREADSTART Start a New Thread Computation
//@@Section THREAD
//@@Usage
//The @|threadstart| function starts a new computation on a
//FreeMat thread, and you must provide a function (no scripts 
//are allowed) to run inside the thread, pass any parameters that
//the thread function requires, as well as the number of output
//arguments expected.  The general syntax for the 
//@|threadstart| function is
//@[
//   threadstart(threadid,function,nargout,arg1,arg2,...)
//@]
//where @|threadid| is a thread handle (returned by @|threadnew|),
//where @|function| is a valid function name (it can be a built-in
//imported or M-function), @|nargout| is the number of output arguments
//expected from the function, and @|arg1| is the first argument that
//is passed to the function.  Because the function runs in its 
//own thread, the return values of the function are not available
//imediately.  Instead, execution of that function will continue
//in parallel with the current thread.  To retrieve the output
//of the thread function, you must wait for the thread to complete
//using the @|threadwait| function, and then call @|threadvalue|
// to retrieve the result.  You can also stop the running thread
//prematurely by using the @|threadkill| function.  It is important
//to call @|threadfree| on the handle you get from @|threadnew|
//when you are finished with the thread to ensure that the resoures
//are properly freed.  
//
//It is also perfectly reasonable to use a single thread multiple
//times, calling @|threadstart| and @|threadreturn| multiple times
//on a single thread.  The context is preserved between threads.
//When calling @|threadstart| on a pre-existing thread, FreeMat
//will attempt to wait on the thread.  If the wait fails, then
//an error will occur.
//
//Some additional important information.  Thread functions operate
//in their own context or workspace, which means that data cannot
//be shared between threads.  The exception is @|global| variables,
//which provide a thread-safe way for multiple threads to share data.
//Accesses to global variables are serialized so that they can 
//be used to share data.  Threads and FreeMat are a new feature, so
//there is room for improvement in the API and behavior.  The best
//way to improve threads is to experiment with them, and send feedback.
//
//@@Example
//Here we do something very simple.  We want to obtain a listing of
//all files on the system, but do not want the results to stop our
//computation.  So we run the @|system| call in a thread.
//@<
//a = threadnew;                         % Create the thread
//threadstart(a,'system',1,'ls -lrt /'); % Start the thread
//b = rand(100)\rand(100,1);             % Solve some equations simultaneously
//c = threadvalue(a);                    % Retrieve the file list
//size(c)                                % It is large!
//threadfree(a);
//@>
//The possibilities for threads are significant.  For example,
//we can solve equations in parallel, or take Fast Fourier Transforms
//on multiple threads.  On multi-processor machines or multicore CPUs,
//these threaded calculations will execute in parallel.  Neat.
//
//The reason for the  @|nargout| argument is best illustrated with
//an example.  Suppose we want to compute the Singular Value 
//Decomposition @|svd| of a matrix @|A| in a thread.  
//The documentation for the @|svd| function tells us that
//the behavior depends on the number of output arguments we request.
//For example, if we want a full decomposition, including the left 
//and right singular vectors, and a diagonal singular matrix, we
//need to use the three-output syntax, instead of the single output
//syntax (which returns only the singular values in a column vector):
//@<
//A = float(rand(4))
//[u,s,v] = svd(A)    % Compute the full decomposition
//sigmas = svd(A)     % Only want the singular values
//@>
//
//Normally, FreeMat uses the left hand side of an assignment to calculate
//the number of outputs for the function.  When running a function in a
//thread, we separate the assignment of the output from the invokation
//of the function.  Hence, we have to provide the number of arguments at the
//time we invoke the function.  For example, to compute a full decomposition
//in a thread, we specify that we want 3 output arguments:
//@<
//a = threadnew;               % Create the thread
//threadstart(a,'svd',3,A);    % Start a full decomposition 
//[u1,s1,v1] = threadvalue(a); % Retrieve the function values
//threadfree(a);
//@>
//If we want to compute just the singular values, we start the thread
//function with only one output argument:
//@<
//a = threadnew;
//threadstart(a,'svd',1,A);
//sigmas = threadvalue(a);
//threadfree(a)
//@>
//@@Signature
//sfunction threadstart ThreadStartFunction
//inputs threadid function nargout varargin
//outputs none
//!
ArrayVector ThreadStartFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 3) throw Exception("threadstart requires at least three arguments (the thread id, the function to spawn, and the number of output arguments)");
  int32 handle = arg[0].asInteger();
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  QString fnc = arg[1].asString();
  // Lookup this function in base interpreter to see if it is defined
  FuncPtr val;
  if (!eval->lookupFunction(fnc, val))
    throw Exception(QString("Unable to map ") + fnc + " to a defined function ");
  val->updateCode(eval);
  // if (val->scriptFlag)
  //   throw Exception(string("Cannot use a script as the main function in a thread."));
  int tnargout = arg[2].asInteger();
  if (!thread->wait(1))  throw Exception("thread was busy");
  ArrayVector args(arg);
  args.pop_front();
  args.pop_front();
  args.pop_front();
  thread->setThreadFunc(val,tnargout,args);
  thread->start();
  return ArrayVector();
}

//!
//@Module THREADVALUE Retrieve the return values from a thread
//@@Section THREAD
//@@Usage
//The @|threadvalue| function retrieves the values returned
//by the function specified in the @|threadnew| call.  The
//syntax for its use is
//@[
//   [arg1,arg2,...,argN] = threadvalue(handle)
//@]
//where @|handle| is the value returned by a @|threadnew| call.
//Note that there are issues with @|nargout|.  See the examples
//section of @|threadnew| for details on how to work around this
//limitation.  Because the function you have spawned with @|threadnew|
//may still be executing, @|threadvalue| must first @|threadwait|
//for the function to complete before retrieving the output values.
//This wait may take an arbitrarily long time if the thread function
//is caught in an infinite loop.  Hence, you can also specify
//a timeout parameter to @|threadvalue| as
//@[
//   [arg1,arg2,...,argN] = threadvalue(handle,timeout)
//@]
//where the @|timeout| is specified in milliseconds.  If the
//wait times out, an error is raised (that can be caught with a
//@|try| and @|catch| block.  
//
//In either case, if the thread function itself caused an error
//and ceased execution abruptly, then calling @|threadvalue| will
//cause that function to raise an error, allowing you to retrieve
//the error that was caused and correct it.  See the examples section
//for more information.
//@@Example
//Here we do something very simple.  We want to obtain a listing of
//all files on the system, but do not want the results to stop our
//computation.  So we run the @|system| call in a thread.
//@<
//a = threadnew;                         % Create the thread
//threadstart(a,'system',1,'ls -lrt /'); % Start the thread
//b = rand(100)\rand(100,1);             % Solve some equations simultaneously
//c = threadvalue(a);                    % Retrieve the file list
//size(c)                                % It is large!
//threadfree(a);
//@>
//In this example, we force the threaded function to cause an
//exception (by calling the @|error| function as the thread 
//function).  When we call @|threadvalue|, we get an error, instead
//of the return value of the function
//@<1
//a = threadnew
//threadstart(a,'error',0,'Hello world!'); % Will immediately stop due to error
//c = threadvalue(a)                     % The error comes to us
//threadfree(a)
//@>
//Note that the error has the text @|Thread:| prepended to the message
//to help you identify that this was an error in a different thread.
//@@Signature
//function threadvalue ThreadValueFunction
//inputs handle timeout
//outputs varargout
//!
ArrayVector ThreadValueFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadvalue requires at least one argument (thread id to retrieve value from)");
  int32 handle = arg[0].asInteger();
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  unsigned long timeout = ULONG_MAX;
  if (arg.size() > 1)
    timeout = (unsigned long) arg[1].asInteger();
  if (!thread->wait()) throw Exception("error waiting for thread to complete");
  if (thread->getLastErrorState())
    throw Exception("Thread: " + thread->getLastErrorString());
  return thread->getThreadFuncReturn();
}

//!
//@Module THREADWAIT Wait on a thread to complete execution
//@@Section THREAD
//@@Usage
//The @|threadwait| function waits for the given thread to complete
//execution, and stops execution of the current thread (the one calling
//@|threadwait|) until the given thread completes.  The syntax for its
//use is 
//@[
//   success = threadwait(handle)
//@]
//where @|handle| is the value returned by @|threadnew| and @|success|
//is a @|logical| vaariable that will be @|1| if the wait was successful
//or @|0| if the wait times out.  By default, the wait is indefinite.  It
//is better to use the following form of the function
//@[
//   success = threadwait(handle,timeout)
//@]
//where @|timeout| is the amount of time (in milliseconds) for 
//the @|threadwait| function to wait before a timeout occurs.  
//If the @|threadwait| function succeeds, then the return 
//value is a logical @|1|, and if it fails, the return value 
//is a logical @|0|.  Note that you can call @|threadwait| multiple
//times on a thread, and if the thread is completed, each one
//will succeed.
//@@Example
//Here we lauch the @|sleep| function in a thread with a time delay of 
//10 seconds.  This means that the thread function will not complete
//until 10 seconds have elapsed.  When we call @|threadwait| on this
//thread with a short timeout, it fails, but not when the timeout
//is long enough to capture the end of the function call.
//@<
//a = threadnew;
//threadstart(a,'sleep',0,10);  % start a thread that will sleep for 10
//threadwait(a,2000)            % 2 second wait is not long enough
//threadwait(a,10000)           % 10 second wait is long enough
//threadfree(a)
//@>
//@@Signature
//function threadwait ThreadWaitFunction
//inputs handle timeout
//outputs success
//!
ArrayVector ThreadWaitFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadwait requires at least one argument (thread id to wait on)");
  int32 handle = arg[0].asInteger();
  unsigned long timeout = ULONG_MAX;
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  if (arg.size() > 1)
    timeout = arg[1].asInteger();
  bool retval = thread->wait(timeout);
  if (retval && thread->getLastErrorState())
    throw Exception("Thread: " + thread->getLastErrorString());
  return ArrayVector(Array(bool(retval)));
}

//!
//@Module THREADKILL Halt execution of a thread
//@@Section THREAD
//@@Usage
//The @|threadkill| function stops (or attempts to stop) execution
//of the given thread.  It works only for functions defined in M-files
//(i.e., not for built in or imported functions), and it works by 
//setting a flag that causes the thread to stop execution at the next
//available statement.  The syntax for this function is 
//@[
//  threadkill(handle)
//@]
//where @|handle| is the value returned by a @|threadnew| call.  
//Note that the @|threadkill| function returns immediately.  It 
//is still your responsibility to call @|threadfree| to free
//the thread you have halted.
//
//You cannot kill the main thread (thread id @|1|).
//@@Example
//Here is an example of stopping a runaway thread using @|threadkill|.
//Note that the thread function in this case is an M-file function.
//We start by setting up a free running counter, where we can access 
//the counter from the global variables.  
//@{ freecount.m
//function freecount
//  global count
//  if (~exist('count')) count = 0; end  % Initialize the counter
//  while (1)
//    count = count + 1;                 % Update the counter
//  end
//@}
//We now launch this function in a thread, and use @|threadkill| to
//stop it:
//@<
//a = threadnew;
//global count                   % register the global variable count
//count = 0;
//threadstart(a,'freecount',0)   % start the thread
//count                          % it is counting
//sleep(1)                       % Wait a bit
//count                          % it is still counting
//threadkill(a)                  % kill the counter
//threadwait(a,1000)             % wait for it to finish
//count                          % The count will no longer increase
//sleep(1)
//count
//threadfree(a)
//@>
//@@Signature
//function threadkill ThreadKillFunction
//inputs handle
//outputs none
//!
ArrayVector ThreadKillFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadkill requires at least one argument (thread id to kill)");
  int32 handle = arg[0].asInteger();
  if (handle == 1) throw Exception("threadkill cannot be used on the main thread");
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  thread->setKill();
  return ArrayVector();
}


//!
//@Module THREADFREE Free thread resources
//@@Section THREAD
//@@Usage
//The @|threadfree| is a function to free the resources claimed
//by a thread that has finished.  The syntax for its use is
//@[
//   threadfree(handle)
//@]
//where @|handle| is the handle returned by the call to @|threadnew|.
//The @|threadfree| function requires that the thread be completed.  
//Otherwise it will wait for the thread to complete, potentially 
//for an arbitrarily long period of time.  To fix this, you can
//either call @|threadfree| only on threads that are known to have
//completed, or you can call it using the syntax
//@[
//   threadfree(handle,timeout)
//@]
//where @|timeout| is a time to wait in milliseconds.  If the thread
//fails to complete before the timeout expires, an error occurs.
//@@Signature
//function threadfree ThreadFreeFunction
//inputs handle timeout
//outputs none
//!
ArrayVector ThreadFreeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadfree requires at least one argument (thread id to wait on) - the optional second argument is the timeout to wait for the thread to finish");
  int32 handle = arg[0].asInteger();
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  thread->setKill();
  unsigned long timeout = ULONG_MAX;
  if (arg.size() > 1)
    timeout = (unsigned long) arg[1].asInteger();
  if (!thread->wait(timeout))
    throw Exception("Cannot free thread... it is still running");
  delete thread;
  m_threadHandles.deleteHandle(handle);
  return ArrayVector();
}

//!
//@Module CLC Clear Dislplay
//@@Section FREEMAT
//@@Usage
//The @|clc| function clears the current display.  The syntax for
//its use is
//@[
//   clc
//@]
//@@Signature
//gfunction clc ClcFunction
//inputs none
//outputs none
//!
ArrayVector ClcFunction(int nargout, const ArrayVector& arg) {
  m_app->GetKeyManager()->ClearDisplayCommand();
  return ArrayVector();
}

// The database of samples.  There is one profile per function.
//std::map<std::string, unsigned[1000] >  profileDB;
typedef std::vector<unsigned> ProfileVector;
typedef std::map<QString, ProfileVector> ProfileDB;
static ProfileDB m_profileDB;
static bool m_profiler_active = false;
static double m_profiler_ticks = 0;

static void DumpProfileDB() {
  //  double profiler_ticks = m_profiler_ticks;
  dbout << "Total ticks " << m_profiler_ticks << "\r\n";
  for (ProfileDB::const_iterator i=m_profileDB.begin();i!=m_profileDB.end();i++) {
    dbout << "Module " << i->first << "\r\n";
    const ProfileVector &p(i->second);
    double ticks_this_module = 0;
    for (unsigned j=0;j<p.size();j++) {
      //      if (p[j] > 0)
	dbout << "Line " << j << " " << p[j] << " counts " << p[j]/m_profiler_ticks*100.0 << "%\r\n";
      ticks_this_module += p[j];
    }
    //dbout << "Total time in " << i->first << " " << ticks_this_module << " counts "  
    //	      << ticks_this_module/profiler_ticks*100.0 << "%\r\n";
  }
}

class SampleList{
public:
    QString module;
    int line;
    int samples;
    SampleList( const QString& module_, int line_, int samples_ ) : module( module_ ), line( line_ ), samples( samples_ ){};
    static bool CompSample( SampleList p1, SampleList p2 ){ return p1.samples > p2.samples; }
};

//!
//@Module PROFILER Control Profiling
//@@Section FREEMAT
//@@Usage
//The @|profile| function allows you to control the FreeMat profiler.
//It has two modes of operation.  The first is to enable-disable
//the profiler.  To turn on profiling:
//@[
//  profiler on
//@]
//to turn off profiling, use
//@[
//  profiler off
//@]
//Note that regardless of the state of the profiler, only functions
//and scripts are profiled.  Commands entered on the command line
//are not profiled.  To see information that has accumulated in a
//profile, you use the variant of the command:
//@[
//  profiler list
//@]
//which lists current sorted profiling resuls. You can use this form to obtain 
//profiler results as a cell array
//@[
//  r=profiler('list')
//@]
//If you want to see current profile
//status issue a @|profile| command with no arguments.
//@[
//   profiler
//@]
//@@Signature
//sfunction profiler ProfilerFunction
//inputs varargin
//outputs varargout
//!
ArrayVector ProfilerFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
	ArrayVector retall;
	ArrayVector ret;

  if (arg.size() < 1) {
    if (m_profiler_active)
      return ArrayVector(Array(QString("on")));
    else
      return ArrayVector(Array(QString("off")));
  } else {
    if (!arg[0].isString())
      throw Exception("second argument to profile function must be either on/off/list");
    QString txt = arg[0].asString().toUpper();
    if (txt == "ON")
      m_app->EnableProfiler(true);
    else if (txt == "OFF")
      m_app->EnableProfiler(false);
    else if (txt == "DUMP")
      DumpProfileDB();
     else if (txt == "LIST") {
	 std::vector<SampleList> sl;

	  for (ProfileDB::const_iterator i=m_profileDB.begin();i!=m_profileDB.end();i++) {
	    const ProfileVector &p(i->second);
	    for (unsigned j=0;j<p.size();j++) {
	      if (p[j] > 0)
		    sl.push_back( SampleList( i->first, j, p[j] ) );
	    }
	  }
	  sort( sl.begin(), sl.end(), SampleList::CompSample );

	  for( std::vector<SampleList>::const_iterator i=sl.begin(); i!=sl.end(); ++i ){

		  QString out;
	      if( i->line == 0 ){
			out=(QString("%1% %2 (built-in)\n").arg((double)i->samples/m_profiler_ticks*100.,5,'f',3).arg( i->module ));
			if( nargout == 1 ){
				ret << StructConstructor( StringVector() << "sample_fraction" << "module" << "line", ArrayVector() << Array( (double)i->samples/m_profiler_ticks ) << CellArrayFromStringVector( QStringList(i->module) ) << Array(0.));
			}
		  }
		  else{
			out=(QString("%1% %2:%3\n").arg((double)i->samples/m_profiler_ticks*100.,5,'f',3).arg( i->module ).arg(i->line));
			if( nargout == 1 ){
				ret << StructConstructor( StringVector() << "sample_fraction" << "module" << "line", ArrayVector() << Array( (double)i->samples/m_profiler_ticks ) << CellArrayFromStringVector( QStringList(i->module) ) << Array( i->line ));
			}

		  }

		  eval->outputMessage( out );
	  }
		retall << CellConstructor(ret);
     }
  }
  return retall;
}


void MainApp::EnableRepaint() {
  GfxEnableRepaint();
}

void MainApp::DisableRepaint() {
  GfxDisableRepaint();
}

void  LoadBuiltinFunctionsFreeMat(Context* context,bool guimode);
void  LoadBuiltinFunctionsCore(Context* context,bool guimode);
void  LoadBuiltinFunctionsGraphics(Context* context,bool guimode);
void  LoadBuiltinFunctionsFN(Context* context,bool guimode);
void  LoadBuiltinFunctionsMain(Context* context,bool guimode);

#ifdef HAVE_VTK
void LoadBuiltinFunctionsVTKRendering(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKInfovis(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKWidgets(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKCommon(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKImaging(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKHybrid(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKParallel(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKVolumeRendering(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKIO(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKViews(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKFiltering(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKGeovis(Context *context, bool guiflag);
void LoadBuiltinFunctionsVTKGraphics(Context *context, bool guiflag);
void LoadBuiltinFunctionsITK(Context *context, bool guiflag);
#endif

			 
void LoadBuiltinFunctions(Context *context, bool guimode)
{
  LoadBuiltinFunctionsFreeMat(context,guimode);
  LoadBuiltinFunctionsCore(context,guimode);
  LoadBuiltinFunctionsGraphics(context,guimode);
  LoadBuiltinFunctionsFN(context,guimode);
  LoadBuiltinFunctionsMain(context,guimode);
#ifdef HAVE_VTK
  LoadBuiltinFunctionsVTKRendering(context,guimode);
  LoadBuiltinFunctionsVTKInfovis(context,guimode);
  LoadBuiltinFunctionsVTKWidgets(context,guimode);
  LoadBuiltinFunctionsVTKCommon(context,guimode);
  LoadBuiltinFunctionsVTKImaging(context,guimode);
  LoadBuiltinFunctionsVTKHybrid(context,guimode);
  LoadBuiltinFunctionsVTKParallel(context,guimode);
  LoadBuiltinFunctionsVTKVolumeRendering(context,guimode);
  LoadBuiltinFunctionsVTKIO(context,guimode);
  LoadBuiltinFunctionsVTKViews(context,guimode);
  LoadBuiltinFunctionsVTKFiltering(context,guimode);
  LoadBuiltinFunctionsVTKGeovis(context,guimode);
  LoadBuiltinFunctionsVTKGraphics(context,guimode);
  LoadBuiltinFunctionsITK(context,guimode);
#endif
}

void InitializeFileSubsystem();

static bool first_time = true;

Context *MainApp::NewContext() {
  Context *context = new Context(m_global);
  LoadBuiltinFunctions(context,guimode);
  if (first_time) {
    first_time = false;
    InitializeHandleGraphics();
    InitializeFileSubsystem();
  }
#ifdef DYN_BLAS
  void LoadBlasFunctions( void );
  LoadBlasFunctions();
#endif
  return context;
}

QString GetRootPath() {
  if (inBundleMode()) {
    QDir dir(qApp->applicationDirPath() + "/../Resources");
    if (dir.exists())
      return dir.canonicalPath();
    else
      return "";
  }
  QSettings settings("FreeMat", Interpreter::getVersionString());
  QDir dir(settings.value("root", RESOURCEDIR).toString());
  if (dir.exists())
    return dir.canonicalPath();
  else
#ifdef Q_WS_WIN
    return qApp->applicationDirPath()+QString("/../");
#else 
#ifdef Q_WS_MAC
    return "";
#else
    return QDir(qApp->applicationDirPath()+QString("/../share/FreeMat-" VERSION)).canonicalPath();
#endif
#endif
}

void MainApp::UpdateBasePath(QString rootpath) {
  QDir dir1(rootpath + "/toolbox");
  if (dir1.exists()) {
    QString path1(dir1.canonicalPath());
    basePath += GetRecursiveDirList(path1);
  } else {
    WarningMessage("Cannot find toolbox directory at " + rootpath + "/toobox.  Please adjust your rootpath.");
  }
  QDir dir2(rootpath + "/help/text");
  if (dir2.exists()) {
    QString path2(dir2.canonicalPath());
    basePath += GetRecursiveDirList(path2);
  } else {
    WarningMessage("Cannot find help directory at " + rootpath + "/help/text.  Please adjust your rootpath.");
  }
}

void MainApp::SetRootPath(QString path, Interpreter* eval) {
  if (inBundleMode()) 
    eval->warningMessage("FreeMat is in bundle mode.  The rootpath function has no effect.");
  QDir dir(path);
  if (!dir.exists())
    eval->warningMessage("Root path <" + path + "> does not exist!");
  QSettings settings("FreeMat", Interpreter::getVersionString());
  settings.setValue("root",path);
  UpdateBasePath(path);
  eval->setBasePath(basePath);
  eval->rescanPath();
}

void MainApp::UpdatePaths() {
  static bool paths_set = false;
  if (!paths_set) {
    QString rootpath = GetRootPath();
    UpdateBasePath(rootpath);
    QSettings settings("FreeMat", Interpreter::getVersionString());
    userPath = settings.value("interpreter/path").toStringList();
    paths_set = true;
  }
}

//!
//@Module ROOTPATH Set FreeMat Root Path
//@@Section FREEMAT
//@@Usage
//In order to function properly, FreeMat needs to know where to
//find the @|toolbox| directory as well as the @|help| directory.
//These directories are located on what is known as the @|root path|.
//Normally, FreeMat should know where these directories are located.
//However under some circumstances (usually when FreeMat is installed
//into a non-default location), it may be necessary to indicate
//a different root path location, or to specify a particular one.
//Note that on the Mac OS platform, FreeMat is installed as a bundle,
//and will use the toolbox that is installed in the bundle regardless of
//the setting for @|rootpath|.
//For Linux, FreeMat will typically use @|/usr/local/share/FreeMat-<Version>/|
//for the root path.  Installations from source code will generally work,
//but binary installations (e.g., from an @|RPM|) may need to have the
//rootpath set.
//
//The @|rootpath| function has two forms.  The first form takes no arguments
//and returns the current root path
//@[
//   rootpath
//@]
//The second form will set a rootpath directly from the command line
//@[
//   rootpath(path)
//@]
//where @|path| is the full path to where the @|toolbox| and @|help| 
//directories are located.  For example, @|rootpath('/usr/share/FreeMat-4.0')|.
//The third form enables the GUI form 
//@[
//   rootpath gui
//@]
//which activates a dialog box to pick a directory that is the root directory
//of the FreeMat installation (e.g., where @|help| and @|toolbox| are located.
//Changes to @|rootpath| are persistent (you do not need to run it every
//time you start FreeMat).
//@@Signature
//sgfunction rootpath RootPathFunction
//inputs path
//outptus none
//!
ArrayVector RootPathFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (inBundleMode()) 
    eval->warningMessage("FreeMat is in bundle mode.  The rootpath function has no effect.");
  QString path;
  if (arg.size() != 0) {
    path = arg[0].asString();
    if (path == "gui" || path == "GUI")
      path = QFileDialog::getExistingDirectory(0,QString("Select FreeMat root path"),GetRootPath()); 
    m_app->SetRootPath(path,eval);
  }
  return ArrayVector(Array(GetRootPath()));
}

static int m_mainID;

void WarningMessage(QString txt) {
  Interpreter *m_eval = NULL;
  try {
    m_eval = m_threadHandles.lookupHandle(m_mainID);
  } catch (Exception &e) { }
  if (m_eval)
    m_eval->warningMessage(txt);
}

void ImportPrintMessage(const char* t) {
  Interpreter *m_eval = m_threadHandles.lookupHandle(m_mainID);
  if (m_eval) m_eval->outputMessage(QString(t));
}

int MainApp::StartNewInterpreterThread() {
  Interpreter *p_eval = new Interpreter(NewContext());
  p_eval->setBasePath(basePath);
  p_eval->setUserPath(userPath);
  connect(p_eval,SIGNAL(outputRawText(QString)),m_term,SLOT(OutputRawString(QString)));
  connect(p_eval,SIGNAL(SetPrompt(QString)),m_keys,SLOT(SetPrompt(QString)));
  connect(p_eval,SIGNAL(doGraphicsCall(Interpreter*,FuncPtr,ArrayVector,int)),
	  this,SLOT(DoGraphicsCall(Interpreter*,FuncPtr,ArrayVector,int)));
  connect(p_eval,SIGNAL(CWDChanged(QString)),m_keys,SIGNAL(UpdateCWD(QString)));
  connect(p_eval,SIGNAL(updateDirView(QVariant)),m_keys,SIGNAL(updateDirView(QVariant)));
  connect(p_eval,SIGNAL(updateVarView(QVariant)),m_keys,SIGNAL(updateVarView(QVariant)));
  connect(p_eval,SIGNAL(updateStackView(QStringList)),
	  m_keys,SIGNAL(updateStackView(QStringList)));
  connect(p_eval,SIGNAL(QuitSignal()),this,SLOT(Quit()));
  connect(p_eval,SIGNAL(CrashedSignal()),this,SLOT(Crashed()));
  connect(p_eval,SIGNAL(EnableRepaint()),this,SLOT(EnableRepaint()));
  connect(p_eval,SIGNAL(DisableRepaint()),this,SLOT(DisableRepaint()));
  p_eval->setTerminalWidth(m_keys->getTerminalWidth());
  p_eval->setGreetingFlag(skipGreeting);
  int threadID = m_threadHandles.assignHandle(p_eval);
  p_eval->setThreadID(threadID);
#ifdef __OpenBSD__
  /* 64 frames / calls deep */
  p_eval->setStackSize(262144);
#endif
  return threadID;
}

//
// This method is called to collect information on the IP counter for a 
// thread (currently only the main thread can be sampled).  This should
// suspend the thread it samples, or at worst, we should protect the
// instruction pointer with a mutex when profiling is on.
//
void MainApp::CollectProfileSample() {
  unsigned linenumber;
  QString ip_name = m_eval->sampleInstructionPointer(linenumber);
  if ((linenumber != 0) || (ip_name != "CLI")) {
	m_profiler_ticks++;
    ProfileVector &p(m_profileDB[ip_name]);
    if (p.size() <= linenumber) 
	p.resize(linenumber+1);
    p[linenumber]++;
  }
  // Register this as a data sample
  //  profileDB[ip_name][linenumber]++;
}

void MainApp::EnableProfiler( bool bEnable )
{
    emit SetEnable( bEnable );
}

void MainApp::ControlProfiler(bool enableflag) {
  if (enableflag)
    profilerTimer->start();
  else
    profilerTimer->stop();
  m_profiler_active = enableflag;
}

void MainApp::RegisterInterrupt() {
  // Get the main interpreter thread
  m_eval = m_threadHandles.lookupHandle(m_mainID);
  if (m_eval)
    m_eval->setInterrupt();
}

void MainApp::RefreshFigures() {
  RefreshFigs();
}

int MainApp::Run() {
  UpdatePaths();
  qRegisterMetaType<FuncPtr>("FuncPtr");
  qRegisterMetaType<ArrayVector>("ArrayVector");
  qRegisterMetaType<Interpreter*>("Interpreter*");
  qRegisterMetaType<StringVector>("StringVector");
  qRegisterMetaType<QVariant>("QVariant");
  connect(m_keys,SIGNAL(ExecuteLine(QString)),this,SLOT(ExecuteLine(QString)));
  connect(m_keys,SIGNAL(UpdateTermWidth(int)),this,SLOT(UpdateTermWidth(int)));
  connect(m_keys,SIGNAL(RegisterInterrupt()),this,SLOT(RegisterInterrupt()));
  // Set up the profile timer (but don't start it)
  profilerTimer = new QTimer(this);
  profilerTimer->setSingleShot(false);
  // Start out with a sampling frequency of 1/100 sec
  profilerTimer->setInterval(10); 
  connect(profilerTimer,SIGNAL(timeout()),this,SLOT(CollectProfileSample()));
  connect( this, SIGNAL( SetEnable( bool ) ), this, SLOT( ControlProfiler( bool ) ) );

  refreshTimer = new QTimer(this);
  refreshTimer->setSingleShot(false);
  refreshTimer->setInterval(50);
  connect(refreshTimer,SIGNAL(timeout()),this,SLOT(RefreshFigures()));
  // Get a new thread
  GfxEnableRepaint();
  m_mainID = StartNewInterpreterThread();
  // Assign this to the main thread
  m_eval = m_threadHandles.lookupHandle(m_mainID);
  m_eval->setLiveUpdateFlag(true);
  m_eval->setJITControl(true);
  m_keys->SetCompletionContext(m_eval->getContext());
  FuncPtr doCLI;
  if (!m_eval->lookupFunction("docli",doCLI))
    return 0;
  m_eval->setThreadFunc(doCLI,0,ArrayVector() );
  m_eval->start();
  refreshTimer->start();
  emit Initialize();
  return 0;
}


