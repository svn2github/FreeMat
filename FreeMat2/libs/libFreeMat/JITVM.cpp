// Still need:

//  Type promotion on loop entry.
//  Range checking.
//  Dynamic array resizing.
//

//
// Just for fun, mind you....
//

#include "JITVM.hpp"

void JITVM::dispatch(const JITInstruction &inst) {
  const JITScalar &dest = inst.dest;
  const JITScalar &arg1 = inst.arg1;
  const JITScalar &arg2 = inst.arg2;
  const JITScalar &arg3 = inst.arg3;
  switch (inst.op) {
  case op_add:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for add instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() + reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() + arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() + reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() + arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() + reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() + arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() + reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() + arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() + reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() + arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() + reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() + arg2.d()); break;
      }
      break;
    }
    break;
  case op_sub:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for sub instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() - reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() - arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() - reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() - arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() - reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() - arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() - reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() - arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() - reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() - arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() - reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() - arg2.d()); break;
      }
      break;
    }
    break;
  case op_mul:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for mul instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() * reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() * arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() * reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() * arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() * reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() * arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() * reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() * arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() * reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() * arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() * reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() * arg2.d()); break;
      }
      break;
    }
    break;
  case op_div:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for div instruction");
    case type_i:
      throw Exception("unhandled type integer for div instruction");
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() / reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() / arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() / reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() / arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() / reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() / arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() / reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() / arg2.d()); break;
      }
      break;
    }
    break;
  case op_or:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].b() || reg[arg2.reg()].b()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].b() || arg2.b()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.b() || reg[arg2.reg()].b()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.b() || arg2.b()); break;
      }
      break;
    case type_i:
    case type_f:
    case type_d:
      throw Exception("boolean operators required boolean output");
    }
    break;
  case op_neg:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for neg instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(-reg[arg1.reg()].i()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(-arg1.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(-reg[arg1.reg()].f()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(-arg1.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(-reg[arg1.reg()].d()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(-arg1.d()); break;
      }
      break;
    }
    break;
  case op_and:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].b() && reg[arg2.reg()].b()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].b() && arg2.b()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.b() && reg[arg2.reg()].b()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.b() && arg2.b()); break;
      }
      break;
    case type_i:
    case type_f:
    case type_d:
      throw Exception("boolean operators required boolean output");
    }
    break;
  case op_not:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(!reg[arg1.reg()].b()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(!arg1.b()); break;
      }
      break;
    case type_i:
    case type_f:
    case type_d:
      throw Exception("boolean operators required boolean output");
    }
    break;
  case op_lt:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for lt instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() < reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() < arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() < reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() < arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() < reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() < arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() < reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() < arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() < reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() < arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() < reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() < arg2.d()); break;
      }
      break;
    }
    break;
  case op_le:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for le instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() <= reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() <= arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() <= reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() <= arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() <= reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() <= arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() <= reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() <= arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() <= reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() <= arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() <= reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() <= arg2.d()); break;
      }
      break;
    }
    break;
  case op_ne:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for ne instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() != reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() != arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() != reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() != arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() != reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() != arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() != reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() != arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() != reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() != arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() != reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() != arg2.d()); break;
      }
      break;
    }
    break;
  case op_eq:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for eq instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() == reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() == arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() == reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() == arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() == reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() == arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() == reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() == arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() == reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() == arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() == reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() == arg2.d()); break;
      }
      break;
    }
    break;
  case op_gt:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for gt instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() > reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() > arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() > reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() > arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() > reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() > arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() > reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() > arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() > reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() > arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() > reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() > arg2.d()); break;
      }
      break;
    }
    break;
  case op_ge:
    switch (inst.type) {
    case type_b:
      throw Exception("unhandled type boolean for ge instruction");
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].i() >= reg[arg2.reg()].i()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() >= arg2.i()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.i() >= reg[arg2.reg()].i()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.i() >= arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].f() >= reg[arg2.reg()].f()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() >= arg2.f()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.f() >= reg[arg2.reg()].f()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.f() >= arg2.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	reg[dest.reg()].set(reg[arg1.reg()].d() >= reg[arg2.reg()].d()); break;
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() >= arg2.d()); break;
      case arg_ir:
	reg[dest.reg()].set(arg1.d() >= reg[arg2.reg()].d()); break;
      case arg_ii:
	reg[dest.reg()].set(arg1.d() >= arg2.d()); break;
      }
      break;
    }
    break;
  case op_set:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].b()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.b()); break;
      }
      break;
    case type_i:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.d()); break;
      }
      break;
    }
    break;
  case op_castb:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].b()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.b()); break;
      }
      break;
    case type_i:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i() != 0); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.i() != 0); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f() != 0); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.f() != 0); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d() != 0); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.d() != 0); break;
      }
      break;
    }
    break;
  case op_casti:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	if (reg[arg1.reg()].b())
	  reg[dest.reg()].set(1); 
	else
	  reg[dest.reg()].set(0);
	break;
      case arg_ir:
      case arg_ii:
	if (arg1.b())
	  reg[dest.reg()].set(1); 
	else
	  reg[dest.reg()].set(0);
	break;
      }
      break;
    case type_i:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].i()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.d()); break;
      }
      break;
    }
    break;
  case op_castf:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	if (reg[arg1.reg()].b())
	  reg[dest.reg()].set(1.0f); 
	else
	  reg[dest.reg()].set(0.0f);
	break;
      case arg_ir:
      case arg_ii:
	if (arg1.b())
	  reg[dest.reg()].set(1.0f); 
	else
	  reg[dest.reg()].set(0.0f);
	break;
      }
      break;
    case type_i:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set((float)(reg[arg1.reg()].i())); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set((float)(arg1.i())); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].f()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.f()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set((float)(reg[arg1.reg()].d())); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set((float)(arg1.d())); break;
      }
      break;
    }
    break;
  case op_castd:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	if (reg[arg1.reg()].b())
	  reg[dest.reg()].set(1.0); 
	else
	  reg[dest.reg()].set(0.0);
	break;
      case arg_ir:
      case arg_ii:
	if (arg1.b())
	  reg[dest.reg()].set(1.0); 
	else
	  reg[dest.reg()].set(0.0);
	break;
      }
      break;
    case type_i:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set((double)(reg[arg1.reg()].i())); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set((double)(arg1.i())); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set((double)(reg[arg1.reg()].f())); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set((double)(arg1.f())); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	reg[dest.reg()].set(reg[arg1.reg()].d()); break;
      case arg_ir:
      case arg_ii:
	reg[dest.reg()].set(arg1.d()); break;
      }
      break;
    }
    break;
  case op_nop:
    break;
  case op_jit:
    switch (inst.ri) {
    case arg_rr:
      if (reg[arg1.reg()].b()) ip += reg[arg2.reg()].i(); break;
    case arg_ri:
      if (reg[arg1.reg()].b()) ip += arg2.i(); break;
    case arg_ir:
      if (arg1.b()) ip += reg[arg2.reg()].i(); break;
    case arg_ii:
      if (arg1.b()) ip += arg2.i(); break;
    }
    break;
  case op_mload:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
	dispatch_mloadb(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),reg[arg3.reg()].i()); break;
      case arg_ri:
	dispatch_mloadb(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),arg3.i()); break;
      case arg_ir:
	dispatch_mloadb(dest.reg(),reg[arg1.reg()].p(),arg2.i(),reg[arg3.reg()].i()); break;
      case arg_ii:
	dispatch_mloadb(dest.reg(),reg[arg1.reg()].p(),arg2.i(),arg3.i()); break;
      }
      break;
    case type_i:
      switch (inst.ri) {
      case arg_rr:
	dispatch_mloadi(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),reg[arg3.reg()].i()); break;
      case arg_ri:
	dispatch_mloadi(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),arg3.i()); break;
      case arg_ir:
	dispatch_mloadi(dest.reg(),reg[arg1.reg()].p(),arg2.i(),reg[arg3.reg()].i()); break;
      case arg_ii:
	dispatch_mloadi(dest.reg(),reg[arg1.reg()].p(),arg2.i(),arg3.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
	dispatch_mloadf(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),reg[arg3.reg()].i()); break;
      case arg_ri:
	dispatch_mloadf(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),arg3.i()); break;
      case arg_ir:
	dispatch_mloadf(dest.reg(),reg[arg1.reg()].p(),arg2.i(),reg[arg3.reg()].i()); break;
      case arg_ii:
	dispatch_mloadf(dest.reg(),reg[arg1.reg()].p(),arg2.i(),arg3.i()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
	dispatch_mloadd(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),reg[arg3.reg()].i()); break;
      case arg_ri:
	dispatch_mloadd(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),arg3.i()); break;
      case arg_ir:
	dispatch_mloadd(dest.reg(),reg[arg1.reg()].p(),arg2.i(),reg[arg3.reg()].i()); break;
      case arg_ii:
	dispatch_mloadd(dest.reg(),reg[arg1.reg()].p(),arg2.i(),arg3.i()); break;
      }
      break;
    }
    break;
  case op_vload:
    switch (inst.type) {
    case type_b:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	dispatch_vloadb(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i()); break;
      case arg_ir:
      case arg_ii:
	dispatch_vloadb(dest.reg(),reg[arg1.reg()].p(),arg2.i()); break;
      }
      break;
    case type_i:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	dispatch_vloadi(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i()); break;
      case arg_ir:
      case arg_ii:
	dispatch_vloadi(dest.reg(),reg[arg1.reg()].p(),arg2.i()); break;
      }
      break;
    case type_f:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	dispatch_vloadf(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i(),arg3); break;
      case arg_ir:
      case arg_ii:
	dispatch_vloadf(dest.reg(),reg[arg1.reg()].p(),arg2.i()); break;
      }
      break;
    case type_d:
      switch (inst.ri) {
      case arg_rr:
      case arg_ri:
	dispatch_vloadd(dest.reg(),reg[arg1.reg()].p(),reg[arg2.reg()].i()); break;
      case arg_ir:
      case arg_ii:
	dispatch_vloadd(dest.reg(),reg[arg1.reg()].p(),arg2.i()); break;
      }
      break;
    }
    break;
}

