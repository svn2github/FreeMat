#ifndef __fmhelpapp_hpp__
#define __fmhelpapp_hpp__

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

#endif
