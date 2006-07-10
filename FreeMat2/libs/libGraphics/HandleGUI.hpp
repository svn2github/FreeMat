#ifndef __HandleGUI_hpp__
#define __HandleGUI_hpp__

class CallbackSignalProxy : public QObject {
  Q_OBJECT
public:
  CallbackSignalProxy();
  void RegisterCallback(QWidget* source, );
private slots:

};

#endif
