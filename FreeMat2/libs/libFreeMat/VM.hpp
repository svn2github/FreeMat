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
  LOADI,
  LOADF,  
  LOADD,
  LOADS,
  LOADA,
  MOVE,
  MOVE_DOT,
  MOVE_PARENS,
  PUSH,
  POP
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
  LITERALI,
  LITERALD,
  LITERALF,
  LITERALS,
  LITERALA,
  ADDRESS,
  REGISTER
} VMOperandType;

typedef union {
} VMOperandValue;

class VMOperand {
  friend class VMStream;
  friend class VMInstruction;
  VMOperandType type;
  union {
    Array  *address;
    uint8  uint8_value;
    uint16 uint16_value;
    uint32 uint32_value;
    uint64 uint64_value;
    int8   int8_value;
    int16  int16_value;
    int32  int32_value;
    int64  int64_value;
    float  float_value;
    double double_value;
    float  complex_value[2];
    double dcomplex_value[2];
  } scalar_value;
  string string_value;
  Array  array_value;
public:
  VMOperand();
  VMOperand(VMOperandType type_t, tindex value_t);
  VMOperand(VMOperandType type_t, int32 value_t);
  VMOperand(VMOperandType type_t, double value_t);
  VMOperand(VMOperandType type_t, float value_t);
  VMOperand(VMOperandType type_t, string value_t);
  VMOperand(VMOperandType type_t, Array value_t);
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
  VMOpcode op;
  VMOperand src1;
  VMOperand src2;
  VMOperand dst;
public:
  VMInstruction(VMOpcode op_t, VMOperand src1_t, VMOperand src2_t, VMOperand dst_t);
};

class VMStream {
  vector<VMInstruction> instr;
  map<string,tindex> vars;
public:
  void EmitOpCode(VMOpcode, tindex src1, tindex src2, tindex dst);
  void EmitOpCode(VMOpcode, tindex src1, VMOperand src2, tindex dst);
  void EmitOpCode(VMOpcode, tindex src1, tindex dst);
  void EmitOpCode(VMOpcode, VMOperand value, tindex dst);
  tindex LookupVariable(string name);
  string GetAliasName(tindex);
  void PrintTriop(string name, VMInstruction ins);
  void PrintBiop(string name, VMInstruction ins);
  void PrintInstruction(VMInstruction ins);
  void PrintMe();
  void PrintOperand(VMOperand value);
};

void CompileToVMStream(const tree &t, VMStream &dst);

#endif
