class Slot {
public:
  bool    slotFilled;
  bool    success;
  Array   value;
  QString errMsg;
  QUuid   senderID;
};

class RPCClient : QObject {
  Q_OBJECT
  QTcpSocket *m_sock;
  Slot *m_dest;
  quint64 blockSize;
public:
  RPCClient(QTcpSocket *sock, Slot *dst)
private slots:
  void readData();
  void error(QAbstractSocket::SocketError socketError);
}

class RPCMgr : QObject {
  Q_OBJECT
  QTcpServer *tcpServer;
public:
  bool open(uint16 portnum);
private slots:
  void newConnection();
};

