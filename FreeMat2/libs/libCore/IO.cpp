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
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "Malloc.hpp"
#include "HandleList.hpp"
#include "WalkTree.hpp"
#include "File.hpp"
#include "Serialize.hpp"
// #include "Socket.hpp"
// #include "ServerSocket.hpp"
#include "IEEEFP.hpp"

namespace FreeMat {
  class FilePtr {
  public:
    FILE *fp;
    bool swapflag;
  };
  
//   class SockPtr {
//   public:
//     Socket *sock;
//     Serialize *ser;
//   };

  HandleList<FilePtr*> fileHandles;
  //  HandleList<ServerSocket*> servers;
//   HandleList<SockPtr*> sockets;

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
  //are not flushed.
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
	throw Exception(strerror(errno));
      fileHandles.deleteHandle(handle+1);
      delete fptr;
    }
    return ArrayVector();
  }

  //!
  //@Module FREAD File Read Function
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
      fsize = ftell(fptr->fp) - fcpos + 1;
      fseek(fptr->fp,fcpos,SEEK_SET);
      dp[infiniteDim] = ceil(fsize/elementSize/elementCount);
      elementCount *= (int) dp[infiniteDim];
    }
    // Next, we allocate space for the result
    void *qp = Malloc(elementCount*elementSize);
    // Read in the requested number of data points...
    if (fread(qp,elementSize,elementCount,fptr->fp) != elementCount)
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
  //fp = fopen('test.dat','rb');
  //x = fread(fp,[1,inf],'float');
  //fseek(fp,0,'bof');
  //y = fread(fp,[1,inf],'float');
  //who x y
  //fseek(fp,2048,'cof');
  //x = fread(fp,[512,1],'float');
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
    
  //!
  //@Module SPRINTF Formated String Output Function (C-Style)
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
	    if (nextVal.isEmpty()) {
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
    Array outString(Array::stringConstructor(op));
    free(op);
    ArrayVector retval;
    retval.push_back(outString);
    return retval;
  }
  
  //!
  //@Module PRINTF Formated Output Function (C-Style)
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
    char *frmt = format.getContentsAsCString();
    char *buff = (char*) malloc(strlen(frmt)+1);
    strcpy(buff,frmt);
    // Search for the start of a format subspec
    char *dp = buff;
    char *np;
    char sv;
    int nextArg = 1;
    Interface *io;
    io = eval->getInterface();
    // Scan the string
    while (*dp) {
      np = dp;
      while ((*dp) && (*dp != '%')) dp++;
      // Print out the formatless segment
      sv = *dp;
      *dp = 0;
      io->outputMessage(np);
      *dp = sv;
      char nbuff[4096];
      // Process the format spec
      if (*dp) {
	np = validateFormatSpec(dp+1);
	if (!np)
	  throw Exception("erroneous format specification " + std::string(dp));
	else {
	  if (*(np-1) == '%') {
	    io->outputMessage("%");
	    dp+=2;
	  } else {
	    sv = *np;
	    *np = 0;
	    if (arg.size() <= nextArg)
	      throw Exception("not enough arguments to satisfy format specification");
	    Array nextVal(arg[nextArg++]);
	    if (nextVal.isEmpty()) {
	      io->outputMessage("[]");
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
		io->outputMessage(nbuff);
		break;
	      case 'e':
	      case 'E':
	      case 'f':
	      case 'F':
	      case 'g':
	      case 'G':
	      nextVal.promoteType(FM_DOUBLE);
	      sprintf(nbuff,dp,*((double*)nextVal.getDataPointer()));
	      io->outputMessage(nbuff);
	      break;
	      case 's':
		sprintf(nbuff,dp,nextVal.getContentsAsCString());
		io->outputMessage(nbuff);
	      }
	    }
	    *np = sv;
	    dp = np;
	  }
	}
      }
    }
    return ArrayVector();
  }

  //!
  //@Module FGETLINE Read a String from a File
  //@@Usage
  //Reads a string from a file.  The general syntax for its use
  //is
  //@[
  //  s = fgetline(handle)
  //@]
  //This function reads characters from the file @|handle| into
  //a @|string| array @|s| until it encounters the end of the file
  //or a newline.  The newline, if any, is retained in the output
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
    Array retval(Array::stringConstructor(buffer));
    ArrayVector retvals;
    retvals.push_back(retval);
    return retvals;
  }

  //!
  //@Module FSCANF Formatted File Input Function (C-Style)
  //@@Usage
  //Reads values from a file.  The general syntax for its use is
  //@[
  //  [a1,...,an] = fscanf(handle,format)
  //@]
  //Here @|format| is the format string, which is a string that
  //controls the format of the input.  Each value that is parsed from
  //the file described by @|handle| occupies one output slot.
  //See @|printf| for a description of the format.
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
		values.push_back(Array::int16Constructor(sdumint));
	      } else {
		int sdumint;
		fscanf(fptr->fp,dp,&sdumint);
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
		values.push_back(Array::int32Constructor(sdumint));
	      } else {
		unsigned int dumint;
		fscanf(fptr->fp,dp,&dumint);
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
		values.push_back(Array::doubleConstructor(dumfloat));
	      } else {
		float dumfloat;
		fscanf(fptr->fp,dp,&dumfloat);
		values.push_back(Array::floatConstructor(dumfloat));
	      }
	      break;
	    case 's':
	      char stbuff[4096];
	      fscanf(fptr->fp,dp,stbuff);
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
    char *frmt = format.getContentsAsCString();
    char *buff = (char*) malloc(strlen(frmt)+1);
    strcpy(buff,frmt);
    // Search for the start of a format subspec
    char *dp = buff;
    char *np;
    char sv;
    int nextArg = 2;
    // Scan the string
    while (*dp) {
      np = dp;
      while ((*dp) && (*dp != '%')) dp++;
      // Print out the formatless segment
      sv = *dp;
      *dp = 0;
      fprintf(fptr->fp,np);
      *dp = sv;
      // Process the format spec
      if (*dp) {
	np = validateFormatSpec(dp+1);
	if (!np)
	  throw Exception("erroneous format specification " + std::string(dp));
	else {
	  if (*(np-1) == '%') {
	    fprintf(fptr->fp,"%%");
	    dp+=2;
	  } else {
	    sv = *np;
	    *np = 0;
	    if (arg.size() <= nextArg)
	      throw Exception("not enough arguments to satisfy format specification");
	    Array nextVal(arg[nextArg++]);
	    if (!nextVal.isEmpty()) {
	      switch (*(np-1)) {
	      case 'd':
	      case 'i':
	      case 'o':
	      case 'u':
	      case 'x':
	      case 'X':
	      case 'c':
		nextVal.promoteType(FM_INT32);
		fprintf(fptr->fp,dp,*((int32*)nextVal.getDataPointer()));
		break;
	      case 'e':
	      case 'E':
	      case 'f':
	      case 'F':
	      case 'g':
	      case 'G':
		nextVal.promoteType(FM_DOUBLE);
		fprintf(fptr->fp,dp,*((double*)nextVal.getDataPointer()));
		break;
	      case 's':
		fprintf(fptr->fp,dp,nextVal.getContentsAsCString());
	      }
	    }
	    *np = sv;
	    dp = np;
	  }
	}
      }
    }
    return ArrayVector();
  }

  //!
  //@Module SAVE Save Variables To A File
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
  //types (in particular, machines that support IEEE floating point).
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
      Array toWrite;
      char flags;
      if (!(names[i].compare("ans") == 0)) {
	if (!cntxt->lookupVariable(names[i],toWrite))
	  throw Exception(std::string("unable to find variable ")+
			names[i]+" to save to file "+fname);
	flags = 'n';
	if (cntxt->isVariableGlobal(names[i]))
	  flags = 'g';
	if (cntxt->isVariablePersistent(names[i]))	
	  flags = 'p';
	output.putString(names[i].c_str());
	output.putByte(flags);
	output.putArray(toWrite);
      }
    }
    output.putString("__eof");
    return ArrayVector();
  }

  //!
  //@Module LOAD Load Variables From A File
  //@@Usage
  //Loads a set of variables from a file in a machine independent format.
  //The @|load| function takes one argument:
  //@[
  //  load filename
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

