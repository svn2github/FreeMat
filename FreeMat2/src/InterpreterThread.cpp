#include <QtCore>
#include "InterpreterThread.hpp"
#include "Context.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "HandleCommands.hpp"
#include "Module.hpp"
#include "Core.hpp"
#include "Common.hpp"
#include <QtGui>

using namespace FreeMat;
using namespace std;

void InterpreterThread::run() {
  //  qDebug("interpreter thread on standby...\n");
  eval->sendGreeting();
  emit Ready();
  forever {
    std::string cmdline;
    mutex.lock();
    if (cmd_buffer.empty())
      condition.wait(&mutex);
    cmdline = *cmd_buffer.begin();
    cmd_buffer.erase(cmd_buffer.begin());
    mutex.unlock();
    eval->ExecuteLine(cmdline);
    emit Ready();
  }
}

void InterpreterThread::Setup() {
  Context *context = new Context;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  if (guimode) {
    LoadGUICoreFunctions(context);
    LoadHandleGraphicsFunctions(context);  
  }
  QStringList basePath;
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
    QDir dir1(QApplication::applicationDirPath()+"/"+
	      QString(BASEPATH)+"/toolbox");   
    if (dir1.exists()) {
      QString path1(dir1.canonicalPath());
      basePath += GetRecursiveDirList(path1);
    }
    QDir dir2(QApplication::applicationDirPath()+"/"+
	      QString(BASEPATH) + "/help/text");
    if (dir2.exists()) {
      QString path2(dir2.canonicalPath());
      basePath += GetRecursiveDirList(path2);
    }
  }
  QSettings settings("FreeMat","FreeMat");
  QStringList userPath = settings.value("interpreter/path").toStringList();
  eval = new Interpreter(context);
  eval->setBasePath(basePath);
  eval->setUserPath(userPath);
  eval->setAppPath(qApp->applicationDirPath().toStdString());
  eval->rescanPath();
}

void InterpreterThread::ExecuteLine(std::string cmd) {
  mutex.lock();
  cmd_buffer.push_back(cmd);
  condition.wakeAll();
  mutex.unlock();
}
