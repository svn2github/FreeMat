TEMPLATE = app

TARGET = FreeMat

INCLUDEPATH += . ../libs/libFreeMat ../libs/libCore ../libs/libFN ../libs/libGraphics ../libs/libffi/include ../libs/libXP  ../..

DEPENDPATH += INCLUDEPATH

HEADERS = DumbTerminal.hpp MainApp.hpp SocketCB.hpp application.h
SOURCES = DumbTerminal.cpp MainApp.cpp SocketCB.cpp application.cpp main.cpp

unix {
HEADERS += Terminal.hpp
SOURCES += Terminal.cpp
LIBS += -L../libs/libCore -lCore -L../libs/libFN -lFN -L../libs/libGraphics -lGraphics   -L../libs/libFFTPack -lFFTPack  -L../libs/libFreeMat -lFreeMatLib -L../libs/libffi -lffi -L../libs/libXP -lXP -L../libs/libARPACK -lARPACK -L../libs/libUMFPACK/UMFPACKdi -lUMFPACKdi -L../libs/libUMFPACK/UMFPACKzi -lUMFPACKzi -L../libs/libUMFPACK/AMD -lAMD -lblas -llapack -lcurses -lg2c

TARGETDEPS += ../libs/libCore/libCore.a ../libs/libFN/libFN.a ../libs/libGraphics/libGraphics.a ../libs/libFFTPack/libFFTPack.a ../libs/libFreeMat/libFreeMatLib.a ../libs/libffi/libffi.a ../libs/libXP/libXP.a ../libs/libARPACK/libARPACK.a ../libs/libUMFPACK/UMFPACKdi/libUMFPACKdi.a ../libs/libUMFPACK/UMFPACKzi/libUMFPACKzi.a ../libs/libUMFPACK/AMD/libAMD.a
}

win32 {
QT += qt3support
### Will break qt3...
LIBS += -L../libs/libCore/Debug -lCore -L../libs/libFN/Debug -lFN -L../libs/libGraphics/Debug -lGraphics   -L../libs/libFFTPack/Debug -lFFTPack  -L../libs/libFreeMat/Debug -lFreeMatLib -L../libs/libffi -lffi -L../libs/libXP/Debug -lXP -L../libs/libARPACK/Debug -lARPACK -L../libs/libUMFPACK/UMFPACKdi/Debug -lUMFPACKdi -L../libs/libUMFPACK/UMFPACKzi/Debug -lUMFPACKzi -L../libs/libUMFPACK/AMD/Debug -lAMD  ../extern/LAPACK/lapack_MINGW.a -L../extern/blas -lblas -lg2c -lws2_32

TARGETDEPS += ../libs/libCore/Debug/libCore.a ../libs/libFN/Debug/libFN.a ../libs/libGraphics/Debug/libGraphics.a ../libs/libFFTPack/Debug/libFFTPack.a ../libs/libFreeMat/Debug/libFreeMatLib.a ../libs/libffi/libffi.a ../libs/libXP/Debug/libXP.a ../libs/libARPACK/Debug/libARPACK.a ../libs/libUMFPACK/UMFPACKdi/Debug/libUMFPACKdi.a ../libs/libUMFPACK/UMFPACKzi/Debug/libUMFPACKzi.a ../libs/libUMFPACK/AMD/Debug/libAMD.a
}

CONFIG += assistant
