#include "ffi.h"
#include <stdlib.h>

/*
 * The test function...
 */
double testFunction(int a, float *p, short c, double q) {
  return (a + *p + c + q);
}

/*
 * Call it...
 */
int main() {
  int a;
  float p;
  float *paddr;
  short c;
  double q;
  double ret1, ret2;
  ffi_cif cif;
  ffi_type **args;
  void **values;
  
  a = 31;
  p = 2.12;
  c = -2;
  q = 3.1415;
  ret1 = testFunction(a,&p,c,q);
  printf("call1 = %f\n",ret1);
  
  /*
   * Call code
   */
  args = (ffi_type**) malloc(sizeof(ffi_type*)*4);
  args[0] = &ffi_type_sint32;
  args[1] = &ffi_type_pointer;
  args[2] = &ffi_type_sint16;
  args[3] = &ffi_type_double;
  values = (void**) malloc(sizeof(void*)*4);
  values[0] = &a;
  paddr = &p;
  values[1] = &paddr;
  values[2] = &c;
  values[3] = &q;
  if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 4, &ffi_type_double, args) == FFI_OK)
    {
      ffi_call(&cif, testFunction, &ret2, values);
      printf("call2 = %f\n",ret2);
    }
}
