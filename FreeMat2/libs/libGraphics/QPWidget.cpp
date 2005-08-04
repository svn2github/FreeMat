#include "QPWidget.hpp"

namespace FreeMat {

QPWidget::QPWidget(QWidget* parent, const char *Name) :
  QWidget(parent,Name) {
}

void QPWidget::paintEvent(QPaintEvent* e) {
  QWidget::paintEvent(e);
  QPainter gc(this);
  DrawMe(gc);
}

}
