set(FM_SRCS NewLex.cpp Array.cpp
AST.cpp Math.cpp
Context.cpp Data.cpp
Dimensions.cpp EigenDecompose.cpp
Exception.cpp FunctionDef.cpp
IEEEFP.cpp LAPACK.cpp
LeastSquaresSolver.cpp
LinearEqSolver.cpp LUDecompose.cpp
Malloc.cpp MatrixMultiply.cpp
Module.cpp QRDecompose.cpp
Reserved.cpp Scope.cpp
Serialize.cpp
SingularValueDecompose.cpp
WalkTree.cpp File.cpp
Interface.cpp Sparse.cpp
Class.cpp NumericArray.cpp
Parser.cxx
)

add_library(FM STATIC ${FM_SRCS})
target_link_libraries ( FM Mex ${QT_LIBRARIES} 
${AVCALL_LIBRARY} ${FFTW_LIBRARY} ${ARPACK_LIBRARY}
${FFTWF_LIBRARY}  ${UMFPACK_LIBRARY} ${AMD_LIBRARY}
${LAPACK_LIBRARY} ${BLAS_LIBRARY}) 

