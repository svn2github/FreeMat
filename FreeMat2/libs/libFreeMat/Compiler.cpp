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

#include "Compiler.hpp"
#include "Tree.hpp"
#include <QString>
#include <QTextStream>
#include <QFile>
#include <set>

static int indent_level = 0;
static QFile *fp;
static QTextStream *ts;
static Interpreter *eval;
stringVector localIdents;
stringVector returnVals;
int forDepth;

bool IsVariableDefined(string varname) {
  for (int i=0;i<localIdents.size();i++)
    if (localIdents[i] == varname)
      return true;
  return false;
}

void EmitIndent() {
  for (int i=0;i<indent_level;i++)
    (*ts) << "   ";
}

void Emit(QString t) {
  (*ts) << t;
}

void EmitExpression(const tree &t);

void EmitMultiExpression(const tree &t);

void EmitSingleDeref(const tree &t) {
  if (t.is(TOK_PARENS)) {
    Emit(".getNDimSubset(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is(TOK_BRACES)) {
    Emit(".getNDimContents(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is('.')) {
    Emit(QString(".getField(") + QString::fromStdString(t.first().text()) + ")");
  } else if (t.is(TOK_DYN)) {
    Emit(".getField(ArrayToString(");
    EmitExpression(t.first());
    Emit("))");
  }
}

void EmitMultiDeref(const tree &t) {
  if (t.is(TOK_PARENS)) {
    Emit(".getNDimSubset(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is(TOK_BRACES)) {
    Emit(".getNDimContentsAsList(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is('.')) {
    Emit(QString(".getFieldAsList(") + QString::fromStdString(t.first().text()) + ")");
  } else if (t.is(TOK_DYN)) {
    Emit(".getFieldAsList(ArrayToString(");
    EmitExpression(t.first());
    Emit("))");
  }
}

void EmitMultiExpressionVariable(const tree &t) {
  Emit(QString::fromStdString(t.first().text()));
  for (int index=1;index<t.numchildren()-1;index++)
    EmitSingleDeref(t.child(index));
  if (t.numchildren() > 1)
    EmitMultiDeref(t.last());
}

void EmitMultiExpression(const tree &t) {
  if (t.is(TOK_VARIABLE)) {
    if (IsVariableDefined(t.first().text()))
      EmitMultiExpressionVariable(t);
//     else
//       EmitMultiExpressionFunction(t);
  } else if (!t.is(TOK_KEYWORD))
    EmitExpression(t);
}

void EmitBinaryOperator(const tree &t, QString opname) {
  Emit(QString("%1").arg(opname));
  Emit("(");
  EmitExpression(t.first());
  Emit(",");
  EmitExpression(t.second());
  Emit(")");
}

void EmitUnaryOperator(const tree &t, QString opname) {
  Emit(QString("%1").arg(opname));
  Emit("(");
  EmitExpression(t.first());
  Emit(")");
}

void EmitRHS(const tree &t) {
  // Check our list of defined variables
  if (IsVariableDefined(t.first().text())) {
    Emit(QString::fromStdString(t.first().text()));
    for (int index=1;index<t.numchildren();index++)
      EmitSingleDeref(t.child(index));
  } else {
    Emit("ToSingleArray(");
    Emit(QString("%1Function(").arg(QString::fromStdString(t.first().text())));
    Emit("1,");
    Emit("ArrayVector()");
    for (int i=0;i<t.second().numchildren();i++) {
      Emit(" << ");
      EmitMultiExpression(t.second().child(i));
    }
    Emit("))");
  }
}

void EmitExpression(const tree &t) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    EmitRHS(t);
    return;
  case TOK_INTEGER:
    Emit("Array::int32Constructor(");
    Emit(QString::fromStdString(t.text()));
    Emit(")");
    return;
  case TOK_FLOAT:
    Emit("Array::floatConstructor(");
    Emit(QString::fromStdString(t.text()));
    Emit(")");
    return;
  case TOK_DOUBLE:
    Emit("Array::doubleConstructor(");
    Emit(QString::fromStdString(t.text()));
    Emit(")");
    return;
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
    throw Exception("Unimplemented");
  case TOK_STRING:
    Emit(QString("\"%1\"").arg(QString::fromStdString(t.text())));
    return;
  case TOK_END:
    throw Exception("Unimplemented");
  case ':':
    if (t.numchildren() == 0) {
      Emit(QString("Array::stringConstructor(\":\")"));
    } else if (t.first().is(':')) {
      Emit("DoubleColon(");
      EmitExpression(t.first().first());
      Emit(",");
      EmitExpression(t.first().second());
      Emit(",");
      EmitExpression(t.second());
      Emit(")");
    } else {
      Emit("UnitColon(");
      EmitExpression(t.first());
      Emit(",");
      EmitExpression(t.second());
      Emit(")");
    }
    break;
  case TOK_MATDEF: 
    //    return matrixDefinition(t); 
    break;
  case TOK_CELLDEF: 
    //    return cellDefinition(t); 
    break;
  case '+': 
    EmitBinaryOperator(t,"Add");
    break;
  case '-': 
    EmitBinaryOperator(t,"Subtract");
    break;
  case '*': 
    EmitBinaryOperator(t,"Multiply");
    break;
  case '/': 
    EmitBinaryOperator(t,"RightDivide");
    break;
  case '\\': 
    EmitBinaryOperator(t,"LeftDivide");
    break;
  case TOK_SOR: 
  case '|': 
    throw Exception("Unimplemented");    
    break;
  case TOK_SAND: 
  case '&':  
    throw Exception("Unimplemented");    
    break;
  case '<': 
    EmitBinaryOperator(t,"LessThan");
    break;
  case TOK_LE: 
    EmitBinaryOperator(t,"LessEquals");
    break;
  case '>': 
    EmitBinaryOperator(t,"GreaterThan");
    break;
  case TOK_GE: 
    EmitBinaryOperator(t,"GreaterEquals");
    break;
  case TOK_EQ: 
    EmitBinaryOperator(t,"Equals");
    break;
  case TOK_NE: 
    EmitBinaryOperator(t,"NotEquals");
    break;
  case TOK_DOTTIMES: 
    EmitBinaryOperator(t,"DotMultiply");
    break;
  case TOK_DOTRDIV: 
    EmitBinaryOperator(t,"DotRightDivide");
    break;
  case TOK_DOTLDIV: 
    EmitBinaryOperator(t,"DotLeftDivide");
    break;
  case TOK_UNARY_MINUS: 
    EmitUnaryOperator(t,"Negate");
    break;
  case TOK_UNARY_PLUS: 
    EmitUnaryOperator(t,"Plus");
    break;
  case '~': 
    EmitUnaryOperator(t,"Not");
    break;
  case '^': 
    EmitBinaryOperator(t,"Power");
    break;
  case TOK_DOTPOWER: 
    EmitBinaryOperator(t,"DotPower");
    break;
  case '\'': 
    EmitUnaryOperator(t,"Transpose");
    break;
  case TOK_DOTTRANSPOSE: 
    EmitUnaryOperator(t,"DotTranspose");
    break;
  case '@':
    throw Exception("Unimplemented");    
  default:
    throw Exception("Unrecognized expression!");
  }
}

void EmitArgumentAliases(MFunctionDef *m_def) {
  localIdents.clear();
  EmitIndent();
  Emit("Array ");
  for (int i=0;i<m_def->arguments.size();i++) {
    Emit(QString::fromStdString(m_def->arguments[i]));
    if (i < m_def->arguments.size()-1)
      Emit(",");
  }
  Emit(";\n");
  for (int i=0;i<m_def->arguments.size();i++) {
    EmitIndent();
    Emit(QString("if (args.size() >= %1) %2 = args[%1];\n").arg(i).arg(QString::fromStdString(m_def->arguments[i])));
    localIdents.push_back(m_def->arguments[i]);
  }
  returnVals = m_def->returnVals;
}

// a(1,2).foo.goo{3} = rhs
//
// t1_ = a(1,2)
// t2_ = t1_.foo
// t3_ = t2_.goo
// t3_{3} = rhs
// 
//
//  t_ = a(1,2).foo.goo
//  t_{3} = rhs;
//  a(1,2).foo.goo = t_;
//
//

void EmitAssign(string name, const tree &s) {
  if (s.is(TOK_PARENS)) {
    Emit(QString("%1.setNDimSubset(").arg(QString::fromStdString(name)));
    Emit("ArrayVector()");
    for (int p=0;p<s.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(s.child(p));
    }
    Emit(",_rhs)");
  } else if (s.is(TOK_BRACES)) {
    Emit(QString("%1.setNDimContentsAsList(").arg(QString::fromStdString(name)));
    Emit("ArrayVector()");
    for (int p=0;p<s.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(s.child(p));
    }
    Emit(",_rhs)");
  } else if (s.is('.')) {
    Emit(QString("%1.setFieldAsList(").arg(QString::fromStdString(name)));
    Emit(QString("%2,_rhs)").arg(QString::fromStdString(s.first().text())));
  } else if (s.is(TOK_DYN)) {
  }
}

void EmitAssignment(const tree &t, bool printIt) {
  const tree &var(t.first());
  const tree &rhs(t.second());
  string name(var.first().text());
  if (!IsVariableDefined(name)) {
    Emit(QString("Array %1;\n").arg(QString::fromStdString(name)));
    localIdents.push_back(name);
    EmitIndent();
  }
  if (var.numchildren() == 1) {
    Emit(QString("%1 = ").arg(QString::fromStdString(name)));
    EmitExpression(rhs);
    Emit(";\n");
    return;
  }
  if (var.numchildren() == 2) {
    Emit("{\n");
    indent_level++;
    EmitIndent();
    Emit(QString("Array _rhs("));
    EmitExpression(rhs);
    Emit(");\n");
    EmitIndent();
    EmitAssign(name,var.second());
    Emit(";\n");
    indent_level--;
    EmitIndent();
    Emit("}\n");
    return;
  }
}

void EmitBlock(const tree &code);

void EmitReturn() {
  Emit("return ArrayVector()");
  for (int i=0;i<returnVals.size();i++) {
    Emit(" << ");
    if (IsVariableDefined(returnVals[i]))
      Emit(QString::fromStdString(returnVals[i]));
    else
      Emit("Array::emptyConstructor()");
  }
  Emit(";\n");
}

void EmitForStatement(const tree &t) {
  QString loopvar = QString("_loop%1").arg(forDepth);
  QString indexvar = QString("_index%1").arg(forDepth);
  forDepth++;
  QString myloopvar = QString::fromStdString(t.first().first().text());
  Emit(QString("Array %1 = ").arg(indexvar));
  EmitExpression(t.first().second());
  Emit(";\n");
  EmitIndent();
  Emit(QString("for (int %1=1;%1<=%2.getLength();%1++) {\n").arg(loopvar).arg(indexvar));
  indent_level++;
  EmitIndent();
  Emit(QString("Array %1(%2.getVectorSubset(Array::int32Constructor(%3)));\n").arg(myloopvar).arg(indexvar).arg(loopvar));
  localIdents.push_back(t.first().first().text());
  EmitBlock(t.second());
  indent_level--;
  EmitIndent();
  Emit("}\n");
  forDepth--;
}

void EmitWhileStatement(const tree &t) {
  Emit("while (!(");
  EmitExpression(t.first());
  Emit(").isRealAllZeros())\n");
  EmitIndent();
  Emit("{\n");
  indent_level++;
  EmitBlock(t.second());
  indent_level--;
  EmitIndent();
  Emit("}\n");
}

void EmitIfStatement(const tree &t) {
  Emit("if (!(");
  EmitExpression(t.first());
  Emit(").isRealAllZeros())\n");
  EmitIndent();
  Emit("{\n");
  indent_level++;
  EmitBlock(t.second());
  indent_level--;
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    EmitIndent();
    Emit("}\n");
    EmitIndent();
    Emit("else if (!(");
    EmitExpression(t.child(n).first());
    Emit(").isRealAllZeros()))\n");
    EmitIndent();
    Emit("{\n");
    indent_level++;
    EmitBlock(t.child(n).second());
    indent_level--;
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    EmitIndent();
    Emit("}\n");
    EmitIndent();
    Emit("else\n");
    EmitIndent();
    Emit("{\n");
    EmitIndent();
    EmitBlock(t.last().first());
    EmitIndent();
    Emit("}\n");
  } else {
    EmitIndent();
    Emit("}\n");
  }
}

void EmitStatementType(const tree &t, bool printIt) {
  switch(t.token()) {
  case '=': 
    EmitAssignment(t,printIt);
    break;
  case TOK_MULTI:
    //     multiFunctionCall(t,printIt);
    break;
  case TOK_SPECIAL:
    //     specialFunctionCall(t,printIt);
    break;
  case TOK_FOR:
    EmitForStatement(t);
    break;
  case TOK_WHILE:
    EmitWhileStatement(t);
    break;
  case TOK_IF:
    EmitIfStatement(t);
    break;
  case TOK_BREAK:
    Emit("break;\n");
    break;
  case TOK_CONTINUE:
    Emit("continue;\n");
    break;
  case TOK_DBSTEP:
    break;
  case TOK_DBTRACE:
    break;
  case TOK_RETURN:
    EmitReturn();
    break;
  case TOK_SWITCH:
    //    switchStatement(t);
    break;
  case TOK_TRY:
    //    tryStatement(t);
    break;
  case TOK_QUIT:
    break;
  case TOK_RETALL:
    break;
  case TOK_KEYBOARD:
    break;
  case TOK_GLOBAL:
    //    globalStatement(t);
    break;
  case TOK_PERSISTENT:
    //    persistentStatement(t);
    break;
  case TOK_EXPR:
    //    expressionStatement(t,printIt);
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void EmitStatement(const tree &t) {
  if (t.is(TOK_QSTATEMENT))
    EmitStatementType(t.first(),false);
  else if (t.is(TOK_STATEMENT))
    EmitStatementType(t.first(),true);
  Emit(";\n");
}

void EmitBlock(const tree &code) {
  for (int i=0;i<code.numchildren();i++) {
    EmitIndent();
    EmitStatement(code.child(i));
  }
}

void ProtoCompile(Interpreter *m_eval, MFunctionDef *m_def, string filename) {
  m_def->updateCode(m_eval);
  fp = new QFile(QString::fromStdString(filename));
  eval = m_eval;
  if (!fp->open(QIODevice::WriteOnly))
    throw Exception("Unable to open file " + filename + " for writing.");
  ts = new QTextStream(fp);
  EmitIndent();
  Emit(QString("ArrayVector %1Function(int nargout, const ArrayVector& args) {\n").arg(QString::fromStdString(m_def->name)));
  forDepth = 0;
  indent_level++;
  // Emit aliases for the arguments
  EmitArgumentAliases(m_def);
  EmitBlock(m_def->code);
  EmitIndent();
  EmitReturn();
  indent_level--;
  EmitIndent();
  Emit(QString("}\n"));
  fp->close();
  delete fp;
}

ArrayVector fccFunction(int nargout, const ArrayVector& args, Interpreter* m_eval) {
  for (int i=0;i<args.size();i++) {
    FuncPtr val;
    string name(ArrayToString(args[i]));
    if (!m_eval->lookupFunction(name,val))
      throw Exception("Unable to resolve " + name + " to a function");
    if (!(val->type() == FM_M_FUNCTION))
      throw Exception("Function " + name + " is not an M file (and cannot be compiled");
    ProtoCompile(m_eval,(MFunctionDef*) val,name+".cpp");
  }
  return ArrayVector();
}

static void VariableReferencesList(const tree & t, stringVector& idents) {
  if (t.is(TOK_VARIABLE)) {
    bool exists = false;
    for (int i=0;(i<idents.size());i++) {
      exists = (idents[i] == t.first().text());
      if (exists) break;
    }
    if (!exists)
      idents.push_back(t.first().text());
  }
  for (int i=0;i<t.numchildren();i++)
    VariableReferencesList(t.child(i),idents);
}

class LoopSignature {
  std::set<string> scalars;
  std::set<string> matrices;
public:
  std::set<string> get_scalars() {
    return scalars;
  }
  std::set<string> get_matrices() {
    return matrices;
  }
  void add_scalar(string scalar) {
    scalars.insert(scalar);
  }
  void add_matrix(string matrix) {
    matrices.insert(matrix);
  }
  void remove_scalar(string scalar) {
    scalars.erase(scalar);
  }
  void remove_matrix(string matrix) {
    matrices.erase(matrix);
  }
  void print() {
    for (std::set<string>::const_iterator i = scalars.begin();
	 i!=scalars.end();i++) {
      std::cout << "scalar " << *i << "\r\n";
    }
    for (std::set<string>::const_iterator i = matrices.begin();
	 i!=matrices.end();i++) {
      std::cout << "matrix " << *i << "\r\n";
    }
  }
};


typedef enum {
  boolean,
  unsigned_integer,
  integer,
  single_float,
  double_float,
  pointer
} opcodeClass;

typedef enum {
  ADD,
  SUB,
  SET,
  NOP,
  LOAD,
  STORE,
  MUL,
  LDIV,
  RDIV,
  OR,
  AND,
  XOR,
  LT,
  LE,
  EQ,
  NEQ,
  GE,
  GT,
  JIT,
  JIF,
  JMP,
  CASTI,
  CASTU,
  CASTF,
  CASTD,
  CASTB,
  NEG,
  POS,
  NOT,
  RET
} opcodeType;

typedef union {
  bool b;
  unsigned u;
  int i;
  float f;
  double d;
  void* p;
} registerType;

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
  case boolean: 
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
  case CASTB: return "CASTB";
  case CASTI: return "CASTI";
  case CASTU: return "CASTU";
  case CASTF: return "CASTF";
  case CASTD: return "CASTD";
  case NEG:   return "NEG  ";
  case POS:   return "POS  ";
  case NOT:   return "NOT  ";
  case RET:   return "RET  ";
  }
}

std::string OpCodeClass(opcodeClass topclass) {
  switch(topclass) {
  case boolean: return "BOL";
  case unsigned_integer: return "USN";
  case integer: return "INT";
  case single_float: return "FLT";
  case double_float: return "DBL";
  case pointer: return "PNT";
  }
}

class VMInstruction {
public:
  opcodeType opcode;
  opcodeClass opclass;
  unsigned arg1;
  unsigned arg2;
  unsigned dest;
  registerType literal;
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		unsigned targ1,
		unsigned targ2,
		registerType tlit) : opcode(topcode), opclass(topclass),
				     arg1(targ1), arg2(targ2), dest(tdest),
				     literal(tlit)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		unsigned targ1,
		unsigned targ2) : opcode(topcode), opclass(topclass),
				  arg1(targ1), arg2(targ2), dest(tdest)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		unsigned targ1) : opcode(topcode), opclass(topclass),
				  arg1(targ1), dest(tdest)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		registerType tlit) : opcode(topcode), opclass(topclass),
				     dest(tdest), literal(tlit)
  {}
  VMInstruction(opcodeType topcode,
		registerType tlit) : opcode(topcode), literal(tlit)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		registerType tlit) : opcode(topcode), opclass(topclass),
				     literal(tlit)
  {}
  VMInstruction(opcodeType topcode) : opcode(topcode)
  {}
  void print(std::ostream& o) {
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
};

