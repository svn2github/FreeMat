#ifndef __InterpreterThread_hpp__
#define __InterpreterThread_hpp__

#include <QThread>
#include "Interpreter.hpp"
#include "Context.hpp"

using namespace FreeMat;

class InterpreterThread : public QThread
{
  Q_OBJECT
  Context *context;
  Interpreter *eval;
  bool guimode;

public slots:
  void ExecuteLine(std::string);
protected:
  void run();
public:
  void SetGuiMode(bool t) {guimode = t;}
  Interpreter* GetInterpreter() {return eval;}
  void Setup();
};

#endif
