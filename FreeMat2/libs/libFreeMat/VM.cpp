#include "VM.hpp"

static tindex NewTemp = 1;

tindex GetNewTemporary() {
  return NewTemp++;
}

VMInstruction::VMInstruction(VMOpcode op_t, VMOperand src1_t, VMOperand src2_t, VMOperand dst_t) :
op(op_t), src1(src1_t), src2(src2_t), dst(dst_t) {
}

void VMStream::EmitOpCode(VMOpcode code, VMOperand value, tindex dest) {
  instr.push_back(VMInstruction(code,value,VMOperand(),VMOperand(REGISTER,dest)));
}

void VMStream::EmitOpCode(VMOpcode code, tindex src, tindex dest) {
  instr.push_back(VMInstruction(code,VMOperand(REGISTER,src),VMOperand(),VMOperand(REGISTER,dest)));
}

void VMStream::EmitOpCode(VMOpcode code, tindex left, tindex right, tindex dest) {
  instr.push_back(VMInstruction(code,VMOperand(REGISTER,left),VMOperand(REGISTER,right),VMOperand(REGISTER,dest)));
}

void VMStream::EmitOpCode(VMOpcode code, tindex left, VMOperand right, tindex dest) {
  instr.push_back(VMInstruction(code,VMOperand(REGISTER,left),right,VMOperand(REGISTER,dest)));
}

tindex VMStream::LookupVariable(string name) {
  if (vars.count(name) == 0)
    vars[name] = GetNewTemporary();
  return (vars.find(name)->second);
}

string VMStream::GetAliasName(tindex n) {
  for (map<string,tindex>::iterator i=vars.begin(); i!= vars.end(); i++) {
    if (i->second == n) return (i->first);
  }
  return string("$") + n;
}

VMOperand::VMOperand() {
  type = NONE;
}

VMOperand::VMOperand(VMOperandType type_t, tindex value_t) {
  type = REGISTER;
  scalar_value.uint32_value = value_t;
}

VMOperand::VMOperand(VMOperandType type_t, int32 value_t) {
  type = LITERALI;
  scalar_value.int32_value = value_t;
}

VMOperand::VMOperand(VMOperandType type_t, double value_t) {
  type = LITERALD;
  scalar_value.double_value = value_t;
}

VMOperand::VMOperand(VMOperandType type_t, float value_t) {
  type = LITERALF;
  scalar_value.float_value = value_t;
}

VMOperand::VMOperand(VMOperandType type_t, string value_t) {
  type = LITERALS;
  string_value = value_t;
}
VMOperand::VMOperand(VMOperandType type_t, Array value_t) {
  type = LITERALA;
  array_value = value_t;
}

void VMStream::PrintOperand(VMOperand opand) {
  switch(opand.type) {
  case NONE:
    cout << "none";
    break;
  case REGISTER:
    cout << GetAliasName(opand.scalar_value.uint32_value);
    break;
  case LITERALI:
    cout << opand.scalar_value.int32_value;
    break;
  case LITERALD:
    cout << opand.scalar_value.double_value;
    break;
  case LITERALF:
    cout << opand.scalar_value.float_value;
    break;
  case LITERALS:
    cout << opand.string_value;
    break;
  case LITERALA:
    cout << "(array)";
  }
}

void VMStream::PrintTriop(string name, VMInstruction ins) {
  cout << name << "\t";
  PrintOperand(ins.src1);
  cout << ",";
  PrintOperand(ins.src2);
  cout << ",";
  PrintOperand(ins.dst);
  cout << "\n";
}

void VMStream::PrintBiop(string name, VMInstruction ins) {
  cout << name << "\t";
  PrintOperand(ins.src1);
  cout << ",";
  PrintOperand(ins.dst);
  cout << "\n";
}

void VMStream::PrintInstruction(VMInstruction ins) {
  switch(ins.op) {
  case ADD:
    PrintTriop("ADD",ins);
    break;
  case SUBTRACT:
    PrintTriop("SUB",ins);
    break;
  case MTIMES:
    PrintTriop("MML",ins);
    break;
  case MRDIVIDE:
    PrintTriop("MRD",ins);
    break;
  case MLDIVIDE:
    PrintTriop("MLD",ins);
    break;
  case OR:
    PrintTriop("OR ",ins);
    break;
  case AND:
    PrintTriop("AND",ins);
    break;
  case LT:
    PrintTriop("LT ",ins);
    break;
  case LE:
    PrintTriop("LE ",ins);
    break;
  case GT:
    PrintTriop("GT ",ins);
    break;
  case GE:
    PrintTriop("GE ",ins);
    break;
  case EQ:
    PrintTriop("EQ ",ins);
    break;
  case NE:
    PrintTriop("NE ",ins);
    break;
  case TIMES:
    PrintTriop("MUL",ins);
    break;
  case RDIVIDE:
    PrintTriop("RDV",ins);
    break;
  case LDIVIDE:
    PrintTriop("LDV",ins);
    break;
  case UMINUS:
    PrintBiop("NEG",ins);
    break;
  case UPLUS:
    PrintBiop("POS",ins);
    break;
  case NOT:
    PrintBiop("NOT",ins);
    break;
  case MPOWER:
    PrintTriop("MPW",ins);
    break;
  case POWER:
    PrintTriop("POW",ins);
    break;
  case HERMITIAN:
    PrintBiop("HRM",ins);
    break;
  case TRANSPOSE:
    PrintBiop("TRN",ins);
    break;
  case LOADI:
    PrintBiop("LDI",ins);
    break;
  case LOADF:
    PrintBiop("LDF",ins);
    break;
  case LOADD:
    PrintBiop("LDD",ins);
    break;
  case LOADS:
    PrintBiop("LDS",ins);
    break;
  case LOADA:
    PrintBiop("LDA",ins);
    break;
  case MOVE:
    PrintBiop("MOV",ins);
    break;
  case MOVE_DOT:
    PrintTriop("MDT",ins);
    break;
  case PUSH:
    PrintUop("Push",ins);
    break;
  case MOVE_PARENS:
    PrintBiop("MPS",ins);
    break;
  }
}

