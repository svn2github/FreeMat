#include "Print.hpp"
#include "IEEEFP.hpp"
#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include <math.h>
#include <string.h>

#define MSGBUFLEN 2048

FMFormatMode formatMode = format_short;

void SetPrintFormatMode(FMFormatMode m) {
  formatMode = m;
}

FMFormatMode GetPrintFormatMode() {
  return formatMode;
}

class ArrayFormatInfo {
public:
  int width;
  bool floatasint;
  int decimals;
  bool expformat;
  double scalefact;
  ArrayFormatInfo(int w = 0, bool f = false, int d = 0, bool e = false, double s = 1.0) : 
    width(w), floatasint(f), decimals(d), expformat(e), scalefact(s) {}
};

/**
 * Summary of this object when it is an element of a cell array.  This is
 * generally a shorthand summary of the description of the object.
 */
template <class T>
string NumericCellEntry(const T* data, Dimensions dims, string name, 
			const char *formatcode, bool isSparse = false) {
  char msgBuffer[MSGBUFLEN];
  if (dims.isScalar()) {
    snprintf(msgBuffer,MSGBUFLEN,formatcode,data[0]);
    return msgBuffer;
  } else {
    string ret = "[" + dims.asString() + " ";
    if (isSparse)
      ret += " sparse";
    ret += name + "]";
    return ret;
  }
}

template <class T>
string ComplexNumericCellEntry(const T* data, Dimensions dims, string name, 
			       const char *formatcode, bool isSparse = false) {
  char msgBuffer[MSGBUFLEN];
  if (dims.isScalar()) {
    snprintf(msgBuffer,MSGBUFLEN,formatcode,data[0],data[1]);
    return msgBuffer;
  } else {
    string ret = "[" + dims.asString() + " ";
    if (isSparse)
      ret += " sparse";
    ret += name + "]";
    return ret;
  }
}

string SummarizeArrayCellEntry(Array dp)  {
  if (dp.isEmpty()) 
    return ("[]");
  else {
    switch(dp.dataClass()) {
    case FM_FUNCPTR_ARRAY:
      return "{" + dp.dimensions().asString() + " function pointer array }";
    case FM_CELL_ARRAY:
      return "{" + dp.dimensions().asString() + " cell }";
    case FM_STRUCT_ARRAY: 
      {
	string ret;
	ret = "[" + dp.dimensions().asString();
	if (dp.isUserClass()) {
	  ret += " " + dp.className().back() + " object";
	} else 
	  ret += " struct array";
	return ret + "]";
      }
    case FM_STRING:
      if (dp.dimensions().getRows() == 1)
	return "['" + dp.getContentsAsString() + "']";
      else
	return "[" + dp.dimensions().asString() + " string ]";
    case FM_LOGICAL:
      return NumericCellEntry((const logical *)dp.getDataPointer(),dp.dimensions(),"logical","[%u]");
    case FM_UINT8:
      return NumericCellEntry((const uint8 *)dp.getDataPointer(),dp.dimensions(),"uint8","[%u]");
    case FM_INT8:
      return NumericCellEntry((const uint8 *)dp.getDataPointer(),dp.dimensions(),"int8","[%d]");
    case FM_UINT16:
      return NumericCellEntry((const uint16 *)dp.getDataPointer(),dp.dimensions(),"uint16","[%u]");
    case FM_INT16:
      return NumericCellEntry((const uint16 *)dp.getDataPointer(),dp.dimensions(),"int16","[%d]");
    case FM_UINT32:
      return NumericCellEntry((const uint32 *)dp.getDataPointer(),dp.dimensions(),"uint32","[%u]");
    case FM_INT32:
      return NumericCellEntry((const uint32 *)dp.getDataPointer(),dp.dimensions(),"int32","[%d]");
    case FM_UINT64:
      return NumericCellEntry((const uint64 *)dp.getDataPointer(),dp.dimensions(),"uint64","[%llu]");
    case FM_INT64:
      return NumericCellEntry((const uint64 *)dp.getDataPointer(),dp.dimensions(),"int64","[%lld]");
    case FM_DOUBLE:
      return NumericCellEntry((const double *)dp.getDataPointer(),dp.dimensions(),"double","[%lg]",dp.sparse());
    case FM_FLOAT:
      return NumericCellEntry((const float *)dp.getDataPointer(),dp.dimensions(),"float","[%g]",dp.sparse());
    case FM_COMPLEX:
      return ComplexNumericCellEntry((const float *)dp.getDataPointer(),dp.dimensions(),
				     "complex","[%g+%gi]",dp.sparse());
    case FM_DCOMPLEX:
      return ComplexNumericCellEntry((const float *)dp.getDataPointer(),dp.dimensions(),
				     "complex","[%lg+%lgi]",dp.sparse());
    }
  }
  return string("?");
}
 
