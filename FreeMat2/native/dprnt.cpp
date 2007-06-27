#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

//  Scalars are handled differently from arrays.  For scalars, exponential format would be used.
//  For arrays
// >> g*100
//
// ans =
//
//   -23.4004   62.3234   23.7882   38.9880  -94.9904
//          0   79.9049 -100.7763    8.7987   78.1182
//    31.4809   94.0890  -74.2045  -63.5465   56.8961
//   144.3508  -99.2092  108.2295  -55.9573  -82.1714
//   -35.0975   21.2035  -13.1500   44.3653  -26.5607
//>> g*700
//
// ans =
//
//    1.0e+03 *
//
//    -0.1638    0.4363    0.1665    0.2729   -0.6649
//          0    0.5593   -0.7054    0.0616    0.5468
//     0.2204    0.6586   -0.5194   -0.4448    0.3983
//     1.0105   -0.6945    0.7576   -0.3917   -0.5752
//    -0.2457    0.1484   -0.0920    0.3106   -0.1859
//This is %9.4f
//>> g*50
//
//ans =
//
// -11.70020238280165  31.16169255692471  11.89410364377894  19.49402448435195 -47.49518992738227
//                  0  39.95243090738892 -50.38816958391340   4.39935532898965  39.05908089391957
//  15.74045216975279  47.04449703638902 -37.10223760668019 -31.77326127396581  28.44803228616369
//  72.17541221749103 -49.60458677718977  54.11474765776669 -27.97866510981205 -41.08571458481278
// -17.54873691638709  10.60175760825277  -6.57498514726368  22.18267447518337 -13.28034256662745
//
//>> g*100
//
//ans =
//
//   1.0e+02 *
//
//  -0.23400404765603   0.62323385113849   0.23788207287558   0.38988048968704  -0.94990379854765
//                  0   0.79904861814778  -1.00776339167827   0.08798710657979   0.78118161787839
//   0.31480904339506   0.94088994072778  -0.74204475213360  -0.63546522547932   0.56896064572327
//   1.44350824434982  -0.99209173554380   1.08229495315533  -0.55957330219624  -0.82171429169626
//  -0.35097473832774   0.21203515216506  -0.13149970294527   0.44365348950367  -0.26560685133255
//
//>> single(g*100)
//
//ans =
//
//   1.0e+02 *
//
//  -0.2340040   0.6232338   0.2378821   0.3898805  -0.9499038
//           0   0.7990486  -1.0077634   0.0879871   0.7811816
//   0.3148090   0.9408900  -0.7420448  -0.6354652   0.5689606
//   1.4435083  -0.9920918   1.0822949  -0.5595733  -0.8217143
//  -0.3509747   0.2120352  -0.1314997   0.4436535  -0.2656068
//
//>> single(g*10)
//
//ans =
//
//  -2.3400404   6.2323384   2.3788207   3.8988049  -9.4990377
//           0   7.9904861 -10.0776339   0.8798711   7.8118162
//   3.1480904   9.4088993  -7.4204473  -6.3546524   5.6896067
//  14.4350824  -9.9209175  10.8229494  -5.5957332  -8.2171431
//  -3.5097473   2.1203516  -1.3149971   4.4365349  -2.6560686
//
// >> single(g)+i*single(g)

// ans =

//   Columns 1 through 4

//  -0.2340041 - 0.2340041i  0.6232339 + 0.6232339i  0.2378821 + 0.2378821i  0.3898805 + 0.3898805i
//           0               0.7990486 + 0.7990486i -1.0077634 - 1.0077634i  0.0879871 + 0.0879871i
//   0.3148091 + 0.3148091i  0.9408900 + 0.9408900i -0.7420447 - 0.7420447i -0.6354652 - 0.6354652i
//   1.4435083 + 1.4435083i -0.9920917 - 0.9920917i  1.0822949 + 1.0822949i -0.5595733 - 0.5595733i
//  -0.3509747 - 0.3509747i  0.2120351 + 0.2120351i -0.1314997 - 0.1314997i  0.4436535 + 0.4436535i

//   Column 5

//  -0.9499038 - 0.9499038i
//   0.7811816 + 0.7811816i
//   0.5689607 + 0.5689607i
//  -0.8217143 - 0.8217143i
//  -0.2656069 - 0.2656069i

// >> single(50*g)+i*single(50*g)

// ans =

//   Columns 1 through 4

// -11.7002020 -11.7002020i 31.1616917 +31.1616917i 11.8941040 +11.8941040i 19.4940243 +19.4940243i
//           0              39.9524307 +39.9524307i-50.3881683 -50.3881683i  4.3993554 + 4.3993554i
//  15.7404518 +15.7404518i 47.0444984 +47.0444984i-37.1022377 -37.1022377i-31.7732620 -31.7732620i
//  72.1754150 +72.1754150i-49.6045876 -49.6045876i 54.1147461 +54.1147461i-27.9786644 -27.9786644i
// -17.5487366 -17.5487366i 10.6017580 +10.6017580i -6.5749850 - 6.5749850i 22.1826744 +22.1826744i

