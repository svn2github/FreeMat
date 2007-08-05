// Still need:
//  Type promotion on loop entry.
//  Range checking.
//  Dynamic array resizing.
//

//
// 
//

//
// A prototype compiler for FreeMat.  Consider the simplest function
//
//  function y = add(a,b)
//     y = a + b;
// The corresponding C++ code is
//
//  ArrayVector addFunction(const ArrayVector& args) {
//     Array a(args[0]);
//     Array b(args[1]);
//     Array y(Add(a,b));
//     ArrayVector varargout;
//     varargout.push_back(y);
//     return varargout;
//  }
//
// 
//
// Just for fun, mind you....
//

#include "JITVM.hpp"

void JITVM::dispatch(opcodeType op) {
  switch (op) {
    // 32 bit integer add
  case add_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() + reg[arg2.reg()].i32()); break;
  case add_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() + arg2.i32()); break;
  case add_i32_ir:
    reg[dest.reg()].set(arg1.i32() + reg[arg2.reg()].i32()); break;
  case add_i32_ii:
    reg[dest.reg()].set(arg1.i32() + arg2.i32()); break;
    // 32 bit float add
  case add_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() + reg[arg2.reg()].f()); break;
  case add_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() + arg2.f()); break;
  case add_f_ir:
    reg[dest.reg()].set(arg1.f() + reg[arg2.reg()].f()); break;
  case add_f_ii:
    reg[dest.reg()].set(arg1.f() + arg2.f()); break;
    // 64 bit float add
  case add_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() + reg[arg2.reg()].d()); break;
  case add_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() + arg2.d()); break;
  case add_d_ir:
    reg[dest.reg()].set(arg1.d() + reg[arg2.reg()].d()); break;
  case add_d_ii:
    reg[dest.reg()].set(arg1.d() + arg2.d()); break;
    // 32 bit integer subtract
  case sub_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() - reg[arg2.reg()].i32()); break;
  case sub_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() - arg2.i32()); break;
  case sub_i32_ir:
    reg[dest.reg()].set(arg1.i32() - reg[arg2.reg()].i32()); break;
  case sub_i32_ii:
    reg[dest.reg()].set(arg1.i32() - arg2.i32()); break;
    // 32 bit float subtract
  case sub_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() - reg[arg2.reg()].f()); break;
  case sub_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() - arg2.f()); break;
  case sub_f_ir:
    reg[dest.reg()].set(arg1.f() - reg[arg2.reg()].f()); break;
  case sub_f_ii:
    reg[dest.reg()].set(arg1.f() - arg2.f()); break;
    // 64 bit float subtract
  case sub_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() - reg[arg2.reg()].d()); break;
  case sub_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() - arg2.d()); break;
  case sub_d_ir:
    reg[dest.reg()].set(arg1.d() - reg[arg2.reg()].d()); break;
  case sub_d_ii:
    reg[dest.reg()].set(arg1.d() - arg2.d()); break;
    // 32 bit integer multiply
  case mul_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() * reg[arg2.reg()].i32()); break;
  case mul_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() * arg2.i32()); break;
  case mul_i32_ir:
    reg[dest.reg()].set(arg1.i32() * reg[arg2.reg()].i32()); break;
  case mul_i32_ii:
    reg[dest.reg()].set(arg1.i32() * arg2.i32()); break;
    // 32 bit float multiply
  case mul_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() * reg[arg2.reg()].f()); break;
  case mul_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() * arg2.f()); break;
  case mul_f_ir:
    reg[dest.reg()].set(arg1.f() * reg[arg2.reg()].f()); break;
  case mul_f_ii:
    reg[dest.reg()].set(arg1.f() * arg2.f()); break;
    // 64 bit float multiply
  case mul_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() * reg[arg2.reg()].d()); break;
  case mul_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() * arg2.d()); break;
  case mul_d_ir:
    reg[dest.reg()].set(arg1.d() * reg[arg2.reg()].d()); break;
  case mul_d_ii:
    reg[dest.reg()].set(arg1.d() * arg2.d()); break;
    // 32 bit float divide
  case div_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() / reg[arg2.reg()].f()); break;
  case div_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() / arg2.f()); break;
  case div_f_ir:
    reg[dest.reg()].set(arg1.f() / reg[arg2.reg()].f()); break;
  case div_f_ii:
    reg[dest.reg()].set(arg1.f() / arg2.f()); break;
    // 64 bit float divide
  case div_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() / reg[arg2.reg()].d()); break;
  case div_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() / arg2.d()); break;
  case div_d_ir:
    reg[dest.reg()].set(arg1.d() / reg[arg2.reg()].d()); break;
  case div_d_ii:
    reg[dest.reg()].set(arg1.d() / arg2.d()); break;
    // boolean ops
  case or_b_rr:
    reg[dest.reg()].set(reg[arg1.reg()].b() || reg[arg2.reg()].b()); break;
  case or_b_ri:
    reg[dest.reg()].set(reg[arg1.reg()].b() || arg2.b()); break;
  case or_b_ir:
    reg[dest.reg()].set(arg1.b() || reg[arg2.reg()].b()); break;
  case or_b_ii:
    reg[dest.reg()].set(arg1.b() || arg2.b()); break;
  case and_b_rr:
    reg[dest.reg()].set(reg[arg1.reg()].b() && reg[arg2.reg()].b()); break;
  case and_b_ri:
    reg[dest.reg()].set(reg[arg1.reg()].b() && arg2.b()); break;
  case and_b_ir:
    reg[dest.reg()].set(arg1.b() && reg[arg2.reg()].b()); break;
  case and_b_ii:
    reg[dest.reg()].set(arg1.b() && arg2.b()); break;
  case not_b_r:
    reg[dest.reg()].set(!reg[arg1.reg()].b()); break;
  case not_b_i:
    reg[dest.reg()].set(!arg1.b()); break;
    // 32 bit integer less than
  case lt_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() < reg[arg2.reg()].i32()); break;
  case lt_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() < arg2.i32()); break;
  case lt_i32_ir:
    reg[dest.reg()].set(arg1.i32() < reg[arg2.reg()].i32()); break;
  case lt_i32_ii:
    reg[dest.reg()].set(arg1.i32() < arg2.i32()); break;
    // 32 bit float less than
  case lt_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() < reg[arg2.reg()].f()); break;
  case lt_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() < arg2.f()); break;
  case lt_f_ir:
    reg[dest.reg()].set(arg1.f() < reg[arg2.reg()].f()); break;
  case lt_f_ii:
    reg[dest.reg()].set(arg1.f() < arg2.f()); break;
    // 64 bit float less than
  case lt_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() < reg[arg2.reg()].d()); break;
  case lt_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() < arg2.d()); break;
  case lt_d_ir:
    reg[dest.reg()].set(arg1.d() < reg[arg2.reg()].d()); break;
  case lt_d_ii:
    reg[dest.reg()].set(arg1.d() < arg2.d()); break;
    // 32 bit integer less equals
  case le_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() <= reg[arg2.reg()].i32()); break;
  case le_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() <= arg2.i32()); break;
  case le_i32_ir:
    reg[dest.reg()].set(arg1.i32() <= reg[arg2.reg()].i32()); break;
  case le_i32_ii:
    reg[dest.reg()].set(arg1.i32() <= arg2.i32()); break;
    // 32 bit float less equals
  case le_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() <= reg[arg2.reg()].f()); break;
  case le_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() <= arg2.f()); break;
  case le_f_ir:
    reg[dest.reg()].set(arg1.f() <= reg[arg2.reg()].f()); break;
  case le_f_ii:
    reg[dest.reg()].set(arg1.f() <= arg2.f()); break;
    // 64 bit float less equals
  case le_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() <= reg[arg2.reg()].d()); break;
  case le_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() <= arg2.d()); break;
  case le_d_ir:
    reg[dest.reg()].set(arg1.d() <= reg[arg2.reg()].d()); break;
  case le_d_ii:
    reg[dest.reg()].set(arg1.d() <= arg2.d()); break;
    // 32 bit integer greater than
  case gt_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() > reg[arg2.reg()].i32()); break;
  case gt_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() > arg2.i32()); break;
  case gt_i32_ir:
    reg[dest.reg()].set(arg1.i32() > reg[arg2.reg()].i32()); break;
  case gt_i32_ii:
    reg[dest.reg()].set(arg1.i32() > arg2.i32()); break;
    // 32 bit float greater than
  case gt_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() > reg[arg2.reg()].f()); break;
  case gt_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() > arg2.f()); break;
  case gt_f_ir:
    reg[dest.reg()].set(arg1.f() > reg[arg2.reg()].f()); break;
  case gt_f_ii:
    reg[dest.reg()].set(arg1.f() > arg2.f()); break;
    // 64 bit float greater than
  case gt_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() > reg[arg2.reg()].d()); break;
  case gt_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() > arg2.d()); break;
  case gt_d_ir:
    reg[dest.reg()].set(arg1.d() > reg[arg2.reg()].d()); break;
  case gt_d_ii:
    reg[dest.reg()].set(arg1.d() > arg2.d()); break;
    // 32 bit integer greater equals
  case ge_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() >= reg[arg2.reg()].i32()); break;
  case ge_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() >= arg2.i32()); break;
  case ge_i32_ir:
    reg[dest.reg()].set(arg1.i32() >= reg[arg2.reg()].i32()); break;
  case ge_i32_ii:
    reg[dest.reg()].set(arg1.i32() >= arg2.i32()); break;
    // 32 bit float greater equals
  case ge_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() >= reg[arg2.reg()].f()); break;
  case ge_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() >= arg2.f()); break;
  case ge_f_ir:
    reg[dest.reg()].set(arg1.f() >= reg[arg2.reg()].f()); break;
  case ge_f_ii:
    reg[dest.reg()].set(arg1.f() >= arg2.f()); break;
    // 64 bit float greater equals
  case ge_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() >= reg[arg2.reg()].d()); break;
  case ge_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() >= arg2.d()); break;
  case ge_d_ir:
    reg[dest.reg()].set(arg1.d() >= reg[arg2.reg()].d()); break;
  case ge_d_ii:
    reg[dest.reg()].set(arg1.d() >= arg2.d()); break;
    // 32 bit integer not equals
  case ne_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() != reg[arg2.reg()].i32()); break;
  case ne_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() != arg2.i32()); break;
  case ne_i32_ir:
    reg[dest.reg()].set(arg1.i32() != reg[arg2.reg()].i32()); break;
  case ne_i32_ii:
    reg[dest.reg()].set(arg1.i32() != arg2.i32()); break;
    // 32 bit float not equals
  case ne_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() != reg[arg2.reg()].f()); break;
  case ne_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() != arg2.f()); break;
  case ne_f_ir:
    reg[dest.reg()].set(arg1.f() != reg[arg2.reg()].f()); break;
  case ne_f_ii:
    reg[dest.reg()].set(arg1.f() != arg2.f()); break;
    // 64 bit float not equals
  case ne_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() != reg[arg2.reg()].d()); break;
  case ne_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() != arg2.d()); break;
  case ne_d_ir:
    reg[dest.reg()].set(arg1.d() != reg[arg2.reg()].d()); break;
  case ne_d_ii:
    reg[dest.reg()].set(arg1.d() != arg2.d()); break;
    // 32 bit integer equals
  case eq_i32_rr:
    reg[dest.reg()].set(reg[arg1.reg()].i32() == reg[arg2.reg()].i32()); break;
  case eq_i32_ri:
    reg[dest.reg()].set(reg[arg1.reg()].i32() == arg2.i32()); break;
  case eq_i32_ir:
    reg[dest.reg()].set(arg1.i32() == reg[arg2.reg()].i32()); break;
  case eq_i32_ii:
    reg[dest.reg()].set(arg1.i32() == arg2.i32()); break;
    // 32 bit float equals
  case eq_f_rr:
    reg[dest.reg()].set(reg[arg1.reg()].f() == reg[arg2.reg()].f()); break;
  case eq_f_ri:
    reg[dest.reg()].set(reg[arg1.reg()].f() == arg2.f()); break;
  case eq_f_ir:
    reg[dest.reg()].set(arg1.f() == reg[arg2.reg()].f()); break;
  case eq_f_ii:
    reg[dest.reg()].set(arg1.f() == arg2.f()); break;
    // 64 bit float equals
  case eq_d_rr:
    reg[dest.reg()].set(reg[arg1.reg()].d() == reg[arg2.reg()].d()); break;
  case eq_d_ri:
    reg[dest.reg()].set(reg[arg1.reg()].d() == arg2.d()); break;
  case eq_d_ir:
    reg[dest.reg()].set(arg1.d() == reg[arg2.reg()].d()); break;
  case eq_d_ii:
    reg[dest.reg()].set(arg1.d() == arg2.d()); break;
    // 32 bit integer neg
  case neg_i32_r:
    reg[dest.reg()].set(-reg[arg1.reg()].i32()); break;
  case neg_i32_i:
    reg[dest.reg()].set(-arg1.i32()); break;
    // 32 bit float neg
  case neg_f_r:
    reg[dest.reg()].set(-reg[arg1.reg()].f()); break;
  case neg_f_ir:
    reg[dest.reg()].set(-arg1.f()); break;
    // 64 bit float neg
  case neg_d_r:
    reg[dest.reg()].set(-reg[arg1.reg()].d()); break;
  case neg_d_i:
    reg[dest.reg()].set(-arg2.d()); break;
    // 32 bit integer set
  case set_i32_r:
    reg[dest.reg()].set(reg[arg1.reg()].i32()); break;
  case set_i32_i:
    reg[dest.reg()].set(arg1.i32()); break;
    // 32 bit float set
  case set_f_r:
    reg[dest.reg()].set(reg[arg1.reg()].f()); break;
  case set_f_ir:
    reg[dest.reg()].set(arg1.f()); break;
    // 64 bit float set
  case set_d_r:
    reg[dest.reg()].set(reg[arg1.reg()].d()); break;
  case set_d_i:
    reg[dest.reg()].set(arg2.d()); break;
    // 32 bit integer cast
  case casti32_f_r:
    reg[dest.reg()].set((int32)(reg[arg1.reg()].f())); break;
  case casti32_f_i:
    reg[dest.reg()].set((int32)(arg1.f())); break;
  case casti32_d_r:
    reg[dest.reg()].set((int32)(reg[arg1.reg()].d())); break;
  case casti32_d_i:
    reg[dest.reg()].set((int32)(arg1.d())); break;
    // 32 bit float cast
  case castf_i32_r:
    reg[dest.reg()].set((float)(reg[arg1.reg()].i32())); break;
  case castf_i32_i:
    reg[dest.reg()].set((float)(arg1.i32())); break;
  case castf_d_r:
    reg[dest.reg()].set((float)(reg[arg1.reg()].d())); break;
  case castf_d_i:
    reg[dest.reg()].set((float)(arg1.d())); break;
    // 64 bit float cast
  case castd_i32_r:
    reg[dest.reg()].set((double)(reg[arg1.reg()].i32())); break;
  case castd_i32_i:
    reg[dest.reg()].set((double)(arg1.i32())); break;
  case castd_f_r:
    reg[dest.reg()].set((double)(reg[arg1.reg()].f())); break;
  case castd_f_i:
    reg[dest.reg()].set((double)(arg1.f())); break;
    // boolean cast
  case castb_i32_r:
    reg[dest.reg()].set(reg[arg1.reg()].i32() != 0); break;
  case castb_i32_i:
    reg[dest.reg()].set(arg1.i32() != 0); break;
  case castb_f_r:
    reg[dest.reg()].set(reg[arg1.reg()].f() != 0); break;
  case castb_f_i:
    reg[dest.reg()].set(arg1.f() != 0); break;
  case castb_d_r:
    reg[dest.reg()].set(reg[arg1.reg()].d() != 0); break;
  case castb_d_i:
    reg[dest.reg()].set(arg1.d() != 0); break;
    // NO-OP
  case nop:
    break;
    // Jump if true
  case jit_b_r:
    if (reg[arg1.reg()].b()) ip += arg2.i32(); break;
  case jit_b_i:
    if (arg1.b()) ip += arg2.i32(); break;
    // Jump if false
  case jif_b_r:
    if (!reg[arg1.reg()].b()) ip += arg2.i32(); break;
  case jif_b_i:
    if (!arg1.b()) ip += arg2.i32(); break;
    
  load_b_rr,
  load_b_ri,
  load_i32_rr,
  load_i32_ri,
  load_i64_rr,
  load_i64_ri,
  load_f_rr,
  load_f_ri,
  load_d_rr,
  load_d_ri,
  store_b_rr,
  store_b_ri,
  store_i32_rr,
  store_i32_ri,
  store_i64_rr,
  store_i64_ri,
  store_f_rr,
  store_f_ri,
  store_d_rr,
  store_d_ri,
    }
}


