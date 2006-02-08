#include "FileTool.hpp"
#include <QtGui>

FileTool::FileTool(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout;
  model = new QDirModel;
  tree = new QTreeView; 
  layout->addWidget(tree);
  setLayout(layout);
  tree->setModel(model);
  connect(tree,SIGNAL(doubleClicked(const QModelIndex&)), 
	  this,SLOT(doubleClicked(const QModelIndex&)));
}

void FileTool::doubleClicked(const QModelIndex& index) {
  emit sendCommand("'"+model->filePath(index)+"'");
}
