
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${LOCAL_PATH} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/Interpreter.hpp ${CMAKE_CURRENT_SOURCE_DIR}/Intrepreter.moc.cpp)
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/DebugStream.hpp ${CMAKE_CURRENT_SOURCE_DIR}/DebugStream.moc.cpp )
 
ADD_LIBRARY( FreeMatlib 
Array.cpp 
Algorithms.cpp 
Interpreter.cpp 
Context.cpp 
EigenDecompose.cpp 
Exception.cpp 
File.cpp 
IEEEFP.cpp 
IndexArray.cpp 
LAPACK.cpp 
LeastSquaresSolver.cpp 
LinearEqSolver.cpp 
LUDecompose.cpp 
Math.cpp 
MatrixMultiply.cpp 
Module.cpp 
Scope.cpp 
Serialize.cpp 
SparseMatrix.cpp 
Class.cpp 
Print.cpp 
Token.cpp 
Parser.cpp 
Scanner.cpp 
Tree.cpp 
FunctionDef.cpp 
FuncPtr.cpp 
AnonFunc.cpp 
SparseCCS.cpp 
Stats.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Intrepreter.moc.cpp
DebugStream.cpp
${CMAKE_CURRENT_SOURCE_DIR}/DebugStream.moc.cpp
)