template <class T>
bool AllIntegerValues(T *t, int len) {
  if (len == 0) return false;
  bool allInts = true;
  for (int i=0;(i<len) && allInts;i++) {
    allInts = (t[i] == ((T) ((int64) t[i])));
  }
  return allInts;
}

template <class T>
int GetNominalWidthSignedInteger(const T*array, int count) {
  char buffer[MSGBUFLEN];
    
  int maxdigit = 0;
  for (int i=0;i<count;i++) {
    memset(buffer,0,MSGBUFLEN);
    sprintf(buffer,"%lld",(int64)array[i]);
    int j = maxdigit;
    while (buffer[j] && j>=maxdigit)
      j++;
    maxdigit = j;
  }
  return maxdigit;  
}

template <class T>
int GetNominalWidthUnsignedInteger(const T*array, int count) {
  char buffer[MSGBUFLEN];
    
  int maxdigit = 0;
  for (int i=0;i<count;i++) {
    memset(buffer,0,MSGBUFLEN);
    sprintf(buffer,"%llu",(uint64)array[i]);
    int j = maxdigit;
    while (buffer[j] && j>=maxdigit)
      j++;
    maxdigit = j;
  }
  return maxdigit;  
}
 
template <class T>
void ComputeScaleFactor(const T* array, int count, ArrayFormatInfo& format) {
  T max_amplitude = 0;
  if (count == 0) return;
  if (format.expformat) return;
  bool finiteElementFound = false;
  for (int i=0;i<count;i++) {
    if (IsFinite(array[i]) && !finiteElementFound) {
      max_amplitude = array[i];
      finiteElementFound = true;
    }
    if ((IsFinite(array[i])) && 
	(fabs((double) array[i]) > fabs((double) max_amplitude)))
      max_amplitude = array[i];
  }
  if (!finiteElementFound) return;
  if (max_amplitude >= 100)
    format.scalefact = pow(10.0,floor(log10(max_amplitude)));
  else if (max_amplitude <= -100)
    format.scalefact = pow(10.0,floor(log10(-max_amplitude)));
  else if ((max_amplitude <= .1) && (max_amplitude>0))
    format.scalefact = pow(10.0,floor(log10(max_amplitude)));
  else if ((max_amplitude >= -.1) && (max_amplitude<0))
    format.scalefact = pow(10.0,floor(log10(-max_amplitude)));
  else
    format.scalefact = 1.0;
}

