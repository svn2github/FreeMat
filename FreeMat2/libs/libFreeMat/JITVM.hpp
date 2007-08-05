#ifndef __Compiler_hpp__
#define __Compiler_hpp__

#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include <string>

using namespace std;

typedef union {
  bool b;
  int32   i32;
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
  c_register
} scalarClass;

class JITScalar {
  scalarClass type;
  scalarValue value;
public:
  inline JITScalar(scalarClass t, scalarValue v) type(t), value(v) {};
  inline JITScalar(bool v) type(c_bool), value.b(v) {};
  inline JITScalar(int32 v) type(c_int32), value.i32(v) {};
  inline JITScalar(int64 v) type(c_int64), value.i64(v) {};
  inline JITScalar(float v) type(c_float), value.f(v) {};
  inline JITScalar(double v) type(c_double), value.d(v) {};
  inline JITScalar(void* v) type(c_pointer), value.p(v) {};
  inline JITScalar(uint32 v) type(c_register), value.reg(v) {};
  inline JITScalar() {};
  inline scalarClass type() {return type;}
  inline void setType(scalarClass t) {type = t;}
  inline scalarValue value() {return value;}
  inline void set(scalarValue v) {value = v;}
  inline void set(bool v) {value.b = v; type = c_bool;}
  inline void set(int32 v) {value.i32 = v; type = c_int32;}
  inline void set(int64 v) {value.i64 = v; type = c_int64;}
  inline void set(float v) {value.f = v; type = c_float;}
  inline void set(double v) {value.d = v; type = c_double;}
  inline void set(void* v) {value.p = v; type = c_pointer;}
  inline void set(uint32 v) {value.p = v; type = c_register;}
  inline bool   b()   {JITAssert(type == c_bool); return value.b;}
  inline int32  i32() {JITAssert(type == c_int32); return value.i32;}
  inline int64  i64() {JITAssert(type == c_int64); return value.i64;}
  inline float  f()   {JITAssert(type == c_float); return value.f;}
  inline double d()   {JITAssert(type == c_double); return value.d;}
  inline void*  p()   {JITAssert(type == c_pointer); return value.p;}
  inline int32  reg() {JITAssert(type == c_register); return value.reg;}
};

