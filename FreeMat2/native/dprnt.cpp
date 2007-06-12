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

template <class T>
void PrintFloatArray(T* t, int len, int width, int decimals) {
  for (int i=0;i<len;i++) {
    printf(" ");
    if (t[i] != 0) 
      printf("%*.*f",width,decimals,t[i]);
    else
      printf("%*d",width,0);
  }
  printf("\n");
}

template <class T>
void PrintComplexArray(T* t, int len, int width, int decimals) {
  for (int i=0;i<len;i++) {
    printf("  ");
    if ((t[2*i] != 0) || (t[2*i+1] != 0)) {
      printf("%*.*f",width,decimals,t[2*i]);
      if (t[2*i+1] > 0)
	printf(" +%*.*fi",width-1,decimals,t[2*i+1]);
      else
	printf(" -%*.*fi",width-1,decimals,-t[2*i+1]);
    } else 
      printf("%*d%*c",width,0,width+2,' ');
  }
  printf("\n");
}

template <class T>
void fixEprint(int width, int decimals, T val) {
  if (abs(ilogb(val))>99) decimals--;
  printf("%*.*e",width,decimals,val);
}

template <class T>
void PrintFloatArrayENotation(T* t, int len, int width, int decimals) {
  for (int i=0;i<len;i++) {
    printf(" ");
    if (t[i] != 0) 
      fixEprint(width,decimals,t[i]);
    else
      printf("%*d",width,0);
  }
  printf("\n");
}

template <class T>
void PrintComplexArrayENotation(T* t, int len, int width, int decimals) {
  for (int i=0;i<len;i++) {
    printf("  ");
    int mydec = decimals;
    if ((t[2*i] != 0) || (t[2*i+1] != 0)) {
      fixEprint(width,decimals,t[2*i]);
      if (t[2*i+1] > 0) {
	printf(" +");
	fixEprint(width-1,decimals,t[2*i+1]);
      } else {
	printf(" -");
	fixEprint(width-1,decimals,-t[2*i+1]);
      }
      printf("i");
    } else 
      printf("%*d%*c",width,0,width+2,' ');
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  {
    float *a = new float[12];
    a[0] = 27.2152;
    a[1] = 31.754;
    a[2] = -14.96534;
    a[3] = 1.03423;
    a[4] = 62.5754;
    a[5] = 23.2184234;
    a[6] = 0;
    a[7] = 0;
    a[8] = 15.754;
    a[9] = -42.96534;
    a[10] = 11.03423;
    a[11] = 67.5754;
    double *b = new double[12];
    b[0] = 72.1525453423;
    b[1] = 17.554523452345234;
    b[2] = -49.65334534512645734;
    b[3] = 10.34253472455386983;
    b[4] = 6.575408097983;
    b[5] = 22.18423480948502345;
    b[6] = 0;
    b[7] = 0;
    b[8] = -4.334534512645734;
    b[9] = 10.34253472455386983;
    b[10] = 6.575408097983;
    b[11] = 22.18423480948502345;
    printf("float/double short:\n");
    PrintFloatArray(a,4,9,4);
    PrintFloatArray(a+4,4,9,4);
    printf("float/double short e:\n");
    PrintFloatArrayENotation(a,4,11,4);
    PrintFloatArrayENotation(a+4,4,11,4);
    printf("float long:\n");
    PrintFloatArray(a,4,11,7);
    PrintFloatArray(a+4,4,11,7);
    printf("float long e:\n");
    PrintFloatArrayENotation(a,4,14,7);
    PrintFloatArrayENotation(a+4,4,14,7);
    printf("double long:\n");
    PrintFloatArray(b,4,18,14);
    PrintFloatArray(b+4,4,18,14);
    printf("double long e:\n");
    PrintFloatArrayENotation(b,4,21,14);
    PrintFloatArrayENotation(b+4,4,21,14);
    printf("complex/dcomplex short:\n");
    PrintComplexArray(a,5,8,4);
    PrintComplexArray(a+2,5,8,4);
    printf("complex/dcomplex short e:\n");
    PrintComplexArrayENotation(a,5,11,4);
    PrintComplexArrayENotation(a+2,5,11,4);
    printf("complex long:\n");
    PrintComplexArray(a,5,11,7);
    PrintComplexArray(a+2,5,11,7);
    printf("complex long e:\n");
    PrintComplexArrayENotation(a,4,14,7);
    PrintComplexArrayENotation(a+2,4,14,7);
    printf("dcomplex long:\n");
    PrintComplexArray(b,3,18,14);
    PrintComplexArray(b+4,3,18,14);
    printf("dcomplex long e:\n");
    PrintComplexArrayENotation(b,3,21,14);
    b[8] = -4.334534512645734e+190;
    PrintComplexArrayENotation(b+6,3,21,14);
    return 1;
  }
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
