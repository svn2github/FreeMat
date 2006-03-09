#ifndef __disttool_hpp__
#define __disttool_hpp__

#include <QWidget>

class ConsoleWidget : public QWidget {
  Q_OBJECT
private slots:
  void WinBundle();
  void LinuxBundle();
  void MacBundle();
  void SrcBundle();
  void exitNow();
public:
  ConsoleWidget();
  virtual ~ConsoleWidget() {};
};

#endif
