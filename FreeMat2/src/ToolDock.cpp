#include "ToolDock.hpp"
#include <QtGui>

ToolDock::ToolDock(QWidget *parent) : QDockWidget(parent) {
  QToolBox *tb = new QToolBox(this);
  m_history = new HistoryWidget(tb);
  tb->addItem(m_history,"History");
  m_filetool = new FileTool(tb);
  tb->addItem(m_filetool,"Files");
  setWidget(tb);
}

ToolDock::~ToolDock() {
  m_history->close();
  m_filetool->close();
}

HistoryWidget* ToolDock::getHistoryWidget() {
  return m_history;
}
  
FileTool* ToolDock::getFileTool() {
  return m_filetool;
}
