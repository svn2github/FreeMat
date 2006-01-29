TEMPLATE = app

TARGET = FreeMat

CONFIG += assistant debug

INCLUDEPATH += . ../libs/libFreeMat ../libs/libCore ../libs/libFN ../libs/libGraphics ../libs/libXP  ../..

DEPENDPATH += INCLUDEPATH

HEADERS =  MainApp.hpp SocketCB.hpp application.h
SOURCES =  MainApp.cpp SocketCB.cpp application.cpp main.cpp


win32 {

debug {
LIBS += -L../libs/libCore/Debug -lCore -L../libs/libFN/Debug -lFN -L../libs/libGraphics/Debug -lGraphics   -L../libs/libFFTPack/Debug -lFFTPack  -L../libs/libFreeMat/Debug -lFreeMatLib -L../libs/libffi/Debug -lffi -L../libs/libXP/Debug -lXP -L../libs/libARPACK/UTIL/Debug -L../libs/libARPACK/SRC/Debug -lARPACKMain -lARPACKUtil -L../libs/libUMFPACK/UMFPACKdi/Debug -lUMFPACKdi -L../libs/libUMFPACK/UMFPACKzi/Debug -lUMFPACKzi -L../libs/libUMFPACK/AMD/Debug -lAMD

TARGETDEPS += ../libs/libCore/Debug/libCore.a ../libs/libFN/Debug/libFN.a ../libs/libGraphics/Debug/libGraphics.a ../libs/libFFTPack/Debug/libFFTPack.a ../libs/libFreeMat/Debug/libFreeMatLib.a ../libs/libffi/Debug/libffi.a ../libs/libXP/Debug/libXP.a ../libs/libARPACK/SRC/Debug/libARPACKMain.a  ../libs/libARPACK/UTIL/Debug/libARPACKUtil.a ../libs/libUMFPACK/UMFPACKdi/Debug/libUMFPACKdi.a ../libs/libUMFPACK/UMFPACKzi/Debug/libUMFPACKzi.a ../libs/libUMFPACK/AMD/Debug/libAMD.a
}

#release {
#TARGETDEPS += ../libs/libCore/Release/libCore.a ../libs/libFN/Release/libFN.a ../libs/libGraphics/Release/libGraphics.a ../libs/libFFTPack/Release/libFFTPack.a ../libs/libFreeMat/Release/libFreeMatLib.a ../libs/libffi/Release/libffi.a ../libs/libXP/Release/libXP.a ../libs/libARPACK/SRC/Release/libARPACKMain.a  ../libs/libARPACK/UTIL/Release/libARPACKUtil.a ../libs/libUMFPACK/UMFPACKdi/Release/libUMFPACKdi.a ../libs/libUMFPACK/UMFPACKzi/Release/libUMFPACKzi.a ../libs/libUMFPACK/AMD/Release/libAMD.a
#}
}

!win32 {
LIBS += -L../libs/libCore -lCore -L../libs/libFN -lFN -L../libs/libGraphics -lGraphics   -L../libs/libFFTPack -lFFTPack  -L../libs/libFreeMat -lFreeMatLib -L../libs/libffi -lffi -L../libs/libXP -lXP -L../libs/libARPACK/UTIL -L../libs/libARPACK/SRC -lARPACKMain -lARPACKUtil -L../libs/libUMFPACK/UMFPACKdi -lUMFPACKdi -L../libs/libUMFPACK/UMFPACKzi -lUMFPACKzi -L../libs/libUMFPACK/AMD -lAMD

TARGETDEPS += ../libs/libCore/libCore.a ../libs/libFN/libFN.a ../libs/libGraphics/libGraphics.a ../libs/libFFTPack/libFFTPack.a ../libs/libFreeMat/libFreeMatLib.a ../libs/libffi/libffi.a ../libs/libXP/libXP.a ../libs/libARPACK/SRC/libARPACKMain.a  ../libs/libARPACK/UTIL/libARPACKUtil.a ../libs/libUMFPACK/UMFPACKdi/libUMFPACKdi.a ../libs/libUMFPACK/UMFPACKzi/libUMFPACKzi.a ../libs/libUMFPACK/AMD/libAMD.a
}

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

LIBS += ../extern/LAPACK/debug/liblapack.a ../extern/LAPACK/BLAS/debug/libblas.a -lg2c -lws2_32 -lopengl32 -lglu32 -lgdi32

#TARGETDEPS += ../extern/LAPACK/lapack_MINGW.a ../extern/blas/libblas.a

RC_FILE = freemat.rc
}

QT += opengl


