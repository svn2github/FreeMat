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

#include "Array.hpp"
#include "Interpreter.hpp"
#include "Utils.hpp"
#include <stdio.h>
#include "System.hpp"
#include <QtCore>

//!
//@Module CD Change Working Directory Function
//@@Section OS
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
//Generally speaking, @|dirname| is any string that would be accepted 
//by the underlying OS as a valid directory name.  For example, on most 
//systems, @|'.'| refers to the current directory, and @|'..'| refers 
//to the parent directory.  Also, depending on the OS, it may be necessary 
//to ``escape'' the directory seperators.  In particular, if directories 
//are seperated with the backwards-slash character @|'\\'|, then the 
//path specification must use double-slashes @|'\\\\'|. Note: to get 
//file-name completion to work at this time, you must use one of the 
//first two forms of the command.
//
//@@Example
//The @|pwd| command returns the current directory location.  First, 
//we use the simplest form of the @|cd| command, in which the directory 
//name argument is given unquoted.
//@<
//pwd
//cd ..
//pwd
//@>
//Next, we use the ``traditional'' form of the function call, using 
//both the parenthesis and a variable to store the quoted string.
//@<
//a = pwd;
//cd(a)
//pwd
//@>
//!
ArrayVector ChangeDirFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("cd function requires exactly one argument");
  if (!QDir::setCurrent(TildeExpand(ArrayToString(arg[0]))))
    throw Exception(std::string("Unable to change to specified directory:") + 
		    ArrayToString(arg[0]));
  eval->rescanPath();
  return ArrayVector();
}

//!
//@Module DIR List Files Function
//@@Section OS
//@@Usage
//An alias for the @|ls| function.  The general syntax for its use is
//@[
//  dir('dirname1','dirname2',...,'dirnameN')
//@]
//but this can also be expressed as
//@[
//  dir 'dirname1' 'dirname2' ... 'dirnameN'
//@]
//or 
//@[
//  dir dirname1 dirname2 ... dirnameN
//@]
//For compatibility with some environments, the function @|ls| can also be used 
//instead of @|dir|.  Generally speaking, @|dirname| is any string that would be 
//accepted by the underlying OS as a valid directory name.  For example, on 
//most systems, @|'.'| refers to the current directory, and @|'..'| refers to 
//the parent directory.  Two points worth mentioning about the @|dir| function:
//\begin{itemize}
//  \item To get file-name completion to work at this time, you must use 
//one of the first two forms of the command.
//  \item If you want to capture the output of the @|ls| command, use 
//the @|system| function instead.
//\end{itemize}
//For examples, see the @|ls| function.
//!

//!
//@Module LS List Files Function
//@@Section OS
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
ArrayVector ListFilesFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  stringVector sysresult;
  char buffer[4096];
  char *bp;
  int i;

#ifdef WIN32
  sprintf(buffer,"dir ");
  bp = buffer + strlen(buffer);
  for (i=0;i<arg.size();i++) {
    string target = arg[i].getContentsAsString();
    sprintf(bp,"%s ",target.c_str());
    bp = buffer + strlen(buffer);
  }
  sysresult = DoSystemCallCaptured(buffer);
  for (i=0;i<sysresult.size();i++) {
    eval->outputMessage(sysresult[i].c_str());
    eval->outputMessage("\n");
  }
#else
#warning Needs work
  sprintf(buffer,"ls ");
  bp = buffer + strlen(buffer);
  for (i=0;i<arg.size();i++) {
    QString fipath(TildeExpand(ArrayToString(arg[i])));
    const char *target = fipath.toStdString().c_str();
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
  int termwidth = eval->getTerminalWidth()-1;
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
    eval->outputMessage(buffer);
    entryCount++;
    if (entryCount % outcolumns == 0)
      eval->outputMessage("\n");
  }
  eval->outputMessage("\n");
#endif
  return ArrayVector();
}

