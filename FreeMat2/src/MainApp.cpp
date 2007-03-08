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
#include "HandleWindow.hpp"

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
  // The global scope is special
  m_global->mutexSetup();
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

void MainApp::DoGraphicsCall(Interpreter* interp, FuncPtr f, ArrayVector m, int narg) { 
  CheckNonClosable();
  try {
    ArrayVector n(f->evaluateFunction(interp,m,narg));
    interp->RegisterGfxResults(n);
  } catch (Exception& e) {
    interp->RegisterGfxError(e.getMessageCopy());
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
//!
ArrayVector ThreadIDFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector() << Array::uint32Constructor(eval->getThreadID());
}

extern MainApp *m_app;
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
//!
static bool pause_active = true;

ArrayVector PauseFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 1) {
    // Check for the first argument being a string
    if (arg[0].isString()) {
      string parg(arg[0].getContentsAsStringUpper());
      if (parg == "ON")
	pause_active = true;
      else if (parg == "OFF")
	pause_active = false;
      else
	throw Exception("Unrecognized argument to pause function - must be either 'on' or 'off'");
    }
    if (pause_active)
      eval->sleepMilliseconds((unsigned long)(ArrayToDouble(arg[0])*1000));
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
//!
ArrayVector SleepFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("sleep function requires 1 argument");
  int sleeptime;
  Array a(arg[0]);
  sleeptime = a.getContentsAsIntegerScalar();
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
//!
ArrayVector ThreadNewFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  // Create a new thread
  int threadID = m_app->StartNewInterpreterThread();
  return ArrayVector() << Array::uint32Constructor(threadID);
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
//
//!
ArrayVector ThreadStartFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 3) throw Exception("threadstart requires at least three arguments (the thread id, the function to spawn, and the number of output arguments)");
  int32 handle = ArrayToInt32(arg[0]);
  unsigned long timeout = ULONG_MAX;
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  string fnc = ArrayToString(arg[1]);
  // Lookup this function in base interpreter to see if it is defined
  FuncPtr val;
  if (!eval->lookupFunction(fnc, val))
    throw Exception(string("Unable to map ") + fnc + " to a defined function ");
  val->updateCode();
  // if (val->scriptFlag)
  //   throw Exception(string("Cannot use a script as the main function in a thread."));
  int tnargout = ArrayToInt32(arg[2]);
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
//!
ArrayVector ThreadValueFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadvalue requires at least one argument (thread id to retrieve value from)");
  int32 handle = ArrayToInt32(arg[0]);
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  unsigned long timeout = ULONG_MAX;
  if (arg.size() > 1)
    timeout = (unsigned long) ArrayToInt32(arg[1]);
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
//!
ArrayVector ThreadWaitFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadwait requires at least one argument (thread id to wait on)");
  int32 handle = ArrayToInt32(arg[0]);
  unsigned long timeout = ULONG_MAX;
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  if (!thread) throw Exception("invalid thread handle");
  if (arg.size() > 1)
    timeout = (unsigned long) ArrayToInt32(arg[1]);
  bool retval = thread->wait(timeout);
  if (retval && thread->getLastErrorState())
    throw Exception("Thread: " + thread->getLastErrorString());
  return ArrayVector() << Array::logicalConstructor(retval);
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
//We now lauch this function in a thread, and use @|threadkill| to
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
//!
ArrayVector ThreadKillFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadkill requires at least one argument (thread id to kill)");
  int32 handle = ArrayToInt32(arg[0]);
  if (handle == 1) throw Exception("threadkill cannot be used on the main thread");
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  thread->setInterrupt();
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
//
//!
ArrayVector ThreadFreeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1) throw Exception("threadfree requires at least one argument (thread id to wait on) - the optional second argument is the timeout to wait for the thread to finish");
  int32 handle = ArrayToInt32(arg[0]);
  Interpreter* thread = m_threadHandles.lookupHandle(handle);
  thread->setInterrupt();
  unsigned long timeout = ULONG_MAX;
  if (arg.size() > 1)
    timeout = (unsigned long) ArrayToInt32(arg[1]);
  if (!thread->wait(timeout))
    throw Exception("Cannot free thread... it is still running");
  delete thread;
  m_threadHandles.deleteHandle(handle);
  return ArrayVector();
}

void LoadThreadFunctions(Context *context) {
  context->addSpecialFunction("threadid",ThreadIDFunction,0,1,NULL);
  context->addSpecialFunction("threadnew",ThreadNewFunction,0,1,NULL);
  context->addSpecialFunction("threadstart",ThreadStartFunction,-1,0,NULL);
  context->addFunction("threadvalue",ThreadValueFunction,2,-1,"handle","timeout",NULL);
  context->addFunction("threadwait",ThreadWaitFunction,2,1,"handle","timeout",NULL);
  context->addFunction("threadkill",ThreadKillFunction,1,0,"handle",NULL);
  context->addFunction("threadfree",ThreadFreeFunction,2,0,"handle","timeout",NULL);
  context->addGfxSpecialFunction("pause",PauseFunction,1,0,"x",NULL);
  context->addGfxSpecialFunction("sleep",SleepFunction,1,0,"x",NULL);
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
  connect(p_eval,SIGNAL(doGraphicsCall(Interpreter*,FuncPtr,ArrayVector,int)),
	  this,SLOT(DoGraphicsCall(Interpreter*,FuncPtr,ArrayVector,int)));
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
  qRegisterMetaType<Interpreter*>("Interpreter*");
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
  m_eval->setThreadFunc(doCLI,0,ArrayVector());
  m_eval->start();
  emit Initialize();
  return 0;
}
