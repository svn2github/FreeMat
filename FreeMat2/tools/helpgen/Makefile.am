check_PROGRAMS = helpgen

EXTRA_DIST = section_descriptors.txt

helpgen_HFILES = helpgen.hpp

helpgen_MOC = helpgen.moc.cpp

BUILT_SOURCES = $(helpgen_MOC)

helpgen_SOURCES = helpgen.cpp \
$(helpgen_HFILES)

nodist_helpgen_SOURCES = $(helpgen_MOC) 

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libGraphics -I@top_srcdir@/libs/libFN -I@top_srcdir@/src

helpgen_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libXP/libXP.a @top_builddir@/libs/libMex/libMex.a $(QGL_LIBS) $(QT_LIBS)
helpgen_LDFLAGS = $(QT_CXXFLAGS)

AUTOMAKE_OPTIONS 	= foreign

CLEANFILES = $(FreeMat_MOC)

AM_CPPFLAGS = $(QT_CFLAGS) 

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<
