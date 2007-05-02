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


#include <QAbstractScrollArea>
#include <QKeyEvent>
#include <QPaintEvent>
#include <string>

using namespace std;

#define CURSORBIT 1
#define SELECTBIT 2
class tagChar
{
 public:
  char v;
  char flags;
  tagChar(): v(' '), flags(0) {};
  tagChar(char t): v(t), flags(0) {};
  bool noflags() {return(flags == 0);};
  bool cursor() {return(flags & CURSORBIT);};
  bool selected() {return(flags & SELECTBIT);};
  void toggleCursor() {flags ^= CURSORBIT;};
  void setCursor() {flags |= CURSORBIT;};
  void clearCursor() {flags &= ~CURSORBIT;};
  void clearSelection() {flags &= ~SELECTBIT;};
  void setSelection() {flags |= SELECTBIT;};
  char mflags() const {return flags;};
  bool operator == (const tagChar& b) {return (v==b.v) && (flags==b.flags);};
};

const int maxlen = 256;

class tagLine {
public:
  tagChar data[maxlen];
};

class QTTerm : public QAbstractScrollArea {
  Q_OBJECT
  QList<tagLine> buffer;
  int cursor_x, cursor_y;
  QFont fnt;
  int m_char_w, m_char_h;
  int m_term_width, m_term_height;
  QTimer *m_timer_blink;
  bool blinkEnable;
  bool m_blink_skip;
  int selectionStart;
  int selectionStop;
  int scrollback;
public:
  QTTerm();
  QString getSelectionText();
  QString getAllText();
  void setFont(QFont font);
  QFont getFont();
  void clearSelection();
protected:
  void ensureCursorVisible();
  void setChar(char t);
  void nextLine();
  void keyPressEvent(QKeyEvent *e);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
  void focusOutEvent(QFocusEvent *e);
  void focusInEvent(QFocusEvent *e);
  void mousePressEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
private:
  void calcGeometry();
  void clearCursor();
  void setCursor();
  void drawLine(int linenum, QPainter *e, int yval);
  void drawFragment(QPainter *e, QString todraw, char flags, int row, int col);
public slots:
  void blink();
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  void ClearDisplay();
  void OutputRawString(string txt);
signals:
  void OnChar(int c);
  void SetTextWidth(int);
};

#endif
