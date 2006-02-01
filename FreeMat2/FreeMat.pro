TEMPLATE = app

QT += opengl

CONFIG += warn_off

DEFINES -= UNICODE

TARGET = FreeMat

PRE_TARGETDEPS += extern/fftw-3.0.1/.libs/libfftw3.a extern/fftw-3.0.1/.libs/libfftw3f.a extern/ffcall-1.10/avcall/.libs/libavcall.a extern/AMD/Lib/libamd.a extern/UMFPACK/Lib/libumfpack.a  extern/ARPACK/libarpack.a

#fftw_float avcall amd umfpack arpack lapack atlas

fftw_double.target = extern/fftw-3.0.1/.libs/libfftw3.a
fftw_double.commands = cd extern && tar xfz fftw-3.0.1.tar.gz && cd fftw-3.0.1 && configure && make

fftw_single.target = extern/fftw-3.0.1/.libs/libfftw3f.a
fftw_single.commands = cd extern/fftw-3.0.1 && configure --enable-single && make

avcall.target = extern/ffcall-1.10/avcall/.libs/libavcall.a
avcall.commands = cd extern && tar xfz ffcall-1.10_freemat_patch.tar.gz && cd ffcall-1.10 && configure && make

amd.target = extern/AMD/Lib/libamd.a
amd.commands = cd extern && tar xfz UFconfig-1.0_freemat_patch.tar.gz &&  tar xfz AMD-1.2.tar.gz &&  cd AMD && make

umfpack.target = extern/UMFPACK/Lib/libumfpack.a
umfpack.commands = cd extern && tar xfz UMFPACK-4.6.tar.gz && cd UMFPACK && make

arpack.target = extern/ARPACK/libarpack.a
arpack.commands = cd extern && tar xfz arpack96_freemat_patch.tar.gz && cd ARPACK && make

lapack.target = extern/LAPACK/liblapack.a
lapack.commands = cd extern && tar xfz lapak-3.0_freemat_patch.tgz && cd LAPACK/SRC && make -f Makefile_freemat

QMAKE_EXTRA_TARGETS += fftw_double fftw_single avcall amd umfpack arpack lapack atlas

#POST_TARGETDEPS += HELPGEN DISTTOOL

INCLUDEPATH += libs/libFreeMat libs/libCore libs/libFN libs/libGraphics libs/libXP extern/ffcall-1.10/avcall extern/UMFPACK/Include extern/AMD/Include extern/fftw-3.0.1/api

LIBS += extern/fftw-3.0.1/.libs/libfftw3f.a extern/fftw-3.0.1/.libs/libfftw3.a extern/ffcall-1.10/avcall/.libs/libavcall.a extern/UMFPACK/Lib/libumfpack.a extern/AMD/Lib/libamd.a extern/ARPACK/libarpack.a

macx {
LIBS += -framework vecLib -L/sw/lib -lg2c
}

!macx:unix {
#LIBS +=  extern/LAPACK/liblapack.a -lblas -lg2c -lcurses
#LIBS +=  libmkl_p4.so extern/LAPACK/liblapack.a libmkl_p4.so libguide.so extern/LAPACK/liblapack.a -lg2c -lcurses
LIBS +=  extern/LAPACK/liblapack.a libblas.so -lg2c -lcurses -static-libgcc /usr/lib/gcc/i386-redhat-linux/3.4.3/libstdc++.a
}

win32 {
LIBS +=  extern/blas/atlas_prebuilt_win32/libf77blas.a extern/blas/atlas_prebuilt_win32/libatlas.a -lws2_32 -lg2c
}

DEPENDPATH += INCLUDEPATH

