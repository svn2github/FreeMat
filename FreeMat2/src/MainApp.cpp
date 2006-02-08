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
#include "DumbTerminal.hpp"
#include "Terminal.hpp"
#include <fcntl.h>
#include <qsocketnotifier.h>
#include "SocketCB.hpp"

MainApp::MainApp() {
  guimode = true;
  skipGreeting = false;
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

void MainApp::PathTool() {
  ArrayVector dummy;
  PathToolFunction(0,dummy,eval);
}

void MainApp::Editor() {
  ArrayVector dummy;
  EditorFunction(0,dummy,eval);
}

void MainApp::SetGUIMode(bool mode) {
  guimode = mode;
}

void MainApp::SetSkipGreeting(bool skip) {
  skipGreeting = skip;
}

int MainApp::Run() {
  Context *context = new Context;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  if (guimode) {
    LoadGUICoreFunctions(context);
    LoadHandleGraphicsFunctions(context);  
  }
  m_term->setContext(context);
  QDir dir1(qApp->applicationDirPath() + "/../Resources/mfiles");
  QString path1(dir1.canonicalPath());
  QDir dir2(qApp->applicationDirPath() + "/../Resources/help/text");
  QString path2(dir2.canonicalPath());
  QStringList basePath(GetRecursiveDirList(path1) + GetRecursiveDirList(path2));
  m_term->setBasePath(basePath);
  QSettings settings("FreeMat","FreeMat");
  QStringList userPath = settings.value("interpreter/path").toStringList();
  m_term->setUserPath(userPath);
  m_term->setAppPath(qApp->applicationDirPath().toStdString());
  eval = new WalkTree(context,m_term);
  if (!skipGreeting)
    eval->sendGreeting();
  eval->run();
  m_term->RestoreOriginalMode();
  qApp->quit();
  return 0;
}
