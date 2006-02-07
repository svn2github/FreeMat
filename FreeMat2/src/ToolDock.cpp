#include "ToolDock.hpp"
#include <QtGui>

ToolDock::ToolDock(QWidget *parent) : QDockWidget(parent) {
  QToolBox *tb = new QToolBox(this);
  m_history = new HistoryWidget(tb);
  tb->addItem(m_history,"History");
  setWidget(tb);
}

ToolDock::~ToolDock() {
  m_history->close();
}

HistoryWidget* ToolDock::getHistoryWidget() {
  return m_history;
}
  
