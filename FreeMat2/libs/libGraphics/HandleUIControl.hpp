#ifndef __HandleUIControl_hpp__
#define __HandleUIControl_hpp__

#include "HandleObject.hpp"
#include <QObject>

class HandleWindow;
class Interpreter;

class HandleUIControl : public QObject, public HandleObject {
  Q_OBJECT
  QWidget *widget;
  Interpreter *m_eval;
  HandleWindow *parentWidget;
public:
  HandleUIControl();
  virtual ~HandleUIControl();
  virtual void ConstructProperties();
  virtual void SetupDefaults();
  virtual void UpdateState();
  virtual void PaintMe(RenderEngine&) {};
  void Hide();
  void ConstructWidget(HandleWindow *f);
  void SetEvalEngine(Interpreter *eval);
private slots:
void clicked();
};
  

#endif
