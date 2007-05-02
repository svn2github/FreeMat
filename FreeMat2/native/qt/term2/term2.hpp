#include <QAbstractScrollArea>
#include <QKeyEvent>
#include <QPaintEvent>

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

class QScreen : public QAbstractScrollArea {
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
public:
  QScreen();
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
  void OutputRawStringImmediate(string txt);
  void Flush();
signals:
  void OnChar(int c);
  void SetTextWidth(int);
};
