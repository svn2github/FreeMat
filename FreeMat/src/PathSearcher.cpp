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

#include "PathSearcher.hpp"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include "Malloc.hpp"

#ifdef WIN32
#define DELIM "\\"
#else
#define DELIM "/"
#endif

namespace FreeMat {

  PathSearcher *globalPathSearcher;

  char buffer[1024];

  // First, we read the supplied the environment variable 
  PathSearcher::PathSearcher(char* env_var, Context* contxt) {
    char* pathdata = getenv(env_var);
    // Search through the path
    char *saveptr = (char*) Malloc(sizeof(char)*1024);
    char* token;
    token = strtok_r(pathdata,":",&saveptr);
    while (token != NULL) {
      if (strcmp(token,".") != 0)
	dirTab.push_back(std::string(token));
      token = strtok_r(NULL,":",&saveptr);
    }
    context = contxt;
    rescanPath();
  }

  void PathSearcher::rescanPath() {
    //  fileTab.empty();
    int i;
    for (i=0;i<dirTab.size();i++)
      scanDirectory(dirTab[i]);
    // Scan the current working directory.
    char cwd[1024];
    getcwd(cwd,1024);
    scanDirectory(std::string(cwd));
  }

  void PathSearcher::scanDirectory(std::string scdir) {
    // Open the directory
    DIR *dir;

    dir = opendir(scdir.c_str());
    if (dir == NULL) return;
    // Scan through the directory..
    struct dirent *fspec;
    char *fname;
    std::string fullname;
    while (fspec = readdir(dir)) {
      // Get the name of the entry
      fname = fspec->d_name;
      // Check for '.' and '..'
      if ((strcmp(fname,".") == 0) || (strcmp(fname,"..") == 0)) 
	continue;
      // Stat the file...
      fullname = std::string(scdir + std::string(DELIM) + fname);
      procFile(fname,fullname);
    }
    closedir(dir);
  }

  void PathSearcher::procFile(char *fname, 
			      std::string fullname) {
    struct stat filestat;
    char buffer[1024];
  
    stat(fullname.c_str(),&filestat);
    if (S_ISREG(filestat.st_mode)) {
      int namelen;
      namelen = strlen(fname);
      if (fname[namelen-2] == '.' && 
	  (fname[namelen-1] == 'm' ||
	   fname[namelen-1] == 'M')) {
	fname[namelen-2] = 0;
	// Look for the function in the context - only insert it
	// if it is not already defined.
	FunctionDef *fdef;
	if (!context->lookupFunctionGlobally(std::string(fname),fdef)) {
	  MFunctionDef *adef;
	  adef = new MFunctionDef();
	  adef->name = std::string(fname);
	  adef->fileName = fullname;
	  context->insertFunctionGlobally(adef);
	}
      }
    } else if (S_ISDIR(filestat.st_mode))
      scanDirectory(fullname);
    else if (S_ISLNK(filestat.st_mode)) {
      int lncnt = readlink(fullname.c_str(),buffer,1024);
      buffer[lncnt] = 0;
      procFile(fname, std::string(buffer));
    }
  }

}
