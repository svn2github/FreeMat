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
#include "VariablesTool.hpp"
#include "Scope.hpp"
#include <QtGui>

VariablesTool::VariablesTool(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout;
  m_flist = new QListWidget;
  layout->addWidget(m_flist);
  setLayout(layout);
  setObjectName("variables");
}

void VariablesTool::refresh() {
  int cnt = m_flist->count();
  for (int i=0;i<cnt;i++)
    delete m_flist->takeItem(cnt-1-i);
  FreeMat::stringVector varnames(scope->listAllVariables());
  for (int i=0;i<varnames.size();i++)
    new QListWidgetItem(QString::fromStdString(varnames[i]),m_flist);
}

void VariablesTool::setScope(FreeMat::Scope *watch) {
  scope = watch;
  refresh();
}