registerType reg[256];

typedef std::vector<VMInstruction*> VMStream;

static inline void SetOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
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

static inline void AddOp(VMInstruction *op) {
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

static inline void SubOp(VMInstruction *op) {
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

static inline void MulOp(VMInstruction *op) {
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

static inline void RDivOp(VMInstruction *op) {
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

static inline void LDivOp(VMInstruction *op) {
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

static inline void LTOp(VMInstruction *op) {
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

static inline void LEOp(VMInstruction *op) {
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

static inline void EQOp(VMInstruction *op) {
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

static inline void NEQOp(VMInstruction *op) {
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

static inline void GTOp(VMInstruction *op) {
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

static inline void CastBOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
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

static inline void CastUOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
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

static inline void CastIOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
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

static inline void CastFOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
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

static inline void CastDOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
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

static inline void NegOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
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

static inline void NotOp(VMInstruction *op) {
  switch (op->opclass) {
  case boolean:
    reg[op->dest].b = !reg[op->arg1].b;
    return;
  }
  throw Exception("NOT not supported for non-boolean arguments");
}

static inline void GEOp(VMInstruction *op) {
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

static inline void OrOp(VMInstruction *op) {
  reg[op->dest].b = reg[op->arg1].b | reg[op->arg2].b;
}

static inline void AndOp(VMInstruction *op) {
  reg[op->dest].b = reg[op->arg1].b & reg[op->arg2].b;
}

static inline void XorOp(VMInstruction *op) {
  reg[op->dest].b = reg[op->arg1].b ^ reg[op->arg2].b;
}

static inline void LoadOp(VMInstruction *op) {
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

static inline void StoreOp(VMInstruction *op) {
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

void Run(VMStream& data) {
  bool term = false;
  unsigned ip = 0;
  unsigned ops_max = data.size();
  while (ip < ops_max) {
    VMInstruction *op = data[ip++];
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
    case RET:
      return;
    }
  }
}

std::ostream& operator <<(std::ostream& o, VMStream& t) {
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

class RegisterReference {
  unsigned m_index;
  opcodeClass m_type;
public:
  RegisterReference(unsigned t, opcodeClass s) : m_index(t), m_type(s) {}
  RegisterReference() {}
  inline unsigned index() {return m_index;}
  inline opcodeClass type() {return m_type;}
};

static unsigned GetReg() {
  static unsigned reglist = 0;
  return reglist++;
}

class SymbolInformation {
public:
  RegisterReference m_rows;
  RegisterReference m_cols;
  opcodeClass m_type;
  RegisterReference m_base;
  bool m_scalar;
  SymbolInformation(opcodeClass type, unsigned index) :
    m_type(type), m_base(index,type), m_scalar(true) {}
  bool isScalar() {return m_scalar;}
};

SymbolTable<SymbolInformation> symbols;

void JITBlock(VMStream& o, tree t);
RegisterReference JITExpression(VMStream& o, tree t);

RegisterReference JITPromote(VMStream& o, RegisterReference a, opcodeClass c) {
  RegisterReference out(GetReg(),c);
  if (a.type() == boolean) {
    switch(c) {
    case boolean:
      return a;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,boolean,out.index(),a.index());
      return out;
    case integer:
      o << new VMInstruction(CASTI,boolean,out.index(),a.index());
      return a;
    case single_float:
      o << new VMInstruction(CASTF,boolean,out.index(),a.index());
      return out;
    case double_float:
      o << new VMInstruction(CASTD,boolean,out.index(),a.index());
      return out;
    }
  }
  if (a.type() == integer) {
    switch(c) {
    case boolean:
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
    case boolean:
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
    case boolean:
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
    case boolean:
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

RegisterReference JITBooleanOrOperator(VMStream& o, tree t) {
  RegisterReference result(GetReg(),boolean);
  RegisterReference a(JITExpression(o,t.first()));
  RegisterReference atest(JITPromote(o,a,boolean));
  VMInstruction *jump_a_true = new VMInstruction(JIT,unsigned_integer,
						 atest.index(),
						 RTUnsigned(0));
  o << jump_a_true;
  RegisterReference b(JITExpression(o,t.second()));
  RegisterReference btest(JITPromote(o,b,boolean));
  VMInstruction *jump_b_true = new VMInstruction(JIT,unsigned_integer,
						 btest.index(),
						 RTUnsigned(0));
  o << jump_b_true;
  o << new VMInstruction(SET,boolean,result.index(),RTBoolean(false));
  VMInstruction *jump_done = new VMInstruction(JMP,unsigned_integer,
					       RTUnsigned(o.size()+2));
  o << jump_done;
  jump_a_true->literal = RTUnsigned(o.size());
  jump_b_true->literal = RTUnsigned(o.size());
  o << new VMInstruction(SET,boolean,result.index(),RTBoolean(true));
  o << new VMInstruction(NOP);
  return result;
}

RegisterReference JITBooleanNotOperator(VMStream& o, tree t) {
  RegisterReference result(GetReg(),boolean);
  RegisterReference a(JITExpression(o,t.first()));
  RegisterReference atest(JITPromote(o,a,boolean));
  o << new VMInstruction(NOT,boolean,result.index(),atest.index());
  return result;
}

RegisterReference JITBooleanAndOperator(VMStream& o, tree t) {
  RegisterReference result(GetReg(),boolean);
  RegisterReference a(JITExpression(o,t.first()));
  RegisterReference atest(JITPromote(o,a,boolean));
  VMInstruction *jump_a_false = new VMInstruction(JIF,unsigned_integer,
						  atest.index(),
						  RTUnsigned(0));
  o << jump_a_false;
  RegisterReference b(JITExpression(o,t.second()));
  RegisterReference btest(JITPromote(o,b,boolean));
  VMInstruction *jump_b_false = new VMInstruction(JIF,unsigned_integer,
						  btest.index(),
						  RTUnsigned(0));
  o << jump_b_false;
  o << new VMInstruction(SET,boolean,result.index(),RTBoolean(true));
  VMInstruction *jump_done = new VMInstruction(JMP,unsigned_integer,
					       RTUnsigned(o.size()+2));
  o << jump_done;
  jump_a_false->literal = RTUnsigned(o.size());
  jump_b_false->literal = RTUnsigned(o.size());
  o << new VMInstruction(SET,boolean,result.index(),RTBoolean(false));
  o << new VMInstruction(NOP);
  return result;
}

RegisterReference JITComparisonOperator(VMStream& o, tree t, opcodeType op) {
  RegisterReference a(JITExpression(o,t.first()));
  RegisterReference b(JITExpression(o,t.second()));
  opcodeClass outputClass;
  if (a.type() > b.type()) 
    outputClass = a.type();
  else
    outputClass = b.type();
  if (a.type() != outputClass)
    a = JITPromote(o,a,outputClass);
  if (b.type() != outputClass)
    b = JITPromote(o,b,outputClass);
  RegisterReference c(GetReg(),boolean);
  o << new VMInstruction(op,boolean,c.index(),a.index(),b.index());
  return c;
}

RegisterReference JITUnaryOperator(VMStream& o, tree t, opcodeType op) {
  RegisterReference a(JITExpression(o,t.first()));
  RegisterReference c(GetReg(),a.type());
  o << new VMInstruction(op,c.type(),c.index(),a.index());
  return c;
}

RegisterReference JITBinaryOperator(VMStream& o, tree t, opcodeType op) {
  RegisterReference a(JITExpression(o,t.first()));
  RegisterReference b(JITExpression(o,t.second()));
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
  RegisterReference c(GetReg(),outputClass);
  o << new VMInstruction(op,c.type(),c.index(),a.index(),b.index());
  return c;
}

RegisterReference JITRHS(VMStream& o, tree t) { 
  SymbolInformation *v = symbols.findSymbol(t.first().text());
  if (!v) 
    throw Exception("Undefined variable reference:" + t.first().text());
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
    RegisterReference offset_n(JITExpression(o,s.first()));
    RegisterReference offset_u(GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,offset_n.type(),offset_u.index(),offset_n.index());
    RegisterReference val(GetReg(),v->m_type);
    o << new VMInstruction(LOAD,v->m_type,val.index(),v->m_base.index(),offset_u.index());
    return val;
  } else if (s.numchildren() == 2) {
    RegisterReference row_offset_n(JITExpression(o,s.first()));
    RegisterReference row_offset_u(GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,row_offset_n.type(),row_offset_u.index(),row_offset_n.index());
    RegisterReference col_offset_n(JITExpression(o,s.second()));
    RegisterReference col_offset_u(GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,col_offset_n.type(),col_offset_u.index(),col_offset_n.index());
    RegisterReference ndx_offset_u(GetReg(),unsigned_integer);
    o << new VMInstruction(MUL,unsigned_integer,ndx_offset_u.index(),
			   col_offset_u.index(),v->m_rows.index());
    o << new VMInstruction(ADD,unsigned_integer,ndx_offset_u.index(),
			   ndx_offset_u.index(),row_offset_u.index());
    RegisterReference val(GetReg(),v->m_type);
    o << new VMInstruction(LOAD,v->m_type,val.index(),v->m_base.index(),ndx_offset_u.index());
    return val;
  }
  throw Exception("dereference not handled yet...");
}

RegisterReference JITExpression(VMStream& o, tree t) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return JITRHS(o,t);
  case TOK_INTEGER: {
    RegisterReference q(GetReg(),integer);
    o << new VMInstruction(SET,integer,q.index(),
			   RTInteger(ArrayToInt32(t.array())));
    return q;
  }
  case TOK_FLOAT: {
    RegisterReference q(GetReg(),single_float);
    o << new VMInstruction(SET,single_float,q.index(),
			   RTFloat(ArrayToDouble(t.array())));
    return q;
  }
  case TOK_DOUBLE: {
    RegisterReference q(GetReg(),double_float);
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
    throw Exception("Unsupported expression construct!");
  case '+': 
    return JITBinaryOperator(o,t,ADD);
  case '-': 
    return JITBinaryOperator(o,t,SUB);
  case '*': 
  case TOK_DOTTIMES: 
    return JITBinaryOperator(o,t,MUL);
  case '/': 
  case TOK_DOTRDIV: 
    return JITBinaryOperator(o,t,RDIV);
  case '\\': 
  case TOK_DOTLDIV: 
    return JITBinaryOperator(o,t,LDIV);
  case TOK_SOR: 
  case '|': 
    return JITBooleanOrOperator(o,t);
  case TOK_SAND: 
  case '&': 
    return JITBooleanAndOperator(o,t);
  case '<': 
    return JITComparisonOperator(o,t,LT);
  case TOK_LE: 
    return JITComparisonOperator(o,t,LE);
  case '>': 
    return JITComparisonOperator(o,t,GT);
  case TOK_GE: 
    return JITComparisonOperator(o,t,GE);
  case TOK_EQ: 
    return JITComparisonOperator(o,t,EQ);
  case TOK_NE: 
    return JITComparisonOperator(o,t,NEQ);
  case TOK_UNARY_MINUS: 
    return JITUnaryOperator(o,t,NEG);
  case TOK_UNARY_PLUS: 
    return JITUnaryOperator(o,t,POS);
  case '~': 
    return JITBooleanNotOperator(o,t);
    break;
  case '^': 
    //    return DoBinaryOperator(t,Power,"mpower"); 
    break;
  case TOK_DOTPOWER: 
    //    return DoBinaryOperator(t,DotPower,"power"); 
    break;
  case '\'': 
    //    return DoUnaryOperator(t,Transpose,"ctranspose"); 
    break;
  case TOK_DOTTRANSPOSE: 
    //    return DoUnaryOperator(t,DotTranspose,"transpose"); 
    break;
  case '@':
    //    return FunctionPointer(t);
  default:
    std::cout << "******************************************************\r\n";
    t.print();
    throw Exception("Unrecognized expression!");
  }
}

void JITBlock(VMStream& o, tree t);

void JITIfStatement(VMStream& o, tree t) {
  VMStream endInstructions;
  RegisterReference test(JITExpression(o,t.first()));
  VMInstruction *jmp1 = new VMInstruction(JIF,unsigned_integer,
					  test.index(),RTUnsigned(0));
  o << jmp1;
  VMInstruction *prev_fixup = jmp1;
  JITBlock(o,t.second());
  VMInstruction *jmp2 = new VMInstruction(JMP,unsigned_integer,
					  RTUnsigned(0));
  o << jmp2;
  endInstructions << jmp2;
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    prev_fixup->literal = RTUnsigned(o.size());
    RegisterReference ttest(JITExpression(o,t.child(n).first()));
    VMInstruction *jmpn = new VMInstruction(JIF,unsigned_integer,
					    ttest.index(),RTUnsigned(0));
    o << jmpn;
    prev_fixup = jmpn;
    JITBlock(o,t.child(n).second());
    VMInstruction *jmpp = new VMInstruction(JMP,unsigned_integer,
					    RTUnsigned(0));
    o << jmpp;
    endInstructions << jmpp;
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    prev_fixup->literal = RTUnsigned(o.size());
    JITBlock(o,t.last().first());
  }
  int end_address = o.size();
  for (int i=0;i<endInstructions.size();i++)
    endInstructions[i]->literal = RTUnsigned(end_address);
  o << new VMInstruction(NOP);
}

void JITStatementType(VMStream& o, tree t) {
  switch(t.token()) {
  case '=': 
    ///JITAssignment(o,t);
    break;
  case TOK_MULTI:
    //    multiFunctionCall(t,printIt);
    break;
  case TOK_SPECIAL:
    //    specialFunctionCall(t,printIt);
    break;
  case TOK_IF:
    JITIfStatement(o,t);
    break;
  case TOK_BREAK:
    //    if (context->inLoop()) 
    //      throw InterpreterBreakException();
    break;
  case TOK_CONTINUE:
    //    if (context->inLoop()) 
    //      throw InterpreterContinueException();
    break;
  case TOK_DBSTEP:
    //    qDebug() << "**********************DBStep";
    //    dbstepStatement(t);
    //    emit RefreshBPLists();
    //    throw InterpreterReturnException();
    break;
  case TOK_DBTRACE:
    //    qDebug() << "**********************DBTrace";
    //    dbtraceStatement(t);
    //    emit RefreshBPLists();
    //    throw InterpreterReturnException();
    break;
  case TOK_RETURN:
    //    throw InterpreterReturnException();
    break;
  case TOK_SWITCH:
    //    switchStatement(t);
    break;
  case TOK_TRY:
    //    tryStatement(t);
    break;
  case TOK_QUIT:
    //    throw InterpreterQuitException();
    break;
  case TOK_RETALL:
    //    throw InterpreterRetallException();
    break;
  case TOK_KEYBOARD:
    //    doDebugCycle();
    break;
  case TOK_GLOBAL:
    //    globalStatement(t);
    break;
  case TOK_PERSISTENT:
    //    persistentStatement(t);
    break;
  case TOK_EXPR:
    JITExpression(o,t.first());
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void JITStatement(VMStream& o, tree t) {
  // ignore tok_qstatement/tok_statement
  JITStatementType(o,t.first());
}

void JITBlock(VMStream& o, tree t) {
  const treeVector &statements(t.children());
  for (treeVector::const_iterator i=statements.begin();
       i!=statements.end();i++) 
    JITStatement(o,*i);
}

void JITLoop(tree t) {
  VMStream loopjit;
  
  int loop_start(atoi(t.first().second().first().text().c_str()));
  int loop_stop(atoi(t.first().second().second().text().c_str()));
  string loop_index(t.first().first().text());
  std::cout << "Loop: " << loop_start << ":" << loop_stop << "\r\n";
  t.print();
  unsigned loop_index_register = GetReg();
  symbols.insertSymbol(loop_index,
		       SymbolInformation(integer,loop_index_register));
  loopjit << new VMInstruction(SET,integer,loop_index_register,
			       RTInteger(loop_start));
  unsigned loop_max_register = GetReg();
  loopjit << new VMInstruction(SET,integer,loop_max_register,
			       RTInteger(loop_stop));
  unsigned loop_increment_register = GetReg();
  loopjit << new VMInstruction(SET,integer,loop_increment_register,
			       RTInteger(1));
  unsigned loop_test_register = GetReg();
  loopjit << new VMInstruction(SET,boolean,loop_test_register,RTBoolean(false));
  unsigned loop_start_instruction = loopjit.size();
  JITBlock(loopjit,t.second());
  loopjit << new VMInstruction(ADD,integer,loop_index_register,
			       loop_index_register,loop_increment_register);
  loopjit << new VMInstruction(LT,integer,loop_test_register,
			       loop_index_register,loop_max_register);
  loopjit << new VMInstruction(JIT,unsigned_integer,
			       loop_test_register,
			       RTUnsigned(loop_start_instruction+1));
  std::cout << loopjit;
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

bool AnyNonJITStatements(tree t) {
  if (t.is(TOK_DBSTEP) || t.is(TOK_DBTRACE) ||
      t.is(TOK_RETURN) || t.is(TOK_TRY) || t.is(TOK_RETALL) ||
      t.is(TOK_KEYBOARD) || t.is(TOK_GLOBAL) || t.is(TOK_PERSISTENT) ||
      t.is(TOK_NEST_FUNC) || t.is(TOK_END) || t.is(TOK_MATDEF) || 
      t.is(TOK_CELLDEF)) return true;
  for (int i=0;i<t.numchildren();i++)
    if (AnyNonJITStatements(t.child(i))) return true;
  return false;
}

bool HasInnerForStatementsRecurse(tree t) {
  if (t.is(TOK_FOR)) return true;
  for (int i=0;i<t.numchildren();i++)
    if (HasInnerForStatementsRecurse(t.child(i)))
      return true;
  return false;
}

bool HasInnerForStatements(tree t) {
  for (int i=0;i<t.numchildren();i++)
    if (HasInnerForStatementsRecurse(t.child(i))) return true;
  return false;
}

void EvaluateForStatement(tree t) {
  if (!(t.first().is('=') && t.first().second().is(':') &&
	t.first().second().first().is(TOK_INTEGER) &&
	t.first().second().second().is(TOK_INTEGER))) return;
  if (HasInnerForStatements(t)) return;
  if (AnyNonJITStatements(t)) return;
  LoopSignature sig;
  if (!ScalarRequirements(t,sig)) return;
  sig.remove_scalar(t.first().first().text());
  //  InstanatiateScalars(sig.get_scalars());
  //  InstantiateMatrices(sig.get_matrices());
  JITLoop(t);
}

void FindInnerForStatements(tree t) {
  if (t.is(TOK_FOR))
    EvaluateForStatement(t);
  for (int i=0;i<t.numchildren();i++)
    FindInnerForStatements(t.child(i));
}

ArrayVector tqtFunction(int nargout, const ArrayVector& args, Interpreter* m_eval) {
  if (args.size() == 0) return ArrayVector();
  string name(ArrayToString(args[0]));
  FuncPtr val;
  if (!m_eval->lookupFunction(name,val))
    throw Exception("Unable to resolve " + name + " to a function");
  val->updateCode(m_eval);
  if (!(val->type() == FM_M_FUNCTION))
    throw Exception("Function " + name + " is not an M file (and cannot be compiled");
  MFunctionDef *mptr = (MFunctionDef *) val;
  tree code(mptr->code);
  FindInnerForStatements(code);
  return ArrayVector();
}

void LoadCompileFunction(Context* context) {
  context->addSpecialFunction("fcc",fccFunction,-1,0); 
  context->addSpecialFunction("tqt",tqtFunction,-1,0);
}