static inline registerType RTUnsigned(unsigned x) {
  registerType ret;
  ret.u = x;
  return ret;
}

static inline registerType RTBoolean(bool x) {
  registerType ret;
  ret.b = x;
  return ret;
}

static inline registerType RTInteger(int x) {
  registerType ret;
  ret.i = x;
  return ret;
}

static inline registerType RTFloat(float x) {
  registerType ret;
  ret.f = x;
  return ret;  
}

static inline registerType RTDouble(double x) {
  registerType ret;
  ret.d = x;
  return ret;  
}

static inline registerType RTPointer(void* x) {
  registerType ret;
  ret.p = x;
  return ret;  
}

std::string RegisterName(opcodeClass t, registerType topval) {
  char buffer[512];
  sprintf(buffer,"unknown");
  switch(t) {
  case t_boolean: 
    sprintf(buffer,"%u",topval.b);
    break;
  case unsigned_integer:
    sprintf(buffer,"%u",topval.u);
    break;
  case integer:
    sprintf(buffer,"%d",topval.i);
    break;
  case single_float:
    sprintf(buffer,"%g",topval.f);
    break;
  case double_float:
    sprintf(buffer,"%g",topval.d);
    break;
  case pointer:
    sprintf(buffer,"%x",(unsigned)topval.p);
  }
  return buffer;
}