op_type JITVM::compute_oc_code(scalarClass outClass) {
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

scalarClass JITVM::type_of(JITScalar arg) {
  if (arg.type() <= c_pointer) return arg.type();
  if (arg.type() == c_unknown) throw Exception("encountered uninitialized scalar in compilation!");
  if (arg.type() == c_register) return type_of(reg[arg.reg()]);
}
 
JITScalar JITVM::promote(JITScalar arg, scalarClass outClass) {
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
    JITScalar result(new_register(outClass));
    switch (outClass) {
    case c_bool:
      push_instruction(op_castb,compute_oc_code(reg[arg.reg()].type()),arg_rr,arg); break;
    case c_int32:
      push_instruction(op_casti,compute_oc_code(reg[arg.reg()].type()),arg_rr,arg); break;
    case c_float:
      push_instruction(op_castf,compute_oc_code(reg[arg.reg()].type()),arg_rr,arg); break;
    case c_double:
      push_instruction(op_castd,compute_oc_code(reg[arg.reg()].type()),arg_rr,arg); break;
    default:
      throw Exception("unhandled case for promote called");
    }
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
  scalarClass outClass;
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
  scalarClass outClass;
  outClass = type_of(arg1);
  if (type_of(arg2) > outClass) outClass = type_of(arg2);
  if (outClass == c_bool) outClass = c_int32;
  JITScalar result(new_register(c_bool));
  push_instruction(op,compute_oc_code(outClass),result,compute_ri_code(arg1,arg2),arg1,arg2);
  return result;
}

JITScalar JITVM::rhs(tree t, Interpreter* m_eval) {
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
    switch(map_array_type(v->p())) {
    case type_b: 
      {
	JITScalar result(new_register(c_bool));
	JITScalar arg1 = promote(expression(s.first(),m_eval),c_int32);
	push_instruction(op_vloadb,type_b,result,compute_ri_code(arg1,arg1),arg1);
	return result;
      }
    case type_i:
      {
	JITScalar result(new_register(c_int32));
	JITScalar arg1 = promote(expression(s.first(),m_eval),c_int32);
	push_instruction(op_vloadi,type_i,result,compute_ri_code(arg1,arg1),arg1);	
	return result;
      }
    case type_f:
      {
	JITScalar result(new_register(c_float));
	JITScalar arg1 = promote(expression(s.first(),m_eval),c_int32);
	push_instruction(op_vloadf,type_f,result,compute_ri_code(arg1,arg1),arg1);		
	return result;
      }
    case type_d:
      {
	JITScalar result(new_register(c_double));
	JITScalar arg1 = promote(expression(s.first(),m_eval),c_int32);
	push_instruction(op_vloadd,type_d,result,compute_ri_code(arg1,arg1),arg1);		
	return result;
      }
    }
    throw Exception("unhandled load case for JIT::rhs");
  } else if (s.numchildren() == 2) {
    switch(map_array_type(v->p())) {
    case type_b: 
      {
	JITScalar result(new_register(c_bool));
	JITScalar arg1 = promote(expression(s.first(),m_eval),c_int32);
	JITScalar arg2 = promote(expression(s.second(),m_eval),c_int32);
	push_instruction(op_mloadb,type_b,result,compute_ri_code(arg1,arg2),arg1,arg2);
	return result;
      }
    case type_i:
      {
	JITScalar result(new_register(c_int32));
	JITScalar arg1 = promote(expression(s.first(),m_eval),c_int32);
	JITScalar arg2 = promote(expression(s.second(),m_eval),c_int32);
	push_instruction(op_mloadi,type_i,result,compute_ri_code(arg1,arg2),arg1,arg2);	
	return result;
      }
    case type_f:
      {
	JITScalar result(new_register(c_float));
	JITScalar arg1 = promote(expression(s.first(),m_eval),c_int32);
	JITScalar arg2 = promote(expression(s.second(),m_eval),c_int32);
	push_instruction(op_mloadf,type_f,result,compute_ri_code(arg1,arg2),arg1,arg2);
	return result;
      }
    case type_d:
      {
	JITScalar result(new_register(c_double));
	arg1 = promote(expression(),c_int32);
	push_instruction(op_vloadd,type_d,result,compute_ri_code(arg1,arg1),arg1);		
	return result;
      }
    }
  }
  throw Exception("dereference not handled yet...");
}

