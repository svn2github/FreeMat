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

#include "Interpreter.hpp"
#include <math.h>
#include <stdio.h>
#include "Exception.hpp"
#include "Math.hpp"
#include "Array.hpp"
#include "Struct.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "Module.hpp"
#include "File.hpp"
#include <signal.h>
#include "Class.hpp"
#include "Print.hpp"
#include "MemPtr.hpp"
#include <qeventloop.h>
#include <QtCore>
#include <fstream>
#include <stdarg.h>
#include "JITFactory.hpp"
#include "JITInfo.hpp"
#include "IEEEFP.hpp"
#include "Algorithms.hpp"
#include "GetSet.hpp"
#include "FuncPtr.hpp"
#include "AnonFunc.hpp"
#include "Stats.hpp"
#include <QtGui>
#include "DebugStream.hpp"
#include "CArray.hpp"

#ifdef WIN32
#define PATHSEP ";"
#else
#define PATHSEP ":"
#endif

const int max_line_count = 1000000;

/**
 * The database of compiled code segments
 */
QMap<int,JITInfo> m_codesegments;

/**
 * The file system watcher -- watches for changes to the file system
 * Only one interpreter thread should use this watcher at a time.
 */
//QFileSystemWatcher m_watch;


#define SaveEndInfo  \
  ArrayReference oldEndRef = endRef; \
  int oldEndCount = endCount; \
  int oldEndTotal = endTotal; 

#define RestoreEndInfo \
  endRef = oldEndRef; \
  endCount = oldEndCount; \
  endTotal = oldEndTotal;  \

QString TildeExpand(QString path) {
  if ((path.size() > 0) && (path[0] == '~')) {
    path.remove(0,1);
    return QDir::homePath() + path;
  }
  return path;
}

class CLIDisabler {
  Interpreter *p;
  bool CLIFlagSave;
public:
  CLIDisabler(Interpreter *q) : p(q) {
    CLIFlagSave = p->inCLI();
    p->setInCLI(false);
  }
  ~CLIDisabler() {
    p->setInCLI(CLIFlagSave);
  }
};

void Interpreter::setPath(QString path) {
  if (path == m_userCachePath) return;
  QStringList pathset(path.split(PATHSEP,QString::SkipEmptyParts));
  m_userPath.clear();
  for (int i=0;i<pathset.size();i++) 
    if (pathset[i] != ".") {
      QDir tpath(TildeExpand(pathset[i]));
      m_userPath << tpath.absolutePath();
    }
  setupWatcher();
  //  rescanPath();
  updateFileTool();
  m_userCachePath = path;
}

QString Interpreter::getTotalPath() {
  QString retpath;
  QStringList totalPath(QStringList() << m_basePath << m_userPath);
  for (int i=0;i<totalPath.size()-1;i++) 
    retpath = retpath + totalPath[i] + PATHSEP;
  if (totalPath.size() > 0) 
    retpath = retpath + totalPath[totalPath.size()-1];
  return retpath;
}
  
QString Interpreter::getPath() {
  QString retpath;
  QStringList totalPath(m_userPath);
  for (int i=0;i<totalPath.size()-1;i++) 
    retpath = retpath + totalPath[i] + PATHSEP;
  if (totalPath.size() > 0) 
    retpath = retpath + totalPath[totalPath.size()-1];
  return retpath;
}
  
void Interpreter::setLiveUpdateFlag(bool t) {
  m_liveUpdateFlag = t;
  if (t) {
    //    connect(&m_watch,SIGNAL(directoryChanged(const QString &)),
    // 	    this,SLOT(updateFileTool(const QString &)));
  }
}

// static bool DirExists(const QString & path) {
//   QDir tmp(path);
//   return tmp.exists();
// }

void Interpreter::setupWatcher() {
  if (!m_liveUpdateFlag) return;
  //  QStringList pathLists(m_watch.directories());
  //  if (!pathLists.isEmpty())
  //    m_watch.removePaths(pathLists);
  //   if (!m_userPath.isEmpty()) {
  //     for (int i=0;i<m_userPath.size();i++)
  //       if (DirExists(m_userPath[i]))
  // 	m_watch.addPath(m_userPath[i]);
  //   }
  //   if (!m_basePath.isEmpty()) {
  //     for (int i=0;i<m_basePath.size();i++)
  //       if (DirExists(m_basePath[i]))
  // 	m_watch.addPath(m_basePath[i]);
  //   }
  //   m_watch.addPath(QDir::currentPath());
}

void Interpreter::changeDir(QString path) {
  if (QDir(path) == QDir::currentPath())
    return;
  if (!QDir::setCurrent(path))
    throw Exception("Unable to change to specified directory: " + path);
  if (m_liveUpdateFlag)
    emit CWDChanged(QDir::currentPath());
  setupWatcher();
  rescanPath();
  //  updateFileTool();
}
				 
void Interpreter::updateVariablesTool() {
  if (!m_liveUpdateFlag) return;
  StringVector varList(context->listAllVariables());
  QList<QVariant> vars;
  for (int i=0;i<varList.size();i++) {
    QList<QVariant> entry;
    // Icon
    entry << QVariant();
    entry << QVariant(varList[i]);
    Array *dp = context->lookupVariableLocally(varList[i]);
    if (dp) {
      // class
      if (dp->allReal())
	entry << QVariant(dp->className());
      else
	entry << QVariant(dp->className() + " (complex)");
      // value
      entry << QVariant(SummarizeArrayCellEntry(*dp));
      // size
      entry << QVariant(dp->dimensions().toString());
      // bytes min, max, range, mean, var, std
      entry << QVariant(double(dp->bytes()));
      entry += ComputeVariableStats(dp);
    } 
    for (int i=entry.size();i<10;i++)
      entry << QVariant();
    vars << QVariant(entry); 
  }
  emit updateVarView(QVariant(vars));
}

static bool InSpecificScope(Context *context, QString name, QString detail) {
  return ((context->scopeName() == name) &&
	  (context->scopeDetailString() == detail));
}

static bool InKeyboardScope(Context *context) {
  return InSpecificScope(context,"keyboard","keyboard");
}

static QString GetStackToolDescription(Context *context) {
  int line = int(LineNumber(context->scopeTokenID()));
  if (line > 0)
    return QString(context->scopeDetailString() + QString("(%1)").arg(line));
  else
    return context->scopeDetailString();
}

void Interpreter::updateStackTool() {
  QStringList stackInfo;
  // Do a complete dump...
  // Suppose we start with 
  int f_depth = context->scopeDepth();
  context->restoreBypassedScopes();
  int t_depth = context->scopeDepth();
  for (int i=f_depth;i<t_depth;i++) {
    if (!InKeyboardScope(context) && !context->scopeDetailString().isEmpty())
      stackInfo << GetStackToolDescription(context);
    context->bypassScope(1);
  }
  bool firstline = true;
  for (int i=0;i<f_depth;i++) {
    if (!InKeyboardScope(context) && 
	!InSpecificScope(context,"docli","builtin") &&
	!context->scopeDetailString().isEmpty()) {
      if (firstline) {
	stackInfo << QString("*") + GetStackToolDescription(context);
	firstline = false;
      } else 
	stackInfo << GetStackToolDescription(context);
    }
    context->bypassScope(1);
  }
  context->restoreBypassedScopes();
  while (context->scopeDepth() > f_depth) context->bypassScope(1);
  emit updateStackView(stackInfo);
}

void Interpreter::updateFileTool(const QString &) {
  updateFileTool();
}