//   ArrayVector SendFunction(int nargout, const ArrayVector& arg) {
//     if (arg.size() != 2)
//       throw Exception("send requires two arguments - the socket handle and the array to send");
//     Array tmp(arg[0]);
//     int sockHandle = tmp.getContentsAsIntegerScalar();
//     SockPtr *p = sockets.lookupHandle(sockHandle);
//     p->ser->putArray(arg[1]);
//     return ArrayVector();
//   }

//   ArrayVector ReceiveFunction(int nargout, const ArrayVector& arg) {
//     if (arg.size() != 1)
//       throw Exception("receive requires one argument - the socket handle to receive from");
//     Array tmp(arg[0]);
//     int sockHandle = tmp.getContentsAsIntegerScalar();
//     SockPtr *p = sockets.lookupHandle(sockHandle);
//     ArrayVector retval;
//     Array arr;
//     p->ser->getArray(arr);
//     retval.push_back(arr);
//     return retval;
//   }

//   ArrayVector ConnectFunction(int nargout, const ArrayVector& arg) {
//     if (arg.size() != 2)
//       throw Exception("connect requires two arguments, the machine name and the portnumber");
//     char *hostname;
//     Array tmp(arg[0]);
//     hostname = tmp.getContentsAsCString();
//     Array tmp2(arg[1]);
//     int portNum;
//     portNum = tmp2.getContentsAsIntegerScalar();
//     Socket *sock = new Socket(hostname, portNum);
//     Serialize *t = new Serialize(sock);
//     t->handshakeClient();
//     SockPtr *c = new SockPtr;
//     c->sock = sock;
//     c->ser = t;
//     unsigned int rethan = sockets.assignHandle(c);
//     ArrayVector retval;
//     retval.push_back(Array::uint32Constructor(rethan));
//     return retval;    
//   }