typedef enum {
    // 32 bit integer add
  add_i32_rr,
  add_i32_ri,
  add_i32_ir,
  add_i32_ii,
    // 32 bit float add
  add_f_rr,
  add_f_ri,
  add_f_ir,
  add_f_ii,
    // 64 bit float add
  add_d_rr,
  add_d_ri,
  add_d_ir,
  add_d_ii,
    // 32 bit integer subtract
  sub_i32_rr,
  sub_i32_ri,
  sub_i32_ir,
  sub_i32_ii,
    // 32 bit float subtract
  sub_f_rr,
  sub_f_ri,
  sub_f_ir,
  sub_f_ii,
    // 64 bit float subtract
  sub_d_rr,
  sub_d_ri,
  sub_d_ir,
  sub_d_ii,
    // 32 bit integer multiply
  mul_i32_rr,
  mul_i32_ri,
  mul_i32_ir,
  mul_i32_ii,
    // 32 bit float multiply
  mul_f_rr,
  mul_f_ri,
  mul_f_ir,
  mul_f_ii,
    // 64 bit float multiply
  mul_d_rr,
  mul_d_ri,
  mul_d_ir,
  mul_d_ii,
    // 32 bit float divide
  div_f_rr,
  div_f_ri,
  div_f_ir,
  div_f_ii,
    // 64 bit float divide
  div_d_rr,
  div_d_ri,
  div_d_ir,
  div_d_ii,
    // boolean ops
  or_b_rr,
  or_b_ri,
  or_b_ir,
  or_b_ii,
  and_b_rr,
  and_b_ri,
  and_b_ir,
  and_b_ii,
  not_b_r,
  not_b_i,
    // 32 bit integer less than
  lt_i32_rr,
  lt_i32_ri,
  lt_i32_ir,
  lt_i32_ii,
    // 32 bit float less than
  lt_f_rr,
  lt_f_ri,
  lt_f_ir,
  lt_f_ii,
    // 64 bit float less than
  lt_d_rr,
  lt_d_ri,
  lt_d_ir,
  lt_d_ii,
    // 32 bit integer less equals
  le_i32_rr,
  le_i32_ri,
  le_i32_ir,
  le_i32_ii,
    // 32 bit float less equals
  le_f_rr,
  le_f_ri,
  le_f_ir,
  le_f_ii,
    // 64 bit float less equals
  le_d_rr,
  le_d_ri,
  le_d_ir,
  le_d_ii,
    // 32 bit integer greater than
  gt_i32_rr,
  gt_i32_ri,
  gt_i32_ir,
  gt_i32_ii,
    // 32 bit float greater than
  gt_f_rr,
  gt_f_ri,
  gt_f_ir,
  gt_f_ii,
    // 64 bit float greater than
  gt_d_rr,
  gt_d_ri,
  gt_d_ir,
  gt_d_ii,
    // 32 bit integer greater equals
  ge_i32_rr,
  ge_i32_ri,
  ge_i32_ir,
  ge_i32_ii,
    // 32 bit float greater equals
  ge_f_rr,
  ge_f_ri,
  ge_f_ir,
  ge_f_ii,
    // 64 bit float greater equals
  ge_d_rr,
  ge_d_ri,
  ge_d_ir,
  ge_d_ii,
    // 32 bit integer not equals
  ne_i32_rr,
  ne_i32_ri,
  ne_i32_ir,
  ne_i32_ii,
    // 32 bit float not equals
  ne_f_rr,
  ne_f_ri,
  ne_f_ir,
  ne_f_ii,
    // 64 bit float not equals
  ne_d_rr,
  ne_d_ri,
  ne_d_ir,
  ne_d_ii,
    // 32 bit integer equals
  eq_i32_rr,
  eq_i32_ri,
  eq_i32_ir,
  eq_i32_ii,
    // 32 bit float equals
  eq_f_rr,
  eq_f_ri,
  eq_f_ir,
  eq_f_ii,
    // 64 bit float equals
  eq_d_rr,
  eq_d_ri,
  eq_d_ir,
  eq_d_ii,
    // 32 bit integer neg
  neg_i32_r,
  neg_i32_i,
    // 32 bit float neg
  neg_f_r,
  neg_f_ir,
    // 64 bit float neg
  neg_d_r,
  neg_d_i,
    // 32 bit integer set
  set_i32_r,
  set_i32_i,
    // 32 bit float set
  set_f_r,
  set_f_ir,
    // 64 bit float set
  set_d_r,
  set_d_i,
    // 32 bit integer cast
  casti32_f_r,
  casti32_f_i,
  casti32_d_r,
  casti32_d_i,
    // 32 bit float cast
  castf_i32_r,
  castf_i32_i,
  castf_d_r,
  castf_d_i,
    // 64 bit float cast
  castd_i32_r,
  castd_i32_i,
  castd_f_r,
  castd_f_i,
    // boolean cast
  castb_i32_r,
  castb_i32_i,
  castb_f_r,
  castb_f_i,
  castb_d_r,
  castb_d_i,
    // NO-OP
  nop,
    // Jump if true
  jit_b_r,
  jit_b_i,
    // Jump if false
  jif_b_r,
  jif_b_i
} opcodeType;

class JITInstruction {
  opcodeType op;
  JITScalar dest;
  JITScalar arg1;
  JITScalar arg2;
};