std::string OpCodeName(opcodeType topcode) {
  switch(topcode) {
  case ADD:   return "ADD  ";
  case SUB:   return "SUB  ";
  case SET:   return "SET  ";
  case NOP:   return "NOP  ";
  case LOAD:  return "LOAD ";
  case STORE: return "STORE";
  case MUL:   return "MUL  ";
  case RDIV:  return "RDIV ";
  case LDIV:  return "LDIV "; 
  case OR:    return "OR   ";
  case AND:   return "AND  ";
  case XOR:   return "XOR  ";
  case LT:    return "LT   ";
  case LE:    return "LE   ";
  case EQ:    return "EQ   ";
  case NEQ:   return "NEQ  ";
  case GE:    return "GE   ";
  case GT:    return "GT   ";
  case JIT:   return "JIT  ";
  case JIF:   return "JIF  ";
  case JMP:   return "JMP  ";
  case COPY:  return "COPY ";
  case CASTB: return "CASTB";
  case CASTI: return "CASTI";
  case CASTU: return "CASTU";
  case CASTF: return "CASTF";
  case CASTD: return "CASTD";
  case NEG:   return "NEG  ";
  case POS:   return "POS  ";
  case NOT:   return "NOT  ";
  case DEC:   return "DEC  ";
  case RET:   return "RET  ";
  }
}

