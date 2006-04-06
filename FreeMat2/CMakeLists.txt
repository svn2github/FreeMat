project(FreeMat CXX)
enable_language( C )
enable_language( Fortran )

include(${CMAKE_ROOT}/Modules/FindQt4.cmake)
set(CMAKE_MODULE_PATH ./cmake)

find_package(AVCALL)
find_package(FFTW)
find_package(FFTWF)
find_package(LAPACK)
find_package(BLAS)
find_package(AMD)
find_package(UMFPACK)
find_package(ARPACK)
find_package(MATIO)
find_package(FORTRANLIB)
message(STATUS "Looking for curses library")
find_package(Curses)

include(${QT_USE_FILE})

include_directories(libs/libFreeMat libs/libCore libs/libFN
libs/libGraphics libs/libXP src libs/libMex ${AMD_INCLUDE_DIR}
${AVCALL_INCLUDE_DIR} ${FFTW_INCLUDE_DIR} ${FFTWF_INCLUDE_DIR}
${MATIO_INCLUDE_DIR} ${UMFPACK_INCLUDE_DIR} ${QT_INCLUDES}
${CURSES_INCLUDE_PATH}) 

add_subdirectory(libs)

set(FreeMat_MOCS src/application.h src/PathTool.hpp src/ToolDock.hpp src/helpgen.hpp
src/DumbTerminal.hpp src/SocketCB.hpp src/highlighter.hpp
src/FuncMode.hpp src/HistoryWidget.hpp src/VariablesTool.hpp
src/Terminal.hpp src/FileTool.hpp src/MainApp.hpp src/Editor.hpp
libs/libXP/QTTerm.hpp libs/libXP/KeyManager.hpp ) 
set(FreeMat_RCCS FreeMat.qrc)
set(FreeMat_SRCS libs/libMex/mxArray.cpp libs/libMex/MexInterface.cpp
libs/libFN/OptFun.cpp libs/libFN/LoadFN.cpp libs/libFN/Interp1D.cpp
libs/libFN/FNFun.cpp libs/libGraphics/HandleAxis.cpp
libs/libGraphics/HandleObject.cpp libs/libGraphics/HandleProperty.cpp
libs/libGraphics/HandleFigure.cpp libs/libGraphics/HandleText.cpp
libs/libGraphics/GLLabel.cpp libs/libGraphics/GLRenderEngine.cpp
libs/libGraphics/HandleLineSeries.cpp
libs/libGraphics/RenderEngine.cpp libs/libGraphics/HandleCommands.cpp
libs/libGraphics/HandleImage.cpp libs/libGraphics/HandleSurface.cpp
libs/libGraphics/QTRenderEngine.cpp libs/libGraphics/HandleWindow.cpp
libs/libXP/DynLib.cpp libs/libXP/System.cpp libs/libXP/PathSearch.cpp
libs/libXP/KeyManager.cpp libs/libXP/QTTerm.cpp src/MainApp.cpp
src/SocketCB.cpp src/application.cpp src/main.cpp src/highlighter.cpp
src/helpgen.cpp src/PathTool.cpp src/Editor.cpp src/ToolDock.cpp
src/HistoryWidget.cpp src/FileTool.cpp src/Common.cpp src/FuncMode.cpp
src/VariablesTool.cpp src/Terminal.cpp src/DumbTerminal.cpp
libs/libFN/lmdif1.f libs/libFN/lmdif.f 
libs/libFN/lmpar.f libs/libFN/qrfac.f libs/libFN/dpmpar.f
libs/libFN/enorm.f libs/libFN/qrsolv.f libs/libFN/fdjac2.f
libs/libFN/erf.f libs/libFN/derf.f libs/libFN/ei.f libs/libFN/dei.f
libs/libFN/daw.f libs/libFN/ddaw.f libs/libFN/psi.f libs/libFN/dpsi.f
libs/libFN/gamma.f libs/libFN/dgamma.f libs/libFN/algama.f
libs/libFN/dlgama.f  
)


# generate rules for building source files from the resources
qt4_add_resources(FreeMat_RCC_SRCS ${FreeMat_RCCS})
# generate rules for building source files that moc generates
qt4_wrap_cpp(FreeMat_MOC_SRCS ${FreeMat_MOCS})

# build sources, moc'd sources, and rcc'd sources
add_executable(  FreeMat ${FreeMat_SRCS} ${FreeMat_MOC_SRCS} ${FreeMat_RCC_SRCS})

target_link_libraries ( FreeMat Core FM ${QT_LIBRARIES} ${QT_QTOPENGL_LIBRARY}
${AVCALL_LIBRARY} ${FFTW_LIBRARY} ${ARPACK_LIBRARY}
${FFTWF_LIBRARY}  ${MATIO_LIBRARY} ${UMFPACK_LIBRARY} ${AMD_LIBRARY}
${LAPACK_LIBRARY} ${BLAS_LIBRARY} ${FORT_LIBRARY} ${CURSES_LIBRARY}) 

SET_TARGET_PROPERTIES( FreeMat PROPERTIES LINKER_LANGUAGE CXX )

