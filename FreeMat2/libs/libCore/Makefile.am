EXTRA_DIST = libCore.dsp

noinst_LIBRARIES = libCore.a

INCLUDES= -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libtecla -I@top_builddir@/libs/libffi/include -I@top_srcdir@/libs/libXP

libCore_a_SOURCES = Cast.cpp \
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
Utils.hpp \
IO.cpp \
Inspect.cpp \
Random.cpp \
LoadCore.cpp \
LoadCore.hpp \
Core.hpp \
MPIWrap.hpp \
MPIWrap.cpp

AUTOMAKE_OPTIONS 	= foreign
