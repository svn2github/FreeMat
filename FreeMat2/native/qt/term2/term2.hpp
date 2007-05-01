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
  void toggleCursor() {flags ^= CURSORBIT;};
  void setCursor() {flags |= CURSORBIT;};
  void clearCursor() {flags &= ~CURSORBIT;};
  void clearSelection() {flags &= ~SELECTBIT;};
  void setSelection() {flags |= SELECTBIT;};
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
public:
  QScreen();
  void setFont(int size);
  void ensureCursorVisible();
  void outputString(QString txt);
protected:
  void setChar(char t);
  void nextLine();
  void keyPressEvent(QKeyEvent *e);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);
private:
  void calcGeometry();
  void drawLine(int linenum, QPainter *e, int yval);
};
