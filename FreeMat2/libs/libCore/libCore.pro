TEMPLATE = lib

TARGET = Core

CONFIG += staticlib warn_off create_prl

INCLUDEPATH = ../libFreeMat ../libXP ../../extern/fftw-3.0.1/api

DEPENDPATH += INCLUDEPATH

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