void Interpreter::updateFileTool() {
  // Build the info to send to the file tool
  QDir dir(QDir::currentPath());
  dir.setFilter(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
  QFileInfoList list(dir.entryInfoList());
  QList<QVariant> files;
  QList<QVariant> entry;
  entry << QVariant(QString("dir"));
  entry << QVariant(" .. (Parent Folder)");
  entry << QVariant();
  entry << QVariant();
  entry << QVariant("Folder");
  files << QVariant(entry);
  for (int i=0;i<((int)list.size());i++) {
    QList<QVariant> entry;
    QFileInfo fileInfo(list.at(i));
    if (fileInfo.isDir())
      entry << QVariant(QString("dir"));
    else
      entry << QVariant(QString("file"));
    entry << QVariant(fileInfo.fileName());
    entry << QVariant(fileInfo.size());
    entry << QVariant(fileInfo.lastModified());
    if (fileInfo.isDir())
      entry << QVariant(QString("Folder"));
    else if (fileInfo.suffix().isEmpty())
      entry << QVariant("File");
    else
      entry << QVariant(QString(fileInfo.suffix() + " File"));
    files << QVariant(entry);
  }
  emit updateDirView(QVariant(files));
}

void Interpreter::rescanPath() {
  if (m_disablerescan) return;
  if (!context) return;
  context->flushTemporaryGlobalFunctions();
  for (int i=0;i<m_basePath.size();i++)
    scanDirectory(m_basePath[i],false,"");
  for (int i=0;i<m_userPath.size();i++)
    scanDirectory(m_userPath[i],false,"");
  // Scan the current working directory.
  scanDirectory(QDir::currentPath(),true,"");
  updateFileTool();
}
  

void Interpreter::setBasePath(QStringList pth) {
  m_basePath = pth;
}

void Interpreter::setUserPath(QStringList pth) {
  m_userPath = pth;
}
  
static QString mexExtension() {
#ifdef Q_OS_LINUX
  return "fmxglx";
#endif
#ifdef Q_OS_MACX
  return "fmxmac";
#endif
#ifdef Q_OS_WIN32
  return "fmxw32";
#endif
  return "fmx";
}
  
void Interpreter::scanDirectory(QString scdir, bool tempfunc,
				QString prefix) {
  QDir dir(scdir);
  dir.setFilter(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
  dir.setNameFilters(QStringList() << "*.m" << "*.p" 
		     << "@*" << "private" << "*."+mexExtension());
  QFileInfoList list(dir.entryInfoList());
  for (int i=0;i<((int)list.size());i++) {
    QFileInfo fileInfo(list.at(i));
    QString fileSuffix(fileInfo.suffix());
    QString fileBaseName(fileInfo.baseName());
    QString fileAbsoluteFilePath(fileInfo.absoluteFilePath());
    if (fileSuffix == "m" || fileSuffix == "M") 
      if (prefix.isEmpty())
	procFileM(fileBaseName,fileAbsoluteFilePath,tempfunc);
      else
	procFileM(prefix + ":" + fileBaseName,fileAbsoluteFilePath,tempfunc);
    else if (fileSuffix == "p" || fileSuffix == "P")
      if (prefix.isEmpty())
	procFileP(fileBaseName,fileAbsoluteFilePath,tempfunc);
      else
	procFileP(prefix + ":" + fileBaseName,fileAbsoluteFilePath,tempfunc);
    else if (fileBaseName[0] == '@')
      scanDirectory(fileAbsoluteFilePath,tempfunc,fileBaseName);
    else if (fileBaseName == "private") 
      scanDirectory(fileAbsoluteFilePath,tempfunc,fileAbsoluteFilePath);
    else
      procFileMex(fileBaseName,fileAbsoluteFilePath,tempfunc);
  }
}
  
void Interpreter::procFileM(QString fname, QString fullname, bool tempfunc) {
  MFunctionDef *adef;
  adef = new MFunctionDef();
  adef->name = fname;
  adef->fileName = fullname;
  adef->temporaryFlag = tempfunc;
  FuncPtr val;
  if (context->lookupFunction(fname,val)) 
    if (val->type() == FM_BUILT_IN_FUNCTION) 
      warningMessage("built in function " + fname + " will be shadowed by the script " + fullname);
  context->insertFunction(adef, tempfunc);
}
  
void Interpreter::procFileP(QString fname, QString fullname, bool tempfunc) {
  throw Exception("P-files are not supported in this version of FreeMat");
}

void Interpreter::procFileMex(QString fname, QString fullname, bool tempfunc) {
  MexFunctionDef *adef;
  adef = new MexFunctionDef(fullname);
  adef->name = fname;
  if (adef->LoadSuccessful())
    context->insertFunction((MFunctionDef*)adef,tempfunc);
  else
    delete adef;
}

void Interpreter::RegisterGfxResults(ArrayVector m) {
  mutex.lock();
  gfx_buffer.push_back(m);
  gfxBufferNotEmpty.wakeAll();
  mutex.unlock();
}

void Interpreter::RegisterGfxError(QString msg) {
  mutex.lock();
  gfxError = msg;
  gfxErrorOccured = true;
  gfxBufferNotEmpty.wakeAll();
  mutex.unlock();
}

ArrayVector Interpreter::doFunction(FuncPtr f, ArrayVector& m, 
				    int narg_out, VariableTable *vtable) {
  CLIDisabler dis(this);
  PopContext saver(context,0);
  context->pushScope(f->functionName(),f->detailedName(),false);
  if (f->graphicsFunction) {
    gfxErrorOccured = false;
    QMutexLocker lock(&mutex);
    emit doGraphicsCall(this,f,m,narg_out);
    if (!gfxErrorOccured && gfx_buffer.empty()) {
      gfxBufferNotEmpty.wait(&mutex);
    } else {
      dbout << "Wha??\n";
    }
    if (gfxErrorOccured) {
      throw Exception(gfxError);
    }
    if (gfx_buffer.empty())
      dbout << "Warning! graphics empty on return\n";
    ArrayVector ret(gfx_buffer.front());
    gfx_buffer.erase(gfx_buffer.begin());
    return ret;
  } else {
    ArrayVector ret(f->evaluateFunc(this,m,narg_out,vtable));
    if (context->scopeStepTrap() >= 1) {
      tracetrap = 1;
      tracecurrentline = 0;
      warningMessage("dbstep beyond end of function " + context->scopeDetailString() +
		     " -- setting single step mode\n");
      context->setScopeStepTrap(0);
    }
    return ret;
  }
}

void Interpreter::setTerminalWidth(int ncols) {
  mutex.lock();
  m_ncols = ncols;
  mutex.unlock();
}

int Interpreter::getTerminalWidth() {
  return m_ncols;
}

QString TranslateString(QString x) {
  return x.replace("\n","\r\n");
}

void Interpreter::diaryMessage(QString msg) {
  QFile file(m_diaryFilename);
  if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream os(&file);
    os << msg;
  }
}


void Interpreter::outputMessage(QString msg) {
  if (m_diaryState) diaryMessage(msg);
  if (m_captureState) 
    m_capture += msg;
  else
    if (m_quietlevel < 2)
      emit outputRawText(TranslateString(msg));
}

void Interpreter::outputMessage(const char* format,...) {
  char buffer[4096];
  va_list ap;
  va_start(ap,format);
  vsnprintf(buffer,4096,format,ap);
  va_end(ap);
  outputMessage(QString(buffer));
}

void Interpreter::errorMessage(QString msg) {
  if (m_diaryState) diaryMessage("Error: " + msg + "\n");
  if (m_captureState) 
    m_capture += "Error: " + msg + "\n";
  else
    if (m_quietlevel < 2)
      emit outputRawText(TranslateString("Error: " + msg + "\r\n"));
}

void Interpreter::warningMessage(QString msg) {
  static QString lastWarning;
  static bool lastWarningRepeat = false;
  if (!m_enableWarnings) return;
  if (m_diaryState) diaryMessage("Warning: " + msg + "\n");
  if (m_captureState) 
    m_capture += "Warning: " + msg + "\n";
  else
    if (m_quietlevel < 2) {
      if (lastWarning != msg) {
	emit outputRawText(TranslateString("Warning: " +msg + "\r\n"));
	lastWarningRepeat = false;
	lastWarning = msg;
      } else {
	if (!lastWarningRepeat) {
	  emit outputRawText(TranslateString("Warning: Last warning repeats... suppressing more of these\r\n"));
	  lastWarningRepeat = true;
	}
      }
    }
}

static bool isMFile(QString arg) {
  // Not completely right...
  return (((arg[arg.size()-1] == 'm') ||
	   (arg[arg.size()-1] == 'p')) && 
	  (arg[arg.size()-2] == '.'));
}

QString TrimFilename(QString arg) {
  int ndx = arg.lastIndexOf(QDir::separator());
  if (ndx>=0)
    arg.remove(0,ndx+1);
  return arg;
}

QString TrimExtension(QString arg) {
  if (arg.size() > 2 && arg[arg.size()-2] == '.')
    arg.remove(arg.size()-2,arg.size());
  return arg;
}

static QString PrivateMangleName(QString currentFunctionPath, QString fname) {
  if (currentFunctionPath.isEmpty()) return "";
  // First look to see if we are already a private function
  QString separator("/");
  int ndx1 = currentFunctionPath.lastIndexOf(separator + "private" + separator);
  if (ndx1>=0) {
    // The current function is already in a private directory
    // In that case, try to find a private function in the same directory
    currentFunctionPath.remove(ndx1+1,currentFunctionPath.size());
    return currentFunctionPath + "private:" + fname;
  }
  int ndx;
  ndx = currentFunctionPath.lastIndexOf(separator);
  if (ndx>=0)
    currentFunctionPath.remove(ndx+1,currentFunctionPath.size());
  return currentFunctionPath + "private:" + fname;
}

static QString LocalMangleName(QString currentFunctionPath, QString fname) {
  int ndx = currentFunctionPath.lastIndexOf("/");
  if (ndx >= 0)
    currentFunctionPath.remove(ndx,currentFunctionPath.size());
  QString tmp = currentFunctionPath + "/" + fname;
  return currentFunctionPath + "/" + fname;
}

static QString NestedMangleName(QString cfunc, QString fname) {
  return cfunc + "/" + fname;
}

QString Interpreter::getVersionString() {
  return QString("FreeMat v" VERSION);
}

// Run the thread function
void Interpreter::run() {
  if (m_threadFunc) {
    try {
      m_threadFuncRets = doFunction(m_threadFunc,m_threadFuncArgs,m_threadNargout);
    } catch (Exception &e) {
      m_threadErrorState = true;      
      lasterr = e.msg();
    } catch (InterpreterQuitException &e) {
      m_threadErrorState = true;      
      lasterr = "'quit' called in non-main thread";
    } catch (InterpreterKillException &e) {
      m_kill = false;
    } catch (InterpreterRetallException &e) {
    } catch (exception& e) {
      m_threadErrorState = true;      
      lasterr = "thread crashed!! - you have encountered a bug in FreeMat - please file bug report describing what happened";
    } catch (...) {
      m_threadErrorState = true;      
      lasterr = "thread crashed!! - you have encountered a bug in FreeMat - please file bug report describing what happened";
    }
  }
}

void Interpreter::doCLI() {
  //  rescanPath();
  emit CWDChanged(QDir::currentPath());
  updateFileTool();
  if (!m_skipflag)
    sendGreeting();
  try {
    while (1) {
      int scope_stackdepth = context->scopeDepth(); 
      try {
	evalCLI();
      } catch (InterpreterRetallException) {
      } catch (InterpreterReturnException &e) {
      }
      while (context->scopeDepth() > scope_stackdepth) context->popScope();
    }
  } catch (InterpreterQuitException &e) {
    emit QuitSignal();
  } catch (std::exception& e) {
    emit CrashedSignal();
  } catch (...) {
    emit CrashedSignal();
  }
}

void Interpreter::sendGreeting() {
  outputMessage(" " + getVersionString() + "\n");
  outputMessage(" Copyright (c) 2002-2008 by Samit Basu\n");
  outputMessage(" Licensed under the GNU Public License (GPL)\n");
  outputMessage(" Type <help license> to find out more\n");
  outputMessage("      <helpwin> for online help\n");
  outputMessage("      <pathtool> to set or change your path\n");
  outputMessage(" Use <dbauto on/off> to control stop-on-error behavior\n");
  outputMessage(" Use ctrl-b to stop execution of a function/script\n");
  outputMessage(" JIT is enabled by default - use jitcontrol to change it\n");
  outputMessage(" Use <rootpath gui> to set/change where the FreeMat toolbox is installed\n");
  outputMessage("");
}

bool Interpreter::inMFile() const {
  return (isMFile(context->activeScopeName()));
}

void Interpreter::debugDump() {
  int depth = context->scopeDepth();
  qDebug() << "******************************\n";
  for (int i=0;i<depth;i++) {
    if (context->isScopeActive())
      qDebug() << "In " << context->scopeName() << " (" << context->scopeDetailString() << ")*";
    else
      qDebug() << "In " << context->scopeName() << " (" << context->scopeDetailString() << ")";
    context->bypassScope(1);
  }
  context->restoreScope(depth);
}

//!
//@Module DBUP Move Up One Debug Level
//@@Section DEBUG
//@@Usage
//The @|dbup| function moves up one level in the debug
//hierarchy.  The syntax for the @|dbup| function is
//@[
// dbup
//@]
//!
void Interpreter::dbup() {
  // The stack should look like -- 
  // base, foo, keyboard, dbup
  // so to do a dbup, we have to save the top two of the
  // stack, move foo to the backup stack, and then restore
  //
  // Consider the following.  The stack looks like this:
  // main: base foo1 keyboard foo2 keyboard dbup
  // bypass: <empty>
  //
  // Now we do a dbup
  // main: base foo1 keyboard keyboard dbup
  // bypass foo2
  //
  // Suppose we 
  // We need the "keyboard" states on the stack because they
  // capture the context updates for the command line routines.
  // 
  if (InSpecificScope(context,"docli","builtin")) 
    return;
  context->reserveScope();
  while (InKeyboardScope(context))
    context->bypassScope(1);
  if (!InSpecificScope(context,"base","base") &&
      !InSpecificScope(context,"docli","builtin")) {
    // Bypass a single non-keyboard context
    context->bypassScope(1);
  }
  while (InKeyboardScope(context))
    context->bypassScope(1);
  context->unreserveScope();
}

//!
//@Module DBDown Move Down One Debug Level
//@@Section DEBUG
//@@Usage
//The @|dbdown| function moves up one level in the debug
//hierarchy.  The syntax for the @|dbdown| function is
//@[
// dbdown
//@]
//!
void Interpreter::dbdown() {
  // Save the one for the "dbdown" command
  // Save the one for the "keyboard" command that we are currently in
  context->reserveScope();
  // Restore until we get a non-"keyboard" scope
  context->restoreScope(1);
  while (InKeyboardScope(context))
    context->restoreScope(1);
  context->unreserveScope();
  dbdown_executed = true;
}

QString Interpreter::getLocalMangledName(QString fname) {
  QString ret;
  if (isMFile(context->activeScopeName()))
    ret = LocalMangleName(context->activeScopeDetailString(),fname);
  else
    ret = fname;
  return ret;
}

QString Interpreter::getPrivateMangledName(QString fname) {
  QString ret;
  if (isMFile(context->scopeName())) 
    ret = PrivateMangleName(context->scopeName(),fname);
  else {
    ret = QDir::currentPath() + 
      QString(QDir::separator()) + 
      QString("private:" + fname);
  }
  return ret; 
}

QString Interpreter::getMFileName() {
  if (isMFile(context->scopeName())) 
    return TrimFilename(TrimExtension(context->scopeName()));
  // TESTME
  //   for (int i=cstack.size()-1;i>=0;i--)
  //     if (isMFile(cstack[i].cname)) 
  //       return TrimFilename(TrimExtension(cstack[i].cname));
  return QString("");
}

// called by editor
QString Interpreter::getInstructionPointerFileName() {
  if (!InCLI) return QString("");
  ParentScopeLocker lock(context);
  QString filename(context->scopeName());
  if (isMFile(filename)) 
    return filename;
  return QString("");
}

Array Interpreter::DoBinaryOperator(const Tree & t, BinaryFunc fnc, 
				    QString funcname) {
  Array a(expression(t.first()));
  Array b(expression(t.second()));
  if (!(a.isUserClass() || b.isUserClass())) 
    return fnc(a,b);
  return ClassBinaryOperator(a,b,funcname,this);
}

Array Interpreter::DoUnaryOperator(const Tree & t, UnaryFunc fnc, 
				   QString funcname) {
  Array a(expression(t.first()));
  if (!a.isUserClass())
    return fnc(a);
  return ClassUnaryOperator(a,funcname,this);
}

void Interpreter::setPrintLimit(int lim) {
  printLimit = lim;
}
  
int Interpreter::getPrintLimit() {
  return(printLimit);
}
 
//!
//@Module MATRIX Matrix Definitions
//@@Section VARIABLES
//@@Usage
//The matrix is the basic datatype of FreeMat.  Matrices can be
//defined using the following syntax
//@[
//  A = [row_def1;row_def2;...,row_defN]
//@]
//where each row consists of one or more elements, seperated by
//commas
//@[
//  row_defi = element_i1,element_i2,...,element_iM
//@]
//Each element can either be a scalar value or another matrix,
//provided that the resulting matrix definition makes sense.
//In general this means that all of the elements belonging
//to a row have the same number of rows themselves, and that
//all of the row definitions have the same number of columns.
//Matrices are actually special cases of N-dimensional arrays
//where @|N<=2|.  Higher dimensional arrays cannot be constructed
//using the bracket notation described above.  The type of a
//matrix defined in this way (using the bracket notation) is
//determined by examining the types of the elements.  The resulting
//type is chosen so no information is lost on any of the elements
//(or equivalently, by choosing the highest order type from those
//present in the elements).
//@@Examples
//Here is an example of a matrix of @|int32| elements (note that
//untyped integer constants default to type @|int32|).
//@<
//A = [1,2;5,8]
//@>
//Now we define a new matrix by adding a column to the right of
//@|A|, and using float constants.
//@<
//B = [A,[3.2f;5.1f]]
//@>
//Next, we add extend @|B| by adding a row at the bottom.  Note
//how the use of an untyped floating point constant forces the
//result to be of type @|double|
//@<
//C = [B;5.2,1.0,0.0]
//@>
//If we instead add a row of @|complex| values (recall that @|i| is
//a @|complex| constant, not a @|dcomplex| constant)
//@<
//D = [B;2.0f+3.0f*i,i,0.0f]
//@>
//Likewise, but using @|dcomplex| constants
//@<
//E = [B;2.0+3.0*i,i,0.0]
//@>
//Finally, in FreeMat, you can construct matrices with strings
//as contents, but you have to make sure that if the matrix has
//more than one row, that all the strings have the same length.
//@<
//F = ['hello';'there']
//@>
//@@Tests
//@{ test_matcat1.m
//% Check the matcat function
//function test_val = test_matcat1
//a = [1;2];
//b = 3;
//d = 4;
//c = [a,[b;d]];
//test_val = test(c==[1,3;2,4]);
//@}
//@{ test_matcat2.m
//% Check the matcat function with n-dimensional arrays
//function test_val = test_matcat2
//a = 1;
//a(1,1,1) = 1;
//a(1,2,1) = 2;
//a(1,1,2) = 5;
//a(1,2,2) = 6;
//c = [a;a];
//c1 = c(:,:,1);
//c2 = c(:,:,2);
//test_val = test(c1==[1,2;1,2]) & test(c2==[5,6;5,6]);
//@}
//@{ test_matcat3.m
//% Check the matcat function with n-dimensional arrays
//function test_val = test_matcat3
//a = 1;
//a(1,1,1,1,1) = 1;
//a(1,2,1,1,1) = 2;
//a(1,1,1,1,2) = 5;
//a(1,2,1,1,2) = 6;
//c = [a;a];
//c1 = c(:,:,1,1,1);
//c2 = c(:,:,1,1,2);
//test_val = test(c1==[1,2;1,2]) & test(c2==[5,6;5,6]);
//@}
//@{ test_matcat4.m
//% Check the type promotion for the matrix cat function
//function test_val = test_matcat4
//a = [1,2;3.0f,4f+i];
//test_val = test(strcmp(typeof(a),'single'));
//@}
//@{ test_matcat5.m
//% Check the type promotion for the matrix cat function
//function test_val = test_matcat5
//a = [1,2;3.0,4.0+i];
//test_val = test(strcmp(typeof(a),'double'));
//@}
//@{ test_matcat6.m
//% Check the type promotion for the matrix cat function
//function test_val = test_matcat6
//a = [1,2;3.0f,4.0f+i];
//test_val = test(strcmp(typeof(a),'single'));
//@}
//@{ test_matcat7.m
//% Check that matrix cat works properly with spaces before the continuation
//function test_val = test_matcat7
//a = [1;2;...   
//     3;4; ...
//     5;6];
//b = [1, 2, 3, ...
//     4, 5, 6 ];
//c=[1, 2, ...
//              3];
//test_val = 1;
//@}
//@{ test_vec1.m
//% Check the case of continuation with vectors
//function test_val = test_vec1
//a = [1 2 3 4...
//       5 6 7 8];
//test_val = 1;
//@}
//@{ test_matcat8.m
//% Check that [1:0] is sane
//function test_val = test_matcat8
//test_val = isa([1:0],'double');
//@}
//!
//Works
Array Interpreter::matrixDefinition(const Tree & t) {
  ArrayMatrix m;
  if (t.numChildren() == 0) 
    return EmptyConstructor();
  for (int i=0;i<t.numChildren();i++) {
    const Tree & s(t.child(i));
    ArrayVector n;
    for (int j=0;j<s.numChildren();j++)
      multiexpr(s.child(j),n);
    m.push_back(n);
  }
  // Check if any of the elements are user defined classes
  bool anyuser = false;
  for (int i=0;i<m.size() && !anyuser;i++)
    for (int j=0;j<m[i].size() && !anyuser;j++)
      if (m[i][j].isUserClass()) 
	anyuser = true;
  if (!anyuser)
    return MatrixConstructor(m);
  else
    return ClassMatrixConstructor(m,this);
}

//!
//@Module CELL Cell Array Definitions
//@@Section VARIABLES
//@@Usage
//The cell array is a fairly powerful array type that is available
//in FreeMat.  Generally speaking, a cell array is a heterogenous
//array type, meaning that different elements in the array can 
//contain variables of different type (including other cell arrays).
//For those of you familiar with @|C|, it is the equivalent to the
//@|void *| array.  The general syntax for their construction is
//@[
//   A = {row_def1;row_def2;...;row_defN}
//@]
//where each row consists of one or more elements, seperated by
//commas
//@[
//  row_defi = element_i1,element_i2,...,element_iM
//@]
//Each element can be any type of FreeMat variable, including
//matrices, arrays, cell-arrays, structures, strings, etc.  The
//restriction on the definition is that each row must have the
//same number of elements in it.
//@@Examples
//Here is an example of a cell-array that contains a number,
//a string, and an array
//@<
//A = {14,'hello',[1:10]}
//@>
//Note that in the output, the number and string are explicitly
//printed, but the array is summarized.
//We can create a 2-dimensional cell-array by adding another
//row definition
//@<
//B = {pi,i;e,-1}
//@>
//Finally, we create a new cell array by placing @|A| and @|B|
//together
//@<
//C = {A,B}
//@>
//!
//Works
Array Interpreter::cellDefinition(const Tree & t) {
  ArrayMatrix m;
  if (t.numChildren() == 0)
    return Array(CellArray);
  for (int i=0;i<t.numChildren();i++) {
    const Tree & s(t.child(i));
    ArrayVector n;
    for (int j=0;j<s.numChildren();j++) 
      multiexpr(s.child(j),n);
    m.push_back(n);
  }
  return CellConstructor(m);
}

Array Interpreter::ShortCutOr(const Tree &  t) {
  Array a(expression(t.first()));
  Array retval;
  if (!a.isScalar())
    retval = DoBinaryOperator(t,Or,"or");
  else {
    // A is a scalar - is it true?
    if (a.toClass(Bool).constRealScalar<bool>())
      retval = a.toClass(Bool);
    else 
      retval = DoBinaryOperator(t,Or,"or");
  }
  return retval;
}

Array Interpreter::ShortCutAnd(const Tree & t) {
  context->setScopeTokenID(t.context());
  Array a(expression(t.first()));
  context->setScopeTokenID(t.context());
  Array retval;
  if (!a.isScalar()) {
    retval = DoBinaryOperator(t,And,"and");
  } else {
    // A is a scalar - is it false?
    if (!a.toClass(Bool).constRealScalar<bool>())
      retval = a.toClass(Bool);
    else 
      retval = DoBinaryOperator(t,And,"and");
  }
  return retval;
}

//Works
// Need to take care

ArrayVector Interpreter::handleReindexing(const Tree & t, const ArrayVector &p) {
  if (t.numChildren() > 2)
    if (p.size() > 1)
      throw Exception("reindexing of function expressions not allowed when multiple values are returned by the function");
    else {
      Array r;
      if (p.size() == 1)
	r = p[0];
      else
	r = EmptyConstructor();
      for (int index = 2;index < t.numChildren();index++) 
	deref(r,t.child(index));
      return ArrayVector() << r;
    }
  else
    return p;
}

void Interpreter::multiexpr(const Tree & t, ArrayVector &q, index_t lhsCount, bool output_optional) {
  if (t.is(TOK_VARIABLE)) {
    ArrayReference ptr(context->lookupVariable(t.first().text()));
    if (!ptr.valid()) {
      ArrayVector p;
      functionExpression(t,int(lhsCount),output_optional,p);
      q += handleReindexing(t,p);
      return;
    }
    if (t.numChildren() == 1) {
      q.push_back(*ptr);
      return;
    }
    if (ptr->isUserClass() && !stopoverload) {
      q += ClassRHSExpression(*ptr,t,this);
      return;
    }
    Array r(*ptr);
    for (int index = 1;index < t.numChildren()-1;index++) 
      deref(r,t.child(index));
    SaveEndInfo;
    endRef = &r;
    const Tree & s(t.last());
    if (s.is(TOK_PARENS)) {
      ArrayVector m;
      endTotal = s.numChildren();
      if (s.numChildren() == 0)
	q.push_back(r);
      else {
	for (int p = 0;p < s.numChildren(); p++) {
	  endCount = m.size();
	  multiexpr(s.child(p),m);
	}
	subsindex(m);
	if (m.size() == 1)
	  q.push_back(r.get(m.front()));
	else
	  q.push_back(r.get(m));
      }
    } else if (s.is(TOK_BRACES)) {
      ArrayVector m;
      endTotal = s.numChildren();
      for (int p = 0;p < s.numChildren(); p++) {
	endCount = m.size();
	multiexpr(s.child(p),m);
      }
      subsindex(m);
      if (m.size() == 1)
	q += ArrayVectorFromCellArray(r.get(m.front()));
      else
	q += ArrayVectorFromCellArray(r.get(m));
    } else if (s.is('.')) {
      q += r.get(s.first().text());
    } else if (s.is(TOK_DYN)) {
      QString field;
      try {
	Array fname(expression(s.first()));
	field = fname.asString();
      } catch (Exception &e) {
	throw Exception("dynamic field reference to structure requires a string argument");
      }
      q += r.get(field);
    }
    RestoreEndInfo;
  } else if (!t.is(TOK_KEYWORD))
    q.push_back(expression(t));
}

Array Interpreter::expression(const Tree & t) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return rhs(t);
  case TOK_REAL:
  case TOK_IMAG:
  case TOK_REALF:
  case TOK_IMAGF:
  case TOK_STRING:
    return t.array();
  case TOK_REINDEX:
    {
      Array r = expression(t.first());
      for (int index = 1;index < t.numChildren();index++)
	deref(r,t.child(index));
      return r;
    }
  case TOK_INCR_PREFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Add(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return ret;
    }
  case TOK_DECR_PREFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Subtract(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return ret;
    }
  case TOK_INCR_POSTFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Add(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return dummy;
    }
  case TOK_DECR_POSTFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Subtract(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return dummy;
    }
  case TOK_END:
    if (!endRef.valid()) 
      throw Exception("END keyword not allowed for undefined variables");
    if (endTotal == 1)
      return Array(double(endRef->length()));
    else
      return Array(double(endRef->dimensions()[endCount]));
  case ':':
    if (t.numChildren() == 0) {
      return Array(QString(":"));
    } else if (t.first().is(':')) {
      return doubleColon(t);
    } else {
      return unitColon(t);
    }
    break;
  case TOK_MATDEF: 
    return matrixDefinition(t); 
    break;
  case TOK_CELLDEF: 
    return cellDefinition(t); 
    break;
  case '+': 
    return DoBinaryOperator(t,Add,"plus"); 
    break;
  case '-': 
    return DoBinaryOperator(t,Subtract,"minus"); 
    break;
  case '*': 
    return DoBinaryOperator(t,Multiply,"mtimes"); 
    break;
  case '/': 
    return DoBinaryOperator(t,RightDivide,"mrdivide");
    break;
  case '\\': 
    return DoBinaryOperator(t,LeftDivide,"mldivide"); 
    break;
  case TOK_SOR: 
    return ShortCutOr(t); 
    break;
  case '|': 
    return DoBinaryOperator(t,Or,"or");
    break;
  case TOK_SAND: 
    return ShortCutAnd(t); 
    break;
  case '&': 
    return DoBinaryOperator(t,And,"and");
  case '<': 
    return DoBinaryOperator(t,LessThan,"lt"); 
    break;
  case TOK_LE: 
    return DoBinaryOperator(t,LessEquals,"le"); 
    break;
  case '>': 
    return DoBinaryOperator(t,GreaterThan,"gt"); 
    break;
  case TOK_GE: 
    return DoBinaryOperator(t,GreaterEquals,"ge"); 
    break;
  case TOK_EQ: 
    return DoBinaryOperator(t,Equals,"eq"); 
    break;
  case TOK_NE: 
    return DoBinaryOperator(t,NotEquals,"ne"); 
    break;
  case TOK_DOTTIMES: 
    return DoBinaryOperator(t,DotMultiply,"times"); 
    break;
  case TOK_DOTRDIV: 
    return DoBinaryOperator(t,DotRightDivide,"rdivide"); 
    break;
  case TOK_DOTLDIV: 
    return DoBinaryOperator(t,DotLeftDivide,"ldivide"); 
    break;
  case TOK_UNARY_MINUS: 
    return DoUnaryOperator(t,Negate,"uminus"); 
    break;
  case TOK_UNARY_PLUS: 
    return DoUnaryOperator(t,Plus,"uplus"); 
    break;
  case '~': 
    return DoUnaryOperator(t,Not,"not"); 
    break;
  case '^': 
    return DoBinaryOperator(t,Power,"mpower"); 
    break;
  case TOK_DOTPOWER: 
    return DoBinaryOperator(t,DotPower,"power"); 
    break;
  case '\'': 
    return DoUnaryOperator(t,Hermitian,"ctranspose"); 
    break;
  case TOK_DOTTRANSPOSE: 
    return DoUnaryOperator(t,Transpose,"transpose"); 
    break;
  case '@':
    return FunctionPointer(t);
  default:
    throw Exception("Unrecognized expression!");
  }
}

Array Interpreter::FunctionPointer(const Tree & t) {
   if (t.first().is(TOK_ANONYMOUS_FUNC)) {
     return AnonFuncConstructor(this,t.first());
   } else {
     FuncPtr val;
     if (!lookupFunction(t.first().text(),val))
       throw Exception("unable to resolve " + t.first().text() + 
		       " to a function call");
     return FuncPtrConstructor(this,val);
   }
}

//!
//@Module COLON Index Generation Operator
//@@Section OPERATORS
//@@Usage
//There are two distinct syntaxes for the colon @|:| operator - the two argument form
//@[
//  y = a : c
//@]
//and the three argument form
//@[
//  y = a : b : c
//@]
//The two argument form is exactly equivalent to @|a:1:c|.  The output @|y| is the vector
//\[
//  y = [a,a+b,a+2b,\ldots,a+nb]
//\]
//where @|a+nb <= c|.  There is a third form of the colon operator, the 
//no-argument form used in indexing (see @|indexing| for more details).
//@@Function Internals
//The colon operator turns out to be trickier to implement than one might 
//believe at first, primarily because the floating point versions should
//do the right thing, which is not the obvious behavior.  For example,
//suppose the user issues a three point colon command
//@[
//   y = a : b : c
//@]
//The first question that one might need to answer is: how many points
//in this vector?  If you answered
//\[
//   n = \frac{c-a}{b}+1
//\]
//then you would be doing the straighforward, but not correct thing.
//because a, b, and c are all floating point values, there are errors
//associated with each of the quantities that can lead to n not being
//an integer.  A better way (and the way FreeMat currently does the
//calculation) is to compute the bounding values (for b positive)
//\[
//   n \in \left[\frac{(c-a) \rightarrow 0}{b \rightarrow \infty},
//               \frac{(c-a) \rightarrow \infty}{b \rightarrow 0} \right] + 1
//\]
//where 
//\[
//  x \rightarrow y 
//\]
//means we replace x by the floating point number that is closest to it
//in the direction of y.  Once we have determined the number of points
//we have to compute the intermediate values
//\[
//  [a, a+b, a+2*b, \ldots, a+n*b]
//\]
//but one can readily verify for themselves that this may \emph{not} be
//the same as the vector
//\[
//  \mathrm{fliplr} [c, c-b, c-2*b, \ldots, c-n*b]
//\]
//even for the case where
//\[
//   c = a + n*b
//\]
//for some n.  The reason is that the roundoff in the calculations may
//be different depending on the nature of the sum.  FreeMat uses the
//following strategy to compute the double-colon vector:
//\begin{enumerate}
//\item The value @|n| is computed by taking the floor of the larger 
// value in the interval defined above.
//\item If @|n| falls inside the interval defined above, then it is 
//assumed that the user intended @|c = a + n*b|, and the symmetric 
//algorithm is used.  Otherwise, the nonsymmetric algorithm is used.
//\item The symmetric algorithm computes the vector via
//\[
//  [a, a+b, a+2b,\ldots,c-2b,c-b,c]
//\]
//working symmetrically from both ends of the vector 
//(hence the nomenclature), while the nonsymmetric algorithm computes
//\[
//  [a, a+b ,a+2b,\ldots,a+nb]
//\]
//In practice, the entries are computed by repeated accumulation instead 
//of multiplying the step size by an integer.
//\item The real interval calculation is modified so that we get the
//exact same result with @|a:b:c| and @|c:-b:a| (which basically means
//that instead of moving towards infinity, we move towards the signed 
//infinity where the sign is inherited from @|b|).
//\end{enumerate}
//If you think this is all very obscure, it is.  But without it, you will
//be confronted by mysterious vectors where the last entry is dropped,
//or where the values show progressively larger amounts of accumulated
//roundoff error.
//@@Examples
//Some simple examples of index generation.
//@<
//y = 1:4
//@>
//Now by half-steps:
//@<
//y = 1:.5:4
//@>
//Now going backwards (negative steps)
//@<
//y = 4:-.5:1
//@>
//If the endpoints are the same, one point is generated, regardless of the step size (middle argument)
//@<
//y = 4:1:4
//@>
//If the endpoints define an empty interval, the output is an empty matrix:
//@<
//y = 5:4
//@>
//@@Tests
//@{ test_range1.m
//% Check the range function for integer types positive with 3 args & horizontal concatenation
//function test_val = test_range1
//range = 0:1:3;
//range2 = [0,1,2,3];
//test_val = test(range == range2);
//@}
//@{ test_range2.m
//% Check the range function for integer types positive with 3 args & horizontal concatenation
//function test_val = test_range2
//range = (0:1:3)';
//range2 = [0;1;2;3];
//test_val = test(range == range2);
//@}
//@{ test_range3.m
//% Check the range function for float types with 2 args & horizontal concatenation
//function test_val = test_range3
//range = 0.2f:2.2f;
//range2 = [0.2f,1.2f,2.2f];
//test_val = test(range == range2);
//@}
//@{ test_range4.m
//% Check the range function for double types with 3 args & vertical concatenation
//function test_val = test_range4
//range = 0.45:3.45;
//range2 = [0.45,1.45,2.45,3.45];
//test_val = test(range == range2);
//@}
//@{ test_range5.m
//% Check the range function with negative integer
//function test_val = test_range5
//range = -2:2;
//range2 = [-2,-1,0,1,2];
//test_val = test(range == range2);
//@}
//@{ test_range6.m
//% Check the range function with a negative float
//function test_val = test_range6
//range = -2.f:3.f;
//range2 = [-2.f,-1.f,0.f,1.f,2.f,3.f];
//test_val = test(range == range2);
//@}
//@{ test_range7.m
//% Check the range function with a negative step size
//function test_val = test_range7
//range = 3:-1:-2;
//range2 = [3,2,1,0,-1,-2];
//test_val = test(range == range2);
//@}
//@{ test_range8.m
//% Check the range function with a negative step size
//function test_val = test_range8
//range = 3f:-1f:-2f;
//range2 = [3f,2f,1f,0f,-1f,-2f];
//test_val = test(range == range2);
//@}
//@{ test_range9.m
//% Check the range function with an empty range
//function test_val = test_range9
//range = 3:2;
//test_val = isempty(range);
//@}
//!
//Works
Array Interpreter::unitColon(const Tree & t) {
  Array a, b;
  a = expression(t.first());
  b = expression(t.second());
  if (!(a.isUserClass() || b.isUserClass()))
    return UnitColon(a,b);
  else
    return ClassBinaryOperator(a,b,"colon",this);
}

