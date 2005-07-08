TEMPLATE = lib

TARGET = FreeMatLib

CONFIG += staticlib
 warn_off

INCLUDEPATH = ../libffi/include ../libXP ../libUMFPACK/UMFPACK/Include ../..


DEFINES -= UNICODE

DEPENDPATH += INCLUDEPATH

HEADERS += Array.hpp \
AST.hpp \
Math.hpp \
Command.hpp \
Context.hpp \
Data.hpp \
Dimensions.hpp \
EigenDecompose.hpp \
Exception.hpp \
FunctionDef.hpp \
HandleList.hpp \
IEEEFP.hpp \
LAPACK.hpp \
LeastSquaresSolver.hpp \
LexerInterface.hpp \
LinearEqSolver.hpp \
LUDecompose.hpp \
Malloc.hpp \
MatrixMultiply.hpp \
Module.hpp \
ParserInterface.hpp \
QRDecompose.hpp \
Reserved.hpp \
Scope.hpp \
Serialize.hpp \
SingularValueDecompose.hpp \
Types.hpp \
WalkTree.hpp \
File.hpp \
Stream.hpp \
Interface.hpp \
SymbolTable.hpp \
Sparse.hpp \
Class.hpp \
NumericArray.hpp \
Parser.h

SOURCES += NewLex.cpp \
Array.cpp \
AST.cpp \
Math.cpp \
Command.cpp \
Context.cpp \
Data.cpp \
Dimensions.cpp \
EigenDecompose.cpp \
Exception.cpp \
FunctionDef.cpp \
IEEEFP.cpp \
LAPACK.cpp \
LeastSquaresSolver.cpp \
LinearEqSolver.cpp \
LUDecompose.cpp \
Malloc.cpp \
MatrixMultiply.cpp \
Module.cpp \
QRDecompose.cpp \
Reserved.cpp \
Scope.cpp \
Serialize.cpp \
SingularValueDecompose.cpp \
WalkTree.cpp \
File.cpp \
Interface.cpp \
Sparse.cpp \
Class.cpp \
NumericArray.cpp \
Parser.cxx
