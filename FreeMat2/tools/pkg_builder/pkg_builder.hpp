#include <QtNetwork>
#include <QtCore>

class URLRetriever : public QObject
{
  Q_OBJECT

  QUrl m_url;
  QFile *m_file;
  double m_timeout;
  int m_httpGetId;
  QEventLoop m_event;
  QHttp m_http;
  bool m_err;
public:
  URLRetriever(QUrl url, QFile *file, double timeout) :
    m_url(url), m_file(file), m_timeout(timeout), m_err(true) {}
  bool error() {
    return m_err;
  }
  void run() {
    m_http.setHost(m_url.host(), m_url.port() != -1 ? m_url.port() : 80);
    if (!m_url.userName().isEmpty())
      m_http.setUser(m_url.userName(), m_url.password());
    m_httpGetId = m_http.get(m_url.path(), m_file);
    QTimer::singleShot((int)m_timeout, &m_http, SLOT(abort()));
    connect(&m_http, SIGNAL(done(bool)), &m_event, SLOT(quit()));
    connect(&m_http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
    m_event.exec();
  }
public slots:
  void requestFinished(int id, bool err) {
    qDebug() << QString("Request ID %1").arg(id);
    if (id != m_httpGetId)
      return;
    m_err = err;
    if (err) 
      qDebug() << QString("Download failed: %1").arg(m_http.errorString());
  }
};

