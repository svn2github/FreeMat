#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Scrollbar.H>
#include <string>

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

class TermWidget : public Fl_Widget
{
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
  Fl_Scrollbar *m_scrollbar;
  //  QTimer *m_timer_refresh;
  //  QTimer *m_timer_blink;
  bool cursorOn;
  bool blinkEnable;
  int m_scrollback;
  int m_history_lines;
  bool m_scrolling;
  bool m_mousePressed;
  int selectionStart;
  int selectionStop;
  int m_scroll_offset;
 public:
  TermWidget(int x, int y, int w, int h, const char *label = 0);
  void setScroller(Fl_Scrollbar* scrollbar);
  void scrollBack(int val);
  void setScrollbar(int val);
  virtual ~TermWidget();
  void resizeTextSurface();
  void ProcessChar(char c);
  void OutputString(std::string txt);

  void refresh();
  void blink();
  void adjustScrollbarPosition();
  void setCursor(int x, int y);
  //  void markDirty(QRect& e);

  //TK dependant functions
 protected:
  void resize(int x, int y, int w, int h);
  //  void paintEvent( QPaintEvent *e );
  void keyPressEvent( int key );
  void paintContents();
  
  void setFont(int size);
  void mousePressEvent(  int x, int y );
  void mouseDragEvent( int x, int y );
  void mouseReleaseEvent( int x, int y );

  // draw the widget
  void draw();
  int handle(int event);
  
};


#endif
