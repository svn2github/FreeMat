#ifndef __SocketCB_hpp__
#define __SocketCB_hpp__

#include <qobject.h>

typedef void (*cb)();

class SocketCB : public QObject {
  Q_OBJECT

  cb m_cb;
public:
  SocketCB(cb acb) {m_cb = acb;};
  virtual ~SocketCB() {};
 public slots:
 void activated(int) {m_cb();}
};

#endif