void Interpreter::deleteHandleClass(StructArray *ap)
{
  // We need to call the destructor on 
  Array b(*ap);
  delete ap;
  FuncPtr val;
  if (b.isUserClass() && ClassResolveFunction(this,b,"delete",val)) 
    {
      val->updateCode(this);
      ArrayVector args(b);
      doFunction(val,args,1);
    }
}

//Works
Array Interpreter::doubleColon(const Tree & t) {
  Array a, b, c;
  a = expression(t.first().first());
  b = expression(t.first().second());
  c = expression(t.second());
  if (!(a.isUserClass() || b.isUserClass() || c.isUserClass()))
    return DoubleColon(a,b,c);
  else
    return ClassTrinaryOperator(a,b,c,"colon",this);
}

/**
 * This somewhat strange test is used by the switch statement.
 * If x is a scalar, and we are a scalar, this is an equality
 * test.  If x is a string and we are a string, this is a
 * strcmp test.  If x is a scalar and we are a cell-array, this
 * test is applied on an element-by-element basis, looking for
 * any matches.  If x is a string and we are a cell-array, then
 * this is applied on an element-by-element basis also.
 */
bool Interpreter::testCaseStatement(const Tree & t, Array s) {
  Array r(expression(t.first()));
  bool caseMatched = TestForCaseMatch(s,r);
  if (caseMatched)
    block(t.second());
  return caseMatched;
}

//!
//@Module TRY-CATCH Try and Catch Statement
//@@Section FLOW
//@@Usage
//The @|try| and @|catch| statements are used for error handling
//and control.  A concept present in @|C++|, the @|try| and @|catch|
//statements are used with two statement blocks as follows
//@[
//   try
//     statements_1
//   catch
//     statements_2
//   end
//@]
//The meaning of this construction is: try to execute @|statements_1|,
//and if any errors occur during the execution, then execute the
//code in @|statements_2|.  An error can either be a FreeMat generated
//error (such as a syntax error in the use of a built in function), or
//an error raised with the @|error| command.
//@@Examples
//Here is an example of a function that uses error control via @|try|
//and @|catch| to check for failures in @|fopen|.
//@{ read_file.m
//function c = read_file(filename)
//try
//   fp = fopen(filename,'r');
//   c = fgetline(fp);
//   fclose(fp);
//catch
//   c = ['could not open file because of error :' lasterr]
//end
//@}
//Now we try it on an example file - first one that does not exist,
//and then on one that we create (so that we know it exists).
//@<
//read_file('this_filename_is_invalid')
//fp = fopen('test_text.txt','w');
//fprintf(fp,'a line of text\n');
//fclose(fp);
//read_file('test_text.txt')
//@>
//!
//Works
void Interpreter::tryStatement(const Tree & t) {
  // Turn off autostop for this statement block
  bool autostop_save = autostop;
  autostop = false;
  bool intryblock_save = intryblock;
  intryblock = true;
  // Get the state of the IDnum stack and the
  // contextStack and the cnameStack
  int stackdepth = context->scopeDepth();
  try {
    block(t.first());
  } catch (Exception &e) {
    while (context->scopeDepth() > stackdepth) context->popScope();
    if (t.numChildren()>1) {
      autostop = autostop_save;
      block(t.second().first());
    }
  } 
  autostop = autostop_save;
  intryblock = intryblock_save;
}


//!
//@Module SWITCH Switch statement
//@@Section FLOW
//@@Usage
//The @|switch| statement is used to selective execute code
//based on the value of either scalar value or a string.
//The general syntax for a @|switch| statement is
//@[
//  switch(expression)
//    case test_expression_1
//      statements
//    case test_expression_2
//      statements
//    otherwise
//      statements
//  end
//@]
//The @|otherwise| clause is optional.  Note that each test
//expression can either be a scalar value, a string to test
//against (if the switch expression is a string), or a 
//@|cell-array| of expressions to test against.  Note that
//unlike @|C| @|switch| statements, the FreeMat @|switch|
//does not have fall-through, meaning that the statements
//associated with the first matching case are executed, and
//then the @|switch| ends.  Also, if the @|switch| expression
//matches multiple @|case| expressions, only the first one
//is executed.
//@@Examples
//Here is an example of a @|switch| expression that tests
//against a string input:
//@{ switch_test.m
//function c = switch_test(a)
//  switch(a)
//    case {'lima beans','root beer'}
//      c = 'food';
//    case {'red','green','blue'}
//      c = 'color';
//    otherwise
//      c = 'not sure';
//  end
//@}
//Now we exercise the switch statements
//@<
//switch_test('root beer')
//switch_test('red')
//switch_test('carpet')
//@>
//@@Tests
//@{ test_switch1.m
//% Test the switch statement with a string argument
//function test_val = test_switch1
//      
//test_val = 0;
//x = 'astring';
//
//switch x
//case 'bstring'
//test_val = 0;
//case 'astring';
//test_val = 1;
//case 'cstring';
//test_val = 0;
//otherwise
//test_val = 0;
//end
//@}
//@{ test_switch2.m
//% Test the switch statement with a string argument and one integer case
//% The type mismatch should not present a problem.
//function test_val = test_switch2
//      
//test_val = 0;
//x = 'astring';
//
//switch x
//case 30
//test_val = 0;
//case 'astring';
//test_val = 1;
//case 'cstring';
//test_val = 0;
//otherwise
//test_val = 0;
//end
//@}
//@{ test_switch3.m
//% Test the switch statement with a scalar argument
//function test_val = test_switch3
//
//test_val = 0;
//x = 1.0f + i;
//z = 1.0 + i;
//switch x
//case 1
//test_val = 0;
//case i
//test_val = 0;
//case z
//test_val = 1;
//case 'astring'
//test_val = 0;
//otherwise
//test_val = 0;
//end
//@}
//@{ test_switch4.m
//% Test the switch statement with a vector argument (should be an error)
//function test_val = test_switch4
//
//x = [1 1];
//test_val = 0;
//try
//switch x
//case 1
//test_val = 0;
//otherwise
//test_val = 0;
//end
//catch
//test_val = 1;
//end
//@}
//@{ test_switch5.m
//% Test the switch statement with a numerical argument
//function test_val = test_switch5
//
//x = 1.2f;
//test_val = 0;
//switch x
//case 1
//test_val = 0;
//case 1.1f
//test_val = 0;
//case 1.2+i;
//test_val = 0;
//case 1.2;
//test_val = 0;
//case 1.2f;
//test_val = 1;
//end
//@}
//@{ test_switch6.m
//% Test the switch statement with a string argument and a cell array
//function test_val = test_switch6
//      
//test_val = 0;
//x = 'astring';
//
//switch x
//case 'bstring'
//test_val = 0;
//case {'dstring',5,'astring'};
//test_val = 1;
//case 'cstring';
//test_val = 0;
//otherwise
//test_val = 0;
//end
//@}
//@{ test_switch7.m
//% Test the switch statement with a numerical argument and a cell array
//function test_val = test_switch7
//      
//test_val = 0;
//x = 5;
//
//switch x
//case 'bstring'
//test_val = 0;
//case {'dstring',5.0,'astring'};
//test_val = 1;
//case 'cstring';
//test_val = 0;
//otherwise
//test_val = 0;
//end
//@}
//@{ test_switch8.m
//% Test the switch statement with a blank line after the switch clause
//function test_val = test_switch8
//   v = '3';
//
//   switch(v)
//
//   case '3'
//     vv = 111;
//   end
//   test_val = 1;
//@}
//!
//Works
void Interpreter::switchStatement(const Tree & t) {
  Array switchVal;
  // First, extract the value to perform the switch on.
  switchVal = expression(t.first());
  // Assess its type to determine if this is a scalar switch
  // or a string switch.
  if (!switchVal.isScalar() && !switchVal.isString())
    throw Exception("Switch statements support scalar and string arguments only.");
  int n=1;
  while (n < t.numChildren() && t.child(n).is(TOK_CASE)) {
    if (testCaseStatement(t.child(n),switchVal))
      return;
    n++;
  }
  if (t.last().is(TOK_OTHERWISE))
    block(t.last().first());
}

//!
//@Module IF-ELSEIF-ELSE Conditional Statements
//@@Section FLOW
//@@Usage
//The @|if| and @|else| statements form a control structure for
//conditional execution.  The general syntax involves an @|if|
//test, followed by zero or more @|elseif| clauses, and finally
//an optional @|else| clause:
//@[
//  if conditional_expression_1
//    statements_1
//  elseif conditional_expression_2
//    statements_2
//  elseif conditional_expresiion_3
//    statements_3
//  ...
//  else
//    statements_N
//  end
//@]
//Note that a conditional expression is considered true if 
//the real part of the result of the expression contains
//any non-zero elements (this strange convention is adopted
//for compatibility with MATLAB).
//@@Examples
//Here is an example of a function that uses an @|if| statement
//@{ if_test.m
//function c = if_test(a)
//  if (a == 1)
//     c = 'one';
//  elseif (a==2)
//     c = 'two';
//  elseif (a==3)
//     c = 'three';
//  else
//     c = 'something else';
//  end
//@}
//Some examples of @|if_test| in action:
//@<
//if_test(1)
//if_test(2)
//if_test(3)
//if_test(pi)
//@>
//@@Tests
//@{ test_if1.m
//% Test the if statement
//function test_val = test_if1
//a = 1;
//test_val = 0;
//if (a == 1)
//  test_val = 1;
//end
//@}
//@{ test_if2.m
//% Test the if-else statement
//function test_val = test_if2
//a = 1;
//test_val = 0;
//if (a == 0)
//  test_val = 0;
//else
//  test_val = 1;
//end
//@}
//@{ test_if3.m
//% Test the if-elseif-else statement
//function test_val = test_if3
//a = 1;
//test_val = 0;
//if (a == 0)
//  test_val = 0;
//elseif (a == 3);
//  test_val = 0;
//elseif (a == 1);
//  test_val = 1;
//else
//  test_val = 0;
//end
//@}
//!
//Works
void Interpreter::ifStatement(const Tree & t) {
  bool condtest = !(RealAllZeros(expression(t.first())));
  if (condtest) {
    block(t.second());
    return;
  } else {
    int n=2;
    while (n < t.numChildren() && t.child(n).is(TOK_ELSEIF)) {
      if (!(RealAllZeros(expression(t.child(n).first())))) {
	block(t.child(n).second());
	return;
      }
      n++;
    }
  }
  if (t.last().is(TOK_ELSE))
    block(t.last().first());
}

static bool compileJITBlock(Interpreter *interp, const Tree & t, JITInfo & ref) {
  delete ref.JITFunction();
  ref.setJITState(JITInfo::FAILED);
  JITFuncBase *cg = JITFactory::GetJITFunc(interp);
  if (!cg) return false;
  bool success = false;
  try {
    if (!cg->compile(t))
      {
	delete cg;
	success = false;
	ref.setJITState(JITInfo::FAILED);
	return success;
      }
    success = true;
    ref.setJITState(JITInfo::SUCCEEDED);
    ref.setJITFunction(cg);
    dbout << "Block JIT compiled at line " 
	  << LineNumber(interp->getContext()->scopeTokenID())
	  << " of " << interp->getContext()->scopeName() << "\n";
  } catch (Exception &e) {
    dbout << "JIT compile failed:" << e.msg() << " at line " 
      	  << LineNumber(interp->getContext()->scopeTokenID())
	  << " of " << interp->getContext()->scopeName() << "\n";
    delete cg;
    success = false;
    ref.setJITState(JITInfo::FAILED);
  }
  return success;
}

bool Interpreter::tryJitCode(const Tree & t) {
  // Try to compile this block to an instruction stream  
  if (jitcontrol) {
    int UID = t.node().UID();
    JITInfo & ref = m_codesegments[UID];
    if (ref.JITState() == JITInfo::UNTRIED) {
      bool success = compileJITBlock(this,t,ref);
      if (success) 
	{
	  if (ref.JITFunction()->run() == CJIT_Success)
	    return true;
	  ref.setJITState(JITInfo::FAILED);
	  return false;
	}
    } else if (ref.JITState() == JITInfo::SUCCEEDED) {
      int stat = ref.JITFunction()->run();
      if (stat == CJIT_Success)
	return true;
      // If the prep stage failed, we can try to recompile
      dbout << "Prep failed for JIT block retrying\n";
      if (stat == CJIT_Prepfail)
	{
	  bool success = compileJITBlock(this,t,ref);
	  if (success)
	    {
	      if (ref.JITFunction()->run() == CJIT_Success)
		return true;
	    }
	}
    }
    ref.setJITState(JITInfo::FAILED);
  }
  return false;
}

//!
//@Module WHILE While Loop
//@@Section FLOW
//@@Usage
//The @|while| loop executes a set of statements as long as
//a the test condition remains @|true|.  The syntax of a 
//@|while| loop is
//@[
//  while test_expression
//     statements
//  end
//@]
//Note that a conditional expression is considered true if 
//the real part of the result of the expression contains
//any non-zero elements (this strange convention is adopted
//for compatibility with MATLAB).
//@@Examples
//Here is a @|while| loop that adds the integers from @|1|
//to @|100|:
//@<
//accum = 0;
//k=1;
//while (k<=100), accum = accum + k; k = k + 1; end
//accum
//@>
//@@Tests
//@{ test_while1.m
//% Test the while statement for normal operation
//function test_val = test_while1
//sum = 0;
//i = 1;
//while (i<=10)
//  sum = sum + i;
//  i = i + 1;
//end
//test_val = test(sum == 55);
//@}
//@{ test_while2.m
//% Test a simple 'while' statement with a continue clause
//function test_val = test_while2
//sum = 0;
//i = 0;
//while (i < 10)
//  i = i + 1;
//  sum = sum + i;
//  if (i==5), continue; end
//  sum = sum - 1;
//end
//test_val = test(sum == 46);
//@}
//@{ test_while3.m
//% Test a simple 'while' statement with a continue clause
//function test_val = test_while3
//sum = 0;
//i = 1;
//while (i < 10)
//  sum = sum + i;
//  if (i==5), break; end
//  i = i + 1;
//end
//test_val = test(sum == 15);
//@}
//@{ testeq.m
//function x = testeq(a,b)
//  if (size(a,1) ~= size(b,1) || size(a,2) ~= size(b,2))
//    x = 0;
//    return;
//  end
//  d = full(a)-full(b);
//  if (strcmp(typeof(d),'double') | strcmp(typeof(d),'dcomplex'))
//    x = isempty(find(abs(d)>10*eps));
//  else
//    x = isempty(find(abs(d)>10*feps));
//  end
//@}
//!
//Works
void Interpreter::whileStatement(const Tree & t) {
  if (tryJitCode(t)) return;
  const Tree & testCondition(t.first());
  const Tree & codeBlock(t.second());
  bool breakEncountered = false;
  Array condVar(expression(testCondition));
  bool conditionTrue = !RealAllZeros(condVar);
  context->enterLoop();
  breakEncountered = false;
  while (conditionTrue && !breakEncountered) {
    try {
      block(codeBlock);
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterBreakException& e) {
      breakEncountered = true;
    } catch (InterpreterReturnException& e) {
      context->exitLoop();
      throw;
    } catch (InterpreterRetallException& e) {
      context->exitLoop();
      throw;
    }
    if (!breakEncountered) {
      condVar = expression(testCondition);
      conditionTrue = !RealAllZeros(condVar);
    } 
  }
  context->exitLoop();
}

//Helper functions for FOR loops.  This template function
//handles the index variable with the correct type.  Reducing
//the net loop time

class ContextLoopLocker {
  Context* m_context;
public:
  ContextLoopLocker(Context* a): m_context(a) {m_context->enterLoop();}
  ~ContextLoopLocker() {m_context->exitLoop();}
};

inline bool IsIntegerDataClass( const Array& a )
{
    return (a.dataClass() >= Int8) && (a.dataClass() <= UInt64);
}

template <class T>
void ForLoopHelper(const Tree & codeBlock, const Array& indexSet, 
		   index_t count, const QString& indexName, Interpreter *eval) {
  for (index_t m=1;m<=count;m++) {
    Array *vp = eval->getContext()->lookupVariableLocally( indexName );
    if ((!vp) || (!vp->isScalar())) {
	eval->getContext()->insertVariableLocally(indexName,Array());
	vp = eval->getContext()->lookupVariableLocally(indexName);
    }

    *vp = indexSet.get(m);
    try {
      eval->block(codeBlock);
    } catch (InterpreterContinueException &) {
    } catch (InterpreterBreakException &) {
      break;
    } 
  }
}

void ForLoopIterator( const Tree & codeBlock, QString indexName, 
		     Array& first, Array& last, Array& step, Interpreter *eval)
{
    int nsteps;

    if( !( first.isScalar() && last.isScalar() && step.isScalar() ) )
	throw Exception("Loop parameters must be scalar.");

    Array *vp = eval->getContext()->lookupVariableLocally( indexName );
    if ((!vp) || (!vp->isScalar())) {
	eval->getContext()->insertVariableLocally(indexName,Array());
	vp = eval->getContext()->lookupVariableLocally(indexName);
    }

    bool bIntLoop = (IsIntegerDataClass(first) || IsIntegerDataClass(last) || IsIntegerDataClass(step));

    if( bIntLoop ){
	//integer loop path
	Array temp1;

	temp1 = DotRightDivide( Subtract( last, first ), step );    //( ( l - f ) / s )
	nsteps = temp1.asInteger() + 1;	    //( ( l - f ) / s )+1
	if( nsteps < 0 ) return;

	for (int m=0;m<nsteps;m++) {
	    *vp = Add( first, DotMultiply( Array(m), step ) );
	    try {
		eval->block(codeBlock);
	    } 
	    catch (InterpreterContinueException &e) {
	    } 
	    catch (InterpreterBreakException &e) {
		break;
	    } 
	}
    }
    else{
	//floating point loop path
	bool bFloatLoop = ( first.dataClass() == Float || last.dataClass() == Float || step.dataClass() == Float );
	double f = first.asDouble();
	double l = last.asDouble();
	double s = step.asDouble();

	if( bFloatLoop )
	    nsteps = num_for_loop_iter_f(f, s, l);  
	else
	    nsteps = num_for_loop_iter(f, s, l);
        
	for (double m=0;m<nsteps;m++) { //array variable should be of type double for correct typing of DotMultiply
	    *vp = Add( first, DotMultiply( Array(m), step ) );
	    try {
		eval->block(codeBlock);
	    } 
	    catch (InterpreterContinueException &) {} 
	    catch (InterpreterBreakException &) {
		break;
	    } 
	}
    }
}

