#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <qframe.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qpixmap.h>

class TermWidget : public QFrame
{
  Q_OBJECT

  char* m_surface; // width x height
  int m_height;    // height of terminal in characters
  int m_width;     // width of terminal in characters
  int m_cursor_x;  // column position of the cursor
  int m_cursor_y;  // row position of the cursor
  char* m_onscreen; // width x height - contains contents of the screen
  int m_char_w;    // width of a character
  int m_char_h;    // height of a character
  bool m_clearall;
  QScrollBar *m_scrollbar;
  QTimer *m_timer;
  //  QPixmap buffer;
 public:
  TermWidget(QWidget *parent=0, const char *name=0);
  virtual ~TermWidget();
  void resizeTextSurface();
  void ProcessChar(char c);
  void OutputString(std::string txt);
  public slots:
  void refresh();
  //TK dependant functions
 protected:
  void resizeEvent( QResizeEvent *e );
  void paintEvent( QPaintEvent *e );
  void keyPressEvent( QKeyEvent *e );
  void paintContents(QPainter &paint, const QRect &rect, bool pm);
  void setFont(int size);
};


#endif