class JITInstructionStream {
  std::vector<JITInstruction> data;
public:
  void push_add(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_sub(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_mul(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_div(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_or(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_and(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_not(JITScalar dest, JITScalar arg1);
  void push_lt(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_le(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_gt(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_ge(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_eq(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_ne(JITScalar dest, JITScalar arg1, JITScalar arg2);
  void push_neg(JITScalar dest, JITScalar arg1);
  void push_set(JITScalar dest, JITScalar arg1);
  void push_casti32(JITScalar dest, JITScalar arg1);
  void push_castf(JITScalar dest, JITScalar arg1);
  void push_castd(JITScalar dest, JITScalar arg1);
  void push_castb(JITScalar dest, JITScalar arg1);
  void push_nop();
  void push_jit(JITScalar comp, JITScalar disp);
  void push_jif(JITScalar comp, JITScalar disp);
};

class JITVM {
public:
  void dispatch(opcodeType op);
};


class SymbolInformation {
public:
  RegisterReference m_rows;
  RegisterReference m_cols;
  opcodeClass m_type;
  RegisterReference m_base;
  bool m_scalar;
  bool m_argument;
  bool m_readonly;
  SymbolInformation() {}
  SymbolInformation(opcodeClass type, unsigned index) :
    m_type(type), m_base(index,type), m_scalar(true), m_argument(false) {}
  bool isScalar() {return m_scalar;}
  bool isArgument() {return m_argument;}
  bool isReadOnly() {return m_readonly;}
};

class VMStream {
  std::vector<VMInstruction*> p_data;
  SymbolTable<SymbolInformation> p_symbols;
  unsigned reglist;
  registerType reg[4096];
protected:
  inline void SetOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = op->literal.b;
      break;
    case unsigned_integer:
      reg[op->dest].u = op->literal.u;
      break;
    case integer:
      reg[op->dest].i = op->literal.i;
      break;
    case single_float:
      reg[op->dest].f = op->literal.f;
      break;
    case double_float:
      reg[op->dest].d = op->literal.d;
      break;
    case pointer:
      reg[op->dest].p = op->literal.p;
    }
  }
  inline void AddOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u + reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i + reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f + reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d + reg[op->arg2].d;
      break;
    }
  }
  inline void SubOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u - reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i - reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f - reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d - reg[op->arg2].d;
      break;
    }
  }
  inline void MulOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u * reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i * reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f * reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d * reg[op->arg2].d;
      break;
    }
  }
  inline void RDivOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u / reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i / reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f / reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d / reg[op->arg2].d;
      break;
    }
  }
  inline void LDivOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg2].u / reg[op->arg1].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg2].i / reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg2].f / reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg2].d / reg[op->arg1].d;
      break;
    }
  }
  inline void LTOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u < reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i < reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f < reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d < reg[op->arg2].d;
      break;
    }
  }
  inline void LEOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u <= reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i <= reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f <= reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d <= reg[op->arg2].d;
      break;
    }
  }
  inline void EQOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u == reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i == reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f == reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d == reg[op->arg2].d;
      break;
    }
  }
  inline void NEQOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u != reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i != reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f != reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d != reg[op->arg2].d;
      break;
    }
  }
  inline void GTOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u > reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i > reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f > reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d > reg[op->arg2].d;
      break;
    }
  }
  inline void CopyOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = reg[op->arg1].b;
      break;
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d;
      break;
    }
  }
  inline void CastBOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = reg[op->arg1].b;
      break;
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u != 0;
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i != 0;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f != 0;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d != 0;
      break;
    }
  }
  inline void CastUOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].u = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].u = (unsigned) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].u = (unsigned) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].u = (unsigned) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].u = (unsigned) reg[op->arg1].d;
      break;
    }
  }
  inline void CastIOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].i = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].i = (int) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].i = (int) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].i = (int) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].i = (int) reg[op->arg1].d;
      break;
    }
  }
  inline void CastFOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].f = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].f = (float) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].f = (float) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = (float) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].f = (float) reg[op->arg1].d;
      break;
    }
  }
  inline void DecOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      throw Exception("Decrement not supported for logical variables");
      break;
    case unsigned_integer:
      reg[op->dest].u--;
      break;
    case integer:
      reg[op->dest].i--;
      break;
    case single_float:
      reg[op->dest].f--;
      break;
    case double_float:
      reg[op->dest].d--;
      break;
    }
  }
  inline void CastDOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].d = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].d = (double) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].d = (double) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].d = (double) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = (double) reg[op->arg1].d;
      break;
    }
  }
  inline void NegOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      throw Exception("Neg not supported for boolean arguments");
      break;
    case unsigned_integer:
      throw Exception("Neg not supported for unsigned arguments");
      break;
    case integer:
      reg[op->dest].i = -reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = -reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = -reg[op->arg1].d;
      break;
    }
  }
  inline void NotOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = !reg[op->arg1].b;
      return;
    }
    throw Exception("NOT not supported for non-boolean arguments");
  }
  inline void GEOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u >= reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i >= reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f >= reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d >= reg[op->arg2].d;
      break;
    }
  }
  inline void OrOp(VMInstruction *op) {
    reg[op->dest].b = reg[op->arg1].b | reg[op->arg2].b;
  }
  inline void AndOp(VMInstruction *op) {
    reg[op->dest].b = reg[op->arg1].b & reg[op->arg2].b;
  }
  inline void XorOp(VMInstruction *op) {
    reg[op->dest].b = reg[op->arg1].b ^ reg[op->arg2].b;
  }
  inline void LoadOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = ((unsigned*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    case integer:
      reg[op->dest].i = ((int*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    case single_float:
      reg[op->dest].f = ((float*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    case double_float:
      reg[op->dest].d = ((double*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    }
  }
  inline void StoreOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      ((unsigned*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].u;
      break;
    case integer:
      ((int*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].i;
      break;
    case single_float:
      ((float*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].f;
      break;
    case double_float:
      ((double*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].d;
      break;
    }
  }
public:
  VMStream() : reglist(0) {}
  inline void push_back(VMInstruction *t) {p_data.push_back(t);}
  inline unsigned size() {return p_data.size();}
  VMInstruction* operator[](int i) {return p_data[i];}
  SymbolInformation* find_symbol(string name) {return p_symbols.findSymbol(name);}
  void add_symbol(const string &key, const SymbolInformation &val) {p_symbols.insertSymbol(key,val);}
  stringVector get_symbol_names() {return p_symbols.getCompletions("");}
  void Run(Interpreter* m_eval);
  inline unsigned GetReg() { return reglist++; }
};

VMStream CompileForBlock(const tree t, Interpreter* m_eval);
std::ostream& operator <<(std::ostream& o, VMStream& t);
#endif
