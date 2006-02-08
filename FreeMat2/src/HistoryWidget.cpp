#include <QtGui>
#include <QDebug>
#include "HistoryWidget.hpp"

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout;
  m_flist = new QListWidget;
  layout->addWidget(m_flist);
  setLayout(layout);
  readSettings();
  new QListWidgetItem("% " + QDateTime::currentDateTime().toString(),m_flist);
  connect(m_flist,SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	  this,SLOT(doubleClicked(QListWidgetItem*)));
}

void HistoryWidget::doubleClicked(QListWidgetItem* item) {
  emit sendCommand(item->text()+"\n");
}

void HistoryWidget::closeEvent(QCloseEvent *ce) {
  writeSettings();
  ce->accept();
}

void HistoryWidget::addCommand(QString t) {
  t.chop(1);
  if (!t.isEmpty())
    new QListWidgetItem(t,m_flist);
}

void HistoryWidget::readSettings() {
  QSettings settings("FreeMat", "FreeMat");
  QStringList historyList = settings.value("interpreter/history").toStringList();
  for (int i=0;i<historyList.size();i++)
    new QListWidgetItem(historyList[i],m_flist);
}

void HistoryWidget::writeSettings() {
  QSettings settings("FreeMat","FreeMat");
  QStringList historyList;
  for (int i=0;i<m_flist->count();i++) {
    QListWidgetItem *item = m_flist->item(i);
    historyList << item->text();
  }
  settings.setValue("interpreter/history",historyList);
}

