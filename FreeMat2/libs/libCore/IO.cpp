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
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "Malloc.hpp"
#include "HandleList.hpp"
#include "WalkTree.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include "IEEEFP.hpp"
#include "matio.h"
#include "Sparse.hpp"

namespace FreeMat {
  class FilePtr {
  public:
    FILE *fp;
    bool swapflag;
  };
  
  HandleList<FilePtr*> fileHandles;

  void InitializeFileSubsystem() {
    FilePtr *fptr = new FilePtr();
    fptr->fp = stdin;
    fptr->swapflag = false;
    fileHandles.assignHandle(fptr);
    fptr = new FilePtr();
    fptr->fp = stdout;
    fptr->swapflag = false;
    fileHandles.assignHandle(fptr);
    fptr = new FilePtr();
    fptr->fp = stderr;
    fptr->swapflag = false;
    fileHandles.assignHandle(fptr);
  }

  //!
  //@Module SETPRINTLIMIT Set Limit For Printing Of Arrays
  //@@Section IO
  //@@Usage
  //Changes the limit on how many elements of an array are printed
  //using either the @|disp| function or using expressions on the
  //command line without a semi-colon.  The default is set to 
  //one thousand elements.  You can increase or decrease this
  //limit by calling
  //@[
  //  setprintlimit(n)
  //@]
  //where @|n| is the new limit to use.
  //@@Example
  //Setting a smaller print limit avoids pages of output when you forget the semicolon on an expression.
  //@<
  //A = randn(512);
  //setprintlimit(10)
  //A
  //@>
  //!
  ArrayVector SetPrintLimitFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("setprintlimit requires one, scalar integer argument");
    Array tmp(arg[0]);
    eval->setPrintLimit(tmp.getContentsAsIntegerScalar());
    return ArrayVector();
  }

  //!
  //@Module GETPRINTLIMIT Get Limit For Printing Of Arrays
  //@@Section IO
  //@@Usage
  //Returns the limit on how many elements of an array are printed
  //using either the @|disp| function or using expressions on the
  //command line without a semi-colon.  The default is set to 
  //one thousand elements.  You can increase or decrease this
  //limit by calling @|setprintlimit|.  This function is provided
  //primarily so that you can temporarily change the output truncation
  //and then restore it to the previous value (see the examples).
  //@[
  //   n=getprintlimit
  //@]
  //where @|n| is the current limit in use.
  //@@Example
  //Here is an example of using @|getprintlimit| along with @|setprintlimit| to temporarily change the output behavior of FreeMat.
  //@<
  //A = randn(100,1);
  //n = getprintlimit
  //setprintlimit(5);
  //A
  //setprintlimit(n)
  //@>
  //!
  ArrayVector GetPrintLimitFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    Array tmp(Array::uint32Constructor(eval->getPrintLimit()));
    ArrayVector retval;
    retval.push_back(tmp);
    return retval;
  }

  void SwapBuffer(char* cp, int count, int elsize) {
    char tmp;
    for (int i=0;i<count;i++)
      for (int j=0;j<elsize/2;j++) {
	tmp = cp[i*elsize+j];
	cp[i*elsize+j] = cp[i*elsize+elsize-1-j];
	cp[i*elsize+elsize-1-j] = tmp;
      }    
  }

#define MATCH(x) (strcmp(prec,x)==0)
  void processPrecisionString(char *prec, Class &dataClass, int& elementSize, int& swapSize) {
    if (MATCH("uint8") || MATCH("uchar") || MATCH("unsigned char")) {
      dataClass = FM_UINT8;
      elementSize = 1;
      swapSize = 1;
      return;
    }
    if (MATCH("int8") || MATCH("char") || MATCH("integer*1")) {
      dataClass = FM_INT8;
      elementSize = 1;
      swapSize = 1;
      return;
    }
    if (MATCH("uint16") || MATCH("unsigned short")) {
      dataClass = FM_UINT16;
      elementSize = 2;
      swapSize = 2;
      return;
    }
    if (MATCH("int16") || MATCH("short") || MATCH("integer*2")) {
      dataClass = FM_INT16;
      elementSize = 2;
      swapSize = 2;
      return;
    }
    if (MATCH("uint32") || MATCH("unsigned int")) {
      dataClass = FM_UINT32;
      elementSize = 4;
      swapSize = 4;
      return;
    }
    if (MATCH("int32") || MATCH("int") || MATCH("integer*4")) {
      dataClass = FM_INT32;
      elementSize = 4;
      swapSize = 4;
      return;
    }
    if (MATCH("single") || MATCH("float32") || MATCH("float") || MATCH("real*4")) {
      dataClass = FM_FLOAT;
      elementSize = 4;
      swapSize = 4;
      return;
    }
    if (MATCH("double") || MATCH("float64") || MATCH("real*8")) {
      dataClass = FM_DOUBLE;
      elementSize = 8;
      swapSize = 8;
      return;
    }
    if (MATCH("complex") || MATCH("complex*8")) {
      dataClass = FM_COMPLEX;
      elementSize = 8;
      swapSize = 4;
      return;
    }
    if (MATCH("dcomplex") || MATCH("complex*16")) {
      dataClass = FM_DCOMPLEX;
      elementSize = 16;
      swapSize = 8;
      return;
    }
    throw Exception("invalid precision type");
  }