//   ArrayVector AcceptFunction(int nargout, const ArrayVector& arg) {
//     int servHandle;
//     ServerSocket *t;
//     if (arg.size() == 0)
//       throw Exception("must supply a server handle to acceptconnection");
//     Array tmp(arg[0]);
//     servHandle = tmp.getContentsAsIntegerScalar();
//     t = servers.lookupHandle(servHandle);
//     Socket *s = t->AcceptConnection();
//     Serialize *z = new Serialize(s);
//     z->handshakeServer();
//     SockPtr *c = new SockPtr;
//     c->sock = s;
//     c->ser = z;
//     unsigned int rethan = sockets.assignHandle(c);
//     ArrayVector retval;
//     retval.push_back(Array::uint32Constructor(rethan));
//     return retval;
//   }

//   ArrayVector ServerFunction(int nargout, const ArrayVector& arg) {
//     int portNumber;
//     // Retrieve the portnumber
//     portNumber = 0;
//     if (arg.size() == 1) {
//       Array tmp(arg[0]);
//       portNumber = tmp.getContentsAsIntegerScalar();
//     }
//     // Get a new socket server
//     ServerSocket *t = new ServerSocket(portNumber);
//     // Assign a handle
//     unsigned int rethan = servers.assignHandle(t);
//     // Query it to find out what portnumber it got...
//     unsigned int outPortNum;
//     outPortNum = t->getPortNumber();
//     // Return the two values...
//     ArrayVector retval;
//     retval.push_back(Array::uint32Constructor(rethan));
//     retval.push_back(Array::uint32Constructor(outPortNum));
//     return retval;
//   }
}
