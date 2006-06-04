#include "Print.hpp"
#include "IEEEFP.hpp"
#include "FunctionDef.hpp"
#include <math.h>

namespace FreeMat {

#define MSGBUFLEN 2048
  static char msgBuffer[MSGBUFLEN];

  class ArrayFormat {
  public:
    int digits;
    int decimals;
    bool expformat;
    int width;
    ArrayFormat(int dig, int dec, bool eform, int wid) : 
      digits(dig), decimals(dec), expformat(eform), width(wid) {
    };
    ArrayFormat() {
    };
  };
  
  template <class T>
  int GetNominalWidthInteger(const T*array, int count) {
    char buffer[100];
    
    int maxdigit = 0;
    for (int i=0;i<count;i++) {
      memset(buffer,0,100);
      sprintf(buffer,"%d",array[i]);
      int j = maxdigit;
      while (buffer[j] && j>=maxdigit)
	j++;
      maxdigit = j;
    }
    return maxdigit;
  }
  
  template <class T>
  void PrintIntegers(const T*array, int count, int width) {
    for (int i=0;i<count;i++)
      printf("%*d\n",width,array[i]);
  }
  
  template <class T>
  int GetNominalWidthFloat(const T*array, int count, int maxdigits_try, int &leading) {
    char buffer[100];
  
    int maxdigit = 0;
    leading = 0;
    for (int i=0;i<count;i++) {
      // try different numbers of digits
      int j = 0;
      bool match = false;
      int dig = 0;
      if (IsNaN(array[i])) {
	match = true;
	j = 0;
	dig = 3;
      }
      while ((j < maxdigits_try)  && (!match)) {
	memset(buffer,0,100);
	sprintf(buffer,"%0.*f",j,array[i]);
	match = (((T) atof(buffer)) == array[i]);
	if (!match) j++;
      }
      if (!match) return -1;
      maxdigit = std::max(maxdigit,j);
      while (buffer[dig] && buffer[dig] != '.') dig++;
      leading = std::max(leading,dig);
    }
    return maxdigit;
  }

  template <class T>
  int GetNominalWidthFloatE(const T*array, int count, int maxdigits_try) {
    char buffer[100];
  
    int maxdigit = 0;
    for (int i=0;i<count;i++) {
      // try different numbers of digits
      int j = 0;
      bool match = false;
      while ((j < maxdigits_try)  && (!match)) {
	memset(buffer,0,100);
	sprintf(buffer,"%0.*e",j,array[i]);
	match = (((T) atof(buffer)) == array[i]);
	if (!match) j++;
      }
      if (!match) return -1;
      maxdigit = std::max(maxdigit,j);
    }
    return maxdigit;
  }
  
