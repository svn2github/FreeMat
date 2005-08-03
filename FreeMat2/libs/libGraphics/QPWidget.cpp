#include "QPWidget.hpp"

namespace FreeMat {

QPWidget::QPWidget(QWidget* parent, const char *Name, WFlags f) :
  QWidget(parent,Name,f) {
}

void QPWidget::paintEvent(QPaintEvent* e) {
  QWidget::paintEvent(e);
  QPainter gc(this);
  DrawMe(gc);
}

}
