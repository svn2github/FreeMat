#ifndef __XML_hpp__
#define __XML_hpp__

#include <QUrl>
#include <QFile>
#include <QThread>
#include <QDebug>
#include <QHttp>
#include <QEventLoop>

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
  bool error() {return m_err;}
  void run();
public slots:
  void requestFinished(int id, bool err);
};

#endif
