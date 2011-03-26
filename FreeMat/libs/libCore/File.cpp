/*
 * Copyright (c) 2009 Samit Basu
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

#include <QtCore>
#include "Array.hpp"
#include "HandleList.hpp"
#include "IEEEFP.hpp"
#include "MemPtr.hpp"
#include "Printf.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"

HandleList<FilePtr*> fileHandles;

static bool init = false;

void InitializeFileSubsystem() {
  if (init) 
    return;
  FilePtr *fptr = new FilePtr();
  fptr->fp = new QFile();
  fptr->fp->open(stdin,QIODevice::ReadOnly);
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  fptr = new FilePtr();
  fptr->fp = new QFile();
  fptr->fp->open(stdout,QIODevice::WriteOnly);
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  fptr = new FilePtr();
  fptr->fp = new QFile();
  fptr->fp->open(stderr,QIODevice::WriteOnly);
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  init = true;
}


#define MATCH(x) (prec==x)

static DataClass processPrecisionString(QString prec) {
  prec = prec.trimmed().toLower();
  if (MATCH("uint8") || MATCH("uchar") || MATCH("unsigned char"))
    return UInt8;
  if (MATCH("int8") || MATCH("char") || MATCH("integer*1"))
    return Int8;
  if (MATCH("uint16") || MATCH("unsigned short")) 
    return UInt16;
  if (MATCH("int16") || MATCH("short") || MATCH("integer*2"))
    return Int16;
  if (MATCH("uint32") || MATCH("unsigned int")) 
    return UInt32;
  if (MATCH("int32") || MATCH("int") || MATCH("integer*4"))
    return Int32;
  if (MATCH("uint64"))
    return UInt64;
  if (MATCH("int64") || MATCH("integer*8"))
    return Int64;
  if (MATCH("single") || MATCH("float32") || MATCH("float") || MATCH("real*4"))
    return Float;
  if (MATCH("double") || MATCH("float64") || MATCH("real*8"))
    return Double;
  throw Exception("invalid precision type");
}
#undef MATCH

void ComputePrecisionString(QString cmd, DataClass &in, DataClass &out) {
  // Check for type => type
  QRegExp rxlen("(.*)=>(.*)");
  int pos = rxlen.indexIn(cmd);
  if (pos > -1) {
    in = processPrecisionString(rxlen.cap(1));
    out = processPrecisionString(rxlen.cap(2));
    if (rxlen.cap(2).trimmed().toLower() == "char") out = StringArray;
    return;
  }
  if (cmd.startsWith("*")) {
    cmd.remove(0,1);
    in = processPrecisionString(cmd);
    out = in;
    if (cmd.trimmed().toLower() == "char") out = StringArray;
    return;
  }
  in = processPrecisionString(cmd);
  out = Double;
}

//!
//@Module FOPEN File Open Function
//@@Section IO
//@@Usage
//Opens a file and returns a handle which can be used for subsequent
//file manipulations.  The general syntax for its use is
//@[
//  fp = fopen(fname,mode,byteorder)
//@]
//Here @|fname| is a string containing the name of the file to be 
//opened.  @|mode| is the mode string for the file open command.
//The first character of the mode string is one of the following:
//\begin{itemize}
//  \item @|'r'|  Open  file  for  reading.  The file pointer is placed at
//          the beginning of the file.  The file can be read from, but
//	  not written to.
//  \item @|'r+'|   Open for reading and writing.  The file pointer is
//          placed at the beginning of the file.  The file can be read
//	  from and written to, but must exist at the outset.
//  \item @|'w'|    Open file for writing.  If the file already exists, it is
//          truncated to zero length.  Otherwise, a new file is
//	  created.  The file pointer is placed at the beginning of
//	  the file.
//  \item @|'w+'|   Open for reading and writing.  The file is created  if  
//          it  does not  exist, otherwise it is truncated to zero
//	  length.  The file pointer placed at the beginning of the file.
//  \item @|'a'|    Open for appending (writing at end of file).  The file  is  
//          created  if it does not exist.  The file pointer is placed at
//	  the end of the file.
//  \item @|'a+'|   Open for reading and appending (writing at end of file).   The
//          file  is created if it does not exist.  The file pointer is
//	  placed at the end of the file.
//\end{itemize}
//Starting with FreeMat 4, all files are treated as binary files by default.
//To invoke the operating systems 'CR/LF <-> CR' translation (on Win32)
//add a 't' to the mode string, as in 'rt+'.
//
//Also, you can supply a second argument to @|fopen| to retrieve error
//messages if the @|fopen| fails.
//@[
//  [fp,messages] = fopen(fname,mode,byteorder)
//@]
//
//Finally, FreeMat has the ability to read and write files of any
//byte-sex (endian).  The third (optional) input indicates the 
//byte-endianness of the file.  If it is omitted, the native endian-ness
//of the machine running FreeMat is used.  Otherwise, the third
//argument should be one of the following strings:
//\begin{itemize}
//   \item @|'le','ieee-le','little-endian','littleEndian','little','l','ieee-le.l64','s'|
//   \item @|'be','ieee-be','big-endian','bigEndian','big','b','ieee-be.l64','a'|
//\end{itemize}
//	
//If the file cannot be opened, or the file mode is illegal, then
//an error occurs. Otherwise, a file handle is returned (which is
//an integer).  This file handle can then be used with @|fread|,
//@|fwrite|, or @|fclose| for file access.
//
//Note that three handles are assigned at initialization time:
//\begin{itemize}
//   \item Handle 0 - is assigned to standard input
//   \item Handle 1 - is assigned to standard output
//   \item Handle 2 - is assigned to standard error
//\end{itemize}
//These handles cannot be closed, so that user created file handles start at @|3|.
//
//@@Examples
//Here are some examples of how to use @|fopen|.  First, we create a new 
//file, which we want to be little-endian, regardless of the type of the machine.
//We also use the @|fwrite| function to write some floating point data to
//the file.
//@<
//fp = fopen('test.dat','w','ieee-le')
//fwrite(fp,float([1.2,4.3,2.1]))
//fclose(fp)
//@>
//Next, we open the file and read the data back
//@<
//fp = fopen('test.dat','r','ieee-le')
//fread(fp,[1,3],'float')
//fclose(fp)
//@>
//Now, we re-open the file in append mode and add two additional @|float|s to the
//file.
//@<
//fp = fopen('test.dat','a+','le')
//fwrite(fp,float([pi,e]))
//fclose(fp)
//@>
//Finally, we read all 5 @|float| values from the file
//@<
//fp = fopen('test.dat','r','ieee-le')
//fread(fp,[1,5],'float')
//fclose(fp)
//@>
//@@Signature
//function fopen FopenFunction
//inputs fname mode byteorder
//outputs handle message
//!
ArrayVector FopenFunction(int nargout, const ArrayVector& arg) {
  try {
    uint32 testEndian = 0xFEEDFACE;
    uint8 *dp;
    bool bigEndian;
    
    dp = (uint8*) &testEndian;
    bigEndian = (dp[0] == 0xFE);
    
    if (arg.size() > 3)
      throw Exception("too many arguments to fopen");
    if (arg.size() < 1)
      throw Exception("fopen requires at least one argument (a filename)");
    if (!(arg[0].isString()))
      throw Exception("First argument to fopen must be a filename");
    QString fname = arg[0].asString();
    QString mode = "r";
    if (arg.size() > 1) {
      if (!arg[1].isString())
	throw Exception("Access mode to fopen must be a string");
      mode = arg[1].asString();
    }
    bool swapendian = false;
    if (arg.size() > 2) {
      QString swapflag = arg[2].asString();
      if (swapflag=="swap") {
	swapendian = true;
      } else if ((swapflag=="le") ||
		 (swapflag=="ieee-le") ||
		 (swapflag=="little-endian") ||
		 (swapflag=="littleEndian") ||
		 (swapflag=="little") ||
		 (swapflag == "l") ||
		 (swapflag == "ieee-le.l64") ||
		 (swapflag == "s")) {
	swapendian = bigEndian;
      } else if ((swapflag=="be") ||
		 (swapflag=="ieee-be") ||
		 (swapflag=="big-endian") ||
		 (swapflag=="bigEndian") ||
		 (swapflag=="big") ||
		 (swapflag=="b") ||
		 (swapflag=="ieee-be.l64") ||
		 (swapflag=="a")) {
	swapendian = !bigEndian;
      } else if (!arg[2].isEmpty())
	throw Exception("swap flag must be 'swap' or an endian spec (see help fopen for the complete list)");
    }
    QFlags<QIODevice::OpenModeFlag> modeFlag;
    mode = mode.toLower();
    // r  means ReadOnly && MustExist
    // r+ means ReadOnly | WriteOnly && MustExist
    // w  means Writeonly | Truncate
    // w+ means ReadOnly | WriteOnly | Truncate
    // a  means Append | WriteOnly
    // a+ means Append | WriteOnly | ReadOnly
    // t  means * | Text 
    bool mustExist = false;
    bool has_r = mode.contains("r");
    bool has_p = mode.contains("+");
    bool has_w = mode.contains("w");
    bool has_a = mode.contains("a");
    bool has_t = mode.contains("t");
    if (has_r) {
      modeFlag |= QIODevice::ReadOnly;
      mustExist = true;
    }
    if (has_w) modeFlag |= QIODevice::WriteOnly;
    if (has_p) modeFlag |= QIODevice::ReadOnly;
    if (has_p && has_r)  modeFlag |= QIODevice::WriteOnly;
    if (has_w) modeFlag |= QIODevice::Truncate;
    if (has_a) modeFlag |= QIODevice::Append | QIODevice::WriteOnly;
    if (has_t) modeFlag |= QIODevice::Text;
    
    FilePtr *fptr = new FilePtr();
    fptr->fp = new QFile(fname);
    if (mustExist && !fptr->fp->exists())
      throw Exception("Access mode " + mode + " requires file to exist ");
    if (!fptr->fp->open(modeFlag))
      throw Exception(fptr->fp->errorString() + QString(" for fopen argument ") + fname);
    fptr->swapflag = swapendian;
    unsigned int rethan = fileHandles.assignHandle(fptr);
    return ArrayVector(Array(double(rethan-1)));
  } catch (Exception& e) {
    if (nargout > 1) {
      ArrayVector ret;
      ret.push_back(Array(double(-1)));
      ret.push_back(Array(e.msg()));
      return ret;
    }
    throw;
  }
}

//!
//@Module FCLOSE File Close Function
//@@Section IO
//@@Usage
//Closes a file handle, or all open file handles.  The general syntax
//for its use is either
//@[
//  fclose(handle)
//@]
//or
//@[
//  fclose('all')
//@]
//In the first case a specific file is closed,  In the second, all open
//files are closed.  Note that until a file is closed the file buffers
//are not flushed.  Returns a '0' if the close was successful and a '-1' if
//the close failed for some reason.
//@@Example
//A simple example of a file being opened with @|fopen| and then closed with @|fclose|.
//@<
//fp = fopen('test.dat','wb','ieee-le')
//fclose(fp)
//@>
//@@Signature
//function fclose FcloseFunction
//inputs handle
//outputs none
//!
ArrayVector FcloseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Fclose must have one argument, either 'all' or a file handle");
  bool closingAll = false;
  if (arg[0].isString()) {
    QString allflag = arg[0].asString().toLower();
    if (allflag == "all") {
      closingAll = true;
      int maxHandle(fileHandles.maxHandle());
      for (int i=3;i<maxHandle;i++) {
	try {
	  FilePtr* fptr = fileHandles.lookupHandle(i+1);
	  delete fptr->fp;
	  fileHandles.deleteHandle(i+1);
	  delete fptr;
	} catch (Exception & e) {
	}
      }
    } else
      throw Exception("Fclose must have one argument, either 'all' or a file handle");
  } else {
    Array tmp(arg[0]);
    int handle = tmp.asInteger();
    if (handle <= 2)
      throw Exception("Cannot close handles 0-2, the standard in/out/error file handles");
    FilePtr* fptr = (fileHandles.lookupHandle(handle+1));
    delete fptr->fp;
    fileHandles.deleteHandle(handle+1);
    delete fptr;
  }
  return ArrayVector();
}

//!
//@Module FREAD File Read Function
//@@Section IO
//@@Usage
//Reads a block of binary data from the given file handle into a variable
//of a given shape and precision.  The general use of the function is
//@[
//  A = fread(handle,size,precision)
//@]
//The @|handle| argument must be a valid value returned by the fopen 
//function, and accessable for reading.  The @|size| argument determines
//the number of values read from the file.  The @|size| argument is simply
//a vector indicating the size of the array @|A|.  The @|size| argument
//can also contain a single @|inf| dimension, indicating that FreeMat should
//calculate the size of the array along that dimension so as to read as
//much data as possible from the file (see the examples listed below for
//more details).  The data is stored as columns in the file, not 
//rows.
//    
//Alternately, you can specify two return values to the @|fread| function,
//in which case the second value contains the number of elements read
//@[
//   [A,count] = fread(...)
//@]
//where @|count| is the number of elements in @|A|.
//
//The third argument determines the type of the data.  Legal values for this
//argument are listed below:
//\begin{itemize}
//   \item 'uint8','uchar','unsigned char' for an unsigned, 8-bit integer.
//   \item 'int8','char','integer*1' for a signed, 8-bit integer.
//   \item 'uint16','unsigned short' for an unsigned, 16-bit  integer.
//   \item 'int16','short','integer*2' for a signed, 16-bit integer.
//   \item 'uint32','unsigned int' for an unsigned, 32-bit integer.
//   \item 'int32','int','integer*4' for a signed, 32-bit integer.
//   \item 'single','float32','float','real*4' for a 32-bit floating point.
//   \item 'double','float64','real*8' for a 64-bit floating point.
//\end{itemize}
//
//Starting with FreeMat 4, the format for the third argument has changed.
//If you specify only a type, such as @|'float'|, the data is read in as
//single precision, but the output type is always @|'double'|.  This behavior
//is consistent with Matlab.  If you want the output type to match the input
//type (as was previous behavior in FreeMat), you must preface the precision
//string with a @|'*'|.  Thus, the precision string @|'*float'| implies
//that data is read in as single precision, and the output is also single
//precision.
//
//The third option is to specify the input and output types explicitly.
//You can do this by specifiying a precision string of the form 
//@|'type1 => type2'|, where @|'type1'| is the input type and 
//@|'type2'| is the output type.  For example, if the input type is
//@|'double'| and the output type is to be a @|'float'|, then a type spec
//of @|'double => float'| should be used.
//
//@@Example
//First, we create an array of @|512 x 512| Gaussian-distributed @|float| random variables, and then writing them to a file called @|test.dat|.
//@<
//A = float(randn(512));
//fp = fopen('test.dat','w');
//fwrite(fp,A);
//fclose(fp);
//@>
//Read as many floats as possible into a row vector
//@<
//fp = fopen('test.dat','r');
//x = fread(fp,[1,inf],'float');
//fclose(fp);
//who x
//@>
//Note that @|x| is a @|double| array.  This behavior is new to FreeMat 4.
//Read the same floats into a 2-D float array.
//@<
//fp = fopen('test.dat','r');
//x = fread(fp,[512,inf],'float');
//fclose(fp);
//who x
//@>
//To read them as a single precision float array, we can use the
//following form:
//@<
//fp = fopen('test.dat','r');
//x = fread(fp,[512,inf],'*float');
//fclose(fp);
//who x
//@>
//
//@@Signature
//function fread FreadFunction
//inputs handle size precision
//outputs A count
//!

template <typename T>
Array Tread(QFile* fp, NTuple dim, bool swapflag) {
  size_t bufsize = sizeof(T)*size_t(dim.count());
  if ((fp->size() - fp->pos()) < bufsize) {
    bufsize = fp->size() - fp->pos();
    dim = NTuple(ceil((double)bufsize/sizeof(T)),1);
  }
  if (!swapflag) {
    BasicArray<T> rp(dim);
    fp->read((char*)(rp.data()),bufsize);
    return Array(rp);
  } else {
    MemBlock<T> pbuf(size_t(bufsize/sizeof(T)));
    T* pb = &pbuf;
    fp->read((char*)pb,bufsize);
    SwapBuffer((char*)pb,bufsize/sizeof(T),sizeof(T));
    BasicArray<T> rp(dim);
    memcpy(rp.data(),pb,bufsize);
    return Array(rp);
  }
}

#define MacroRead(ctype,cls)			\
  case cls: { A = Tread<ctype>(fptr->fp,dims,fptr->swapflag).toClass(dcOut); break; }

ArrayVector FreadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("fread requires three arguments, the file handle, size, and precision");
  Array tmp(arg[0]);
  int handle = tmp.asInteger();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  if (!arg[2].isString())
    throw Exception("second argument to fread must be a precision");
  QString prec = arg[2].asString().toLower();
  // Get the size argument
  Array sze(arg[1].asDenseArray().toClass(Index));
  // Check for a single infinity
  BasicArray<index_t> dp(sze.real<index_t>());
  bool infinityFound = false;
  index_t elementCount = 1;
  index_t infiniteDim = 0;
  for (index_t i=1;i<=dp.length();i++) {
    if (IsNaN(dp[i])) throw Exception("nan not allowed in size argument");
    if (IsInfinite(dp[i])) {
      if (infinityFound) throw Exception("only a single inf is allowed in size argument");
      infinityFound = true;
      infiniteDim = i;
    } else {
      if (dp[i] < 0) throw Exception("illegal negative size argument");
      elementCount *= dp[i];
    }
  }
  // Map the precision string to a data class
  DataClass dcIn, dcOut;
  ComputePrecisionString(prec,dcIn,dcOut);
  // If there is an infinity in the dimensions, we have to calculate the
  // appropriate value
  if (infinityFound) {
    int64 bytes_left = fptr->fp->size() - fptr->fp->pos();
    dp[infiniteDim] = ceil(double(bytes_left)/ByteSizeOfDataClass(dcIn)/elementCount);
    elementCount *= dp[infiniteDim];
  }
  NTuple dims(1,1);
  for (index_t j=1;j<=qMin(NDims,int(dp.length()));j++) 
    dims[int(j-1)] = dp[j];
  Array A;
  switch (dcIn) {
  default: throw Exception("data type not supported for fread");
    MacroExpandCasesSimple(MacroRead);
  }
  ArrayVector retval;
  retval.push_back(A);
  if (nargout == 2)
    retval.push_back(Array(double(A.length())));
  return retval;
}

//!
//@Module FWRITE File Write Function
//@@Section IO
//@@Usage
//Writes an array to a given file handle as a block of binary (raw) data.
//The general use of the function is
//@[
//  n = fwrite(handle,A)
//@]
//The @|handle| argument must be a valid value returned by the fopen 
//function, and accessable for writing. The array @|A| is written to
//the file a column at a time.  The form of the output data depends
//on (and is inferred from) the precision of the array @|A|.  If the 
//write fails (because we ran out of disk space, etc.) then an error
//is returned.  The output @|n| indicates the number of elements
//successfully written.
//
//Note that unlike MATLAB, FreeMat 4 does not default to @|uint8| for
//writing arrays to files.  Alternately, the type of the data to be
//written to the file can be specified with the syntax
//@[
//  n = fwrite(handle,A,type)
//@]
//where @|type| is one of the following legal values:
//\begin{itemize}
//   \item 'uint8','uchar','unsigned char' for an unsigned, 8-bit integer.
//   \item 'int8','char','integer*1' for a signed, 8-bit integer.
//   \item 'uint16','unsigned short' for an unsigned, 16-bit  integer.
//   \item 'int16','short','integer*2' for a signed, 16-bit integer.
//   \item 'uint32','unsigned int' for an unsigned, 32-bit integer.
//   \item 'int32','int','integer*4' for a signed, 32-bit integer.
//   \item 'single','float32','float','real*4' for a 32-bit floating point.
//   \item 'double','float64','real*8' for a 64-bit floating point.
//\end{itemize}
//
//@@Example
//Heres an example of writing an array of @|512 x 512| Gaussian-distributed @|float| random variables, and then writing them to a file called @|test.dat|.
//@<
//A = float(randn(512));
//fp = fopen('test.dat','w');
//fwrite(fp,A,'single');
//fclose(fp);
//@>
//@@Signature
//function fwrite FwriteFunction
//inputs handle A type
//outputs count
//!

template <typename T>
int64 Twrite(QFile* fp, const Array &A, bool swapflag) {
  size_t bufsize = sizeof(T)*size_t(A.length());
  if (!swapflag)
    return fp->write((const char*)(A.constReal<T>().constData()),bufsize);
  else {
    MemBlock<T> pbuf(size_t(A.length()));
    T* pb = &pbuf;
    memcpy(pb,A.constReal<T>().constData(),bufsize);
    SwapBuffer((char*)pb,size_t(A.length()),sizeof(T));
    return fp->write((const char*)pb,bufsize);
  }
}

#define MacroWrite(ctype,cls)						\
  case cls: return ArrayVector(Array(double(Twrite<ctype>(fptr->fp,x,fptr->swapflag))));

ArrayVector FwriteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("fwrite requires at least two arguments, the file handle, and the variable to be written");
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  if (arg[1].isReferenceType())
    throw Exception("cannot write reference data types with fwrite");
  Array x(arg[1]);
  if (arg.size() >= 3) {
    if (!arg[2].isString())
      throw Exception("type argument must be a string");
    QString prec = arg[2].asString();
    x = x.toClass(processPrecisionString(prec));
  }
  if (x.isString()) x = x.toClass(UInt8);
  switch (x.dataClass()) {
  default: throw Exception("data type not supported for fwrite");
    MacroExpandCasesSimple(MacroWrite);
  }
}

#undef MacroWrite

//!
//@Module FFLUSH Force File Flush
//@@Section IO
//@@Usage
//Flushes any pending output to a given file.  The general use of
//this function is
//@[
//   fflush(handle)
//@]
//where @|handle| is an active file handle (as returned by @|fopen|).
//@@Signature
//function fflush FflushFunction
//inputs handle
//outputs none
//!
 ArrayVector FflushFunction(int nargout, const ArrayVector& arg) {
   if (arg.size() != 1)
     throw Exception("fflush requires an argument, the file handle.");
   int handle = arg[0].asInteger();
   FilePtr *fptr = (fileHandles.lookupHandle(handle+1));
   fptr->fp->flush();
   return ArrayVector();
 }

//!
//@Module FTELL File Position Function
//@@Section IO
//@@Usage
//Returns the current file position for a valid file handle.
//The general use of this function is
//@[
//  n = ftell(handle)
//@]
//The @|handle| argument must be a valid and active file handle.  The
//return is the offset into the file relative to the start of the
//file (in bytes).
//@@Example
//Here is an example of using @|ftell| to determine the current file 
//position.  We read 512 4-byte floats, which results in the file 
//pointer being at position 512*4 = 2048.
//@<
//fp = fopen('test.dat','wb');
//fwrite(fp,randn(512,1));
//fclose(fp);
//fp = fopen('test.dat','rb');
//x = fread(fp,[512,1],'float');
//ftell(fp)
//@>
//@@Signature
//function ftell FtellFunction
//inputs handle
//outputs n
//!
ArrayVector FtellFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("ftell requires an argument, the file handle.");
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  return ArrayVector(Array(double(fptr->fp->pos())));
}

//!
//@Module FEOF End Of File Function
//@@Section IO
//@@Usage
//Check to see if we are at the end of the file.  The usage is
//@[
//  b = feof(handle)
//@]
//The @|handle| argument must be a valid and active file handle.  The
//return is true (logical 1) if the current position is at the end of
//the file, and false (logical 0) otherwise.  Note that simply reading
//to the end of a file will not cause @|feof| to return @|true|.  
//You must read past the end of the file (which will cause an error 
//anyway).  See the example for more details.
//@@Example
//Here, we read to the end of the file to demonstrate how @|feof| works.
//At first pass, we force a read of the contents of the file by specifying
//@|inf| for the dimension of the array to read.  We then test the
//end of file, and somewhat counter-intuitively, the answer is @|false|.
//We then attempt to read past the end of the file, which causes an
//error.  An @|feof| test now returns the expected value of @|true|.
//@<
//fp = fopen('test.dat','rb');
//x = fread(fp,[512,inf],'float');
//feof(fp)
//x = fread(fp,[1,1],'float');
//feof(fp)
//@>
//@@Signature
//function feof FeofFunction
//inputs handle
//outputs flag
//!
ArrayVector FeofFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("feof requires an argument, the file handle.");
  Array tmp(arg[0]);
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  return ArrayVector(Array(bool(fptr->fp->atEnd())));
}
  
//!
//@Module FSEEK Seek File To A Given Position
//@@Section IO
//@@Usage
//Moves the file pointer associated with the given file handle to 
//the specified offset (in bytes).  The usage is
//@[
//  fseek(handle,offset,style)
//@]
//The @|handle| argument must be a value and active file handle.  The
//@|offset| parameter indicates the desired seek offset (how much the
//file pointer is moved in bytes).  The @|style| parameter determines
//how the offset is treated.  Three values for the @|style| parameter
//are understood:
//\begin{itemize}
//\item string @|'bof'| or the value -1, which indicate the seek is relative
//to the beginning of the file.  This is equivalent to @|SEEK_SET| in
//ANSI C.
//\item string @|'cof'| or the value 0, which indicates the seek is relative
//to the current position of the file.  This is equivalent to 
//@|SEEK_CUR| in ANSI C.
//\item string @|'eof'| or the value 1, which indicates the seek is relative
//to the end of the file.  This is equivalent to @|SEEK_END| in ANSI
//C.
//\end{itemize}
//The offset can be positive or negative.
//@@Example
//The first example reads a file and then ``rewinds'' the file pointer by seeking to the beginning.
//The next example seeks forward by 2048 bytes from the files current position, and then reads a line of 512 floats.
//@<
//% First we create the file
//fp = fopen('test.dat','wb');
//fwrite(fp,float(rand(4096,1)));
//fclose(fp);
//% Now we open it
//fp = fopen('test.dat','rb');
//% Read the whole thing
//x = fread(fp,[1,inf],'float');
//% Rewind to the beginning
//fseek(fp,0,'bof');
//% Read part of the file
//y = fread(fp,[1,1024],'float');
//who x y
//% Seek 2048 bytes into the file
//fseek(fp,2048,'cof');
//% Read 512 floats from the file
//x = fread(fp,[512,1],'float');
//% Close the file
//fclose(fp);
//@>
//@@Signature
//function fseek FseekFunction
//inputs handle offset style
//outputs none
//!
ArrayVector FseekFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("fseek requires three arguments, the file handle, the offset, and style");
  FilePtr *fptr=(fileHandles.lookupHandle(arg[0].asInteger()+1));
  int64 offset = int64(arg[1].asDouble());
  int style;
  if (arg[2].isString()) {
    QString styleflag = arg[2].asString().toLower();
    if (styleflag=="bof")
      style = -1;
    else if (styleflag=="cof")
      style = 0;
    else if (styleflag=="eof")
      style = 1;
    else
      throw Exception("unrecognized style format for fseek");
  } else {
    style = arg[2].asInteger();
    if ((style != -1) && (style != 0) && (style != 1))
      throw Exception("unrecognized style format for fseek");	
  }
  switch (style) {
  case -1:
    if (!fptr->fp->seek(offset))
      throw Exception("Seek error:" + fptr->fp->errorString());
    break;
  case 0:
    if (!fptr->fp->seek(fptr->fp->pos() + offset))
      throw Exception("Seek error:" + fptr->fp->errorString());
    break;
  case 1:
    if (!fptr->fp->seek(fptr->fp->size() - 1 - offset))
      throw Exception("Seek error:" + fptr->fp->errorString());
    break;
  }
  return ArrayVector();
}

//!
//@Module FGETLINE Read a String from a File
//@@Section IO
//@@Usage
//Reads a string from a file.  The general syntax for its use
//is
//@[
//  s = fgetline(handle)
//@]
//This function reads characters from the file @|handle| into
//a @|string| array @|s| until it encounters the end of the file
//or a newline.  The newline, if any, is retained in the output
//string.  If the file is at its end, (i.e., that @|feof| would
//return true on this handle), @|fgetline| returns an empty
//string.
//@@Example
//First we write a couple of strings to a test file.
//@<
//fp = fopen('testtext','w');
//fprintf(fp,'String 1\n');
//fprintf(fp,'String 2\n');
//fclose(fp);
//@>
//Next, we read then back.
//@<
//fp = fopen('testtext','r')
//fgetline(fp)
//fgetline(fp)
//fclose(fp);
//@>
//@@Signature
//function fgetline FgetlineFunction
//inputs handle
//outputs string
//!
ArrayVector FgetlineFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("fgetline takes one argument, the file handle");
  int handle = arg[0].asInteger();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  if (fptr->fp->atEnd())
    return ArrayVector(EmptyConstructor());
  else 
    return ArrayVector(Array(ReadQStringFromFile(fptr->fp)));
}

ArrayVector ScanfHelperFunction( QFile *fp, const ArrayVector& arg );
//!
//@Module FSCANF Formatted File Input Function (C-Style)
//@@Section IO
//@@Usage
//Reads values from a file.  The general syntax for its use is
//@[
//  [a,count] = fscanf(handle,format,[size])
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the input, @|size| specifies the amount of data to be read. Values that are parsed
//from the @|text| are stored in a. Note that fscanf is vectorized - the format string is reused as long as
//there are entries in the @|text| string.
//See @|printf| for a description of the format.  Note that if
//the file is at the end-of-file, the fscanf will return 
//@@Signature
//function fscanf FscanfFunction
//inputs varargin
//outputs varargout
//!
ArrayVector FscanfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("fscanf takes two arguments, the file handle and the format string");
  int handle = arg[0].asInteger();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));

  ArrayVector helper_arg; 
  helper_arg << arg[1];
  if( arg.size() == 3 )
      helper_arg << arg[2];

  return ScanfHelperFunction(fptr->fp, helper_arg);
}
