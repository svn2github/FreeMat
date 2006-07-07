#ifndef __WalkThread_hpp__
#define __WalkThread_hpp__

#include <QThread>
#include "WalkTree.hpp"

class WalkThread : public QThread
{
  Q_OBJECT
  Context *context;
  WalkTree *eval;
  bool guimode;

public slots:
  void evalString(std::string);
signals:
  void outputMessage(std::string);  
  void errorMessage(std::string);
  void warningMessage(std::string);
protected:
  void run();
public:
  void SetGuiMode(bool t) {guimode = t;}
};

#endif
