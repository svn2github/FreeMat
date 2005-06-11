TEMPLATE = app

TARGET = FreeMat

INCLUDEPATH += . ../../libs/libFreeMat ../../libs/libCore ../../libs/libFN ../../libs/libGraphics ../../libs/libffi/include ../../libs/libXP  ../..

DEPENDPATH += INCLUDEPATH

HEADERS = MainApp.hpp

SOURCES = MainApp.cpp main.cpp

unix {
HEADERS += Terminal.hpp SocketCB.hpp
SOURCES += Terminal.cpp
}

LIBS += -L../../libs/libCore -lCore -L../../libs/libFN -lFN -L../../libs/libGraphics -lGraphics   -L../../libs/libFFTPack -lFFTPack  -L../../libs/libFreeMat -lFreeMatLib -L../../libs/libffi -lffi -L../../libs/libXP -lXP -L../../libs/libARPACK -lARPACK -L../../libs/libUMFPACK/UMFPACKdi -lUMFPACKdi -L../../libs/libUMFPACK/UMFPACKzi -lUMFPACKzi -L../../libs/libUMFPACK/AMD -lAMD -lblas -llapack -lcurses -lg2c

TARGETDEPS += ../../libs/libCore/libCore.a ../../libs/libFN/libFN.a ../../libs/libGraphics/libGraphics.a ../../libs/libFFTPack/libFFTPack.a ../../libs/libFreeMat/libFreeMatLib.a ../../libs/libffi/libffi.a ../../libs/libXP/libXP.a ../../libs/libARPACK/libARPACK.a ../../libs/libUMFPACK/UMFPACKdi/libUMFPACKdi.a ../../libs/libUMFPACK/UMFPACKzi/libUMFPACKzi.a ../../libs/libUMFPACK/AMD/libAMD.a
