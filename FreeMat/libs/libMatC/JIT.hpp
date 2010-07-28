/*
 * Copyright (c) 2009 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __JIT_hpp__
#define __JIT_hpp__

#ifdef HAVE_LLVM

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include <stdint.h>

#ifndef INT64_C
#define INT64_C(x) __INT64_C(x)
#endif
#ifndef UINT64_C
#define UINT64_C(x) __UINT64_C(x)
#endif

#include "Types.hpp"

#include "llvm/Module.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Instruction.h"
#include "llvm/Module.h"
#include "llvm/CallingConv.h"
#include "llvm/Value.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"

#include <QString>

typedef llvm::Value* JITScalar;
typedef const llvm::Type* JITType;
typedef const llvm::FunctionType* JITFunctionType;
typedef llvm::Function* JITFunction;
typedef llvm::BasicBlock* JITBlock;
typedef llvm::ExecutionEngine* JITEngine;
typedef llvm::Module* JITModule;
typedef llvm::GenericValue JITGeneric;
typedef llvm::FunctionPassManager* JITOptimizer;

// A wrapper interface - this is a way to abstract out the details of the
// LLVM interface.  This version is based on double precision values only.
// For its more sophisticated predecessor, see SVN 3417 for the various
// types. Unfortunately, a pure double precision is not sufficient.  This
// is because of boolean results -- boolean results are coded as double
// precision values with out of band information that they are logical
// values.  Thus, at a minimum, the JIT has to support two types: double
// and bool.
class JIT {
private:
  JITBlock ip;
  JITFunction func;
  JITEngine ee;
  JITModule m;
  JITOptimizer opt;
  bool initialized;
  JITScalar JITBinOp(llvm::Instruction::BinaryOps op, JITScalar A, JITScalar B);
  JIT();
  ~JIT();
public:
  static JIT* Instance();
  // Optimize the function -- cannot be modified once this is called
  void            OptimizeCode();
  JITFunctionType FunctionType(JITType rettype, std::vector<JITType> args);
  JITFunctionType FunctionType(QString rettype, QString args);
  // Get a double-precision metatype
  static JITType DoubleType();
  // Get a single-precision metatype
  static JITType FloatType();
  // Get a boolean metatype
  static JITType BoolType();
  // For convenience, a type encoding scheme is available for
  // function defs.
  // f = float, d = double, b = bool, v = void
  // F = pointer-to-float, D = pointer-to-double, B = pointer-to-bool, V = pointer-to-void
  static JITType MapTypeCode(QChar c);
  // Get a pointer metatype (to another metatype)
  static JITType PointerType(JITType t);
  // Get the void metatype
  static JITType VoidType();
  // Get the metatype of a scalar
  static JITType TypeOf(JITScalar x);
  // Get a double precision value
  static JITScalar DoubleValue(double x);
  // Get a float precision value
  static JITScalar FloatValue(float x);
  // Get a boolean value
  static JITScalar BoolValue(bool x);
  // Convert the given value to a double precision type
  JITScalar ToDouble(JITScalar x);
  // Convert the given value to a float precision type
  JITScalar ToFloat(JITScalar x);  
  // Convert the given value to a boolean type
  JITScalar ToBool(JITScalar x);
  // Convert the given value to the destination type
  JITScalar ToType(JITScalar x, JITType t);
  // Test for boolean
  bool IsBool(JITScalar x);
  // Test for double
  bool IsDouble(JITScalar x);
  // Test for float
  bool IsFloat(JITScalar x);
  // Test for boolean
  bool IsBool(JITType x);
  // Test for double
  bool IsDouble(JITType x);
  // Test for float
  bool IsFloat(JITType x);
  // Allocate space for a scalar on the stack -- returns address
  JITScalar Alloc(JITType T, QString name);
  // Create a new (named) block
  JITBlock NewBlock(QString name);
  JITScalar And(JITScalar A, JITScalar B);
  JITScalar Or(JITScalar A, JITScalar B);
  JITScalar Xor(JITScalar A, JITScalar B);
  JITScalar Mul(JITScalar A, JITScalar B);
  JITScalar Div(JITScalar A, JITScalar B);
  JITScalar Sub(JITScalar A, JITScalar B);
  JITScalar Add(JITScalar A, JITScalar B);
  JITScalar LessThan(JITScalar A, JITScalar B);
  JITScalar LessEquals(JITScalar A, JITScalar B);
  JITScalar Equals(JITScalar A, JITScalar B);
  JITScalar GreaterEquals(JITScalar A, JITScalar B);
  JITScalar GreaterThan(JITScalar A, JITScalar B);
  JITScalar NotEqual(JITScalar A, JITScalar B);
  JITScalar Not(JITScalar A);
  JITScalar Negate(JITScalar A);
  void Store(JITScalar Value, JITScalar Address);
  JITScalar Load(JITScalar Address);
  JITScalar String(QString text);
  void Jump(JITBlock B);
  void Branch(JITBlock IfTrue, JITBlock IfFalse, JITScalar TestValue);
  void SetCurrentBlock(JITBlock B);
  JITBlock CurrentBlock();
  JITScalar Call(JITFunction, std::vector<JITScalar> args);
  JITScalar Call(JITScalar, std::vector<JITScalar> args);
  JITScalar Call(JITFunction, JITScalar arg1);
  JITScalar Call(JITFunction, JITScalar arg1, JITScalar arg2);
  JITScalar Call(JITFunction, JITScalar arg1, JITScalar arg2, JITScalar arg3);
  JITScalar Call(JITFunction, JITScalar arg1, JITScalar arg2, JITScalar arg3, JITScalar arg4);
  JITScalar Call(JITFunction, JITScalar arg1, JITScalar arg2, JITScalar arg3, JITScalar arg4, JITScalar arg5);
  JITScalar GetElement(JITScalar BaseAddress, JITScalar Offset);
  JITFunction DefineFunction(JITFunctionType functype, QString name);
  void SetCurrentFunction(JITFunction A);
  JITFunction CurrentFunction();
  // Shortcut to define a non-JIT function
  // Type codes are "v - void, b - bool, f - float, d - double,
  //                           B - bool pointer, F - float pointer, D - double pointer
  JITFunction DefineLinkFunction(QString name, QString rettype, QString args);
  JITScalar FunctionArgument(int n, QString name);
  void CloseFunction();
  void Return(JITScalar t);
  void Return();
  void Dump();
  void Dump(JITFunction f);
  void Dump( const QString& fname );
  void Dump( const QString& fname, JITFunction f );
  JITGeneric Invoke(JITFunction f, JITGeneric arg);
  JITGeneric Invoke(JITFunction f, std::vector<JITGeneric> arg);
  void Invoke(JITFunction f, void** arg);
};

#else

class JIT {
};

#endif

#endif
