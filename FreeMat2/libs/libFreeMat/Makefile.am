EXTRA_DIST = libFreeMat.vcproj  

noinst_LIBRARIES = libFreeMat.a

CFLAGS=@CFLAGS@ 
CXXFLAGS=@CXXFLAGS@ -DYY_NEVER_INTERACTIVE
INCLUDES=-I@top_srcdir@/libs/libGraphics -I@top_builddir@/libs/libffi/include -I@top_srcdir@/libs/libXP -I@top_srcdir@/src/x11 -I@top_srcdir@/libs/libFLTK -I@top_srcdir@/libs/libUMFPACK/UMFPACK/Include

libFreeMat_a_SOURCES = Parser.yxx \
Parser.h \
NewLex.cpp \
Array.cpp \
Array.hpp \
AST.cpp \
AST.hpp \
Math.cpp \
Math.hpp \
Command.cpp \
Command.hpp \
Context.cpp \
Context.hpp \
Data.cpp \
Data.hpp \
Dimensions.cpp \
Dimensions.hpp \
EigenDecompose.cpp \
EigenDecompose.hpp \
Exception.cpp \
Exception.hpp \
FunctionDef.cpp \
FunctionDef.hpp \
HandleList.hpp \
IEEEFP.cpp \
IEEEFP.hpp \
LAPACK.cpp \
LAPACK.hpp \
LeastSquaresSolver.cpp \
LeastSquaresSolver.hpp \
LexerInterface.hpp \
LinearEqSolver.cpp \
LinearEqSolver.hpp \
LUDecompose.cpp \
LUDecompose.hpp \
Malloc.cpp \
Malloc.hpp \
MatrixMultiply.cpp \
MatrixMultiply.hpp \
Module.cpp \
Module.hpp \
ParserInterface.hpp \
QRDecompose.hpp \
QRDecompose.cpp \
Reserved.cpp \
Reserved.hpp \
Scope.cpp \
Scope.hpp \
Serialize.cpp \
Serialize.hpp \
SingularValueDecompose.cpp \
SingularValueDecompose.hpp \
Types.hpp \
WalkTree.cpp \
WalkTree.hpp \
File.cpp \
File.hpp \
Stream.hpp \
Interface.hpp \
Interface.cpp \
SymbolTable.hpp \
Sparse.cpp \
Sparse.hpp \
Class.cpp \
Class.hpp \
NumericArray.cpp \
NumericArray.hpp

AUTOMAKE_OPTIONS 	= foreign