std::string OpCodeClass(opcodeClass topclass) {
  switch(topclass) {
  case t_boolean: return "BOL";
  case unsigned_integer: return "USN";
  case integer: return "INT";
  case single_float: return "FLT";
  case double_float: return "DBL";
  case pointer: return "PNT";
  }
}

void VMInstruction::print(std::ostream& o) {
  if (opcode == NOP) {
    o << OpCodeName(opcode) << "\r\n";
  } else if (opcode == SET) {
    o << OpCodeName(opcode) << " <" << OpCodeClass(opclass) << "> \t";
    o << "r" << dest << "," << RegisterName(opclass,literal) << "\r\n";
  } else if ((opcode == JIT) || (opcode == JIF)) {
    o << OpCodeName(opcode) << "       \t";
    o << "r" << dest << "," << RegisterName(opclass,literal) << "\r\n";
  } else if (opcode == JMP) {
    o << OpCodeName(opcode) << "       \t";
    o << RegisterName(opclass,literal) << "\r\n";
  } else if (opcode == DEC) {
    o << OpCodeName(opcode) << "       \t";
    o << "r" << dest << "\r\n";
  } else if ((opcode == CASTI) || (opcode == CASTU) || 
	     (opcode == CASTF) || (opcode == CASTD) ||
	     (opcode == NEG) || (opcode == POS) || 
	     (opcode == NOT)) {
    o << OpCodeName(opcode) << " <" << OpCodeClass(opclass) << "> \t";
    o << "r" << dest << ",";
    o << "r" << arg1 << "\r\n";
  } else {
    o << OpCodeName(opcode) << " <" << OpCodeClass(opclass) << "> \t";
    o << "r" << dest << ",";
    o << "r" << arg1 << ",";
    o << "r" << arg2 << "\r\n";
  }
}

void VMStream::Run(Interpreter* m_eval) {
  // Fill out the arguments
  stringVector syms(get_symbol_names());
  for (int i=0;i<syms.size();i++) {
    SymbolInformation *psym = find_symbol(syms[i]);
    if (psym && psym->isArgument()) {
      ArrayReference ptr(m_eval->getContext()->lookupVariable(syms[i]));
      if (!ptr.valid())
	throw Exception("Undefined variable reference: " + syms[i]);
      if (psym->isScalar() && ptr->isScalar()) {
	switch (ptr->dataClass()) {
	case FM_FUNCPTR_ARRAY:
	case FM_CELL_ARRAY:
	case FM_STRUCT_ARRAY:
	case FM_UINT8:
	case FM_INT8:
	case FM_UINT16:
	case FM_INT16:
	case FM_UINT64:
	case FM_INT64:
	case FM_COMPLEX:
	case FM_DCOMPLEX:
	case FM_STRING:
	  throw Exception("Cannot JIT code because of type of :" + syms[i]);
	case FM_LOGICAL:
	  if (psym->m_type != t_boolean)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].b = ((const logical*)(ptr->getDataPointer()))[0];
	  break;
	case FM_UINT32:
	  if (psym->m_type != unsigned_integer)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].u = ((const uint32*)(ptr->getDataPointer()))[0];
	  break;
	case FM_INT32:
	  if (psym->m_type != integer)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].i = ((const int32*)(ptr->getDataPointer()))[0];
	  break;
	case FM_FLOAT:
	  if (psym->m_type != single_float)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].f = ((const float*)(ptr->getDataPointer()))[0];
	  break;
	case FM_DOUBLE:
	  if (psym->m_type != double_float)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].d = ((const double*)(ptr->getDataPointer()))[0];
	}
      } else {
	if (psym->isReadOnly()) {
	  reg[psym->m_base.index()].p = (void*) ptr->getDataPointer();
	} else {
	  reg[psym->m_base.index()].p = ptr->getReadWriteDataPointer();
	}
	reg[psym->m_rows.index()].u = ptr->rows();
	reg[psym->m_cols.index()].u = ptr->columns();
      }      
    }
  }
  
  bool term = false;
  unsigned ip = 0;
  unsigned ops_max = p_data.size();
  while (ip < ops_max) {
    VMInstruction *op = p_data[ip++];
    switch(op->opcode) {
    case ADD:
      AddOp(op);
      break;
    case SET:
      SetOp(op);
      break;
    case NOP:
      break;
    case SUB:
      SubOp(op);
      break;
    case MUL:
      MulOp(op);
      break;
    case RDIV:
      RDivOp(op);
      break;
    case LDIV:
      LDivOp(op);
      break;
    case LT:
      LTOp(op);
      break;
    case LE:
      LEOp(op);
      break;
    case EQ:
      EQOp(op);
      break;
    case NEQ:
      NEQOp(op);
      break;
    case GE:
      GEOp(op);
      break;
    case GT:
      GTOp(op);
      break;
    case OR:
      OrOp(op);
      break;
    case AND:
      AndOp(op);
      break;
    case XOR:
      XorOp(op);
      break;
    case LOAD:
      LoadOp(op);
      break;
    case STORE:
      StoreOp(op);
      break;
    case JIT:
      if (reg[op->dest].b)
	ip = op->literal.u;
      break;
    case JIF:
      if (!reg[op->dest].b)
	ip = op->literal.u;
      break;
    case COPY:
      CopyOp(op);
      break;
    case CASTB:
      CastBOp(op);
      break;
    case CASTU:
      CastUOp(op);
      break;
    case CASTI:
      CastIOp(op);
      break;
    case CASTF:
      CastFOp(op);
      break;
    case CASTD:
      CastDOp(op);
      break;
    case NEG:
      NegOp(op);
      break;
    case POS:
      break;
    case NOT:
      NotOp(op);
      break;
    case DEC:
      DecOp(op);
      break;
    case RET:
      return;
    }
  }
}

