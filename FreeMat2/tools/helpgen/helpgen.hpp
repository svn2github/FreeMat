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
#ifndef __helpgen_hpp__
#define __helpgen_hpp__

#include <QWidget>

class ConsoleWidget : public QWidget {
  Q_OBJECT
private slots:
  void LaunchHelpWindow();
  void exitNow();
  void Run();
public:
  ConsoleWidget();
  virtual ~ConsoleWidget() {};
};

class OutputHelper : public QObject {
  Q_OBJECT
public:
  OutputHelper() : QObject() {}
private slots:
  void AddOutputString(std::string);
  void MoveDown();
};

void DoHelpGen(QString src, QString dst);

#endif
