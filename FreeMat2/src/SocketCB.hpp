#ifndef __SocketCB_hpp__
#define __SocketCB_hpp__

#include <qobject.h>

typedef void (*cb)();

class SocketCB : public QObject {
  Q_OBJECT

  cb m_cb;
public:
  SocketCB(cb acb);
  virtual ~SocketCB();
 public slots:
 void activated(int);
};

#endif