std::ostream& operator <<(std::ostream& o, VMStream& t) {
  stringVector syms(t.get_symbol_names());
  for (int i=0;i<syms.size();i++) {
    SymbolInformation *psym = t.find_symbol(syms[i]);
    if (psym && psym->isArgument()) {
      std::cout << "ARG: " << syms[i] << " : ";
      std::cout << "Base: " << psym->m_base.index() << "\r\n";
    }
  }
  for (int i=0;i<t.size();i++) {
    char buffer[1000];
    sprintf(buffer,"%03u: ",i);
    o << buffer;
    t[i]->print(o);
  }
  return o;
}

VMStream& operator <<(VMStream& o, VMInstruction* t) {
  o.push_back(t);
  return o;
}

void JITBlock(VMStream& o, tree t, Interpreter* m_eval);
RegisterReference JITExpression(VMStream& o, tree t, Interpreter* m_eval);

RegisterReference JITPromote(VMStream& o, RegisterReference a, opcodeClass c) {
  RegisterReference out(o.GetReg(),c);
  if (a.type() == t_boolean) {
    switch(c) {
    case t_boolean:
      return a;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,t_boolean,out.index(),a.index());
      return out;
    case integer:
      o << new VMInstruction(CASTI,t_boolean,out.index(),a.index());
      return a;
    case single_float:
      o << new VMInstruction(CASTF,t_boolean,out.index(),a.index());
      return out;
    case double_float:
      o << new VMInstruction(CASTD,t_boolean,out.index(),a.index());
      return out;
    }
  }
  if (a.type() == integer) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,integer,out.index(),a.index());
      return out;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,integer,out.index(),a.index());
      return out;
    case integer:
      return a;
    case single_float:
      o << new VMInstruction(CASTF,integer,out.index(),a.index());
      return out;
    case double_float:
      o << new VMInstruction(CASTD,integer,out.index(),a.index());
      return out;
    }
  } else if (a.type() == unsigned_integer) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,unsigned_integer,out.index(),a.index());
      return out;      
    case unsigned_integer:
      return a;
    case integer:
      o << new VMInstruction(CASTI,unsigned_integer,out.index(),a.index());
      return out;
    case single_float:
      o << new VMInstruction(CASTF,unsigned_integer,out.index(),a.index());
      return out;
    case double_float:
      o << new VMInstruction(CASTD,unsigned_integer,out.index(),a.index());
      return out;
    }    
  } else if (a.type() == single_float) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,single_float,out.index(),a.index());
      return out;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,single_float,out.index(),a.index());
      return out;
    case integer:
      o << new VMInstruction(CASTI,single_float,out.index(),a.index());
      return out;
    case single_float:
      return a;
    case double_float:
      o << new VMInstruction(CASTD,single_float,out.index(),a.index());
      return out;
    }        
  } else if (a.type() == double_float) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,double_float,out.index(),a.index());
      return out;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,double_float,out.index(),a.index());
      return out;
    case integer:
      o << new VMInstruction(CASTI,double_float,out.index(),a.index());
      return out;
    case single_float:
      o << new VMInstruction(CASTF,double_float,out.index(),a.index());
      return out;
    case double_float:
      return a;
    }            
  }
  throw Exception("Illegal type promotion call.");
}

RegisterReference JITBooleanOrOperator(VMStream& o, tree t, Interpreter* m_eval) {
  RegisterReference result(o.GetReg(),t_boolean);
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference atest(JITPromote(o,a,t_boolean));
  VMInstruction *jump_a_true = new VMInstruction(JIT,unsigned_integer,
						 atest.index(),
						 RTUnsigned(0));
  o << jump_a_true;
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  RegisterReference btest(JITPromote(o,b,t_boolean));
  VMInstruction *jump_b_true = new VMInstruction(JIT,unsigned_integer,
						 btest.index(),
						 RTUnsigned(0));
  o << jump_b_true;
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(false));
  VMInstruction *jump_done = new VMInstruction(JMP,unsigned_integer,
					       RTUnsigned(o.size()+2));
  o << jump_done;
  jump_a_true->literal = RTUnsigned(o.size());
  jump_b_true->literal = RTUnsigned(o.size());
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(true));
  o << new VMInstruction(NOP);
  return result;
}

RegisterReference JITBooleanNotOperator(VMStream& o, tree t, Interpreter* m_eval) {
  RegisterReference result(o.GetReg(),t_boolean);
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference atest(JITPromote(o,a,t_boolean));
  o << new VMInstruction(NOT,t_boolean,result.index(),atest.index());
  return result;
}

