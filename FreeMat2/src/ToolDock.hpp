#ifndef __ToolDock_hpp__
#define __ToolDock_hpp__

#include <QDockWidget>
#include "HistoryWidget.hpp"
#include "FileTool.hpp"

class ToolDock : public QDockWidget {
  Q_OBJECT
  HistoryWidget *m_history;
  FileTool *m_filetool;
public:
  ToolDock(QWidget *parent);
  ~ToolDock();
  HistoryWidget* getHistoryWidget();
  FileTool* getFileTool();
};

#endif
