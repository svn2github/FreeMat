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
#ifndef __GUITerminal_hpp__
#define __GUITerminal_hpp__

#include "QTTerm.hpp"
#include "KeyManager.hpp"

class GUITerminal : public QTTerm, public KeyManager {
  Q_OBJECT

  std::vector<std::string> cmd_list;
 protected:
  void resizeEvent( QResizeEvent *e );
 public:
  GUITerminal(QWidget *parent);
  void OutputRawString(std::string txt);
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  int getTerminalWidth() {return getTextWidth();};
  void ProcessChar(int c) {OnChar(c);};
  virtual char* getLine(std::string aprompt);
signals:
  void CommandLine(QString);
protected slots:
  void QueueString(QString t);
};

#endif
