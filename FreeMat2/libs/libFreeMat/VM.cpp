#include "VM.hpp"
#include "Print.hpp"
#include "Math.hpp"

VMInstruction::VMInstruction(VMOpcode op_t, VMOperand src1_t, VMOperand src2_t, VMOperand dst_t) :
op(op_t), src1(src1_t), src2(src2_t), dst(dst_t) {
}

void VMStream::EmitOpCode(VMOpcode code, VMOperand value, tindex dest) {
  instr.push_back(VMInstruction(code,value,VMOperand(),VMOperand(REGISTER,dest)));
}

void VMStream::EmitOpCode(VMOpcode code, tindex src, tindex dest) {
  instr.push_back(VMInstruction(code,VMOperand(REGISTER,src),VMOperand(),VMOperand(REGISTER,dest)));
}

void VMStream::EmitOpCode(VMOpcode code, tindex src) {
  instr.push_back(VMInstruction(code,VMOperand(REGISTER,src),VMOperand(),VMOperand()));
}

void VMStream::EmitOpCode(VMOpcode code) {
  instr.push_back(VMInstruction(code,VMOperand(),VMOperand(),VMOperand()));
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

void VM::DumpVars() {
  for (map<string,tindex>::iterator i=mycode.vars.begin(); i!= mycode.vars.end(); i++) 
    cout << "Variable " << i->first << " = " << ArrayToPrintableString(symtab[i->second]) << "\n";
}

string VMStream::GetAliasName(tindex n) {
  for (map<string,tindex>::iterator i=vars.begin(); i!= vars.end(); i++) {
    if (i->second == n) return (i->first);
  }
  return string("$") + n;
}

VMStream::VMStream() {
  TempCount = 0;
}

VMOperand::VMOperand() {
  type = NONE;
}

VMOperand::VMOperand(VMOperandType type_t, tindex value_t) {
  type = type_t;
  value = value_t;
}

tindex VMStream::GetNewTemporary() {
  return TempCount++;
}

void VMStream::PrintOperand(VMOperand opand) {
  switch(opand.type) {
  case NONE:
    cout << "none";
    break;
  case REGISTER:
    cout << GetAliasName(opand.value);
    break;
  case LITERAL:
    cout << GetLiteralString(opand.value);
    break;
  }
}

string VMStream::GetLiteralString(tindex ndx) {
  return ArrayToPrintableString(literals[ndx]);
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

void VMStream::PrintUop(string name, VMInstruction ins) {
  cout << name << "\t";
  PrintOperand(ins.src1);
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
  case MOVE:
    PrintBiop("MOV",ins);
    break;
  case MOVE_DOT:
    PrintTriop("MDT",ins);
    break;
  case MOVE_DYN:
    PrintTriop("MDN",ins);
    break;
  case PUSH:
    PrintUop("PSH",ins);
    break;
  case MOVE_PARENS:
    PrintTriop("MPS",ins);
    break;
  case MOVE_BRACES:
    PrintTriop("MBS",ins);
    break;
  case UCOLON:
    PrintTriop("CLN",ins);
    break;
  case DCOLON:
    PrintUop("DCL",ins);
    break;
  case JIT:
    PrintTriop("JIT",ins);
    break;
  case RETURN:
    cout << "RET\n";
    break;
  }
}

void VMStream::PrintMe() {
  cout << "****************************\n";
  for (int i=0;i<instr.size();i++)
    PrintInstruction(instr[i]);
  cout << "****************************\n";
}

tindex VMStream::AllocateLiteral(Array val) {
  literals.push_back(val);
  return (literals.size()-1);
}

tindex CompileExpression(const tree &t, VMStream &dst);

// If we have something like:
// b = p(5).goo{2}(1,3)
tindex CompileVariableDereference(const tree &t, VMStream &dst, tindex output) {
  tindex input = dst.LookupVariable(t.first().text());
  for (int i=0;i<t.numchildren()-1;i++) {
    tindex out = dst.GetNewTemporary();
    const tree &s(t.child(i+1));
    if (s.is(TOK_PARENS)) {
      for (int j=0;j<s.numchildren();j++)
	dst.EmitOpCode(PUSH,CompileExpression(s.child(j),dst));
      dst.EmitOpCode(MOVE_PARENS,input,
		     VMOperand(LITERAL,
			       dst.AllocateLiteral(Array::int32Constructor((int32)s.numchildren()))),out);
    } else if (s.is(TOK_BRACES)) {
      for (int j=0;j<s.numchildren();j++)
	dst.EmitOpCode(PUSH,CompileExpression(s.child(j),dst));
      dst.EmitOpCode(MOVE_BRACES,input,
		     VMOperand(LITERAL,
			       dst.AllocateLiteral(Array::int32Constructor((int32)s.numchildren()))),out);
    } else if (s.is('.')) {
      dst.EmitOpCode(MOVE_DOT,input,
		     VMOperand(LITERAL,
			       dst.AllocateLiteral(Array::stringConstructor(s.first().text()))),out);
    } else if (s.is(TOK_DYN)) {
      dst.EmitOpCode(MOVE_DYN,input,CompileExpression(s.first(),dst),out);
    }
    input = out;
  }
  dst.EmitOpCode(MOVE,input,output);
}

tindex CompileExpression(const tree &t, VMStream &dst) {
  tindex retval = dst.GetNewTemporary();
  switch(t.token()) {
  case TOK_VARIABLE:
    CompileVariableDereference(t,dst,retval);
    break;
  case TOK_INTEGER:
  case TOK_FLOAT:
  case TOK_DOUBLE:
  case TOK_STRING:
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
    dst.EmitOpCode(MOVE,VMOperand(LITERAL,dst.AllocateLiteral(t.array())),retval);
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
  case ':':
    if (t.first().is(':')) {
      dst.EmitOpCode(PUSH,CompileExpression(t.first().first(),dst));
      dst.EmitOpCode(PUSH,CompileExpression(t.first().second(),dst));
      dst.EmitOpCode(PUSH,CompileExpression(t.second(),dst));
      dst.EmitOpCode(DCOLON,retval);
    } else {
      dst.EmitOpCode(UCOLON,CompileExpression(t.first(),dst),
		     CompileExpression(t.second(),dst),retval);
    }
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

void CompileBlock(const tree &t, VMStream &dst);

void CompileForStatement(const tree &t, VMStream &dst) {
  // There are two cases to consider... first is the case of 
  // for <i>=start:ndx:stop
  if (t.first().is('=') &&
      t.first().second().is(':') &&
      t.first().second().first().is(TOK_INTEGER) &&
      t.first().second().second().is(TOK_INTEGER)) {
    string varname(t.first().first().text());
    int32 lim1(ArrayToInt32(t.first().second().first().array()));
    int32 lim2(ArrayToInt32(t.first().second().second().array()));
    if (lim2<lim1) return;
    tindex variable = dst.LookupVariable(varname);
    tindex cmp = dst.GetNewTemporary();
    dst.EmitOpCode(MOVE,VMOperand(LITERAL,dst.AllocateLiteral(Array::int32Constructor(lim1))),variable);
    int32 lineno = dst.GetLineNumber()+1;
    CompileBlock(t.second(),dst);
    dst.EmitOpCode(ADD,variable,
		   VMOperand(LITERAL,dst.AllocateLiteral(Array::int32Constructor((int32)1))),variable);
    dst.EmitOpCode(LE,variable,VMOperand(LITERAL,dst.AllocateLiteral(Array::int32Constructor((int32)lim2))),cmp);
    dst.EmitOpCode(JIT,cmp,VMOperand(LITERAL,dst.AllocateLiteral(Array::int32Constructor((int32)lineno))),cmp);
  }
}

void CustomStream(VMStream &dst) {
  tindex m = dst.LookupVariable("m");
  tindex n = dst.LookupVariable("n");
  tindex one = dst.AllocateLiteral(Array::int32Constructor(1));
  tindex zero = dst.AllocateLiteral(Array::int32Constructor(0));
  tindex three = dst.AllocateLiteral(Array::int32Constructor(3));
  tindex million = dst.AllocateLiteral(Array::int32Constructor(1000000));
  tindex tst = dst.GetNewTemporary();
  dst.EmitOpCode(MOVE,VMOperand(LITERAL,zero),m);
  dst.EmitOpCode(MOVE,VMOperand(LITERAL,one),n);
  dst.EmitOpCode(ADD,m,VMOperand(LITERAL,one),m);
  dst.EmitOpCode(ADD,n,VMOperand(LITERAL,one),n);
  dst.EmitOpCode(LE,n,VMOperand(LITERAL,million),tst);
  dst.EmitOpCode(JIT,tst,VMOperand(LITERAL,three),tst);
  dst.EmitOpCode(RETURN);
}

uint32 VMStream::GetLineNumber() {
  return instr.size();
}

void CompileStatementType(const tree &t, VMStream &dst, bool printIt) {
  // check the debug flag
  switch(t.token()) {
  case '=':
    CompileAssignmentStatement(t,dst,printIt);
    break;
  case TOK_FOR:
    CompileForStatement(t,dst);
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
    cout << "Error: " << e.getMessageCopy() << "\n";
  }
  dst.EmitOpCode(RETURN);
}

VMOpcode VM::OpCode() {
  return mycode.instr[ip].op;
}

const Array & VM::Op1() {
  return DecodeOperand(mycode.instr[ip].src1);
}

const Array & VM::Op2() {
  return DecodeOperand(mycode.instr[ip].src2);
}

tindex VM::Dst() {
  return mycode.instr[ip].dst.value;
}

const Array & VM::DecodeOperand(const VMOperand & op) {
  if (op.type == REGISTER)
    return symtab[op.value];
  else if (op.type == LITERAL)
    return mycode.literals[op.value];
  throw Exception("Error decoding operand!\n");
}

void VM::Run(const VMStream &code) {
  ip = 0;
  mycode = code;
  for (int i=0;i<100;i++)
    symtab.push_back(Array::emptyConstructor());
  while (1) {
    switch(OpCode()) {
    case ADD: 
      symtab[Dst()] = Add(Op1(),Op2());
      ip++;
      break;
    case SUBTRACT:
      symtab[Dst()] = Subtract(Op1(),Op2());
      ip++;
      break;
    case MTIMES:
      symtab[Dst()] = Multiply(Op1(),Op2());
      ip++;
      break;
    case MRDIVIDE:
      symtab[Dst()] = RightDivide(Op1(),Op2());
      ip++;
      break;
    case MLDIVIDE:
      symtab[Dst()] = LeftDivide(Op1(),Op2());
      ip++;
      break;
    case OR:
      symtab[Dst()] = Or(Op1(),Op2());
      ip++;
      break;
    case AND:
      symtab[Dst()] = And(Op1(),Op2());
      ip++;
      break;
    case LT:
      symtab[Dst()] = LessThan(Op1(),Op2());
      ip++;
      break;
    case LE:
      symtab[Dst()] = LessEquals(Op1(),Op2());
      ip++;
      break;
    case GT:
      symtab[Dst()] = GreaterThan(Op1(),Op2());
      ip++;
      break;
    case GE:
      symtab[Dst()] = GreaterEquals(Op1(),Op2());
      ip++;
      break;
    case EQ:
      symtab[Dst()] = Equals(Op1(),Op2());
      ip++;
      break;
    case NE:
      symtab[Dst()] = NotEquals(Op1(),Op2());
      ip++;
      break;
    case TIMES:
      symtab[Dst()] = DotMultiply(Op1(),Op2());
      ip++;
      break;
    case RDIVIDE:
      symtab[Dst()] = DotRightDivide(Op1(),Op2());
      ip++;
      break;
    case LDIVIDE:
      symtab[Dst()] = DotLeftDivide(Op1(),Op2());
      ip++;
      break;
    case UMINUS:
      symtab[Dst()] = Negate(Op1());
      ip++;
      break;
    case UPLUS:
      symtab[Dst()] = Plus(Op1());
      ip++;
      break;
    case NOT:
      symtab[Dst()] = Not(Op1());
      ip++;
      break;
    case MPOWER:
      symtab[Dst()] = Power(Op1(),Op2());
      ip++;
      break;
    case POWER:
      symtab[Dst()] = DotPower(Op1(),Op2());
      ip++;
      break;
    case HERMITIAN:
      symtab[Dst()] = Transpose(Op1());
      ip++;
      break;
    case TRANSPOSE:
      symtab[Dst()] = DotTranspose(Op1());
      ip++;
      break;
    case MOVE:
      symtab[Dst()] = Op1();
      ip++;
      break;
//     case MOVE_DOT:
//       symtab[Dst()] = DOT(Op1(),Op2());
//       ip++;
//       break;
//     case MOVE_DYN:
//       symtab[Dst()] = DYN(Op1(),Op2());
//       ip++;
//       break;
//     case MOVE_PARENS:
//       symtab[Dst()] = PARENS(Op1(),Op2());
//       ip++;
//       break;
//     case MOVE_BRACES:
//       symtab[Dst()] = BRACES(Op1(),Op2());
//       ip++;
//       break;
    case PUSH:
      vstack.push_back(Op1());
      ip++;
      break;
    case POP:
      vstack.pop_back();
      ip++;
      break;
    case DCOLON: 
      {
	Array a3(vstack.back()); vstack.pop_back();
	Array a2(vstack.back()); vstack.pop_back();
	Array a1(vstack.back()); vstack.pop_back();
	symtab[Dst()] = DoubleColon(a1,a2,a3);
      }
      ip++;
      break;
    case UCOLON:
      symtab[Dst()] = UnitColon(Op1(),Op2());
      ip++;
      break;
    case JIT:
      if (!Op1().isRealAllZeros())
	ip = ArrayToInt32(Op2()) - 1;
      else
	ip++;
      break;
    case RETURN:
      return;
    }
   }
}
