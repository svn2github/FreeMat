noinst_LIBRARIES = libFreeMat.a

CFLAGS=@CFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cflags`
CXXFLAGS=@CXXFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cxxflags` -I@top_srcdir@/libs/libGraphics -I@top_srcdir@/extlibs/libtecla -I@top_srcdir@/extlibs/libffi-2.00-beta/include

libFreeMat_a_SOURCES = Parser.yy \
Lexer.ll \
Array.cpp \
Array.hpp \
AST.cpp \
AST.h \
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
IEEEFP.cpp \
IEEEFP.hpp \
LAPACK.cpp \
LAPACK.hpp \
LeastSquaresSolver.cpp \
LeastSquaresSolver.hpp \
LexerInterface.hpp \
LinearEqSolver.cpp \
LinearEqSolver.hpp \
Malloc.cpp \
Malloc.hpp \
MatrixMultiply.cpp \
MatrixMultiply.hpp \
Module.cpp \
Module.hpp \
ParserInterface.hpp \
Reserved.cpp \
Reserved.h \
Scope.cpp \
Scope.hpp \
Serialize.cpp \
Serialize.hpp \
ServerSocket.cpp \
ServerSocket.hpp \
SingularValueDecompose.cpp \
SingularValueDecompose.hpp \
Socket.cpp \
Socket.hpp \
Types.hpp \
VectorOps_assist.f \
VectorOps.cpp \
VectorOps.hpp \
WalkTree.cpp \
WalkTree.hpp \
File.cpp \
File.hpp \
Stream.hpp \
Interface.hpp \
SymbolTable.hpp 

EXTRA_DIST		= bootstrap
AUTOMAKE_OPTIONS 	= foreign
