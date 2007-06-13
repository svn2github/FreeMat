#include "Print.hpp"
#include "IEEEFP.hpp"
#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include <math.h>
#include <string.h>

#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

FMFormatMode formatMode = format_short;

void SetPrintFormatMode(FMFormatMode m) {
  formatMode = m;
}

FMFormatMode GetPrintFormatMode() {
  return formatMode;
}


#if 0
template <class T>
void PrintSignedInteger(char *buffer, T data) {
  memset(buffer,0,MSGBUFLEN);
  sprintf(buffer,"%lld",(int64) T);
}

template <class T>
void PrintUnsignedInteger(char *buffer, T data) {
  memset(buffer,0,MSGBUFLEN);
  sprintf(buffer,"%llu",(uint64) T);
}

void PrintFloat(char *buffer, float  v, bool expformat) {
  if (formatMode == format_short) {
    if (expformat)
      sprintf(buffer,'%.4e',v);
    else {
      sprintf(buffer,'%.4f',v);
      int tlen = strlen(buffer);
      while ((cp[tlen] == '0') && (tlen
    
  }
}

class ArrayFormat {
public:
  int digits;
  int decimals;
  bool expformat;
  bool gformat;
  int width;
  ArrayFormat(int dig, int dec, bool eform, int wid, bool gform = false) : 
    digits(dig), decimals(dec), expformat(eform), width(wid), gformat(gform) {
  };
  ArrayFormat() : digits(0), decimals(0), expformat(false), gformat(false), width(0){
  };
};

template <class T>
int GetNominalWidthInteger(const T*array, int count) {
  char buffer[MSGBUFLEN];
    
  int maxdigit = 0;
  for (int i=0;i<count;i++) {
    memset(buffer,0,MSGBUFLEN);
    sprintf(buffer,"%lld",(uint64)array[i]);
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
    printf("%*lld\n",width,(int64)array[i]);
}
  
template <class T>
int GetNominalWidthFloat(const T*array, int count, int maxdigits_try, int &leading) {
  char buffer[MSGBUFLEN];
  
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
      memset(buffer,0,MSGBUFLEN);
      sprintf(buffer,"%0.*f",j,array[i]);
      match = (((T) atof(buffer)) == array[i]);
      if (!match) j++;
    }
    if (match && (strlen(buffer) > maxdigits_try+5)) return -1;
    if (!match) return -1;
    maxdigit = std::max(maxdigit,j);
    while (buffer[dig] && buffer[dig] != '.') dig++;
    leading = std::max(leading,dig);
  }
  return maxdigit;
}

template <class T>
int GetNominalWidthFloatE(const T*array, int count, int maxdigits_try) {
  char buffer[MSGBUFLEN];
  
  int maxdigit = 0;
  for (int i=0;i<count;i++) {
    // try different numbers of digits
    int j = 0;
    bool match = false;
    while ((j < maxdigits_try)  && (!match)) {
      memset(buffer,0,MSGBUFLEN);
      sprintf(buffer,"%0.*e",j,array[i]);
      match = (((T) atof(buffer)) == array[i]);
      if (!match) j++;
    }
    if (!match) return -1;
    maxdigit = std::max(maxdigit,j);
  }
  return maxdigit;
}

void emitFormattedElement(Interpreter* io, char *msgBuffer, ArrayFormat format, const void *dp, int num, Class dcls) {
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
  case FM_INT64: {
    const int64 *ap;
    ap = (const int64*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%*lld",format.digits,ap[num]);
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_UINT64: {
    const uint64 *ap;
    ap = (const uint64*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%*llu",format.digits,ap[num]);
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
      if (!format.gformat)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f",format.width,format.decimals,ap[num]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%*.*g",format.width,format.decimals,ap[num]);	
    else
      snprintf(msgBuffer,MSGBUFLEN,"%*.*e",format.width,format.decimals,ap[num]);
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_DOUBLE: {
    const double *ap;
    ap = (const double*) dp;
    if (!format.expformat)
      if (!format.gformat)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f",format.width,format.decimals,ap[num]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%*.*g",format.width,format.decimals,ap[num]);
    else
      snprintf(msgBuffer,MSGBUFLEN,"%*.*e",format.width,format.decimals,ap[num]);
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_COMPLEX: {
    const float *ap;
    ap = (const float*) dp;
    if (!format.expformat) {
      if (!format.gformat) {
	if (ap[2*num+1] >= 0)
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*f+%*.*f i",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
	else
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*f%*.*f i",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      } else {
	if (ap[2*num+1] >= 0)
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*g+%*.*g i",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
	else
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*g%*.*g i",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      }
    }
    else {
      if (ap[2*num+1] >= 0)
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e+%0.*e i",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e%0.*e i",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
    }
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_DCOMPLEX: {
    const double *ap;
    ap = (const double*) dp;
    if (!format.expformat) {
      if (!format.gformat) {
	if (ap[2*num+1] >= 0) 
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*f+%*.*fi",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
	else
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*f%*.*fi",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      } else {
	if (ap[2*num+1] >= 0) 
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*g+%*.*gi",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
	else
	  snprintf(msgBuffer,MSGBUFLEN,"%*.*g%*.*gi",
		   format.digits+format.decimals+1,format.decimals,ap[2*num],
		   format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      }
    }
    else {
      if (ap[2*num+1] >= 0) 
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e+%0.*ei",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e%0.*ei",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
    }
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_CELL_ARRAY: {
    Array *ap;
    ap = (Array*) dp;
    if (ap == NULL)
      io->outputMessage("[]");
    else
      ap[num].summarizeCellEntry(io);
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
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
    }
  }
  }
}

// Print the class type and size information for the array
void PrintArrayClassAndSize(Array A, Interpreter* io) {
  switch(A.dataClass()) {
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
  case FM_UINT64:
    io->outputMessage("  <uint64>  ");
    break;
  case FM_INT64:
    io->outputMessage("  <int64>  ");
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
      io->outputMessage(A.className().back().c_str());
      io->outputMessage(" object");
    } else
      io->outputMessage("  <structure array> ");
    break;
  case FM_FUNCPTR_ARRAY:
    io->outputMessage("  <function ptr array>  ");
  }
  io->outputMessage("- size: ");
  A.dimensions().printMe(io);
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
  int decimals = GetNominalWidthFloat<T>(data,count,19,leading);
  if (decimals < 0) {
    decimals = GetNominalWidthFloatE<T>(data,count,19);
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
  case FM_UINT64:
    return ScanFormatArrayInteger<uint64>((const uint64*) data,count);
  case FM_INT64:
    return ScanFormatArrayInteger<int64>((const int64*) data,count);
  case FM_LOGICAL:
    return ScanFormatArrayInteger<logical>((const logical*) data,count);
  case FM_FLOAT: {
    switch (formatMode) {
    case format_native:
      return ScanFormatArrayFloat<float>((const float*) data,count);
    case format_short:
    case format_long: 
      return ArrayFormat(11,4,false,11,true);
    case format_short_e:
    case format_long_e:
      return ArrayFormat(11,4,true,11,false);
    }
    return ScanFormatArrayFloat<float>((const float*) data,count);
  }
  case FM_DOUBLE: {
    switch (formatMode) {
    case format_native:
      return ScanFormatArrayFloat<double>((const double*) data,count);
    case format_short: 
      return ArrayFormat(11,4,false,11,true);
    case format_long: 
      return ArrayFormat(18,14,false,18,true);
    case format_short_e:
      return ArrayFormat(11,4,true,11,false);
    case format_long_e:
      return ArrayFormat(18,14,true,18,false);
    }
    return ScanFormatArrayFloat<double>((const double*) data,count);
  }
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

int getNumericElementWidth(Class dcls) {
  switch (dcls) {
  case FM_INT8: 
    return 4;
  case FM_UINT8: 
    return 3;
  case FM_INT16: 
    return 6;
  case FM_UINT16: 
    return 5;
  case FM_INT32:
    return 10;
  case FM_UINT32: 
    return 9;
  case FM_INT64: 
    return 21;
  case FM_UINT64: 
    return 20;
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
    if (!format.expformat) {
      if (ap[2*num+1] >= 0)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f+%*.*f i",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f%*.*f i",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
    }
    else {
      if (ap[2*num+1] >= 0)
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e+%0.*e i",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e%0.*e i",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
    }
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_DCOMPLEX: {
    const double *ap;
    ap = (const double*) dp;
    if (!format.expformat) {
      if (ap[2*num+1] >= 0) 
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f+%*.*fi",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f%*.*fi",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
    }
    else {
      if (ap[2*num+1] >= 0) 
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e+%0.*ei",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e%0.*ei",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
    }
    io->outputMessage(msgBuffer);
    break;
  }
  }
}

void emitFormattedNumericElement(Interpreter* io, char *msgBuffer, const void *dp, int num, Class dcls) {
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
  case FM_INT64: {
    const int64 *ap;
    ap = (const int64*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%*lld",format.digits,ap[num]);
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_UINT64: {
    const uint64 *ap;
    ap = (const uint64*) dp;
    snprintf(msgBuffer,MSGBUFLEN,"%*llu",format.digits,ap[num]);
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
    if (!format.expformat) {
      if (ap[2*num+1] >= 0)
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f+%*.*f i",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f%*.*f i",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
    }
    else {
      if (ap[2*num+1] >= 0)
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e+%0.*e i",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e%0.*e i",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
    }
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_DCOMPLEX: {
    const double *ap;
    ap = (const double*) dp;
    if (!format.expformat) {
      if (ap[2*num+1] >= 0) 
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f+%*.*fi",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%*.*f%*.*fi",
		 format.digits+format.decimals+1,format.decimals,ap[2*num],
		 format.digits+format.decimals+1,format.decimals,ap[2*num+1]);
    }
    else {
      if (ap[2*num+1] >= 0) 
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e+%0.*ei",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
      else
	snprintf(msgBuffer,MSGBUFLEN,"%0.*e%0.*ei",
		 format.decimals,ap[2*num],
		 format.decimals,ap[2*num+1]);
    }
    io->outputMessage(msgBuffer);
    break;
  }
  case FM_CELL_ARRAY: {
    Array *ap;
    ap = (Array*) dp;
    if (ap == NULL)
      io->outputMessage("[]");
    else
      ap[num].summarizeCellEntry(io);
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
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
    }
  }
  }
}

void PrintNumericSheet(Interpreter*io, int rows, int columns, 
		       int offset, const void* data, Class aclass, int termWidth,
		       int &printlimit) {
  if (printlimit == 0) return;
  // Determine how many columns will fit across
  // the terminal width
  int colsPerPage;
  int width = GetElementWidth(aclass);
  colsPerPage = (int) floor((termWidth-1)/((double) width + 3));
  colsPerPage = (colsPerPage < 1) ? 1 : colsPerPage;
  int pageCount;
  pageCount = (int) ceil(columns/((double)colsPerPage));
  for (int k=0;k<pageCount;k++) {
    int colsInThisPage;
    colsInThisPage = columns - colsPerPage*k;
    colsInThisPage = (colsInThisPage > colsPerPage) ? 
      colsPerPage : colsInThisPage;
    if (rows*columns > 1) {
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
			     data,
			     i+(k*colsPerPage+j)*rows+offset,
			     aclass);
	printlimit--;
	if (printlimit <= 0) return;
	io->outputMessage("  ");
      }
      snprintf(msgBuffer,MSGBUFLEN,"\n");
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN);
    }
  }
}

// Helper function - print an array using "classic" notation
void PrintArrayClassic(Array A, int printlimit, Interpreter* io,  
		       bool showClassSize) {
  if (printlimit == 0) return;
  int termWidth = io->getTerminalWidth();
  if (!A.isEmpty() && showClassSize && (formatMode == format_native))
    PrintArrayClassAndSize(A,io);
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
    sprintf(msgBuffer,"\tMatrix is sparse with %d nonzeros\n",
	    A.getNonzeros());
    io->outputMessage(msgBuffer);
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
	ap[n].summarizeCellEntry(io);
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
    if (Adims.getLength() == 2) {
      int rows = Adims.getRows();
      int columns = Adims.getColumns(); 
      PrintSheet(io,rows,columns,0,A.getDataPointer(),Aclass,termWidth,printlimit);
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
	  snprintf(msgBuffer,MSGBUFLEN,",%d",wdims.get(m)+1);
	  io->outputMessage(msgBuffer);
	}
	snprintf(msgBuffer,MSGBUFLEN,") = \n");
	io->outputMessage(msgBuffer);
	PrintSheet(io,rows,columns,offset,A.getDataPointer(),Aclass,termWidth,printlimit);
	offset += rows*columns;
	wdims.incrementModulo(Adims,2);
      }
    }
  }
  if (printlimit == 0)
    io->outputMessage("\nPrint limit has been reached.  Use setprintlimit function to enable longer printouts\n");
}


#endif

/**********************************************************************/

class ArrayFormatInfo {
public:
  int width;
  bool floatasint;
  int decimals;
  bool expformat;
  ArrayFormatInfo(int w, bool f = false, int d = 0, bool e = false) : width(w), floatasint(f), decimals(d), expformat(e) {}
};

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

ArrayFormatInfo ComputeArrayFormatInfo(const void *dp, int length, Class aclass) {
  switch (aclass) {
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
      if (formatMode == format_short)
	return ArrayFormatInfo(9,false,4);
      else if (formatMode == format_long)
	return ArrayFormatInfo(11,false,7);
    }
  case FM_DOUBLE:  
    if (AllIntegerValues((const double*)dp,length)) 
      return ArrayFormatInfo(GetNominalWidthSignedInteger((const double*)dp,length),
			     true);
    else {
      if (formatMode == format_short)
	return ArrayFormatInfo(9,false,4);
      else if (formatMode == format_long)
	return ArrayFormatInfo(18,false,14);
    }
  case FM_COMPLEX: 
    return ArrayFormatInfo(19,false,4);
  case FM_DCOMPLEX: 
    return ArrayFormatInfo(19,false,4);
  case FM_CELL_ARRAY:
    return ArrayFormatInfo(20);
  case FM_FUNCPTR_ARRAY:
    return ArrayFormatInfo(20);
  }  
}

template <class T>
void emitSignedInteger(Interpreter* io, T val, const ArrayFormatInfo &format) {
  snprintf(msgBuffer,MSGBUFLEN,"%*lld",format.width,(int64)val);
  io->outputMessage(msgBuffer);
}

template <class T>
void emitUnsignedInteger(Interpreter* io, T val, const ArrayFormatInfo &format) {
  snprintf(msgBuffer,MSGBUFLEN,"%*llu",format.width,(uint64)val);
  io->outputMessage(msgBuffer);
}

template <class T>
void emitFloat(Interpreter*io, T val, const ArrayFormatInfo &format) {
  if (val != 0) 
    snprintf(msgBuffer,MSGBUFLEN,"%*.*f",format.width,format.decimals,val);
  else
    snprintf(msgBuffer,MSGBUFLEN,"%*d",format.width,0);
  io->outputMessage(msgBuffer);
}

template <class T>
void emitComplex(Interpreter* io, T real, T imag, const ArrayFormatInfo &format) {
  int width = format.width/2;
  if ((real != 0) || (imag != 0)) {
    snprintf(msgBuffer,MSGBUFLEN,"%*.*f",width,format.decimals,real);
    io->outputMessage(msgBuffer);
    if (imag < 0)
      snprintf(msgBuffer,MSGBUFLEN," -%*.*fi",width-1,format.decimals,-imag);
    else
      snprintf(msgBuffer,MSGBUFLEN," +%*.*fi",width-1,format.decimals,imag);
    io->outputMessage(msgBuffer);
  } else {
    snprintf(msgBuffer,MSGBUFLEN,"%*d%*c",width,0,width+2,' ');
    io->outputMessage(msgBuffer);    
  }
}

void emitFormattedElement(Interpreter* io, char *msgBuffer, const void *dp, 
			  const ArrayFormatInfo &format, int num, Class dcls) {
  switch (dcls) {
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
    snprintf(msgBuffer,MSGBUFLEN,"%c\0",((const char*) dp)[num]);
    io->outputMessage(msgBuffer);
    return;
  case FM_CELL_ARRAY: {
    Array *ap;
    ap = (Array*) dp;
    if (ap == NULL)
      io->outputMessage("[]");
    else
      ap[num].summarizeCellEntry(io);
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
      snprintf(msgBuffer,MSGBUFLEN,"  ");
      io->outputMessage(msgBuffer);
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
    colsPerPage = (int) floor((termWidth-1)/((double) format.width + 3));
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
    if (rows*columns > 1) {
      snprintf(msgBuffer,MSGBUFLEN," \nColumns %d to %d\n\n",
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
			     data,
			     format,
			     i+(k*colsPerPage+j)*rows+offset,
			     aclass);
	printlimit--;
	if (printlimit <= 0) return;
	if (aclass != FM_STRING) io->outputMessage("  ");
      }
      snprintf(msgBuffer,MSGBUFLEN,"\n");
      io->outputMessage(msgBuffer);
      memset(msgBuffer,0,MSGBUFLEN);
    }
  }
  snprintf(msgBuffer,MSGBUFLEN,"\n");
  io->outputMessage(msgBuffer);
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
    sprintf(msgBuffer,"\tMatrix is sparse with %d nonzeros\n",
	    A.getNonzeros());
    io->outputMessage(msgBuffer);
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
	ap[n].summarizeCellEntry(io);
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
      Dimensions wdims(Adims.getLength());
      int rows(Adims.getRows());
      int columns(Adims.getColumns());
      int offset = 0;
      while (wdims.inside(Adims)) {
	snprintf(msgBuffer,MSGBUFLEN,"(:,:");
	io->outputMessage(msgBuffer);
	for (int m=2;m<Adims.getLength();m++) {
	  snprintf(msgBuffer,MSGBUFLEN,",%d",wdims.get(m)+1);
	  io->outputMessage(msgBuffer);
	}
	snprintf(msgBuffer,MSGBUFLEN,") = \n");
	io->outputMessage(msgBuffer);
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
  