ArrayFormatInfo ComputeArrayFormatInfo(const void *dp, int length, Class aclass) {
  switch (aclass) {
  default: throw Exception("unexpected class for ComputeArrayFormatInfo");    
  case FM_INT8:    
    return ArrayFormatInfo(GetNominalWidthSignedInteger((const int8*)dp,length));
  case FM_UINT8:   
    return ArrayFormatInfo(GetNominalWidthUnsignedInteger((const uint8*)dp,length));
  case FM_INT16:   
    return ArrayFormatInfo(GetNominalWidthSignedInteger((const int16*)dp,length));
  case FM_UINT16:  
    return ArrayFormatInfo(GetNominalWidthUnsignedInteger((const uint16*)dp,length));
  case FM_INT32:   
    return ArrayFormatInfo(GetNominalWidthSignedInteger((const int32*)dp,length));
  case FM_UINT32:  
    return ArrayFormatInfo(GetNominalWidthUnsignedInteger((const uint32*)dp,length));
  case FM_INT64:   
    return ArrayFormatInfo(GetNominalWidthSignedInteger((const int64*)dp,length));
  case FM_UINT64:  
    return ArrayFormatInfo(GetNominalWidthUnsignedInteger((const uint64*)dp,length));
  case FM_LOGICAL: 
    return ArrayFormatInfo(1);
  case FM_STRING:  
    return ArrayFormatInfo(1);
  case FM_FLOAT:   
    if (AllIntegerValues((const float*)dp,length)) 
      return ArrayFormatInfo(GetNominalWidthSignedInteger((const float*)dp,length),
			     true);
    else {
      ArrayFormatInfo ret;
      if (formatMode == format_short) 
 	ret = ArrayFormatInfo(9,false,4);
      else if (formatMode == format_long)
 	ret = ArrayFormatInfo(11,false,7);
      else if (formatMode == format_short_e)
 	ret = ArrayFormatInfo(11,false,4,true);
      else if (formatMode == format_long_e)
 	ret = ArrayFormatInfo(14,false,7,true);
      ComputeScaleFactor((const float*) dp,length,ret);
      return ret;
    }
  case FM_DOUBLE:  
    if (AllIntegerValues((const double*)dp,length)) 
      return ArrayFormatInfo(GetNominalWidthSignedInteger((const double*)dp,length),
  			     true);
    else {
      ArrayFormatInfo ret;
      if (formatMode == format_short)
	ret = ArrayFormatInfo(9,false,4);
      else if (formatMode == format_long)
	ret = ArrayFormatInfo(18,false,14);
      else if (formatMode == format_short_e)
	ret = ArrayFormatInfo(11,false,4,true);
      else if (formatMode == format_long_e)
	ret = ArrayFormatInfo(21,false,14,true);
      ComputeScaleFactor((const double*) dp, length, ret);
      return ret;
    }
  case FM_COMPLEX:
    {
      ArrayFormatInfo ret;
      if (formatMode == format_short)
 	ret = ArrayFormatInfo(19,false,4);
      else if (formatMode == format_long)
 	ret = ArrayFormatInfo(23,false,7);
      else if (formatMode == format_short_e)
 	ret = ArrayFormatInfo(19,false,4,true);
      else if (formatMode == format_long_e)
 	ret = ArrayFormatInfo(23,false,7,true);
      ComputeScaleFactor((const float*) dp,length*2,ret);
      return ret;
    }
  case FM_DCOMPLEX:  
    {
      ArrayFormatInfo ret;
      if (formatMode == format_short)
 	ret = ArrayFormatInfo(19,false,4);
      else if (formatMode == format_long)
 	ret = ArrayFormatInfo(37,false,14);
      else if (formatMode == format_short_e)
 	ret = ArrayFormatInfo(19,false,4,true);
      else if (formatMode == format_long_e)
 	ret = ArrayFormatInfo(37,false,14,true);
      ComputeScaleFactor((const double*) dp,length*2,ret);
      return ret;
    }
  case FM_CELL_ARRAY: 
    {
      int maxwidth = 1;
      const Array *ap = (const Array *) dp;
      for (int i=0;i<length;i++) {
	int len = SummarizeArrayCellEntry(ap[i]).size();
	maxwidth = max(maxwidth,len);
      }
      return ArrayFormatInfo(maxwidth);
    }
  case FM_FUNCPTR_ARRAY:
    return ArrayFormatInfo(20);
  }  
}

template <class T>
void emitSignedInteger(Interpreter* io, T val, const ArrayFormatInfo &format) {
  io->outputMessage("%*lld",format.width,(int64)val);
}

