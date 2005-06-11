TEMPLATE = app

TARGET = FreeMat

INCLUDEPATH += . ../../libs/libFreeMat ../../libs/libCore ../../libs/libFN ../../libs/libGraphics ../../libs/libffi/include ../../libs/libXP  ../..

SOURCES += main.cpp MainApp.cpp 

HEADERS += MainApp.hpp

macx {
LIBS += -L../../libs/libCore -lCore -L../../libs/libFN -lFN -L../../libs/libGraphics -lGraphics   -L../../libs/libFFTPack -lFFTPack  -L../../libs/libFreeMat -lFreeMatLib -L../../libs/libffi -lffi -L../../libs/libXP -lXP -L../../libs/libARPACK -lARPACK -L../../libs/libUMFPACK/UMFPACKdi -lUMFPACKdi -L../../libs/libUMFPACK/UMFPACKzi -lUMFPACKzi -L../../libs/libUMFPACK/AMD -lAMD -framework vecLib -L/sw/lib -lg2c
}

unix {
LIBS += -L../../libs/libCore -lCore -L../../libs/libFN -lFN -L../../libs/libGraphics -lGraphics   -L../../libs/libFFTPack -lFFTPack  -L../../libs/libFreeMat -lFreeMatLib -L../../libs/libffi -lffi -L../../libs/libXP -lXP -L../../libs/libARPACK -lARPACK -L../../libs/libUMFPACK/UMFPACKdi -lUMFPACKdi -L../../libs/libUMFPACK/UMFPACKzi -lUMFPACKzi -L../../libs/libUMFPACK/AMD -lAMD -lblas -llapack -L/sw/lib -lg2c
}

TARGETDEPS += ../../libs/libCore/libCore.a ../../libs/libFN/libFN.a ../../libs/libGraphics/libGraphics.a ../../libs/libFFTPack/libFFTPack.a ../../libs/libFreeMat/libFreeMatLib.a ../../libs/libffi/libffi.a ../../libs/libXP/libXP.a ../../libs/libARPACK/libARPACK.a ../../libs/libUMFPACK/UMFPACKdi/libUMFPACKdi.a ../../libs/libUMFPACK/UMFPACKzi/libUMFPACKzi.a ../../libs/libUMFPACK/AMD/libAMD.a
