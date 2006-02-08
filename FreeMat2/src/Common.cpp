#include "Common.hpp"
#include <QtCore>

QStringList GetRecursiveDirList(QString basedir) {
  QStringList ret;
  QFileInfo fi(basedir);
  if ((fi.baseName().left(1) == QString("@")) ||
      (fi.baseName().toLower() == QString("private")))
    return ret;
  ret << basedir;
  QDir dir(basedir);
  dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    ret += GetRecursiveDirList(fileInfo.absoluteFilePath());
  }
  return ret;
}
