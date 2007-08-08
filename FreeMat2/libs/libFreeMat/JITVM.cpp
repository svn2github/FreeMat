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
    case arg_ri:
      if (reg[dest.reg()].b()) ip -= reg[arg1.reg()].i(); break;
    case arg_ir:
    case arg_ii:
      if (reg[dest.reg()].b()) ip -= arg1.i(); break;
    }
    break;
  case op_mload:
    switch (inst.ri) {
    case arg_rr:
      dispatch_mload(inst.type,dest,arg1.p(),reg[arg2.reg()].i(),reg[arg3.reg()].i()); break;
    case arg_ri:
      dispatch_mload(inst.type,dest,arg1.p(),reg[arg2.reg()].i(),arg3.i()); break;
    case arg_ir:
      dispatch_mload(inst.type,dest,arg1.p(),arg2.i(),reg[arg3.reg()].i()); break;
    case arg_ii:
      dispatch_mload(inst.type,dest,arg1.p(),arg2.i(),arg3.i()); break;
    }
    break;
  case op_vload:
    switch (inst.ri) {
    case arg_ir:
    case arg_rr:
      dispatch_vload(inst.type,dest,arg1.p(),reg[arg2.reg()].i()); break;
    case arg_ri:
    case arg_ii:
      dispatch_vload(inst.type,dest,arg1.p(),arg2.i()); break;
    }
    break;
  case op_mstore:
    switch (inst.ri) {
    case arg_rr:
      dispatch_mstore(inst.type,dest,arg1.p(),reg[arg2.reg()].i(),reg[arg3.reg()].i()); break;
    case arg_ri:
      dispatch_mstore(inst.type,dest,arg1.p(),reg[arg2.reg()].i(),arg3.i()); break;
    case arg_ir:
      dispatch_mstore(inst.type,dest,arg1.p(),arg2.i(),reg[arg3.reg()].i()); break;
    case arg_ii:
      dispatch_mstore(inst.type,dest,arg1.p(),arg2.i(),arg3.i()); break;
    }
    break;
  case op_vstore:
    switch (inst.ri) {
    case arg_ir:
      dispatch_vstore(inst.type,dest.p(),arg1.i(),reg[arg2.reg()]); break;
    case arg_rr:
      dispatch_vstore(inst.type,dest.p(),reg[arg1.reg()].i(),reg[arg2.reg()]); break;
    case arg_ri:
      dispatch_vstore(inst.type,dest.p(),reg[arg1.reg()].i(),arg2); break;
    case arg_ii:
      dispatch_vstore(inst.type,dest.p(),arg1.i(),arg2); break;
    }
    break;
  }
}

void JITVM::dispatch_mstore(op_type type, JITScalar dest, void *ptr, int32 row, int32 col) {
  Array *a = (Array*) ptr;
  if ((row <= a->rows()) && (row >= 1) && (col <= a->columns()) && (col >= 1)) {
    switch (type) {
    case type_b:
      ((logical*) a->getDataPointer())[row-1+(col-1)*a->rows()] = reg[dest.reg()].b();
      break;
    case type_i:
      ((int32*) a->getDataPointer())[row-1+(col-1)*a->rows()] = reg[dest.reg()].i();
      break;
    case type_f:
      ((float*) a->getDataPointer())[row-1+(col-1)*a->rows()] = reg[dest.reg()].f();
      break;
    case type_d:
      ((double*) a->getDataPointer())[row-1+(col-1)*a->rows()] = reg[dest.reg()].d();
      break;
    }
  } else {
    throw Exception("Out of range access");
  }
}

void JITVM::dispatch_mload(op_type type, JITScalar dest, void *ptr, int32 row, int32 col) {
  Array *a = (Array*) ptr;
  if ((row <= a->rows()) && (row >= 1) && (col <= a->columns()) && (col >= 1)) {
    switch (type) {
    case type_b:
      reg[dest.reg()].set((bool) ((logical*) a->getDataPointer())[row-1+(col-1)*a->rows()]);
      break;
    case type_i:
      reg[dest.reg()].set((int32) ((int32*) a->getDataPointer())[row-1+(col-1)*a->rows()]);
      break;
    case type_f:
      reg[dest.reg()].set((float) ((float*) a->getDataPointer())[row-1+(col-1)*a->rows()]);
      break;
    case type_d:
      reg[dest.reg()].set((double) ((double*) a->getDataPointer())[row-1+(col-1)*a->rows()]);
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

void JITVM::dispatch_vstore(op_type type, void *ptr, int32 row, JITScalar dest) {
  Array *a = (Array*) ptr;
  if ((row <= (a->rows()*a->columns())) && (row >= 1)) {
    switch (type) {
    case type_b:
      ((logical*) a->getDataPointer())[row-1] = dest.b();
      break;
    case type_i:
      ((int32*) a->getDataPointer())[row-1] = dest.i();
      break;
    case type_f:
      ((float*) a->getDataPointer())[row-1] = dest.f();
      break;
    case type_d:
      ((double*) a->getDataPointer())[row-1] = dest.d();
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
  if (arg.type() <= c_pointer) return arg.type();
  if (arg.type() == c_unknown) throw Exception("encountered uninitialized scalar in compilation!");
  if (arg.type() == c_register) return type_of(reg[arg.reg()]);
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
			     op_ri ri_code, JITScalar arg1, JITScalar arg2, JITScalar arg3) {
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
  case op_mload:  o << "mload "; break;
  case op_vload:  o << "vload "; break;
  case op_mstore: o << "mstore"; break;
  case op_vstore: o << "vstore"; break;
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
    if (v->type() != type_of(rhs))
      throw Exception("polymorphic assignment to scalar detected.");
    if (!v->isscalar())
      throw Exception("scalar assignment to array variable.");
    push_instruction(op_set,compute_oc_code(v->type()),*v,compute_ri_code(rhs),rhs);
    return;
  }
  if (s.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isscalar())
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
    push_instruction(op_mstore,compute_oc_code(type_of(rhs)),
		     rhs,compute_ri_code(arg1,arg2),v,arg1,arg2);
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
    switch (oclass) {
    case c_bool:
      add_symbol(name,JITScalar((bool) ((const logical*)(a->getDataPointer()))[0]));
      break;
    case c_int32:
      add_symbol(name,JITScalar((int32) ArrayToInt32(*a)));
      break;      
    case c_float:
      add_symbol(name,JITScalar((float) ArrayToDouble(*a)));
      break;
    case c_double:
      add_symbol(name,JITScalar((double) ArrayToDouble(*a)));
      break;
    }
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
		     result,compute_ri_code(arg1),v,arg1);
    return result;
  } else if (s.numchildren() == 2) {
    scalar_class output_type(map_array_type(v->p()));
    JITScalar result(new_register(output_type));
    JITScalar arg1 = promote(compile_expression(s.first(),m_eval),c_int32);
    JITScalar arg2 = promote(compile_expression(s.second(),m_eval),c_int32);
    push_instruction(op_mload,compute_oc_code(output_type),
		     result,compute_ri_code(arg1,arg2),arg1,arg2);
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
    //    compile_if_statement(t,m_eval);
    throw Exception("if statements do not JIT compile");
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
  if (t.is(TOK_STATEMENT))
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
  push_instruction(op_jit,type_b,loop_test_register,arg_ii,
		   JITScalar((int32)(loop_end_instruction-loop_start_instruction+1)));
}

void JITVM::run(Interpreter *m_eval) {
  ip = 0;
  unsigned ops_max = data.size();
  while (ip < ops_max)
    dispatch(data[ip++]);
}
