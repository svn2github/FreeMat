#include <QLabel>
#include <QImage>
#include <QScrollArea>
#include <QApplication>
#include <QKeyEvent>

class QScreen : public QWidget {
  //class QScreen : public QLabel {
Q_OBJECT
QStringList m_text;
public:
  QScreen();
  QSize sizeHint() const;
protected:
  void keyPressEvent(QKeyEvent *e);
  void paintEvent(QPaintEvent *e);
};
