// This is an experiment in performance improvements for FreeMat 2.1
// In particular, I am thinking about rearranging the memory allocations
// to get better performance.
// The new array class would replace Array, and is a "smart" pointer to
// a memory 

#include "array.hpp"
#include <string.h>

array::array(int a) {
  m_flags.scalar = 1;
  m_value.int_val = a;
}
array::array(int ndims, int *dimdata, void *dataptr) {
  m_flags.scalar = 0;
  m_flags.ndim = ndims;
  memcpy(m_dims,dimdata,ndims*sizeof(int));
  m_value.ptr_val = dataptr;
}

array array::get(array I, array J) {
  array retval(*this);
  int offset = ((int) I) + ((int) J) * m_dims[0];
  retval.m_value.ptr_val = ((double*)m_value.ptr_val) + offset;
  return retval;
}

void array::set(array I, array J, array V) {
  int offset = ((int) I) + ((int) J) * m_dims[0];
  ((double*) m_value.ptr_val)[offset] = (double) V;
}