//!
//@Module DIRSEP Director Seperator
//@@Section OS
//@@Usage
//Returns the directory seperator character for the current platform.  The 
//general syntax for its use is
//@[
//   y = dirsep
//@]
//This function can be used to build up paths (or see @|fullfile| for another
//way to do this.
//!
ArrayVector DirSepFunction(int nargout, const ArrayVector& arg) {
  QString psep(QDir::separator());
  return singleArrayVector(Array::stringConstructor(psep.toStdString()));
}

//!
//@Module PWD Print Working Directory Function
//@@Section OS
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
  return ArrayVector() << Array::stringConstructor(QDir::currentPath().toStdString());
}

//!
//@Module GETPATH Get Current Search Path
//@@Section OS
//@@Usage
//Returns a @|string| containing the current FreeMat search path.  The general syntax for
//its use is
//@[
//  y = getpath
//@]
//The delimiter between the paths depends on the system being used.  For Win32, the
//delimiter is a semicolon.  For all other systems, the delimiter is a colon.
//
//@@Example
//The @|getpath| function is straightforward.
//@<
//getpath
//@>
//!
ArrayVector GetPathFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  ArrayVector retval;
  retval.push_back(Array::stringConstructor(eval->getPath()));
  return retval;
}

//!
//@Module SETPATH Set Current Search Path
//@@Section OS
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
ArrayVector SetPathFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("setpath function requires exactly one string argument");
  eval->setPath(arg[0].getContentsAsString());
  return ArrayVector();
}

//!
//@Module RMDIR Remove Directory
//@@Section OS
//@@Usage
//Deletes a directory.  The general syntax for its use is
//@[
//  rmdir('dirname')
//@]
//which removes the directory @|dirname| if it is empty.  If you
//want to delete the directory and all subdirectories and files
//in it, use the syntax
//@[
//  rmdir('dirname','s')
//@]
//!
void RemoveDirectory(string dirname) {
  if (!QDir::current().rmdir(QString::fromStdString(dirname)))
    throw Exception(string("unable to delete directory ") + dirname);
}

void RemoveDirectoryRecursive(string dirname) {
  QDir dir(QString::fromStdString(dirname));
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      RemoveDirectoryRecursive(fileInfo.absoluteFilePath().toStdString());
    else
      dir.remove(fileInfo.absoluteFilePath());
  }
  if (!QDir::current().rmdir(QString::fromStdString(dirname)))
    throw Exception(string("unable to delete directory ") + dirname);
}

ArrayVector RMDirFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("rmdir requires at least one argument (the directory to remove)");
  if (arg.size() == 1)
    RemoveDirectory(ArrayToString(arg[0]));
  else if (arg.size() == 2) {
    string arg1 = ArrayToString(arg[1]);
    if ((arg1 == "s") || (arg1 == "S"))
      RemoveDirectoryRecursive(ArrayToString(arg[0]));
    else
      throw Exception("rmdir second argment must be a 's' to do a recursive delete");
  }
  return ArrayVector();
}

// See mkdir.m for documentation
ArrayVector MKDirCoreFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("mkdir requires at least one argument (the directory to create)");
  if (QDir::current().mkpath(QString::fromStdString(ArrayToString(arg[0]))))
    return ArrayVector() << Array::logicalConstructor(1);
  else
    return ArrayVector() << Array::logicalConstructor(0);
}

//!
//@Module FILEPARTS Extract Filename Parts
//@@Section OS
//@@Usage
//The @|fileparts| takes a filename, and returns the path, filename, extension, and
//(for MATLAB-compatibility) an empty version number of the file.  The syntax for its use is
//@[
//    [path,name,extension,version] = fileparts(filename)
//@]
//where @|filename| is a string containing the description of the file, and @|path|
//is the @|path| to the file, 
//!
ArrayVector FilePartsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("fileparts requires a filename");
  QFileInfo fi(QString::fromStdString(ArrayToString(arg[0])));
  return ArrayVector() << Array::stringConstructor(fi.path().toStdString()) 
		       << Array::stringConstructor(fi.completeBaseName().toStdString())
		       << Array::stringConstructor("." + fi.suffix().toStdString())
		       << Array::stringConstructor("");
}
