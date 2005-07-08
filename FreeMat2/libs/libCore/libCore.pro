TEMPLATE = lib

TARGET = Core

CONFIG += staticlib

INCLUDEPATH = ../libFreeMat ../libXP ../..

DEPENDPATH += INCLUDEPATH

HEADERS += Utils.hpp \
LoadCore.hpp \
Core.hpp \
MPIWrap.hpp \
RanLib.hpp \
hwin.h

SOURCES += Cast.cpp \
Constructors.cpp \
FFT.cpp \
StringOps.cpp \
Transcendental.cpp \
Trigonometric.cpp \
Misc.cpp \
Constants.cpp \
Directory.cpp \
Analyze.cpp \
Utils.cpp \
IO.cpp \
Inspect.cpp \
Random.cpp \
LoadCore.cpp \
MPIWrap.cpp \
RanLib.cpp \
hwin.cpp

QT += qt3support

CONFIG += assistant

