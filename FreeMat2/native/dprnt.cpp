#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

typedef long long int64;
typedef unsigned long long uint64;


template <class T>
void PrintSignedInteger(char *buffer, T data) {
  sprintf(buffer,"%lld",(int64) data);
}

template <class T>
void PrintUnsignedInteger(char *buffer, T data) {
  sprintf(buffer,"%llu",(uint64) data);
}

template <class T>
void PrintFloat(char *buffer, T data, int digits, bool expformat, bool asinteger, bool useg) {
  if (asinteger) {
    sprintf(buffer,"%d",(int) data);
    return;
  }
  if (useg) {
    sprintf(buffer,"%.*g",digits,data);
  }
  if (expformat) {
    sprintf(buffer,"%.*e",digits,data);
    return;
  }
  if ((fabs(data) < 100) && (fabs(data) >= 10)) digits--;
  if ((fabs(data) < 1000) && (fabs(data) >= 100)) digits-=2;
  sprintf(buffer,"%.*f",digits,data);
}

template <class T>
bool NeedExpFormat(T *t, int len) {
  if (len == 0) return false;
  T maxval = fabs(t[0]);
  for (int i=0;i<len;i++)
    maxval = std::max(maxval,(T)fabs((double)t[i]));
  return ((maxval < 1e-3) || (maxval > 1e3));
}

template <class T>
bool AllIntegerValues(T *t, int len) {
  if (len == 0) return false;
  bool allInts = true;
  for (int i=0;(i<len) && allInts;i++) {
    allInts = (t[i] == ((T) ((int) t[i])));
  }
  return allInts;
}

typedef enum {
  format_native,
  format_short,
  format_long,
  format_short_g,
  format_long_g,
  format_short_e,
  format_long_e
} FMFormatMode;

FMFormatMode format = format_short;

template <class T>
void PrintFloatArray(T *t, int len, int digits) {
  char buffer[1023];
  bool expFormatFlag = NeedExpFormat(t,len);
  bool allIntegerFlag = AllIntegerValues(t,len);
  bool useGflag = (format == format_short_g) || (format == format_long_g);
  if ((format == format_short_e) || (format == format_long_e))
    expFormatFlag = true;
  for (int i=0;i<len;i++) {
    PrintFloat(buffer,t[i],digits,expFormatFlag,allIntegerFlag,useGflag);
    printf("%s\n",buffer);
  }
}

template <class T>
void PrintFloatArray(T* t, int len) {
  if (sizeof(T) == sizeof(float)) {
    if ((format == format_short) || (format == format_short_e) || (format == format_short_g))
      PrintFloatArray(t,len,4);
    else
      PrintFloatArray(t,len,7);
  } else {
	if ((format == format_short) || (format == format_short_e) || (format == format_short_g))
      PrintFloatArray(t,len,4);
    else
      PrintFloatArray(t,len,15);
  }
}

int main(int argc, char *argv[]) {
  {
    float *a = new float[20];
    float b = .000005123456789;
    for (int i=0;i<20;i++)  {
      a[i] = b;
      b = b*10;
    }
    a[9] = 0;
    printf("Short - float\n");
    format = format_short;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Long - float\n");
    format = format_long;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Short E- float\n");
    format = format_short_e;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Long E- float\n");
    format = format_long_e;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Short G- float\n");
    format = format_short_g;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Long G- float\n");
    format = format_long_g;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
  }
  {
    double *a = new double[20];
    double b = .000005123456789;
    for (int i=0;i<20;i++)  {
      a[i] = b;
      b = b*10;
    }
    a[9] = 0;
    printf("Short - double\n");
    format = format_short;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Long - double\n");
    format = format_long;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Short E - double\n");
    format = format_short_e;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Long E - double\n");
    format = format_long_e;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Short G - double\n");
    format = format_short_g;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
    printf("Long G - double\n");
    format = format_long_g;
    for (int i=0;i<20;i++)
      PrintFloatArray(a+i,1);
    PrintFloatArray(a,20);
  }
  return 0;
}
