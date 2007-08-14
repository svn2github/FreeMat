#ifndef __JITVM_hpp__
#define __JITVM_hpp__

#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include <string>

using namespace std;

//#define JITAssert(x) if (!(x)) throw Exception("JIT Assert failed");
#define JITAssert(x)

typedef union {
  bool b;
  int32   i;
  float   f;
  double  d;
  void*   p;
  uint32  reg;
} scalar_value;

typedef enum {
  c_bool,
  c_int32,
  c_float,
  c_double,
  c_pointer,
  c_register,
  c_unknown
} scalar_class;

class JITScalar {
  scalar_class m_type;
  scalar_value m_value;
public:
  inline JITScalar(scalar_class t, scalar_value v) : m_type(t), m_value(v) {}
  inline JITScalar(bool v) : m_type(c_bool) {m_value.b = v;}
  inline JITScalar(int32 v) : m_type(c_int32) {m_value.i = v;}
  inline JITScalar(float v) : m_type(c_float) {m_value.f = v;}
  inline JITScalar(double v) : m_type(c_double) {m_value.d = v;}
  inline JITScalar(void* v) : m_type(c_pointer) {m_value.p = v;}
  inline JITScalar(uint32 v) : m_type(c_register) {m_value.reg = v;}
  inline JITScalar() : m_type(c_unknown) {m_value.d = 0;};
  inline scalar_class type()  const {return m_type;}
  inline void setType(scalar_class t) {m_type = t;}
  inline scalar_value value() const {return m_value;}
  inline void set(scalar_value v) {m_value = v;}
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
  inline bool   isp() const {return (m_type == c_pointer);}
  inline bool   isscalar() const {return (m_type <= c_double);}
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
  op_jif,
  op_jmp,
  op_vload,
  op_vstore,
  op_mset,
  op_mload,
  op_mstore,
  op_end
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
  JITInstruction(opcode p, op_type t, JITScalar d, op_ri r, JITScalar a1) :
    op(p), type(t), ri(r), dest(d), arg1(a1) {}
  JITInstruction(opcode p, op_type t, JITScalar d, op_ri r, JITScalar a1, JITScalar a2) :
    op(p), type(t), ri(r), dest(d), arg1(a1), arg2(a2) {}
  JITInstruction(opcode p, op_type t, JITScalar d, op_ri r, JITScalar a1, 
		 JITScalar a2, JITScalar a3) :
    op(p), type(t), ri(r), dest(d), arg1(a1), arg2(a2), arg3(a3) {}
  void dump(ostream& o);
};

class JITVM;

typedef void (JITVM::*op_ptr_bool) (int32, bool, bool);
typedef void (JITVM::*op_ptr_int) (int32, int32, int32);
typedef void (JITVM::*op_ptr_float) (int32, float, float);
typedef void (JITVM::*op_ptr_double) (int32, double, double);