extern "C"
float num_for_loop_iter_f( float first, float step, float last )
{
    int signum = (step > 0) - (step < 0);
    int nsteps = (int) floor( ( last - first ) / step ) + 1;  
    if( nsteps < 0 ) 
	return 0;

    float mismatch = signum*(first + nsteps*step - last);
    if( (mismatch > 0) && ( mismatch < 3.*fepsf(last) ) && ( step != rint(step) ) ) //allow overshoot by 3 eps in some cases
	nsteps++;
    
    return nsteps;
}

extern "C"
double num_for_loop_iter( double first, double step, double last )
{
    int signum = (step > 0) - (step < 0);
    int nsteps = (int) floor( ( last - first ) / step ) + 1;  
    if( nsteps < 0 ) 
	return 0;

    double mismatch = signum*(first + nsteps*step - last);
    if( (mismatch > 0) && ( mismatch < 3.*feps(last) ) && ( step != rint(step) ) ) //allow overshoot by 3 eps in some cases
	nsteps++;

    return nsteps;
}

//!
//@Module FOR For Loop
//@@Section FLOW
//@@Usage
//The @|for| loop executes a set of statements with an 
//index variable looping through each element in a vector.
//The syntax of a @|for| loop is one of the following:
//@[
//  for (variable=expression)
//     statements
//  end
//@]
//Alternately, the parenthesis can be eliminated
//@[
//  for variable=expression
//     statements
//  end
//@]
//or alternately, the index variable can be pre-initialized
//with the vector of values it is going to take:
//@[
//  for variable
//     statements
//  end
//@]
//The third form is essentially equivalent to @|for variable=variable|,
//where @|variable| is both the index variable and the set of values
//over which the for loop executes.  See the examples section for
//an example of this form of the @|for| loop.
//@@Examples
//Here we write @|for| loops to add all the integers from
//@|1| to @|100|.  We will use all three forms of the @|for|
//statement.
//@<
//accum = 0;
//for (i=1:100); accum = accum + i; end
//accum
//@>
//The second form is functionally the same, without the
//extra parenthesis
//@<
//accum = 0;
//for i=1:100; accum = accum + i; end
//accum
//@>
//In the third example, we pre-initialize the loop variable
//with the values it is to take
//@<
//@>
//@@Tests
//@{ test_for1.m
//% Test a simple 'for' statement
//function test_val = test_for1
//sum = 0;
//for i = 1:10;
// sum = sum + i;
//end
//test_val = test(sum == 55);
//@}
//@{ test_for2.m
//% Test a simple 'for' statement with a continue clause
//function test_val = test_for2
//sum = 0;
//for i = 1:10;
// sum = sum + i;
// if (i==5), continue; end
// sum = sum - 1;
//end
//test_val = test(sum == 46);
//@}
//@{ test_for3.m
//% Test a simple 'for' statement with a continue clause
//function test_val = test_for3
//sum = 0;
//for i = 1:10;
// sum = sum + i;
// if (i==5), break; end;
//end
//test_val = test(sum == 15);
//@}
//@{ test_for4.m
//% Test a simple 'for' statement with a return clause
//function test_val = test_for4
//tval = test_for4_assist;
//test_val = test(tval == 15);
//
//function sum = test_for4_assist
//sum = 0;
//for i = 1:10;
// sum = sum + i;
// if (i==5), return; end;
//end
//@}
//!
//Works

void Interpreter::forStatement(const Tree & t) {
  if (tryJitCode(t)) return;
  Array indexSet;
  QString indexVarName;
    /* Get the name of the indexing variable */
    if( !t.first().is('=') )
	throw Exception( "Incorrect format of for operator" );

    indexVarName = t.first().first().text();

    if( t.first().second().is(TOK_MATDEF) ||
	t.first().second().is(TOK_VARIABLE) )   { 
	  //case "for j=[1:10]"
	  //case "for j=K" skb
    	/* Evaluate the index set */
	indexSet = expression(t.first().second());

	/* Get the code block */
	const Tree & codeBlock(t.second());
	index_t elementCount = indexSet.length();
	DataClass loopType(indexSet.dataClass());
	ContextLoopLocker lock(context);
	switch(loopType) {
	  case Invalid:
  	      throw Exception("Invalid arrays not supported");
  	  case Struct:
	      throw Exception("Structure arrays are not supported as for loop index sets");
	  case CellArray:
	      ForLoopHelper<Array>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Bool:
	      ForLoopHelper<logical>(codeBlock, indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt8:
	      ForLoopHelper<uint8>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int8:
	      ForLoopHelper<int8>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt16:
	      ForLoopHelper<uint16>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int16:
	      ForLoopHelper<int16>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt32:
	      ForLoopHelper<uint32>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int32:
	      ForLoopHelper<int32>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt64:
	      ForLoopHelper<uint64>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int64:
	      ForLoopHelper<int64>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Float:
	      ForLoopHelper<float>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Double:
	      ForLoopHelper<double>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case StringArray:
	      ForLoopHelper<uint8>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	}
    }
    else if( t.first().second().token() == ':' ){
	if (t.first().second().numChildren() == 0) 
	    throw Exception( "Incorrect format of loop operator parameters" );
  
	Array first, step, last;
	const Tree & codeBlock(t.second());
	ContextLoopLocker lock(context);

	if (t.first().second().first().is(':')) {
	  first = expression(t.first().second().first().first());
	  step = expression(t.first().second().first().second());
	  last = expression(t.first().second().second());
	  ForLoopIterator( codeBlock, indexVarName, first, last, step, this);
	    //return doubleColon(t);
	} 
	else {
	  first = expression(t.first().second().first());
	  last = expression(t.first().second().second());
	  Array tmp(BasicArray<double>(1));
	  ForLoopIterator( codeBlock, indexVarName, first, last, tmp, this);
	    //return unitColon(t);
	}
    }
}

//!
//@Module GLOBAL Global Variables
//@@Section VARIABLES
//@@Usage
//Global variables are shared variables that can be
//seen and modified from any function or script that 
//declares them.  The syntax for the @|global| statement
//is
//@[
//  global variable_1 variable_2 ...
//@]
//The @|global| statement must occur before the variables
//appear.
//@@Example
//Here is an example of two functions that use a global
//variable to communicate an array between them.  The
//first function sets the global variable.
//@{ set_global.m
//function set_global(x)
//  global common_array
//  common_array = x;
//@}
//The second function retrieves the value from the global
//variable
//@{ get_global.m
//function x = get_global
//  global common_array
//  x = common_array;
//@}
//Here we exercise the two functions
//@<
//set_global('Hello')
//get_global
//@>
//@@Tests
//@{ test_global1.m
//function test_val = test_global1
//accum = 0;
//for (i=1:10)
//  accum = accum + test_global1_assist;
//end
//test_val = test(accum == 55);
//
//function tick = test_global1_assist
//global count_test_global1_assist
//count_test_global1_assist = count_test_global1_assist + 1;
//tick = count_test_global1_assist;
//@}
//!
void Interpreter::globalStatement(const Tree & t) {
  for (int i=0;i<t.numChildren();i++) {
    QString name = t.child(i).text();
    context->addGlobalVariable(name);
    if (!context->lookupVariable(name).valid())
      context->insertVariable(name,EmptyConstructor());
  }
}

//!
//@Module PERSISTENT Persistent Variables
//@@Section VARIABLES
//@@Usage
//Persistent variables are variables whose value persists between
//calls to a function or script.  The general syntax for its
//use is
//@[
//   persistent variable1 variable2 ... variableN
//@]
//The @|persistent| statement must occur before the variable
//is the tagged as persistent.  Per the MATLAB API documentation
//an empty variable is created when the @|persistent| statement
//is called.
//@@Example
//Here is an example of a function that counts how many
//times it has been called.
//@{ count_calls.m
//function count_calls
//  persistent ccount
//  if isempty(ccount); ccount = 0; end;
//  ccount = ccount + 1;
//  printf('Function has been called %d times\n',ccount);
//@}
//We now call the function several times:
//@<
//for i=1:10; count_calls; end
//@>
//@@Tests
//@{ test_persistent1.m
//% Test the persistent variable declaration
//function test_val = test_persistent1
//accum = 0;
//for (i=1:10)
//  accum = accum + test_persistent1_assist;
//end
//test_val = test(accum == 55);
//
//function tick = test_persistent1_assist
//persistent count;
//count = count + 1;
//tick = count;
//@}
//@{ test_persistent2.m
//function y = test_persistent2
//persistent a b c d e
//global a1 b1 c1 d1 e1
//y = 1;
//@}
//!
void Interpreter::persistentStatement(const Tree & t) {
  for (int i=0;i<t.numChildren();i++) {
    QString name = t.child(i).text();
    context->addPersistentVariable(name);
    if (!context->lookupVariable(name).valid())
      context->insertVariable(name,EmptyConstructor());
  }
}

//!
//@Module CONTINUE Continue Execution In Loop
//@@Section FLOW
//@@Usage
//The @|continue| statement is used to change the order of
//execution within a loop.  The @|continue| statement can
//be used inside a @|for| loop or a @|while| loop.  The
//syntax for its use is 
//@[
//   continue
//@]
//inside the body of the loop.  The @|continue| statement
//forces execution to start at the top of the loop with
//the next iteration.  The examples section shows how
//the @|continue| statement works.
//@@Example
//Here is a simple example of using a @|continue| statement.
//We want to sum the integers from @|1| to @|10|, but not
//the number @|5|.  We will use a @|for| loop and a continue
//statement.
//@{ continue_ex.m
//function accum = continue_ex
//  accum = 0;
//  for i=1:10
//    if (i==5)
//      continue;
//    end
//    accum = accum + 1; %skipped if i == 5!
//  end
//@}
//The function is exercised here:
//@<
//continue_ex
//sum([1:4,6:10])
//@>
//@@Tests
//@{ test_continue1.m
//% Test the continuation character with white space after it (bug 1642992)
//function test_val = test_continue1
//test_val = ...    % This should be ignored
//   ...            % So should this
//   1;
//@}
//!

//!
//@Module BREAK Exit Execution In Loop
//@@Section FLOW
//@@Usage
//The @|break| statement is used to exit a loop prematurely.
//It can be used inside a @|for| loop or a @|while| loop.  The
//syntax for its use is
//@[
//   break
//@]
//inside the body of the loop.  The @|break| statement forces
//execution to exit the loop immediately.
//@@Example
//Here is a simple example of how @|break| exits the loop.
//We have a loop that sums integers from @|1| to @|10|, but
//that stops prematurely at @|5| using a @|break|.  We will
//use a @|while| loop.
//@{ break_ex.m
//function accum = break_ex
//  accum = 0;
//  i = 1;
//  while (i<=10) 
//    accum = accum + i;
//    if (i == 5)
//      break;
//    end
//    i = i + 1;
//  end
//@}
//The function is exercised here:
//@<
//break_ex
//sum(1:5)
//@>
//!

//!
//@Module RETURN Return From Function
//@@Section FLOW
//@@Usage
//The @|return| statement is used to immediately return from
//a function, or to return from a @|keyboard| session.  The 
//syntax for its use is
//@[
//  return
//@]
//Inside a function, a @|return| statement causes FreeMat
//to exit the function immediately.  When a @|keyboard| session
//is active, the @|return| statement causes execution to
//resume where the @|keyboard| session started.
//@@Example
//In the first example, we define a function that uses a
//@|return| to exit the function if a certain test condition 
//is satisfied.
//@{ return_func.m
//function ret = return_func(a,b)
//  ret = 'a is greater';
//  if (a > b)
//    return;
//  end
//  ret = 'b is greater';
//  printf('finishing up...\n');
//@}
//Next we exercise the function with a few simple test
//cases:
//@<
//return_func(1,3)
//return_func(5,2)
//@>
//In the second example, we take the function and rewrite
//it to use a @|keyboard| statement inside the @|if| statement.
//@{ return_func2.m
//function ret = return_func2(a,b)
//  if (a > b)
//     ret = 'a is greater';
//     keyboard;
//  else
//     ret = 'b is greater';
//  end
//  printf('finishing up...\n');
//@}
//Now, we call the function with a larger first argument, which
//triggers the @|keyboard| session.  After verifying a few
//values inside the @|keyboard| session, we issue a @|return|
//statement to resume execution.
//@<
//return_func2(2,4)
//return_func2(5,1)
//ret
//a
//b
//return
//@>
//!

//!
//@Module QUIT Quit Program
//@@Section FREEMAT
//@@Usage
//The @|quit| statement is used to immediately exit the FreeMat
//application.  The syntax for its use is
//@[
//   quit
//@]
//!

//!
//@Module RETALL Return From All Keyboard Sessions
//@@Section FLOW
//@@Usage
//The @|retall| statement is used to return to the base workspace
//from a nested @|keyboard| session.  It is equivalent to forcing
//execution to return to the main prompt, regardless of the level
//of nesting of @|keyboard| sessions, or which functions are 
//running.  The syntax is simple
//@[
//   retall
//@]
//The @|retall| is a convenient way to stop debugging.  In the
//process of debugging a complex program or set of functions,
//you may find yourself 5 function calls down into the program
//only to discover the problem.  After fixing it, issueing
//a @|retall| effectively forces FreeMat to exit your program
//and return to the interactive prompt.
//@@Example
//Here we demonstrate an extreme example of @|retall|.  We
//are debugging a recursive function @|self| to calculate the sum
//of the first N integers.  When the function is called,
//a @|keyboard| session is initiated after the function
//has called itself N times.  At this @|keyboard| prompt,
//we issue another call to @|self| and get another @|keyboard|
//prompt, this time with a depth of 2.  A @|retall| statement
//returns us to the top level without executing the remainder
//of either the first or second call to @|self|:
//@{ self.m
//function y = self(n)
//  if (n>1)
//    y = n + self(n-1);
//    printf('y is %d\n',y);
//  else
//    y = 1;
//    printf('y is initialized to one\n');
//    keyboard
//  end
//@}
//@<
//self(4)
//self(6)
//retall
//@>
//!

//!
//@Module KEYBOARD Initiate Interactive Debug Session
//@@Section FLOW
//@@Usage
//The @|keyboard| statement is used to initiate an
//interactive session at a specific point in a function.
//The general syntax for the @|keyboard| statement is
//@[
//   keyboard
//@]
//A @|keyboard| statement can be issued in a @|script|,
//in a @|function|, or from within another @|keyboard| session.
//The result of a @|keyboard| statement is that execution
//of the program is halted, and you are given a prompt
//of the form:
//@[
// [scope,n] -->
//@]
//where @|scope| is the current scope of execution (either
//the name of the function we are executing, or @|base| otherwise).
//And @|n| is the depth of the @|keyboard| session. If, for example,
//we are in a @|keyboard| session, and we call a function that issues
//another @|keyboard| session, the depth of that second session 
//will be one higher.  Put another way, @|n| is the number of @|return|
//statements you have to issue to get back to the base workspace.
//Incidentally, a @|return| is how you exit the @|keyboard| session
//and resume execution of the program from where it left off.  A
//@|retall| can be used to shortcut execution and return to the base
//workspace.
//
//The @|keyboard| statement is an excellent tool for debugging
//FreeMat code, and along with @|eval| provide a unique set of
//capabilities not usually found in compiled environments.  Indeed,
//the @|keyboard| statement is equivalent to a debugger breakpoint in 
//more traditional environments, but with significantly more inspection
//power.
//@@Example
//Here we demonstrate a two-level @|keyboard| situation.  We have
//a simple function that calls @|keyboard| internally:
//@{ key_one.m
//function c = key_one(a,b)
//c = a + b;
//keyboard
//@}
//Now, we execute the function from the base workspace, and
//at the @|keyboard| prompt, we call it again.  This action
//puts us at depth 2.  We can confirm that we are in the second
//invocation of the function by examining the arguments.  We
//then issue two @|return| statements to return to the base
//workspace.
//@<
//key_one(1,2)
//key_one(5,7)
//a
//b
//c
//return
//a
//b
//c
//return
//@>
//!

void Interpreter::doDebugCycle() {
  depth++;
  PopContext saver(context,0);
  {
    context->pushScope("keyboard","keyboard");
    PopContext saver2(context,0);
    context->setScopeActive(false);
    try {
      evalCLI();
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterBreakException& e) {
    } catch (InterpreterReturnException& e) {
    } catch (InterpreterRetallException& e) {
      depth--;
      throw;
    }
  }
  depth--;
}

void Interpreter::displayArray(Array b) {
  // Check for a user defined class
  FuncPtr val;
  if (b.isUserClass() && ClassResolveFunction(this,b,"display",val)) {
    if (val->updateCode(this)) refreshBreakpoints();
    ArrayVector args(b);
    ArrayVector retvec(doFunction(val,args,1));
  } else
    PrintArrayClassic(b,printLimit,this);
}

//Works
void Interpreter::expressionStatement(const Tree & s, bool printIt) {
  ArrayVector m;
  if (!s.is(TOK_EXPR)) throw Exception("Unexpected statement type!");
  const Tree & t(s.first());
  // There is a special case to consider here - when a 
  // function call is made as a statement, we do not require
  // that the function have an output.
  Array b;
  ArrayReference ptr;
  if (t.is(TOK_VARIABLE)) {
    ptr = context->lookupVariable(t.first().text());
    if (!ptr.valid()) {
      functionExpression(t,0,true,m);
      m = handleReindexing(t,m);
      bool emptyOutput = false;
      if (m.size() == 0) {
	b = EmptyConstructor();
	emptyOutput = true;
      } else 
	b = m[0];
      if (printIt && (!emptyOutput)) {
	outputMessage(QString("\nans = \n"));
	displayArray(b);
      }
    } else {
      multiexpr(t,m);
      if (m.size() == 0)
	b = EmptyConstructor();
      else {
	b = m[0];
	if (printIt) {
	  outputMessage(QString("\nans = \n"));
	  for (int j=0;j<m.size();j++) {
	    char buffer[1000];
	    if (m.size() > 1) {
	      sprintf(buffer,"\n%d of %d:\n",j+1,m.size());
	      outputMessage(QString(buffer));
	    }
	    displayArray(m[j]);
	  }
	}
      }
    }
  } else {
    b = expression(t);
    if (printIt) {
      outputMessage(QString("\nans = \n"));
      displayArray(b);
    } 
  }
  context->insertVariable("ans",b);
}

void Interpreter::multiassign(ArrayReference r, const Tree & s, ArrayVector &data) {
  SaveEndInfo;
  endRef = r;
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numChildren();
    if (s.numChildren() == 0) 
      throw Exception("The expression A() is not legal unless A is a function");
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->set(m[0],data[0]);
    else
      r->set(m,data[0]);
    data.pop_front();
  } else if (s.is(TOK_BRACES)) {
    ArrayVector m;
    endTotal = s.numChildren();
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      SetCellContents(*r,m[0],data);
    else
      SetCellContents(*r,m,data);
  } else if (s.is('.')) {
    r->set(s.first().text(),data);
  } else if (s.is(TOK_DYN)) {
    QString field;
    try {
      Array fname(expression(s.first()));
      field = fname.asString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    r->set(field,data);
  }
  RestoreEndInfo;
}

void Interpreter::assign(ArrayReference r, const Tree & s, Array &data) {
  SaveEndInfo;
  endRef = r;  
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numChildren();
    if (s.numChildren() == 0)
      throw Exception("The expression A() is not legal unless A is a function");
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->set(m[0],data);
    else
      r->set(m,data);
  } else if (s.is(TOK_BRACES)) {
    ArrayVector datav(data);
    ArrayVector m;
    endTotal = s.numChildren();
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      SetCellContents(*r,m[0],datav);
    else
      SetCellContents(*r,m,datav);
  } else if (s.is('.')) {
    ArrayVector datav(data);
    r->set(s.first().text(),datav);
  } else if (s.is(TOK_DYN)) {
    QString field;
    try {
      Array fname(expression(s.first()));
      field = fname.asString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    ArrayVector datav(data);
    r->set(field,datav);
  }
  RestoreEndInfo;
}


ArrayReference Interpreter::createVariable(QString name) {
  FuncPtr p;
  PopContext saver(context,0);
  if (context->lookupFunction(name,p) && (name.size() > 1))
    warningMessage("Newly defined variable " + name + " shadows a function of the same name.  Use clear " + name + " to recover access to the function");
  // Are we in a nested scope?
  if (!context->isCurrentScopeNested() || context->variableLocalToCurrentScope(name)) {
    // if not, just create a local variable in the current scope, and move on.
    context->insertVariable(name,EmptyConstructor());
  } else {
    // if so - walk up the scope chain until we are no longer nested
    QString localScopeName = context->scopeName();
    context->bypassScope(1);
    while (context->currentScopeNests(localScopeName))
      context->bypassScope(1);
    context->restoreScope(1);
    // We wre now pointing to the highest scope that contains the present
    // (nested) scope.  Now, we start walking down the chain looking for
    // someone who accesses this variable
    while (!context->currentScopeVariableAccessed(name) &&
	   context->scopeName() != localScopeName) 
      context->restoreScope(1);
    // Either we are back in the local scope, or we are pointing to
    // a scope that (at least theoretically) accesses a variable with 
    // the given name.
    context->insertVariable(name,EmptyConstructor());
  } 
  ArrayReference np(context->lookupVariable(name));
  if (!np.valid()) 
    throw Exception("error creating variable name " + name + 
		    " with scope " + context->scopeName());
  return np;
}

//Works
    // The case of a(1,2).foo.goo{3} = rhs
    // The tree looks like this:
    // Variable
    //    Identifier
    //    ()
    //      etc
    //    .
    //      foo
    //    .
    //      goo
    //    {}
    //      3
    //
    // We have to do:
    //   a1 = id       data = id
    //   a2 = id(etc)  stack[0] = id(etc)
    //   a3 = a2.foo   stack[1] = stack[0].foo
    //   a4 = a3.goo   stack[2] = stack[1].goo
    //   a3{3} = rhs   data{3} = rhs
    //   a2.foo = a3   
    //   id(etc) = a2;

//!
//@Module ASSIGN Making assignments
//@@Section ARRAY
//@@Usage
//FreeMat assignments take a number of different forms, depending 
//on the type of the variable you want to make an assignment to.
//For numerical arrays and strings, the form of an assignment
//is either
//@[
//  a(ndx) = val
//@]
//where @|ndx| is a set of vector indexing coordinates.  This means
//that the values @|ndx| takes reference the elements of @|a| in column
//order.  So, if, for example @|a| is an @|N x M| matrix, the first column
//has vector indices @|1,2,...,N|, and the second column has indices
//@|N+1,N+2,...,2N|, and so on.  Alternately, you can use multi-dimensional
//indexing to make an assignment:
//@[
//  a(ndx_1,ndx_2,..,ndx_m) = val
//@]
//where each indexing expression @|ndx_i| corresponds to the @|i-th| dimension
//of @|a|.  In both cases, (vector or multi-dimensional indexing), the
//right hand side @|val| must either be a scalar, an empty matrix, or of the
//same size as the indices.  If @|val| is an empty matrix, the assignment acts
//like a delete.  Note that the type of @|a| may be modified by the assignment.
//So, for example, assigning a @|double| value to an element of a @|float| 
//array @|a| will cause the array @|a| to become @|double|.  
//
//For cell arrays, the above forms of assignment will still work, but only
//if @|val| is also a cell array.  If you want to assign the contents of
//a cell in a cell array, you must use one of the two following forms, either
//@[
//  a{ndx} = val
//@]
//or
//@[
//  a{ndx_1,ndx_2,...,ndx_m} = val
//@]
//which will modify the contents of the cell.
//@@Tests
//@{ test_assign1.m
//% Test the assignment of a value to a cell array as a vector-contents
//function test_val = test_assign1
//a = {1,2,3,4,5};
//a{5} = -1;
//test_val = test(a{5} == -1);
//@}
//@{ test_assign2.m
//% Test the assignment of a value to a cell array as a vector-contents.
//function test_val = test_assign2
//a = {1,2,3,4,5};
//test_val = 0;
//try
//a{5:6} = -1;
//catch
//test_val = 1;
//end
//@}
//@{ test_assign3.m
//% Test the assignment of a value to a cell-array using content-based n-dim indexing
//function test_val = test_assign3
//a = {1,2;4,5};
//test_val = 0;
//try
//a{2,1:2} = -1;
//catch
//test_val = 1;
//end
//@}
//@{ test_assign4.m
//% Test the assignment of a value to a cell array as an n-dim indexing
//function test_val = test_assign4
//a = {1,2,3;4,5,6};
//a{2,3} = -1;
//test_val = test(a{6} == -1);
//@}
//@{ test_assign5.m
//% Test the field assignment with a scalar struct
//function test_val = test_assign5
//a = struct('foo',5);
//a.foo = -1;
//test_val = test(a.foo == -1);
//@}
//@{ test_assign6.m
//% Test the field assignment with a vector struct (should cause an error)
//function test_val = test_assign6
//a = struct('foo',{5,7});
//test_val = 0;
//try
//a.foo = -1;
//catch
//test_val = 1;
//end
//@}
//@{ test_assign7.m
//% Test the empty assignment for the empty cell array
//function test_val = test_assign7
//a = {};
//test_val = isempty(a);
//@}
//@{ test_assign8.m
//% Test the empty assignment for the empty array
//function test_val = test_assign8
//a = [];
//test_val = isempty(a);
//@}
//@{ test_assign9.m
//% Test for reassign bug in repmat
//function t = test_assign9
//a = {1,2,3};
//b = repmat(a,2);
//b = 'good';
//t = 1;
//@}
//@{ test_assign10.m
//% Test for multiple assign with structures
//function test_val = test_assign10
//a = zeros(3,4);
//[m.x m.y] = size(a);
//test_val = (m.x == 3) & (m.y == 4);
//@}
//@{ test_assign11.m
//% Test for multiple assign with arrays
//function test_val = test_assign11
//a = zeros(3,4);
//[b(1), b(2)] = size(a);
//test_val = (b(1) == 3) & (b(2) == 4);
//@}
//@{ test_assign12.m
//% Test for column assignment bug with complex arrays
//function test_val = test_assign12
//    A = [1+i 3-i 5+3i
//         7-i 8+9i 6+i
//         9   8-i   5i];
//    B = A;
//    A(:,2) = 0;
//    B(1:3,2) = 0;
//    test_val = all(A==B);
//@}
//@{ test_assign13.m
//% Test for assignment of empties to an empty
//function test_val = test_assign13
//    A = [];
//    A([],[],[]) = [];
//    test_val = 1;
//@}
//@{ test_assign14.m
//% Test for auto sizing of assignment to undefined variables
//function test_val = test_assign14
//   r = [2,3;3,4];
//   a(2,:,:) = r;
//   b = zeros(2,2,2);
//   b(2,:,:) = r;
//   test_val = all(a(:) == b(:)) && all(size(a) == size(b));
//@}
//@{ test_assign15.m
//% Test for error on illegal (incomplete) assign to empty variables
//function test_val = test_assign15
//   a = [2,3;4,5];
//   test_val = 0;
//   try
//     c(2,:) = a;
//   catch
//     test_val = 1;
//   end
//@}
//@{ test_assign16.m
//% Test for bug 1808557 - incorrect subset assignment with complex arrays
//function test_val = test_assign16
//   x = rand(4,2)+i*rand(4,2);
//   y = rand(2,2)+i*rand(2,2);
//   x(2:2:4,:)=y;
//   z = x;
//   for i=1:2;
//     for j=1:2;
//       z(2+(i-1)*2,j) = y(i,j);
//     end
//   end
//  q = x;
//  q(:,1) = y(:);
//  p = [x,x];
//  p(1,:) = y(:);
//  test_val = all(vec(z==x)) && all(q(:,1) == y(:)) && all(p(1,:).' == y(:)); 
//@}
//@{ test_assign17.m
//function test_val = test_assign17
//  x = [];
//  x(:,1) = 3;
//  test_val = (x == 3);
//@}
//@{ test_assign18.m
//function test_val = test_assign18
//  x = [];
//  x(:) = 3;
//  test_val = (x == 3);
//@}
//@{ test_assign19.m
//function test_val = test_assign19
//  g(3).foo = 3;
//  g(1) = [];
//  test_val = (g(2).foo == 3);
//@}
//@{ test_assign20.m
//function test_val = test_assign20
//  p = [];
//  p(:,1) = [1;3;5];
//  p(:,2) = [2;4;6];
//  q = [1,2;3,4;5,6];
//  test_val = testeq(p,q);
//@}
//@{ test_assign21.m
//function test_val = test_assign21
//  p = {'hello'};
//  q = p{1};
//  test_val = testeq(q,'hello');
//@}
//@{ test_assign22.m
//function test_val = test_assign22
//  A = [1,2,3];
//  test_val = 0;
//  try
//    A(0) = 2;
//  catch
//    test_val = 1;
//  end
//@}
//@{ test_sparse56.m
//% Test DeleteSparseMatrix function
//function x = test_sparse56
//xi = sparse_test_mat('int32',100);
//xf = sparse_test_mat('float',100);
//xd = sparse_test_mat('double',100);
//xc = sparse_test_mat('complex',100);
//xz = sparse_test_mat('dcomplex',100);
//xi = [];
//xf = [];
//xd = [];
//xc = [];
//xz = [];
//x = isempty(xi) & isempty(xf) & isempty(xd) & isempty(xc) & isempty(xz);
//@}
//@{ test_sparse63.m
//% Test sparse matrix array vector-subset extraction
//function x = test_sparse63
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//ndx = randi(ones(1500,1),300*400*ones(1500,1));
//a1 = yi1(ndx); b1 = zi1(ndx);
//a2 = yf1(ndx); b2 = zf1(ndx);
//a3 = yd1(ndx); b3 = zd1(ndx);
//a4 = yc1(ndx); b4 = zc1(ndx);
//a5 = yz1(ndx); b5 = zz1(ndx);
//x = testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
//ndx = [];
//a1 = yi1(ndx); b1 = zi1(ndx);
//a2 = yf1(ndx); b2 = zf1(ndx);
//a3 = yd1(ndx); b3 = zd1(ndx);
//a4 = yc1(ndx); b4 = zc1(ndx);
//a5 = yz1(ndx); b5 = zz1(ndx);
//x = x & testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
//@}
//@{ test_sparse64.m
//% Test sparse matrix array ndim-subset extraction
//function x = test_sparse64
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//row_ndx = randi(ones(150,1),300*ones(150,1));
//col_ndx = randi(ones(150,1),400*ones(150,1));
//a1 = yi1(row_ndx,col_ndx); b1 = zi1(row_ndx,col_ndx);
//a2 = yf1(row_ndx,col_ndx); b2 = zf1(row_ndx,col_ndx);
//a3 = yd1(row_ndx,col_ndx); b3 = zd1(row_ndx,col_ndx);
//a4 = yc1(row_ndx,col_ndx); b4 = zc1(row_ndx,col_ndx);
//a5 = yz1(row_ndx,col_ndx); b5 = zz1(row_ndx,col_ndx);
//x = testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
//row_ndx = [];
//a1 = yi1(row_ndx,col_ndx); b1 = zi1(row_ndx,col_ndx);
//a2 = yf1(row_ndx,col_ndx); b2 = zf1(row_ndx,col_ndx);
//a3 = yd1(row_ndx,col_ndx); b3 = zd1(row_ndx,col_ndx);
//a4 = yc1(row_ndx,col_ndx); b4 = zc1(row_ndx,col_ndx);
//a5 = yz1(row_ndx,col_ndx); b5 = zz1(row_ndx,col_ndx);
//x = x & testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
//col_ndx = [];
//a1 = yi1(row_ndx,col_ndx); b1 = zi1(row_ndx,col_ndx);
//a2 = yf1(row_ndx,col_ndx); b2 = zf1(row_ndx,col_ndx);
//a3 = yd1(row_ndx,col_ndx); b3 = zd1(row_ndx,col_ndx);
//a4 = yc1(row_ndx,col_ndx); b4 = zc1(row_ndx,col_ndx);
//a5 = yz1(row_ndx,col_ndx); b5 = zz1(row_ndx,col_ndx);
//x = x & testeq(a1,b1) & testeq(a2,b2) & testeq(a3,b3) & testeq(a4,b4) & testeq(a5,b5);
//@}
//@{ test_sparse65.m
//% Test sparse matrix array vector-subset assignment
//function x = test_sparse65
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//ndx = randi(ones(1500,1),300*400*ones(1500,1));
//gi = int32(100*randn(1500,1));
//gf = float(randn(1500,1));
//gd = randn(1500,1);
//gc = complex(randn(1500,1)+i*randn(1500,1));
//gz = dcomplex(randn(1500,1)+i*randn(1500,1));
//yi1(ndx) = gi; zi1(ndx) = gi;
//yf1(ndx) = gf; zf1(ndx) = gf;
//yd1(ndx) = gd; zd1(ndx) = gd;
//yc1(ndx) = gc; zc1(ndx) = gc;
//yz1(ndx) = gz; zz1(ndx) = gz;
//% Cannot use testeq because if we set the same element to two
//% different values, the result is different for full versus sparse
//% matrices
//x = testeq2(yi1,zi1,ndx) & testeq2(yf1,zf1,ndx) & testeq2(yd1,zd1,ndx) & testeq2(yc1,zc1,ndx) & testeq2(yz1,zz1,ndx);
//
//function n = length(x)
//n = prod(size(x));
//
//function x = testeq2(a,b,ndx)
//k = find(a-b);
//x = 1;
//for i = 1:length(k)
//  x = x & length(find(ndx == k(i)));
//end
//@}
//@{ test_sparse66.m
//% Test sparse matrix array ndim-subset assignment
//function x = test_sparse66
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//ndxr = randi(ones(100,1),300*ones(100,1));
//ndxc = randi(ones(100,1),400*ones(100,1));
//gi = int32(100*randn(100,100));
//gf = float(randn(100,100));
//gd = randn(100,100);
//gc = complex(randn(100,100)+i*randn(100,100));
//gz = dcomplex(randn(100,100)+i*randn(100,100));
//yi1(ndxr,ndxc) = gi; zi1(ndxr,ndxc) = gi;
//yf1(ndxr,ndxc) = gf; zf1(ndxr,ndxc) = gf;
//yd1(ndxr,ndxc) = gd; zd1(ndxr,ndxc) = gd;
//yc1(ndxr,ndxc) = gc; zc1(ndxr,ndxc) = gc;
//yz1(ndxr,ndxc) = gz; zz1(ndxr,ndxc) = gz;
//x = testeq(yi1,zi1) & testeq(yf1,zf1) & testeq(yd1,zd1) & testeq(yc1,zc1) & testeq(yz1,zz1);
//@}
//@{ test_sparse67.m
//% Test sparse matrix individual element retrieval
//function x = test_sparse67
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1(150,200),zi1(150,200)) & testeq(yf1(150,200),zf1(150,200)) & testeq(yd1(150,200),zd1(150,200)) & testeq(yc1(150,200),zc1(150,200)) & testeq(yz1(150,200),zz1(150,200));
//@}
//@{ test_sparse71.m
//% Test sparse matrix array column deletion
//function x = test_sparse71
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//ndxc = randi(ones(100,1),400*ones(100,1));
//yi1(:,ndxc) = [];
//yf1(:,ndxc) = [];
//yd1(:,ndxc) = [];
//yc1(:,ndxc) = [];
//yz1(:,ndxc) = [];
//zi1(:,ndxc) = [];
//zf1(:,ndxc) = [];
//zd1(:,ndxc) = [];
//zc1(:,ndxc) = [];
//zz1(:,ndxc) = [];
//x = testeq(yi1,zi1) & testeq(yf1,zf1) & testeq(yd1,zd1) & testeq(yc1,zc1) & testeq(yz1,zz1);
//@}
//@{ test_sparse72.m
//% Test sparse matrix array row deletion
//function x = test_sparse72
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//ndxr = randi(ones(100,1),300*ones(100,1));
//yi1(ndxr,:) = [];
//yf1(ndxr,:) = [];
//yd1(ndxr,:) = [];
//yc1(ndxr,:) = [];
//yz1(ndxr,:) = [];
//zi1(ndxr,:) = [];
//zf1(ndxr,:) = [];
//zd1(ndxr,:) = [];
//zc1(ndxr,:) = [];
//zz1(ndxr,:) = [];
//x = testeq(yi1,zi1) & testeq(yf1,zf1) & testeq(yd1,zd1) & testeq(yc1,zc1) & testeq(yz1,zz1);
//@}
//@{ test_sparse73.m
//% Test sparse matrix array vector deletion
//function x = test_sparse73
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//ndxr = randi(ones(200,1),400*300*ones(200,1));
//yi1(ndxr) = [];
//yf1(ndxr) = [];
//yd1(ndxr) = [];
//yc1(ndxr) = [];
//yz1(ndxr) = [];
//zi1(ndxr) = [];
//zf1(ndxr) = [];
//zd1(ndxr) = [];
//zc1(ndxr) = [];
//zz1(ndxr) = [];
//x = testeq(yi1,zi1) & testeq(yf1,zf1) & testeq(yd1,zd1) & testeq(yc1,zc1) & testeq(yz1,zz1);
//@}
//@{ test_sparse81.m
//function x = test_sparse81
//A = sparse(rand(10));
//C = [];
//D = [C,A];
//x = testeq(D,A);
//@}
//@{ test_sparse83.m
//function x = test_sparse83
//  A = sparse(rand(10));
//  B = sparse([]);
//  C = [B,B,B;B,B,B,A,B];
//  x = testeq(C,A);
//@}
//@{ test_sparse116.m
//function x = test_sparse116
//  a = [1,0,3,4,5;6,2,3,5,0;0,0,0,0,2];
//  A = sparse(a);
//  p = [3;4;5;9;10;20];
//  a(p) = 1;
//  A(p) = 1;
//  x = testeq(a,A);
//@}
//!
void Interpreter::assignment(const Tree & var, bool printIt, Array &b) {
  QString name(var.first().text());
  ArrayReference ptr(context->lookupVariable(name));
  if (!ptr.valid()) 
    ptr = createVariable(name);
  if (var.numChildren() == 1) {
    (*ptr) = b;
  } else if (ptr->isUserClass() && 
	     !inMethodCall(ptr->className()) && 
	     !stopoverload) {
    ClassAssignExpression(ptr,var,b,this);
  } else if (var.numChildren() == 2)
    assign(ptr,var.second(),b);
  else {
    ArrayVector stack;
    Array data(*ptr);
    int varCount = var.numChildren();
    for (int index=1;index<varCount-1;index++) {
      if (!data.isEmpty()) {
	try {
	  deref(data,var.child(index));
	} catch (Exception &e) {
	  data = EmptyConstructor();
	}
      }
      stack.push_back(data);
    }
    assign(&data,var.child(varCount-1),b);
    Array rhs(data);
    if (stack.size() > 0) {
      stack.pop_back();
      int ptr = 0;
      while (stack.size() > 0) {
	data = stack.back();
	assign(&data,var.child(varCount-2-ptr),rhs);
	rhs = data;
	stack.pop_back();
	ptr++;
      }
    }
    assign(ptr,var.child(1),rhs);
  }
  if (printIt) {
    outputMessage("\n");
    outputMessage(name);
    outputMessage(" = \n");
    displayArray(*ptr);
  }
}

