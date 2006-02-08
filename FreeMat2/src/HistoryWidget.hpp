#ifndef __HistoryWidget_hpp__
#define __HistoryWidget_hpp__

#include <QWidget>
#include <QListWidget>
class HistoryWidget : public QWidget {
  Q_OBJECT
public:
  HistoryWidget(QWidget *parent);
private:
  QListWidget *m_flist;
  void readSettings();
  void writeSettings();
protected:
  void closeEvent(QCloseEvent*);
protected slots:
  void addCommand(QString t);
  void doubleClicked(QListWidgetItem* item);
signals:
  void sendCommand(QString t);
};

#endif
