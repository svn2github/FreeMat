/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
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
  setObjectName("history");
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
  if (!t.isEmpty()) {
    if (m_flist->count() >= 1000) {
      QListWidgetItem *p = m_flist->takeItem(0);
      delete p;
    }
    QListWidgetItem *d = new QListWidgetItem(t,m_flist);
    m_flist->setCurrentItem(d);
  }
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

