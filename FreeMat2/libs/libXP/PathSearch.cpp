// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

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
    stat(filename.c_str(),&filestat);
    return (S_ISREG(filestat.st_mode));
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
    bool found = false;
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
