// Still need:

//  Type promotion on loop entry.
//  Range checking.
//  Dynamic array resizing.
//

//
// Just for fun, mind you....
//

#define __STDC_LIMIT_MACROS

#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/ModuleProvider.h"
#include "llvm/CallingConv.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/GenericValue.h"

#include "JITVM.hpp"

using namespace llvm;


// Instruction rules:
// All instructions are tri-ops:   instr type dest op1 op2
//    the destination _must_ be a register

JITVM::JITVM() {
  next_reg = 0;
  // Initialize the op table.
  for (int i=0;i<op_end;i++) {
    opcode_table_b[i] = &JITVM::illegal_op_bool;
    opcode_table_i[i] = &JITVM::illegal_op_int32;
    opcode_table_f[i] = &JITVM::illegal_op_float;
    opcode_table_d[i] = &JITVM::illegal_op_double;
  }
  // Initialize ADD
  opcode_table_i[op_add] = &JITVM::add_func<int32>;
  opcode_table_f[op_add] = &JITVM::add_func<float>;
  opcode_table_d[op_add] = &JITVM::add_func<double>;
  // Initialize SUB
  opcode_table_i[op_sub] = &JITVM::sub_func<int32>;
  opcode_table_f[op_sub] = &JITVM::sub_func<float>;
  opcode_table_d[op_sub] = &JITVM::sub_func<double>;
  // Initialize MUL
  opcode_table_i[op_mul] = &JITVM::mul_func<int32>;
  opcode_table_f[op_mul] = &JITVM::mul_func<float>;
  opcode_table_d[op_mul] = &JITVM::mul_func<double>;
  // Initialize DIV
  opcode_table_f[op_div] = &JITVM::div_func<float>;
  opcode_table_d[op_div] = &JITVM::div_func<double>;
  // Initialize BOOL
  opcode_table_b[op_or] = &JITVM::or_func;
  opcode_table_b[op_and] = &JITVM::and_func;
  opcode_table_b[op_not] = &JITVM::not_func;
  // Initialize COMPARISON OPS
  opcode_table_i[op_lt] = &JITVM::lt_func<int32>;
  opcode_table_f[op_lt] = &JITVM::lt_func<float>;
  opcode_table_d[op_lt] = &JITVM::lt_func<double>;
  opcode_table_i[op_le] = &JITVM::le_func<int32>;
  opcode_table_f[op_le] = &JITVM::le_func<float>;
  opcode_table_d[op_le] = &JITVM::le_func<double>;
  opcode_table_i[op_gt] = &JITVM::gt_func<int32>;
  opcode_table_f[op_gt] = &JITVM::gt_func<float>;
  opcode_table_d[op_gt] = &JITVM::gt_func<double>;
  opcode_table_i[op_ge] = &JITVM::ge_func<int32>;
  opcode_table_f[op_ge] = &JITVM::ge_func<float>;
  opcode_table_d[op_ge] = &JITVM::ge_func<double>;
  opcode_table_i[op_ne] = &JITVM::ne_func<int32>;
  opcode_table_f[op_ne] = &JITVM::ne_func<float>;
  opcode_table_d[op_ne] = &JITVM::ne_func<double>;
  opcode_table_i[op_eq] = &JITVM::eq_func<int32>;
  opcode_table_f[op_eq] = &JITVM::eq_func<float>;
  opcode_table_d[op_eq] = &JITVM::eq_func<double>;
  // Set Ops
  opcode_table_b[op_set] = &JITVM::set_func<bool>;
  opcode_table_i[op_set] = &JITVM::set_func<int32>;
  opcode_table_f[op_set] = &JITVM::set_func<float>;
  opcode_table_d[op_set] = &JITVM::set_func<double>;
  // Cast Ops - to Bool
  opcode_table_i[op_castb] = &JITVM::castb_func<int32>;
  opcode_table_f[op_castb] =  &JITVM::castb_func<float>;
  opcode_table_d[op_castb] = &JITVM::castb_func<double>;
  // Cast Ops - to Int
  opcode_table_b[op_casti] = &JITVM::casti_b_func;
  opcode_table_f[op_casti] = &JITVM::casti_func<float>;
  opcode_table_d[op_casti] = &JITVM::casti_func<double>;
  // Cast Ops - to Float
  opcode_table_b[op_castf] = &JITVM::castf_b_func;
  opcode_table_i[op_castf] = &JITVM::castf_func<int32>;
  opcode_table_d[op_castf] = &JITVM::castf_func<double>;
  // Cast Ops - to Double
  opcode_table_b[op_castd] = &JITVM::castd_b_func;
  opcode_table_i[op_castd] = &JITVM::castd_func<int32>;
  opcode_table_f[op_castd] = &JITVM::castd_func<float>;
  // NOP
  opcode_table_b[op_nop] = &JITVM::nop_func<bool>;
  opcode_table_i[op_nop] = &JITVM::nop_func<int32>;
  opcode_table_f[op_nop] = &JITVM::nop_func<float>;
  opcode_table_d[op_nop] = &JITVM::nop_func<double>;
  // JIT
  opcode_table_i[op_jit] = &JITVM::jit_func;
  opcode_table_i[op_jif] = &JITVM::jif_func;
  opcode_table_i[op_jmp] = &JITVM::jmp_func;
 }

void JITVM::illegal_op_bool(int32, bool, bool) {
  throw Exception("illegal op encountered with boolean type");
}

void JITVM::illegal_op_int32(int32, int32, int32) {
  throw Exception("illegal op encountered with integer  type");
}

void JITVM::illegal_op_float(int32, float, float) {
  throw Exception("illegal op encountered with float type");
}

void JITVM::illegal_op_double(int32, double, double) {
  throw Exception("illegal op encountered with double type");
}

void JITVM::set_matrix_arg(int32 row, int32 col) {
  m_row = row;
  m_col = col;
}