HEADERS += libs/libFreeMat/Array.hpp \
libs/libFreeMat/AST.hpp \
libs/libFreeMat/Math.hpp \
libs/libFreeMat/Command.hpp \
libs/libFreeMat/Context.hpp \
libs/libFreeMat/Data.hpp \
libs/libFreeMat/Dimensions.hpp \
libs/libFreeMat/EigenDecompose.hpp \
libs/libFreeMat/Exception.hpp \
libs/libFreeMat/FunctionDef.hpp \
libs/libFreeMat/HandleList.hpp \
libs/libFreeMat/IEEEFP.hpp \
libs/libFreeMat/LAPACK.hpp \
libs/libFreeMat/LeastSquaresSolver.hpp \
libs/libFreeMat/LexerInterface.hpp \
libs/libFreeMat/LinearEqSolver.hpp \
libs/libFreeMat/LUDecompose.hpp \
libs/libFreeMat/Malloc.hpp \
libs/libFreeMat/MatrixMultiply.hpp \
libs/libFreeMat/Module.hpp \
libs/libFreeMat/ParserInterface.hpp \
libs/libFreeMat/QRDecompose.hpp \
libs/libFreeMat/Reserved.hpp \
libs/libFreeMat/Scope.hpp \
libs/libFreeMat/Serialize.hpp \
libs/libFreeMat/SingularValueDecompose.hpp \
libs/libFreeMat/Types.hpp \
libs/libFreeMat/WalkTree.hpp \
libs/libFreeMat/File.hpp \
libs/libFreeMat/Stream.hpp \
libs/libFreeMat/Interface.hpp \
libs/libFreeMat/SymbolTable.hpp \
libs/libFreeMat/Sparse.hpp \
libs/libFreeMat/Class.hpp \
libs/libFreeMat/NumericArray.hpp \
libs/libFreeMat/Parser.h 

HEADERS += libs/libCore/Utils.hpp \
libs/libCore/LoadCore.hpp \
libs/libCore/Core.hpp \
libs/libCore/MPIWrap.hpp \
libs/libCore/RanLib.hpp \
libs/libCore/helpwidget.hpp

HEADERS += libs/libFN/LoadFN.hpp \
libs/libFN/FN.hpp

HEADERS += libs/libGraphics/HandleAxis.hpp \
libs/libGraphics/HandleObject.hpp \
libs/libGraphics/HandleProperty.hpp \
libs/libGraphics/HandleFigure.hpp \
libs/libGraphics/GLLabel.hpp \
libs/libGraphics/GLRenderEngine.hpp \
libs/libGraphics/HandleText.hpp \
libs/libGraphics/RenderEngine.hpp \
libs/libGraphics/HandleLineSeries.hpp \
libs/libGraphics/RenderEngine.hpp \
libs/libGraphics/HandleCommands.hpp \
libs/libGraphics/HandleImage.hpp \
libs/libGraphics/HandleSurface.hpp \
libs/libGraphics/QTRenderEngine.hpp \
libs/libGraphics/HandleWindow.hpp

HEADERS+=libs/libXP/DynLib.hpp \
libs/libXP/System.hpp \
libs/libXP/PathSearch.hpp \
libs/libXP/KeyManager.hpp \
libs/libXP/TermWidget.hpp \
libs/libXP/GUITerminal.hpp \
libs/libXP/QTTerm.hpp 

HEADERS+=src/MainApp.hpp src/SocketCB.hpp src/application.h src/highlighter.hpp

SOURCES += libs/libFreeMat/NewLex.cpp \
libs/libFreeMat/Array.cpp \
libs/libFreeMat/AST.cpp \
libs/libFreeMat/Math.cpp \
libs/libFreeMat/Command.cpp \
libs/libFreeMat/Context.cpp \
libs/libFreeMat/Data.cpp \
libs/libFreeMat/Dimensions.cpp \
libs/libFreeMat/EigenDecompose.cpp \
libs/libFreeMat/Exception.cpp \
libs/libFreeMat/FunctionDef.cpp \
libs/libFreeMat/IEEEFP.cpp \
libs/libFreeMat/LAPACK.cpp \
libs/libFreeMat/LeastSquaresSolver.cpp \
libs/libFreeMat/LinearEqSolver.cpp \
libs/libFreeMat/LUDecompose.cpp \
libs/libFreeMat/Malloc.cpp \
libs/libFreeMat/MatrixMultiply.cpp \
libs/libFreeMat/Module.cpp \
libs/libFreeMat/QRDecompose.cpp \
libs/libFreeMat/Reserved.cpp \
libs/libFreeMat/Scope.cpp \
libs/libFreeMat/Serialize.cpp \
libs/libFreeMat/SingularValueDecompose.cpp \
libs/libFreeMat/WalkTree.cpp \
libs/libFreeMat/File.cpp \
libs/libFreeMat/Interface.cpp \
libs/libFreeMat/Sparse.cpp \
libs/libFreeMat/Class.cpp \
libs/libFreeMat/NumericArray.cpp \
libs/libFreeMat/Parser.cxx

