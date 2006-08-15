#ifndef __HandleMultiEdit_hpp__
#define __HandleMultiEdit_hpp__

#include <QTextEdit>

class HandleMultiEdit : public QTextEdit
{
  Q_OBJECT

public:
  HandleMultiEdit(QWidget* parent);
  void focusOutEvent(QFocusEvent* e);
  void keyPressEvent(QKeyEvent* e);
signals:
  void editingFinished();
};

#endif
