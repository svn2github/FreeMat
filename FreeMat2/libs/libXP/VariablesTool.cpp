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
#include "Array.hpp"
#include "Print.hpp"

VariablesTool::VariablesTool(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout;
  m_flist = new QTableWidget;
  layout->addWidget(m_flist);
  setLayout(layout);
  setObjectName("variables");
  context = NULL;
}

void VariablesTool::refresh() {
  if (!context) return;
  if (!context->getMutex()->tryLock()) return;
  QMutexLocker lock(context->getMutex());
  context->getMutex()->unlock();
  m_flist->clear();
  stringVector varnames(context->getCurrentScope()->listAllVariables());
  std::sort(varnames.begin(),varnames.end());
  m_flist->setRowCount(varnames.size());
  m_flist->setColumnCount(5);
  m_flist->setHorizontalHeaderLabels(QStringList() << "Name" << "Type" << "Flags" << "Size" << "Value");
  for (int i=0;i<varnames.size();i++) {
    QString varname(QString::fromStdString(varnames[i]));
    QString type;
    QString flags;
    QString size;
    QString value;
    Array lookup, *ptr;
    ptr = context->lookupVariable(varnames[i]);
    if (!ptr) {
      type = "undefined";
    } else {
      lookup = *ptr;
      Class t = lookup.getDataClass();
      switch(t) {
      case FM_CELL_ARRAY:
	type = "cell";
	break;
      case FM_STRUCT_ARRAY:
	if (lookup.isUserClass())
	  type = QString::fromStdString(lookup.getClassName().back());
	else
	  type = "struct";
	break;
      case FM_LOGICAL:
	type = "logical";
	break;
      case FM_UINT8:
	type = "uint8";
	break;
      case FM_INT8:
	type = "int8";
	break;
      case FM_UINT16:
	type = "uint16";
	break;
      case FM_INT16:
	type = "int16";
	break;
      case FM_UINT32:
	type = "uint32";
	break;
      case FM_INT32:
	type = "int32";
	break;
      case FM_UINT64:
	type = "uint64";
	break;
      case FM_INT64:
	type = "int64";
	break;
      case FM_FLOAT:
	type = "float";
	break;
      case FM_DOUBLE:
	type = "double";
	break;
      case FM_COMPLEX:
	type = "complex";
	break;
      case FM_DCOMPLEX:
	type = "dcomplex";
	break;
      case FM_STRING:
	type = "string";
	break;
      case FM_FUNCPTR_ARRAY:
	type = "func ptr";
	break;
      }
      if (lookup.isSparse())
	flags = "Sparse ";
      if (context->isVariableGlobal(varnames[i])) {
	flags += "Global ";
      } else if (context->isVariablePersistent(varnames[i])) {
	flags += "Persistent ";
      }
      size = QString::fromStdString(lookup.getDimensions().asString());
      value = QString::fromStdString(ArrayToPrintableString(lookup));
    }
    m_flist->setItem(i,0,new QTableWidgetItem(varname));
    m_flist->setItem(i,1,new QTableWidgetItem(type));
    m_flist->setItem(i,2,new QTableWidgetItem(flags));
    m_flist->setItem(i,3,new QTableWidgetItem(size));
    m_flist->setItem(i,4,new QTableWidgetItem(value));
  }
  m_flist->resizeColumnsToContents();
  m_flist->verticalHeader()->hide();
}

void VariablesTool::setContext(Context *watch) {
  context = watch;
  refresh();
}