void Interpreter::processBreakpoints(const Tree & t) {
  for (int i=0;i<bpStack.size();i++) {
    if ((bpStack[i].cname == context->scopeName()) && 
	((LineNumber(context->scopeTokenID()) == bpStack[i].tokid))) {
      doDebugCycle();
      context->setScopeTokenID(t.context());
    }
  }
  if (tracetrap > 0) {
    if ((LineNumber(context->scopeTokenID()) != tracecurrentline)) {
      tracetrap--;
      if (tracetrap == 0)
	doDebugCycle();
    }
  }
  if (context->scopeStepTrap() > 0) {
    if ((LineNumber(context->scopeTokenID())) != 
	context->scopeStepCurrentLine()) {
      context->setScopeStepTrap(context->scopeStepTrap()-1);
      if (context->scopeStepTrap() == 0)
	doDebugCycle();
    }
  }
}

void Interpreter::statementType(const Tree & t, bool printIt) {
  // check the debug flag
  context->setScopeTokenID(t.context());
  processBreakpoints(t);
  switch(t.token()) {
  case '=': 
    {
      Array b(expression(t.second()));
      assignment(t.first(),printIt,b);
    }
    break;
  case TOK_MULTI:
    multiFunctionCall(t,printIt);
    break;
  case TOK_SPECIAL:
    specialFunctionCall(t,printIt);
    break;
  case TOK_FOR:
    forStatement(t);
    break;
  case TOK_WHILE:
    whileStatement(t);
    break;
  case TOK_IF:
    ifStatement(t);
    break;
  case TOK_BREAK:
    if (context->inLoop()) 
      throw InterpreterBreakException();
    break;
  case TOK_CONTINUE:
    if (context->inLoop()) 
      throw InterpreterContinueException();
    break;
  case TOK_DBSTEP:
    dbstepStatement(t);
    emit RefreshBPLists();
    throw InterpreterReturnException();
    break;
  case TOK_DBTRACE:
    dbtraceStatement(t);
    emit RefreshBPLists();
    throw InterpreterReturnException();
    break;
  case TOK_DBUP:
    dbup();
    break;
  case TOK_DBDOWN:
    dbdown();
    break;
  case TOK_RETURN:
    throw InterpreterReturnException();
    break;
  case TOK_SWITCH:
    switchStatement(t);
    break;
  case TOK_TRY:
    tryStatement(t);
    break;
  case TOK_QUIT:
    throw InterpreterQuitException();
    break;
  case TOK_RETALL:
    throw InterpreterRetallException();
    break;
  case TOK_KEYBOARD:
    doDebugCycle();
    break;
  case TOK_GLOBAL:
    globalStatement(t);
    break;
  case TOK_PERSISTENT:
    persistentStatement(t);
    break;
  case TOK_EXPR:
    expressionStatement(t,printIt);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}


//Trapping at the statement level is much better! - two
//problems... try/catch and multiline statements (i.e.,atell.m)
//The try-catch one is easy, I think...  When a try occurs,
//we capture the stack depth... if an exception occurs, we 
//unwind the stack to this depth..
//The second one is trickier - suppose we have a conditional
//statement
//if (a == 3)
//    bfunc
//else
//    cfunc
//end
//this is represented in the parse tree as a single construct...
//

// 
//Works
void Interpreter::statement(const Tree & t) {
  try {
    if (t.is(TOK_QSTATEMENT))
      statementType(t.first(),false);
    else if (t.is(TOK_STATEMENT))
      statementType(t.first(),m_quietlevel == 0);
    else
      throw Exception("Unexpected statement type!\n");
  } catch (Exception& e) {
    if (autostop && !InCLI) {
      errorCount++;
      e.printMe(this);
      stackTrace();
      doDebugCycle();
    } else  {
      if (!e.wasHandled() && !InCLI && !intryblock) {
	stackTrace();
	e.markAsHandled();
      }
      throw;
    }
  }
}

//Works
void Interpreter::block(const Tree & t) {
  try {
    const TreeList statements(t.children());
    for (TreeList::const_iterator i=statements.begin();
	 i!=statements.end();i++) {
      if (m_kill)
	throw InterpreterKillException();
      if (m_interrupt) {
	outputMessage("Interrupt (ctrl-b) encountered\n");
	stackTrace();
	m_interrupt = false;
	doDebugCycle();
      } else 
	statement(*i);
    }
  } catch (Exception &e) {
    lasterr = e.msg();
    throw;
  }
}

// I think this is too complicated.... there should be an easier way
// Works
index_t Interpreter::countLeftHandSides(const Tree & t) {
  Array lhs;
  ArrayReference ptr(context->lookupVariable(t.first().text()));
  if (!ptr.valid())
    lhs = EmptyConstructor();
  else
    lhs = *ptr;
  if (t.numChildren() == 1) return 1;
  if (t.last().is(TOK_PARENS)) return 1;
  for (int index = 1;index < t.numChildren()-1;index++) {
    try {
      deref(lhs,t.child(index));
    } catch (Exception& e) {
      lhs = EmptyConstructor();
    }
  }
  if (t.last().is(TOK_BRACES)) {
    const Tree & s(t.last());
    ArrayVector m;
    for (int p = 0; p < s.numChildren(); p++) 
      multiexpr(s.child(p),m);
    subsindex(m);
    if (m.size() == 0)
      throw Exception("Expected indexing expression!");
    if (m.size() == 1) {
      // m[0] should have only one element...
      if (IsColonOp(m[0]))
	return (lhs.length());
      return (IndexArrayFromArray(m[0]).length());
    } else {
      int i=0;
      index_t outputCount=1;
      while (i<m.size()) {
	if (IsColonOp(m[i])) 
	  outputCount *= lhs.dimensions()[i];
	else {
	  outputCount *= IndexArrayFromArray(m[i]).length();
	}
	i++;
      }
      return (outputCount);
    }
  }
  if (t.last().is('.')) 
    return std::max((index_t)1,lhs.length());
  return 1;
}

Array Interpreter::AllColonReference(Array v, int index, int count) {
  if (v.isUserClass()) return EmptyConstructor();
  return Array(QString(":"));
}
  
//test
void Interpreter::specialFunctionCall(const Tree & t, bool printIt) {
  ArrayVector m;
  StringVector args;
  for (int index=0;index < t.numChildren();index++) 
    args.push_back(t.child(index).text());
  if (args.empty()) return;
  ArrayVector n;
  for (int i=1;i<args.size();i++)
    n.push_back(Array(args[i]));
  FuncPtr val;
  if (!lookupFunction(args[0],val,n))
    throw Exception("unable to resolve " + args[0] + " to a function call");
  if (val->updateCode(this)) refreshBreakpoints();
  m = doFunction(val,n,0);
}
 
void Interpreter::setBreakpoint(stackentry bp, bool enableFlag) {
  FuncPtr val;
  bool isFun = lookupFunction(bp.detail,val);
  if (!isFun) {
    warningMessage(QString("unable to find function ") + 
		   bp.detail + " to set breakpoint");
    return;
  }
  if (val->type() != FM_M_FUNCTION) {
    warningMessage("function " + bp.detail + 
		   " is not an m-file, and does not support breakpoints");
    return;
  }
  //  try {
  //     //    ((MFunctionDef*)val)->SetBreakpoint(bp.tokid,enableFlag);
  //   } catch (Exception &e) {
  //     e.printMe(this);
  //   }
}
 
void Interpreter::addBreakpoint(stackentry bp) {
  bpStack.push_back(bp);
  refreshBreakpoints();
  emit RefreshBPLists();
}

void Interpreter::refreshBreakpoints() {
  for (int i=0;i<bpStack.size();i++)
    setBreakpoint(bpStack[i],true);
}

//Some notes on the multifunction call...  This one is pretty complicated, and the current logic is hardly transparent.  Assume we have an expression of the form:
//
//[expr1 expr2 ... exprn] = fcall
//
//where fcall is a function call (obviously).  Now, we want to determine how many output arguments fcall should have.  There are several interesting cases to consider:
//
//expr_i is an existing numeric variable -- lhscount += 1
//
//expr_i is an existing cell array -- lhscount += size(expr_i)
//
//expr_i is an existing struct array -- lhscount += size(expr_i)
//
//expr_i does not exist -- lhscount += 1
//
//Where this will fail is in one case.  If expr_i is a cell reference for a variable that does not exist, and has a sized argument, something like
//[eg{1:3}]
//in which case the lhscount += 3, even though eg does not exist. 
// WORKS
void Interpreter::multiFunctionCall(const Tree & t, bool printIt) {
  ArrayVector m;
  TreeList s;
  Array c;
  index_t lhsCount;

  if (!t.first().is(TOK_BRACKETS))
    throw Exception("Illegal left hand side in multifunction expression");
  s = t.first().children();
  // We have to make multiple passes through the LHS part of the AST.
  // The first pass is to count how many function outputs are actually
  // being requested. 
  // Calculate how many lhs objects there are
  lhsCount = 0;
  for (int ind=0;ind<s.size();ind++) 
    lhsCount += countLeftHandSides(s[ind]);

  multiexpr(t.second(),m,lhsCount);

  int index;
  for (index=0;(index<s.size()) && (m.size() > 0);index++) {
    const Tree & var(s[index]);
    QString name(var.first().text());
    ArrayReference ptr(context->lookupVariable(name));
    if (!ptr.valid()) 
      ptr = createVariable(name);
    if (ptr->isUserClass() && 
	!inMethodCall(ptr->className()) && 
	!stopoverload && (var.numChildren() > 1)) {
      ClassAssignExpression(ptr,var,m.front(),this);
      m.pop_front();
      return;
    }
    if (var.numChildren() == 1) {
      (*ptr) = m.front();
      m.pop_front();
    } else if (var.numChildren() == 2)
      multiassign(ptr,var.second(),m);
    else {
      ArrayVector stack;
      Array data(*ptr);
      int varCount = var.numChildren();
      for (int index=1;index<varCount-1;index++) {
	if (!data.isEmpty()) {
	  try {
	    deref(data,var.child(index));
	  } catch (Exception &e) {
	    data = EmptyConstructor();
	  }
	}
	stack.push_back(data);
      }
      multiassign(&data,var.child(varCount-1),m);
      Array rhs(data);
      if (stack.size() > 0) {
	stack.pop_back();
	int ptr = 0;
	while (stack.size() > 0) {
	  data = stack.back();
	  assign(&data,var.child(varCount-2-ptr),rhs);
	  rhs = data;
	  stack.pop_back();
	  ptr++;
	}
      }
      assign(ptr,var.child(1),rhs);
    }
    if (printIt) {
      outputMessage(name);
      outputMessage(" = \n");
      displayArray(*ptr);
    }
  }
  if (index < s.size())
    warningMessage("one or more outputs not assigned in call.");
}

int getArgumentIndex(StringVector list, QString t) {
  bool foundArg = false;
  QString q;
  int i = 0;
  while (i<list.size() && !foundArg) {
    q = list[i];
    if (q[0] == '&')
      q.remove(0,1);
    foundArg = (q == t);
    if (!foundArg) i++;
  }
  if (foundArg)
    return i;
  else
    return -1;
}

//!
//@Module FUNCTION Function Declarations
//@@Section FUNCTIONS
//@@Usage
//There are several forms for function declarations in FreeMat.
//The most general syntax for a function declaration is the 
//following:
//@[
//  function [out_1,...,out_M,varargout] = fname(in_1,...,in_N,varargin)
//@]
//where @|out_i| are the output parameters, @|in_i| are the input
//parameters, and @|varargout| and @|varargin| are special keywords
//used for functions that have variable inputs or outputs.  For 
//functions with a fixed number of input or output parameters, the 
//syntax is somewhat simpler:
//@[
//  function [out_1,...,out_M] = fname(in_1,...,in_N)
//@]
//Note that functions that have no return arguments can omit
//the return argument list (of @|out_i|) and the equals sign:
//@[
//  function fname(in_1,...,in_N)
//@]
//Likewise, a function with no arguments can eliminate the list
//of parameters in the declaration:
//@[
//  function [out_1,...,out_M] = fname
//@]
//Functions that return only a single value can omit the brackets
//@[
//  function out_1 = fname(in_1,...,in_N)
//@]
//
//In the body of the function @|in_i| are initialized with the
//values passed when the function is called.  Also, the function
//must assign values for @|out_i| to pass values to the caller.
//Note that by default, FreeMat passes arguments by value, meaning
//that if we modify the contents of @|in_i| inside the function,
//it has no effect on any variables used by the caller.  Arguments
//can be passed by reference by prepending an ampersand @|&|
//before the name of the input, e.g.
//@[
//  function [out1,...,out_M] = fname(in_1,&in_2,in_3,...,in_N)
//@]
//in which case @|in_2| is passed by reference and not by value.
//Also, FreeMat works like @|C| in that the caller does not have
//to supply the full list of arguments.  Also, when @|keywords|
//(see help @|keywords|) are used, an arbitrary subset of the 
//parameters may be unspecified. To assist in deciphering 
//the exact parameters that were passed,
//FreeMat also defines two variables inside the function context:
//@|nargin| and @|nargout|, which provide the number of input
//and output parameters of the caller, respectively. See help for 
//@|nargin| and @|nargout| for more details.  In some 
//circumstances, it is necessary to have functions that
//take a variable number of arguments, or that return a variable
//number of results.  In these cases, the last argument to the 
//parameter list is the special argument @|varargin|.  Inside
//the function, @|varargin| is a cell-array that contains
//all arguments passed to the function that have not already
//been accounted for.  Similarly, the function can create a
//cell array named @|varargout| for variable length output lists.
//See help @|varargin| and @|varargout| for more details.
//
//The function name @|fname| can be any legal FreeMat identifier.
//Functions are stored in files with the @|.m| extension.  Note
//that the name of the file (and not the function name @|fname| 
//used in the declaration) is how the function appears in FreeMat.
//So, for example, if the file is named @|foo.m|, but the declaration
//uses @|bar| for the name of the function, in FreeMat, it will 
//still appear as function @|foo|.  Note that this is only true
//for the first function that appears in a @|.m| file.  Additional
//functions that appear after the first function are known as
//@|helper functions| or @|local| functions.  These are functions that
//can only be called by other functions in the same @|.m| file.  Furthermore
//the names of these helper functions are determined by their declaration
//and not by the name of the @|.m| file.  An example of using
//helper functions is included in the examples.
//
//Another important feature of functions, as opposed to, say @|scripts|,
//is that they have their own @|scope|.  That means that variables
//defined or modified inside a function do not affect the scope of the
//caller.  That means that a function can freely define and use variables
//without unintentionally using a variable name reserved elsewhere.  The
//flip side of this fact is that functions are harder to debug than
//scripts without using the @|keyboard| function, because the intermediate
//calculations used in the function are not available once the function
//exits.
//@@Examples
//Here is an example of a trivial function that adds its
//first argument to twice its second argument:
//@{ addtest.m
//function c = addtest(a,b)
//  c = a + 2*b;
//@}
//@<
//addtest(1,3)
//addtest(3,0)
//@>
//Suppose, however, we want to replace the value of the first 
//argument by the computed sum.  A first attempt at doing so
//has no effect:
//@{ addtest2.m
//function addtest2(a,b)
//  a = a + 2*b;
//@}
//@<
//arg1 = 1
//arg2 = 3
//addtest2(arg1,arg2)
//arg1
//arg2
//@>
//The values of @|arg1| and @|arg2| are unchanged, because they are
//passed by value, so that any changes to @|a| and @|b| inside 
//the function do not affect @|arg1| and @|arg2|.  We can change
//that by passing the first argument by reference:
//@{ addtest3.m
//function addtest3(&a,b)
//  a = a + 2*b
//@}
//Note that it is now illegal to pass a literal value for @|a| when
//calling @|addtest3|:
//@<1
//addtest3(3,4)
//addtest3(arg1,arg2)
//arg1
//arg2
//@>
//The first example fails because we cannot pass a literal like the
//number @|3| by reference.  However, the second call succeeds, and
//note that @|arg1| has now changed.  Note: please be careful when
//passing by reference - this feature is not available in MATLAB
//and you must be clear that you are using it.
//
//As variable argument and return functions are covered elsewhere,
//as are keywords, we include one final example that demonstrates
//the use of helper functions, or local functions, where
//multiple function declarations occur in the same file.
//@{ euclidlength.m
//function y = foo(x,y)
//  square_me(x);
//  square_me(y);
//  y = sqrt(x+y);
//
//function square_me(&t)
//  t = t^2;
//@}
//@<
//euclidlength(3,4)
//euclidlength(2,0)
//@>
//@@Tests
//@{ test_call1.m
//% test the multiple left-hand side call with a pre-existing cell-array
//function test_val = test_call1
//d1 = {1,2};
//d2 = {5};
//d3 = {6};
//[d1,d2,d3] = test_call1_assist;
//test_val = test(d1 == 1) & test(d2 == 4) & test(d3 == 3);
//
//function [a,b,c] = test_call1_assist
//a = 1;
//b = 4;
//c = 3;
//@}
//@{ test_call2.m
//% test the multiple left-hand side call with a pre-existing cell-array
//function test_val = test_call2
//[d1{1:3}] = test_call2_assist;
//test_val = test(d1{1} == 1) & test(d1{2} == 4) & test(d1{3} == 3);
//
//function [a,b,c] = test_call2_assist
//a = 1;
//b = 4;
//c = 3;
//@}
//@{ test_call3.m
//% test calls with pass-by-reference
//function test_val = test_call3
//d1 = 3;
//test_call3_assist(d1);
//test_val = test(d1 == 1);
//
//function test_call3_assist(&x)
//x = 1;
//@}
//@{ test_call4.m
//% test calls with pass-by-reference
//function test_val = test_call4
//d1.foo = 3;
//test_call4_assist(d1.foo);
//test_val = test(d1.foo == 1);
//
//function test_call4_assist(&x)
//x = 1;
//@}
//@{ test_fcall1.m
//% Check to RHS function calls with multiple return values (b989865)
//function test_val = test_fcall1
//try 
//b = 1:10;
//B = addthem(min(b),max(b));
//test_val = test(B == 11);
//catch
//test_val = 0;
//end
//
//function c = addthem(a,b)
//c = a + b;
//@}
//@{ test_fcall2.m
//% Check function declaration with two return args
//function test_val = test_fcall2
//try
//   [a,b] = fc2fuc(1,3,2);
//   test_val = test(a == 4);
//catch
//   test_val = 0;
//end
//
//function [a b] = fc2fuc(w,x,y)
//  a = w+x;
//  b = y;
//@}
//!


//!
//@Module ANONYMOUS Anonymous Functions
//@@Section FUNCTIONS
//@@Usage
//Anonymous functions are simple, nameless functions that can be defined
//anywhere (in a script, function, or at the prompt).  They are intended
//to supplant @|inline| functions.  The syntax for an anonymous function
//is simple:
//@[
//   y = @(arg1,arg2,...,argn) expression
//@]
//where @|arg1,arg2,...,argn| is a list of valid identifiers that define
//the arguments to the function, and @|expression| is the expression to
//compute in the function.  The returned value @|y| is a function handle
//for the anonymous function that can then be used to evaluate the expression.
//Note that @|y| will capture the value of variables that are not indicated
//in the argument list from the current scope or workspace at the time
//it is defined.  So, for example, consider the simple anonymous function
//definition
//@[
//   y = @(x) a*(x+b)
//@]
//In order for this definition to work, the variables @|a| and @|b| need to
//be defined in the current workspace.  Whatever value they have is captured
//in the function handle @|y|.  To change the values of @|a| and @|b| in the
//anonymous function, you must recreate the handle using another call.  See
//the examples section for more information.  In order to use the anonymous
//function, you can use it just like any other function handle.  For example,
//@[
//   p = y(3)
//   p = y()
//   p = feval(y,3)
//@]
//are all examples of using the @|y| anonymous function to perform a calculation.
//@@Examples
//Here are some examples of using an anonymous function
//@<
//a = 2; b = 4;    % define a and b (slope and intercept)
//y = @(x) a*x+b   % create the anonymous function
//y(2)             % evaluate it for x = 2
//a = 5; b = 7;    % change a and b
//y(2)             % the value did not change!  because a=2,b=4 are captured in y
//y = @(x) a*x+b   % recreate the function
//y(2)             % now the new values are used
//@>
//!

//!
//@Module KEYWORDS Function Keywords
//@@Section FUNCTIONS
//@@Usage
//A feature of IDL that FreeMat has adopted is a modified
//form of @|keywords|.  The purpose of @|keywords| is to 
//allow you to call a function with the arguments to the
//function specified in an arbitrary order.  To specify
//the syntax of @|keywords|, suppose there is a function 
//with prototype
//@[
//  function [out_1,...,out_M] = foo(in_1,...,in_N)
//@]
//Then the general syntax for calling function @|foo| using keywords
//is
//@[
//  foo(val_1, val_2, /in_k=3)
//@]
//which is exactly equivalent to
//@[
//  foo(val_1, val_2, [], [], ..., [], 3),
//@]
//where the 3 is passed as the k-th argument, or alternately,
//@[
//  foo(val_1, val_2, /in_k)
//@]
//which is exactly equivalent to
//@[
//  foo(val_1, val_2, [], [], ..., [], logical(1)),
//@]
//Note that you can even pass reference arguments using keywords.
//@@Example
//The most common use of keywords is in controlling options for
//functions.  For example, the following function takes a number
//of binary options that control its behavior.  For example,
//consider the following function with two arguments and two
//options.  The function has been written to properly use and
//handle keywords.  The result is much cleaner than the MATLAB
//approach involving testing all possible values of @|nargin|,
//and forcing explicit empty brackets for don't care parameters.
//@{ keyfunc.m
//function c = keyfunc(a,b,operation,printit)
//  if (~isset('a') | ~isset('b')) 
//    error('keyfunc requires at least the first two 2 arguments'); 
//  end;
//  if (~isset('operation'))
//    % user did not define the operation, default to '+'
//    operation = '+';
//  end
//  if (~isset('printit'))
//    % user did not specify the printit flag, default is false
//    printit = 0;
//  end
//  % simple operation...
//  eval(['c = a ' operation ' b;']);
//  if (printit) 
//    printf('%f %s %f = %f\n',a,operation,b,c);
//  end
//@}
//Now some examples of how this function can be called using
//@|keywords|.
//@<1
//keyfunc(1,3)                % specify a and b, defaults for the others
//keyfunc(1,3,/printit)       % specify printit is true
//keyfunc(/operation='-',2,3) % assigns a=2, b=3
//keyfunc(4,/operation='*',/printit) % error as b is unspecified
//@>
//!

//!
//@Module VARARGIN Variable Input Arguments
//@@Section FUNCTIONS
//@@Usage
//FreeMat functions can take a variable number of input arguments
//by setting the last argument in the argument list to @|varargin|.
//This special keyword indicates that all arguments to the
//function (beyond the last non-@|varargin| keyword) are assigned
//to a cell array named @|varargin| available to the function.
//Variable argument functions are usually used when writing 
//driver functions, i.e., functions that need to pass arguments
//to another function.  The general syntax for a function that
//takes a variable number of arguments is
//@[
//  function [out_1,...,out_M] = fname(in_1,..,in_M,varargin)
//@]
//Inside the function body, @|varargin| collects the arguments 
//to @|fname| that are not assigned to the @|in_k|.
//@@Example
//Here is a simple wrapper to @|feval| that demonstrates the
//use of variable arguments functions.
//@{ wrapcall.m
//function wrapcall(fname,varargin)
//  feval(fname,varargin{:});
//@}
//Now we show a call of the @|wrapcall| function with a number
//of arguments
//@<
//wrapcall('printf','%f...%f\n',pi,e)
//@>
//A more serious driver routine could, for example, optimize
//a one dimensional function that takes a number of auxilliary
//parameters that are passed through @|varargin|.
//!

//!
//@Module VARARGOUT Variable Output Arguments
//@@Section FUNCTIONS
//@@Usage
//FreeMat functions can return a variable number of output arguments
//by setting the last argument in the argument list to @|varargout|.
//This special keyword indicates that the number of return values
//is variable.  The general syntax for a function that returns
//a variable number of outputs is
//@[
//  function [out_1,...,out_M,varargout] = fname(in_1,...,in_M)
//@]
//The function is responsible for ensuring that @|varargout| is
//a cell array that contains the values to assign to the outputs
//beyond @|out_M|.  Generally, variable output functions use
//@|nargout| to figure out how many outputs have been requested.
//@@Example
//This is a function that returns a varying number of values
//depending on the value of the argument.
//@{ varoutfunc.m
//function [varargout] = varoutfunc
//  switch(nargout)
//    case 1
//      varargout = {'one of one'};
//    case 2
//      varargout = {'one of two','two of two'};
//    case 3
//      varargout = {'one of three','two of three','three of three'};
//  end
//@}
//Here are some examples of exercising @|varoutfunc|:
//@<
//[c1] = varoutfunc
//[c1,c2] = varoutfunc
//[c1,c2,c3] = varoutfunc
//@>
//!

//!
//@Module SCRIPT Script Files
//@@Section FUNCTIONS
//@@Usage
//A script is a sequence of FreeMat commands contained in a
//@|.m| file.  When the script is called (via the name of the
//file), the effect is the same as if the commands inside the
//script file were issued one at a time from the keyboard.
//Unlike @|function| files (which have the same extension,
//but have a @|function| declaration), script files share
//the same environment as their callers.  Hence, assignments,
//etc, made inside a script are visible to the caller (which
//is not the case for functions.
//@@Example
//Here is an example of a script that makes some simple 
//assignments and @|printf| statements.
//@{ tscript.m
//a = 13;
//printf('a is %d\n',a);
//b = a + 32
//@}
//If we execute the script and then look at the defined variables
//@<
//tscript
//who
//@>
//we see that @|a| and @|b| are defined appropriately.
//!

//!
//@Module SPECIAL Special Calling Syntax
//@@Section FUNCTIONS
//@@Usage
//To reduce the effort to call certain functions, FreeMat supports
//a special calling syntax for functions that take string arguments.
//In particular, the three following syntaxes are equivalent, with
//one caveat:
//@[
//   functionname('arg1','arg2',...,'argn')
//@]
//or the parenthesis and commas can be removed
//@[
//   functionname 'arg1' 'arg2' ... 'argn'
//@]
//The quotes are also optional (providing, of course, that the
//argument strings have no spaces in them)
//@[
//   functionname arg1 arg2 ... argn
//@]
//This special syntax enables you to type @|hold on| instead of
//the more cumbersome @|hold('on')|.  The caveat is that FreeMat
//currently only recognizes the special calling syntax as the
//first statement on a line of input.  Thus, the following construction
//@[
//  for i=1:10; plot(vec(i)); hold on; end
//@]
//would not work.  This limitation may be removed in a future
//version.
//@@Example
//Here is a function that takes two string arguments and
//returns the concatenation of them.
//@{ strcattest.m
//function strcattest(str1,str2)
//  str3 = [str1,str2];
//  printf('str1 = %s, str2 = %s, str3 = %s\n',str1,str2,str3);
//@}
//We call @|strcattest| using all three syntaxes.
//@<
//strcattest('hi','ho')
//strcattest 'hi' 'ho'
//strcattest hi ho
//@>
//!
void Interpreter::collectKeywords(const Tree & q, ArrayVector &keyvals,
				  TreeList &keyexpr, StringVector &keywords) {
  // Search for the keyword uses - 
  // To handle keywords, we make one pass through the arguments,
  // recording a list of keywords used and using ::expression to
  // evaluate their values. 
  for (int index=0;index < q.numChildren();index++) {
    if (q.child(index).is(TOK_KEYWORD)) {
      keywords.push_back(q.child(index).first().text());
      if (q.child(index).numChildren() > 1) {
	keyvals.push_back(expression(q.child(index).second()));
	keyexpr.push_back(q.child(index).second());
      } else {
	keyvals.push_back(Array(bool(true)));
	keyexpr.push_back(Tree());
      }
    }
  }
}

int* Interpreter::sortKeywords(ArrayVector &m, StringVector &keywords,
			       StringVector arguments, ArrayVector keyvals) {
  // If keywords were used, we have to permute the
  // entries of the arrayvector to the correct order.
  int *keywordNdx = new int[keywords.size()];
  int maxndx;
  maxndx = 0;
  // Map each keyword to an argument number
  for (int i=0;i<keywords.size();i++) {
    int ndx;
    ndx = getArgumentIndex(arguments,keywords[i]);
    if (ndx == -1)
      throw Exception("out-of-order argument /" + keywords[i] + " is not defined in the called function!");
    keywordNdx[i] = ndx;
    if (ndx > maxndx) maxndx = ndx;
  }
  // Next, we have to determine how many "holes" there are
  // in the argument list - we get the maximum list
  int holes;
  holes = maxndx + 1 - keywords.size();
  // At this point, holes is the number of missing arguments
  // If holes > m.size(), then the total number of arguments
  // is just maxndx+1.  Otherwise, its 
  // maxndx+1+(m.size() - holes)
  int totalCount;
  if (holes > m.size())
    totalCount = maxndx+1;
  else
    totalCount = maxndx+1+(m.size() - holes);
  // Next, we allocate a vector to hold the values
  ArrayVector toFill;
  for (int i=0;i<totalCount;i++)
    toFill.push_back(Array());
  //  ArrayVector toFill(totalCount);
  bool *filled = new bool[totalCount];
  int *argTypeMap = new int[totalCount];
  for (int i=0;i<totalCount;i++) {
    filled[i] = false;
    argTypeMap[i] = -1;
  }
  // Finally...
  // Copy the keyword values in
  for (int i=0;i<keywords.size();i++) {
    toFill[keywordNdx[i]] = keyvals[i];
    filled[keywordNdx[i]] = true;
    argTypeMap[keywordNdx[i]] = i;
  }
  // Fill out the rest of the values from m
  int n = 0;
  int p = 0;
  while (n < m.size()) {
    if (!filled[p]) {
      toFill[p] = m[n];
      filled[p] = true;
      argTypeMap[p] = -2;
      n++;
    } 
    p++;
  }
  // Finally, fill in empty matrices for the
  // remaining arguments
  for (int i=0;i<totalCount;i++)
    if (!filled[i])
      toFill[i] = EmptyConstructor();
  // Clean up
  delete[] filled;
  delete[] keywordNdx;
  // Reassign
  m = toFill;
  return argTypeMap;
}

// arguments is exactly what it should be - the vector of arguments
// m is vector of argument values
// keywords is the list of values passed as keywords
// keyexpr is the   
void Interpreter::handlePassByReference(Tree q, StringVector arguments,
					ArrayVector m,StringVector keywords, 
					TreeList keyexpr, int* argTypeMap) {
  Tree p;
  // M functions can modify their arguments
  int maxsearch = m.size(); 
  if (maxsearch > arguments.size()) maxsearch = arguments.size();
  int qindx = 0;
  for (int i=0;i<maxsearch;i++) {
    // Was this argument passed out of order?
    if ((keywords.size() > 0) && (argTypeMap[i] == -1)) continue;
    if ((keywords.size() > 0) && (argTypeMap[i] >=0)) {
      p = keyexpr[argTypeMap[i]];
    } else {
      p = q.second().child(qindx);
      qindx++;
      if (qindx >= q.second().numChildren())
	qindx = q.second().numChildren()-1;
    }
    QString args(arguments[i]);
    if (args[0] == '&') {
      args.remove(0,1);
      // This argument was passed by reference
      if (!p.valid() || !(p.is(TOK_VARIABLE)))
	throw Exception("Must have lvalue in argument passed by reference");
      assignment(p,false,m[i]);
    }
  }
}

//Test
void Interpreter::functionExpression(const Tree & t, 
				     int narg_out, 
				     bool outputOptional,
				     ArrayVector &output) {
  ArrayVector m, n;
  StringVector keywords;
  ArrayVector keyvals;
  TreeList keyexpr;
  FuncPtr funcDef;
  int* argTypeMap;
  // Because of the introduction of user-defined classes, we have to 
  // first evaluate the keywords and the arguments, before we know
  // which function to call.
  // First, check for arguments
  if ((t.numChildren()>=2) && t.second().is(TOK_PARENS)) {
    // Collect keywords
    collectKeywords(t.second(),keyvals,keyexpr,keywords);
    // Evaluate function arguments
    try {
      const Tree & s(t.second());
      for (int p=0;p<s.numChildren();p++)
	multiexpr(s.child(p),m);
    } catch (Exception &e) {
      // Transmute the error message about illegal use of ':'
      // into one about undefined variables.  Its crufty,
      // but it works.
      if (e.matches("Illegal use of the ':' operator"))
	throw Exception("Undefined variable " + t.text());
      else
	throw;
    }
  } 
  // Now that the arguments have been evaluated, we have to 
  // find the dominant class
  if (!lookupFunction(t.first().text(),funcDef,m))
    throw Exception("Undefined function or variable " + 
		    t.first().text());
  if (funcDef->updateCode(this)) refreshBreakpoints();
  if (funcDef->scriptFlag) {
    if (t.numChildren()>=2)
      throw Exception(QString("Cannot use arguments in a call to a script."));
    if ((narg_out > 0) && !outputOptional)
      throw Exception(QString("Cannot assign outputs in a call to a script."));
    context->pushScope(((MFunctionDef*)funcDef)->fileName,
		       ((MFunctionDef*)funcDef)->name,false);
    context->setScopeActive(false);
    block(((MFunctionDef*)funcDef)->code);
    if (context->scopeStepTrap() >= 1) {
      tracetrap = 1;
      tracecurrentline = 0;
      warningMessage("dbstep beyond end of script " + context->scopeDetailString() +
		     " -- setting single step mode\n");
      context->setScopeStepTrap(0);
    }
    context->popScope();
  } else {
    // We can now adjust the keywords (because we know the argument list)
    // Apply keyword mapping
    if (!keywords.empty()) 
      argTypeMap = sortKeywords(m,keywords,funcDef->arguments,keyvals);
    else
      argTypeMap = NULL;
    if ((funcDef->inputArgCount() >= 0) && 
	(m.size() > funcDef->inputArgCount()))
      throw Exception(QString("Too many inputs to function ")+t.first().text());
    if ((funcDef->outputArgCount() >= 0) && 
	(narg_out > funcDef->outputArgCount() && !outputOptional))
      throw Exception(QString("Too many outputs to function ")+t.first().text());
    n = doFunction(funcDef,m,narg_out);
    // Check for any pass by reference
    if (t.hasChildren() && (funcDef->arguments.size() > 0)) 
      handlePassByReference(t,funcDef->arguments,m,keywords,keyexpr,argTypeMap);
  }
  // Some routines (e.g., min and max) will return more outputs
  // than were actually requested... so here we have to trim 
  // any elements received that we didn't ask for.
  // preserve one output if we were called as an expression (for ans)
  if (outputOptional) narg_out = (narg_out == 0) ? 1 : narg_out;
  while (n.size() > narg_out)
    n.pop_back();
  output += n;
}


void Interpreter::toggleBP(QString fname, int lineNumber) {
  if (isBPSet(fname,lineNumber)) {
    QString fname_string(fname);
    for (int i=0;i<bpStack.size();i++) 
      if ((bpStack[i].cname == fname_string) &&
	  (LineNumber(bpStack[i].tokid) == lineNumber)) {
	deleteBreakpoint(bpStack[i].number);
	return;
      }
  } else {
    addBreakpoint(fname,lineNumber);
  }    
}

MFunctionDef* Interpreter::lookupFullPath(QString fname) {
  StringVector allFuncs(context->listAllFunctions());
  FuncPtr val;
  for (int i=0;i<allFuncs.size();i++) {
    bool isFun = context->lookupFunction(allFuncs[i],val);
    if (!isFun || !val) return NULL;
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      if (mptr->fileName ==  fname) return mptr;
    }
  }
  return NULL;
}