//   Column 5

// -47.4951897 -47.4951897i
//  39.0590820 +39.0590820i
//  28.4480324 +28.4480324i
// -41.0857162 -41.0857162i
// -13.2803421 -13.2803421i
// >> single(100*g)+i*single(100*g)

// ans =

//    1.0e+02 *

//   Columns 1 through 4

//  -0.2340040 - 0.2340040i  0.6232338 + 0.6232338i  0.2378821 + 0.2378821i  0.3898805 + 0.3898805i
//           0               0.7990486 + 0.7990486i -1.0077634 - 1.0077634i  0.0879871 + 0.0879871i
//   0.3148090 + 0.3148090i  0.9408900 + 0.9408900i -0.7420448 - 0.7420448i -0.6354652 - 0.6354652i
//   1.4435083 + 1.4435083i -0.9920918 - 0.9920918i  1.0822949 + 1.0822949i -0.5595733 - 0.5595733i
//  -0.3509747 - 0.3509747i  0.2120352 + 0.2120352i -0.1314997 - 0.1314997i  0.4436535 + 0.4436535i

//   Column 5

//  -0.9499038 - 0.9499038i
//   0.7811816 + 0.7811816i
//   0.5689606 + 0.5689606i
//  -0.8217143 - 0.8217143i
//  -0.2656068 - 0.2656068i
// >> format short
// >> single(100*g)+i*single(100*g)
//
// ans =
//
//    1.0e+02 *
//
//   -0.2340 - 0.2340i   0.6232 + 0.6232i   0.2379 + 0.2379i   0.3899 + 0.3899i  -0.9499 - 0.9499i
//         0             0.7990 + 0.7990i  -1.0078 - 1.0078i   0.0880 + 0.0880i   0.7812 + 0.7812i
//    0.3148 + 0.3148i   0.9409 + 0.9409i  -0.7420 - 0.7420i  -0.6355 - 0.6355i   0.5690 + 0.5690i
//    1.4435 + 1.4435i  -0.9921 - 0.9921i   1.0823 + 1.0823i  -0.5596 - 0.5596i  -0.8217 - 0.8217i
//   -0.3510 - 0.3510i   0.2120 + 0.2120i  -0.1315 - 0.1315i   0.4437 + 0.4437i  -0.2656 - 0.2656i
// >> format short e
// >> a = format
// ??? Error using ==> format
// Too many output arguments.
//
// >> single(100*g)+i*single(100*g)
//
// ans =
//
//   Columns 1 through 4
//
//  -2.3400e+01 -2.3400e+01i  6.2323e+01 +6.2323e+01i  2.3788e+01 +2.3788e+01i  3.8988e+01 +3.8988e+01i
//            0               7.9905e+01 +7.9905e+01i -1.0078e+02 -1.0078e+02i  8.7987e+00 +8.7987e+00i
//   3.1481e+01 +3.1481e+01i  9.4089e+01 +9.4089e+01i -7.4204e+01 -7.4204e+01i -6.3547e+01 -6.3547e+01i
//   1.4435e+02 +1.4435e+02i -9.9209e+01 -9.9209e+01i  1.0823e+02 +1.0823e+02i -5.5957e+01 -5.5957e+01i
//  -3.5097e+01 -3.5097e+01i  2.1204e+01 +2.1204e+01i -1.3150e+01 -1.3150e+01i  4.4365e+01 +4.4365e+01i
//
//   Column 5
//
//  -9.4990e+01 -9.4990e+01i
//   7.8118e+01 +7.8118e+01i
//   5.6896e+01 +5.6896e+01i
//  -8.2171e+01 -8.2171e+01i
//  -2.6561e+01 -2.6561e+01i



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
void PrintFloatArrayENotation(T* t, int len, int width, int decimals) {
  for (int i=0;i<len;i++) {
    printf(" ");
    if (t[i] != 0) 
      printf("%*.*e",width,decimals,t[i]);
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
      printf("%*.*e",width,decimals,t[2*i]);
      if (t[2*i+1] > 0) {
	printf(" +");
	printf("%*.*e",width-1,decimals,t[2*i+1]);
      } else {
	printf(" -");
	printf("%*.*e",width-1,decimals,-t[2*i+1]);
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
    PrintComplexArrayENotation(b,3,22,14);
    b[8] = -4.334534512645734e+190;
    PrintComplexArrayENotation(b+6,3,22,14);
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
