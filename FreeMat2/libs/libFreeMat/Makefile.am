noinst_LIBRARIES = libFreeMat.a

INCLUDES = -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libMex 

libFreeMat_a_MOC = Interpreter.moc.cpp

BUILT_SOURCES = $(libFreeMat_a_MOC)

libFreeMat_a_HFILES = Array.hpp \
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
LAPACK.hpp \
LeastSquaresSolver.hpp \
LinearEqSolver.hpp \
LUDecompose.hpp \
Malloc.hpp \
Math.hpp \
MatrixMultiply.hpp \
MemPtr.hpp \
Module.hpp \
NumericArray.hpp \
QRDecompose.hpp \
Scope.hpp \
Serialize.hpp \
SingularValueDecompose.hpp \
Sparse.hpp \
Stream.hpp \
SymbolTable.hpp \
Types.hpp \
Interpreter.hpp \
Print.hpp \
Token.hpp \
Parser.hpp \
Scanner.hpp \
Tree.hpp \
RefVec.hpp \
Transform.hpp

libFreeMat_a_SOURCES = Array.cpp \
Interpreter.cpp \
Context.cpp \
Data.cpp \
Dimensions.cpp \
EigenDecompose.cpp \
Exception.cpp \
File.cpp \
IEEEFP.cpp \
LAPACK.cpp \
LeastSquaresSolver.cpp \
LinearEqSolver.cpp \
LUDecompose.cpp \
Malloc.cpp \
Math.cpp \
MatrixMultiply.cpp \
Module.cpp \
NumericArray.cpp \
QRDecompose.cpp \
Scope.cpp \
Serialize.cpp \
SingularValueDecompose.cpp \
Sparse.cpp \
Class.cpp \
Print.cpp \
Token.cpp \
Parser.cpp \
Scanner.cpp \
Tree.cpp \
FunctionDef.cpp \
Transform.cpp \
$(libFreeMat_a_HFILES) 

nodist_libFreeMat_a_SOURCES = $(libFreeMat_a_MOC)

CLEANFILES = $(libFreeMat_a_MOC)

AUTOMAKE_OPTIONS 	= foreign

AM_CPPFLAGS = $(QT_CFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<