void VMStream::PrintMe() {
  for (int i=0;i<instr.size();i++)
    PrintInstruction(instr[i]);
}


// If we have something like:
// b = p(5).goo{2}(1,3)
tindex CompileVariableDereference(const tree &t, VMStream &dst, tindex output) {
  tindex input = dst.LookupVariable(t.first().text());
  for (int i=0;i<t.numchildren()-1;i++) {
    tindex out = GetNewTemporary();
    const tree &s(t.child(i+1));
    if (s.is(TOK_PARENS)) {
      for (int j=0;j<s.children();j++)
	dst.EmitOpCode(PUSH,CompileExpression(s.child(j),dst));
      dst.EmitOpCode(MOVE_PARENS,input,out);
    } else if (s.is(TOK_BRACES)) {
    } else if (s.is('.')) {
      dst.EmitOpCode(MOVE_DOT,input,VMOperand(LITERALS,s.first().text()),out);
    } else if (t.child(i+1).is(TOK_DYN)) {
    }
    input = out;
  }
  dst.EmitOpCode(MOVE,input,output);
}

tindex CompileExpression(const tree &t, VMStream &dst) {
  tindex retval = GetNewTemporary();
  switch(t.token()) {
  case TOK_VARIABLE:
    CompileVariableDereference(t,dst,retval);
    break;
  case TOK_INTEGER:
    dst.EmitOpCode(LOADI,VMOperand(LITERALI,ArrayToInt32(t.array())),retval);
    break;
  case TOK_FLOAT:
    dst.EmitOpCode(LOADF,VMOperand(LITERALF,(float) ArrayToDouble(t.array())),retval);
    break;
  case TOK_DOUBLE:
    dst.EmitOpCode(LOADD,VMOperand(LITERALD,ArrayToDouble(t.array())),retval);
    break;
  case TOK_STRING:
    dst.EmitOpCode(LOADS,VMOperand(LITERALS,ArrayToString(t.array())),retval);
    break;
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
    dst.EmitOpCode(LOADA,VMOperand(LITERALA,t.array()),retval);
    break;
  case '+': 
    dst.EmitOpCode(ADD,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case '-': 
    dst.EmitOpCode(SUBTRACT,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case '*': 
    dst.EmitOpCode(MTIMES,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case '/': 
    dst.EmitOpCode(MRDIVIDE,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case '\\': 
    dst.EmitOpCode(MLDIVIDE,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_SOR: 
  case '|': 
    dst.EmitOpCode(OR,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_SAND: 
  case '&': 
    dst.EmitOpCode(AND,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case '<': 
    dst.EmitOpCode(LT,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_LE: 
    dst.EmitOpCode(LE,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case '>': 
    dst.EmitOpCode(GT,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_GE: 
    dst.EmitOpCode(GE,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_EQ: 
    dst.EmitOpCode(EQ,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_NE: 
    dst.EmitOpCode(NE,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_DOTTIMES: 
    dst.EmitOpCode(TIMES,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_DOTRDIV: 
    dst.EmitOpCode(RDIVIDE,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_DOTLDIV: 
    dst.EmitOpCode(LDIVIDE,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_UNARY_MINUS: 
    dst.EmitOpCode(UMINUS,CompileExpression(t.first(),dst),retval);
    break;
  case TOK_UNARY_PLUS: 
    dst.EmitOpCode(UPLUS,CompileExpression(t.first(),dst),retval);
    break;
  case '~': 
    dst.EmitOpCode(NOT,CompileExpression(t.first(),dst),retval);
    break;
  case '^': 
    dst.EmitOpCode(MPOWER,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case TOK_DOTPOWER: 
    dst.EmitOpCode(POWER,CompileExpression(t.first(),dst),CompileExpression(t.second(),dst),retval);
    break;
  case '\'': 
    dst.EmitOpCode(HERMITIAN,CompileExpression(t.first(),dst),retval);
    break;
  case TOK_DOTTRANSPOSE: 
    dst.EmitOpCode(TRANSPOSE,CompileExpression(t.first(),dst),retval);
    break;
  default:
    throw Exception("Unrecognized expression!");
  }
  return retval;
}

void CompileAssignmentStatement(const tree &t, VMStream &dst, bool printIt) {
  tindex Variable = dst.LookupVariable(t.first().first().text());
  tindex Expression = CompileExpression(t.second(),dst);
  dst.EmitOpCode(MOVE,Expression,Variable);
}

void CompileStatementType(const tree &t, VMStream &dst, bool printIt) {
  // check the debug flag
  switch(t.token()) {
  case '=':
    CompileAssignmentStatement(t,dst,printIt);
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void CompileStatement(const tree &t, VMStream &dst) {
  if (t.is(TOK_QSTATEMENT))
    CompileStatementType(t.first(),dst,false);
  else if (t.is(TOK_STATEMENT))
    CompileStatementType(t.first(),dst,true);
  else 
    throw Exception("Unexpected statement type!\n");
}

void CompileBlock(const tree &t, VMStream &dst) {
  treeVector statements(t.children());
  for (treeVector::iterator i=statements.begin(); i!=statements.end(); i++) 
    CompileStatement(*i,dst);
}

void CompileToVMStream(const tree &t, VMStream &dst) {
  try {
    CompileBlock(t,dst);
  } catch (Exception &e) {
  }
}
