#ifndef __JITVM_hpp__
#define __JITVM_hpp__

#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include <string>

using namespace std;

#define JITAssert(x) if (!(x)) throw Exception("JIT Assert failed");

typedef union {
  bool b;
  int32   i;
  float   f;
  double  d;
  void*   p;
  uint32  reg;
} scalarValue;

typedef enum {
  c_bool,
  c_int32,
  c_float,
  c_double,
  c_pointer,
  c_register,
  c_unknown
} scalarClass;

class JITScalar {
  scalarClass m_type;
  scalarValue m_value;
public:
  inline JITScalar(scalarClass t, scalarValue v) : m_type(t), m_value(v) {}
  inline JITScalar(bool v) : m_type(c_bool) {m_value.b = v;}
  inline JITScalar(int32 v) : m_type(c_int32) {m_value.i = v;}
  inline JITScalar(float v) : m_type(c_float) {m_value.f = v;}
  inline JITScalar(double v) : m_type(c_double) {m_value.d = v;}
  inline JITScalar(void* v) : m_type(c_pointer) {m_value.p = v;}
  inline JITScalar(uint32 v) : m_type(c_register) {m_value.reg = v;}
  inline JITScalar() : m_type(c_unknown) {};
  inline scalarClass type()  const {return m_type;}
  inline void setType(scalarClass t) {m_type = t;}
  inline scalarValue value() const {return m_value;}
  inline void set(scalarValue v) {m_value = v;}
  inline void set(bool v) {m_value.b = v; m_type = c_bool;}
  inline void set(int32 v) {m_value.i = v; m_type = c_int32;}
  inline void set(float v) {m_value.f = v; m_type = c_float;}
  inline void set(double v) {m_value.d = v; m_type = c_double;}
  inline void set(void* v) {m_value.p = v; m_type = c_pointer;}
  inline void set(uint32 v) {m_value.reg = v; m_type = c_register;}
  inline bool   b()   const {JITAssert(m_type == c_bool); return m_value.b;}
  inline int32  i()   const {JITAssert(m_type == c_int32); return m_value.i;}
  inline float  f()   const {JITAssert(m_type == c_float); return m_value.f;}
  inline double d()   const {JITAssert(m_type == c_double); return m_value.d;}
  inline void*  p()   const {JITAssert(m_type == c_pointer); return m_value.p;}
  inline int32  reg() const {JITAssert(m_type == c_register); return m_value.reg;}
};

typedef enum {
  op_add,
  op_sub,
  op_mul,
  op_div,
  op_or,
  op_and,
  op_not,
  op_lt,
  op_le,
  op_eq,
  op_ge,
  op_gt,
  op_ne,
  op_neg,
  op_set,
  op_castb,
  op_casti,
  op_castf,
  op_castd,
  op_nop,
  op_jit,
  op_mload,
  op_vload,
  op_mstore,
  op_vstore
} opcode;

typedef enum {
  type_b,
  type_i,
  type_f,
  type_d
} op_type;

typedef enum {
  arg_rr,
  arg_ri,
  arg_ir,
  arg_ii
} op_ri;

class JITInstruction {
public:
  opcode  op;
  op_type type;
  op_ri   ri;
  JITScalar   dest;
  JITScalar   arg1;
  JITScalar   arg2;
  JITScalar   arg3;
};

class JITVM {
  JITScalar reg[1024];
  std::vector<JITInstruction> data;
  unsigned ip;
  void dispatch_mloadb(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_mloadi(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_mloadf(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_mloadd(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_vloadb(JITScalar dest, void *p, int32 row);
  void dispatch_vloadi(JITScalar dest, void *p, int32 row);
  void dispatch_vloadf(JITScalar dest, void *p, int32 row);
  void dispatch_vloadd(JITScalar dest, void *p, int32 row);
  void dispatch_mstoreb(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_mstorei(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_mstoref(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_mstored(JITScalar dest, void *p, int32 row, int32 col);
  void dispatch_vstoreb(JITScalar dest, void *p, int32 row);
  void dispatch_vstorei(JITScalar dest, void *p, int32 row);
  void dispatch_vstoref(JITScalar dest, void *p, int32 row);
  void dispatch_vstored(JITScalar dest, void *p, int32 row);
  void dispatch(const JITInstruction &inst);
private:
  op_type compute_oc_code(scalarClass outClass);
  op_ri compute_ri_code(JITScalar arg1, JITScalar arg2);
  scalarClass type_of(JITScalar arg1);
  bool isi(JITScalar);
  bool isfd(JITScalar);
  JITScalar new_register(scalarClass outClass);
  JITScalar promote(JITScalar arg1, scalarClass outClass);
  JITScalar boolean_op(opcode op, JITScalar arg1, JITScalar arg2, string inst);
  JITScalar binary_op(opcode op, JITScalar arg1, JITScalar arg2, string inst);
  JITScalar comparison_op(opcode op, JITScalar arg1, JITScalar arg2, string inst);
  JITScalar rhs(tree t, Interpreter* m_eval);
  JITScalar expression(tree t, Interpreter* m_eval);
  void push_instruction(opcode,op_type,JITScalar result,op_ri,JITScalar arg1);
  void push_instruction(opcode,op_type,JITScalar result,op_ri,JITScalar arg1,JITScalar arg2);
  void push_instruction(opcode,op_type,JITScalar result,op_ri,JITScalar arg1,JITScalar arg2,JITScalar arg3);
public:
  void compile_for_block(tree t, Interpreter *m_eval);
  void run(Interpreter *m_eval);
};

#endif
