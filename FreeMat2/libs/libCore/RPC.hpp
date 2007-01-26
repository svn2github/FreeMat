#ifndef __RPC_hpp__
#define __RPC_hpp__

#include <QString>
#include <QUuid>
#include <QTcpSocket>
#include <QTcpServer>
#include "Array.hpp"

class Slot {
public:
  bool    slotFilled;
  bool    success;
  Array   value;
  QString errMsg;
  QUuid   senderID;
};

class RPCClient : public QObject {
  Q_OBJECT
  QTcpSocket *m_sock;
  Slot *m_dest;
  quint64 blockSize;
public:
  RPCClient(QTcpSocket *sock, Slot *dst);
  void failMsg(QString errMsg);
private slots:
  void readData();
  void error(QAbstractSocket::SocketError socketError);
};

class RPCMgr : public QObject {
  Q_OBJECT
  QTcpServer *tcpServer;
public:
  bool open(uint16 portnum);
private slots:
  void newConnection();
};

#endif
