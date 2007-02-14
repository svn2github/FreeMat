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

#include "PathSearch.hpp"
#include "Exception.hpp"
#include <QtCore>

#ifdef WIN32
#define PATHSEP ";"
#else
#define PATHSEP ":"
#endif

PathSearcher::PathSearcher(std::string mpath) {
  path = QString::fromStdString(mpath);
  pathList = path.split(PATHSEP,QString::SkipEmptyParts);
}

std::string PathSearcher::ResolvePath(std::string fname) {
  QString qfname(QString::fromStdString(fname));
  if (QDir::current().exists(qfname));
    return fname;
  for (int i=0;i<pathList.size();i++) {
    QDir pdir(pathList[i]);
    if (pdir.exists(qfname))
      return pdir.absoluteFilePath(qfname).toStdString();
  }
  throw Exception("Unable to find file " + fname + " on the current path!");
}