template <class T>
void emitUnsignedInteger(Interpreter* io, T val, const ArrayFormatInfo &format) {
  io->outputMessage("%*llu",format.width,(uint64)val);
}

template <class T>
void emitFloat(Interpreter*io, T val, const ArrayFormatInfo &format) {
  if (val != 0) 
    if (format.expformat)
      io->outputMessage("%*.*e",format.width,format.decimals,val);
    else
      io->outputMessage("%*.*f",format.width,format.decimals,val/format.scalefact);
  else
    io->outputMessage("%*d",format.width,0);
}

template <class T>
void emitComplex(Interpreter* io, T real, T imag, const ArrayFormatInfo &format) {
  int width = format.width/2;
  if ((real != 0) || (imag != 0)) {
    if (format.expformat)
      io->outputMessage("%*.*e",width,format.decimals,real/format.scalefact);
    else
      io->outputMessage("%*.*f",width,format.decimals,real/format.scalefact);
    if (imag < 0) {
      if (format.expformat)
	io->outputMessage(" -%*.*ei",width-1,format.decimals,-imag/format.scalefact);
      else
	io->outputMessage(" -%*.*fi",width-1,format.decimals,-imag/format.scalefact);
    } else {
      if (format.expformat)
	io->outputMessage(" +%*.*ei",width-1,format.decimals,imag/format.scalefact);
      else
	io->outputMessage(" +%*.*fi",width-1,format.decimals,imag/format.scalefact);
    }
  } else 
    io->outputMessage("%*d%*c",width,0,width+2,' ');
}

void emitFormattedElement(Interpreter* io, const void *dp, 
			  const ArrayFormatInfo &format, int num, Class dcls) {
  switch (dcls) {
  default: throw Exception("unexpected class for emitFormattedElement");
  case FM_INT8:   
    emitSignedInteger(io,((const int8*) dp)[num],format);
    return;
  case FM_INT16:  
    emitSignedInteger(io,((const int16*) dp)[num],format);
    return;
  case FM_INT32:  
    emitSignedInteger(io,((const int32*) dp)[num],format);
    return;
  case FM_INT64:  
    emitSignedInteger(io,((const int64*) dp)[num],format);
    return;
  case FM_UINT8:  
    emitUnsignedInteger(io,((const uint8*) dp)[num],format);
    return;
  case FM_UINT16: 
    emitUnsignedInteger(io,((const uint16*) dp)[num],format);
    return;
  case FM_UINT32: 
    emitUnsignedInteger(io,((const uint32*) dp)[num],format);
    return;
  case FM_UINT64: 
    emitUnsignedInteger(io,((const uint64*) dp)[num],format);
    return;
  case FM_LOGICAL:
    emitUnsignedInteger(io,((const logical*) dp)[num],format);
    return;
  case FM_FLOAT: 
    if (format.floatasint)
      emitSignedInteger(io,((const float*) dp)[num],format);
    else 
      emitFloat(io,((const float*) dp)[num],format);
    return;
  case FM_DOUBLE: 
    if (format.floatasint)
      emitSignedInteger(io,((const double*) dp)[num],format);
    else
      emitFloat(io,((const double*) dp)[num],format);
    return;
  case FM_COMPLEX: 
    emitComplex(io,((const float*) dp)[2*num],((const float*) dp)[2*num+1],format);
    return;
  case FM_DCOMPLEX: 
    emitComplex(io,((const double*) dp)[2*num],((const double*) dp)[2*num+1],format);
    return;
  case FM_STRING:
    io->outputMessage("%c\0",((const char*) dp)[num]);
    return;
  case FM_CELL_ARRAY: {
    Array *ap;
    ap = (Array*) dp;
    if (ap == NULL)
      io->outputMessage("[]");
    else
      io->outputMessage(SummarizeArrayCellEntry(ap[num]));
    io->outputMessage("  ");
    break;
  }
  case FM_FUNCPTR_ARRAY: {
    const FuncPtr* ap;
    ap = (const FuncPtr*) dp;
    if (!ap[num]) {
      io->outputMessage("[]  ");
    } else {
      io->outputMessage("@");
      io->outputMessage(ap[num]->name.c_str());
      io->outputMessage("  ");
    }
  }
  }
}

