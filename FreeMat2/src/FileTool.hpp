#ifndef __FileTool_hpp__
#define __FileTool_hpp__

#include <QDialog>
#include <QDirModel>
#include <QTreeView>

class FileTool : public QWidget {
  Q_OBJECT
  QDirModel *model;
  QTreeView *tree;
public:
  FileTool(QWidget *parent);
protected slots:
  void doubleClicked(const QModelIndex& index);
signals:
  void sendCommand(QString t);
};

#endif
