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

#include "Array.hpp"
#include "WalkTree.hpp"
#include "Utils.hpp"
#include <unistd.h>
#include <stdio.h>

namespace FreeMat {
  ArrayVector ChangeDirFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("cd function requires exactly one argument");
    char* cdir = arg[0].getContentsAsCString();
    if (chdir(cdir) != 0)
      throw Exception(std::string("Unable to change to specified directory:") + 
		      cdir);
    eval->getInterface()->rescanPath();
    return ArrayVector();
  }

  ArrayVector ListFilesFunction(int nargout, const ArrayVector& arg) {
    char buffer[4096];
    char *bp;

#ifdef WIN32
    sprintf(buffer,"dir ");
#else
    sprintf(buffer,"ls ");
#endif

    bp = buffer + strlen(buffer);
    for (int i=0;i<arg.size();i++) {
      char *target = arg[i].getContentsAsCString();
      sprintf(bp,"%s ",target);
      bp = buffer + strlen(buffer);
    }
    system(buffer);
    return ArrayVector();
  }

  ArrayVector PrintWorkingDirectoryFunction(int nargout, const ArrayVector& arg) {
    char buffer[1000];
    getcwd(buffer,sizeof(buffer));
    ArrayVector retval;
    retval.push_back(Array::stringConstructor(buffer));
    return retval;
  }
}