RegisterReference JITBooleanAndOperator(VMStream& o, tree t, Interpreter* m_eval) {
  RegisterReference result(o.GetReg(),t_boolean);
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference atest(JITPromote(o,a,t_boolean));
  VMInstruction *jump_a_false = new VMInstruction(JIF,unsigned_integer,
						  atest.index(),
						  RTUnsigned(0));
  o << jump_a_false;
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  RegisterReference btest(JITPromote(o,b,t_boolean));
  VMInstruction *jump_b_false = new VMInstruction(JIF,unsigned_integer,
						  btest.index(),
						  RTUnsigned(0));
  o << jump_b_false;
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(true));
  VMInstruction *jump_done = new VMInstruction(JMP,unsigned_integer,
					       RTUnsigned(o.size()+2));
  o << jump_done;
  jump_a_false->literal = RTUnsigned(o.size());
  jump_b_false->literal = RTUnsigned(o.size());
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(false));
  o << new VMInstruction(NOP);
  return result;
}

RegisterReference JITComparisonOperator(VMStream& o, tree t, opcodeType op, Interpreter* m_eval) {
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  opcodeClass outputClass;
  if (a.type() > b.type()) 
    outputClass = a.type();
  else
    outputClass = b.type();
  if (a.type() != outputClass)
    a = JITPromote(o,a,outputClass);
  if (b.type() != outputClass)
    b = JITPromote(o,b,outputClass);
  RegisterReference c(o.GetReg(),t_boolean);
  o << new VMInstruction(op,t_boolean,c.index(),a.index(),b.index());
  return c;
}

RegisterReference JITUnaryOperator(VMStream& o, tree t, opcodeType op, Interpreter* m_eval) {
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference c(o.GetReg(),a.type());
  o << new VMInstruction(op,c.type(),c.index(),a.index());
  return c;
}

RegisterReference JITBinaryOperator(VMStream& o, tree t, opcodeType op, Interpreter* m_eval) {
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  opcodeClass outputClass;
  if (a.type() > b.type()) 
    outputClass = a.type();
  else
    outputClass = b.type();
  if ((op == LDIV) || (op == RDIV) && (outputClass == integer))
    outputClass = double_float;
  if (a.type() != outputClass)
    a = JITPromote(o,a,outputClass);
  if (b.type() != outputClass)
    b = JITPromote(o,b,outputClass);
  RegisterReference c(o.GetReg(),outputClass);
  o << new VMInstruction(op,c.type(),c.index(),a.index(),b.index());
  return c;
}

SymbolInformation* JITAddArgument(VMStream& o, string name, Interpreter*m_eval, bool scalarVariable) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
  if (!ptr.valid())
    throw Exception("Undefined variable reference:" + name);
  if (!ptr->is2D())
    throw Exception("Cannot JIT multi-dimensional array:" + name);
  if (ptr->isString() || ptr->isReferenceType())
    throw Exception("Cannot JIT strings or reference types:" + name);
  if (ptr->isComplex())
    throw Exception("Cannot JIT complex arrays:" + name);
  if (!ptr->isScalar() && scalarVariable)
    throw Exception("JIT requires " + name + " be a scalar");
  opcodeClass q_type;
  switch (ptr->dataClass()) {
  case FM_FUNCPTR_ARRAY:
  case FM_CELL_ARRAY:
  case FM_STRUCT_ARRAY:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT64:
  case FM_INT64:
  case FM_COMPLEX:
  case FM_DCOMPLEX:
  case FM_STRING:
    throw Exception("Cannot JIT code because of type of :" + name);
  case FM_LOGICAL:
    q_type = t_boolean;
    break;
  case FM_UINT32:
    q_type = unsigned_integer;
    break;
  case FM_INT32:
    q_type = integer;
    break;
  case FM_FLOAT:
    q_type = single_float;
    break;
  case FM_DOUBLE:
    q_type = double_float;
  }
  // Allocate a symbol for the variable
  SymbolInformation sym;
  sym.m_rows = RegisterReference(o.GetReg(),unsigned_integer);
  sym.m_cols = RegisterReference(o.GetReg(),unsigned_integer);
  sym.m_type = q_type;
  if (!scalarVariable)
    sym.m_base = RegisterReference(o.GetReg(),pointer);
  else
    sym.m_base = RegisterReference(o.GetReg(),q_type);
  sym.m_argument = true;
  sym.m_scalar = scalarVariable;
  sym.m_readonly = true;
  o.add_symbol(name,sym);
  return o.find_symbol(name);
}

RegisterReference JITRHS(VMStream& o, tree t, Interpreter* m_eval) { 
  string symname(t.first().text());
  SymbolInformation *v = o.find_symbol(symname);
  if (!v) 
    // Try to look up the symbol in the current context
    v = JITAddArgument(o,symname,m_eval,t.numchildren() == 1);
  if (t.numchildren() == 1) {
    if (!v->isScalar()) 
      throw Exception("Non-scalar reference returned in scalar context!");
    return v->m_base;
  }
  if (t.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar())
    throw Exception("array indexing of scalar values...");
  tree s(t.second());
  if (!s.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s.numchildren() == 1) {
    RegisterReference offset_n(JITExpression(o,s.first(),m_eval));
    RegisterReference offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,offset_n.type(),offset_u.index(),offset_n.index());
    o << new VMInstruction(DEC,offset_u.type(),offset_u.index());
    RegisterReference val(o.GetReg(),v->m_type);
    o << new VMInstruction(LOAD,v->m_type,val.index(),v->m_base.index(),offset_u.index());
    return val;
  } else if (s.numchildren() == 2) {
    RegisterReference row_offset_n(JITExpression(o,s.first(),m_eval));
    RegisterReference row_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,row_offset_n.type(),row_offset_u.index(),row_offset_n.index());
    o << new VMInstruction(DEC,row_offset_u.type(),row_offset_u.index());
    RegisterReference col_offset_n(JITExpression(o,s.second(),m_eval));
    RegisterReference col_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,col_offset_n.type(),col_offset_u.index(),col_offset_n.index());
    o << new VMInstruction(DEC,col_offset_u.type(),col_offset_u.index());    
    RegisterReference ndx_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(MUL,unsigned_integer,ndx_offset_u.index(),
			   col_offset_u.index(),v->m_rows.index());
    o << new VMInstruction(ADD,unsigned_integer,ndx_offset_u.index(),
			   ndx_offset_u.index(),row_offset_u.index());
    RegisterReference val(o.GetReg(),v->m_type);
    o << new VMInstruction(LOAD,v->m_type,val.index(),v->m_base.index(),ndx_offset_u.index());
    return val;
  }
  throw Exception("dereference not handled yet...");
}