#undef MATCH

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
  //On some platforms (e.g. Win32) it is necessary to add a 'b' for 
  //binary files to avoid the operating system's 'CR/LF<->CR' translation.
  //
  //Finally, FreeMat has the ability to read and write files of any
  //byte-sex (endian).  The third (optional) input indicates the 
  //byte-endianness of the file.  If it is omitted, the native endian-ness
  //of the machine running FreeMat is used.  Otherwise, the third
  //argument should be one of the following strings:
  //\begin{itemize}
  //   \item @|'le','ieee-le','little-endian','littleEndian','little'|
  //   \item @|'be','ieee-be','big-endian','bigEndian','big'|
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
  //fp = fopen('test.dat','wb','ieee-le')
  //fwrite(fp,float([1.2,4.3,2.1]))
  //fclose(fp)
  //@>
  //Next, we open the file and read the data back
  //@<
  //fp = fopen('test.dat','rb','ieee-le')
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
  //fp = fopen('test.dat','rb','ieee-le')
  //fread(fp,[1,5],'float')
  //fclose(fp)
  //@>
  //!
  ArrayVector FopenFunction(int nargout, const ArrayVector& arg) {
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
    char *fname = arg[0].getContentsAsCString();
    char *mode = "rb";
    if (arg.size() > 1) {
      if (!arg[1].isString())
	throw Exception("Access mode to fopen must be a string");
      mode = arg[1].getContentsAsCString();
    }
    bool swapendian = false;
    if (arg.size() > 2) {
      char *swapflag = arg[2].getContentsAsCString();
      if (strcmp(swapflag,"swap") == 0) {
	swapendian = true;
      } else if ((strcmp(swapflag,"le") == 0) ||
		 (strcmp(swapflag,"ieee-le") == 0) ||
		 (strcmp(swapflag,"little-endian") == 0) ||
		 (strcmp(swapflag,"littleEndian") == 0) ||
		 (strcmp(swapflag,"little") == 0)) {
	swapendian = bigEndian;
      } else if ((strcmp(swapflag,"be") == 0) ||
		 (strcmp(swapflag,"ieee-be") == 0) ||
		 (strcmp(swapflag,"big-endian") == 0) ||
		 (strcmp(swapflag,"bigEndian") == 0) ||
		 (strcmp(swapflag,"big") == 0)) {
	swapendian = !bigEndian;
      } else if (!arg[2].isEmpty())
	throw Exception("swap flag must be 'swap' or an endian spec ('le','ieee-le','little-endian','littleEndian','little','be','ieee-be','big-endian','bigEndian','big')");
    }
    FILE *fp = fopen(fname,mode);
    if (!fp)
      throw Exception(strerror(errno));
    FilePtr *fptr = new FilePtr();
    fptr->fp = fp;
    fptr->swapflag = swapendian;
    unsigned int rethan = fileHandles.assignHandle(fptr);
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(rethan-1));
    return retval;
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
  //!
  ArrayVector FcloseFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Fclose must have one argument, either 'all' or a file handle");
    bool closingAll = false;
    int retval = 0;
    if (arg[0].isString()) {
      char *allflag = arg[0].getContentsAsCString();
      if (strcmp(allflag,"all") == 0) {
	closingAll = true;
      } else
	throw Exception("Fclose must have one argument, either 'all' or a file handle");
    } else {
      Array tmp(arg[0]);
      int handle = tmp.getContentsAsIntegerScalar();
      if (handle <= 2)
	throw Exception("Cannot close handles 0-2, the standard in/out/error file handles");
      FilePtr* fptr = (fileHandles.lookupHandle(handle+1));
      if (fclose(fptr->fp))
	retval = -1;
      fileHandles.deleteHandle(handle+1);
      delete fptr;
    }
    return singleArrayVector(Array::int32Constructor(retval));
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
  //   \item 'complex','complex*8' for a 64-bit complex floating point (32 bits for the real and imaginary part).
  //   \item 'dcomplex','complex*16' for a 128-bit complex floating point (64 bits for the real and imaginary part).
  //\end{itemize}
  //@@Example
  //First, we create an array of @|512 x 512| Gaussian-distributed @|float| random variables, and then writing them to a file called @|test.dat|.
  //@<
  //A = float(randn(512));
  //fp = fopen('test.dat','wb');
  //fwrite(fp,A);
  //fclose(fp);
  //@>
  //Read as many floats as possible into a row vector
  //@<
  //fp = fopen('test.dat','rb');
  //x = fread(fp,[1,inf],'float');
  //who x
  //@>
  //Read the same floats into a 2-D float array.
  //@<
  //fp = fopen('test.dat','rb');
  //x = fread(fp,[512,inf],'float');
  //who x
  //@>
  //!
  ArrayVector FreadFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 3)
      throw Exception("fread requires three arguments, the file handle, size, and precision");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
    if (!arg[2].isString())
      throw Exception("second argument to fread must be a precision");
    char *prec = arg[2].getContentsAsCString();
    // Get the size argument
    Array sze(arg[1]);
    // Promote sze to a float argument
    sze.promoteType(FM_FLOAT);
    // Check for a single infinity
    int dimCount(sze.getLength());
    float *dp = ((float *) sze.getReadWriteDataPointer());
    bool infinityFound = false;
    int elementCount = 1;
    int infiniteDim = 0;
    for (int i=0;i<dimCount;i++) {
      if (IsNaN(dp[i])) throw Exception("nan not allowed in size argument");
      if (IsInfinite(dp[i])) {
	if (infinityFound) throw Exception("only a single inf is allowed in size argument");
	infinityFound = true;
	infiniteDim = i;
      } else {
	if (dp[i] < 0) throw Exception("illegal negative size argument");
	elementCount *= (int) dp[i];
      }
    }
    // Map the precision string to a data class
    Class dataClass;
    int elementSize;
    int swapSize;
    processPrecisionString(prec,dataClass,elementSize,swapSize);
    // If there is an infinity in the dimensions, we have to calculate the
    // appropriate value
    if (infinityFound) {
      long fsize;
      long fcpos;
      fcpos = ftell(fptr->fp);
      fseek(fptr->fp,0L,SEEK_END);
      fsize = ftell(fptr->fp) - fcpos;
      fseek(fptr->fp,fcpos,SEEK_SET);
      dp[infiniteDim] = ceil((float)(fsize/elementSize/elementCount));
      elementCount *= (int) dp[infiniteDim];
    }
    // Next, we allocate space for the result
    void *qp = Malloc(elementCount*elementSize);
    // Read in the requested number of data points...
    size_t g = fread(qp,elementSize,elementCount,fptr->fp);
    if (g != elementCount)
      throw Exception("Insufficient data remaining in file to fill out requested size");
    if (ferror(fptr->fp)) 
      throw Exception(strerror(errno));
    if (fptr->swapflag)
      SwapBuffer((char *)qp,elementCount*elementSize/swapSize,swapSize);
    // Convert dp to a Dimensions
    Dimensions dims(dimCount);
    for (int j=0;j<dimCount;j++)
      dims[j] = (int) dp[j];
    if (dimCount == 1)
      dims[1] = 1;
    ArrayVector retval;
    retval.push_back(Array::Array(dataClass,dims,qp));
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
  //@@Example
  //Heres an example of writing an array of @|512 x 512| Gaussian-distributed @|float| random variables, and then writing them to a file called @|test.dat|.
  //@<
  //A = float(randn(512));
  //fp = fopen('test.dat','wb');
  //fwrite(fp,A);
  //fclose(fp);
  //@>
  //!
  ArrayVector FwriteFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
      throw Exception("fwrite requires two arguments, the file handle, and the variable to be written");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
    if (arg[1].isReferenceType())
      throw Exception("cannot write reference data types with fwrite");
    Array toWrite(arg[1]);
    unsigned int written;
    unsigned int count(toWrite.getLength());
    unsigned int elsize(toWrite.getElementSize());
    if (!fptr->swapflag || (elsize == 1)) {
      const void *dp=(toWrite.getDataPointer());
      written = fwrite(dp,elsize,count,fptr->fp);      
    } else {
      void *dp=(toWrite.getReadWriteDataPointer());
      SwapBuffer((char*) dp, count, elsize);
      written = fwrite(dp,elsize,count,fptr->fp);
    }
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(written));
    return retval;    
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
  //!
  ArrayVector FtellFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("ftell requires an argument, the file handle.");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
    unsigned int fpos;
    fpos = ftell(fptr->fp);
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(fpos));
    return retval;
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
  //@<1
  //fp = fopen('test.dat','rb');
  //x = fread(fp,[512,inf],'float');
  //feof(fp)
  //x = fread(fp,[1,1],'float');
  //feof(fp)
  //@>
  //!
  ArrayVector FeofFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("feof requires an argument, the file handle.");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
    int ateof;
    ateof = feof(fptr->fp);
    ArrayVector retval;
    retval.push_back(Array::logicalConstructor(ateof));
    return retval;
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
  //!
  ArrayVector FseekFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 3)
      throw Exception("fseek requires three arguments, the file handle, the offset, and style");
    Array tmp1(arg[0]);
    int handle = tmp1.getContentsAsIntegerScalar();
    FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
    Array tmp2(arg[1]);
    int offset = tmp2.getContentsAsIntegerScalar();
    Array tmp3(arg[2]);
    int style;
    if (tmp3.isString()) {
      char *styleflag = arg[2].getContentsAsCString();
      if ((strcmp(styleflag,"bof") == 0) ||
	  (strcmp(styleflag,"BOF") == 0))
	style = -1;
      else if ((strcmp(styleflag,"cof") == 0) ||
	       (strcmp(styleflag,"COF") == 0))
	style = 0;
      else if ((strcmp(styleflag,"eof") == 0) ||
	       (strcmp(styleflag,"EOF") == 0))
	style = 1;
      else
	throw Exception("unrecognized style format for fseek");
    } else {
      style = tmp3.getContentsAsIntegerScalar();
      if ((style != -1) && (style != 0) && (style != 1))
	throw Exception("unrecognized style format for fseek");	
    }
    switch (style) {
    case -1:
      if (fseek(fptr->fp,offset,SEEK_SET))
	throw Exception(strerror(errno));
      break;
    case 0:
      if (fseek(fptr->fp,offset,SEEK_CUR))
	throw Exception(strerror(errno));
      break;
    case 1:
      if (fseek(fptr->fp,offset,SEEK_END))
	throw Exception(strerror(errno));
      break;
    }
    return ArrayVector();
  }

  int flagChar(char c) {
    return ((c == '#') ||  (c == '0') || (c == '-') ||  
	    (c == ' ') ||  (c == '+'));
  }
  
  int convspec(char c) {
    return ((c == 'd') || (c == 'i') || (c == 'o') || 
	    (c == 'u') || (c == 'x') || (c == 'X') ||
	    (c == 'e') || (c == 'E') || (c == 'f') || 
	    (c == 'F') || (c == 'g') || (c == 'G') ||
	    (c == 'a') || (c == 'A') || (c == 'c') || 
	    (c == 's'));
  }
    
  char* validateFormatSpec(char* cp) {
    if (*cp == '%') return cp+1;
    while ((*cp) && flagChar(*cp)) cp++;
    while ((*cp) && isdigit(*cp)) cp++;
    while ((*cp) && (*cp == '.')) cp++;
    while ((*cp) && isdigit(*cp)) cp++;
    if ((*cp) && convspec(*cp)) 
      return cp+1;
    else
      return 0;
  }

  char* validateScanFormatSpec(char* cp) {
    if (*cp == '%') return cp+1;
    while ((*cp) && flagChar(*cp)) cp++;
    while ((*cp) && isdigit(*cp)) cp++;
    while ((*cp) && (*cp == '.')) cp++;
    while ((*cp) && isdigit(*cp)) cp++;
    if ((*cp) && (convspec(*cp) || (*cp == 'h') || (*cp == 'l')))
      return cp+1;
    else
      return 0;
  }
    
  bool isEscape(char *dp) {
    return ((dp[0] == '\\') &&
	    ((dp[1] == 'n') ||
	     (dp[1] == 't') ||
	     (dp[1] == 'r') ||
	     (dp[1] == '\\')));
  }
  
  void convertEscapeSequences(char *dst, char* src) {
    char *sp;
    char *dp;
    sp = src;
    dp = dst;
    while (*sp) {
      // Is this an escape sequence?
      if (isEscape(sp)) {
	switch (sp[1]) {
	case '\\':
	  *(dp++) = '\\';
	  break;
	case 'n':
	  *(dp++) = '\n';
	  break;
	case 't':
	  *(dp++) = '\t';
	  break;
	case 'r':
	  *(dp++) = '\r';
	  break;
	}
	sp += 2;
      } else
	*(dp++) = *(sp++);
    }
    // Null terminate
    *dp = 0;
  }
  
  //Common routine used by sprintf,printf,fprintf.  They all
  //take the same inputs, and output either to a string, the
  //console or a file.  For output to a console or a file, 
  //we want escape-translation on.  For output to a string, we
  //want escape-translation off.  So this common routine prints
  //the contents to a string, which is then processed by each 
  //subroutine.
  char* xprintfFunction(int nargout, const ArrayVector& arg) {
    Array format(arg[0]);
    char *frmt = format.getContentsAsCString();
    char *buff = (char*) malloc(strlen(frmt)+1);
    strcpy(buff,frmt);
    // Search for the start of a format subspec
    char *dp = buff;
    char *np;
    char sv;
    // Buffer to hold each sprintf result
    char nbuff[4096];
    // Buffer to hold the output
    char *op;
    op = (char*) malloc(sizeof(char));
    *op = 0;
    int nextArg = 1;
    // Scan the string
    while (*dp) {
      np = dp;
      while ((*dp) && (*dp != '%')) dp++;
      // Print out the formatless segment
      sv = *dp;
      *dp = 0;
      sprintf(nbuff,np);
      op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
      strcat(op,nbuff);
      *dp = sv;
      // Process the format spec
      if (*dp) {
	np = validateFormatSpec(dp+1);
	if (!np)
	  throw Exception("erroneous format specification " + std::string(dp));
	else {
	  if (*(np-1) == '%') {
	    sprintf(nbuff,"%%");
	    op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
	    strcat(op,nbuff);
	    dp+=2;
	  } else {
	    sv = *np;
	    *np = 0;
	    if (arg.size() <= nextArg)
	      throw Exception("not enough arguments to satisfy format specification");
	    Array nextVal(arg[nextArg++]);
	    if ((*(np-1) != 's') && (nextVal.isEmpty())) {
	      op = (char*) realloc(op,strlen(op)+strlen("[]")+1);
	      strcat(op,"[]");
	    } else {
	      switch (*(np-1)) {
	      case 'd':
	      case 'i':
	      case 'o':
	      case 'u':
	      case 'x':
	      case 'X':
	      case 'c':
		nextVal.promoteType(FM_INT32);
		sprintf(nbuff,dp,*((int32*)nextVal.getDataPointer()));
		op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
		strcat(op,nbuff);
		break;
	      case 'e':
	      case 'E':
	      case 'f':
	      case 'F':
	      case 'g':
	      case 'G':
		nextVal.promoteType(FM_DOUBLE);
		sprintf(nbuff,dp,*((double*)nextVal.getDataPointer()));
		op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
		strcat(op,nbuff);
		break;
	      case 's':
		sprintf(nbuff,dp,nextVal.getContentsAsCString());
		op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
		strcat(op,nbuff);
	      }
	    }
	    *np = sv;
	    dp = np;
	  }
	}
      }
    }
    free(buff);
    return op;
  }

  //!
  //@Module SPRINTF Formated String Output Function (C-Style)
  //@@Section IO
  //@@Usage
  //Prints values to a string.  The general syntax for its use is
  //@[
  //  y = sprintf(format,a1,a2,...).
  //@]
  //Here @|format| is the format string, which is a string that
  //controls the format of the output.  The values of the variables
  //@|a_i| are substituted into the output as required.  It is
  //an error if there are not enough variables to satisfy the format
  //string.  Note that this @|sprintf| command is not vectorized!  Each
  //variable must be a scalar.  The returned value @|y| contains the
  //string that would normally have been printed. For
  //more details on the format string, see @|printf|.  
  //@@Examples
  //Here is an example of a loop that generates a sequence of files based on
  //a template name, and stores them in a cell array.
  //@<
  //l = {}; for i = 1:5; s = sprintf('file_%d.dat',i); l(i) = {s}; end;
  //l
  //@>
  //!
  ArrayVector SprintfFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() == 0)
      throw Exception("sprintf requires at least one (string) argument");
    Array format(arg[0]);
    if (!format.isString())
      throw Exception("sprintf format argument must be a string");
    char *op = xprintfFunction(nargout,arg);
    Array outString(Array::stringConstructor(op));
    free(op);
    return singleArrayVector(outString);
  }
  
  //!
  //@Module PRINTF Formated Output Function (C-Style)
  //@@Section IO
  //@@Usage
  //Prints values to the output.  The general syntax for its use is
  //@[
  //  printf(format,a1,a2,...)
  //@]
  //Here @|format| is the format string, which is a string that
  //controls the format of the output.  The values of the variables
  //@|a_i| are substituted into the output as required.  It is
  //an error if there are not enough variables to satisfy the format
  //string.  Note that this @|printf| command is not vectorized!  Each
  //variable must be a scalar.
  //
  //@@Format of the format string:
  //
  //The  format  string  is a character string, beginning and ending in its
  //initial shift state, if any.  The format string is composed of zero  or
  //more   directives:  ordinary  characters  (not  %),  which  are  copied
  //unchanged to the output stream; and conversion specifications, each  of
  //which results in fetching zero or more subsequent arguments.  Each 
  //conversion specification is introduced by the character %, and ends with a
  //conversion  specifier.  In between there may be (in this order) zero or
  //more flags, an optional minimum field width, and an optional precision.
  //
  //The  arguments must correspond properly (after type promotion) with the
  //conversion specifier, and are used in the order given.
  //
  //@@The flag characters:
  //The character @|%| is followed by zero or more of the following flags:
  //\begin{itemize}
  //  \item @|\#|   The  value  should be converted to an ``alternate form''.  For @|o| conversions, the first character of the output  string  is  made  zero (by prefixing a @|0| if it was not zero already).  For @|x| and @|X| conversions, a nonzero result has the string @|'0x'| (or @|'0X'| for  @|X|  conversions) prepended to it.  For @|a, A, e, E, f, F, g,| and @|G|  conversions, the result will always  contain  a  decimal  point,  even  if  no digits follow it (normally, a decimal point appears  in the results of those conversions only if  a  digit  follows).  For @|g| and @|G| conversions, trailing zeros are not removed from the  result as they would otherwise be.  For other  conversions,  the  result is undefined.
  //  \item @|0|   The value should be zero padded.  For @|d, i, o, u, x, X, a, A, e, E, f, F, g,| and @|G| conversions, the converted value is padded  on the  left  with  zeros rather than blanks.  If the @|0| and @|-| flags  both appear, the @|0| flag is ignored.  If  a  precision  is  given  with  a numeric conversion @|(d, i, o, u, x, and X)|, the @|0| flag is  ignored.  For other conversions, the behavior is undefined.
  //  \item @|-|   The converted value is to be left adjusted on the  field  boundary.  (The default is right justification.) Except for @|n| conversions, the converted value is padded on the right  with  blanks, rather than on the left with blanks or zeros.  A @|-| overrides a @|0| if both are given.
  //  \item @|' '| (a space) A blank should be left before a  positive  number  (or empty string) produced by a signed conversion.
  //  \item @|+| A  sign  (@|+| or @|-|) always be placed before a number produced by a signed conversion.  By default a sign is used only for  negative numbers. A @|+| overrides a space if both are used.
  //\end{itemize}
  //@@The field width:
  //An  optional decimal digit string (with nonzero first digit) specifying a 
  //minimum field width.  If the converted  value  has  fewer  characters than 
  //the  field  width,  it will be padded with spaces on the left (or right, 
  //if the left-adjustment flag has been given).  A  negative  field width is 
  //taken as a @|'-'| flag followed by a positive field  width. In no case does 
  //a non-existent or small field width cause truncation of a field; if the 
  //result of a conversion is wider than the  field  width, the field is 
  //expanded to contain the conversion result.
  //
  //@@The precision:
  //
  //An  optional  precision,  in the form of a period (@|'.'|)  followed by an optional decimal digit string.  If the precision is given as just @|'.'|, or the precision is negative, the precision is  taken  to  be zero.   This  gives the minimum number of digits to appear for @|d, i, o, u, x|, and @|X| conversions, the number of digits to appear after the radix character  for  @|a, A, e, E, f|, and @|F| conversions, the maximum number of significant digits for @|g| and @|G| conversions, or the  maximum  number  of  characters to be printed from a string for s conversions.
  //
  //@@The conversion specifier:
  //
  //A character that specifies the type of conversion to be  applied.   The
  //conversion specifiers and their meanings are:
  //\begin{itemize}
  //\item @|d,i|   The  int  argument is converted to signed decimal notation.  The  precision, if any, gives the minimum number of digits that  must   appear;  if  the  converted  value  requires fewer digits, it is    padded on the left with zeros. The default precision is @|1|.  When @|0|  is printed with an explicit precision @|0|, the output is empty.
  //\item @|o,u,x,X|   The unsigned int argument is converted to  unsigned  octal  (@|o|),  unsigned  decimal  (@|u|),  or unsigned hexadecimal (@|x| and @|X|) notation.  The letters @|abcdef| are used for @|x| conversions;  the  letters @|ABCDEF| are used for @|X| conversions.  The precision, if any,  gives the minimum number of digits that must appear; if the converted  value  requires  fewer  digits, it is padded on the left  with zeros. The default precision is @|1|.  When @|0| is printed  with  an explicit precision @|0|, the output is empty.
  //\item @|e,E|    The  double  argument  is  rounded  and  converted  in the style  @|[-]d.ddde dd| where there is one digit before  the  decimal-point  character and the number of digits after it is equal to the precision; if the precision is missing, it is taken as  @|6|;  if  the    precision  is  zero,  no  decimal-point character appears.  An @|E|  conversion uses the letter @|E| (rather than @|e|)  to  introduce  the  exponent.   The exponent always contains at least two digits; if  the value is zero, the exponent is @|00|.
  //\item @|f,F|   The double argument is rounded and converted to decimal notation  in  the  style  @|[-]ddd.ddd|, where the number of digits after the decimal-point character is equal to the precision specification.  If  the precision is missing, it is taken as @|6|; if the precision  is explicitly zero, no decimal-point character  appears.   If  a   decimal point appears, at least one digit appears before it.
  //\item @|g,G|   The double argument is converted in style @|f| or @|e| (or @|F| or @|E|  for  @|G|  conversions).  The precision specifies the number of significant digits.  If the precision is missing, @|6| digits  are  given;  if  the  precision is zero, it is treated as @|1|.  Style e is used   if the exponent from its conversion is less than @|-4|  or  greater than or equal to the precision.  Trailing zeros are removed from  the fractional part of the result; a decimal point appears  only  if it is followed by at least one digit.
  //\item @|c| The int argument is  converted  to  an  unsigned  char, and  the resulting character is written.
  //\item @|s| The string argument is printed.
  //\item @|%|   A @|'%'| is written. No argument is converted. The complete conversion specification is @|'%%'|.
  //\end{itemize}
  //@@Example
  //Here are some examples of the use of @|printf| with various arguments.  First we print out an integer and double value.
  //@<
  //printf('intvalue is %d, floatvalue is %f\n',3,1.53);
  //@>
  //Next, we print out a string value.
  //@<
  //printf('string value is %s\n','hello');
  //@>
  //Now, we print out an integer using 12 digits, zeros up front.
  //@<
  //printf('integer padded is %012d\n',32);
  //@>
  //Print out a double precision value with a sign, a total of 18 characters (zero prepended if necessary), a decimal point, and 12 digit precision.
  //@<
  //printf('float value is %+018.12f\n',pi);
  //@>
  //!
  ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() == 0)
      throw Exception("printf requires at least one (string) argument");
    Array format(arg[0]);
    if (!format.isString())
      throw Exception("printf format argument must be a string");
    char *op = xprintfFunction(nargout,arg);
    char *buff = (char*) malloc(strlen(op)+1);
    convertEscapeSequences(buff,op);
    eval->getInterface()->outputMessage(buff);
    free(buff);
    free(op);
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
  //!
  ArrayVector FgetlineFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("fgetline takes one argument, the file handle");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
    char buffer[4096];
    fgets(buffer,sizeof(buffer),fptr->fp);
    if (feof(fptr->fp))
      return singleArrayVector(Array::emptyConstructor());
    return singleArrayVector(Array::stringConstructor(buffer));
  }

  //!
  //@Module STR2NUM Convert a String to a Number
  //@@Section IO
  //@@Usage
  //Converts a string to a number.  The general syntax for its use
  //is
  //@[
  //  x = str2num(string)
  //@]
  //Here @|string| is the data string, which contains the data to 
  //be converted into a number.  The output is in double precision,
  //and must be typecasted to the appropriate type based on what
  //you need.
  //!
  ArrayVector Str2NumFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("str2num takes a single argument, the string to convert into a number");
    Array data(arg[0]);
    if (!data.isString())
      throw Exception("the first argument to str2num must be a string");
    double num = atof(data.getContentsAsCString());
    ArrayVector retval;
    retval.push_back(Array::doubleConstructor(num));
    return retval;
  }

  // How does sscanf work...
  // a whitespace matches any number/type of whitespace
  // a non-whitespace must be matched exactly
  // a %d, %ld, etc.  reads a number... 
  // a %s matches a sequence of characters that does
  // not contain whitespaces...
  // a mismatch - 

  //!
  //@Module SSCANF Formated String Input Function (C-Style)
  //@@Section IO
  //@@Usage
  //Reads values from a string.  The general syntax for its use is
  //@[
  //  [a1,...,an] = sscanf(text,format)
  //@]
  //Here @|format| is the format string, which is a string that
  //controls the format of the input.  Each value that is parsed
  //from the @|text| occupies one output slot.  See @|printf|
  //for a description of the format.
  //!
