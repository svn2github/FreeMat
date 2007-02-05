#include <QtNetwork>

int main(int argc, char *argv[]) {
  QTcpServer server;
  server.listen(QHostAddress::Any,6003);
  while (1) {
    qDebug() << "Waiting";
    if (!server.waitForNewConnection(30000))
      qDebug() << "Timeout!";
    else {
      qDebug() << "Connecting";
      QTcpSocket *sock = server.nextPendingConnection();
      qDebug() << "State: " << sock->state();
      while (sock->bytesAvailable() < (int)sizeof(quint64)) {
	if (!sock->waitForReadyRead(1000)) {
	  qDebug() << "TIMEOUT: " << sock->errorString();
	  exit(1);
	}
      }
      QDataStream in(sock);
      quint64 magic;
      in >> magic;
      qDebug() << "Read";
      qDebug() << "Got magic number... " << magic;
      QByteArray block;
      QDataStream out(&block, QIODevice::WriteOnly);
      out << magic;
      sock->write(block);
      qDebug() << "Write";
      if (!sock->waitForBytesWritten(1000)) {
	qDebug() << "TIMEOUT: " << sock->errorString();
	exit(1);
      }
      sock->disconnectFromHost();
      qDebug() << "Disconnect";
      if (sock->state() != QAbstractSocket::UnconnectedState)
	if (!sock->waitForDisconnected(1000)) {
	  qDebug() << "TIMEOUT: " << sock->errorString();
	  exit(1);
	}
      delete sock;
    }
  }
}
