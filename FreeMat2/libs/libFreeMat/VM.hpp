#ifndef __VM_hpp__
#define __VM_hpp__

#include "Array.hpp"
#include "Tree.hpp"
#include <map>

using namespace std;

typedef uint32 tindex;

/**
 * The opcodes are enumerated here:
 */
typedef enum {
  ADD,
  SUBTRACT,
  MTIMES,
  MRDIVIDE,
  MLDIVIDE,
  OR,
  AND,
  LT,
  LE,
  GT,
  GE,
  EQ,
  NE,
  TIMES,
  RDIVIDE,
  LDIVIDE,
  UMINUS,
  UPLUS,
  NOT,
  MPOWER,
  POWER,
  HERMITIAN,
  TRANSPOSE,
  MOVE,
  MOVE_DOT,
  MOVE_DYN,
  MOVE_PARENS,
  MOVE_BRACES,
  PUSH,
  POP,
  DCOLON,
  UCOLON,
  JIT,
  RETURN,
  NEWFRAME,
  CALL
} VMOpcode;

/**
 * An operand could be:
 *   1. Literal
 *   2. Address of a Variable
 *   3. Temporary (stack location)
 * In each case additional information is needed.
 * For a literal, the value is needed.
 * For a variable, the location where 
 * the variable is stored is needed.
 * For a temporary, and index into the stack is needed.
 *
 */
typedef enum {
  NONE,
  LITERAL,
  REGISTER
} VMOperandType;

class VMOperand {
  friend class VMStream;
  friend class VMInstruction;
  friend class VM;
  VMOperandType type;
  tindex value;
public:
  VMOperand();
  VMOperand(VMOperandType type_t, tindex value_t);
};

/**
 * A base instruction in the VM.  It consists of
 * an opcode, and three operands.  The opcode
 * indicates the nature of the instruction, and
 * the operands indicate the source and destination
 * of the operations.
 */
class VMInstruction {
  friend class VMStream;
  friend class VM;
  VMOpcode op;
  VMOperand src1;
  VMOperand src2;
  VMOperand dst;
public:
  VMInstruction(VMOpcode op_t, VMOperand src1_t, VMOperand src2_t, VMOperand dst_t);
};

class VMStream {
  friend class VM;
  vector<VMInstruction> instr;
  vector<Array> literals;
  map<string,tindex> vars;
  tindex TempCount;
public:
  VMStream();
  tindex GetNewTemporary();
  uint32 GetLineNumber();
  void EmitOpCode(VMOpcode, tindex src1, tindex src2, tindex dst);
  void EmitOpCode(VMOpcode, tindex src1, VMOperand src2, tindex dst);
  void EmitOpCode(VMOpcode, VMOperand src1, VMOperand src2, tindex dst);
  void EmitOpCode(VMOpcode, tindex src1, tindex dst);
  void EmitOpCode(VMOpcode, tindex src1);
  void EmitOpCode(VMOpcode);
  void EmitOpCode(VMOpcode, VMOperand value, tindex dst);
  bool IsVariableDefined(string name);
  tindex LookupVariable(string name);
  tindex AllocateLiteral(Array val);
  string GetAliasName(tindex);
  string GetLiteralString(tindex);
  void PrintTriop(string name, VMInstruction ins);
  void PrintBiop(string name, VMInstruction ins);
  void PrintUop(string name, VMInstruction ins);
  void PrintInstruction(VMInstruction ins);
  void PrintMe();
  void PrintOperand(VMOperand value);
};

void CompileToVMStream(const tree &t, VMStream &dst);

void CustomStream(VMStream &dst);

class VM {
  vector<Array> symtab;
  vector<ArrayVector> vstack;
  VMStream mycode;
  int ip;

  VMOpcode OpCode();
  const Array& Op1();
  const Array& Op2();
  const Array& DecodeOperand(const VMOperand&);
  tindex Dst();
  Array Parens(Array var);
  const Array& Back();
  void Pop();
  void Push(const Array & a);
  void NewFrame();
  void DeleteFrame();
  ArrayVector Frame();
public:
  void Run(const VMStream &code);
  void Exec();
  void DumpVars();
};

#endif
