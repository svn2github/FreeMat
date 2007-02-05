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
#include <stack>
#include <math.h>
#include <stdio.h>
#include "Exception.hpp"
#include "Math.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "Module.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include <signal.h>
#include "Class.hpp"
#include "Print.hpp"
#include <qapplication.h>
#include <sys/types.h>
#include <qeventloop.h>
#include <QtCore>

#ifdef WIN32
#define DELIM "\\"
#include <direct.h>
#define getcwd _getcwd
#else
#include <pwd.h>
#define DELIM "/"
#include <glob.h>
#endif

#ifdef WIN32
#define DELIM "\\"
//#define S_ISREG(x) (x & _S_IFREG)
#include <direct.h>
#define PATH_DELIM ";"
#else
#define DELIM "/"
#define PATH_DELIM ":"
#endif

#define MAXSTRING 65535

/**
 * Pending control-C
 */
bool InterruptPending = false;

#define SaveEndInfo  \
  ArrayReference oldEndRef = endRef; \
  int oldEndCount = endCount; \
  int oldEndTotal = endTotal; 

#define RestoreEndInfo \
  endRef = oldEndRef; \
  endCount = oldEndCount; \
  endTotal = oldEndTotal;  \

char* TildeExpand(char* path) {
#ifdef WIN32
  return path;
#else
  char *newpath = path;
  if (path[0] == '~' && (path[1] == '/') || (path[1] == 0)) {
    char *home;
    home = getenv("HOME");
    if (home) {
      newpath = (char*) malloc(strlen(path) + strlen(home));
      strcpy(newpath,home);
      strcat(newpath,path+1);
    }
  } else if (path[0] == '~' && isalpha(path[1])) {
    char username[4096];
    char *cp, *dp;
    // Extract the user name
    cp = username;
    dp = path+1;
    while (*dp != '/')
      *cp++ = *dp++;
    *cp = 0;
    // Call getpwnam...
    struct passwd *dat = getpwnam(cp);
    if (dat) {
      newpath = (char*) malloc(strlen(path) + strlen(dat->pw_dir));
      strcpy(newpath,dat->pw_dir);
      strcat(newpath,dp);
    }
  }
  return newpath;
#endif
}

void Interpreter::setPath(std::string path) {
  char* pathdata = strdup(path.c_str());
  // Search through the path
  char *saveptr = (char*) malloc(sizeof(char)*1024);
  char* token;
  token = strtok(pathdata,PATH_DELIM);
  m_userPath.clear();
  while (token != NULL) {
    if (strcmp(token,".") != 0)
      m_userPath << QString(TildeExpand(token));
    token = strtok(NULL,PATH_DELIM);
  }
  rescanPath();
}

std::string Interpreter::getTotalPath() {
  std::string retpath;
  QStringList totalPath(QStringList() << m_basePath << m_userPath);
  for (int i=0;i<totalPath.size()-1;i++) 
    retpath = retpath + totalPath[i].toStdString() + PATH_DELIM;
  if (totalPath.size() > 0) 
    retpath = retpath + totalPath[totalPath.size()-1].toStdString();
  return retpath;
}
  
std::string Interpreter::getPath() {
  std::string retpath;
  QStringList totalPath(m_userPath);
  for (int i=0;i<totalPath.size()-1;i++) 
    retpath = retpath + totalPath[i].toStdString() + PATH_DELIM;
  if (totalPath.size() > 0) 
    retpath = retpath + totalPath[totalPath.size()-1].toStdString();
  return retpath;
}
  