#if 0
  ArrayVector SscanfFunction(int nargout, const ArrayVector& arg) {
    if ((arg.size() != 2) || (!arg[0].isString()) || (!arg[1].isString()))
      throw Exception("sscanf takes two arguments, the text and the format string");
    Array text(arg[0]);
    Array format(arg[1]);
    char *txt = text.getContentsAsCString();
    char *frmt = format.getContentsAsCString();
    char *buff = (char*) malloc(strlen(frmt)+1);
    strcpy(buff,frmt);
    // Search for the start of a format subspec
    char *dp = buff;
    char *np;
    char sv;
    int nextArg = 2;
    bool shortarg;
    bool doublearg;
    // Scan the string
    ArrayVector values;
    // while there is more string to scan...
    while (*fp) {
      // If *dp is a whitespace...
      if (iswhitespace(*fp)) {
	// match zero or more whitespaces from the source string
	while (*dp && iswhitespace(*dp)) dp++;
      } else if (*fp != '%') {
	// we must match it
	if (!(*dp && (*dp == *fp)))
	  // punt
	  return values;
	else {
	  fp++;
	  dp++;
	}
      } else {
	// No... so we need to collect input...
	// Now the nice thing is that we can use sscanf to 
	// retrieve the value... the difficulty is
	// that we don't know how much to advance dp....
	// but we have to scan forward
	// to determine which type to use.
	np = validateScanFormatSpec(dp);
	if (!np)
	  throw Exception("erroneous format specification " + std::string(dp));
	if (*(np-1) == '%') {
	  dp += 2;
	  fp ++;
	} else {
	  shortarg = false;
	  doublearg = false;
	  if (*(np-1) == 'h') {
	    shortarg = true;
	    np++;
	  } else if (*(np-1) == 'l') {
	    doublearg = true;
	    np++;
	  } 
	  sv = *np;
	  *np = 0;
	  switch (*(np-1)) {
	  case 'd':
	  case 'i':
	    if (shortarg) {
	      short sdumint;
	      if (!sscanf(fp,dp,&sdumint))
		return values;
	      else
		values.push_back(Array::int16Constructor(sdumint));
	      
	    } else {
	      int sdumint;
	      fscanf(fptr->fp,dp,&sdumint);
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else
		values.push_back(Array::int32Constructor(sdumint));
	    }
	    break;
	  case 'o':
	  case 'u':
	  case 'x':
	  case 'X':
	  case 'c':
	    if (shortarg) {
	      int sdumint;
	      fscanf(fptr->fp,dp,&sdumint);
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else
		values.push_back(Array::int32Constructor(sdumint));
	    } else {
	      unsigned int dumint;
	      fscanf(fptr->fp,dp,&dumint);
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else
		values.push_back(Array::uint32Constructor(dumint));
	    }
	    break;
	  case 'e':
	  case 'E':
	  case 'f':
	  case 'F':
	  case 'g':
	  case 'G':
	    if (doublearg) {
	      double dumfloat;
	      fscanf(fptr->fp,dp,&dumfloat);
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else
		values.push_back(Array::doubleConstructor(dumfloat));
	    } else {
	      float dumfloat;
	      fscanf(fptr->fp,dp,&dumfloat);
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else
		values.push_back(Array::floatConstructor(dumfloat));
	    }
	    break;
	  case 's':
	    char stbuff[4096];
	    fscanf(fptr->fp,dp,stbuff);
	    if (feof(fptr->fp))
	      values.push_back(Array::emptyConstructor());
	    else
	      values.push_back(Array::stringConstructor(stbuff));
	    break;
	  default:
	    throw Exception("unsupported fscanf configuration");
	  }
	}
      }
      return values;
    }
#endif

    //!
    //@Module FSCANF Formatted File Input Function (C-Style)
    //@@Section IO
    //@@Usage
    //Reads values from a file.  The general syntax for its use is
    //@[
    //  [a1,...,an] = fscanf(handle,format)
    //@]
    //Here @|format| is the format string, which is a string that
    //controls the format of the input.  Each value that is parsed from
    //the file described by @|handle| occupies one output slot.
    //See @|printf| for a description of the format.  Note that if
    //the file is at the end-of-file, the fscanf will return 
    //!
    ArrayVector FscanfFunction(int nargout, const ArrayVector& arg) {
      if (arg.size() != 2)
	throw Exception("fscanf takes two arguments, the file handle and the format string");
      Array tmp(arg[0]);
      int handle = tmp.getContentsAsIntegerScalar();
      FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
      Array format(arg[1]);
      if (!format.isString())
	throw Exception("fscanf format argument must be a string");
      if (feof(fptr->fp))
	return singleArrayVector(Array::emptyConstructor());
      char *frmt = format.getContentsAsCString();
      char *buff = (char*) malloc(strlen(frmt)+1);
      strcpy(buff,frmt);
      // Search for the start of a format subspec
      char *dp = buff;
      char *np;
      char sv;
      int nextArg = 2;
      bool shortarg;
      bool doublearg;
      // Scan the string
      ArrayVector values;
      while (*dp) {
	np = dp;
	while ((*dp) && (*dp != '%')) dp++;
	// Print out the formatless segment
	sv = *dp;
	*dp = 0;
	fscanf(fptr->fp,np);
	if (feof(fptr->fp))
	  values.push_back(Array::emptyConstructor());
	*dp = sv;
	// Process the format spec
	if (*dp) {
	  np = validateScanFormatSpec(dp+1);
	  if (!np)
	    throw Exception("erroneous format specification " + std::string(dp));
	  else {
	    if (*(np-1) == '%') {
	      fscanf(fptr->fp,"%%");
	      dp+=2;
	    } else {
	      shortarg = false;
	      doublearg = false;
	      if (*(np-1) == 'h') {
		shortarg = true;
		np++;
	      } else if (*(np-1) == 'l') {
		doublearg = true;
		np++;
	      } 
	      sv = *np;
	      *np = 0;
	      switch (*(np-1)) {
	      case 'd':
	      case 'i':
		if (shortarg) {
		  short sdumint;
		  fscanf(fptr->fp,dp,&sdumint);
		  if (feof(fptr->fp))
		    values.push_back(Array::emptyConstructor());
		  else
		    values.push_back(Array::int16Constructor(sdumint));
		} else {
		  int sdumint;
		  fscanf(fptr->fp,dp,&sdumint);
		  if (feof(fptr->fp))
		    values.push_back(Array::emptyConstructor());
		  else
		    values.push_back(Array::int32Constructor(sdumint));
		}
		break;
	      case 'o':
	      case 'u':
	      case 'x':
	      case 'X':
	      case 'c':
		if (shortarg) {
		  int sdumint;
		  fscanf(fptr->fp,dp,&sdumint);
		  if (feof(fptr->fp))
		    values.push_back(Array::emptyConstructor());
		  else
		    values.push_back(Array::int32Constructor(sdumint));
		} else {
		  unsigned int dumint;
		  fscanf(fptr->fp,dp,&dumint);
		  if (feof(fptr->fp))
		    values.push_back(Array::emptyConstructor());
		  else
		    values.push_back(Array::uint32Constructor(dumint));
		}
		break;
	      case 'e':
	      case 'E':
	      case 'f':
	      case 'F':
	      case 'g':
	      case 'G':
		if (doublearg) {
		  double dumfloat;
		  fscanf(fptr->fp,dp,&dumfloat);
		  if (feof(fptr->fp))
		    values.push_back(Array::emptyConstructor());
		  else
		    values.push_back(Array::doubleConstructor(dumfloat));
		} else {
		  float dumfloat;
		  fscanf(fptr->fp,dp,&dumfloat);
		  if (feof(fptr->fp))
		    values.push_back(Array::emptyConstructor());
		  else
		    values.push_back(Array::floatConstructor(dumfloat));
		}
		break;
	      case 's':
		char stbuff[4096];
		fscanf(fptr->fp,dp,stbuff);
		if (feof(fptr->fp))
		  values.push_back(Array::emptyConstructor());
		else
		  values.push_back(Array::stringConstructor(stbuff));
		break;
	      default:
		throw Exception("unsupported fscanf configuration");
	      }
	      *np = sv;
	      dp = np;
	    }
	  }
	}
      }
      return values;
    }

    //!
    //@Module FPRINTF Formated File Output Function (C-Style)
    //@@Section IO
    //@@Usage
    //Prints values to a file.  The general syntax for its use is
    //@[
    //  fprintf(fp,format,a1,a2,...).
    //@]
    //Here @|format| is the format string, which is a string that
    //controls the format of the output.  The values of the variables
    //@|ai| are substituted into the output as required.  It is
    //an error if there are not enough variables to satisfy the format
    //string.  Note that this @|fprintf| command is not vectorized!  Each
    //variable must be a scalar.  The value @|fp| is the file handle.  For
    //more details on the format string, see @|printf|.  Note also 
    //that @|fprintf| to the file handle @|1| is effectively equivalent to @|printf|.
    //@@Examples
    //A number of examples are present in the Examples section of the @|printf| command.
    //!
    ArrayVector FprintfFunction(int nargout, const ArrayVector& arg) {
      if (arg.size() < 2)
	throw Exception("fprintf requires at least two arguments, the file handle and theformat string");
      Array tmp(arg[0]);
      int handle = tmp.getContentsAsIntegerScalar();
      FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
      Array format(arg[1]);
      if (!format.isString())
	throw Exception("fprintf format argument must be a string");
      ArrayVector argcopy(arg);
      argcopy.erase(argcopy.begin());
      char *op = xprintfFunction(nargout,argcopy);
      char *buff = (char*) malloc(strlen(op)+1);
      convertEscapeSequences(buff,op);
      fprintf(fptr->fp,"%s",buff);
      free(buff);
      free(op);
      return ArrayVector();
    }

    matvar_t * Mat_VarCreateHandling(const Array& toWrite, char * varName, 
				     int isGlobal) {
      matvar_t *matvar;
      int flags = MEM_CONSERVE;
      if (isGlobal) {
	flags |= MAT_F_GLOBAL;
      }
      Class dclass(toWrite.getDataClass());
      Dimensions dims = toWrite.getDimensions();
      switch (dclass) {
      case FM_CELL_ARRAY: {
	const Array *dp = ((const Array *) toWrite.getDataPointer());
	matvar_t **matsubvar = (matvar_t **) Malloc((toWrite.getLength()) * 
						    sizeof(matvar_t *));
	for (int j = 0; j < toWrite.getLength(); j++) {
	  matsubvar[j] = Mat_VarCreateHandling(dp[j], "dummy", isGlobal);
	}
	matvar = Mat_VarCreate(varName, MAT_C_CELL, MAT_T_CELL, dims.getLength(), 
			       dims.getDimensionData(), (void *) matsubvar, flags);
	break;
      }
      case FM_STRUCT_ARRAY: {
	stringVector fieldNames(toWrite.getFieldNames());
	int ncount(fieldNames.size());
	const Array *dp = ((const Array *) toWrite.getDataPointer());
	matvar_t **matsubvar = (matvar_t **) Malloc((toWrite.getLength() * ncount + 1) * sizeof(matvar_t *));
	for (int j = 0; j < toWrite.getLength(); j++) {
	  for (int k = 0; k < ncount; k++) {
	    matsubvar[j * ncount + k] = Mat_VarCreateHandling(dp[j * ncount + k], 
							      (char*) fieldNames[k].c_str(), 
							      isGlobal);
	  }
	}
	matsubvar[toWrite.getLength() * ncount] = NULL;
	matvar = Mat_VarCreate(varName, MAT_C_STRUCT, MAT_T_STRUCT, 
			       dims.getLength(), dims.getDimensionData(), 
			       (void *) matsubvar, flags);
	break;
      }
      case FM_LOGICAL: {
	flags |= MAT_F_LOGICAL;
	const logical *dp = ((const logical *) toWrite.getDataPointer());
	matvar = Mat_VarCreate(varName, MAT_C_UINT8, MAT_T_UINT8, 
			       dims.getLength(), dims.getDimensionData(), 
			       (void *) dp, flags);
	break;
      }
      case FM_STRING: {
	const uint8 *dp = ((const uint8 *) toWrite.getDataPointer());
	matvar = Mat_VarCreate(varName, MAT_C_CHAR, MAT_T_UINT8, dims.getLength(), 
			       dims.getDimensionData(), (void *) dp, flags);
	break;
      }
      case FM_UINT8: {
	const uint8 *dp = ((const uint8 *) toWrite.getDataPointer());
	matvar = Mat_VarCreate(varName, MAT_C_UINT8, MAT_T_UINT8, dims.getLength(), 
			       dims.getDimensionData(), (void *) dp, flags);
	break;
      }
      case FM_UINT16: {
	const uint16 *dp = ((const uint16 *) toWrite.getDataPointer());
	matvar = Mat_VarCreate(varName, MAT_C_UINT16, MAT_T_UINT16, dims.getLength(), 
			       dims.getDimensionData(), (void *) dp, flags);
	break;
      }
      case FM_UINT32: {
	const uint32 *dp = ((const uint32 *) toWrite.getDataPointer());
	matvar = Mat_VarCreate(varName, MAT_C_UINT32, MAT_T_UINT32, dims.getLength(), 
			       dims.getDimensionData(), (void *) dp, flags);
	break;
      }
      case FM_INT8: {
	const int8 *dp = ((const int8 *) toWrite.getDataPointer());
	matvar = Mat_VarCreate(varName, MAT_C_INT8, MAT_T_INT8, dims.getLength(), 
			       dims.getDimensionData(), (void *) dp, flags);
	break;
      }
      case FM_INT16: {
	const int16 *dp = ((const int16 *) toWrite.getDataPointer());
	matvar = Mat_VarCreate(varName, MAT_C_INT16, MAT_T_INT16, dims.getLength(), 
			       dims.getDimensionData(), (void *) dp, flags);
	break;
      }
      case FM_INT32: {
	if (!toWrite.isSparse()) {
	  const int32 *dp = ((const int32 *) toWrite.getDataPointer());
	  matvar = Mat_VarCreate(varName, MAT_C_INT32, MAT_T_INT32, dims.getLength(), 
				 dims.getDimensionData(), (void *) dp, flags);
	}
	else {
	  int32 *dp;
	  uint32 *ir;
	  uint32 *jc;
	  int nnz;
	  dp = (int32 *) SparseToIJV2(FM_INT32, toWrite.getDimensionLength(0), 
				      toWrite.getDimensionLength(1), 
				      toWrite.getSparseDataPointer(), ir, jc, nnz);
	  sparse_t * sparse = (sparse_t *) Malloc(sizeof(sparse_t));
	  sparse->nzmax = nnz;
	  sparse->nir   = nnz;
	  sparse->ir    = (int32 *) ir;
	  sparse->njc   = toWrite.getDimensionLength(1) + 1;
	  sparse->jc    = (int32 *) jc;
	  sparse->ndata = nnz;
	  sparse->data  = dp;
	  matvar = Mat_VarCreate(varName, MAT_C_SPARSE, MAT_T_INT32, dims.getLength(), 
				 dims.getDimensionData(), (void *) sparse, flags);
	}
	break;
      }
      case FM_FLOAT: {
	if (!toWrite.isSparse()) {
	  const float *dp = ((const float *) toWrite.getDataPointer());
	  matvar = Mat_VarCreate(varName, MAT_C_SINGLE, MAT_T_SINGLE, dims.getLength(), 
				 dims.getDimensionData(), (void *) dp, flags);
	}
	else {
	  float *dp;
	  uint32 *ir;
	  uint32 *jc;
	  int nnz;
	  dp = (float *) SparseToIJV2(FM_FLOAT, toWrite.getDimensionLength(0), 
				      toWrite.getDimensionLength(1), 
				      toWrite.getSparseDataPointer(), ir, jc, nnz);
	  sparse_t * sparse = (sparse_t *) Malloc(sizeof(sparse_t));
	  sparse->nzmax = nnz;
	  sparse->nir   = nnz;
	  sparse->ir    = (int32 *) ir;
	  sparse->njc   = toWrite.getDimensionLength(1) + 1;
	  sparse->jc    = (int32 *) jc;
	  sparse->ndata = nnz;
	  sparse->data  = dp;
	  matvar = Mat_VarCreate(varName, MAT_C_SPARSE, MAT_T_SINGLE, dims.getLength(), 
				 dims.getDimensionData(), (void *) sparse, flags);
	}
	break;
      }
      case FM_DOUBLE: {
	if (!toWrite.isSparse()) {
	  const double *dp = ((const double *) toWrite.getDataPointer());
	  matvar = Mat_VarCreate(varName, MAT_C_DOUBLE, MAT_T_DOUBLE, dims.getLength(), 
				 dims.getDimensionData(), (void *) dp, flags);
	}
	else {
	  double *dp;
	  uint32 *ir;
	  uint32 *jc;
	  int nnz;
	  dp = (double *) SparseToIJV2(FM_DOUBLE, toWrite.getDimensionLength(0), 
				       toWrite.getDimensionLength(1), 
				       toWrite.getSparseDataPointer(), ir, jc, nnz);
	  sparse_t * sparse = (sparse_t *) Malloc(sizeof(sparse_t));
	  sparse->nzmax = nnz;
	  sparse->nir   = nnz;
	  sparse->ir    = (int32 *) ir;
	  sparse->njc   = toWrite.getDimensionLength(1) + 1;
	  sparse->jc    = (int32 *) jc;
	  sparse->ndata = nnz;
	  sparse->data  = dp;
	  matvar = Mat_VarCreate(varName, MAT_C_SPARSE, MAT_T_DOUBLE, 
				 dims.getLength(), dims.getDimensionData(), (void *) sparse, flags);
	}
	break;
      }
      case FM_COMPLEX: {
	flags |= MAT_F_COMPLEX;
	if (!toWrite.isSparse()) {
	  const float *dp = ((const float *) toWrite.getDataPointer());
	  int length = toWrite.getLength();
	  int length2= length + length;
	  float *dstPtr = (float *) Malloc(sizeof(double) * length2);
	  int j, k = 0;
	  // unfortunately need to convert data from interleaved to split storage
	  for (j = 0; j < length2; j++) {
	    dstPtr[k] = dp[j++];
	    dstPtr[length + k++] = dp[j];
	  }
	  matvar = Mat_VarCreate(varName, MAT_C_SINGLE, MAT_T_SINGLE, 
				 dims.getLength(), dims.getDimensionData(), (void *) dstPtr, flags);
	}
	else {
	  float *dp;
	  uint32 *ir;
	  uint32 *jc;
	  int nnz;
	  dp = (float *) SparseToIJV2(FM_COMPLEX, toWrite.getDimensionLength(0), 
				      toWrite.getDimensionLength(1), 
				      toWrite.getSparseDataPointer(), ir, jc, nnz);
	  sparse_t * sparse = (sparse_t *) malloc(sizeof(sparse_t));
	  sparse->nzmax = 2 * nnz;
	  sparse->nir   = nnz;
	  sparse->ir    = (int32 *) ir;
	  sparse->njc   = toWrite.getDimensionLength(1) + 1;
	  sparse->jc    = (int32 *) jc;
	  sparse->ndata = nnz;
	  sparse->data  = dp;
	  matvar = Mat_VarCreate(varName, MAT_C_SPARSE, MAT_T_SINGLE, dims.getLength(), 
				 dims.getDimensionData(), (void *) sparse, flags);
	}
	break;
      }
      case FM_DCOMPLEX: {
	flags |= MAT_F_COMPLEX;
	if (!toWrite.isSparse()) {
	  const double *dp = ((const double *) toWrite.getDataPointer());
	  int length = toWrite.getLength();
	  int length2= length + length;
	  double *dstPtr = (double *) Malloc(sizeof(double) * length2);
	  int j, k = 0;
	  // unfortunately need to convert data from interleaved to split storage
	  for (j = 0; j < length2; j++) {
	    dstPtr[k] = dp[j++];
	    dstPtr[length + k++] = dp[j];
	  }
	  matvar = Mat_VarCreate(varName, MAT_C_DOUBLE, MAT_T_DOUBLE, dims.getLength(), 
				 dims.getDimensionData(), (void *) dstPtr, flags);
	}
	else {
	  double *dp;
	  uint32 *ir;
	  uint32 *jc;
	  int nnz;
	  dp = (double *) SparseToIJV2(FM_DCOMPLEX, toWrite.getDimensionLength(0), 
				       toWrite.getDimensionLength(1), 
				       toWrite.getSparseDataPointer(), ir, jc, nnz);
	  sparse_t * sparse = (sparse_t *) Malloc(sizeof(sparse_t));
	  sparse->nzmax = 2 * nnz;
	  sparse->nir   = nnz;
	  sparse->ir    = (int32 *) ir;
	  sparse->njc   = toWrite.getDimensionLength(1) + 1;
	  sparse->jc    = (int32 *) jc;
	  sparse->ndata = nnz;
	  sparse->data  = dp;
	  matvar = Mat_VarCreate(varName, MAT_C_SPARSE, MAT_T_DOUBLE, dims.getLength(), 
				 dims.getDimensionData(), (void *) sparse, flags);
	}
	break;
      }
      }
      return matvar;
    }

    void Mat_VarFreeHandling(const Array& toWrite, matvar_t *matvar) {
      Class dclass(toWrite.getDataClass());
      switch (dclass) {
      case FM_CELL_ARRAY: {
	const Array *dp = ((const Array *) toWrite.getDataPointer());
	matvar_t **fields = (matvar_t **) matvar->data;
	for (int j = 0; j < toWrite.getLength(); j++) {
	  Mat_VarFreeHandling(dp[j], fields[j]);
	}
	Free((void *) matvar->data);
	matvar->data = NULL;
	break;
      }
      case FM_STRUCT_ARRAY: {
	stringVector fieldNames(toWrite.getFieldNames());
	int ncount(fieldNames.size());
	const Array *dp = ((const Array *) toWrite.getDataPointer());
	matvar_t **fields = (matvar_t **) matvar->data;
	for (int j = 0; j < toWrite.getLength() * ncount; j++) {
	  Mat_VarFreeHandling(dp[j], fields[j]);
	}
	Free((void *) matvar->data);
	matvar->data = NULL;
	break;
      }
      case FM_INT32:
      case FM_FLOAT:
      case FM_DOUBLE: {
	if (toWrite.isSparse()) {
	  sparse_t *sparse = (sparse_t *) matvar->data;
	  Free((void *) sparse->data);
	  Free((void *) sparse->ir);
	  Free((void *) sparse->jc);
	  Free((void *) sparse);
	  sparse = NULL;
	}
	break;
      }
      case FM_COMPLEX:
      case FM_DCOMPLEX: {
	if (!toWrite.isSparse()) {
	  Free((void *) matvar->data);
	  matvar->data = NULL;
	}
	else {
	  sparse_t *sparse = (sparse_t *) matvar->data;
	  Free((void *) sparse->data);
	  Free((void *) sparse->ir);
	  Free((void *) sparse->jc);
	  Free((void *) sparse);
	  sparse = NULL;
	}
	break;
      }
      }
    }


    ArrayVector SaveMatFunction(int nargout, const ArrayVector& arg,
				WalkTree* eval) {
      if (arg.size() == 0) {
	throw Exception("save requires at least one argument (the filename)");
      }
      Array filename(arg[0]);
      char *fname = filename.getContentsAsCString();
      Context *cntxt = eval->getContext();
      stringVector names;
      int i;
      if (arg.size() == 1) {
	names = cntxt->getCurrentScope()->listAllVariables();
      } else {
	for (i = 1; i < arg.size(); i++) {
	  Array varName(arg[i]);
	  names.push_back(varName.getContentsAsCString());
	}
      }
      char header[116];
      time_t t = time(NULL);
      mat_snprintf(header, 116, 
		   "MATLAB 5.0 MAT-file, Platform: %s, Created on: %s by %s", 
		   MATIO_PLATFORM, ctime(&t), 
		   WalkTree::getVersionString().c_str());
      mat_t *mat = Mat_Create(fname, header);
      if (mat) {
	for (i = 0; i < names.size(); i++) {
	  if (!(names[i].compare("ans") == 0)) {
	    Array *toWrite = cntxt->lookupVariable(names[i]);
	    int isGlobal = (cntxt->isVariableGlobal(names[i])) ? 1 : 0;
	    matvar_t *matvar = Mat_VarCreateHandling(*toWrite, (char*) names[i].c_str(), isGlobal);
	    if (matvar) {
	      Mat_VarWrite(mat, matvar, 0);
	      Mat_VarFreeHandling(*toWrite, matvar);
	      Mat_VarFree(matvar);
	    }
	    else {
	      throw Exception(std::string("unable to create MAT-file variable ") + names[i]);
	    }
	  }
	}
	Mat_Close(mat);
	Free((void *) fname);
	names.clear();
      }
      else {
	throw Exception(std::string("unable to create MAT-file ") + fname);
      }
      return ArrayVector();
    }
  
    ArrayVector SaveNativeFunction(int nargout, const ArrayVector& arg, 
				   WalkTree* eval) {
      if (arg.size() == 0)
	throw Exception("save requires at least one argument (the filename)");
      Array filename(arg[0]);
      char *fname = filename.getContentsAsCString();
      File ofile(fname,"wb");
      Serialize output(&ofile);
      output.handshakeServer();
      Context *cntxt;
      cntxt = eval->getContext();
      stringVector names;
      int i;
      if (arg.size() == 1)
	names = cntxt->getCurrentScope()->listAllVariables();
      else {
	for (i=1;i<arg.size();i++) {
	  Array varName(arg[i]);
	  names.push_back(varName.getContentsAsCString());
	}
      }
      for (i=0;i<names.size();i++) {
	Array *toWrite;
	char flags;
	if (!(names[i].compare("ans") == 0)) {
	  toWrite = cntxt->lookupVariable(names[i]);
	  if (!toWrite)
	    throw Exception(std::string("unable to find variable ")+
			    names[i]+" to save to file "+fname);
	  flags = 'n';
	  if (cntxt->isVariableGlobal(names[i]))
	    flags = 'g';
	  if (cntxt->isVariablePersistent(names[i]))	
	    flags = 'p';
	  output.putString(names[i].c_str());
	  output.putByte(flags);
	  output.putArray(*toWrite);
	}
      }
      output.putString("__eof");
      return ArrayVector();
    }
  

    //!
    //@Module SAVE Save Variables To A File
    //@@Section IO
    //@@Usage
    //Saves a set of variables to a file in a machine independent format.
    //There are two formats for the function call.  The first is the explicit
    //form, in which a list of variables are provided to write to the file:
    //@[
    //  save filename a1 a2 ...
    //@]
    //In the second form,
    //@[
    //  save filename
    //@]
    //all variables in the current context are written to the file.  The 
    //format of the file is a simple binary encoding (raw) of the data
    //with enough information to restore the variables with the @|load|
    //command.  The endianness of the machine is encoded in the file, and
    //the resulting file should be portable between machines of similar
    //types (in particular, machines that support IEEE floating point 
    //representation).
    //
    //You can also specify both the filename as a string, in which case
    //you also have to specify the names of the variables to save.  In
    //particular
    //@[
    //   save('filename','a1','a2')
    //@]
    //will save variables @|a1| and @|a2| to the file.
    //
    //Starting with version 2.0, FreeMat can also read and write MAT
    //files (the file format used by MATLAB) thanks to substantial 
    //work by Thomas Beutlich.  Support for MAT files
    //is still in the alpha stages, so please be cautious with using
    //it to store critical data.  Also, things like objects wont be
    //saved properly, as will variables that dont exist in MATLAB
    //such as single-precision sparse types.  The file format is triggered
    //by the extension.  To save files with a MAT format, simply
    //use a filename with a ".mat" ending.
    //@@Example
    //Here is a simple example of @|save|/@|load|.  First, we save some variables to a file.
    //@< 
    //D = {1,5,'hello'};
    //s = 'test string';
    //x = randn(512,1);
    //z = zeros(512);
    //who
    //save loadsave.dat
    //@>
    //Next, we clear all of the variables, and then load them back from the file.
    //@<
    //clear all
    //who
    //load loadsave.dat
    //who
    //@>
    //!
    ArrayVector SaveFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
      if (arg.size() == 0)
	throw Exception("save requires at least one argument (the filename)");
      Array filename(arg[0]);
      char *fname = filename.getContentsAsCString();
      int len = strlen(fname);
      if ((len >= 4) && (fname[len-4] == '.') &&
	  ((fname[len-3] == 'M') || (fname[len-3] == 'm')) &&
	  ((fname[len-2] == 'A') || (fname[len-2] == 'a')) &&
	  ((fname[len-1] == 'T') || (fname[len-1] == 't')))
	return SaveMatFunction(nargout,arg,eval);
      return SaveNativeFunction(nargout,arg,eval);
    }

    void Mat_VarGetSparseArray(Array& toRead, matvar_t * matvar, const Dimensions& dims, Class dclass) {
      sparse_t * sparse = (sparse_t *) matvar->data;
      int i;
      uint32 * ir = (uint32 *) Malloc(sizeof(uint32) * sparse->nzmax);
      uint32 * jc = (uint32 *) Malloc(sizeof(uint32) * sparse->nzmax);
      memset(ir, 0, sizeof(uint32) * sparse->nzmax);
      memset(jc, 0, sizeof(uint32) * sparse->nzmax);
      for (i = 0; i < sparse->nzmax; i++) {
	ir[i] = (uint32) sparse->ir[i] + 1;
      }
      int k = 0;
      for (int col = 0; col < sparse->njc; col++) {
	int starting_row_index = sparse->jc[col];
	int stopping_row_index = sparse->jc[col + 1];
	if (starting_row_index == stopping_row_index) {
	  continue;
	}
	else {
	  for (int current_row_index = starting_row_index; 
	       current_row_index < stopping_row_index; current_row_index++) {
	    jc[k++] = (uint32) col + 1;
	  }
	}
      }
      k = sparse->nzmax;
      for (i = sparse->nzmax - 1; i>= 0; i--) {
	if (jc[i] == 0) k--;
      }

      switch (dclass) {
      case FM_COMPLEX:
	{
	  int elCount = 2 * sparse->nzmax, k = 0;
	  float *dp = (float *) Malloc(elCount * sizeof(float));
	  // unfortunately need to convert data from split to interleaved storage
	  for (int j = 0; j < elCount; j++) {
	    dp[j++] = ((float *) sparse->data)[k];
	    dp[j] = ((float *) sparse->data)[elCount / 2 + k++];
	  }
	  toRead = Array(dclass, dims, makeSparseFromIJV(dclass, matvar->dims[0], matvar->dims[1], k, ir, 1, jc, 1, dp, 1), true);
	}
	break;
      case FM_DCOMPLEX:
	{
	  int elCount = 2 * sparse->nzmax, k = 0;
	  double *dp = (double *) Malloc(elCount * sizeof(double));
	  // unfortunately need to convert data from split to interleaved storage
	  for (int j = 0; j < elCount; j++) {
	    dp[j++] = ((double *) sparse->data)[k];
	    dp[j] = ((double *) sparse->data)[elCount / 2 + k++];
	  }
	  toRead = Array(dclass, dims, makeSparseFromIJV(dclass, matvar->dims[0], matvar->dims[1], k, ir, 1, jc, 1, dp, 1), true);
	}
	break;
      default:
	{
	  if (matvar->isLogical) {
	    int elCount = sparse->nzmax;
	    int32 *dp = (int32 *) Malloc(elCount * sizeof(int32));
	    for (int j = 0; j < elCount; j++) { // convert from logical to int32
	      dp[j] = ((int32) ((uint8 *) sparse->data)[j]);
	    }
	    toRead = Array(FM_INT32, dims, makeSparseFromIJV(FM_INT32, matvar->dims[0], matvar->dims[1], k, ir, 1, jc, 1, dp, 1), true);
	  }
	  else {
	    toRead = Array(dclass, dims, makeSparseFromIJV(dclass, matvar->dims[0], matvar->dims[1], k, ir, 1, jc, 1, sparse->data, 1), true);
	  }
	}
      }
      Free((void *) ir);
      Free((void *) jc);
    }

    void Mat_VarGetHandling(Array& toRead, matvar_t *matvar) {
      int i;
      Dimensions dims;
      for (i = 0; i < matvar->rank; i++) {
	dims[i] = matvar->dims[i];
      }
      switch (matvar->class_type) {
      case MAT_C_CELL: {
	int elCount = matvar->nbytes / matvar->data_size;
	Array *dp = new Array[elCount];
	matvar_t **cells = (matvar_t **) matvar->data;
	for (i = 0; i < elCount; i++) {
	  Mat_VarGetHandling(dp[i], cells[i]);
	}
	toRead = Array(FM_CELL_ARRAY, dims, dp);
	break;
      }
      case MAT_C_STRUCT: {
	stringVector fieldNames;
	int elCount = 1;
	for (i = 0; i < matvar->rank; i++) {
	  elCount *= matvar->dims[i];
	}
	int nfields = matvar->nbytes / matvar->data_size / elCount;
	matvar_t **fields = (matvar_t **) matvar->data;
	Array *dp = new Array[nfields * elCount];
	for (i = 0; i < nfields; i++) {
	  fieldNames.push_back((fields[i])->name);
	}
	for (i = 0; i < nfields * elCount; i++) {
	  Mat_VarGetHandling(dp[i], fields[i]);
	}
	toRead = Array(FM_STRUCT_ARRAY, dims, dp, false, fieldNames);
	break;
      }
      case MAT_C_CHAR: {
	char *dp = (char *) Malloc(matvar->nbytes);
	memcpy((void *) dp, matvar->data, matvar->nbytes);
	toRead = Array(FM_STRING, dims, dp);
	break;
      }
      case MAT_C_UINT8: {
	uint8 *dp = (uint8 *) Malloc(matvar->nbytes);
	memcpy((void *) dp, matvar->data, matvar->nbytes);
	if (matvar->isLogical) {
	  toRead = Array(FM_LOGICAL, dims, dp);
	}
	else {
	  toRead = Array(FM_UINT8, dims, dp);
	}
	break;
      }
      case MAT_C_UINT16: {
	uint16 *dp = (uint16 *) Malloc(matvar->nbytes);
	memcpy((void *) dp, matvar->data, matvar->nbytes);
	toRead = Array(FM_UINT16, dims, dp);
	break;
      }
      case MAT_C_UINT32: {
	uint32 *dp = (uint32 *) Malloc(matvar->nbytes);
	memcpy((void *) dp, matvar->data, matvar->nbytes);
	toRead = Array(FM_UINT32, dims, dp);
	break;
      }
      case MAT_C_INT8: {
	int8 *dp = (int8 *) Malloc(matvar->nbytes);
	memcpy((void *) dp, matvar->data, matvar->nbytes);
	toRead = Array(FM_INT8, dims, dp);
	break;
      }
      case MAT_C_INT16: {
	int16 *dp = (int16 *) Malloc(matvar->nbytes);
	memcpy((void *) dp, matvar->data, matvar->nbytes);
	toRead = Array(FM_INT16, dims, dp);
	break;
      }
      case MAT_C_INT32: {
	if (matvar->class_type != MAT_C_SPARSE) {
	  int32 *dp = (int32 *) Malloc(matvar->nbytes);
	  memcpy((void *) dp, matvar->data, matvar->nbytes);
	  toRead = Array(FM_INT32, dims, dp);
	}
	else {
	  Mat_VarGetSparseArray(toRead, matvar, dims, FM_INT32);
	}
	break;
      }
      case MAT_C_SINGLE: {
	float *dp = (float *) Malloc(matvar->nbytes);
	if (matvar->isComplex) {
	  int elCount = matvar->nbytes / matvar->data_size, k = 0;
	  // unfortunately need to convert data from split to interleaved storage
	  for (int j = 0; j < elCount; j++) {
	    dp[j++] = ((float *) matvar->data)[k];
	    dp[j] = ((float *) matvar->data)[elCount / 2 + k++];
	  }
	  toRead = Array(FM_COMPLEX, dims, dp);
	}
	else {
	  memcpy((void *) dp, matvar->data, matvar->nbytes);
	  toRead = Array(FM_FLOAT, dims, dp);
	}
	break;
      }
      case MAT_C_DOUBLE: {
	double *dp = (double *) Malloc(matvar->nbytes);
	if (matvar->isComplex) {
	  int elCount = matvar->nbytes / matvar->data_size, k = 0;
	  // unfortunately need to convert data from split to interleaved storage
	  for (int j = 0; j < elCount; j++) {
	    dp[j++] = ((double *) matvar->data)[k];
	    dp[j] = ((double *) matvar->data)[elCount / 2 + k++];
	  }
	  toRead = Array(FM_DCOMPLEX, dims, dp);
	}
	else {
	  memcpy((void *) dp, matvar->data, matvar->nbytes);
	  toRead = Array(FM_DOUBLE, dims, dp);
	}
	break;
      }
      case MAT_C_SPARSE: {
	if (matvar->isComplex) {
	  switch (matvar->data_type) {
	  case MAT_T_SINGLE:
	    Mat_VarGetSparseArray(toRead, matvar, dims, FM_COMPLEX);
	    break;
	  case MAT_T_DOUBLE:
	    Mat_VarGetSparseArray(toRead, matvar, dims, FM_DCOMPLEX);
	    break;
	  default:
	    throw Exception("unknown data type of sparse MAT-file variable " + std::string(matvar->name));
	    break;
	  }
	}
	else {
	  switch (matvar->data_type) {
	  case MAT_T_INT32:
	    Mat_VarGetSparseArray(toRead, matvar, dims, FM_INT32);
	    break;
	  case MAT_T_SINGLE:
	    Mat_VarGetSparseArray(toRead, matvar, dims, FM_FLOAT);
	    break;
	  case MAT_T_DOUBLE:
	    Mat_VarGetSparseArray(toRead, matvar, dims, FM_DOUBLE);
	    break;
	  case MAT_T_UINT8:
	    if (matvar->isLogical) {
	      Mat_VarGetSparseArray(toRead, matvar, dims, FM_INT32);
	    }
	    break;
	  default:
	    throw Exception("unknown data type of sparse MAT-file variable " + std::string(matvar->name));
	    break;
	  }
	}
	break;
      }
      case MAT_C_OBJECT:
      case MAT_C_FUNCTION:
	// do nothing
	break;
      default:
	if (matvar->nbytes == 0 && matvar->class_type <= MAT_C_FUNCTION) {
	  double *dp = (double *) Malloc(0);
	  memcpy((void *) dp, matvar->data, 0);
	  toRead = Array(FM_DOUBLE, dims, dp);
	}
	else {
	  throw Exception("unknown class type of variable " + std::string(matvar->name));
	}
	break;
      }
    }

    ArrayVector LoadMatFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
      if (arg.size() != 1)
	throw Exception("load requires exactly one argument (the filename)");
      Array filename(arg[0]);
      char *fname = filename.getContentsAsCString();
      if (arg.size() != 1) {
	throw Exception("load requires exactly one argument (the filename)");
      }
      Mat_LogInit("dummy");
      mat_t *mat = Mat_Open(fname, MAT_ACC_RDONLY);
      if (!mat) {
	mat = Mat_Open(fname, MAT_ACC_RDONLY | MAT_V4);
      }
      if (mat) {
	matvar_t *matvar;
	stringVector invalidmatvars;
	while (NULL != (matvar = Mat_VarReadNext(mat))) {
	  char *arrayName = strdup(matvar->name);
	  if (NULL == arrayName || (0 == strcmp(arrayName, ""))) {
	    continue;
	  }
	  Array toRead;
	  Mat_VarGetHandling(toRead, matvar);
	  if (NULL != toRead.getSparseDataPointer()) {
	    if (matvar->isGlobal) {
	      eval->getContext()->addGlobalVariable(arrayName);
	    }
	    eval->getContext()->insertVariable(arrayName, toRead);
	  }
	  else {
	    invalidmatvars.push_back(arrayName);
	  }
	}
	Mat_Close(mat);
	Free((void *) fname);
	if (invalidmatvars.size() > 0) {
	  char buffer[250];
	  Interface *io = eval->getInterface();
	  io->outputMessage("Warning: Ignored MAT-file variable(s): ");
	  for (int i = 0; i < invalidmatvars.size() - 1; i++) {
	    io->outputMessage(invalidmatvars[i].c_str());
	    io->outputMessage(", ");
	  }
	  io->outputMessage(invalidmatvars[invalidmatvars.size() - 1].c_str());
	  io->outputMessage("\n");
	  invalidmatvars.clear();
	}
      }
      else {
	throw Exception(std::string("unable to open MAT-file ") + fname);
      }
      return ArrayVector();
    }

    ArrayVector LoadNativeFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
      if (arg.size() != 1)
	throw Exception("load requires exactly one argument (the filename)");
      Array filename(arg[0]);
      char *fname = filename.getContentsAsCString();
      File ofile(fname,"rb");
      Serialize input(&ofile);
      input.handshakeClient();
      char *arrayName;
      arrayName = input.getString();
      while (strcmp(arrayName,"__eof") != 0) {
	Array toRead;
	char flag;
	flag = input.getByte();
	input.getArray(toRead);
	switch (flag) {
	case 'n':
	  break;
	case 'g':
	  eval->getContext()->addGlobalVariable(arrayName);
	  break;
	case 'p':
	  eval->getContext()->addPersistentVariable(arrayName);
	  break;
	default:
	  throw Exception(std::string("unrecognized variable flag ") + flag + 
			  std::string(" on variable ") + arrayName);
	}
	eval->getContext()->insertVariable(arrayName,toRead);
	arrayName = input.getString();
      }
      return ArrayVector();
    }

    //!
    //@Module LOAD Load Variables From A File
    //@@Section IO
    //@@Usage
    //Loads a set of variables from a file in a machine independent format.
    //The @|load| function takes one argument:
    //@[
    //  load filename,
    //@]
    //or alternately,
    //@[
    //  load('filename')
    //@]
    //This command is the companion to @|save|.  It loads the contents of the
    //file generated by @|save| back into the current context.  Global and 
    //persistent variables are also loaded and flagged appropriately.
    //@@Example
    //Here is a simple example of @|save|/@|load|.  First, we save some variables to a file.
    //@<
    //D = {1,5,'hello'};
    //s = 'test string';
    //x = randn(512,1);
    //z = zeros(512);
    //who
    //save loadsave.dat
    //@>
    //Next, we clear all of the variables, and then load them back from the file.
    //@<
    //clear all
    //who
    //load loadsave.dat
    //who
    //@>
    //!
    ArrayVector LoadFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
      if (arg.size() != 1)
	throw Exception("load requires exactly one argument (the filename)");
      Array filename(arg[0]);
      char *fname = filename.getContentsAsCString();
      int len = strlen(fname);
      if ((len >= 4) && (fname[len-4] == '.') &&
	  ((fname[len-3] == 'M') || (fname[len-3] == 'm')) &&
	  ((fname[len-2] == 'A') || (fname[len-2] == 'a')) &&
	  ((fname[len-1] == 'T') || (fname[len-1] == 't')))
	return LoadMatFunction(nargout,arg,eval);
      return LoadNativeFunction(nargout,arg,eval);
    }
  }

