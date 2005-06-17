#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <qframe.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qpixmap.h>
#include "KeyManager.hpp"

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
  void toggleCursor() {flags ^= CURSORBIT;};
  void setCursor() {flags |= CURSORBIT;};
  void clearCursor() {flags &= ~CURSORBIT;};
  void clearSelection() {flags &= ~SELECTBIT;};
  void setSelection() {flags |= SELECTBIT;};
  bool operator == (const tagChar& b) {return (v==b.v) && (flags==b.flags);};
};

class TermWidget : public QFrame
{
  Q_OBJECT

protected:
  tagChar* m_surface;  // width x height
  int m_height;        // height of terminal in characters
  int m_width;         // width of terminal in characters
  int m_cursor_x;      // column position of the cursor
  int m_cursor_y;      // row position of the cursor
  tagChar* m_onscreen; // width x height - contains contents of the screen
  tagChar* m_history;  // width x scrollheight - contains history of terminal
  int m_char_w;        // width of a character
  int m_char_h;        // height of a character
  int m_active_width;  // width of the active text region.
  bool m_clearall;
  QScrollBar *m_scrollbar;
  QTimer *m_timer_refresh;
  QTimer *m_timer_blink;
  bool cursorOn;
  bool blinkEnable;
  QRect cursorRect;
  int m_scrollback;
  int m_history_lines;
  QPixmap pm_cursor;
  bool m_scrolling;
  //  QPixmap buffer;
  bool m_mousePressed;
  int selectionStart;
  int selectionStop;
  int m_scroll_offset;
  bool m_firsttime;
 public slots:
  void scrollBack(int val);
 public:
  TermWidget(QWidget *parent=0, const char *name=0);
  virtual ~TermWidget();
  void resizeTextSurface();
  virtual void ProcessChar(int c) = 0;
  void PutString(std::string txt);
  public slots:
  void refresh();
  void blink();
  void adjustScrollbarPosition();
  char* getSelectionText();
  char* getTextSurface(int& count, int& width);
  void setCursor(int x, int y);
  void markDirty(QRect& e);
  int  getTextWidth() {return m_width;};
  //TK dependant functions
  void setFont(QFont font);
  QFont getFont();
 protected:
  void setScrollbar(int val);
  void resizeEvent( QResizeEvent *e );
  void paintEvent( QPaintEvent *e );
  void keyPressEvent( QKeyEvent *e );
  void paintContents(QPainter &paint);
  void setFont(int size);
  void mousePressEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
};


#endif
