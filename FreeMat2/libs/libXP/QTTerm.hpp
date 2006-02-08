#ifndef __QTTerm_hpp__
#define __QTTerm_hpp__

#include "TermWidget.hpp"
#include <qscrollbar.h>
#include <qtimer.h>
#include <qpixmap.h>

class QTTerm :  public QWidget, public TermWidget {
  Q_OBJECT

  QScrollBar *m_scrollbar;
  QTimer *m_timer_refresh;
  QTimer *m_timer_blink;
  QPainter *paint;
  QPixmap surface;
public:
  QTTerm(QWidget *parent, const char *name);
  ~QTTerm();

  // implement the TermWidget Interface
  virtual int GetHeight();
  virtual int GetWidth();
  virtual void InstallEventTimers();
  virtual void ScrollLineUp();
  virtual void ScrollLineDown();
  virtual void SetScrollBarValue(int val);
  virtual void SetupScrollBar(int minval, int maxval, 
			      int step, int page, int val);
  virtual void BeginDraw();
  virtual void DrawTagChar(QPainter* paint, int x, int y, tagChar g);
  virtual void PutTagChar(int x, int y, tagChar g);
  virtual void EndDraw();
  virtual void setFont(int size);
  virtual void Erase();
  virtual void ProcessChar(int c) = 0;
  void adjustScrollbarPosition();

  char *getSelectionText();
  char* QTTerm::getTextSurface(int &count, int &width);

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
};

#endif
