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
#include "ToolDock.hpp"
#include <QtGui>

ToolDock::ToolDock(QWidget *parent) : QDockWidget(parent) {
  QToolBox *tb = new QToolBox(this);
  m_history = new HistoryWidget(tb);
  tb->addItem(m_history,"History");
  m_filetool = new FileTool(tb);
  tb->addItem(m_filetool,"Files");
  m_variables = new VariablesTool(tb);
  tb->addItem(m_variables,"Workspace");
  setWidget(tb);
  setObjectName("tooldock");
}

ToolDock::~ToolDock() {
  m_history->close();
  m_filetool->close();
  m_variables->close();
}

HistoryWidget* ToolDock::getHistoryWidget() {
  return m_history;
}
  
FileTool* ToolDock::getFileTool() {
  return m_filetool;
}

VariablesTool* ToolDock::getVariablesTool() {
  return m_variables;
}
