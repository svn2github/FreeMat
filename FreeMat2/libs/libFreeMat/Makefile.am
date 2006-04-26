noinst_LIBRARIES = libFreeMat.a

INCLUDES = -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libMex 

libFreeMat_a_HFILES = Array.hpp \
AST.hpp \
Class.hpp \
Context.hpp \
Data.hpp \
Dimensions.hpp \
EigenDecompose.hpp \
Exception.hpp \
File.hpp \
FunctionDef.hpp \
HandleList.hpp \
IEEEFP.hpp \
Interface.hpp \
LAPACK.hpp \
LeastSquaresSolver.hpp \
LexerInterface.hpp \
LinearEqSolver.hpp \
LUDecompose.hpp \
Malloc.hpp \
Math.hpp \
MatrixMultiply.hpp \
MemPtr.hpp \
Module.hpp \
NumericArray.hpp \
ParserInterface.hpp \
Parser.h \
QRDecompose.hpp \
Reserved.hpp \
Scope.hpp \
Serialize.hpp \
SingularValueDecompose.hpp \
Sparse.hpp \
Stream.hpp \
SymbolTable.hpp \
Types.hpp \
WalkTree.hpp

libFreeMat_a_SOURCES = Array.cpp \
AST.cpp \
Class.cpp \
Context.cpp \
Data.cpp \
Dimensions.cpp \
EigenDecompose.cpp \
Exception.cpp \
File.cpp \
FunctionDef.cpp \
IEEEFP.cpp \
Interface.cpp \
LAPACK.cpp \
LeastSquaresSolver.cpp \
LinearEqSolver.cpp \
LUDecompose.cpp \
Malloc.cpp \
Math.cpp \
MatrixMultiply.cpp \
Module.cpp \
NewLex.cpp \
NumericArray.cpp \
Parser.cxx \
QRDecompose.cpp \
Reserved.cpp \
Scope.cpp \
Serialize.cpp \
SingularValueDecompose.cpp \
Sparse.cpp \
WalkTree.cpp \
$(libFreeMat_a_HFILES)

AUTOMAKE_OPTIONS 	= foreign

AM_CPPFLAGS = $(QT_CXXFLAGS) 
