TEMPLATE = lib

TARGET = Core

CONFIG += staticlib

INCLUDEPATH = ../libFreeMat ../libXP ../..

HEADERS += Utils.hpp \
LoadCore.hpp \
Core.hpp \
MPIWrap.hpp \
RanLib.hpp

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
RanLib.cpp 
