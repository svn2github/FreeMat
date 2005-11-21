#include "QPWidget.hpp"
#include "QPainterDrawEngine.hpp"
#include <qpainter.h>

namespace FreeMat {

QPWidget::QPWidget(QWidget* parent, const char *Name) :
  QWidget(parent) {
}

void QPWidget::paintEvent(QPaintEvent* e) {
  QWidget::paintEvent(e);
  QPainter gc(this);
  QPainterDrawEngine dc(gc);
  DrawMe(dc);
}

}
