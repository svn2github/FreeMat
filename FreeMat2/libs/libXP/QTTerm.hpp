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

#include <QWidget>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qpixmap.h>
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
  char mflags() {return flags;};
  bool noflags() {return(flags == 0);};
  bool cursor() {return(flags & CURSORBIT);};
  void toggleCursor() {flags ^= CURSORBIT;};
  void setCursor() {flags |= CURSORBIT;};
  void clearCursor() {flags &= ~CURSORBIT;};
  void clearSelection() {flags &= ~SELECTBIT;};
  void setSelection() {flags |= SELECTBIT;};
  bool operator == (const tagChar& b) {return (v==b.v) && (flags==b.flags);};
};

class tagArray {
  tagChar* m_buffer;
  int m_length;
 public:
  tagArray(int length = 0, tagChar* ptr = 0);
  tagChar* buffer();
  tagChar& operator[](int pos);
};

class QTTerm :  public QWidget {
  Q_OBJECT

  QScrollBar *m_scrollbar;
  QTimer *m_timer_refresh;
  QTimer *m_timer_blink;
  QPainter *paint;
public:
  QTTerm(QWidget *parent);
  ~QTTerm();

  void resizeTextSurface();
  void PutString(string txt);
  void SetCursor(int x, int y);
  int  getTextWidth() {return m_width;};
  void Initialize();
  //TK dependant functions
public:
  void setScrollbar(int val);
  void DrawContent();
  void EnableBlink();
  void DisableBlink();
  // implement the TermWidget Interface
  virtual int GetHeight();
  virtual int GetWidth();
  virtual void InstallEventTimers();
  virtual void ScrollLineUp();
  virtual void ScrollLineDown();
  virtual void SetScrollBarValue(int val);
  virtual void SetupScrollBar(int minval, int maxval, 
			      int step, int page, int val);
  virtual void DrawFragment(QPainter*, QString text, char flags, int row, int col);
  virtual void PutTagChar(int x, int y, tagChar g);
  virtual void setFont(int size);
  void adjustScrollbarPosition();

  char *getSelectionText();
  char *getTextSurface(int &count, int &width);

  // The QT interface goes here
  void setFont(QFont font);
  QFont getFont();
protected:
  void resizeEvent( QResizeEvent *e );
  void paintEvent( QPaintEvent *e );
  void keyPressEvent( QKeyEvent *e );
  void paintContents(QPainter &paint);
  void mousePressEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
  void focusOutEvent(QFocusEvent *e);
  void focusInEvent(QFocusEvent *e);
  void PaintRectangle(QPainter *paint, QRect rect);
  bool event(QEvent *e);
 public slots:
  void scrollBack(int val);
  void blink();
  void refresh();
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  void OutputRawString(string txt);
signals:
 void OnChar(int c);
 void SetTextWidth(int);
protected:
  int m_height;        // height of terminal in characters
  int m_width;         // width of terminal in characters
  int m_cursor_x;      // column position of the cursor
  int m_cursor_y;      // row position of the cursor
  tagArray m_surface;  // width x height
  tagArray m_onscreen; // width x height - contains contents of the screen
  tagArray m_history;  // width x scrollheight - contains history of terminal
  int m_char_w;        // width of a character
  int m_char_h;        // height of a character
  int m_char_decent;   // descent of a character
  int m_active_width;  // width of the active text region.
  bool m_blink_skip;
  bool m_clearall;
  bool cursorOn;
  bool blinkEnable;
  int m_scrollback;
  int m_history_lines;
  bool m_scrolling;
  bool m_mousePressed;
  int selectionStart;
  int selectionStop;
  int m_scroll_offset;
  int m_scrollmin, m_scrollmax, m_scrollline, m_scrollpage;
  bool m_firsttime;
  bool m_surface_initialized;
  bool m_history_initialized;
};

#endif
