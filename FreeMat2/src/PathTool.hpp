#ifndef __PathTool_hpp__
#define __PathTool_hpp__

#include <QDialog>
#include <QListWidget>
#include <QDirModel>
#include <QTreeView>
#include <QSplitter>

class PathTool : public QDialog {
  Q_OBJECT
  QListWidget *m_flist;
  QDirModel *model;
  QTreeView *tree;
  QSplitter *splitter;
  bool modified;
public:
  PathTool();
private slots:
  void add();
  void addsub();
  void remove();
  void done();
  void save();
  void up();
  void down();
protected:
  void readSettings();
  void writeSettings();
};

#endif
