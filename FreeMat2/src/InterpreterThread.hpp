#ifndef __InterpreterThread_hpp__
#define __InterpreterThread_hpp__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "Interpreter.hpp"
#include "Context.hpp"

using namespace FreeMat;

class InterpreterThread : public QThread
{
  Q_OBJECT
  Context *context;
  Interpreter *eval;
  std::vector<std::string> cmd_buffer;
  bool guimode;
  QMutex mutex;
  QWaitCondition condition;

protected:
  void run();
public:
  void ExecuteLine(std::string cmd);
  void SetGuiMode(bool t) {guimode = t;}
  Interpreter* GetInterpreter() {return eval;}
  void Setup();
signals:
  void Ready();
};

#endif