SOURCES += libs/libCore/Cast.cpp \
libs/libCore/Constructors.cpp \
libs/libCore/FFT.cpp \
libs/libCore/StringOps.cpp \
libs/libCore/Transcendental.cpp \
libs/libCore/Trigonometric.cpp \
libs/libCore/Misc.cpp \
libs/libCore/Constants.cpp \
libs/libCore/Directory.cpp \
libs/libCore/Analyze.cpp \
libs/libCore/Utils.cpp \
libs/libCore/IO.cpp \
libs/libCore/Inspect.cpp \
libs/libCore/Random.cpp \
libs/libCore/LoadCore.cpp \
libs/libCore/MPIWrap.cpp \
libs/libCore/RanLib.cpp \
libs/libCore/helpwidget.cpp

SOURCES += libs/libFN/OptFun.cpp \
libs/libFN/LoadFN.cpp \
libs/libFN/Interp1D.cpp  \
libs/libFN/FNFun.cpp 

F77_SOURCES = libs/libFN/lmdif1.f \ 
libs/libFN/lmdif.f \  
libs/libFN/lmpar.f \  
libs/libFN/qrfac.f \
libs/libFN/dpmpar.f \ 
libs/libFN/enorm.f \ 
libs/libFN/qrsolv.f \ 
libs/libFN/fdjac2.f \
libs/libFN/erf.f \ 
libs/libFN/derf.f \ 
libs/libFN/ei.f \ 
libs/libFN/dei.f \ 
libs/libFN/daw.f \ 
libs/libFN/ddaw.f \
libs/libFN/psi.f \ 
libs/libFN/dpsi.f \ 
libs/libFN/gamma.f \ 
libs/libFN/dgamma.f \ 
libs/libFN/algama.f \ 
libs/libFN/dlgama.f  

SOURCES += libs/libGraphics/HandleAxis.cpp \
libs/libGraphics/HandleObject.cpp \
libs/libGraphics/HandleProperty.cpp \
libs/libGraphics/HandleFigure.cpp \
libs/libGraphics/HandleText.cpp \
libs/libGraphics/GLLabel.cpp \
libs/libGraphics/GLRenderEngine.cpp \
libs/libGraphics/HandleLineSeries.cpp \
libs/libGraphics/RenderEngine.cpp \
libs/libGraphics/HandleCommands.cpp \
libs/libGraphics/HandleImage.cpp \
libs/libGraphics/HandleSurface.cpp \
libs/libGraphics/QTRenderEngine.cpp \
libs/libGraphics/HandleWindow.cpp

SOURCES += libs/libXP/DynLib.cpp \
libs/libXP/TermWidget.cpp \
libs/libXP/System.cpp \
libs/libXP/PathSearch.cpp \
libs/libXP/KeyManager.cpp \
libs/libXP/GUITerminal.cpp \
libs/libXP/QTTerm.cpp 

SOURCES += src/MainApp.cpp src/SocketCB.cpp src/application.cpp src/main.cpp src/highlighter.cpp

cbundle.path = tools/disttool/FreeMat/Contents/bin
cbundle.files = release/FreeMat.exe 

INSTALLS += cbundle

!mac:unix {
  HEADERS += src/Terminal.hpp src/DumbTerminal.hpp
  SOURCES += src/Terminal.cpp src/DumbTerminal.cpp
}

F77 = g77

ff77.output = ${QMAKE_FILE_BASE}.o
ff77.commands = $$F77 -c ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
ff77.input = F77_SOURCES
QMAKE_EXTRA_COMPILERS += ff77


win32 {
RC_FILE = src/freemat.rc
}

!win32 {
OBJECTS_DIR = build
}

mac {
RC_FILE = src/appIcon.icns
}
