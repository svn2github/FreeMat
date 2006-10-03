// This is an experiment in performance improvements for FreeMat 2.1
// In particular, I am thinking about rearranging the memory allocations
// to get better performance.
// The new array class would replace Array, and is a "smart" pointer to
// a memory 

#include "array.hpp"
#include <string.h>

array::array(int a) {
  m_flags.scalar = 1;
  int *dp = (int*) malloc(sizeof(int));
  m_ptr = new refcntarray(dp,sizeof(int));
  m_ptr->decrement();
  m_offset = 0;
  m_length = 1;
  m_flags.sparse = 0;
  m_flags.ndim = 2;
  m_dims[0] = m_dims[1] = 1;
}
array::array(int ndims, int *dimdata, void *data, int bytes) {
  m_flags.scalar = 0;
  m_flags.ndim = ndims;
  memcpy(m_dims,dimdata,ndims*sizeof(int));
  m_ptr = new refcntarray(data,bytes);
  m_ptr->increment();
  m_offset = 0;
}

array array::get(const array &I, const array &J) const {
  array retval(*this);
  retval.m_offset += ((int) I) + ((int) J) * m_dims[0];
  retval.m_length = 1;
  retval.m_flags.scalar = 1;
  retval.m_flags.sparse = 0;
  retval.m_flags.ndim = 2;
  retval.m_dims[0] = retval.m_dims[1] = 1;
  return retval;
}

void array::set(const array &I, const array &J, const array &V) {
  int offset = ((int) I) + ((int) J) * m_dims[0];
  m_ptr = m_ptr->singleOwner();
  ((double*) m_ptr->data())[offset] = (double) V;
}

array::array(const array& copy) : m_flags(copy.m_flags),
				  m_length(copy.m_length),
				  m_offset(copy.m_offset) {
  m_ptr = copy.m_ptr;
  m_ptr->increment();
  memcpy(m_dims,copy.m_dims,m_flags.ndim*sizeof(int));
}

array::~array() {
  m_ptr->decrement();
  if (m_ptr->isfree()) delete m_ptr;
}

void array::operator=(const array &copy) {
  if (this == &copy) return;
  m_ptr->decrement();
  if (m_ptr->isfree()) delete m_ptr;
  m_flags = copy.m_flags;
  m_length = copy.m_length;
  m_offset = copy.m_offset;
  m_ptr = copy.m_ptr;
  m_ptr->increment();
  memcpy(m_dims,copy.m_dims,m_flags.ndim*sizeof(int));
}
    
