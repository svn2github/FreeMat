#ifndef __MainApp_hpp__
#define __MainApp_hpp__

#include <string>
#include <qobject.h>
#include "KeyManager.hpp"
#include "WalkTree.hpp"

class MainApp : public QObject
{
  Q_OBJECT
  KeyManager* m_term;
  std::string m_helpPath;
  WalkTree* eval;
  bool guimode;
  bool skipGreeting;
public:
  MainApp();
  ~MainApp();
  void SetTerminal(KeyManager* term);
  void SetHelpPath(std::string helpPath);
  void SetGUIMode(bool mode);
  void SetSkipGreeting(bool skip);
public slots:
  int Run();
  void HelpWin();
  void PathTool();
  void Editor();
};

#endif
