#ifndef __blastuneapp_hpp__
#define __blastuneapp_hpp__

#include <QWidget>
#include <QTableWidget>

class SingleSelectTable : public QTableWidget {
  Q_OBJECT
private slots:
  void forceSingleSelect();
public:
  SingleSelectTable(int rows, int cols);
};

class TunerWidget : public QWidget {
  Q_OBJECT
private slots:
  void addlibs();
  void runtime();
  void selectlib();
public:
  TunerWidget();
  virtual ~TunerWidget() {};
};

#endif
