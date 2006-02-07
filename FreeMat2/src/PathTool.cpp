#include "PathTool.hpp"
#include <QtGui>

PathTool::PathTool() {
  QHBoxLayout *lay = new QHBoxLayout(this);
  splitter = new QSplitter(this);
  lay->addWidget(splitter);
  setWindowIcon(QPixmap(":/images/freemat-2.xpm"));

  model = new QDirModel;
  model->setFilter(QDir::AllDirs);

  tree = new QTreeView(splitter);
  tree->setModel(model);
  tree->setColumnHidden(1,true);
  tree->setColumnHidden(2,true);
  tree->setColumnHidden(3,true);

  m_flist = new QListWidget(splitter);

  QWidget *buttonpane = new QWidget;
  QVBoxLayout *blayout = new QVBoxLayout(buttonpane);
  QPushButton *add = new QPushButton("Add");
  QPushButton *remove = new QPushButton("Remove");
  QPushButton *up = new QPushButton("Move Up");
  QPushButton *down = new QPushButton("Move Down");
  QPushButton *save = new QPushButton("Save");
  QPushButton *done = new QPushButton("Done");
  blayout->addWidget(add);
  blayout->addWidget(remove);
  blayout->addWidget(up);
  blayout->addWidget(down);
  blayout->addWidget(save);
  blayout->addWidget(done);

  lay->addWidget(buttonpane);

  setLayout(lay);

  connect(add,SIGNAL(clicked()),this,SLOT(add()));
  connect(remove,SIGNAL(clicked()),this,SLOT(remove()));
  connect(up,SIGNAL(clicked()),this,SLOT(up()));
  connect(down,SIGNAL(clicked()),this,SLOT(down()));
  connect(save,SIGNAL(clicked()),this,SLOT(save()));
  connect(done,SIGNAL(clicked()),this,SLOT(done()));
  // Populate the list based on the current path...
  QSettings settings("FreeMat","FreeMat");
  QStringList path = settings.value("interpreter/path").toStringList();
  for (int i=0;i<path.size();i++)
    new QListWidgetItem(path[i],m_flist);
  readSettings();
}

void PathTool::readSettings() {
  QSettings settings("FreeMat", "FreeMat");
  QPoint gpos = settings.value("pathtool/pos", QPoint(200, 200)).toPoint();
  QSize gsize = settings.value("pathtool/size", QSize(650, 400)).toSize();
  int size0 = settings.value("pathtool/splitter/left", 250).toInt();
  int size1 = settings.value("pathtool/splitter/right", 250).toInt();
  QList<int> sizes;
  sizes << size0 << size1;
  resize(gsize);
  move(gpos);
  splitter->setSizes(sizes);
}

void PathTool::writeSettings() {
  QSettings settings("FreeMat", "FreeMat");
  settings.setValue("pathtool/pos", pos());
  settings.setValue("pathtool/size", size());
  settings.setValue("pathtool/splitter/left", splitter->sizes()[0]);
  settings.setValue("pathtool/splitter/right", splitter->sizes()[1]);
  settings.sync();
}

void PathTool::add() {
  QItemSelectionModel *select = tree->selectionModel();
  new QListWidgetItem(model->filePath(select->currentIndex()),m_flist);
}

void PathTool::remove() {
  QList<QListWidgetItem*> todelete(m_flist->selectedItems());
  for (int i=0;i<todelete.size();i++) {
    m_flist->takeItem(m_flist->row(todelete[i]));
    delete todelete[i];
  }
}

void PathTool::up() {
  QList<QListWidgetItem*> todelete(m_flist->selectedItems());
  if (todelete.size() > 0) {
    int rownum = m_flist->row(todelete[0]);
    m_flist->takeItem(rownum);
    m_flist->insertItem(qMax(0,rownum-1),todelete[0]);
    m_flist->setCurrentItem(todelete[0]);
  }
}

void PathTool::down() {
  QList<QListWidgetItem*> todelete(m_flist->selectedItems());
  if (todelete.size() > 0) {
    int rownum = m_flist->row(todelete[0]);
    m_flist->takeItem(rownum);
    m_flist->insertItem(qMin(m_flist->count(),rownum+1),todelete[0]);
    m_flist->setCurrentItem(todelete[0]);
  }
}

void PathTool::save() {
  QSettings settings("FreeMat","FreeMat");
  QStringList pathList;
  for (int i=0;i<m_flist->count();i++) {
    QListWidgetItem *item = m_flist->item(i);
    pathList << item->text();
  }
  settings.setValue("interpreter/path",pathList);
}

void PathTool::done() {
  writeSettings();
  close();
}

PathTool::~PathTool() {
}