static int bpList = 1;
// Add a breakpoint - name is used to track to a full filename
void Interpreter::addBreakpoint(QString name, int line) {
  FuncPtr val;
  // Map the name argument to a full file name.
  QString fullFileName;
  if (context->lookupFunction(name,val) && (val->type() == FM_M_FUNCTION))
    fullFileName = ((MFunctionDef*) val)->fileName;
  else
    fullFileName = name;
  // Get a list of all functions
  StringVector allFuncs(context->listAllFunctions());
  // We make one pass through the functions, and update 
  // those functions that belong to the given filename
  for (int i=0;i<allFuncs.size();i++) {
    bool isFun = context->lookupFunction(allFuncs[i],val);
    if (!isFun || !val) throw Exception("Cannot add breakpoint to " + name + " :  it does not appear to be a valid M file.");
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr = (MFunctionDef *) val;
      if (mptr->fileName == fullFileName)
	mptr->updateCode(this);
    }
  }
  // Refresh the list of all functions
  allFuncs = context->listAllFunctions();
  // Search through the list for any function defined  - for each function,
  // record the line number closest to it
  MemBlock<int> line_dist_block(allFuncs.size());
  int *line_dist = &line_dist_block;
  for (int i=0;i<allFuncs.size();i++) line_dist[i] = 2*max_line_count;
  for (int i=0;i<allFuncs.size();i++) {
    bool isFun = context->lookupFunction(allFuncs[i],val);
    if (!isFun || !val) throw Exception("Cannot add breakpoint to " + name + " :  it does not appear to be a valid M file.");
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr = (MFunctionDef *) val;
      if (mptr->fileName == fullFileName) {
	try {
	  int dline = mptr->ClosestLine(line);
	  line_dist[i] = dline;
	} catch (Exception& e) {
	}
      }
    }
  }
  // Second pass through it - find the function with a line number closest to the
  // desired one, but not less than it
  int best_func = -1;
  int best_dist = 2*max_line_count;
  for (int i=0;i<allFuncs.size();i++) {
    if ((line_dist[i] >= line) && ((line_dist[i]-line) < best_dist)) {
      best_func = i;
      best_dist = line_dist[i]-line;
    }
  }
  if (best_dist > max_line_count)
