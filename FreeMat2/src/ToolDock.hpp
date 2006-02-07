#ifndef __ToolDock_hpp__
#define __ToolDock_hpp__

#include <QDockWidget>
#include "HistoryWidget.hpp"

class ToolDock : public QDockWidget {
  Q_OBJECT
  HistoryWidget *m_history;
public:
  ToolDock(QWidget *parent);
  ~ToolDock();
  HistoryWidget* getHistoryWidget();
};

#endif
