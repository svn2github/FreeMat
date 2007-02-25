noinst_LIBRARIES = libCore.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP -I@top_builddir@/libs/libXP

libCore_a_MOC = helpwidget.moc.cpp XML.moc.cpp 

BUILT_SOURCES = $(libCore_a_MOC)

libCore_a_HFILES = Core.hpp \
helpwidget.hpp \
LoadCore.hpp \
MPIWrap.hpp \
RanLib.hpp \
Utils.hpp \
MatIO.hpp \
XML.hpp \
RPC.hpp

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
MatIO.cpp \
XML.cpp \
RPC.cpp \
$(libCore_a_HFILES)

nodist_libCore_a_SOURCES = $(libCore_a_MOC) 

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(libCore_a_MOC)

AM_CPPFLAGS = $(QT_CFLAGS) 

XML.moc.cpp: XML.hpp
	$(QT_MOC) -o XML.moc.cpp XML.hpp

helpwidget.moc.cpp: helpwidget.hpp
	$(QT_MOC) -o helpwidget.moc.cpp helpwidget.hpp
