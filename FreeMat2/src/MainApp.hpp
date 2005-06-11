#ifndef __MainApp_hpp__
#define __MainApp_hpp__

#include <string>
#include <qobject.h>
#include "BaseTerminal.hpp"

class MainApp : public QObject
{
  Q_OBJECT
  BaseTerminal* m_term;
  std::string m_helpPath;
public:
  MainApp();
  ~MainApp();
  void SetTerminal(BaseTerminal* term);
  void SetHelpPath(std::string helpPath);
public slots:
  int Run();
};

#endif