JITScalar JITVM::expression(tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return rhs(t,m_eval);
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
    return binary_op(add_i_rr,expression(t.first(),m_eval),
		     expression(t.second(),m_eval),"add");
  case '-': 
    return binary_op(sub_i_rr,expression(t.first(),m_eval),
		     expression(t.second(),m_eval),"sub");
  case '*': 
  case TOK_DOTTIMES: 
    return binary_op(mul_i_rr,expression(t.first(),m_eval),
		     expression(t.second(),m_eval),"mul");
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
    return boolean_op(or_b_rr,expression(t.first(),m_eval),
		      expression(t.second(),m_eval),"or");
  case TOK_SAND: 
  case '&': 
    return boolean_op(and_b_rr,expression(t.first(),m_eval),
		      expression(t.second(),m_eval),"and");
  case '<': 
    return comparison_op(lt_i_rr,expression(t.first(),m_eval),
			 expression(t.second(),m_eval),"lt");
  case TOK_LE: 
    return comparison_op(le_i_rr,expression(t.first(),m_eval),
			 expression(t.second(),m_eval),"le");
  case '>': 
    return comparison_op(gt_i_rr,expression(t.first(),m_eval),
			 expression(t.second(),m_eval),"gt");
  case TOK_GE: 
    return comparison_op(ge_i_rr,expression(t.first(),m_eval),
			 expression(t.second(),m_eval),"ge");
  case TOK_EQ: 
    return comparison_op(eq_i_rr,expression(t.first(),m_eval),
			 expression(t.second(),m_eval),"eq");
  case TOK_NE: 
    return comparison_op(ne_i_rr,expression(t.first(),m_eval),
			 expression(t.second(),m_eval),"ne");
  case TOK_UNARY_MINUS: 
    throw Exception("unary minus not supported yet.");
  case TOK_UNARY_PLUS: 
    return expression(t.first(),m_eval);
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