void PrintSheet(Interpreter*io, const ArrayFormatInfo &format, 
		int rows, int columns, int offset, const void* data, 
		Class aclass, int termWidth, int &printlimit) {
  if (printlimit == 0) return;
  // Determine how many columns will fit across
  // the terminal width
  int colsPerPage;
  if (aclass != FM_STRING)
    colsPerPage = (int) floor((termWidth-1)/((double) format.width + 2));
  else
    colsPerPage = (int) floor((termWidth-1)/((double) format.width));
  colsPerPage = (colsPerPage < 1) ? 1 : colsPerPage;
  int pageCount;
  pageCount = (int) ceil(columns/((double)colsPerPage));
  for (int k=0;k<pageCount;k++) {
    int colsInThisPage;
    colsInThisPage = columns - colsPerPage*k;
    colsInThisPage = (colsInThisPage > colsPerPage) ? 
      colsPerPage : colsInThisPage;
    if ((rows*columns > 1) && (pageCount > 1))
      io->outputMessage(" \nColumns %d to %d\n\n",
			k*colsPerPage+1,k*colsPerPage+colsInThisPage);
    else
      io->outputMessage("\n");
    for (int i=0;i<rows;i++) {
      io->outputMessage(" ");
      for (int j=0;j<colsInThisPage;j++) {
	emitFormattedElement(io,data,format,
			     i+(k*colsPerPage+j)*rows+offset,
			     aclass);
	printlimit--;
	if (printlimit <= 0) return;
	if (aclass != FM_STRING) io->outputMessage(" ");
      }
      io->outputMessage("\n");
    }
  }
  io->outputMessage("\n");
}

void PrintArrayClassic(Array A, int printlimit, Interpreter* io) {
  if (printlimit == 0) return;
  int termWidth = io->getTerminalWidth();
  //   bool showClassSize = true; // FINISH
  //   if (!A.isEmpty() && showClassSize)
  //     PrintArrayClassAndSize(A,io);
  Class Aclass(A.dataClass());
  Dimensions Adims(A.dimensions());
  if (A.isUserClass())
    return;
  if (A.isEmpty()) {
    if (A.dimensions().equals(zeroDim))
      io->outputMessage("  []\n");
    else {
      io->outputMessage("  Empty array ");
      A.dimensions().printMe(io);
      io->outputMessage("\n");
    }
    return;
  }
  if (A.sparse()) {
    io->outputMessage("\tMatrix is sparse with %d nonzeros\n",A.getNonzeros());
    return;
  }
  if (Aclass == FM_STRUCT_ARRAY) {
    if (Adims.isScalar()) {
      Array *ap;
      ap = (Array *) A.getDataPointer();
      for (int n=0;n<A.fieldNames().size();n++) {
	io->outputMessage("    ");
	io->outputMessage(A.fieldNames().at(n).c_str());
	io->outputMessage(": ");
	io->outputMessage(SummarizeArrayCellEntry(ap[n]));
	io->outputMessage("\n");
      }
    } else {
      io->outputMessage("  Fields\n");
      for (int n=0;n<A.fieldNames().size();n++) {
	io->outputMessage("    ");
	io->outputMessage(A.fieldNames().at(n).c_str());
	io->outputMessage("\n");
      }
    }
  } else {
    ArrayFormatInfo format(ComputeArrayFormatInfo(A.getDataPointer(),A.getLength(),Aclass));
    if (!A.isScalar() && (format.scalefact != 1))
      io->outputMessage("\n   %.1e * \n",format.scalefact);
    if (A.isScalar() && (format.scalefact != 1) && !format.floatasint)
      format.expformat = true;
    if (Adims.getLength() == 2) {
      int rows = Adims.getRows();
      int columns = Adims.getColumns(); 
      PrintSheet(io,format,rows,columns,0,
		 A.getDataPointer(),Aclass,termWidth,printlimit);
    } else if (Adims.getLength() > 2) {
      /**
       * For N-ary arrays, data slice  -  start with 
       * [1,1,1,...,1].  We keep doing the matrix
       * print , incrementing from the highest dimension,
       * and rolling downwards.
       */
      io->outputMessage("\n");
      Dimensions wdims(Adims.getLength());
      int rows(Adims.getRows());
      int columns(Adims.getColumns());
      int offset = 0;
      while (wdims.inside(Adims)) {
	io->outputMessage("(:,:");
	for (int m=2;m<Adims.getLength();m++) 
	  io->outputMessage(",%d",wdims.get(m)+1);
	io->outputMessage(") = \n");
	PrintSheet(io,format,rows,columns,offset,
		   A.getDataPointer(),Aclass,termWidth,printlimit);
	offset += rows*columns;
	wdims.incrementModulo(Adims,2);
      }
    }
  }
  if (printlimit == 0)
    io->outputMessage("\nPrint limit has been reached.  Use setprintlimit function to enable longer printouts\n");
}