void JITVM::dispatch(const JITInstruction &inst) {
  JITScalar d_arg1, d_arg2;
  switch (inst.ri) {
  case arg_rr:
    d_arg1 = reg[inst.arg1.reg()];
    d_arg2 = reg[inst.arg2.reg()];
    break;
  case arg_ri:
    d_arg1 = reg[inst.arg1.reg()];
    d_arg2 = inst.arg2;
    break;
  case arg_ir:
    d_arg1 = inst.arg1;
    d_arg2 = reg[inst.arg2.reg()];
    break;
  case arg_ii:
    d_arg1 = inst.arg1;
    d_arg2 = inst.arg2;
    break;
  }
  // Special case the load/store ops
  if (inst.op == op_mset)
    set_matrix_arg(d_arg1.i(),d_arg2.i());
  else if (inst.op == op_vload)
    dispatch_vload(inst.type,inst.dest,d_arg1.p(),d_arg2.i());
  else if (inst.op == op_mload)
    dispatch_mload(inst.type,inst.dest,d_arg1.p());
  else if (inst.op == op_vstore)
    dispatch_vstore(inst.type,inst.dest.p(),d_arg1.i(),d_arg2);
  else if (inst.op == op_mstore)
    dispatch_mstore(inst.type,inst.dest.p(),d_arg1);
  else {
    switch (inst.type) {
    case type_b:
      (this->*opcode_table_b[inst.op])(inst.dest.reg(),d_arg1.b(),d_arg2.b());
      break;
    case type_i:
      (this->*opcode_table_i[inst.op])(inst.dest.reg(),d_arg1.i(),d_arg2.i());
      break;
    case type_f:
      (this->*opcode_table_f[inst.op])(inst.dest.reg(),d_arg1.f(),d_arg2.f());
      break;
    case type_d:
      (this->*opcode_table_d[inst.op])(inst.dest.reg(),d_arg1.d(),d_arg2.d());
      break;
    }
  }
}

void JITVM::dispatch_mstore(op_type type, void *ptr, JITScalar dest) {
  Array *a = (Array*) ptr;
  if ((m_row <= a->rows()) && (m_row >= 1) && (m_col <= a->columns()) && (m_col >= 1)) {
    switch (type) {
    case type_b:
      ((logical*) a->getReadWriteDataPointer())[m_row-1+(m_col-1)*a->rows()] = 
	dest.b();
      break;
    case type_i:
      ((int32*) a->getReadWriteDataPointer())[m_row-1+(m_col-1)*a->rows()] = 
	dest.i();
      break;
    case type_f:
      ((float*) a->getReadWriteDataPointer())[m_row-1+(m_col-1)*a->rows()] = 
	dest.f();
      break;
    case type_d:
      ((double*) a->getReadWriteDataPointer())[m_row-1+(m_col-1)*a->rows()] = 
	dest.d();
      break;
    }
  } else {
    throw Exception("Out of range access");
  }
}

void JITVM::dispatch_vstore(op_type type, void *ptr, int32 row, JITScalar dest) {
  Array *a = (Array*) ptr;
  if ((row <= (a->rows()*a->columns())) && (row >= 1)) {
    switch (type) {
    case type_b:
      ((logical*) a->getReadWriteDataPointer())[row-1] = dest.b();
      break;
    case type_i:
      ((int32*) a->getReadWriteDataPointer())[row-1] = dest.i();
      break;
    case type_f:
      ((float*) a->getReadWriteDataPointer())[row-1] = dest.f();
      break;
    case type_d:
      ((double*) a->getReadWriteDataPointer())[row-1] = dest.d();
      break;
    }
  } else {
    throw Exception("Out of range access");
  }
}

void JITVM::dispatch_mload(op_type type, JITScalar dest, void *ptr) {
  Array *a = (Array*) ptr;
  if ((m_row <= a->rows()) && (m_row >= 1) && (m_col <= a->columns()) && (m_col >= 1)) {
    switch (type) {
    case type_b:
      reg[dest.reg()].set((bool) ((logical*) a->getDataPointer())[m_row-1+(m_col-1)*a->rows()]);
      break;
    case type_i:
      reg[dest.reg()].set((int32) ((int32*) a->getDataPointer())[m_row-1+(m_col-1)*a->rows()]);
      break;
    case type_f:
      reg[dest.reg()].set((float) ((float*) a->getDataPointer())[m_row-1+(m_col-1)*a->rows()]);
      break;
    case type_d:
      reg[dest.reg()].set((double) ((double*) a->getDataPointer())[m_row-1+(m_col-1)*a->rows()]);
      break;
    }
  } else {
    throw Exception("Out of range access");
  }
}

void JITVM::dispatch_vload(op_type type, JITScalar dest, void *ptr, int row) {
  Array *a = (Array*) ptr;
  if ((row <= (a->rows()*a->columns())) && (row >= 1)) {
    switch (type) {
    case type_b:
      reg[dest.reg()].set((bool) ((logical*) a->getDataPointer())[row-1]);
      break;
    case type_i:
      reg[dest.reg()].set((int32) ((int32*) a->getDataPointer())[row-1]);
      break;
    case type_f:
      reg[dest.reg()].set((float) ((float*) a->getDataPointer())[row-1]);
      break;
    case type_d:
      reg[dest.reg()].set((double) ((double*) a->getDataPointer())[row-1]);
      break;
    }
  } else {
    throw Exception("Out of range access");
  }
}

op_type JITVM::compute_oc_code(scalar_class outClass) {
  switch (outClass) {
  case c_bool:
    return type_b;
  case c_int32:
    return type_i;
  case c_float:
    return type_f;
  case c_double:
    return type_d;
  default:
    throw Exception("unable to compute output code");
  }
}

op_ri JITVM::compute_ri_code(JITScalar arg1) {
  return compute_ri_code(arg1,arg1);
}

