#ifndef __HandleUIControl_hpp__
#define __HandleUIControl_hpp__

#include "HandleObject.hpp"
#include <QObject>

namespace FreeMat {
  class HandleWindow;
  class Interpreter;

  class HandleUIControl : public QObject, public HandleObject {
    Q_OBJECT
    QWidget *widget;
    Interpreter *m_eval;
  public:
    HandleUIControl();
    virtual ~HandleUIControl();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
    virtual void PaintMe(RenderEngine&) {};
    void ConstructWidget(HandleWindow *f);
    void SetEvalEngine(Interpreter *eval);
  private slots:
    void clicked();
  };
  
}

#endif
