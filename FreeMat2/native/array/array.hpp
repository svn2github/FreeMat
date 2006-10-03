typedef struct {
  unsigned int scalar:1; // Scalar?
  unsigned int sparse:1; // Sparse?
  unsigned int ndim:8;   // Number of dimensions
  unsigned int type:6;   // Type information
} flags;

typedef union {
  char   char_val;
  short  short_val;
  int    int_val;
  float  float_val;
  double double_val;
  float  complex_val[2];
  double dcomplex_val[2];
  void*  ptr_val;
} value;

class array {
  flags m_flags;
  int   m_dims[7];
  value m_value;
public:
  array(int a);
  array(int ndims, int *dimdata, void *dataptr);
  void set(array I, array J, array V);
  array get(array I, array j);
  inline operator int() {return m_value.int_val;}
  inline operator double() {return m_value.double_val;}
};
