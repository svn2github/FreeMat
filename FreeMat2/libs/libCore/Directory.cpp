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
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>
#include "System.hpp"

#ifdef WIN32
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#endif

namespace FreeMat {

  //!
  //@Module CD Change Working Directory Function
  //@@Usage
  //Changes the current working directory to the one specified as the argument.  The general syntax for its use is
  //@[
  //  cd('dirname')
  //@]
  //but this can also be expressed as
  //@[
  //  cd 'dirname'
  //@]
  //or 
  //@[
  //  cd dirname
  //@]
  //Examples of all three usages are given below.
  //Generally speaking, @|dirname| is any string that would be accepted by the underlying OS as a valid directory name.  For example, on most systems, @|'.'| refers to the current directory, and @|'..'| refers to the parent directory.  Also, depending on the OS, it may be necessary to ``escape'' the directory seperators.  In particular, if directories are seperated with the backwards-slash character @|'\\'|, then the path specification must use double-slashes @|'\\\\'|. Note: to get file-name completion to work at this time, you must use one of the first two forms of the command.
  //@@Example
  //The @|pwd| command returns the current directory location.  First, we use the simplest form of the @|cd| command, in which the directory name argument is given unquoted.
  //@<
  //pwd
  //cd ..
  //pwd
  //@>
  //Next, we use the ``traditional'' form of the function call, using both the parenthesis and a variable to store the quoted string.
  //@<
  //a = pwd;
  //cd(a)
  //pwd
  //@>
  //In the third version, we use only the quoted string argument without parenthesis.
  //@<
  //cd '..'
  //pwd
  //@>
  //!
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

  //!
  //@Module LS/DIR List Files Function
  //@@Usage
  //Lists the files in a directory or directories.  The general syntax for its use is
  //@[
  //  ls('dirname1','dirname2',...,'dirnameN')
  //@]
  //but this can also be expressed as
  //@[
  //  ls 'dirname1' 'dirname2' ... 'dirnameN'
  //@]
  //or 
  //@[
  //  ls dirname1 dirname2 ... dirnameN
  //@]
  //For compatibility with some environments, the function @|dir| can also be used instead of @|ls|.  Generally speaking, @|dirname| is any string that would be accepted by the underlying OS as a valid directory name.  For example, on most systems, @|'.'| refers to the current directory, and @|'..'| refers to the parent directory.  Also, depending on the OS, it may be necessary to ``escape'' the directory seperators.  In particular, if directories are seperated with the backwards-slash character @|'\\'|, then the path specification must use double-slashes @|'\\\\'|. Two points worth mentioning about the @|ls| function:
  //\begin{itemize}
  //  \item To get file-name completion to work at this time, you must use one of the first two forms of the command.
  //  \item If you want to capture the output of the @|ls| command, use the @|system| function instead.
  //\end{itemize}
  //
  //@@Example
  //First, we use the simplest form of the @|ls| command, in which the directory name argument is given unquoted.
  //@<
  //ls m*.m
  //@>
  //Next, we use the ``traditional'' form of the function call, using both the parenthesis and the quoted string.
  //@<
  //ls('m*.m')
  //@>
  //In the third version, we use only the quoted string argument without parenthesis.  
  //@<
  //ls 'm*.m'
  //@>
  //!
  ArrayVector ListFilesFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    stringVector sysresult;
    char buffer[4096];
    char *bp;
	int i;

#ifdef WIN32
    sprintf(buffer,"dir ");
    bp = buffer + strlen(buffer);
    for (i=0;i<arg.size();i++) {
      char *target = arg[i].getContentsAsCString();
      sprintf(bp,"%s ",target);
      bp = buffer + strlen(buffer);
    }
    sysresult = DoSystemCallCaptured(buffer);
	Interface *io;
	io = eval->getInterface();
	for (i=0;i<sysresult.size();i++) {
		io->outputMessage(sysresult[i].c_str());
		io->outputMessage("\n");
	}
#else
    sprintf(buffer,"ls ");
    bp = buffer + strlen(buffer);
    for (i=0;i<arg.size();i++) {
      char *target = arg[i].getContentsAsCString();
      sprintf(bp,"%s ",target);
      bp = buffer + strlen(buffer);
    }
    sysresult = DoSystemCallCaptured(buffer);
    int maxlen = 0;
    // Find the maximal length
    for (i=0;i<sysresult.size();i++) {
      int ellen(sysresult[i].size());
      maxlen = (maxlen < ellen) ? ellen : maxlen;
    }
    // Calculate the number of columns that fit..
    int outcolumns;
    Interface *io;
    io = eval->getInterface();
    int termwidth = io->getTerminalWidth();
    outcolumns = termwidth/(maxlen+1);
    if (outcolumns < 1) outcolumns = 1;
    int colwidth = termwidth/outcolumns;
    int entryCount = 0;
    while (entryCount < sysresult.size()) {
      char buffer[4096];
      sprintf(buffer,"%s",sysresult[entryCount].c_str());
      int wlen;
      wlen = strlen(buffer);
      for (int j=wlen;j<colwidth;j++)
	buffer[j] = ' ';
      buffer[colwidth] = 0;
      io->outputMessage(buffer);
      entryCount++;
      if (entryCount % outcolumns == 0)
	io->outputMessage("\n");
    }
    io->outputMessage("\n");
#endif
    return ArrayVector();
  }

  //!
  //@Module PWD Print Working Directory Function
  //@@Usage
  //Returns a @|string| describing the current working directory.  The general syntax for its use is
  //@[
  //  y = pwd
  //@]
  //
  //@@Example
  //The @|pwd| function is fairly straightforward.
  //@<
  //pwd
  //@>
  //!
  ArrayVector PrintWorkingDirectoryFunction(int nargout, const ArrayVector& arg) {
    char buffer[1000];
    getcwd(buffer,sizeof(buffer));
    ArrayVector retval;
    retval.push_back(Array::stringConstructor(buffer));
    return retval;
  }

  //!
  //@Module GETPATH Get Current Search Path
  //@@Usage
  //Returns a @|string| containing the current FreeMat search path.  The general syntax for
  //its use is
  //@[
  //  y = getpath
  //@]
  //The delimiter between the paths depends on the system being used.  For Win32, the
  //delimiter is a semicolon.  For all other systems, the delimiter is a colon.
  //
  //@Example
  //The @|getpath| function is straightforward.
  //@<
  //getpath
  //@>
  //!
  ArrayVector GetPathFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    Interface *io;
    io = eval->getInterface();
    ArrayVector retval;
    retval.push_back(Array::stringConstructor(io->getPath()));
    return retval;
  }

  //!
  //@Module SETPATH Set Current Search Path
  //@@Usage
  //Changes the current FreeMat search path.  The general syntax for
  //its use is
  //@[
  //  setpath(y)
  //@]
  //where @|y| is a @|string| containing a delimited list of directories
  //to be searched for M files and libraries.  
  //The delimiter between the paths depends on the system being used.  For Win32, the
  //delimiter is a semicolon.  For all other systems, the delimiter is a colon.
  //
  //@Example
  //The @|setpath| function is straightforward.
  //@<
  //getpath
  //setpath('/usr/local/FreeMat/MFiles:/localhome/basu/MFiles')
  //getpath
  //@>
  //!
  ArrayVector SetPathFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("setpath function requires exactly one string argument");
    char *cdir = arg[0].getContentsAsCString();
    Interface *io;
    io = eval->getInterface();
    io->setPath(cdir);
    return ArrayVector();
  }

}