//    warningMessage(QString("Cannot set breakpoint at line ")+line+" of file "+name + ".  \r\nThis can be caused by an illegal line number, or a function that is not on the path or in the current directory.");
    emit IllegalLineOrCurrentPath(name, line);
  else {
    addBreakpoint(stackentry(fullFileName,allFuncs[best_func],best_dist+line,bpList++));
  }
}

bool Interpreter::isBPSet(QString fname, int lineNumber) {
  for (int i=0;i<bpStack.size();i++) 
    if ((bpStack[i].cname == fname) &&
	(LineNumber(bpStack[i].tokid) == lineNumber)) return true;
  return false;
}

// called by editor
bool Interpreter::isInstructionPointer(QString fname, int lineNumber) {
  if (!InCLI) return false;
  ParentScopeLocker lock(context);
  QString filename(context->scopeName());
  int token(context->scopeTokenID());
  return ((fname == filename) && ((lineNumber == LineNumber(token)) ||
				  ((lineNumber == 1) && (LineNumber(token) == 0))));
}

void Interpreter::listBreakpoints() {
  for (int i=0;i<bpStack.size();i++) {
    QString buffer = QString("%1   %2 line %3\n").arg(bpStack[i].number)
      .arg(bpStack[i].cname).arg(LineNumber(bpStack[i].tokid));
    outputMessage(buffer);
  }
}

void Interpreter::deleteBreakpoint(int number) {
  for (int i=0;i<bpStack.size();i++) 
    if (bpStack[i].number == number) {
      bpStack.remove(i);
      emit RefreshBPLists();
      return;
    } 
  warningMessage("Unable to delete specified breakpoint (does not exist)");
  emit RefreshBPLists();
  return;
}

void Interpreter::stackTrace(int skiplevels) {
  bool firstline = true;
  int depth = context->scopeDepth();
  context->bypassScope(skiplevels);
  for (int i=0;i<(depth-skiplevels);i++) {
    if ((context->scopeName() == "keyboard") &&
	(context->scopeDetailString() == "keyboard")) {
      context->bypassScope(1);
      continue;
    }
    if (firstline) {
      firstline = false;
    } else 
      outputMessage(QString("    "));
    outputMessage(QString("In ") + context->scopeName() + "("
		  + context->scopeDetailString() + ")");
    int line = int(LineNumber(context->scopeTokenID()));
    if (line > 0)
      outputMessage(QString(" at line " +
			    QString().setNum(LineNumber(context->scopeTokenID()))));
    outputMessage("\r\n");
    context->bypassScope(1);
  }
  context->restoreScope(depth);
}

bool Interpreter::inMethodCall(QString classname) {
  if (context->scopeDetailString().isEmpty()) return false;
  if (context->scopeDetailString()[0] != '@') return false;
  return (context->scopeDetailString().mid(1,classname.size()) == classname);
}

bool Interpreter::lookupFunction(QString funcName, FuncPtr& val) {
  ArrayVector dummy;
  return(lookupFunction(funcName,val,dummy));
}

bool IsNestedName(QString basename) {
  return (basename.lastIndexOf("/") >= 0);
}

QString StripNestLevel(QString basename) {
  int ndx = basename.lastIndexOf("/");
  if (ndx >= 0)
    basename.remove(ndx,basename.size());
  else
    basename = "";
  return basename;
}

// Look up a function by name.  Use the arguments (if available) to assist
// in resolving method calls for objects
bool Interpreter::lookupFunction(QString funcName, FuncPtr& val, 
				 ArrayVector &args, bool disableOverload) {
  int passcount = 0;
  while(passcount < 2) {
    // This is the order for function dispatch according to the Matlab manual
    // Nested functions - not explicitly listed in the Matlab manual, but 
    // I figure they have the highest priority in the current scope.
    if (isMFile(context->scopeName()) &&
	(context->lookupFunction(NestedMangleName(context->scopeDetailString(),funcName),val)))
      return true;
    if (InCLI && isMFile(context->activeScopeName()) &&
    	(context->lookupFunction(NestedMangleName(context->activeScopeDetailString(),funcName),val)))
      return true;
    // Not a nested function of the current scope.  We have to look for nested
    // functions of all parent scopes. Sigh.
    if (context->isCurrentScopeNested()) {
      QString basename = context->scopeDetailString();
      while (!basename.isEmpty()) {
	if (context->lookupFunction(NestedMangleName(basename,funcName),val))
	  return true;
	basename = StripNestLevel(basename);
      }
    }
    // Subfunctions
    if (inMFile() && 
	(context->lookupFunction(getLocalMangledName(funcName),val)))
      return true;
    // Private functions
    // Not sure if you have to be an M-file in the current directory
    // to access a private function...
    if (context->lookupFunction(getPrivateMangledName(funcName),val))
      return true;
    // Class constructor functions
    if (context->lookupFunction(ClassMangleName(funcName,funcName),val))
      return true;
    if (!(disableOverload || stopoverload)) {
      // Look for a class method
      // Are any of the arguments classes?
      bool anyClasses = false;
      int i=0;
      while ((!anyClasses) && (i < args.size())) {
	anyClasses = args[i].isUserClass();
	if (!anyClasses) i++;
      }
      // Yes, try and resolve the call to a method
      if (anyClasses && ClassResolveFunction(this,args[i],funcName,val))
	return true;
    }
    if (context->lookupFunction(funcName,val)) return true;
    if (passcount == 0)
      rescanPath();
    passcount++;
  }
  return false;
}

//!
//@Module FUNCTIONHANDLES Function Handles
//@@Section VARIABLES
//@@Usage
//Starting with version 1.11, FreeMat now supports @|function handles|,
//or @|function pointers|.  A @|function handle| is an alias for a function
//or script that is stored in a variable.  First, the way to assign
//a function handle is to use the notation
//@[
//    handle = @func
//@]
//where @|func| is the name to point to.  The function @|func| must exist
//at the time we make the call.  It can be a local function (i.e., a
//subfunction).  To use the @|handle|, we can either pass it to @|feval|
//via 
//@[
//   [x,y] = feval(handle,arg1,arg2).
//@]
//Alternately, you can the function directly using the notation
//@[
//   [x,y] = handle(arg1,arg2)
//@]
//!

