TEMPLATE = app

TARGET = FreeMat

INCLUDEPATH += . ../libs/libFreeMat ../libs/libCore ../libs/libFN ../libs/libGraphics ../libs/libffi/include ../libs/libXP  ../..

DEPENDPATH += INCLUDEPATH

HEADERS =  MainApp.hpp SocketCB.hpp application.h
SOURCES =  MainApp.cpp SocketCB.cpp application.cpp main.cpp

LIBS += -L../libs/libCore -lCore -L../libs/libFN -lFN -L../libs/libGraphics -lGraphics   -L../libs/libFFTPack -lFFTPack  -L../libs/libFreeMat -lFreeMatLib -L../libs/libffi -lffi -L../libs/libXP -lXP -L../libs/libARPACK/UTIL -L../libs/libARPACK/SRC -lARPACKMain -lARPACKUtil -L../libs/libUMFPACK/UMFPACKdi -lUMFPACKdi -L../libs/libUMFPACK/UMFPACKzi -lUMFPACKzi -L../libs/libUMFPACK/AMD -lAMD

TARGETDEPS += ../libs/libCore/libCore.a ../libs/libFN/libFN.a ../libs/libGraphics/libGraphics.a ../libs/libFFTPack/libFFTPack.a ../libs/libFreeMat/libFreeMatLib.a ../libs/libffi/libffi.a ../libs/libXP/libXP.a ../libs/libARPACK/SRC/libARPACKMain.a  ../libs/libARPACK/UTIL/libARPACKUtil.a ../libs/libUMFPACK/UMFPACKdi/libUMFPACKdi.a ../libs/libUMFPACK/UMFPACKzi/libUMFPACKzi.a ../libs/libUMFPACK/AMD/libAMD.a


mac {
QT += qt3support
LIBS +=  -framework vecLib -L/sw/lib -lg2c
RC_FILE = appIcon.icns
}

!mac {
unix {
HEADERS += Terminal.hpp DumbTerminal.hpp
SOURCES += Terminal.cpp DumbTerminal.cpp
LIBS += -lblas -llapack -lcurses -lg2c
}
}


win32 {
QT += qt3support

LIBS += ../extern/LAPACK/lapack_MINGW.a -L../extern/blas -lblas -lg2c -lws2_32

TARGETDEPS += ../extern/LAPACK/lapack_MINGW.a ../extern/blas/libblas.a

RC_FILE = freemat.rc
}

CONFIG += assistant 
QT += opengl