string ArrayToPrintableString(const Array& a) {
  static char msgBuffer[MSGBUFLEN];
  if (a.isEmpty())
    return string("[]");
  if (a.sparse())
    return string("");
  if (a.isString())
    return string(ArrayToString(a));
  if (a.isReferenceType())
    return string("");
  if (!a.isScalar() && !a.isString())
    return string("");
  const void *dp = a.getDataPointer();
  switch (a.dataClass()) {
  default:
    return string("?");
  case FM_INT8: {
    const int8 *ap;
    ap = (const int8*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%d",ap[0]);
    return string(msgBuffer);
  }
  case FM_UINT8: {
    const uint8 *ap;
    ap = (const uint8*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%u",ap[0]);
    return string(msgBuffer);
  }
  case FM_INT16: {
    const int16 *ap;
    ap = (const int16*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%d",ap[0]);
    return string(msgBuffer);
  }
  case FM_UINT16: {
    const uint16 *ap;
    ap = (const uint16*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%u",ap[0]);
    return string(msgBuffer);
  }
  case FM_INT32: {
    const int32 *ap;
    ap = (const int32*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%d",ap[0]);
    return string(msgBuffer);
  }
  case FM_UINT32: {
    const uint32 *ap;
    ap = (const uint32*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%u",ap[0]);
    return string(msgBuffer);
  }
  case FM_INT64: {
    const int64 *ap;
    ap = (const int64*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%lld",ap[0]);
    return string(msgBuffer);
  }
  case FM_UINT64: {
    const uint64 *ap;
    ap = (const uint64*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%llu",ap[0]);
    return string(msgBuffer);
  }
  case FM_LOGICAL: {
    const logical *ap;
    ap = (const logical*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%d",ap[0]);
    return string(msgBuffer);
  }
  case FM_FLOAT: {
    const float *ap;
    ap = (const float*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%g",ap[0]);
    return string(msgBuffer);
  }
  case FM_DOUBLE: {
    const double *ap;
    ap = (const double*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%g",ap[0]);
    return string(msgBuffer);
  }
  case FM_COMPLEX: {
    const float *ap;
    ap = (const float*) dp;
    if (ap[1] > 0)
      snprintf(msgBuffer,MSGBUFLEN,"%g+%g i",ap[0],ap[1]);
    else
      snprintf(msgBuffer,MSGBUFLEN,"%g%g i",ap[0],ap[1]);
    return string(msgBuffer);
  }
  case FM_DCOMPLEX: {
    const double *ap;
    ap = (const double*) dp;
    if (ap[1] > 0)
      snprintf(msgBuffer,MSGBUFLEN,"%g+%g i",ap[0],ap[1]);
    else
      snprintf(msgBuffer,MSGBUFLEN,"%g%g i",ap[0],ap[1]);
    return string(msgBuffer);
  }
  }
  return string("");
}
  
