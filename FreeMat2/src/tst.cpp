#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <iostream>

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
    ndx = path.find(":");
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

int main(int argc, char *argv[]) {
  std::string p(GetApplicationPath(argv[0]));
  
  printf("Path is %s\n",p.c_str());
}
