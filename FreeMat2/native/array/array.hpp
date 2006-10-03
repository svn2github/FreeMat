#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int scalar:1; // Scalar?
  unsigned int sparse:1; // Sparse?
  unsigned int ndim:8;   // Number of dimensions
  unsigned int type:6;   // Type information
} flags;

class refcntarray {
  unsigned m_count;
  void *m_data;
  int m_len;
public:
  inline refcntarray(void *data, int len) : m_count(0), m_data(data), m_len(len) {}
  inline void* data() {return m_data;}
  inline void increment() {m_count++;}
  inline void decrement() {m_count--;}
  inline bool isfree() {return m_count<=0;}
  inline ~refcntarray() {free(m_data);}
  inline refcntarray* singleOwner() {
    if (m_count <= 1) 
      return this; 
    void *dp = malloc(m_len);
    memcpy(dp,m_data,m_len);
    refcntarray* copy = new refcntarray(dp,m_len);
    copy->increment();
    return copy;
  }
};

class array {
  flags        m_flags;
  int          m_dims[7];
  refcntarray *m_ptr;
  unsigned     m_length;
  int          m_offset;
public:
  array(int a);
  array(int ndims, int *dimdata, void* data, int bytes);
  array(const array& array);
  ~array();
  void set(const array &I, const array &J, const array &V);
  array get(const array &I, const array &J) const;
  inline operator int() const {return ((int*)m_ptr->data())[0];}
  inline operator double() const {return ((double*)m_ptr->data())[0];}
  void operator=(const array &copy);
};