void Interpreter::rescanPath() {
  if (!context) return;
  context->flushTemporaryGlobalFunctions();
  for (int i=0;i<m_basePath.size();i++)
    scanDirectory(m_basePath[i].toStdString(),false,"");
  for (int i=0;i<m_userPath.size();i++)
    scanDirectory(m_userPath[i].toStdString(),false,"");
  // Scan the current working directory.
  char cwd[1024];
  getcwd(cwd,1024);
  scanDirectory(std::string(cwd),true,"");
  emit CWDChanged();
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
  
void Interpreter::scanDirectory(std::string scdir, bool tempfunc,
				std::string prefix) {
  QDir dir(QString::fromStdString(scdir));
  dir.setFilter(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
  dir.setNameFilters(QStringList() << "*.m" << "*.p" 
		     << "@*" << "private" << "*."+mexExtension());
  QFileInfoList list(dir.entryInfoList());
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo(list.at(i));
    std::string fileSuffix(fileInfo.suffix().toStdString());
    std::string fileBaseName(fileInfo.baseName().toStdString());
    std::string fileAbsoluteFilePath(fileInfo.absoluteFilePath().toStdString());
    if (fileSuffix == "m" || fileSuffix == "M") 
      if (prefix.empty())
	procFileM(fileBaseName,fileAbsoluteFilePath,tempfunc);
      else
	procFileM(prefix + ":" + fileBaseName,fileAbsoluteFilePath,tempfunc);
    else if (fileSuffix == "p" || fileSuffix == "P")
      if (prefix.empty())
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
  
void Interpreter::procFileM(std::string fname, std::string fullname, bool tempfunc) {
  MFunctionDef *adef;
  adef = new MFunctionDef();
  adef->name = fname;
  adef->fileName = fullname;
  context->insertFunctionGlobally(adef, tempfunc);
}
  
void Interpreter::procFileP(std::string fname, std::string fullname, bool tempfunc) {
  MFunctionDef *adef;
  // Open the file
  try {
    File *f = new File(fullname.c_str(),"rb");
    Serialize *s = new Serialize(f);
    s->handshakeClient();
    s->checkSignature('p',2);
    adef = ThawMFunction(s);
    adef->pcodeFunction = true;
    delete f;
    context->insertFunctionGlobally(adef, tempfunc);
  } catch (Exception &e) {
  }
}

void Interpreter::procFileMex(std::string fname, std::string fullname, bool tempfunc) {
  MexFunctionDef *adef;
  adef = new MexFunctionDef(fullname);
  adef->name = fname;
  if (adef->LoadSuccessful())
    context->insertFunctionGlobally((MFunctionDef*)adef,tempfunc);
  else
    delete adef;
}

void Interpreter::RegisterGfxResults(ArrayVector m) {
  mutex.lock();
  gfx_buffer.push_back(m);
  gfxBufferNotEmpty.wakeAll();
  mutex.unlock();
}

void Interpreter::RegisterGfxError(string msg) {
  mutex.lock();
  gfxError = msg;
  gfxErrorOccured = true;
  gfxBufferNotEmpty.wakeAll();
  mutex.unlock();
}

ArrayVector Interpreter::doGraphicsFunction(FuncPtr f, ArrayVector m, int narg_out) {
  //  qDebug() << "Start graphics call\r";
  if (!gfx_buffer.empty())
    qDebug() << "Warning! graphics return buffer not empty on start\r";
  gfxErrorOccured = false;
  QMutexLocker lock(&mutex);
  emit doGraphicsCall(f,m,narg_out);
  if (!gfxErrorOccured && gfx_buffer.empty()) {
    gfxBufferNotEmpty.wait(&mutex);
  } else {
    qDebug() << "Wha??\r";
  }
  if (gfxErrorOccured) {
    qDebug() << "Exception\r";
    throw Exception(gfxError);
  }
  if (gfx_buffer.empty())
    qDebug() << "Warning! graphics empty on return\r";
  ArrayVector ret(gfx_buffer.front());
  gfx_buffer.erase(gfx_buffer.begin());
  return ret;
}

void Interpreter::setTerminalWidth(int ncols) {
  mutex.lock();
  m_ncols = ncols;
  mutex.unlock();
}

int Interpreter::getTerminalWidth() {
  return m_ncols;
}

std::string TranslateString(std::string x) {
  std::string y(x);
  unsigned int n;
  n = 0;
  while (n<y.size()) {
    if (y[n] == '\n') 
      y.insert(n++,"\r");
    n++;
  }
  return y;
}

void Interpreter::outputMessage(std::string msg) {
  emit outputRawText(TranslateString(msg));
}

void Interpreter::errorMessage(std::string msg) {
  emit outputRawText(TranslateString("Error: " + msg + "\r\n"));
}

void Interpreter::warningMessage(std::string msg) {
  emit outputRawText(TranslateString("Warning: " +msg + "\r\n"));
}


void Interpreter::SetContext(int a) {
//   qDebug() << "setting context to line " << (a & 0xffff);
  ip_context = a;
}

static bool isMFile(std::string arg) {
  // Not completely right...
  return (((arg[arg.size()-1] == 'm') ||
	   (arg[arg.size()-1] == 'p')) && 
	  (arg[arg.size()-2] == '.'));
}

std::string TrimFilename(std::string arg) {
  int ndx = arg.rfind(DELIM);
  if (ndx>=0)
    arg.erase(0,ndx+1);
  return arg;
}

std::string TrimExtension(std::string arg) {
  if (arg.size() > 2 && arg[arg.size()-2] == '.')
    arg.erase(arg.size()-2,arg.size());
  return arg;
}

static std::string PrivateMangleName(std::string cfunc, std::string fname) {
  if (cfunc.empty()) return "";
  int ndx;
  ndx = cfunc.rfind(DELIM);
  if (ndx>=0)
    cfunc.erase(ndx+1,cfunc.size());
  return cfunc + "private:" + fname;
}

std::string Interpreter::getVersionString() {
  return std::string("FreeMat v" VERSION);
}

// Run the thread function
void Interpreter::run() {
  if (m_threadFunc) {
    try {
      m_threadFuncRets = m_threadFunc->evaluateFunction(this,m_threadFuncArgs,0);
    } catch (Exception &e) {
      m_threadErrorState = true;      
      lasterr = e.getMessageCopy();
    } catch (InterpreterQuitException &e) {
      m_threadErrorState = true;      
      lasterr = "'quit' called in non-main thread";
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
  emit CWDChanged();
  if (!m_skipflag)
    sendGreeting();
  try {
    while (1) {
      try {
	evalCLI();
      } catch (InterpreterRetallException) {
	clearStacks();
      } catch (InterpreterReturnException &e) {
      }
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
  outputMessage(" " + getVersionString() + " (build 2288)\n");
  outputMessage(" Copyright (c) 2002-2007 by Samit Basu\n");
  outputMessage(" Licensed under the GNU Public License (GPL)\n");
  outputMessage(" Type <help license> to find out more\n");
  outputMessage("      <helpwin> for online help\n");
  outputMessage("      <pathtool> to set or change your path\n");
}

std::string Interpreter::getPrivateMangledName(std::string fname) {
  std::string ret;
  char buff[4096];
  if (isMFile(ip_funcname)) 
    ret = PrivateMangleName(ip_funcname,fname);
  else {
    getcwd(buff,4096);
    ret = std::string(buff) + DELIM + std::string("private:" + fname);
  }
  return ret; 
}

std::string Interpreter::getMFileName() {
  if (isMFile(ip_funcname)) 
    return TrimFilename(TrimExtension(ip_funcname));
  for (int i=cstack.size()-1;i>=0;i--)
    if (isMFile(cstack[i].cname)) 
      return TrimFilename(TrimExtension(cstack[i].cname));
  return std::string("");
}

std::string Interpreter::getInstructionPointerFileName() {
  if (isMFile(ip_funcname)) 
    return ip_funcname;
  return string("");
}

stackentry::stackentry(std::string cntxt, std::string det, 
		       int id, int num, int strp, int stcl) :
  cname(cntxt), detail(det), tokid(id), number(num),
  steptrap(strp), stepcurrentline(stcl)
{
}

stackentry::stackentry() {
}

stackentry::~stackentry() {
}

static Interpreter* myInterp;

void sigInterrupt(int arg) {
  InterruptPending = true;
  myInterp->ExecuteLine("");
}

Array Interpreter::DoBinaryOperator(const tree& t, BinaryFunc fnc, 
				    std::string funcname) {
  Array a(expression(t.first()));
  Array b(expression(t.second()));
  if (!(a.isUserClass() || b.isUserClass())) 
    return fnc(a,b);
  return ClassBinaryOperator(a,b,funcname,this);
}

Array Interpreter::DoUnaryOperator(const tree &t, UnaryFunc fnc, 
				   std::string funcname) {
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
 
void Interpreter::clearStacks() {
  //    cname = "base";
  cstack.clear();
  ip_funcname = "base";
  ip_detailname = "base";
  ip_context = 0;
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
//!
//Works
Array Interpreter::matrixDefinition(const tree &t) {
  ArrayMatrix m;
  if (t.numchildren() == 0) 
    return Array::emptyConstructor();
  for (int i=0;i<t.numchildren();i++) {
    const tree &s(t.child(i));
    ArrayVector n;
    for (int j=0;j<s.numchildren();j++)
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
    return Array::matrixConstructor(m);
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
Array Interpreter::cellDefinition(const tree & t) {
  ArrayMatrix m;
  if (t.numchildren() == 0) {
    Array a(Array::emptyConstructor());
    a.promoteType(FM_CELL_ARRAY);
    return a;
  }
  for (int i=0;i<t.numchildren();i++) {
    const tree &s(t.child(i));
    ArrayVector n;
    for (int j=0;j<s.numchildren();j++) 
      multiexpr(s.child(j),n);
    m.push_back(n);
  }
  return Array::cellConstructor(m);
}

Array Interpreter::ShortCutOr(const tree & t) {
  Array a(expression(t.first()));
  Array retval;
  if (!a.isScalar())
    retval = DoBinaryOperator(t,Or,"or");
  else {
    // A is a scalar - is it true?
    a.promoteType(FM_LOGICAL);
    if (*((const logical*)a.getDataPointer()))
      retval = a;
    else 
      retval = DoBinaryOperator(t,Or,"or");
  }
  return retval;
}

Array Interpreter::ShortCutAnd(const tree &t) {
  SetContext(t.context());
  Array a(expression(t.first()));
  SetContext(t.context());
  Array retval;
  if (!a.isScalar()) {
    retval = DoBinaryOperator(t,And,"and");
  } else {
    // A is a scalar - is it false?
    a.promoteType(FM_LOGICAL);
    if (!*((const logical*)a.getDataPointer()))
      retval = a;
    else 
      retval = DoBinaryOperator(t,And,"and");
  }
  return retval;
}

//Works
// Need to take care

void Interpreter::multiexpr(const tree &t, ArrayVector &q, int lhsCount) {
  if (t.is(TOK_VARIABLE)) {
    ArrayReference ptr(context->lookupVariable(t.first().text()));
    if (!ptr.valid()) {
      functionExpression(t,lhsCount,false,q);
      return;
    }
    if ((ptr->dataClass() == FM_FUNCPTR_ARRAY &&
	 ptr->isScalar()) && (t.numchildren() > 1)) {
      q += FunctionPointerDispatch(*ptr,t.second(),1);
      return;
    }
    if (t.numchildren() == 1) {
      q.push_back(*ptr);
      return;
    }
    //    if (ptr->isUserClass() && !stopoverload && !inMethodCall(ptr->className().back())) {
    //      treeVector indexExpr(t.children());
    //      indexExpr.erase(indexExpr.begin());
    //      return ClassRHSExpression(*ptr,indexExpr,this);
    //    }
    Array r(*ptr);
    for (unsigned index = 1;index < t.numchildren()-1;index++) 
      deref(r,t.child(index));
    SaveEndInfo;
    endRef = &r;
    const tree &s(t.last());
    if (s.is(TOK_PARENS)) {
      ArrayVector m;
      endTotal = s.numchildren();
      for (unsigned p = 0;p < s.numchildren(); p++) {
	endCount = m.size();
	multiexpr(s.child(p),m);
      }
      subsindex(m);
      if (m.size() == 1)
	q.push_back(r.getVectorSubset(m.front()));
      else
	q.push_back(r.getNDimSubset(m));
    } else if (s.is(TOK_BRACES)) {
      ArrayVector m;
      endTotal = s.numchildren();
      for (unsigned p = 0;p < s.numchildren(); p++) {
	endCount = m.size();
	multiexpr(s.child(p),m);
      }
      subsindex(m);
      if (m.size() == 1)
	q += r.getVectorContentsAsList(m.front());
      else
	q += r.getNDimContentsAsList(m);
    } else if (s.is('.')) {
      q += r.getFieldAsList(s.first().text());
    } else if (s.is(TOK_DYN)) {
      const char *field;
      try {
	Array fname(expression(s.first()));
	field = fname.getContentsAsCString();
      } catch (Exception &e) {
	throw Exception("dynamic field reference to structure requires a string argument");
      }
      q += r.getFieldAsList(field);
    }
    RestoreEndInfo;
  } else if (!t.is(TOK_KEYWORD))
    q.push_back(expression(t));
}

Array Interpreter::expression(const tree &t) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return rhs(t);
  case TOK_INTEGER:
  case TOK_FLOAT:
  case TOK_DOUBLE:
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
    return t.array();
  case TOK_END:
    if (!endRef.valid()) 
      throw Exception("END keyword not allowed for undefined variables");
    if (endTotal == 1)
      return Array::int32Constructor(endRef->getLength());
    else
      return Array::int32Constructor(endRef->getDimensionLength(endCount));
  case ':':
    if (t.numchildren() == 0) {
      return Array::stringConstructor(":");
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
  case '|': 
    return ShortCutOr(t); 
    break;
  case TOK_SAND: 
  case '&': 
    return ShortCutAnd(t); 
    break;
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
    return DoUnaryOperator(t,Transpose,"ctranspose"); 
    break;
  case TOK_DOTTRANSPOSE: 
    return DoUnaryOperator(t,DotTranspose,"transpose"); 
    break;
  case '@':
    {
      FuncPtr val;
      if (!lookupFunction(t.first().text(),val))
	throw Exception("unable to resolve " + t.first().text() + 
			" to a function call");
      return Array::funcPtrConstructor(val);
      break;
    }
  default:
    throw Exception("Unrecognized expression!");
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
//!
//Works
Array Interpreter::unitColon(const tree &t) {
  Array a, b;
  a = expression(t.first());
  b = expression(t.second());
  if (!(a.isUserClass() || b.isUserClass()))
    return UnitColon(a,b);
  else
    return ClassBinaryOperator(a,b,"colon",this);
}

//Works
Array Interpreter::doubleColon(const tree &t) {
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
bool Interpreter::testCaseStatement(const tree &t, Array s) {
  int ctxt = t.context();
  Array r(expression(t.first()));
  bool caseMatched = s.testForCaseMatch(r);
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
void Interpreter::tryStatement(const tree &t) {
  // Turn off autostop for this statement block
  bool autostop_save = autostop;
  autostop = false;
  // Get the state of the IDnum stack and the
  // contextStack and the cnameStack
  int stackdepth;
  stackdepth = cstack.size();
  try {
    block(t.first());
  } catch (Exception &e) {
    while (cstack.size() > stackdepth) popDebug();
    if (t.numchildren()>1) {
      autostop = autostop_save;
      block(t.second().first());
    }
  } 
  autostop = autostop_save;
}


bool Interpreter::AutoStop() {
  return autostop;
}
  

void Interpreter::AutoStop(bool a) {
  autostop = a;
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
//!
//Works
void Interpreter::switchStatement(const tree &t) {
  Array switchVal;
  int ctxt = t.context();
  // First, extract the value to perform the switch on.
  switchVal = expression(t.first());
  // Assess its type to determine if this is a scalar switch
  // or a string switch.
  if (!switchVal.isScalar() && !switchVal.isString())
    throw Exception("Switch statements support scalar and string arguments only.");
  unsigned n=1;
  while (n < t.numchildren() && t.child(n).is(TOK_CASE)) {
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
//!
//Works
void Interpreter::ifStatement(const tree &t) {
  bool elseifMatched;
  int ctxt = t.context();
    
  bool condtest = !(expression(t.first()).isRealAllZeros());
  if (condtest) {
    block(t.second());
    return;
  } else {
    unsigned n=2;
    while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
      if (!(expression(t.child(n).first()).isRealAllZeros())) {
	block(t.child(n).second());
	return;
      }
      n++;
    }
  }
  if (t.last().is(TOK_ELSE))
    block(t.last().first());
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
//while (k<100), accum = accum + k; k = k + 1; end
//accum
//@>
//!
//Works
void Interpreter::whileStatement(const tree &t) {
  int ctxt = t.context();
  tree testCondition(t.first());
  tree codeBlock(t.second());
  bool breakEncountered = false;
  Array condVar(expression(testCondition));
  bool conditionTrue = !condVar.isRealAllZeros();
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
      conditionTrue = !condVar.isRealAllZeros();
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

template <class T>
void ForLoopHelper(const tree &codeBlock, Class indexClass, const T *indexSet, 
		   int count, string indexName, Interpreter *eval) {
  Scope *scope = eval->getContext()->getCurrentScope();
  for (int m=0;m<count;m++) {
    Array *vp = scope->lookupVariable(indexName);
    if ((!vp) || (vp->dataClass() != indexClass) || (!vp->isScalar())) {
      scope->insertVariable(indexName,Array(indexClass,Dimensions(1,1)));
      vp = scope->lookupVariable(indexName);
    }
    ((T*) vp->getReadWriteDataPointer())[0] = indexSet[m];
    try {
      eval->block(codeBlock);
    } catch (InterpreterContinueException &e) {
    } catch (InterpreterBreakException &e) {
      break;
    } 
  }
}

template <class T>
void ForLoopHelperComplex(const tree &codeBlock, Class indexClass, 
			  const T *indexSet, int count, 
			  string indexName, Interpreter *eval) {
  Scope *scope = eval->getContext()->getCurrentScope();
  for (int m=0;m<count;m++) {
    Array *vp = scope->lookupVariable(indexName);
    if ((!vp) || (vp->dataClass() != indexClass) || (!vp->isScalar())) {
      scope->insertVariable(indexName,Array(indexClass,Dimensions(1,1)));
      vp = scope->lookupVariable(indexName);
    }
    ((T*) vp->getReadWriteDataPointer())[0] = indexSet[2*m];
    ((T*) vp->getReadWriteDataPointer())[1] = indexSet[2*m+1];
    try {
      eval->block(codeBlock);
    } catch (InterpreterContinueException &e) {
    } catch (InterpreterBreakException &e) {
      break;
    } 
  }
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
//!
//Works
void Interpreter::forStatement(const tree &t) {
  Array indexSet;
  string indexVarName;
  int ctxt = t.context();

  /* Get the name of the indexing variable */
  if (t.first().is('=')) {
    indexVarName = t.first().first().text();
    /* Evaluate the index set */
    indexSet = expression(t.first().second());
  } else {
    indexVarName = t.first().text();
    ArrayReference ptr(context->lookupVariable(indexVarName));
    if (!ptr.valid()) throw Exception("index variable " + indexVarName + " used in for statement must be defined");
    indexSet = *ptr;
  }
  /* Get the code block */
  tree codeBlock(t.second());
  int elementCount = indexSet.getLength();

  Class loopType(indexSet.dataClass());
  ContextLoopLocker lock(context);
  switch(loopType) {
  case FM_FUNCPTR_ARRAY:
    throw Exception("Function pointer arrays are not supported as for loop index sets");
  case FM_STRUCT_ARRAY:
    throw Exception("Structure arrays are not supported as for loop index sets");
  case FM_CELL_ARRAY:
    ForLoopHelper<Array>(codeBlock,loopType,(const Array*)indexSet.getDataPointer(),
			  elementCount,indexVarName,this);
    break;
  case FM_LOGICAL:
    ForLoopHelper<logical>(codeBlock,loopType,(const logical*)indexSet.getDataPointer(),
			   elementCount,indexVarName,this);
    break;
  case FM_UINT8:
    ForLoopHelper<uint8>(codeBlock,loopType,(const uint8*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
  case FM_INT8:
    ForLoopHelper<int8>(codeBlock,loopType,(const int8*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
  case FM_UINT16:
    ForLoopHelper<uint16>(codeBlock,loopType,(const uint16*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
  case FM_INT16:
    ForLoopHelper<int16>(codeBlock,loopType,(const int16*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
  case FM_UINT32:
    ForLoopHelper<uint32>(codeBlock,loopType,(const uint32*)indexSet.getDataPointer(),
			  elementCount,indexVarName,this);
    break;
  case FM_INT32:
    ForLoopHelper<int32>(codeBlock,loopType,(const int32*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
  case FM_UINT64:
    ForLoopHelper<uint64>(codeBlock,loopType,(const uint64*)indexSet.getDataPointer(),
			  elementCount,indexVarName,this);
    break;
  case FM_INT64:
    ForLoopHelper<int64>(codeBlock,loopType,(const int64*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
  case FM_FLOAT:
    ForLoopHelper<float>(codeBlock,loopType,(const float*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
  case FM_DOUBLE:
    ForLoopHelper<double>(codeBlock,loopType,(const double*)indexSet.getDataPointer(),
			  elementCount,indexVarName,this);
    break;
  case FM_COMPLEX:
    ForLoopHelperComplex<float>(codeBlock,loopType,(const float*)indexSet.getDataPointer(),
				elementCount,indexVarName,this);
    break;
  case FM_DCOMPLEX:
    ForLoopHelperComplex<double>(codeBlock,loopType,(const double*)indexSet.getDataPointer(),
				 elementCount,indexVarName,this);
    break;
  case FM_STRING:
    ForLoopHelper<uint8>(codeBlock,loopType,(const uint8*)indexSet.getDataPointer(),
			 elementCount,indexVarName,this);
    break;
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
//!
void Interpreter::globalStatement(const tree &t) {
  for (unsigned i=0;i<t.numchildren();i++)
    context->addGlobalVariable(t.child(i).text());
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
//is the tagged as persistent.
//@@Example
//Here is an example of a function that counts how many
//times it has been called.
//@{ count_calls.m
//function count_calls
//  persistent ccount
//  if (~exist('ccount')) ccount = 0; end;
//  ccount = ccount + 1;
//  printf('Function has been called %d times\n',ccount);
//@}
//We now call the function several times:
//@<
//for i=1:10; count_calls; end
//@>
//!
void Interpreter::persistentStatement(const tree &t) {
  for (unsigned i=0;i<t.numchildren();i++)
    context->addPersistentVariable(t.child(i).text());
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
  try {
    evalCLI();
  } catch (InterpreterContinueException& e) {
  } catch (InterpreterBreakException& e) {
  } catch (InterpreterReturnException& e) {
  }
  depth--;
}

void Interpreter::displayArray(Array b) {
  // Check for a user defined class
  FuncPtr val;
  if (b.isUserClass() && ClassResolveFunction(this,b,"display",val)) {
    if (val->updateCode()) refreshBreakpoints();
    ArrayVector args(singleArrayVector(b));
    ArrayVector retvec(val->evaluateFunction(this,args,1));
  } else
    PrintArrayClassic(b,printLimit,this,true);
}

//Works
void Interpreter::expressionStatement(const tree &s, bool printIt) {
  ArrayVector m;
  if (!s.is(TOK_EXPR)) throw Exception("Unexpected statement type!");
  tree t(s.first());
  // There is a special case to consider here - when a 
  // function call is made as a statement, we do not require
  // that the function have an output.
  Array b;
  ArrayReference ptr;
  if (t.is(TOK_VARIABLE)) {
    ptr = context->lookupVariable(t.first().text());
    if (!ptr.valid()) {
      functionExpression(t,0,true,m);
      bool emptyOutput = false;
      if (m.size() == 0) {
	b = Array::emptyConstructor();
	emptyOutput = true;
      } else 
	b = m[0];
      if (printIt && (!emptyOutput)) {
	outputMessage(std::string("ans = \n"));
	displayArray(b);
      }
    } else {
      multiexpr(t,m);
      if (m.size() == 0)
	b = Array::emptyConstructor();
      else {
	b = m[0];
	if (printIt) {
	  outputMessage(std::string("ans = \n"));
	  for (int j=0;j<m.size();j++) {
	    char buffer[1000];
	    if (m.size() > 1) {
	      sprintf(buffer,"\n%d of %d:\n",j+1,m.size());
	      outputMessage(std::string(buffer));
	    }
	    displayArray(m[j]);
	  }
	}
      }
    }
  } else {
    b = expression(t);
    if (printIt) {
      outputMessage(std::string("ans = \n"));
      displayArray(b);
    } 
  }
  context->insertVariable("ans",b);
}

void Interpreter::multiassign(ArrayReference r, const tree &s, ArrayVector &data) {
  SaveEndInfo;
  endRef = r;
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numchildren();
    for (unsigned p = 0; p < s.numchildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->setVectorSubset(m[0],data[0]);
    else
      r->setNDimSubset(m,data[0]);
    data.pop_front();
  } else if (s.is(TOK_BRACES)) {
    ArrayVector m;
    endTotal = s.numchildren();
    for (unsigned p = 0; p < s.numchildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->setVectorContentsAsList(m[0],data);
    else
      r->setNDimContentsAsList(m,data);
  } else if (s.is('.')) {
    r->setFieldAsList(s.first().text(),data);
  } else if (s.is(TOK_DYN)) {
    const char *field;
    try {
      Array fname(expression(s.first()));
      field = fname.getContentsAsCString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    r->setFieldAsList(field,data);
  }
  RestoreEndInfo;
}

void Interpreter::assign(ArrayReference r, const tree &s, Array &data) {
  SaveEndInfo;
  endRef = r;  
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numchildren();
    for (unsigned p = 0; p < s.numchildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->setVectorSubset(m[0],data);
    else
      r->setNDimSubset(m,data);
  } else if (s.is(TOK_BRACES)) {
    ArrayVector datav(singleArrayVector(data));
    ArrayVector m;
    endTotal = s.numchildren();
    for (unsigned p = 0; p < s.numchildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->setVectorContentsAsList(m[0],datav);
    else
      r->setNDimContentsAsList(m,datav);
  } else if (s.is('.')) {
    ArrayVector datav(singleArrayVector(data));
    r->setFieldAsList(s.first().text(),datav);
  } else if (s.is(TOK_DYN)) {
    const char *field;
    try {
      Array fname(expression(s.first()));
      field = fname.getContentsAsCString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    ArrayVector datav(singleArrayVector(data));
    r->setFieldAsList(field,datav);
  }
  RestoreEndInfo;
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
//!
void Interpreter::assignment(const tree &var, bool printIt, Array &b) {
  string name(var.first().text());
  ArrayReference ptr(context->lookupVariable(name));
  if (!ptr.valid()) {
    context->insertVariable(name,Array::emptyConstructor());
    ptr = context->lookupVariable(name);
  }
  if (var.numchildren() == 1) {
    ptr->setValue(b);
  } else if (ptr->isUserClass() && 
	     !inMethodCall(ptr->className().back()) && 
	     !stopoverload) {
    ClassAssignExpression(ptr,var,b,this);
  } else if (var.numchildren() == 2)
    assign(ptr,var.second(),b);
  else {
    ArrayVector stack;
    Array data(*ptr);
    int varCount = var.numchildren();
    for (int index=1;index<varCount-1;index++) {
      if (!data.isEmpty()) {
	try {
	  deref(data,var.child(index));
	} catch (Exception &e) {
	  data = Array::emptyConstructor();
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
    outputMessage(name);
    outputMessage(" = \n");
    displayArray(*ptr);
  }
}

void Interpreter::processBreakpoints(const tree &t) {
//   qDebug() << "Executing statement:";
//  t.print();
  if (t.getBPflag()) {
    //    qDebug() << "Debug trap:";
    //    t.print();
    doDebugCycle();
    //    qDebug() << "Debug cycle complete...";
    SetContext(t.context());
    //    qDebug() << "Resuming this statement...";
    //    t.print();
  }
  if (tracetrap > 0) {
    //     qDebug() << "Trace trap active...";
    //     qDebug() << "  Line number " << 
    //       ((ip_context) & 0xffff) << "  " << tracecurrentline 
    // 	     << " trap " << tracetrap << "";
    if (((ip_context) & 0xffff) != tracecurrentline) {
      //      qDebug() << "Current line changed...";
      tracetrap--;
      if (tracetrap == 0)
	doDebugCycle();
    }
  }
  if (steptrap > 0) {
    //     qDebug() << "Step trap active...";
    //     qDebug() << "  Line number " << 
    //        ((ip_context) & 0xffff) << "  " << stepcurrentline 
    // 	     << " trap " << steptrap << "";
    if (((ip_context) & 0xffff) != stepcurrentline) {
      //      qDebug() << "Current line changed...";
      steptrap--;
      if (steptrap == 0)
	doDebugCycle();
    }
    // 	if ((steptrap == 1) && ((ip_detailname == stepname) || tracetrap) ) {
    // 	  qDebug() << "Step trap hit...";
    // 	  i->print();
    // 	  steptrap--;
    // 	  SetContext(i->context());
    // 	  doDebugCycle();
    // 	} else if ((steptrap > 1) && ((ip_detailname == stepname)||tracetrap))
    // 	  steptrap--;
  }
}

void Interpreter::statementType(const tree &t, bool printIt) {
  // check the debug flag
  SetContext(t.context());
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
    //    qDebug() << "**********************DBStep";
    dbstepStatement(t);
    emit RefreshBPLists();
    throw InterpreterReturnException();
    break;
  case TOK_DBTRACE:
    //    qDebug() << "**********************DBTrace";
    dbtraceStatement(t);
    emit RefreshBPLists();
    throw InterpreterReturnException();
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
void Interpreter::statement(const tree &t) {
  try {
    if (t.is(TOK_QSTATEMENT))
      statementType(t.first(),false);
    else if (t.is(TOK_STATEMENT))
      statementType(t.first(),true);
    else
      throw Exception("Unexpected statement type!\n");
  } catch (Exception& e) {
    if (autostop && !InCLI) {
      errorCount++;
      char buffer[4096];
      e.printMe(this);
      stackTrace(true);
      doDebugCycle();
    } else  {
      throw;
    }
  }
}

//Works
void Interpreter::block(const tree &t) {
  try {
    treeVector &statements(t.children());
    for (treeVector::iterator i=statements.begin();
	 i!=statements.end();i++) {
      if (m_interrupt)
	throw InterpreterQuitException();
      if (InterruptPending) {
	outputMessage("Interrupt (ctrl-c) encountered\n");
	stackTrace(true);
	InterruptPending = false;
	doDebugCycle();
      } else 
	statement(*i);
    }
  } catch (Exception &e) {
    lasterr = e.getMessageCopy();
    throw;
  }
}

Context* Interpreter::getContext() {
  return context;
}

// I think this is too complicated.... there should be an easier way
// Works
int Interpreter::countLeftHandSides(const tree &t) {
  Array lhs;
  ArrayReference ptr(context->lookupVariable(t.first().text()));
  if (!ptr.valid())
    lhs = Array::emptyConstructor();
  else
    lhs = *ptr;
  if (t.numchildren() == 1) return 1;
  if (t.last().is(TOK_PARENS)) return 1;
  for (unsigned index = 1;index < t.numchildren()-1;index++) {
    try {
      deref(lhs,t.child(index));
    } catch (Exception& e) {
      lhs = Array::emptyConstructor();
    }
  }
  if (t.last().is(TOK_BRACES)) {
    const tree &s(t.last());
    ArrayVector m;
    for (unsigned p = 0; p < s.numchildren(); p++) 
      multiexpr(s.child(p),m);
    subsindex(m);
    if (m.size() == 0)
      throw Exception("Expected indexing expression!");
    if (m.size() == 1) {
      // m[0] should have only one element...
      if (isColonOperator(m[0]))
	return (lhs.getLength());
      m[0].toOrdinalType();
      return (m[0].getLength());
    } else {
      int i=0;
      int outputCount=1;
      while (i<m.size()) {
	if (isColonOperator(m[i])) 
	  outputCount *= lhs.getDimensionLength(i);
	else {
	  m[i].toOrdinalType();
	  outputCount *= m[i].getLength();
	}
	i++;
      }
      return (outputCount);
    }
  }
  if (t.last().is('.')) 
    return std::max(1,lhs.getLength());
  return 1;
}

Array Interpreter::AllColonReference(Array v, int index, int count) {
  if (v.isUserClass()) return Array::emptyConstructor();
  return Array::stringConstructor(":");
}
  
//test
void Interpreter::specialFunctionCall(const tree &t, bool printIt) {
  tree fAST;
  ArrayVector m;
  stringVector args;
  for (unsigned index=0;index < t.numchildren();index++) 
    args.push_back(t.child(index).text());
  if (args.empty()) return;
  ArrayVector n;
  for (int i=1;i<args.size();i++)
    n.push_back(Array::stringConstructor(args[i].c_str()));
  FuncPtr val;
  if (!lookupFunction(args[0],val,n))
    throw Exception("unable to resolve " + args[0] + " to a function call");
  if (val->updateCode()) refreshBreakpoints();
  bool CLIFlagsave = InCLI;
  InCLI = false;
  try {
    if (!val->graphicsFunction)
      m = val->evaluateFunction(this,n,0);
    else
      m = doGraphicsFunction(val,n,0);
  } catch(Exception& e) {
    InCLI = CLIFlagsave;
    throw;
  }
  InCLI = CLIFlagsave;
}
 
void Interpreter::setBreakpoint(stackentry bp, bool enableFlag) {
  FuncPtr val;
  bool isFun = lookupFunction(bp.detail,val);
  if (!isFun) {
    warningMessage(string("unable to find function ") + 
		   bp.detail + " to set breakpoint");
    return;
  }
  if (val->type() != FM_M_FUNCTION) {
    warningMessage("function " + bp.detail + 
		   " is not an m-file, and does not support breakpoints");
    return;
  }
  try {
    ((MFunctionDef*)val)->SetBreakpoint(bp.tokid,enableFlag);
  } catch (Exception &e) {
    e.printMe(this);
  }
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
void Interpreter::multiFunctionCall(const tree &t, bool printIt) {
  ArrayVector m;
  treeVector s;
  Array c;
  int lhsCount;

  if (!t.first().is(TOK_BRACKETS))
    throw Exception("Illegal left hand side in multifunction expression");
  s = t.first().children();
  // We have to make multiple passes through the LHS part of the AST.
  // The first pass is to count how many function outputs are actually
  // being requested. 
  // Calculate how many lhs objects there are
  lhsCount = 0;
  for (unsigned index=0;index<s.size();index++) 
    lhsCount += countLeftHandSides(s[index]);

  multiexpr(t.second(),m,lhsCount);

  unsigned index;
  for (index=0;(index<s.size()) && (m.size() > 0);index++) {
    const tree &var(s[index]);
    string name(var.first().text());
    ArrayReference ptr(context->lookupVariable(name));
    if (!ptr.valid()) {
      context->insertVariable(name,Array::emptyConstructor());
      ptr = context->lookupVariable(name);
    }
    if (ptr->isUserClass() && 
	!inMethodCall(ptr->className().back()) && 
	!stopoverload) {
      ClassAssignExpression(ptr,t,m.front(),this);
      m.pop_front();
      return;
    }
    if (var.numchildren() == 1) {
      ptr->setValue(m.front());
      m.pop_front();
    } else if (var.numchildren() == 2)
      multiassign(ptr,var.second(),m);
    else {
      ArrayVector stack;
      Array data(*ptr);
      int varCount = var.numchildren();
      for (int index=1;index<varCount-1;index++) {
	if (!data.isEmpty()) {
	  try {
	    deref(data,var.child(index));
	  } catch (Exception &e) {
	    data = Array::emptyConstructor();
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

int getArgumentIndex(stringVector list, std::string t) {
  bool foundArg = false;
  std::string q;
  uint32 i;
  i = 0;
  while (i<list.size() && !foundArg) {
    q = list[i];
    if (q[0] == '&')
      q.erase(0,1);
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
//@Module NARGIN Number of Input Arguments
//@@Section FUNCTIONS
//@@Usage
//The special variable @|nargin| is defined inside of all
//functions.  It indicates how many arguments were passed
//to the function when it was called.  FreeMat allows for
//fewer arguments to be passed to a function than were declared,
//and @|nargin|, along with @|isset| can be used to determine
//exactly what subset of the arguments were defined.
//There is no syntax for the use of @|nargin| - it is 
//automatically defined inside the function body.
//@@Example
//Here is a function that is declared to take five 
//arguments, and that simply prints the value of @|nargin|
//each time it is called.
//@{ nargintest.m
//function nargintest(a1,a2,a3,a4,a5)
//  printf('nargin = %d\n',nargin);
//@}
//@<
//nargintest(3);
//nargintest(3,'h');
//nargintest(3,'h',1.34);
//nargintest(3,'h',1.34,pi,e);
//@>
//!

//!
//@Module NARGOUT Number of Output Arguments
//@@Section FUNCTIONS
//@@Usage
//The special variable @|nargout| is defined inside of all
//functions.  It indicates how many return values were requested from
//the function when it was called.  FreeMat allows for
//fewer return values to be requested from a function than were declared,
//and @|nargout| can be used to determine exactly what subset of 
//the functions outputs are required.  There is no syntax for 
//the use of @|nargout| - it is automatically defined inside 
//the function body.
//@@Example
//Here is a function that is declared to return five 
//values, and that simply prints the value of @|nargout|
//each time it is called.
//@{ nargouttest.m
//function [a1,a2,a3,a4,a5] = nargouttest
//  printf('nargout = %d\n',nargout);
//  a1 = 1; a2 = 2; a3 = 3; a4 = 4; a5 = 5;
//@}
//@<
//a1 = nargouttest
//[a1,a2] = nargouttest
//[a1,a2,a3] = nargouttest
//[a1,a2,a3,a4,a5] = nargouttest
//@>
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
void Interpreter::collectKeywords(const tree &q, ArrayVector &keyvals,
				  treeVector &keyexpr, stringVector &keywords) {
  // Search for the keyword uses - 
  // To handle keywords, we make one pass through the arguments,
  // recording a list of keywords used and using ::expression to
  // evaluate their values. 
  for (unsigned index=0;index < q.numchildren();index++) {
    if (q.child(index).is(TOK_KEYWORD)) {
      keywords.push_back(q.child(index).first().text());
      if (q.child(index).numchildren() > 1) {
	keyvals.push_back(expression(q.child(index).second()));
	keyexpr.push_back(q.child(index).second());
      } else {
	keyvals.push_back(Array::logicalConstructor(true));
	keyexpr.push_back(tree());
      }
    }
  }
}

int* Interpreter::sortKeywords(ArrayVector &m, stringVector &keywords,
			       stringVector arguments, ArrayVector keyvals) {
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
      toFill[i] = Array::emptyConstructor();
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
void Interpreter::handlePassByReference(const tree &q, stringVector arguments,
					ArrayVector m,stringVector keywords, 
					treeVector keyexpr, int* argTypeMap) {
  tree p;
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
      if (qindx >= q.second().numchildren())
	qindx = q.second().numchildren()-1;
    }
    std::string args(arguments[i]);
    if (args[0] == '&') {
      args.erase(0,1);
      // This argument was passed by reference
      if (!p.valid() || !(p.is(TOK_VARIABLE)))
	throw Exception("Must have lvalue in argument passed by reference");
      assignment(p,false,m[i]);
    }
  }
}

static ArrayVector mergeVecs(ArrayVector a, ArrayVector b) {
  for (int i=0;i<b.size();i++)
    a.push_back(b[i]);
  return a;
}

//Test
void Interpreter::functionExpression(const tree &t, 
				     int narg_out, 
				     bool outputOptional,
				     ArrayVector &output) {
  ArrayVector m, n;
  stringVector keywords;
  ArrayVector keyvals;
  treeVector keyexpr;
  int i;
  char buffer[2048];
  FuncPtr funcDef;
  int* argTypeMap;
  bool CLIFlagsave;
  CLIFlagsave = InCLI;
  int ctxt = t.context();

    
  try {
    // Because of the introduction of user-defined classes, we have to 
    // first evaluate the keywords and the arguments, before we know
    // which function to call.
    // First, check for arguments
    if ((t.numchildren()>=2) && t.second().is(TOK_PARENS)) {
      // Collect keywords
      collectKeywords(t.second(),keyvals,keyexpr,keywords);
      // Evaluate function arguments
      try {
	const tree &s(t.second());
	for (unsigned p=0;p<s.numchildren();p++)
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
    if (funcDef->updateCode()) refreshBreakpoints();
    if (funcDef->scriptFlag) {
      if (t.numchildren()>=2)
	throw Exception(std::string("Cannot use arguments in a call to a script."));
      if ((narg_out > 0) && !outputOptional)
	throw Exception(std::string("Cannot assign outputs in a call to a script."));
      CLIFlagsave = InCLI;
      InCLI = false;
      pushDebug(((MFunctionDef*)funcDef)->fileName,
		((MFunctionDef*)funcDef)->name);
      block(((MFunctionDef*)funcDef)->code);
      if ((steptrap >= 1) && (ip_detailname == stepname)) {
	if ((cstack.size() > 0) && (cstack.back().cname != "Eval")) {
	  warningMessage("dbstep beyond end of script " + stepname +
			 ".\n Setting single step mode for " + 
			 cstack.back().detail);
	  stepname = cstack.back().detail;
	} else
	  steptrap = 0;
      }
      popDebug();
      InCLI = CLIFlagsave;
    } else {
      // We can now adjust the keywords (because we know the argument list)
      // Apply keyword mapping
      if (!keywords.empty()) 
	argTypeMap = sortKeywords(m,keywords,funcDef->arguments,keyvals);
      else
	argTypeMap = NULL;
      if ((funcDef->inputArgCount() >= 0) && 
	  (m.size() > funcDef->inputArgCount()))
	throw Exception(std::string("Too many inputs to function ")+t.first().text());
      if ((funcDef->outputArgCount() >= 0) && 
	  (narg_out > funcDef->outputArgCount() && !outputOptional))
	throw Exception(std::string("Too many outputs to function ")+t.first().text());
      CLIFlagsave = InCLI;
      InCLI = false;
      if (!funcDef->graphicsFunction)
	n = funcDef->evaluateFunction(this,m,narg_out);
      else
	n = doGraphicsFunction(funcDef,m,narg_out);
      if ((steptrap >= 1) && (funcDef->name == stepname)) {
	if ((cstack.size() > 0) && (ip_funcname != "Eval")) {
	  warningMessage("dbstep beyond end of function " + stepname +
			 ".\n Setting single step mode for " + 
			 ip_detailname);
	  stepname = ip_detailname;
	} else
	  steptrap = 0;
      }
      InCLI = CLIFlagsave;
      // Check for any pass by reference
      if (t.haschildren() && (funcDef->arguments.size() > 0)) 
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
  } catch (Exception& e) {
    InCLI = CLIFlagsave;
    throw;
  } catch (InterpreterRetallException& e) {
    throw;
  }
}

void Interpreter::toggleBP(QString fname, int lineNumber) {
  if (isBPSet(fname,lineNumber)) {
    string fname_string(fname.toStdString());
    for (int i=0;i<bpStack.size();i++) 
      if ((bpStack[i].cname == fname_string) &&
	  ((bpStack[i].tokid & 0xffff) == lineNumber)) {
	//	qDebug() << "Deleting bp " << i << " w/number " << bpStack[i].number << "";
	deleteBreakpoint(bpStack[i].number);
	return;
      }
  } else {
    addBreakpoint(fname.toStdString(),lineNumber);
  }    
}

MFunctionDef* Interpreter::lookupFullPath(string fname) {
  stringVector allFuncs(context->listAllFunctions());
  FuncPtr val;
  for (int i=0;i<allFuncs.size();i++) {
    bool isFun = context->lookupFunction(allFuncs[i],val);
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      if (mptr->fileName ==  fname) return mptr;
    }
  }
  return NULL;
}

static int bpList = 1;
void Interpreter::addBreakpoint(string name, int line) {
  //  qDebug() << "Add bp " << QString::fromStdString(name) << " : " << line << "";
  FuncPtr val;
  MFunctionDef *mptr;
  if (context->lookupFunction(name,val) && (val->type() == FM_M_FUNCTION))
    mptr = (MFunctionDef*) val;
  else
    mptr = lookupFullPath(name);
  if (!mptr || (mptr->type() != FM_M_FUNCTION)) {
    warningMessage(std::string("Cannot resolve ")+name+std::string(" to a function or script "));
    return;
  }
  mptr->updateCode();
  int dline = mptr->ClosestLine(line);
  if (dline != line)
    warningMessage(string("Breakpoint moved to line ") + dline + " of " + name);
  addBreakpoint(stackentry(mptr->fileName,mptr->name,dline,bpList++));
}

bool Interpreter::isBPSet(QString fname, int lineNumber) {
  string fname_string(fname.toStdString());
  for (int i=0;i<bpStack.size();i++) 
    if ((bpStack[i].cname == fname_string) &&
	((bpStack[i].tokid & 0xffff) == lineNumber)) return true;
  return false;
}

bool Interpreter::isInstructionPointer(QString fname, int lineNumber) {
  return ((fname.toStdString() == ip_funcname) && ((lineNumber == (ip_context & 0xffff)) || ((lineNumber == 1) && ((ip_context & 0xffff) == 0))));
}

void Interpreter::listBreakpoints() {
  for (int i=0;i<bpStack.size();i++) {
    //    if (bpStack[i].number > 0) {
    char buffer[2048];
    sprintf(buffer,"%d   %s line %d\n",bpStack[i].number,
	    bpStack[i].cname.c_str(),bpStack[i].tokid & 0xffff);
    outputMessage(buffer);
    //    }
  }
}

void Interpreter::deleteBreakpoint(int number) {
  for (int i=0;i<bpStack.size();i++) 
    if (bpStack[i].number == number) {
      setBreakpoint(bpStack[i],false);
      bpStack.erase(bpStack.begin()+i);
      emit RefreshBPLists();
      return;
    } 
  warningMessage("Unable to delete specified breakpoint (does not exist)");
  emit RefreshBPLists();
  return;
}

void Interpreter::stackTrace(bool includeCurrent) {
  for (int i=0;i<cstack.size();i++) {
    std::string cname_trim(TrimExtension(TrimFilename(cstack[i].cname)));
    outputMessage(string("In ") + cname_trim + "("
		  + cstack[i].detail + ") on line " +
		  (cstack[i].tokid & 0x0000FFFF) + "\r\n");
  }
  if (includeCurrent) {
    std::string ip_trim(TrimExtension(TrimFilename(ip_funcname)));
    outputMessage(string("In ") + ip_trim + "("
		  + ip_detailname + ") on line " +
		  (ip_context & 0x0000FFFF) + "\r\n");
  }
}

bool Interpreter::inMethodCall(std::string classname) {
  if (ip_detailname.empty()) return false;
  if (ip_detailname[0] != '@') return false;
  return (ip_detailname.compare(1,classname.size(),classname)==0);
}

void Interpreter::pushDebug(std::string fname, std::string detail) {
  cstack.push_back(stackentry(ip_funcname,ip_detailname,
			      ip_context,0,steptrap,
			      stepcurrentline));
  //  qDebug() << "Push Debug: " << QString::fromStdString(fname) << ",";
  //  qDebug() << QString::fromStdString(detail) << "";
  ip_funcname = fname;
  ip_detailname = detail;
  ip_context = 0;
  steptrap = 0;
  stepcurrentline = 0;
}

void Interpreter::popDebug() {
  if (!cstack.empty()) {
    //    qDebug() << "Pop Debug";
    ip_funcname = cstack.back().cname;
    ip_detailname = cstack.back().detail;
    ip_context = cstack.back().tokid;
    steptrap = cstack.back().steptrap;
    stepcurrentline = cstack.back().stepcurrentline;
    cstack.pop_back();
  } else
    outputMessage("IDERROR\n");
}

bool Interpreter::isUserClassDefined(std::string classname) {
  UserClass *ret;
  return (classTable.findSymbol(classname)!=NULL);
}
  
UserClass Interpreter::lookupUserClass(std::string classname) {
  return(*(classTable.findSymbol(classname)));
}

void Interpreter::registerUserClass(std::string classname, UserClass cdata) {
  classTable.insertSymbol(classname,cdata);
}


bool Interpreter::lookupFunction(std::string funcName, FuncPtr& val) {
  ArrayVector dummy;
  return(lookupFunction(funcName,val,dummy));
}

// Look up a function by name.  Use the arguments (if available) to assist
// in resolving method calls for objects
bool Interpreter::lookupFunction(std::string funcName, FuncPtr& val, 
				 ArrayVector &args, bool disableOverload) {
  int passcount = 0;
  while(passcount < 2) {
    // This is the order for function dispatch according to the Matlab manual
    // Subfunctions
    if (context->lookupFunctionLocally(funcName,val))
      return true;
    // Private functions
    // Not sure if you have to be an M-file in the current directory
    // to access a private function...
    if (context->lookupFunctionGlobally(getPrivateMangledName(funcName),val))
      return true;
    // Class constructor functions
    if (context->lookupFunctionGlobally(ClassMangleName(funcName,funcName),val))
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
    if (context->lookupFunction(funcName,val))
      return true;
    if (passcount == 0)
      rescanPath();
    passcount++;
  }
  return false;
}

//!
//@Module Function Handles
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
//Works
ArrayVector Interpreter::FunctionPointerDispatch(Array r, const tree &args, 
						 int narg_out) {
  const FunctionDef** dp;
  bool CLIFlagsave;
  dp = (const FunctionDef**) r.getDataPointer();
  FunctionDef* fun = (FunctionDef*) dp[0];
  if (!fun) return ArrayVector();
  if (!args.is(TOK_PARENS))
    throw Exception("Expected either '()' or function arguments inside parenthesis");
  ArrayVector m;
  for (unsigned p = 0; p< args.numchildren(); p++)
    multiexpr(args.child(p),m);
  ArrayVector n;
  if (fun->updateCode()) refreshBreakpoints();
  if (fun->scriptFlag) {
    if (!m.empty())
      throw Exception(std::string("Cannot use arguments in a call to a script."));
    CLIFlagsave = InCLI;
    InCLI = false;
    pushDebug(((MFunctionDef*)fun)->fileName,
	      ((MFunctionDef*)fun)->name);
    try {
      block(((MFunctionDef*)fun)->code);
    } catch (InterpreterReturnException& e) {
    }
    popDebug();
    InCLI = CLIFlagsave;
  } else {
    CLIFlagsave = InCLI;
    InCLI = false;
    //HACK!
    int narg_out = 1;
    n = fun->evaluateFunction(this,m,narg_out);
    InCLI = CLIFlagsave;
  }
  return n;
}

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
void Interpreter::deref(Array &r, const tree &s) {
  SaveEndInfo;
  endRef = &r;
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numchildren();
    for (unsigned p = 0; p < s.numchildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r = r.getVectorSubset(m[0]);
    else
      r = r.getNDimSubset(m);
  } else if (s.is(TOK_BRACES)) {
    ArrayVector m;
    endTotal = s.numchildren();
    for (unsigned p = 0; p < s.numchildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r = r.getVectorContents(m[0]);
    else
      r = r.getNDimContents(m);
  } else if (s.is('.')) {
    r = r.getField(s.first().text());
  } else if (s.is(TOK_DYN)) {
    const char *field;
    try {
      Array fname(expression(s.first()));
      field = fname.getContentsAsCString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    r = r.getField(field);
  }
  RestoreEndInfo;
}
 
 Array Interpreter::rhs(const tree &t) {
   ArrayReference ptr(context->lookupVariable(t.first().text()));
   if (!ptr.valid()) {
     ArrayVector m;
     functionExpression(t,1,false,m);
     if (m.size() >= 1)
       return m[0];
     else
       return Array::emptyConstructor();
   }
   if ((ptr->dataClass() == FM_FUNCPTR_ARRAY &&
	ptr->isScalar()) && (t.numchildren() > 1)) {
     ArrayVector m(FunctionPointerDispatch(*ptr,t.second(),1));
     if (m.size() >= 1)
       return m[0];
     else
       return Array::emptyConstructor();
   }
   if (t.numchildren() == 1)
     return *ptr;
   if (ptr->isUserClass() && !stopoverload && !inMethodCall(ptr->className().back())) {
     ArrayVector m(ClassRHSExpression(*ptr,t,this));
     if (m.size() >= 1)
       return m[0];
     else
       return Array::emptyConstructor();
   }
   Array r(*ptr);
   for (unsigned index = 1;index < t.numchildren();index++) 
     deref(r,t.child(index));
   return r;
 }

  
int Interpreter::getErrorCount() {
  return errorCount;
}

Interpreter::Interpreter(Context* aContext) {
  errorCount = 0;
  lasterr = string("");
  context = aContext;
  depth = 0;
  InterruptPending = false;
  myInterp = this;
  Array::setArrayInterpreter(this);
  signal(SIGINT,sigInterrupt);
  printLimit = 1000;
  autostop = false;
  InCLI = false;
  stopoverload = false;
  m_skipflag = false;
  clearStacks();
  steptrap = 0;
  stepcurrentline = 0;
  tracetrap = 0;
  tracecurrentline = 0;
  endRef = NULL;
  m_threadErrorState = false;
  m_interrupt = false;
}

Interpreter::~Interpreter() {
}


bool Interpreter::getStopOverload() {
  return stopoverload;
}

void Interpreter::setStopOverload(bool flag) {
  stopoverload = flag;
}

// We want dbstep(n) to cause us to advance n statements and then
// stop.  we execute statement-->set step trap,

void Interpreter::dbstepStatement(const tree &t) {
  int lines = 1;
  if (t.haschildren()) {
    Array lval(expression(t.first()));
    lines = lval.getContentsAsIntegerScalar();
  }
  // Get the current function
  if (cstack.size() < 1) throw Exception("cannot dbstep unless inside an M-function");
  stackentry bp(cstack[cstack.size()-1]);
  FuncPtr val;
  if (bp.detail == "base") return;
  if (!lookupFunction(bp.detail,val)) {
    warningMessage(string("unable to find function ") + bp.detail + " to single step");
    return;
  }
  cstack[cstack.size()-1].steptrap = lines;
  cstack[cstack.size()-1].stepcurrentline = bp.tokid & 0xffff;
//   qDebug() << "setting dbstep trap to current line " << 
//     cstack[cstack.size()-1].stepcurrentline << 
//     " with wait of " << lines << " lines";
}

void Interpreter::dbtraceStatement(const tree &t) {
  int lines = 1;
  if (t.haschildren()) {
    Array lval(expression(t.first()));
    lines = lval.getContentsAsIntegerScalar();
  }
  // Get the current function
  if (cstack.size() < 1) throw Exception("cannot dbtrace unless inside an M-function");
  stackentry bp(cstack[cstack.size()-1]);
  FuncPtr val;
  if (bp.detail == "base") return;
  if (!lookupFunction(bp.detail,val)) {
    warningMessage(string("unable to find function ") + bp.detail + " to single step");
    return;
  }
  tracetrap = lines;
  tracecurrentline = bp.tokid & 0xffff;
//   qDebug() << "setting dbtrace trap to current line " << 
//     tracecurrentline << " with wait of " << lines << " lines";
}

static string EvalPrep(string line) {
  string buf1 = line;
  if (buf1[buf1.size()-1] == '\n')
    buf1.erase(buf1.end()-1);
  if (buf1[buf1.size()-1] == '\r')
    buf1.erase(buf1.end()-1);
  if (buf1.size() > 20)
    buf1 = string(buf1,0,20) + "...";
  return buf1;
}

void Interpreter::ExecuteLine(std::string txt) {
  mutex.lock();
  cmd_buffer.push_back(txt);
  bufferNotEmpty.wakeAll();
  mutex.unlock();
}

//PORT
void Interpreter::evaluateString(string line, bool propogateExceptions) {
  tree t;
  InterruptPending = false;
  Scanner S(line,"");
  Parser P(S);
  try{
    t = P.Process();
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
    pushDebug("Eval",EvalPrep(line));
    try {
      block(t);
    } catch (InterpreterReturnException& e) {
      if (depth > 0) {
	popDebug();
	throw;
      }
    } catch (InterpreterQuitException& e) {
      popDebug();
      throw;
    } catch (InterpreterRetallException& e) {
      popDebug();
      throw;
    }
  } catch(Exception &e) {
    if (propogateExceptions) {
      popDebug();
      throw;
    }
    errorCount++;
    e.printMe(this);
  }
  popDebug();
}
  
string Interpreter::getLastErrorString() {
  return lasterr;
}

void Interpreter::setLastErrorString(string txt) {
  lasterr = txt;
}

void Interpreter::setGreetingFlag(bool skip) {
  m_skipflag = skip;
}

bool NeedsMoreInput(Interpreter *eval, string txt) {
  // Check for ... or an open []
  try {
    Scanner S(txt,"");
    while (!S.Next().Is(TOK_EOF))
      S.Consume();
    if (S.InContinuationState() || S.InBracket()) return true;
  } catch (Exception &e) {
  }
  try {
    Scanner S(txt,"");
    Parser P(S);
    tree root = P.Process();
    return false;
  } catch (Exception &e) {
    if (e.getMessageCopy().substr(0,13) == "Expecting end") {
      return true;
    }
  }
  return false;
}

string Interpreter::getLine(string prompt) {
  emit SetPrompt(prompt);
  string retstring;
  mutex.lock();
  if (cmd_buffer.empty())
    bufferNotEmpty.wait(&mutex);
  retstring = cmd_buffer.front();
  cmd_buffer.erase(cmd_buffer.begin());
  mutex.unlock();
  return retstring;
}


// This is a "generic" CLI routine.  The user interface (non-debug)
// version of this is "docli"
void Interpreter::evalCLI() {
  char prompt[150];
  bool rootCLI;

  if ((depth == 0) || (cstack.size() == 0)) {
    sprintf(prompt,"--> ");
    rootCLI = true;
  } else {
    sprintf(prompt,"[%s,%d]--> ",ip_detailname.c_str(),
	    ip_context & 0xffff);
    rootCLI = false;
  }
  while(1) {
    if (rootCLI) {
      tracetrap = 0;
      steptrap = 0;
    }
    emit SetPrompt(prompt);
//     qDebug() << "IP: " << QString::fromStdString(ip_detailname) << ", " << (ip_context & 0xffff) << "";
    emit ShowActiveLine();
    string cmdset;
    std::string cmdline;
    mutex.lock();
    while ((cmdset.empty() || 
	    NeedsMoreInput(this,cmdset)) && (!InterruptPending)) {
      if (cmd_buffer.empty())
	bufferNotEmpty.wait(&mutex);
      cmdline = cmd_buffer.front();
      cmd_buffer.erase(cmd_buffer.begin());
      cmdset += cmdline;
    }
    mutex.unlock();
    if (InterruptPending) {
      InterruptPending = false;
      continue;
    }
    int stackdepth = cstack.size();
    InCLI = true;
    evaluateString(cmdset);
    while (cstack.size() > stackdepth) cstack.pop_back();
  }
}

  
//
// Convert a list of variable into indexing expressions
//  - for user defined classes, we call subsindex for 
//  - the object
Array Interpreter::subsindex(const Array &m) {
  if (m.isUserClass() && !stopoverload) {
    Array t(ClassUnaryOperator(m,"subsindex",this));
    t.promoteType(FM_UINT32);
    return Add(t,Array::uint32Constructor(1));
  }
  return m;
}

 void Interpreter::subsindex(ArrayVector& m) {
   for (unsigned p=0;p<m.size();p++)
     m[p] = subsindex(m[p]);
 }
