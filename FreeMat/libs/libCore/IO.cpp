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
#include "Socket.hpp"
#include "ServerSocket.hpp"

namespace FreeMat {
  class FilePtr {
  public:
    FILE *fp;
    bool swapflag;
  };
  
  class SockPtr {
  public:
    Socket *sock;
    Serialize *ser;
  };

  HandleList<FilePtr*> fileHandles;
  HandleList<ServerSocket*> servers;
  HandleList<SockPtr*> sockets;

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

  ArrayVector SetPrintLimitFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    if (arg.size() != 1)
      throw Exception("setprintlimit requires one, scalar integer argument");
    Array tmp(arg[0]);
    eval->setPrintLimit(tmp.getContentsAsIntegerScalar());
    return ArrayVector();
  }

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
      FilePtr* fptr(fileHandles.lookupHandle(handle+1));
      if (fclose(fptr->fp))
	throw Exception(strerror(errno));
      fileHandles.deleteHandle(handle+1);
      delete fptr;
    }
    return ArrayVector();
  }

  ArrayVector FreadFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 3)
      throw Exception("fread requires three arguments, the file handle, size, and precision");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
    if (!arg[2].isString())
      throw Exception("second argument to fread must be a precision");
    char *prec = arg[2].getContentsAsCString();
    // Get the size argument
    Array sze(arg[1]);
    // Promote sze to a float argument
    sze.promoteType(FM_FLOAT);
    // Check for a single infinity
    int dimCount(sze.getLength());
    float *dp((float *) sze.getReadWriteDataPointer());
    bool infinityFound = false;
    int elementCount = 1;
    int infiniteDim = 0;
    for (int i=0;i<dimCount;i++) {
      if (isnan(dp[i])) throw Exception("nan not allowed in size argument");
      if (isinf(dp[i])) {
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
    ArrayVector retval;
    retval.push_back(Array::Array(dataClass,dims,qp));
    return retval;
  }

  ArrayVector FwriteFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
      throw Exception("fwrite requires two arguments, the file handle, and the variable to be written");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
    if (arg[1].isReferenceType())
      throw Exception("cannot write reference data types with fwrite");
    Array toWrite(arg[1]);
    unsigned int written;
    unsigned int count(toWrite.getLength());
    unsigned int elsize(toWrite.getElementSize());
    if (!fptr->swapflag || (elsize == 1)) {
      const void *dp(toWrite.getDataPointer());
      written = fwrite(dp,elsize,count,fptr->fp);      
    } else {
      void *dp(toWrite.getReadWriteDataPointer());
      SwapBuffer((char*) dp, count, elsize);
      written = fwrite(dp,elsize,count,fptr->fp);
    }
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(written));
    return retval;    
  }

  ArrayVector FtellFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("ftell requires an argument, the file handle.");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
    unsigned int fpos;
    fpos = ftell(fptr->fp);
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(fpos));
    return retval;
  }

  ArrayVector FeofFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("feof requires an argument, the file handle.");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
    int ateof;
    ateof = feof(fptr->fp);
    ArrayVector retval;
    retval.push_back(Array::logicalConstructor(ateof));
    return retval;
  }
  
  ArrayVector FseekFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 3)
      throw Exception("fseek requires three arguments, the file handle, the offset, and style");
    Array tmp1(arg[0]);
    int handle = tmp1.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
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

  ArrayVector PrintfFunction(int nargout, const ArrayVector& arg) {
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
    // Scan the string
    while (*dp) {
      np = dp;
      while ((*dp) && (*dp != '%')) dp++;
      // Print out the formatless segment
      sv = *dp;
      *dp = 0;
      printf(np);
      *dp = sv;
      // Process the format spec
      if (*dp) {
	np = validateFormatSpec(dp+1);
	if (!np)
	  throw Exception("erroneous format specification " + std::string(dp));
	else {
	  if (*(np-1) == '%') {
	    printf("%%");
	    dp+=2;
	  } else {
	    sv = *np;
	    *np = 0;
	    if (arg.size() <= nextArg)
	      throw Exception("not enough arguments to satisfy format specification");
	    Array nextVal(arg[nextArg++]);
	    switch (*(np-1)) {
	    case 'd':
	    case 'i':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
	    case 'c':
	      nextVal.promoteType(FM_INT32);
	      printf(dp,*((int32*)nextVal.getDataPointer()));
	      break;
	    case 'e':
	    case 'E':
	    case 'f':
	    case 'F':
	    case 'g':
	    case 'G':
	      nextVal.promoteType(FM_DOUBLE);
	      printf(dp,*((double*)nextVal.getDataPointer()));
	      break;
	    case 's':
	      printf(dp,nextVal.getContentsAsCString());
	    }
	    *np = sv;
	    dp = np;
	  }
	}
      }
    }
    fflush(stdout);
    return ArrayVector();
  }

  ArrayVector FgetlineFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("fgetline takes one argume, the file handle");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
    char buffer[4096];
    fgets(buffer,sizeof(buffer),fptr->fp);
    Array retval(Array::stringConstructor(buffer));
    ArrayVector retvals;
    retvals.push_back(retval);
    return retvals;
  }

  ArrayVector FscanfFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("fscanf takes two arguments, the file handle and the format string");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
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

  ArrayVector FprintfFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() < 2)
      throw Exception("fprintf requires at least two arguments, the file handle and theformat string");
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    FilePtr *fptr(fileHandles.lookupHandle(handle+1));
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
	    *np = sv;
	    dp = np;
	  }
	}
      }
    }
    return ArrayVector();
  }

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

  ArrayVector SendFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("send requires two arguments - the socket handle and the array to send");
    Array tmp(arg[0]);
    int sockHandle = tmp.getContentsAsIntegerScalar();
    SockPtr *p = sockets.lookupHandle(sockHandle);
    p->ser->putArray(arg[1]);
    return ArrayVector();
  }

  ArrayVector ReceiveFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("receive requires one argument - the socket handle to receive from");
    Array tmp(arg[0]);
    int sockHandle = tmp.getContentsAsIntegerScalar();
    SockPtr *p = sockets.lookupHandle(sockHandle);
    ArrayVector retval;
    Array arr;
    p->ser->getArray(arr);
    retval.push_back(arr);
    return retval;
  }

  ArrayVector ConnectFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 2)
      throw Exception("connect requires two arguments, the machine name and the portnumber");
    char *hostname;
    Array tmp(arg[0]);
    hostname = tmp.getContentsAsCString();
    Array tmp2(arg[1]);
    int portNum;
    portNum = tmp2.getContentsAsIntegerScalar();
    Socket *sock = new Socket(hostname, portNum);
    Serialize *t = new Serialize(sock);
    t->handshakeClient();
    SockPtr *c = new SockPtr;
    c->sock = sock;
    c->ser = t;
    unsigned int rethan = sockets.assignHandle(c);
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(rethan));
    return retval;    
  }

  ArrayVector AcceptFunction(int nargout, const ArrayVector& arg) {
    int servHandle;
    ServerSocket *t;
    if (arg.size() == 0)
      throw Exception("must supply a server handle to acceptconnection");
    Array tmp(arg[0]);
    servHandle = tmp.getContentsAsIntegerScalar();
    t = servers.lookupHandle(servHandle);
    Socket *s = t->AcceptConnection();
    Serialize *z = new Serialize(s);
    z->handshakeServer();
    SockPtr *c = new SockPtr;
    c->sock = s;
    c->ser = z;
    unsigned int rethan = sockets.assignHandle(c);
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(rethan));
    return retval;
  }

  ArrayVector ServerFunction(int nargout, const ArrayVector& arg) {
    int portNumber;
    // Retrieve the portnumber
    portNumber = 0;
    if (arg.size() == 1) {
      Array tmp(arg[0]);
      portNumber = tmp.getContentsAsIntegerScalar();
    }
    // Get a new socket server
    ServerSocket *t = new ServerSocket(portNumber);
    // Assign a handle
    unsigned int rethan = servers.assignHandle(t);
    // Query it to find out what portnumber it got...
    unsigned int outPortNum;
    outPortNum = t->getPortNumber();
    // Return the two values...
    ArrayVector retval;
    retval.push_back(Array::uint32Constructor(rethan));
    retval.push_back(Array::uint32Constructor(outPortNum));
    return retval;
  }
}
