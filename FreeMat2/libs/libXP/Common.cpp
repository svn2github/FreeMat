/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "Common.hpp"
#include <QtCore>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#endif
using namespace std;

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

stringVector GetCompletionList(string pattern) {
  stringVector completions;
#ifdef WIN32
  HANDLE hSearch;
  WIN32_FIND_DATA FileData;
  pattern.append("*");
  hSearch = FindFirstFile(pattern.c_str(),&FileData);
  if (hSearch != INVALID_HANDLE_VALUE) {
    // Windows does not return any part of the path in the completion,
    // So we need to find the base part of the pattern.
    int lastslash;
    std::string prefix;
    lastslash = pattern.find_last_of("/");
    if (lastslash == -1) {
      lastslash = pattern.find_last_of("\\");
    }
    if (lastslash != -1)
      prefix = pattern.substr(0,lastslash+1);
    completions.push_back(prefix + FileData.cFileName);
    while (FindNextFile(hSearch, &FileData))
      completions.push_back(prefix + FileData.cFileName);
  }
  FindClose(hSearch);
  return completions;
#else
  glob_t names;
  std::string pattern(tmp);
  pattern.append("*");
  glob(pattern.c_str(), GLOB_MARK, NULL, &names);
  int i;
  for (i=0;i<names.gl_pathc;i++) 
    completions.push_back(names.gl_pathv[i]);
  globfree(&names);
  free(tmp);
  return completions;
#endif
}