op_ri JITVM::compute_ri_code(JITScalar arg1, JITScalar arg2) {
  if ((arg1.type() == c_register) && (arg2.type() == c_register)) {
    return arg_rr;
  } else if ((arg1.type() == c_register) && (arg2.type() < c_register)) {
    return arg_ri;
  } else if ((arg1.type() < c_register) && (arg2.type() == c_register)) {
    return arg_ir;
  } else if ((arg1.type() < c_register) && (arg2.type() < c_register)) {
    return arg_ii;
  }
  throw Exception("computation for register/immediate code stalled");
}

scalar_class JITVM::type_of(JITScalar arg) {
  if (arg.type() <= c_pointer) 
    return arg.type();
  if (arg.type() == c_unknown) 
    throw Exception("encountered uninitialized scalar in compilation!");
  if (arg.type() == c_register) 
    return type_of(reg[arg.reg()]);
}
 
bool JITVM::isscalar(JITScalar arg) {
  return (type_of(arg) != c_pointer);
}

JITScalar JITVM::new_register(scalar_class outClass) {
  reg[next_reg].setType(outClass);
  return JITScalar((uint32) next_reg++);
}

void JITVM::push_instruction(opcode op, op_type type, JITScalar result, 
			     op_ri ri_code, JITScalar arg1) {
  data.push_back(JITInstruction(op,type,result,ri_code,arg1));
}

void JITVM::push_instruction(opcode op, op_type type, JITScalar result, 
			     op_ri ri_code, JITScalar arg1, JITScalar arg2) {
  data.push_back(JITInstruction(op,type,result,ri_code,arg1,arg2));
}

void JITVM::push_instruction(opcode op, op_type type, JITScalar result, 
			     op_ri ri_code, JITScalar arg1, JITScalar arg2, 
			     JITScalar arg3) {
  data.push_back(JITInstruction(op,type,result,ri_code,arg1,arg2,arg3));
}

static string dump_literal(JITScalar t) {
  char buffer[1024];
  switch (t.type()) {
  case c_bool: 
    {
      if (t.b()) 
	return "true"; 
      else 
	return "false";
    }
  case c_int32: 
    {
      sprintf(buffer,"%i",t.i());
      return buffer;
    }
  case c_float:
    {
      sprintf(buffer,"%f",t.f());
      return buffer;
    }
  case c_double:
    {
      sprintf(buffer,"%f",t.d());
      return buffer;
    }
  case c_register:
    {
      return "***";
    }
  case c_pointer:
    {
      sprintf(buffer,"%p",t.p());
      return buffer;
    }
  }
  return "unknown";
}

static bool binary_op(opcode op) {
  switch(op) {
  case op_add:   
  case op_sub:   
  case op_mul:   
  case op_div:   
  case op_or:    
  case op_and:   
  case op_lt:    
  case op_le:    
  case op_eq:    
  case op_ge:    
  case op_gt:    
  case op_ne:    
  case op_jit:
  case op_jif:
  case op_vload: 
  case op_vstore:
    return true;
  case op_not:   
  case op_neg:   
  case op_set:   
  case op_castb: 
  case op_casti: 
  case op_castf: 
  case op_castd: 
  case op_nop:   
  case op_mload: 
  case op_mstore:
    return false;
  }
}

static bool unary_op(opcode op) {
  switch(op) {
  case op_add:   
  case op_sub:   
  case op_mul:   
  case op_div:   
  case op_or:    
  case op_and:   
  case op_lt:    
  case op_le:    
  case op_eq:    
  case op_ge:    
  case op_gt:    
  case op_ne:    
  case op_jit:   
  case op_vload: 
  case op_vstore:
  case op_mset:
    return false;
  case op_not:   
  case op_neg:   
  case op_set:   
  case op_castb: 
  case op_casti: 
  case op_castf: 
  case op_castd: 
  case op_nop:   
  case op_mload: 
  case op_mstore:
    return true;
  }
}

void JITInstruction::dump(ostream& o) {
  switch(op) {
  case op_add:    o << "add   "; break;
  case op_sub:    o << "sub   "; break;
  case op_mul:    o << "mul   "; break;
  case op_div:    o << "div   "; break;
  case op_or:     o << "or    "; break;
  case op_and:    o << "and   "; break;
  case op_not:    o << "not   "; break;
  case op_lt:     o << "lt    "; break;
  case op_le:     o << "le    "; break;
  case op_eq:     o << "eq    "; break;
  case op_ge:     o << "ge    "; break;
  case op_gt:     o << "gt    "; break;
  case op_ne:     o << "ne    "; break;
  case op_neg:    o << "neg   "; break;
  case op_set:    o << "set   "; break;
  case op_castb:  o << "castb "; break;
  case op_casti:  o << "casti "; break;
  case op_castf:  o << "castf "; break;
  case op_castd:  o << "castd "; break;
  case op_nop:    o << "nop   "; break;
  case op_jit:    o << "jit   "; break;
  case op_jif:    o << "jif   "; break;
  case op_mload:  o << "mload "; break;
  case op_vload:  o << "vload "; break;
  case op_mstore: o << "mstore"; break;
  case op_vstore: o << "vstore"; break;
  case op_mset:   o << "mset  "; break;
  }
  switch(type) {
  case type_b:    o << "<b> "; break;
  case type_i:    o << "<i> "; break;
  case type_f:    o << "<f> "; break;
  case type_d:    o << "<d> "; break;
  }
  
  if (op == op_vstore) {
    o << dump_literal(dest) << ",";
    switch(ri) {
    case arg_rr:
      o << "r"<< arg1.reg() << ",r" << arg2.reg(); break;
    case arg_ri:
      o << "r"<< arg1.reg() << "," << dump_literal(arg2); break;
    case arg_ir:
      o << dump_literal(arg1) << ",r" << arg2.reg(); break;
    case arg_ii:
      o << dump_literal(arg1) << "," << dump_literal(arg2); break;
    }
  } else if (binary_op(op)) {
    o << "r" << dest.reg() << ",";
    switch(ri) {
    case arg_rr:
      o << "r"<< arg1.reg() << ",r" << arg2.reg(); break;
    case arg_ri:
      o << "r"<< arg1.reg() << "," << dump_literal(arg2); break;
    case arg_ir:
      o << dump_literal(arg1) << ",r" << arg2.reg(); break;
    case arg_ii:
      o << dump_literal(arg1) << "," << dump_literal(arg2); break;
    }
  } else if (unary_op(op)) {
    o << "r" << dest.reg() << ",";
    switch(ri) {
    case arg_rr:
    case arg_ri:
      o << "r"<< arg1.reg(); break;
    case arg_ir:
    case arg_ii:
      o << dump_literal(arg1); break;
    }
  }
}

