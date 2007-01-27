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
  m_def->updateCode();
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

void LoadCompileFunction(Context* context) {
  context->addSpecialFunction("fcc",fccFunction,-1,0); 
}
