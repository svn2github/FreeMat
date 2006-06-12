#ifndef __HandleGUI_hpp__
#define __HandleGUI_hpp__

#include "WalkTree.hpp"

class CallbackSignalProxy : public QObject {
  Q_OBJECT
public:
  CallbackSignalProxy();
  void RegisterCallback(QWidget* source, );
private slots:

};

#endif