JITScalar JITVM::promote(JITScalar arg, scalar_class outClass) {
  // For scalar types, just promote the thing
  if (arg.type() < c_pointer) {
    if (arg.type() == c_bool) {
      switch (outClass) {
      case c_bool:
	return arg;
      case c_int32:
	if (arg.b())
	  return JITScalar((int32) 1);
	else
	  return JITScalar((int32) 0);
      case c_float:
	if (arg.b())
	  return JITScalar((float) 1);
	else
	  return JITScalar((float) 0);
      case c_double:
	if (arg.b())
	  return JITScalar((double) 1);
	else
	  return JITScalar((double) 0);
      default:
	throw Exception("unhandled case for promote called from bool");
      }
    } else if (arg.type() == c_int32) {
      switch (outClass) {
      case c_bool:
	return JITScalar(arg.i() != 0);
      case c_int32:
	return arg;
      case c_float:
	return JITScalar((float) arg.i());
      case c_double:
	return JITScalar((double) arg.i());
      default:
	throw Exception("unhandled case for promote called from int32");
      }
    } else if (arg.type() == c_float) {
      switch (outClass) {
      case c_bool:
	return JITScalar(arg.f() != 0);
      case c_int32:
	return JITScalar((int) arg.f());
      case c_float:
	return arg;
      case c_double:
	return JITScalar((double) arg.f());
      default:
	throw Exception("unhandled case for promote called from float");
      }
    } else if (arg.type() == c_double) {
      switch (outClass) {
      case c_bool:
	return JITScalar(arg.d() != 0);
      case c_int32:
	return JITScalar((int) arg.d());
      case c_float:
	return JITScalar((float) arg.d());
      case c_double:
	return arg;
      default:
	throw Exception("unhandled case for promote called from double");
      }
    } else
      throw Exception("unhandled case for promote called");
  }
  if (arg.type() == c_register) {
    if (type_of(arg) == outClass) return arg;
    JITScalar result(new_register(outClass));
    switch (outClass) {
    case c_bool:
      push_instruction(op_castb,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    case c_int32:
      push_instruction(op_casti,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    case c_float:
      push_instruction(op_castf,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    case c_double:
      push_instruction(op_castd,compute_oc_code(reg[arg.reg()].type()),result,arg_rr,arg); break;
    default:
      throw Exception("unhandled case for promote called");
    }
    return result;
  }
  throw Exception("unhandled case for promote called");
}

JITScalar JITVM::boolean_op(opcode op, JITScalar arg1, JITScalar arg2, string inst) {
  arg1 = promote(arg1,c_bool);
  arg2 = promote(arg2,c_bool);
  JITScalar result(new_register(c_bool));
  push_instruction(op,type_b,result,compute_ri_code(arg1,arg2),arg1,arg2);
  return result;
}

bool JITVM::isp(JITScalar x) {
  return (type_of(x) == c_pointer);
}
  
bool JITVM::isi(JITScalar x) {
  return (type_of(x) == c_int32);
}

bool JITVM::isfd(JITScalar x) {
  return ((type_of(x) == c_float) || (type_of(x) == c_double));
}

// add immediate to immediate - this instruction can be replaced by 
// constant folding, but for now, leave it in.
JITScalar JITVM::binary_op(opcode op, JITScalar arg1, JITScalar arg2, string inst) {
  scalar_class outClass;
  outClass = type_of(arg1);
  if (type_of(arg2) > outClass) outClass = type_of(arg2);
  if ((isi(arg1) && isfd(arg2)) || (isi(arg2) && isfd(arg1)))  outClass = c_double;
  if (outClass == c_bool) outClass = c_int32;
  if (outClass >= c_pointer) throw Exception("invalid type argument to " + inst + " instruction");
  arg1 = promote(arg1,outClass);
  arg2 = promote(arg2,outClass);
  JITScalar result(new_register(outClass));
  push_instruction(op,compute_oc_code(outClass),result,compute_ri_code(arg1,arg2),arg1,arg2);
  return result;
}

JITScalar JITVM::comparison_op(opcode op, JITScalar arg1, JITScalar arg2, string inst) {
  scalar_class outClass;
  outClass = type_of(arg1);
  if (type_of(arg2) > outClass) outClass = type_of(arg2);
  if (outClass == c_bool) outClass = c_int32;
  JITScalar result(new_register(c_bool));
  push_instruction(op,compute_oc_code(outClass),result,compute_ri_code(arg1,arg2),arg1,arg2);
  return result;
}

void JITVM::compile_assignment(tree t, Interpreter* m_eval) {
  tree s(t.first());
  string symname(s.first().text());
  JITScalar *v = find_symbol(symname);
  if (!v)
    v = add_argument(symname,m_eval,s.numchildren() == 1);
  JITScalar rhs(compile_expression(t.second(),m_eval));
  if (s.numchildren() == 1) {
    if (type_of(*v) != type_of(rhs))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!isscalar(*v))
      throw Exception("scalar assignment to array variable.");
    push_instruction(op_set,compute_oc_code(type_of(*v)),*v,compute_ri_code(rhs),rhs);
    return;
  }
  if (s.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (isscalar(*v))
    throw Exception("array indexing of scalar values...");
  tree q(s.second());
  if (!q.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q.numchildren() == 1) {
    if (map_array_type(v->p()) != type_of(rhs))
      throw Exception("polymorphic assignment to array detected");
    JITScalar arg1 = promote(compile_expression(q.first(),m_eval),c_int32);
    push_instruction(op_vstore,compute_oc_code(type_of(rhs)),*v,
		     compute_ri_code(arg1,rhs),arg1,rhs);
  } else if (s.numchildren() == 2) {
    if (map_array_type(v->p()) != type_of(rhs))
      throw Exception("polymorphic assignment to array detected");
    JITScalar arg1 = promote(compile_expression(q.first(),m_eval),c_int32);
    JITScalar arg2 = promote(compile_expression(q.second(),m_eval),c_int32);
    push_instruction(op_mset,type_b,JITScalar(),compute_ri_code(arg1,arg2),arg1,arg2);
    push_instruction(op_mstore,compute_oc_code(type_of(rhs)),*v,
		     compute_ri_code(rhs,rhs),rhs);
  }
}

scalar_class JITVM::map_array_type(void* ptr) {
  Array *a = (Array *) ptr;
  switch (a->dataClass()) {
  case FM_FUNCPTR_ARRAY:
  case FM_CELL_ARRAY:
  case FM_STRUCT_ARRAY:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT32:
  case FM_UINT64:
  case FM_INT64:
  case FM_COMPLEX:
  case FM_DCOMPLEX:
  case FM_STRING:
    throw Exception("Unsupported array type");
  case FM_LOGICAL:
    return c_bool;
  case FM_INT32:
    return c_int32;
  case FM_FLOAT:
    return c_float;
  case FM_DOUBLE:
    return c_double;
  }
  throw Exception("Unsupported array type");
}

void JITVM::compile_if_statement(tree t, Interpreter* m_eval) {
  vector<JITInstruction*> endInstructions;
  JITScalar cond(compile_expression(t.first(),m_eval));
  push_instruction(op_jif,type_i,cond,arg_ii,JITScalar((int32)(0)));
  JITInstruction *prev_fixup = &data.back();
  compile_block(t.second(),m_eval);
  push_instruction(op_jmp,type_i,JITScalar(0),arg_ii,JITScalar((int32)(0)));
  endInstructions.push_back(&data.back());
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    prev_fixup->arg1 = JITScalar((int32)(data.size()));
    JITScalar ttest(compile_expression(t.child(n).first(),m_eval));
    push_instruction(op_jif,type_i,ttest,arg_ii,JITScalar((int32) 0));
    prev_fixup = &data.back();
    compile_block(t.child(n).second(),m_eval);
    push_instruction(op_jmp,type_i,JITScalar(0),arg_ii,JITScalar((int32)(0)));
    endInstructions.push_back(&data.back());
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    prev_fixup->arg1 = JITScalar(data.size());
    compile_block(t.last().first(),m_eval);
  }
  int end_address = data.size();
  for (int i=0;i<endInstructions.size();i++)
    endInstructions[i]->arg1 = JITScalar((int32)(end_address));
  push_instruction(op_nop,type_i,JITScalar(),arg_ii,JITScalar());
}

JITScalar* JITVM::add_argument(string name, Interpreter* m_eval, bool scalar) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
  if (!ptr.valid())
    throw Exception("Undefined variable reference:" + name);
  if (!ptr->is2D())
    throw Exception("Cannot JIT multi-dimensional array:" + name);
  if (ptr->isString() || ptr->isReferenceType())
    throw Exception("Cannot JIT strings or reference types:" + name);
  if (ptr->isComplex())
    throw Exception("Cannot JIT complex arrays:" + name);
  if (!ptr->isScalar() && scalar)
    throw Exception("JIT requires " + name + " be a scalar");
  Array* a = &(*ptr);
  scalar_class oclass = map_array_type((void*)a);
  if (scalar) {
    // We use a register to 
    JITScalar result(new_register(oclass));
    add_symbol(name,result);
    JITScalar arg1;
    switch (oclass) {
    case c_bool:
      arg1 = JITScalar((bool) ((const logical*)(a->getDataPointer()))[0]);
      break;
    case c_int32:
      arg1 = JITScalar((int32) ArrayToInt32(*a));
      break;      
    case c_float:
      arg1 = JITScalar((float) ArrayToDouble(*a));
      break;
    case c_double:
      arg1 = JITScalar((double) ArrayToDouble(*a));
      break;
    }
    push_instruction(op_set,compute_oc_code(oclass),result,
		     compute_ri_code(arg1,arg1),arg1,arg1);
  } else 
    add_symbol(name,JITScalar((void*) a));
  return find_symbol(name);
}

void JITVM::dump(ostream& o) {
  for (int i=0;i<data.size();i++) {
    char buffer[1000];
    sprintf(buffer,"%03u: ",i);
    o << buffer;
    data[i].dump(o);
    o << "\r\n";
  }
}

JITScalar JITVM::compile_rhs(tree t, Interpreter* m_eval) {
  string symname(t.first().text());
  JITScalar *v = find_symbol(symname);
  if (!v)
    v = add_argument(symname,m_eval,t.numchildren() == 1);
  if (t.numchildren() == 1) {
    if (v->isp())
      throw Exception("non-scalar reference returned in scalar context!");
    return *v;
  }
  if (t.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (!v->isp())
    throw Exception("array indexing of scalar values...");
  tree s(t.second());
  if (!s.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s.numchildren() == 1) {
    scalar_class output_type(map_array_type(v->p()));
    JITScalar result(new_register(output_type));
    JITScalar arg1 = promote(compile_expression(s.first(),m_eval),c_int32);
    push_instruction(op_vload,compute_oc_code(output_type),
		     result,compute_ri_code(*v,arg1),*v,arg1);
    return result;
  } else if (s.numchildren() == 2) {
    scalar_class output_type(map_array_type(v->p()));
    JITScalar result(new_register(output_type));
    JITScalar arg1 = promote(compile_expression(s.first(),m_eval),c_int32);
    JITScalar arg2 = promote(compile_expression(s.second(),m_eval),c_int32);
    push_instruction(op_mset,type_b,JITScalar(),compute_ri_code(arg1,arg2),arg1,arg2);
    push_instruction(op_mload,compute_oc_code(output_type),
		     result,compute_ri_code(*v,*v),*v);
    return result;
  }
  throw Exception("dereference not handled yet...");
}

JITScalar JITVM::compile_expression(tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return compile_rhs(t,m_eval);
  case TOK_INTEGER:
    return JITScalar(ArrayToInt32(t.array()));
  case TOK_FLOAT:
    return JITScalar((float) ArrayToDouble(t.array()));
  case TOK_DOUBLE:
    return JITScalar((double) ArrayToDouble(t.array()));
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF: 
    throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case '+': 
    return binary_op(op_add,compile_expression(t.first(),m_eval),
		     compile_expression(t.second(),m_eval),"add");
  case '-': 
    return binary_op(op_sub,compile_expression(t.first(),m_eval),
		     compile_expression(t.second(),m_eval),"sub");
  case '*': 
  case TOK_DOTTIMES: 
    return binary_op(op_mul,compile_expression(t.first(),m_eval),
		     compile_expression(t.second(),m_eval),"mul");
  case '/': 
  case TOK_DOTRDIV: 
//     return binary_div(expression(t.first(),m_eval),
// 		      expression(t.second(),m_eval));
    throw Exception("Division is not supported yet.");
  case '\\': 
  case TOK_DOTLDIV: 
//     return binary_div(expression(t.second(),m_eval),
// 		      expression(t.first(),m_eval));
    throw Exception("Division is not supported yet.");
  case TOK_SOR: 
  case '|':
    return boolean_op(op_or,compile_expression(t.first(),m_eval),
		      compile_expression(t.second(),m_eval),"or");
  case TOK_SAND: 
  case '&': 
    return boolean_op(op_and,compile_expression(t.first(),m_eval),
		      compile_expression(t.second(),m_eval),"and");
  case '<': 
    return comparison_op(op_lt,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"lt");
  case TOK_LE: 
    return comparison_op(op_le,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"le");
  case '>': 
    return comparison_op(op_gt,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"gt");
  case TOK_GE: 
    return comparison_op(op_ge,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"ge");
  case TOK_EQ: 
    return comparison_op(op_eq,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"eq");
  case TOK_NE: 
    return comparison_op(op_ne,compile_expression(t.first(),m_eval),
			 compile_expression(t.second(),m_eval),"ne");
  case TOK_UNARY_MINUS: 
    throw Exception("unary minus not supported yet.");
  case TOK_UNARY_PLUS: 
    return compile_expression(t.first(),m_eval);
  case '~': 
    throw Exception("unary not is not supported yet.");
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

void JITVM::compile_statement_type(tree t, Interpreter *m_eval) {
  switch(t.token()) {
  case '=': 
    compile_assignment(t,m_eval);
    break;
  case TOK_MULTI:
    throw Exception("multi function calls do not JIT compile");
  case TOK_SPECIAL:
    throw Exception("special function calls do not JIT compile");
  case TOK_FOR:
    compile_for_block(t,m_eval);
    break;
  case TOK_WHILE:
    throw Exception("nested while loops do not JIT compile");
  case TOK_IF:
    compile_if_statement(t,m_eval);
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
    compile_expression(t.first(),m_eval);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void JITVM::compile_statement(tree t, Interpreter *m_eval) {
  if (t.is(TOK_STATEMENT) && 
      (t.first().is(TOK_EXPR) || t.first().is(TOK_SPECIAL) ||
       t.first().is(TOK_MULTI) || t.first().is('=')))
    throw Exception("JIT compiler doesn't work with verbose statements");
  compile_statement_type(t.first(),m_eval);
}

void JITVM::compile_block(tree t, Interpreter *m_eval) {
  const treeVector &statements(t.children());
  for (treeVector::const_iterator i=statements.begin();
       i!=statements.end();i++) 
    compile_statement(*i,m_eval);
}

void JITVM::compile_for_block(tree t, Interpreter *m_eval) {
  if (!(t.first().is('=') && t.first().second().is(':') &&
	t.first().second().first().is(TOK_INTEGER) &&
	t.first().second().second().is(TOK_INTEGER))) 
    throw Exception("For loop cannot be compiled - need integer bounds");
  int loop_start(atoi(t.first().second().first().text().c_str()));
  int loop_stop(atoi(t.first().second().second().text().c_str()));
  string loop_index(t.first().first().text());
  JITScalar loop_index_register(new_register(c_int32));
  add_symbol(loop_index,loop_index_register);
  push_instruction(op_set,type_i,loop_index_register,arg_ii,JITScalar(loop_start));
  int32 loop_start_instruction(data.size());
  compile_block(t.second(),m_eval);
  push_instruction(op_add,type_i,loop_index_register,
		   arg_ri,loop_index_register,JITScalar(1));
  JITScalar loop_test_register(new_register(c_bool));
  push_instruction(op_le,type_i,loop_test_register,
		   arg_ri,loop_index_register,JITScalar(loop_stop));
  int32 loop_end_instruction(data.size());
  push_instruction(op_jit,type_i,loop_test_register,arg_ii,
		   JITScalar((int32)loop_start_instruction));
}

template <class T> 
static inline void add_local(JITScalar* dest, T op1, T op2) {
  dest->set(op1+op2);
}


// This is just to do:
//
//  for (i=0;i<10000;i++) A[i] = i;
//[basu@bluering test]$ more foo.o.ll
//define void @initArray(float* %A, i32 %N) {
//entry:
//        %tmp1018 = icmp sgt i32 %N, 0           ; <i1> [#uses=1]
//        br i1 %tmp1018, label %bb, label %return
//
//bb:             ; preds = %bb, %entry
//        %i.013.0 = phi i32 [ 0, %entry ], [ %indvar.next, %bb ]         ; <i32> [#uses=3]
//        %tmp1 = sitofp i32 %i.013.0 to float            ; <float> [#uses=1]
//        %tmp4 = getelementptr float* %A, i32 %i.013.0           ; <float*> [#uses=1]
//        store float %tmp1, float* %tmp4
//        %indvar.next = add i32 %i.013.0, 1              ; <i32> [#uses=2]
//        %exitcond = icmp eq i32 %indvar.next, %N                ; <i1> [#uses=1]
//        br i1 %exitcond, label %return, label %bb
//
//return:         ; preds = %bb, %entry
//        ret void
//}
//
//
// According to llvm2cpp:
//
//
//Module* makeLLVMModule() {
//  // Module Construction
//  Module* mod = new Module("foo.o");
//  mod->setDataLayout("e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64
//:64-v128:128:128-a0:0:64");
//  mod->setTargetTriple("i686-pc-linux-gnu");
//
//  // Type Definitions
//  std::vector<const Type*>FuncTy_0_args;
//  PointerType* PointerTy_1 = PointerType::get(Type::FloatTy);
//
//  FuncTy_0_args.push_back(PointerTy_1);
//  FuncTy_0_args.push_back(IntegerType::get(32));
//  ParamAttrsList *FuncTy_0_PAL = 0;
//  FunctionType* FuncTy_0 = FunctionType::get(
//    /*Result=*/Type::VoidTy,
//    /*Params=*/FuncTy_0_args,
//    /*isVarArg=*/false,
//    /*ParamAttrs=*/FuncTy_0_PAL);
//
//
//  // Function Declarations
//
//  Function* func_initArray = new Function(
//    /*Type=*/FuncTy_0,
//    /*Linkage=*/GlobalValue::ExternalLinkage,
//    /*Name=*/"initArray", mod);
//  func_initArray->setCallingConv(CallingConv::C);
//
//  // Global Variable Declarations
//
//
//  // Constant Definitions
//  Constant* const_int32_2 = Constant::getNullValue(IntegerType::get(32));
//  ConstantInt* const_int32_3 = ConstantInt::get(APInt(32,  "1", 10));
//
//  // Global Variable Definitions
//
//
//  // Function Definitions
//
//  // Function: initArray (func_initArray)
//  {
//    Function::arg_iterator args = func_initArray->arg_begin();
//    Value* ptr_A = args++;
//    ptr_A->setName("A");
//    Value* int32_N = args++;
//    int32_N->setName("N");
//
//    BasicBlock* label_entry = new BasicBlock("entry",func_initArray,0);
//    BasicBlock* label_bb = new BasicBlock("bb",func_initArray,0);
//    BasicBlock* label_return = new BasicBlock("return",func_initArray,0);
//
//    // Block entry (label_entry)
//    ICmpInst* int1_tmp1018 = new ICmpInst(ICmpInst::ICMP_SGT, int32_N, const_int32_2, "tmp1018", label_en
//try);
//    new BranchInst(label_bb, label_return, int1_tmp1018, label_entry);
//
//    // Block bb (label_bb)
//    Argument* fwdref_5 = new Argument(IntegerType::get(32));
//    PHINode* int32_i_013_0 = new PHINode(IntegerType::get(32), "i.013.0", label_bb);
//    int32_i_013_0->reserveOperandSpace(2);
//    int32_i_013_0->addIncoming(const_int32_2, label_entry);
//    int32_i_013_0->addIncoming(fwdref_5, label_bb);
//
//    CastInst* float_tmp1 = new SIToFPInst(int32_i_013_0, Type::FloatTy, "tmp1", label_bb);
//    GetElementPtrInst* ptr_tmp4 = new GetElementPtrInst(ptr_A, int32_i_013_0, "tmp4", label_bb);
//    StoreInst* void_6 = new StoreInst(float_tmp1, ptr_tmp4, false, label_bb);
//    BinaryOperator* int32_indvar_next = BinaryOperator::create(Instruction::Add, 
//                 int32_i_013_0, const_int32_3, "indvar.next", label_bb);
//    ICmpInst* int1_exitcond = new ICmpInst(ICmpInst::ICMP_EQ, int32_indvar_next, 
//                 int32_N, "exitcond", label_bb);
//    new BranchInst(label_return, label_bb, int1_exitcond, label_bb);
//
//    // Block return (label_return)
//    new ReturnInst(label_return);
//
//    // Resolve Forward References
//    fwdref_5->replaceAllUsesWith(int32_indvar_next); delete fwdref_5;
//
//  }
//
//  return mod;
//}
//
int JITtest() {
  Module *M = new Module("test");
  std::vector<const Type*> FuncTy_0_args;
  PointerType* Float_PointerTy = PointerType::get(Type::FloatTy);
  FuncTy_0_args.push_back(Float_PointerTy);
  llvm::FunctionType* FuncTy_0 = llvm::FunctionType::get(Type::VoidTy,FuncTy_0_args,false,(ParamAttrsList *)0);
  Function *F = new Function(FuncTy_0,
			     GlobalValue::ExternalLinkage,
			     "initArray", M);
  F->setCallingConv(CallingConv::C);
  Constant* phi_left = Constant::getNullValue(IntegerType::get(32));
  // Get the loop limits
  ConstantInt* loop_start = ConstantInt::get(APInt(32, "0", 10));
  ConstantInt* loop_increment = ConstantInt::get(APInt(32, "1", 10));
  ConstantInt* loop_stop = ConstantInt::get(APInt(32, "1000000", 10));
  // Add A as an argument to the function
  Function::arg_iterator args = F->arg_begin();
  Value* ptr_A = args++;
  ptr_A->setName("A");
  // Add a loop
  BasicBlock* bb_loop_entry = new BasicBlock("",F,0);
  BasicBlock* bb_loop_body = new BasicBlock("",F,0);
  BasicBlock* bb_loop_exit = new BasicBlock("",F,0);
  
  // Check for legal loop limits
  ICmpInst* legal_loop_limits = new ICmpInst(ICmpInst::ICMP_SGT, loop_stop, loop_start, "", bb_loop_entry);
  new BranchInst(bb_loop_body, bb_loop_exit, legal_loop_limits, bb_loop_entry);
  
  // Setup the PHINode
  Argument* fwdref = new Argument(IntegerType::get(32));
  PHINode*  loop_index = new PHINode(IntegerType::get(32), "", bb_loop_body);
  loop_index->reserveOperandSpace(2);
  loop_index->addIncoming(phi_left, bb_loop_entry);
  loop_index->addIncoming(fwdref, bb_loop_body);
  
  // Cast the loop index to a float value
  Value* float_tmp1 = new SIToFPInst(loop_index, Type::FloatTy, "", bb_loop_body);
  Value* ptr_tmp4 = new GetElementPtrInst(ptr_A, loop_index, "",bb_loop_body);
  Value* void_7 = new StoreInst(float_tmp1, ptr_tmp4, bb_loop_body);
  
  // Update the loop index
  Value* loop_next = BinaryOperator::create(Instruction::Add, loop_index, loop_increment, "", bb_loop_body);
  // Compare the loop index to the loop termination
  Value* exit_test = new ICmpInst(ICmpInst::ICMP_EQ, loop_next, loop_stop, "", bb_loop_body);
  new BranchInst(bb_loop_exit, bb_loop_body, exit_test, bb_loop_body);
  
  new ReturnInst(bb_loop_exit);
  
  fwdref->replaceAllUsesWith(loop_next); delete fwdref;

  std::cout << *M;
  
  M->setTargetTriple("i686-pc-linux-gnu");

  ExistingModuleProvider* MP = new ExistingModuleProvider(M);
  ExecutionEngine* EE = ExecutionEngine::create(MP, false);

  {
    float *G = new float[1000000];
    std::vector<GenericValue> args;
    args.push_back(GenericValue(G));
    std::cout << "\r\nSTART\r\n";
    GenericValue gv = EE->runFunction(F,args);
    
    std::cout << "G[92932] = " << G[92932] << "\r\n";
    delete G;
  }
  return 0;
}

int JITmain() {
  // Create some module to put our function into it.
  Module *M = new Module("test");

  // Create the add1 function entry and insert this entry into module M.  The
  // function will have a return type of "int" and take an argument of "int".
  // The '0' terminates the list of argument types.
  Function *Add1F =
    cast<Function>(M->getOrInsertFunction("add1", Type::Int32Ty, Type::Int32Ty,
                                          (Type *)0));

  // Add a basic block to the function. As before, it automatically inserts
  // because of the last argument.
  BasicBlock *BB = new BasicBlock("EntryBlock", Add1F);

  // Get pointers to the constant `1'.
  Value *One = ConstantInt::get(Type::Int32Ty, 1);

  // Get pointers to the integer argument of the add1 function...
  assert(Add1F->arg_begin() != Add1F->arg_end()); // Make sure there's an arg
  Argument *ArgX = Add1F->arg_begin();  // Get the arg
  ArgX->setName("AnArg");            // Give it a nice symbolic name for fun.

  // Create the add instruction, inserting it into the end of BB.
  Instruction *Add = BinaryOperator::createAdd(One, ArgX, "addresult", BB);

  // Create the return instruction and add it to the basic block
  new ReturnInst(Add, BB);

  // Now, function add1 is ready.


  // Now we going to create function `foo', which returns an int and takes no
  // arguments.
  Function *FooF =
    cast<Function>(M->getOrInsertFunction("foo", Type::Int32Ty, (Type *)0));

  // Add a basic block to the FooF function.
  BB = new BasicBlock("EntryBlock", FooF);

  // Get pointers to the constant `10'.
  Value *Ten = ConstantInt::get(Type::Int32Ty, 10);

  // Pass Ten to the call call:
  CallInst *Add1CallRes = new CallInst(Add1F, Ten, "add1", BB);
  Add1CallRes->setTailCall(true);

  // Create the return instruction and add it to the basic block.
  new ReturnInst(Add1CallRes, BB);

  // Now we create the JIT.
  ExistingModuleProvider* MP = new ExistingModuleProvider(M);
  ExecutionEngine* EE = ExecutionEngine::create(MP, false);

  std::cout << "We just constructed this LLVM module:\n\n" << *M;
  std::cout << "\n\nRunning foo: " << std::flush;

  // Call the `foo' function with no arguments:
  std::vector<GenericValue> noargs;
  GenericValue gv = EE->runFunction(FooF, noargs);

  // Import result of execution:
  std::cout << "Result: " << gv.IntVal.toString(10) << "\n";
  return 0;
}

void JITVM::run(Interpreter *m_eval) {
//   void (*tptr)(JITScalar*,int32,int32) = &add_local<int32>;
//   for (int i=0;i<1000*1000*10;i++) {
//     //    (this->*opcode_table_i[op_add])(0,50,50);
//     tptr(&reg[0],50,50);
//   } 
  JITtest();
  return;
  ip = 0;
  unsigned ops_max = data.size();
  while (ip < ops_max)
    dispatch(data[ip++]);
}