RegisterReference JITExpression(VMStream& o, tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return JITRHS(o,t,m_eval);
  case TOK_INTEGER: {
    RegisterReference q(o.GetReg(),integer);
    o << new VMInstruction(SET,integer,q.index(),
			   RTInteger(ArrayToInt32(t.array())));
    return q;
  }
  case TOK_FLOAT: {
    RegisterReference q(o.GetReg(),single_float);
    o << new VMInstruction(SET,single_float,q.index(),
			   RTFloat(ArrayToDouble(t.array())));
    return q;
  }
  case TOK_DOUBLE: {
    RegisterReference q(o.GetReg(),double_float);
    o << new VMInstruction(SET,double_float,q.index(),
			   RTDouble(ArrayToDouble(t.array())));
    return q;
  }
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF: 
    throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case '+': 
    return JITBinaryOperator(o,t,ADD,m_eval);
  case '-': 
    return JITBinaryOperator(o,t,SUB,m_eval);
  case '*': 
  case TOK_DOTTIMES: 
    return JITBinaryOperator(o,t,MUL,m_eval);
  case '/': 
  case TOK_DOTRDIV: 
    return JITBinaryOperator(o,t,RDIV,m_eval);
  case '\\': 
  case TOK_DOTLDIV: 
    return JITBinaryOperator(o,t,LDIV,m_eval);
  case TOK_SOR: 
  case '|': 
    return JITBooleanOrOperator(o,t,m_eval);
  case TOK_SAND: 
  case '&': 
    return JITBooleanAndOperator(o,t,m_eval);
  case '<': 
    return JITComparisonOperator(o,t,LT,m_eval);
  case TOK_LE: 
    return JITComparisonOperator(o,t,LE,m_eval);
  case '>': 
    return JITComparisonOperator(o,t,GT,m_eval);
  case TOK_GE: 
    return JITComparisonOperator(o,t,GE,m_eval);
  case TOK_EQ: 
    return JITComparisonOperator(o,t,EQ,m_eval);
  case TOK_NE: 
    return JITComparisonOperator(o,t,NEQ,m_eval);
  case TOK_UNARY_MINUS: 
    return JITUnaryOperator(o,t,NEG,m_eval);
  case TOK_UNARY_PLUS: 
    return JITUnaryOperator(o,t,POS,m_eval);
  case '~': 
    return JITBooleanNotOperator(o,t,m_eval);
    break;
  case '^': 
    throw Exception("^ is not currently handled by the JIT compiler");
    break;
  case TOK_DOTPOWER: 
    throw Exception(".^ is not currently handled by the JIT compiler");
    break;
  case '\'': 
    throw Exception("' is not currently handled by the JIT compiler");
    break;
  case TOK_DOTTRANSPOSE: 
    throw Exception(".' is not currently handled by the JIT compiler");
    break;
  case '@':
    throw Exception("@ is not currently handled by the JIT compiler");
  default:
    std::cout << "******************************************************\r\n";
    t.print();
    throw Exception("Unrecognized expression!");
  }
}

void JITBlock(VMStream& o, tree t, Interpreter* m_eval);

void JITIfStatement(VMStream& o, tree t, Interpreter* m_eval) {
  VMStream endInstructions;
  RegisterReference test(JITExpression(o,t.first(),m_eval));
  VMInstruction *jmp1 = new VMInstruction(JIF,unsigned_integer,
					  test.index(),RTUnsigned(0));
  o << jmp1;
  VMInstruction *prev_fixup = jmp1;
  JITBlock(o,t.second(),m_eval);
  VMInstruction *jmp2 = new VMInstruction(JMP,unsigned_integer,
					  RTUnsigned(0));
  o << jmp2;
  endInstructions << jmp2;
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    prev_fixup->literal = RTUnsigned(o.size());
    RegisterReference ttest(JITExpression(o,t.child(n).first(),m_eval));
    VMInstruction *jmpn = new VMInstruction(JIF,unsigned_integer,
					    ttest.index(),RTUnsigned(0));
    o << jmpn;
    prev_fixup = jmpn;
    JITBlock(o,t.child(n).second(),m_eval);
    VMInstruction *jmpp = new VMInstruction(JMP,unsigned_integer,
					    RTUnsigned(0));
    o << jmpp;
    endInstructions << jmpp;
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    prev_fixup->literal = RTUnsigned(o.size());
    JITBlock(o,t.last().first(),m_eval);
  }
  int end_address = o.size();
  for (int i=0;i<endInstructions.size();i++)
    endInstructions[i]->literal = RTUnsigned(end_address);
  o << new VMInstruction(NOP);
}

// what happens with:
//   if (b>0)
//     a = 3.1;
//   else
//     a = 5;
//   end
// ??   When we come out of the if statement, what type is a  ??

