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
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#include <direct.h>
#define getcwd _getcwd
#define S_ISREG(x) (x & _S_IFREG)
#define P_DELIM ";"
#else
#define P_DELIM ":"
#endif

namespace FreeMat {
  bool FileExists(std::string filename) {
    struct stat filestat;
    if (stat(filename.c_str(),&filestat)==-1) return false;
    return (S_ISREG(filestat.st_mode));
  }

  std::string GetFilenameOnly(std::string a) {
    // Strip off application name
    int ndx;
    ndx = a.rfind("/");
    if (ndx>=0)
      a.erase(0,ndx);
    return a;
  }

  std::string GetPathOnly(std::string a) {
    // Strip off application name
     int ndx;
    ndx = a.rfind("/");
    a.erase(ndx+1,a.size());
    return a;
  }

  std::string CheckEndSlash(std::string a) {
    if (a[a.size()-1] != '/')
      a.append("/");
    return a;
  }

std::string GetApplicationPath(char *argv0) {
  std::string retpath;
  // Case 1 - absolute path
  if (argv0[0] == '/') 
    return GetPathOnly(std::string(argv0));
  // Case 2 - relative path
  char buffer[4096];
  getcwd(buffer,sizeof(buffer));
  retpath = CheckEndSlash(std::string(buffer)) + std::string(argv0);
  // This file should exist
  if (FileExists(retpath))
    return GetPathOnly(std::string(retpath));
  // Case 3 - file in "PATH" variable
  std::string path(getenv("PATH"));
  bool found = false;
  std::string tpath;
  while (!found && !path.empty()) {
    int ndx;
    ndx = path.find(P_DELIM);
    tpath = path.substr(0,ndx);
    found = FileExists(CheckEndSlash(tpath) + std::string(argv0));
    path.erase(0,ndx+1);
  }
  if (found) {
    return tpath;
  } else {
    fprintf(stderr,"Error: unable to determine application path - support files unavailable!\n\r");
    return std::string();
  }
}

  PathSearcher::PathSearcher(std::string mpath) {
    path = mpath + P_DELIM;
    std::string tpath;
    while (!path.empty()) {
      int ndx;
      ndx = path.find(P_DELIM);
      tpath = path.substr(0,ndx);
      path.erase(0,ndx+1);
      pathList.push_back(tpath);
      fflush(stdout);
    }
  }

  std::string PathSearcher::ResolvePath(std::string fname) {
    int ndx = 0;
    bool found = FileExists(fname);
    if (found)  
      return fname;
    std::string tpath;
    std::string fullname;
    while (!found && (ndx < pathList.size())) {
      tpath = pathList[ndx++];
      fullname = CheckEndSlash(tpath) + fname;
      found = FileExists(fullname);
    }
    if (found)
      return(fullname);
    else {
      throw Exception("Unable to find file " + fname + " on the current path!");
    }
  }
}