class JITVM {
private:
  JITScalar reg[1024];
  op_ptr_bool   opcode_table_b[op_end];
  op_ptr_int    opcode_table_i[op_end];
  op_ptr_float  opcode_table_f[op_end];
  op_ptr_double opcode_table_d[op_end];
  std::vector<JITInstruction> data;
  unsigned ip;
  unsigned next_reg;
  int32 m_row, m_col;
  SymbolTable<JITScalar> symbols;
  template <class T> void add_func(int32 dest, T op1, T op2) {reg[dest].set(op1 + op2);}
  template <class T> void sub_func(int32 dest, T op1, T op2) {reg[dest].set(op1 - op2);}
  template <class T> void mul_func(int32 dest, T op1, T op2) {reg[dest].set(op1 * op2);}
  template <class T> void div_func(int32 dest, T op1, T op2) {reg[dest].set(op1 / op2);}
  void or_func(int32 dest, bool op1, bool op2) {reg[dest].set(op1 || op2);}
  void and_func(int32 dest, bool op1, bool op2) {reg[dest].set(op1 && op2);}
  void not_func(int32 dest, bool op1, bool) {reg[dest].set(!op1);}
  template <class T> void neg_func(int32 dest, T op1, T) {reg[dest].set(-op1);}
  template <class T> void lt_func(int32 dest, T op1, T op2) {reg[dest].set(op1 < op2);}
  template <class T> void le_func(int32 dest, T op1, T op2) {reg[dest].set(op1 <= op2);}
  template <class T> void gt_func(int32 dest, T op1, T op2) {reg[dest].set(op1 > op2);}
  template <class T> void ge_func(int32 dest, T op1, T op2) {reg[dest].set(op1 >= op2);}
  template <class T> void eq_func(int32 dest, T op1, T op2) {reg[dest].set(op1 == op2);}
  template <class T> void ne_func(int32 dest, T op1, T op2) {reg[dest].set(op1 != op2);}
  template <class T> void set_func(int32 dest, T op1, T) {reg[dest].set(op1);}
  template <class T> void castb_func(int32 dest, T op1, T) {reg[dest].set(op1 != 0);}
  template <class T> void casti_func(int32 dest, T op1, T) {reg[dest].set((int32) op1);}
  template <class T> void castf_func(int32 dest, T op1, T) {reg[dest].set((float) op1);}
  template <class T> void castd_func(int32 dest, T op1, T) {reg[dest].set((double) op1);}
  template <class T> void nop_func(int32, T, T) {}
  void jit_func(int32 dest, int32 op1, int32) {if (reg[dest].b()) ip = op1;}
  void jif_func(int32 dest, int32 op1, int32) {if (!reg[dest].b()) ip = op1;}
  void jmp_func(int32, int32 op1, int32) {ip = op1;}
  void casti_b_func(int32 dest, bool op1, bool) {op1 ? reg[dest].set(1) : reg[dest].set(0);}
  void castf_b_func(int32 dest, bool op1, bool) {op1 ? reg[dest].set(1.0f) : reg[dest].set(0.0f);}
  void castd_b_func(int32 dest, bool op1, bool) {op1 ? reg[dest].set(1.0) : reg[dest].set(0.0);}
  void set_matrix_arg(int32 row, int32 col);
  void illegal_op_bool(int32, bool, bool);
  void illegal_op_int32(int32, int32, int32);
  void illegal_op_float(int32, float, float);
  void illegal_op_double(int32, double, double);
  inline JITScalar* find_symbol(string name) {return symbols.findSymbol(name);}
  void add_symbol(string name, JITScalar value) {symbols.insertSymbol(name,value);}
  JITScalar* add_argument(string name, Interpreter* m_eval, bool scalar);
  scalar_class map_array_type(void* ptr);
  void dispatch_vload(op_type type, JITScalar dest, void *p, int32 row);
  void dispatch_vstore(op_type type, void *p, int32 row, JITScalar dest);
  void dispatch_mload(op_type type, JITScalar dest, void *p);
  void dispatch_mstore(op_type type, void *p, JITScalar dest);
  void dispatch(const JITInstruction &inst);
  op_type compute_oc_code(scalar_class outClass);
  op_ri compute_ri_code(JITScalar arg1, JITScalar arg2);
  op_ri compute_ri_code(JITScalar arg1);
  scalar_class type_of(JITScalar arg1);
  bool isscalar(JITScalar);
  bool isi(JITScalar);
  bool isp(JITScalar);
  bool isfd(JITScalar);
  JITScalar new_register(scalar_class outClass);
  JITScalar promote(JITScalar arg1, scalar_class outClass);
  JITScalar boolean_op(opcode op, JITScalar arg1, JITScalar arg2, string inst);
  JITScalar binary_op(opcode op, JITScalar arg1, JITScalar arg2, string inst);
  JITScalar comparison_op(opcode op, JITScalar arg1, JITScalar arg2, string inst);
  void push_instruction(opcode,op_type,JITScalar result,op_ri,JITScalar arg1);
  void push_instruction(opcode,op_type,JITScalar result,op_ri,JITScalar arg1,JITScalar arg2);
  void push_instruction(opcode,op_type,JITScalar result,op_ri,JITScalar arg1,JITScalar arg2,JITScalar arg3);
public:
  JITVM();
  void dump(ostream& o);
  JITScalar compile_expression(tree t, Interpreter* m_eval);
  JITScalar compile_rhs(tree t, Interpreter* m_eval);
  void compile_if_statement(tree t, Interpreter* m_eval);
  void compile_for_block(tree t, Interpreter *m_eval);
  void compile_assignment(tree t, Interpreter *m_eval);
  void compile_statement(tree t, Interpreter *m_eval);
  void compile_statement_type(tree t, Interpreter *m_eval);
  void compile_block(tree t, Interpreter *m_eval);
  void run(Interpreter *m_eval);
};

#endif