void JITAssignment(VMStream& o, tree t, Interpreter* m_eval) {
  tree s(t.first());
  string symname(s.first().text());
  SymbolInformation *v = o.find_symbol(symname);
  if (!v) 
    v = JITAddArgument(o,symname,m_eval,s.numchildren() == 1);
  v->m_readonly = false;
  // evaluate the RHS of the assignment
  RegisterReference RHS(JITExpression(o,t.second(),m_eval));
  if (v->m_type != RHS.type())
    throw Exception("polymorphic assignment to scalar detected.");
  if (s.numchildren() == 1) {
    if (!v->isScalar())
      throw Exception("scalar assignment to array variable.");
    o << new VMInstruction(COPY,RHS.type(),v->m_base.index(),RHS.index());
  }
  if (s.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar())
    throw Exception("array indexing of scalar values...");
  tree q(s.second());
  if (!q.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q.numchildren() == 1) {
    RegisterReference offset_n(JITExpression(o,q.first(),m_eval));
    RegisterReference offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,offset_n.type(),offset_u.index(),offset_n.index());
    o << new VMInstruction(DEC,offset_u.type(),offset_u.index());
    o << new VMInstruction(STORE,v->m_type,v->m_base.index(),RHS.index(),offset_u.index());
  } else if (s.numchildren() == 2) {
    RegisterReference row_offset_n(JITExpression(o,q.first(),m_eval));
    RegisterReference row_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,row_offset_n.type(),row_offset_u.index(),row_offset_n.index());
    o << new VMInstruction(DEC,row_offset_u.type(),row_offset_u.index());
    RegisterReference col_offset_n(JITExpression(o,q.second(),m_eval));
    RegisterReference col_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,col_offset_n.type(),col_offset_u.index(),col_offset_n.index());
    o << new VMInstruction(DEC,col_offset_u.type(),col_offset_u.index());    
    RegisterReference ndx_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(MUL,unsigned_integer,ndx_offset_u.index(),
			   col_offset_u.index(),v->m_rows.index());
    o << new VMInstruction(ADD,unsigned_integer,ndx_offset_u.index(),
			   ndx_offset_u.index(),row_offset_u.index());
    o << new VMInstruction(STORE,v->m_type,v->m_base.index(),RHS.index(),ndx_offset_u.index());
  }
}

void JITForLoop(VMStream& o, tree t, Interpreter* m_eval);

void JITStatementType(VMStream& o, tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case '=': 
    JITAssignment(o,t,m_eval);
    break;
  case TOK_MULTI:
    throw Exception("multi function calls do not JIT compile");
  case TOK_SPECIAL:
    throw Exception("special function calls do not JIT compile");
  case TOK_FOR:
    JITForLoop(o,t,m_eval);
    break;
  case TOK_WHILE:
    throw Exception("nested while loops do not JIT compile");
  case TOK_IF:
    JITIfStatement(o,t,m_eval);
    break;
  case TOK_BREAK:
    throw Exception("break is not currently handled by the JIT compiler");
    break;
  case TOK_CONTINUE:
    throw Exception("continue is not currently handled by the JIT compiler");
    break;
  case TOK_DBSTEP:
    throw Exception("dbstep is not currently handled by the JIT compiler");
    break;
  case TOK_DBTRACE:
    throw Exception("dbtrace is not currently handled by the JIT compiler");
    break;
  case TOK_RETURN:
    throw Exception("return is not currently handled by the JIT compiler");
    break;
  case TOK_SWITCH:
    throw Exception("switch is not currently handled by the JIT compiler");
    break;
  case TOK_TRY:
    throw Exception("try is not currently handled by the JIT compiler");
    break;
  case TOK_QUIT:
    throw Exception("quit is not currently handled by the JIT compiler");
    break;
  case TOK_RETALL:
    throw Exception("retall is not currently handled by the JIT compiler");
    break;
  case TOK_KEYBOARD:
    throw Exception("keyboard is not currently handled by the JIT compiler");
    break;
  case TOK_GLOBAL:
    throw Exception("global is not currently handled by the JIT compiler");
    break;
  case TOK_PERSISTENT:
    throw Exception("persistent is not currently handled by the JIT compiler");
    break;
  case TOK_EXPR:
    JITExpression(o,t.first(),m_eval);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void JITStatement(VMStream& o, tree t, Interpreter* m_eval) {
  // ignore tok_qstatement/tok_statement
  JITStatementType(o,t.first(),m_eval);
}

void JITBlock(VMStream& o, tree t, Interpreter* m_eval) {
  const treeVector &statements(t.children());
  for (treeVector::const_iterator i=statements.begin();
       i!=statements.end();i++) 
    JITStatement(o,*i,m_eval);
}

void JITForLoop(VMStream& o, tree t, Interpreter* m_eval) {
  if (!(t.first().is('=') && t.first().second().is(':') &&
	t.first().second().first().is(TOK_INTEGER) &&
	t.first().second().second().is(TOK_INTEGER))) 
    throw Exception("For loop cannot be compiled - need integer bounds");
  int loop_start(atoi(t.first().second().first().text().c_str()));
  int loop_stop(atoi(t.first().second().second().text().c_str()));
  string loop_index(t.first().first().text());
  unsigned loop_index_register = o.GetReg();
  o.add_symbol(loop_index,
		     SymbolInformation(integer,loop_index_register));
  o.push_set(loop_index_register,JITScalar(loop_start));
  unsigned loop_start_instruction = o.size();
  JITBlock(o,t.second(),m_eval);
  o.push_add(loop_index_register,loop_index_register,JITScalar(1));
  o.push_le(loop_test_register,loop_index_register,JITScalar(loop_stop));
  o.push_jit(loop_test_register,JITScalar(-o.size()+loop_start_instruction));

  o << new VMInstruction(LE,integer,loop_test_register,
			       loop_index_register,loop_max_register);
  o << new VMInstruction(JIT,unsigned_integer,
			       loop_test_register,
			       RTUnsigned(loop_start_instruction));
}

bool ScalarRequirements(tree t, LoopSignature &sig) {
  if (t.is(TOK_VARIABLE)) {
    if (t.numchildren() > 2) return false;
    if (t.numchildren() == 1) {
      sig.add_scalar(t.first().text());
    } else {
      if (!t.second().is(TOK_PARENS)) 
	return false;
      else {
	tree s(t.second());
	for (int i=0;i<s.numchildren();i++)
	  if (s.child(i).is(':'))
	    return false;
      }
      sig.add_matrix(t.first().text());
      if (!ScalarRequirements(t.second(),sig))
	return false;
    }
  } else {
    for (int i=0;i<t.numchildren();i++) 
      if (!ScalarRequirements(t.child(i),sig))
	return false;
  }
  return true;
}

VMStream CompileForBlock(const tree t, Interpreter* m_eval) {
  VMStream o;
  JITForLoop(o,t,m_eval);
  return o;
}

void LoadCompileFunction(Context* context) {
  context->addSpecialFunction("fcc",fccFunction,-1,0); 
}
