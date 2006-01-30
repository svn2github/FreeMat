#include <qapplication.h>
#include <QDir>
#include <QDebug>
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

MainApp::MainApp() {
}

MainApp::~MainApp() {
}

void MainApp::SetTerminal(KeyManager* term) {
  m_term = term;
}

void MainApp::SetHelpPath(std::string helpPath) {
  m_helpPath = helpPath;
}

void MainApp::HelpWin() {
  ArrayVector dummy;
  HelpWinFunction(0,dummy,eval);
}

int MainApp::Run() {
  Context *context = new Context;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadHandleGraphicsFunctions(context);  
  m_term->setContext(context);

  //   QDir dirp(qApp->applicationDirPath() + "/../Plugins");
  //   QString path0(dirp.canonicalPath());
  //   qApp->addLibraryPath(path0);
  QDir dir1(qApp->applicationDirPath() + "/../Resources/mfiles");
  QString path1(dir1.canonicalPath());
  QDir dir2(qApp->applicationDirPath() + "/../Resources/help/text");
  QString path2(dir2.canonicalPath());
#ifdef WIN32
  QString synthpath(path1 + ";" + path2);
#else
  QString synthpath(path1 + ":" + path2);
#endif
  m_term->setPath(synthpath.toStdString());
//   const char *envPtr;
//   envPtr = getenv("FREEMAT_PATH");
//   if (envPtr)
//     m_term->setPath(std::string(envPtr));
//   else 
//     m_term->setPath(std::string(""));
  m_term->setAppPath(qApp->applicationDirPath().toStdString());
  eval = new WalkTree(context,m_term);
  eval->sendGreeting();
  eval->run();
  m_term->RestoreOriginalMode();
  qApp->quit();
  return 0;
}
