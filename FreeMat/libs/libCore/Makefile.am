noinst_LIBRARIES = libCore.a

CFLAGS=@CFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cflags`
CXXFLAGS=@CXXFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cxxflags` -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libtecla -I@top_srcdir@/extlibs/libffi-2.00-beta/include

libCore_a_SOURCES = Cast.cpp \
Constructors.cpp \
FFT.cpp \
StringOps.cpp \
Transcendental.cpp \
Trigonometric.cpp \
Misc.cpp \
Constants.cpp \
Directory.cpp \
Remote.cpp \
Analyze.cpp \
Utils.cpp \
Utils.hpp \
IO.cpp \
Inspect.cpp \
Random.cpp \
LoadCore.cpp \
LoadCore.hpp \
Core.hpp 

EXTRA_DIST		= bootstrap
AUTOMAKE_OPTIONS 	= foreign
