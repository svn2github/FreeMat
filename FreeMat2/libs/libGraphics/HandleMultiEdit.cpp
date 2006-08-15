#include "HandleMultiEdit.hpp"
#include <QtGui>

HandleMultiEdit::HandleMultiEdit(QWidget* parent) : QTextEdit(parent) {
}

void HandleMultiEdit::focusOutEvent(QFocusEvent* e) {
  emit editingFinished();
  QTextEdit::focusOutEvent(e);
}

void HandleMultiEdit::keyPressEvent(QKeyEvent* e) {
  if ((e->key() == Qt::Key_Return) &&
      ((e->modifiers() & Qt::AltModifier) != 0))
    emit editingFinished();
  else
    QTextEdit::keyPressEvent(e);
}
