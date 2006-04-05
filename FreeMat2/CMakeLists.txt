project(FreeMat)

include(${CMAKE_ROOT}/Modules/FindQt4.cmake)
set(CMAKE_MODULE_PATH ./cmake)

find_package(AVCALL)
find_package(FFTW)
find_package(FFTWF)

#find_package(LAPACK)
#find_package(BLAS)


#find_package(amd)
#find_package(umfpack)
#find_package(arpack)
#find_package(lapack)
#find_package(blas)
#find_package(matio)

include(${QT_USE_FILE})

include_directories(libs/libFreeMat libs/libCore libs/libFN
libs/libGraphics libs/libXP src libs/libMex) 

set(FreeMat_MOCS src/PathTool.hpp src/ToolDock.hpp src/helpgen.hpp
src/DumbTerminal.hpp src/SocketCB.hpp src/highlighter.hpp
src/FuncMode.hpp src/HistoryWidget.hpp src/VariablesTool.hpp
src/Terminal.hpp src/FileTool.hpp src/MainApp.hpp src/Editor.hpp
libs/libCore/helpwidget.hpp libs/libXP/QTTerm.hpp libs/libXP/GUITerminal.hpp
libs/libXP/KeyManager.hpp tools/disttool/disttool.hpp) 
set(FreeMat_RCCS FreeMat.qrc)
set(FreeMat_SRCS libs/libMex/mxArray.cpp libs/libMex/MexInterface.cpp
libs/libFreeMat/NewLex.cpp libs/libFreeMat/Array.cpp
libs/libFreeMat/AST.cpp libs/libFreeMat/Math.cpp
libs/libFreeMat/Context.cpp libs/libFreeMat/Data.cpp
libs/libFreeMat/Dimensions.cpp libs/libFreeMat/EigenDecompose.cpp
libs/libFreeMat/Exception.cpp libs/libFreeMat/FunctionDef.cpp
libs/libFreeMat/IEEEFP.cpp libs/libFreeMat/LAPACK.cpp
libs/libFreeMat/LeastSquaresSolver.cpp
libs/libFreeMat/LinearEqSolver.cpp libs/libFreeMat/LUDecompose.cpp
libs/libFreeMat/Malloc.cpp libs/libFreeMat/MatrixMultiply.cpp
libs/libFreeMat/Module.cpp libs/libFreeMat/QRDecompose.cpp
libs/libFreeMat/Reserved.cpp libs/libFreeMat/Scope.cpp
libs/libFreeMat/Serialize.cpp
libs/libFreeMat/SingularValueDecompose.cpp
libs/libFreeMat/WalkTree.cpp libs/libFreeMat/File.cpp
libs/libFreeMat/Interface.cpp libs/libFreeMat/Sparse.cpp
libs/libFreeMat/Class.cpp libs/libFreeMat/NumericArray.cpp
libs/libFreeMat/Parser.cxx libs/libCore/Cast.cpp
libs/libCore/Constructors.cpp libs/libCore/FFT.cpp
libs/libCore/StringOps.cpp libs/libCore/Transcendental.cpp
libs/libCore/Trigonometric.cpp libs/libCore/Misc.cpp
libs/libCore/Constants.cpp libs/libCore/Directory.cpp
libs/libCore/Analyze.cpp libs/libCore/Utils.cpp libs/libCore/IO.cpp
libs/libCore/Inspect.cpp libs/libCore/Random.cpp
libs/libCore/LoadCore.cpp libs/libCore/MPIWrap.cpp
libs/libCore/RanLib.cpp libs/libCore/helpwidget.cpp
libs/libFN/OptFun.cpp libs/libFN/LoadFN.cpp libs/libFN/Interp1D.cpp
libs/libFN/FNFun.cpp)

# generate rules for building source files from the resources
qt4_add_resources(FreeMat_RCC_SRCS ${FreeMat_RCCS})
# generate rules for building source files that moc generates
qt4_wrap_cpp(FreeMat_MOC_SRCS ${FreeMat_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_executable(  FreeMat ${FreeMat_SRCS} ${FreeMat_MOC_SRCS} ${FreeMat_RCC_SRCS})

target_link_libraries ( FreeMat ${QT_LIBRARIES}  )
