noinst_LIBRARIES = libCore.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP 

libCore_a_MOC = helpwidget.moc.cpp

BUILT_SOURCES = $(libCore_a_MOC)

libCore_a_HFILES = Core.hpp \
helpwidget.hpp \
LoadCore.hpp \
MPIWrap.hpp \
RanLib.hpp \
Utils.hpp 

libCore_a_SOURCES = Analyze.cpp \
Cast.cpp \
Constants.cpp \
Constructors.cpp \
Directory.cpp \
FFT.cpp \
helpwidget.cpp \
Inspect.cpp \
IO.cpp \
LoadCore.cpp \
Misc.cpp \
MPIWrap.cpp \
Random.cpp \
RanLib.cpp \
StringOps.cpp \
Transcendental.cpp \
Trigonometric.cpp \
Utils.cpp \
$(libCore_a_MOC) \
$(libCore_a_HFILES)

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(libCore_a_MOC)

AM_CPPFLAGS = $(QT_CXXFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<
