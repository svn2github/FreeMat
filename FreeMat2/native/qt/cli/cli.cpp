#include <QtNetwork>

int main(int argc, char *argv[]) {
  for (int i=0;i<500;i++) {
    QTcpSocket sock;
    sock.connectToHost("127.0.0.1",6003);
    qDebug() << "Connect " << i;
    if (!sock.waitForConnected(1000)) {
      qDebug() << "Timeout on connect: " << sock.errorString();
      exit(1);
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    quint64 magic = 12345;
    out << magic;
    sock.write(block);
    qDebug() << "Write";
    if (!sock.waitForBytesWritten(1000)) {
      qDebug() << "TIMEOUT: " << sock.errorString();
      exit(1);
    }
    while (sock.bytesAvailable() < (int)sizeof(quint64)) {
      if (!sock.waitForReadyRead(1000)) {
	qDebug() << "TIMEOUT: " << sock.errorString();
	exit(1);
      }
    }
    QDataStream in(&sock);
    quint64 magic2;
    in >> magic2;
    qDebug() << "Read : " << magic2;
    sock.disconnectFromHost();
    qDebug() << "Disconnect";
    if (sock.state() != QAbstractSocket::UnconnectedState)
      if (!sock.waitForDisconnected(1000)) {
	qDebug() << "TIMEOUT: " << sock.errorString();
	exit(1);
      }
  }    
  return 0;
}
