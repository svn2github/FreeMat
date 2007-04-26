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
#ifndef __QTTerm_hpp__
#define __QTTerm_hpp__

#include <QTextEdit>
#include <string>
#include <QKeyEvent>
#include <QTimer>

using namespace std;

class QTTerm :  public QTextEdit {
  Q_OBJECT
  QString putbuf;
  QTimer *autoFlush;
  QTextCursor destCursor;
  QFont fnt;
  int m_char_w;
  int m_twidth;
public:
  QTTerm(QWidget *parent);

  // The QT interface goes here
  void setFont(QFont font);
  QFont getFont();
  QString getAllText();
  QString getSelectionText();
  void clearSelection();
  void adjustScrollback();
protected:
  void keyPressEvent( QKeyEvent *e );
  void resizeEvent( QResizeEvent *e );
  void Output(QString frag);
  void UpdateTextWidth();
 public slots:
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  void ClearDisplay();
  void OutputRawString(string txt);
  void OutputRawStringImmediate(string txt);
  void Flush();
signals:
 void OnChar(int c);
 void SetTextWidth(int);
};

#endif
