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
#include "Command.hpp"
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

  ArrayVector ListFilesFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    Command *cp;
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
    cp = new Command(CMD_SystemCapture,
		     Array::stringConstructor(buffer));
    SendGUICommand(cp);
    cp = GetGUIResponse();
    Array qp = cp->data;
    Array *dp = (Array*) (qp.getDataPointer());
    int maxlen = 0;
    // Find the maximal length
    for (int i=0;i<qp.getLength();i++) {
      Array el(dp[i]);
      int ellen(el.getLength());
      maxlen = (maxlen < ellen) ? ellen : maxlen;
    }
    // Calculate the number of columns that fit..
    int outcolumns;
    Interface *io;
    io = eval->getInterface();
    outcolumns = io->getTerminalWidth()/maxlen;
    if (outcolumns < 1) outcolumns = 1;
    int colwidth = io->getTerminalWidth()/outcolumns + 1;
    int entryCount = 0;
    while (entryCount < qp.getLength()) {
      char buffer[4096];
      char *wp;
      wp = dp[entryCount].getContentsAsCString();
      sprintf(buffer,"%s",wp);
      int wlen;
      wlen = strlen(wp);
      for (int j=wlen;j<colwidth;j++)
	buffer[j] = ' ';
      buffer[colwidth] = 0;
      io->outputMessage(buffer);
      entryCount++;
      if (entryCount % outcolumns == 0)
	io->outputMessage("\n");
    }
    io->outputMessage("\n");
    //    eval->getInterface()->outputMessage(cp->data.getContentsAsCString());
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