//!
//@Module INDEXING Indexing Expressions
//@@Section VARIABLES
//@@Usage
//There are three classes of indexing expressions available 
//in FreeMat: @|()|, @|{}|, and @|.|  Each is explained below
//in some detail, and with its own example section.
//@@Array Indexing
//We start with array indexing @|()|,
//which is the most general indexing expression, and can be
//used on any array.  There are two general forms for the 
//indexing expression - the N-dimensional form, for which 
//the general syntax is
//@[
//  variable(index_1,index_2,...,index_n)
//@]
//and the vector form, for which the general syntax is
//@[
//  variable(index)
//@]
//Here each index expression is either a scalar, a range
//of integer values, or the special token @|:|, which is
//shorthand for @|1:end|.  The keyword @|end|, when included
//in an indexing expression, is assigned the length of the 
//array in that dimension.  The concept is easier to demonstrate
//than explain.  Consider the following examples:
//@<
//A = zeros(4)
//B = float(randn(2))
//A(2:3,2:3) = B
//@>
//Here the array indexing was used on the left hand side only.
//It can also be used for right hand side indexing, as in
//@<
//C = A(2:3,1:end)
//@>
//Note that we used the @|end| keyword to avoid having to know
//that @|A| has 4 columns.  Of course, we could also use the 
//@|:| token instead:
//@<
//C = A(2:3,:)
//@>
//An extremely useful example of @|:| with array indexing is for
//slicing.  Suppose we have a 3-D array, that is @|2 x 2 x 3|,
//and we want to set the middle slice:
//@<
//D = zeros(2,2,3)
//D(:,:,2) = int32(10*rand(2,2))
//@>
//In another level of nuance, the assignment expression will
//automatically fill in the indexed rectangle on the left using
//data from the right hand side, as long as the lengths match.
//So we can take a vector and roll it into a matrix using this
//approach:
//@<
//A = zeros(4)
//v = [1;2;3;4]
//A(2:3,2:3) = v
//@>
//
//The N-dimensional form of the variable index is limited
//to accessing only (hyper-) rectangular regions of the 
//array.  You cannot, for example, use it to access only
//the diagonal elements of the array.  To do that, you use
//the second form of the array access (or a loop).  The
//vector form treats an arbitrary N-dimensional array as though
//it were a column vector.  You can then access arbitrary 
//subsets of the arrays elements (for example, through a @|find|
//expression) efficiently.  Note that in vector form, the @|end|
//keyword takes the meaning of the total length of the array
//(defined as the product of its dimensions), as opposed to the
//size along the first dimension.
//@@Cell Indexing
//The second form of indexing operates, to a large extent, in
//the same manner as the array indexing, but it is by no means
//interchangable.  As the name implies, @|cell|-indexing applies
//only to @|cell| arrays.  For those familiar with @|C|, cell-
//indexing is equivalent to pointer derefencing in @|C|.  First,
//the syntax:
//@[
//  variable{index_1,index_2,...,index_n}
//@]
//and the vector form, for which the general syntax is
//@[
//  variable{index}
//@]
//The rules and interpretation for N-dimensional and vector indexing
//are identical to @|()|, so we will describe only the differences.
//In simple terms, applying @|()| to a cell-array returns another
//cell array that is a subset of the original array.  On the other
//hand, applying @|{}| to a cell-array returns the contents of that
//cell array.  A simple example makes the difference quite clear:
//@<
//A = {1, 'hello', [1:4]}
//A(1:2)
//A{1:2}
//@>
//You may be surprised by the response to the last line.  The output
//is multiple assignments to @|ans|!.  The output of a cell-array
//dereference can be used anywhere a list of expressions is required.
//This includes arguments and returns for function calls, matrix
//construction, etc.  Here is an example of using cell-arrays to pass
//parameters to a function:
//@<
//A = {[1,3,0],[5,2,7]}
//max(A{1:end})
//@>
//And here, cell-arrays are used to capture the return.
//@<
//[K{1:2}] = max(randn(1,4))
//@>
//Here, cell-arrays are used in the matrix construction process:
//@<
//C = [A{1};A{2}]
//@>
//Note that this form of indexing is used to implement variable
//length arguments to function.  See @|varargin| and @|varargout|
//for more details.
//@@Structure Indexing
//The third form of indexing is structure indexing.  It can only
//be applied to structure arrays, and has the general syntax
//@[
//  variable.fieldname
//@]
//where @|fieldname| is one of the fields on the structure.  Note that
//in FreeMat, fields are allocated dynamically, so if you reference
//a field that does not exist in an assignment, it is created automatically
//for you.  If variable is an array, then the result of the @|.| 
//reference is an expression list, exactly like the @|{}| operator.  Hence,
//we can use structure indexing in a simple fashion:
//@<
//clear A
//A.color = 'blue'
//B = A.color
//@>
//Or in more complicated ways using expression lists for function arguments
//@<
//clear A
//A(1).maxargs = [1,6,7,3]
//A(2).maxargs = [5,2,9,0]
//max(A.maxargs)
//@>
//or to store function outputs
//@<
//clear A
//A(1).maxreturn = [];
//A(2).maxreturn = [];
//[A.maxreturn] = max(randn(1,4))
//@>
//FreeMat now also supports the so called dynamic-field indexing 
//expressions.  In this mode, the fieldname is supplied through 
//an expression instead of being explicitly provided.  For example,
//suppose we have a set of structure indexed by color,
//@<
//x.red = 430;
//x.green = 240;
//x.blue = 53;
//x.yello = 105
//@>
//Then we can index into the structure @|x| using a dynamic field
//reference:
//@<
//y = 'green'
//a = x.(y)
//@>
//Note that the indexing expression has to resolve to a string for
//dynamic field indexing to work.
//@@Complex Indexing
//The indexing expressions described above can be freely combined
//to affect complicated indexing expressions.  Here is an example
//that exercises all three indexing expressions in one assignment.
//@<
//Z{3}.foo(2) = pi
//@>
//From this statement, FreeMat infers that Z is a cell-array of 
//length 3, that the third element is a structure array (with one
//element), and that this structure array contains a field named
//'foo' with two double elements, the second of which is assigned
//a value of pi.
//@@Tests
//@{ test_subset1.m
//% Test the vector subset operator
//function test_val = test_subset1
//a = [1,2,3,4;5,6,7,8];
//b = [3;4;5];
//c = a(b);
//test_val = test(c==[2;6;3]);
//@}
//@{ test_subset2.m
//% Test the vector subset operator in an error condition
//function test_val = test_subset2
//a = [1,2,3,4];
//b = [4,5,6];
//test_val = 0;
//try
//c = a(b);
//catch
//test_val = 1;
//end
//@}
//@{ test_subset3.m
//% Test the vector subset with a cell array
//function test_val = test_subset3
//a = {1,2,3,4;5,6,7,8};
//b = [3;4];
//c = a(b);
//test_val = test((c{1} == 2) & (c{2} == 6));
//@}
//@{ test_subset4.m
//% Test the vector subset with a struct array
//function test_val = test_subset4
//a = struct('foo',4,'goo',{1,2,3,4});
//c = a(3);
//test_val = test((c.foo == 4) & (c.goo == 3));
//@}
//@{ test_subset5.m
//% Test the vector subset with cell array without contents-based addressing
//function test_val = test_subset5
//a = {1,2,3,4;5,6,7,8};
//b = a(2);
//test_val = test(strcmp(typeof(b),'cell')) & test(b{1} == 5);
//@}
//@{ test_subset6.m
//% Test the ndim subset with a regular vector
//function test_val = test_subset6
//a = [1,2,3;4,5,6;7,8,9];
//b = a(1:2,2:3);
//c = [2,3;5,6];
//test_val = test(b == c);
//@}
//@{ test_subset7.m
//% Test the ndim subset with a cell-array vector
//function test_val = test_subset7
//a = {1,2,3;4,5,6;7,8,9};
//b = a(2,2:3);
//test_val = test(strcmp(typeof(b),'cell')) & test((b{1} == 5) & (b{2} == 6));
//@}
//@{ test_subset8.m
//% Test the ndim subset with a cell-array vector
//function test_val = test_subset8
//a = {1,2,3;4,5,6;7,8,9};
//c = struct('foo',a);
//b = c(2,2:3);
//test_val = test(strcmp(typeof(b),'struct')) & ...
//               test((b(1).foo == 5) & (b(2).foo == 6));
//@}
//@{ test_subset9.m
//% Test the cell vector subset with contents-based addressing
//function test_val = test_subset9
//  a = {5,8;10,3};
//  sm = test_subset9_assist(a{1:3});
//  test_val = test(sm == 23);
//
//function a = test_subset9_assist(b,c,d)
//  a = b + c + d;
//@}
//@{ test_subset10.m
//% Test the cell vector subset with contents-based addressing
//function test_val = test_subset10
//  a = {5,8;10,3};
//  sm = test_subset10_assist(a{2,1:2});
//  test_val = test(sm == 13);
//
//function a = test_subset10_assist(b,c)
//  a = b + c;
//@}
//@{ test_subset11.m
//% Test the vector subset assign function
//function test_val = test_subset11
//a = [1,2,3;4,5,6];
//a(3:4) = [8,9];
//test_val = test(a == [1,8,3;4,9,6]);
//@}
//@{ test_subset12.m
//% Test the ndim subset assign function
//function test_val = test_subset12
//a = [1,2,3;4,5,6];
//a(:,2) = [8,9];
//test_val = test(a == [1,8,3;4,9,6]);
//@}
//@{ test_subset13.m
//% Test the ndim subset contents-based assign function
//function test_val = test_subset13
//a = {1,2,3;4,5,6};
//a{4} = 'goo';
//test_val = test(strcmp(a{2,2},'goo'));
//@}
//@{ test_subset14.m
//% Test the vector contents-based assign for multiple values
//function test_val = test_subset14
//a = {0,0,0,0};
//[a{1:4}] = test_subset14_assist;
//test_val = test(strcmp(typeof(a),'cell')) & ...
//    (a{1} == 1) & (a{2} == 2) & (a{3} == 3) & (a{4} == 4);
//
//function [a,b,c,d] = test_subset14_assist
//  a = 1;
//  b = 2;
//  c = 3;
//  d = 4;
//@}
//@{ test_subset15.m
//% Test the ndim contents-based assign for multiple values
//function test_val = test_subset15
//a = {0,0;0,0};
//[a{1:2,1:2}] = test_subset15_assist;
//test_val = test(strcmp(typeof(a),'cell')) & ...
//    (a{1} == 1) & (a{2} == 2) & (a{3} == 3) & (a{4} == 4);
//
//function [a,b,c,d] = test_subset15_assist
//  a = 1;
//  b = 2;
//  c = 3;
//  d = 4;
//@}
//@{ test_subset16.m
//% Test the ndim contents-based assign for multiple values - with
//% too many outputs
//function test_val = test_subset16
//a = {0,0;0,0};
//test_val = 0;
//try
//  [a{1:2,1:2}] = test_subset16_assist
//catch
//  test_val = 1;
//end
//
//function [a,b,c] = test_subset16_assist
//  a = 1;
//  b = 2;
//  c = 3;
//@}
//@{ test_subset17.m
//% Test the subset assign with an empty variable
//function test_val = test_subset17
//a = [];
//a(:) = [];
//test_val = 1;
//@}
//@{ test_subset18.m
//% Test the subset assign with an empty variable
//function test_val = test_subset18
//a = [];
//b = 1:4;
//a(:) = b;
//test_val = (a(1) == 1) & (a(2) == 2) & (a(3) == 3) & (a(4) == 4);
//@}
//@{ test_subset19.m
//% Torture test for subset indexing
//function test_val = test_subset19
//a1 = 5;
//A{3}(2).foo(2) = a1;
//test_val = test(strcmp(typeof(A),'cell')) & ...
//    test(strcmp(typeof(A{3}),'struct')) & ...
//    test(strcmp(typeof(A{3}(2).foo),'double'));
//@}
//@{ test_subset20.m
//function x = test_subset20
//  a = [];
//  b = rand(14,1);
//  a(:,1) = b;
//  x = testeq(a,b);
//@}
//@{ test_subset21.m
//function x = test_subset21
//  a = zeros(4,4,5);
//  p = randn(4,4,1);
//  a(:,:,end) = p;
//  x = 1;
//@}
//@{ test_subset22.m
//function x = test_subset22
//  a = rand(10,2);
//  p = a(:,2);
//  c = p(:,1);
//  x = issame(p,c);
//@}
//!


// This has a few shortcomings that prevent it from being
// 100% correct.  
//
//   1.  subsindex is not called for argument
//       expressions of user-defined classes.
//   2.  "end" no longer works.
//
// To fix "end", we should use a source transformation technique.
// The original tree looks like this
//
//   variable
//      -> t
//      -> ()
//          -> 2
//          -> end
//
// This should be translated into:
//
//   _t = end(t,2)
//
// This is done in Transform.cpp...  
//
// This does not cover:
//    Function pointers
//    subsindex
//
// 
 
//
// 
void Interpreter::deref(Array &r, const Tree & s) {
  SaveEndInfo;
  endRef = &r;
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numChildren();
    if (s.numChildren() == 0) {
      r = r;
    } else {
      for (int p = 0; p < s.numChildren(); p++) {
	endCount = m.size();
	multiexpr(s.child(p),m);
      }
      subsindex(m);
      if (m.size() == 1)
	r = r.get(m[0]);
      else
	r = r.get(m);
    }
  } else if (s.is(TOK_BRACES)) {
    ArrayVector m;
    endTotal = s.numChildren();
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r = ArrayFromCellArray(r.get(m[0]));
    else
      r = ArrayFromCellArray(r.get(m));
  } else if (s.is('.')) {
    r = r.get(s.first().text()).front();
  } else if (s.is(TOK_DYN)) {
    QString field;
    try {
      Array fname(expression(s.first()));
      field = fname.asString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    r = r.get(field).front();
  }
  RestoreEndInfo;
}
 
 Array Interpreter::rhs(const Tree & t) {
   ArrayReference ptr(context->lookupVariable(t.first().text()));
   if (!ptr.valid()) {
     ArrayVector m;
     functionExpression(t,1,false,m);
     m = handleReindexing(t,m);
     if (m.size() >= 1)
       return m[0];
     else
       return EmptyConstructor();
   }
   if (t.numChildren() == 1)
     return *ptr;
   if (ptr->isUserClass() && !stopoverload) {
     ArrayVector m(ClassRHSExpression(*ptr,t,this));
     if (m.size() >= 1)
       return m[0];
     else
       return EmptyConstructor();
   }
   Array r(*ptr);
   for (int index = 1;index < t.numChildren();index++) 
     deref(r,t.child(index));
   return r;
 }

  
int Interpreter::getErrorCount() {
  int retval = errorCount;
  errorCount = 0;
  return retval;
}

Interpreter::Interpreter(Context* aContext) {
  errorCount = 0;
  lasterr = QString("");
  context = aContext;
  depth = 0;
  printLimit = 1000;
  autostop = false;
  intryblock = false;
  jitcontrol = false;
  stopoverload = false;
  m_skipflag = false;
  m_noprompt = false;
  m_liveUpdateFlag = false;
  tracetrap = 0;
  tracecurrentline = 0;
  endRef = NULL;
  m_interrupt = false;
  m_kill = false;
  m_diaryState = false;
  m_diaryFilename = "diary";
  m_captureState = false;
  m_capture = "";
  m_profile = false;
  m_quietlevel = 0;
  m_enableWarnings = true;
  m_disablerescan = false;
  context->pushScope("base","base",false);
}


Interpreter::~Interpreter() {
  delete context;
}

bool Interpreter::getDisableRescan() {
  return m_disablerescan;
}

void Interpreter::setDisableRescan(bool flag) {
  m_disablerescan = flag;
}

bool Interpreter::getStopOverload() {
  return stopoverload;
}

void Interpreter::setStopOverload(bool flag) {
  stopoverload = flag;
}

// stackentry& Interpreter::activeDebugStack() {
//   if (cstack.isEmpty()) throw Exception("Debug stack is corrupted -- please file a bug report that reproduces this problem!");
//   if (cstack.size() < 2) return cstack[0];
//   return cstack[cstack.size()-2];
// }

// const stackentry& Interpreter::activeDebugStack() const {
//   if (cstack.isEmpty()) throw Exception("Debug stack is corrupted -- please file a bug report that reproduces this problem!");
//   if (cstack.size() < 2) return cstack[0];
//   return cstack[cstack.size()-2];
// }


// We want dbstep(n) to cause us to advance n statements and then
// stop.  we execute statement-->set step trap,
void Interpreter::dbstepStatement(const Tree & t) {
  int lines = 1;
  if (t.hasChildren()) {
    Array lval(expression(t.first()));
    lines = lval.asInteger();
  }
  // Get the current function
  FuncPtr val;
  if (context->scopeName() == "base") return;
  ParentScopeLocker lock(context);
  if (!lookupFunction(context->scopeDetailString(),val)) {
    warningMessage(QString("unable to find function ") + context->scopeDetailString() + " to single step");
    return;
  }
  context->setScopeStepTrap(lines);
  context->setScopeStepCurrentLine(LineNumber(context->scopeTokenID()));
}

void Interpreter::dbtraceStatement(const Tree & t) {
  int lines = 1;
  if (t.hasChildren()) {
    Array lval(expression(t.first()));
    lines = lval.asInteger();
  }
  // Get the current function
  FuncPtr val;
  if (context->scopeDetailString() == "base") return;
  ParentScopeLocker lock(context);
  if (!lookupFunction(context->scopeDetailString(),val)) {
    warningMessage(QString("unable to find function ") + context->scopeDetailString() + " to single step");
    return;
  }
  tracetrap = lines;
  tracecurrentline = LineNumber(context->scopeTokenID());
}

// static QString EvalPrep(QString line) {
//   QString buf1 = line;
//   if (buf1.endsWith('\n'))
//     buf1.chop(1);
//   if (buf1.endsWith('\r'))
//     buf1.chop(1);
//   if (buf1.size() > 20)
//     buf1 = buf1.left(20) + "...";
//   return buf1;
// }

void Interpreter::ExecuteLine(QString txt) {
  mutex.lock();
  cmd_buffer.push_back(txt);
  bufferNotEmpty.wakeAll();
  mutex.unlock();
  if (m_diaryState) diaryMessage(txt);
}

//PORT
void Interpreter::evaluateString(QString line, bool propogateExceptions) {
  Tree b;
  Tree t;
  m_interrupt = false;
  Scanner S(line,"");
  Parser P(S);
  try{
    b = P.process();
    t = b;
    if (!t.is(TOK_SCRIPT))
      throw Exception("Function definition unexpected!");
    t = t.first();
  } catch(Exception &e) {
    if (propogateExceptions)
      throw;
    errorCount++;
    e.printMe(this);
    return;
  }
  try {
    block(t);
  } catch(Exception &e) {
    if (propogateExceptions) throw;
    errorCount++;
    e.printMe(this);
  }
}
  
QString Interpreter::getLastErrorString() {
  return lasterr;
}

void Interpreter::setLastErrorString(QString txt) {
  lasterr = txt;
}

void Interpreter::setGreetingFlag(bool skip) {
  m_skipflag = skip;
}

void Interpreter::setNoPromptFlag(bool noprompt) {
  m_noprompt = noprompt;
}

bool NeedsMoreInput(Interpreter *eval, QString txt) {
  // Check for ... or an open []
  try {
    Scanner S(txt,"");
    while (!S.next().is(TOK_EOF))
      S.consume();
    if (S.inContinuationState() || S.inBracket()) return true;
  } catch (Exception &e) {
  }
  try {
    Scanner S(txt,"");
    Parser P(S);
    P.process();
    return false;
  } catch (Exception &e) {
    if (e.msg().left(13) == "Expecting end") {
      return true;
    }
  }
  return false;
}

void Interpreter::sleepMilliseconds(unsigned long msecs) {
  QThread::msleep(msecs);
}

QString Interpreter::getLine(QString prompt) {
  if (!m_noprompt) emit SetPrompt(prompt);
  if (m_diaryState) diaryMessage(prompt);
  QString retstring;
  emit EnableRepaint();
  mutex.lock();
  if (cmd_buffer.isEmpty())
    bufferNotEmpty.wait(&mutex);
  retstring = cmd_buffer.front();
  cmd_buffer.erase(cmd_buffer.begin());
  mutex.unlock();
  emit DisableRepaint();
  return retstring;
}

// This is a "generic" CLI routine.  The user interface (non-debug)
// version of this is "docli"
void Interpreter::evalCLI() {
  QString prompt;
  bool rootCLI;
  setupWatcher();
  while(1) {
    QString fname;
    int line = 0;
    if ((depth == 0) || (context->scopeDepth() < 2)) {
      prompt = "--> ";
      rootCLI = true;
    } else {
      int bypasscount = 0;
      while (InKeyboardScope(context)) {
	bypasscount++;
	context->bypassScope(1);
      }
      fname = context->scopeName();
      line = LineNumber(context->scopeTokenID());
      QString scopename = context->scopeDetailString();
      if (scopename == "builtin")
	scopename = context->scopeName();
      if (scopename == "docli") 
	scopename = "base";
      prompt = QString("[%1,%2]--> ").arg(scopename).arg(line);
      context->restoreScope(bypasscount);
      rootCLI = false;
    }
    if (rootCLI) {
      tracetrap = 0;
      context->setScopeStepTrap(0);
    }
    if (m_captureState) 
      m_capture += prompt;
    else {
      if (!m_noprompt) emit SetPrompt(prompt);
      if (m_diaryState) diaryMessage(prompt);
    }
    if (m_liveUpdateFlag) {
      updateVariablesTool();
      updateStackTool();
      emit ShowActiveLine(fname,line);
    }
    QString cmdset;
    QString cmdline;
    emit EnableRepaint();
    mutex.lock();
    
    while ((cmdset.isEmpty() || 
	    NeedsMoreInput(this,cmdset)) && (!m_interrupt)) {
      if (cmd_buffer.isEmpty())
	bufferNotEmpty.wait(&mutex);
      cmdline = cmd_buffer.front();
      cmd_buffer.erase(cmd_buffer.begin());
      cmdset += cmdline;
      if (m_captureState) 
	m_capture += cmdline;
    }
    mutex.unlock();
    emit DisableRepaint();
    if (m_interrupt) {
      m_interrupt = false;
      continue;
    }
    int scope_stackdepth = context->scopeDepth(); 
    setInCLI(true);
    dbdown_executed = false;
    evaluateString(cmdset,false);
    if (!dbdown_executed) {
      while (context->scopeDepth() > scope_stackdepth) context->popScope();
    }
  }
}

  
//
// Convert a list of variable into indexing expressions
//  - for user defined classes, we call subsindex for 
//  - the object
Array Interpreter::subsindex(const Array &m) {
  if (m.isUserClass() && !stopoverload) {
    Array t(ClassUnaryOperator(m,"subsindex",this));
    return Add(t.toClass(Double),Array(index_t(1)));
  }
  return m;
}

 void Interpreter::subsindex(ArrayVector& m) {
   for (int p=0;p<((int)m.size());p++)
     m[p] = subsindex(m[p]);
 }
