TEMPLATE = app

QT += opengl

CONFIG += warn_off release

DEFINES -= UNICODE

TARGET = FreeMat

include($$OUT_PWD/conf.pri)

!win32 {
OBJECTS_DIR = build
}
MOC_DIR = build
RCC_DIR = build
DESTDIR = build

blas.target = extern/LAPACK/BLAS/libblas.a
blas.commands = cd extern/LAPACK/BLAS/SRC && make -f Makefile_freemat FC=$$F77

atlas.target = extern/ATLAS/lib/BLAS_FreeMat/libatlas.a
atlas.commands = cd extern && tar xfz atlas3.6.0.tar.gz && cd ATLAS && make < ../chat && make install arch=BLAS_FreeMat

fftw_double.target = extern/fftw-3.0.1/.libs/libfftw3.a
fftw_double.commands = cd extern && tar xfz fftw-3.0.1.tar.gz && cd fftw-3.0.1 && ./configure && make

fftw_single.target = extern/fftw-3.0.1/.libs/libfftw3f.a
fftw_single.commands = cd extern/fftw-3.0.1 && ./configure --enable-single && make

avcall.target = extern/ffcall-1.10/avcall/.libs/libavcall.a
avcall.commands = cd extern && tar xfz ffcall-1.10_freemat_patch.tar.gz && cd ffcall-1.10 && ./configure && make

amd.target = extern/AMD/Lib/libamd.a
amd.commands = cd extern && tar xfz AMD-1.2.tar.gz &&  cd AMD && make

umfpack.target = extern/UMFPACK/Lib/libumfpack.a
umfpack.commands = cd extern && tar xfz UFconfig-1.0_freemat_patch.tar.gz &&  tar xfz AMD-1.2.tar.gz && tar xfz UMFPACK-4.6.tar.gz && cd UMFPACK/Source && make

arpack.target = extern/ARPACK/libarpack.a
arpack.commands = cd extern && tar xfz arpack96_freemat_patch.tar.gz && cd ARPACK && make FC=$$F77

lapack.target = extern/LAPACK/liblapack.a
lapack.commands = cd extern && tar xfz lapack-3.0_freemat_patch.tgz && cd LAPACK/SRC && make -f Makefile_freemat FC=$$F77


!macx {
RUNTARGET = $$DESTDIR/$$TARGET
}

macx {
RUNTARGET = $$DESTDIR/$$TARGET/Contents/MacOS/FreeMat
}

help.target = help
help.depends = FORCE
help.commands = $$RUNTARGET -helpgen

check.target = check
check.depends = FORCE
check.commands = $$RUNTARGET -f "cd tests/core; test_core"

install.target = install
!macx {
install.commands = cd tools/disttool && qmake && make && rm -rf FreeMat && ./disttool -linux && cd ../.. && rm -rf FreeMat && mv tools/disttool/FreeMat .
}

macx {
install.commands = cd tools/disttool && qmake && make && ./disttool -mac && cd ../../ &&  mv build/FreeMat.app . && find FreeMat.app -name '*debug' -exec rm \{\} \;
}

QMAKE_EXTRA_TARGETS += fftw_double fftw_single avcall amd umfpack arpack lapack blas atlas package help check install

INCLUDEPATH += libs/libFreeMat libs/libCore libs/libFN libs/libGraphics libs/libXP src

macx {
LIBS += -framework vecLib -L/sw/lib -lg2c
}

unix:!macx {
LIBS += -lncurses
}

DEPENDPATH += INCLUDEPATH

HEADERS += libs/libFreeMat/Array.hpp \
libs/libFreeMat/AST.hpp \
libs/libFreeMat/Math.hpp \
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

HEADERS+=src/MainApp.hpp src/SocketCB.hpp src/application.h src/highlighter.hpp src/helpgen.hpp src/PathTool.hpp src/Editor.hpp src/ToolDock.hpp src/HistoryWidget.hpp src/FileTool.hpp src/Common.hpp

FMSOURCES += libs/libFreeMat/NewLex.cpp \
libs/libFreeMat/Array.cpp \
libs/libFreeMat/AST.cpp \
libs/libFreeMat/Math.cpp \
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

FMSOURCES += libs/libCore/Cast.cpp \
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

FMSOURCES += libs/libFN/OptFun.cpp \
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

FMSOURCES += libs/libGraphics/HandleAxis.cpp \
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

FMSOURCES += libs/libXP/DynLib.cpp \
libs/libXP/TermWidget.cpp \
libs/libXP/System.cpp \
libs/libXP/PathSearch.cpp \
libs/libXP/KeyManager.cpp \
libs/libXP/GUITerminal.cpp \
libs/libXP/QTTerm.cpp 

SOURCES += $$FMSOURCES src/MainApp.cpp src/SocketCB.cpp src/application.cpp src/main.cpp src/highlighter.cpp src/helpgen.cpp src/PathTool.cpp src/Editor.cpp src/ToolDock.cpp src/HistoryWidget.cpp src/FileTool.cpp src/Common.cpp

!mac:unix {
  HEADERS += src/Terminal.hpp src/DumbTerminal.hpp src/FuncTerminal.hpp
  SOURCES += src/Terminal.cpp src/DumbTerminal.cpp src/FuncTerminal.cpp
}

win32 {
RC_FILE = src/freemat.rc
}


mac {
RC_FILE = src/appIcon.icns
}


ff77.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}.o
ff77.commands = $$F77 -c ${QMAKE_FILE_NAME} -o ${OBJECTS_DIR}${QMAKE_FILE_BASE}.o
ff77.input = F77_SOURCES
QMAKE_EXTRA_COMPILERS += ff77

RESOURCES = FreeMat.qrc


DISTFILES += configure images/close.png images/copy.png images/cut.png images/freemat-2.xpm images/home.png images/new.png images/next.png images/open.png images/paste.png images/previous.png images/quit.png images/save.png images/zoomin.png images/zoomout.png
DISTFILES += extern/AMD-1.2.tar.gz extern/arpack96_freemat_patch.tar.gz extern/atlas3.6.0.tar.gz extern/ffcall-1.10_freemat_patch.tar.gz extern/fftw-3.0.1.tar.gz extern/lapack-3.0_freemat_patch.tgz extern/UFconfig-1.0_freemat_patch.tar.gz extern/UMFPACK-4.6.tar.gz
DISTFILES += help/section_descriptors.txt
DISTFILES += $$system(find MFiles -name '*.m')
DISTFILES += $$system(find tests -name '*.m')
DISTFILES += tools/disttool/disttool.cpp tools/disttool/disttool.hpp tools/disttool/disttool.pro tools/disttool/freemat_nsi.in
