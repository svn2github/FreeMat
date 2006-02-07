#ifndef __helpgen_hpp__
#define __helpgen_hpp__

#include <QWidget>

class ConsoleWidget : public QWidget {
  Q_OBJECT
private slots:
  void LaunchHelpWindow();
  void exitNow();
  void Run();
public:
  ConsoleWidget();
  virtual ~ConsoleWidget() {};
};

void DoHelpGen();

#endif
