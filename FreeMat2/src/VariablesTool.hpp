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
#ifndef __VariablesTool_hpp__
#define __VariablesTool_hpp__

#include <QListWidget>
#include <QWidget>
#include "Scope.hpp"

class VariablesTool : public QWidget {
  Q_OBJECT
public:
  VariablesTool(QWidget *parent);
protected slots:
  void refresh();
  void setScope(FreeMat::Scope *watch);
private:
  FreeMat::Scope *scope;
  QListWidget *m_flist;
};

#endif