  void emitFormattedElement(Interface* io, char *msgBuffer, ArrayFormat format, const void *dp, int num, Class dcls) {
    switch (dcls) {
    case FM_INT8: {
      const int8 *ap;
      ap = (const int8*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%*d",format.digits,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_UINT8: {
      const uint8 *ap;
      ap = (const uint8*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%*u",format.digits,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_INT16: {
      const int16 *ap;
      ap = (const int16*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%*d",format.digits,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_UINT16: {
      const uint16 *ap;
      ap = (const uint16*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%*u",format.digits,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_INT32: {
      const int32 *ap;
      ap = (const int32*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%*d",format.digits,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_UINT32: {
      const uint32 *ap;
      ap = (const uint32*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%*u",format.digits,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_LOGICAL: {
      const logical *ap;
      ap = (const logical*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%*u",format.digits,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_STRING: {
      const char *ap;
      ap = (const char*) dp;
      snprintf(msgBuffer,MSGBUFLEN,"%c\0",ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_FLOAT: {
      const float *ap;
      ap = (const float*) dp;
      if (!format.expformat)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f",format.width,format.decimals,ap[num]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e",format.decimals,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_DOUBLE: {
      const double *ap;
      ap = (const double*) dp;
      if (!format.expformat)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f",format.width,format.decimals,ap[num]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e",format.decimals,ap[num]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_COMPLEX: {
      const float *ap;
      ap = (const float*) dp;
      if (!format.expformat)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f + %*.*f i",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e + %0.*e i",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_DCOMPLEX: {
      const double *ap;
      ap = (const double*) dp;
      if (!format.expformat)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f+%*.*fi",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e+%0.*ei",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      io->outputMessage(msgBuffer);
      break;
    }
    case FM_CELL_ARRAY: {
      Array *ap;
      ap = (Array*) dp;
      if (ap == NULL)
	io->outputMessage("[]");
      else
	ap[num].summarizeCellEntry();
      io->outputMessage("  ");
      break;
    }
    case FM_FUNCPTR_ARRAY: {
      const FunctionDef** ap;
      ap = (const FunctionDef**) dp;
      if (!ap[num]) {
	io->outputMessage("[]  ");
      } else {
	io->outputMessage("@");
	io->outputMessage(ap[num]->name.c_str());
	snprintf(msgBuffer,MSGBUFLEN,"  ");
	io->outputMessage(msgBuffer);
      }
    }
    }
  }

  // Print the class type and size information for the array
  void PrintArrayClassAndSize(Array A, Interface* io) {
    switch(A.getDataClass()) {
    case FM_UINT8:
      io->outputMessage("  <uint8>  ");
      break;
    case FM_INT8:
      io->outputMessage("  <int8>  ");
      break;
    case FM_UINT16:
      io->outputMessage("  <uint16>  ");
      break;
    case FM_INT16:
      io->outputMessage("  <int16>  ");
      break;
    case FM_UINT32:
      io->outputMessage("  <uint32>  ");
      break;
    case FM_INT32:
      io->outputMessage("  <int32>  ");
      break;
    case FM_FLOAT:
      io->outputMessage("  <float>  ");
      break;
    case FM_DOUBLE:
      io->outputMessage("  <double>  ");
      break;
    case FM_LOGICAL:
      io->outputMessage("  <logical>  ");
      break;
    case FM_STRING:
      io->outputMessage("  <string>  ");
      break;
    case FM_COMPLEX:
      io->outputMessage("  <complex>  ");
      break;
    case FM_DCOMPLEX:
      io->outputMessage("  <dcomplex>  ");
      break;
    case FM_CELL_ARRAY:
      io->outputMessage("  <cell array> ");
      break;
    case FM_STRUCT_ARRAY:
      if (A.isUserClass()) {
	io->outputMessage(" ");
	io->outputMessage(A.getClassName().back().c_str());
	io->outputMessage(" object");
      } else
	io->outputMessage("  <structure array> ");
      break;
    case FM_FUNCPTR_ARRAY:
      io->outputMessage("  <function ptr array>  ");
    }
    io->outputMessage("- size: ");
    A.getDimensions().printMe(io);
    io->outputMessage("\n");
  }

  template <class T>
  ArrayFormat ScanFormatArrayInteger(const T* data, int count) {
    ArrayFormat ret;
    ret.digits = GetNominalWidthInteger<T>(data,count);
    ret.decimals = 0;
    ret.expformat = false;
    ret.width = ret.digits;
    return ret;
  }

  template <class T>
  ArrayFormat ScanFormatArrayFloat(const T* data, int count) {
    ArrayFormat ret;
    int leading;
    int decimals = GetNominalWidthFloat<T>(data,count,20,leading);
    if (decimals < 0) {
      decimals = GetNominalWidthFloatE<T>(data,count,20);
      ret.digits = 1;
      ret.decimals = decimals;
      ret.expformat = true;
      if (decimals)
	ret.width = ret.digits+decimals+1;
      else
	ret.width = ret.digits;
    } else {
      ret.digits = leading;
      ret.decimals = decimals;
      ret.expformat = false;
      if (decimals>0)
	ret.width = ret.digits+decimals+1;
      else
	ret.width = ret.digits;
    }
    return ret;
  }

  // Scan a numeric array to figure out how to best display it 
  // the result is encoded in an ArrayFormat object
  ArrayFormat ScanFormatArray(const void *data, Class aclass, int count) {
    switch(aclass) {
    case FM_UINT8:
      return ScanFormatArrayInteger<uint8>((const uint8*) data,count);
    case FM_INT8:
      return ScanFormatArrayInteger<int8>((const int8*) data,count);
    case FM_UINT16:
      return ScanFormatArrayInteger<uint16>((const uint16*) data,count);
    case FM_INT16:
      return ScanFormatArrayInteger<int16>((const int16*) data,count);
    case FM_UINT32:
      return ScanFormatArrayInteger<uint32>((const uint32*) data,count);
    case FM_INT32:
      return ScanFormatArrayInteger<int32>((const int32*) data,count);
    case FM_LOGICAL:
      return ScanFormatArrayInteger<logical>((const logical*) data,count);
    case FM_FLOAT:
      return ScanFormatArrayFloat<float>((const float*) data,count);
    case FM_DOUBLE:
      return ScanFormatArrayFloat<double>((const double*) data,count);
    case FM_STRING:
      return ArrayFormat(1,0,false,1);
    case FM_COMPLEX: {
      ArrayFormat af(ScanFormatArrayFloat<float>((const float*) data,count*2));
      af.width = af.width*2+2;
      return af;
    }
    case FM_DCOMPLEX: {
      ArrayFormat af(ScanFormatArrayFloat<double>((const double*) data,count*2));
      af.width = af.width*2+2;
      return af;
    }      
    case FM_CELL_ARRAY:
    case FM_STRUCT_ARRAY:
    case FM_FUNCPTR_ARRAY:
      return ArrayFormat(20,0,false,20);
    }
      return ArrayFormat(20,0,false,20);
  }

  void PrintSheet(ArrayFormat format, Interface*io, int rows, int columns, 
		  int offset, const void* data, Class aclass, int termWidth) {
    // Determine how many columns will fit across
    // the terminal width
    int colsPerPage;
    if (aclass != FM_STRING)
      colsPerPage = (int) floor((termWidth-1)/((float) format.width + 3));
    else
      colsPerPage = termWidth;
    colsPerPage = (colsPerPage < 1) ? 1 : colsPerPage;
    int pageCount;
    pageCount = (int) ceil(columns/((float)colsPerPage));
    for (int k=0;k<pageCount;k++) {
      int colsInThisPage;
      colsInThisPage = columns - colsPerPage*k;
      colsInThisPage = (colsInThisPage > colsPerPage) ? 
	colsPerPage : colsInThisPage;
      if (rows*columns > 1 && aclass != FM_STRING) {
	snprintf(msgBuffer,MSGBUFLEN," \nColumns %d to %d\n",
		 k*colsPerPage+1,k*colsPerPage+colsInThisPage);
	io->outputMessage(msgBuffer);
      }
      memset(msgBuffer,0,MSGBUFLEN);
      for (int i=0;i<rows;i++) {
	snprintf(msgBuffer,MSGBUFLEN," ");
	io->outputMessage(msgBuffer);
	memset(msgBuffer,0,MSGBUFLEN);
	for (int j=0;j<colsInThisPage;j++) {
	  emitFormattedElement(io,msgBuffer,
			       format,
			       data,
			       i+(k*colsPerPage+j)*rows+offset,
			       aclass);
	  if (aclass != FM_STRING)
	    io->outputMessage("  ");
	}
	snprintf(msgBuffer,MSGBUFLEN,"\n");
	io->outputMessage(msgBuffer);
	memset(msgBuffer,0,MSGBUFLEN);
      }
    }
  }

  // Helper function - print an array using "classic" notation
  void PrintArrayClassic(Array A, int printLimit, Interface* io,  
			 bool showClassSize) {
    int termWidth = io->getTerminalWidth();
    if (showClassSize)
      PrintArrayClassAndSize(A,io);
    Class Aclass(A.getDataClass());
    Dimensions Adims(A.getDimensions());
    if (A.isUserClass())
      return;
    if (A.isEmpty()) {
      io->outputMessage("  []\n");
      return;
    }
    if (A.isSparse()) {
      sprintf(msgBuffer,"\tMatrix is sparse with %d nonzeros\n",
	      A.getNonzeros());
      io->outputMessage(msgBuffer);
      return;
    }
    if (Aclass == FM_STRUCT_ARRAY) {
      if (Adims.isScalar()) {
	Array *ap;
	ap = (Array *) A.getDataPointer();
	for (int n=0;n<A.getFieldNames().size();n++) {
	  io->outputMessage("    ");
	  io->outputMessage(A.getFieldNames()[n].c_str());
	  io->outputMessage(": ");
	  ap[n].summarizeCellEntry();
	  io->outputMessage("\n");
	}
      } else {
	io->outputMessage("  Fields\n");
	for (int n=0;n<A.getFieldNames().size();n++) {
	  io->outputMessage("    ");
	  io->outputMessage(A.getFieldNames()[n].c_str());
	  io->outputMessage("\n");
	}
      }
    } else {
      ArrayFormat format(ScanFormatArray(A.getDataPointer(),Aclass,A.getLength()));
      if (Adims.getLength() == 2) {
	int rows = Adims.getRows();
	int columns = Adims.getColumns();
	PrintSheet(format,io,rows,columns,0,A.getDataPointer(),Aclass,termWidth);
      } else if (Adims.getLength() > 2) {
	/**
	 * For N-ary arrays, data slice  -  start with 
	 * [1,1,1,...,1].  We keep doing the matrix
	 * print , incrementing from the highest dimension,
	 * and rolling downwards.
	 */
	Dimensions wdims(Adims.getLength());
	int rows(Adims.getRows());
	int columns(Adims.getColumns());
	int offset = 0;
	while (wdims.inside(Adims)) {
	  snprintf(msgBuffer,MSGBUFLEN,"(:,:");
	  io->outputMessage(msgBuffer);
	  for (int m=2;m<Adims.getLength();m++) {
	    snprintf(msgBuffer,MSGBUFLEN,",%d",wdims[m]+1);
	    io->outputMessage(msgBuffer);
	  }
	  snprintf(msgBuffer,MSGBUFLEN,") = \n");
	  io->outputMessage(msgBuffer);
	  PrintSheet(format,io,rows,columns,offset,A.getDataPointer(),Aclass,termWidth);
	  offset += rows*columns;
	  wdims.incrementModulo(Adims,2);
	}
      }
    }
  }
}
