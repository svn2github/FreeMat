CFLAGS=@CFLAGS@ 
CXXFLAGS=@CXXFLAGS@   -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libGraphics -I@top_srcdir@/libs/libffi/include -I@top_srcdir@/libs/libXP

bin_PROGRAMS = FreeMat

FreeMat_SOURCES = main.cpp \
WinTerminal.cpp \
WinTerminal.hpp \
XWindow.cpp \
XWindow.hpp

FreeMat_DEPENDENCIES = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libFFTPack/libFFTPack.a  @top_builddir@/libs/libffi/.libs/libffi_convenience.a @top_builddir@/libs/libXP/libXP.a
FreeMat_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libFFTPack/libFFTPack.a  @top_builddir@/libs/libffi/.libs/libffi_convenience.a @top_builddir@/libs/libXP/libXP.a @X_PRE_LIBS@ @X_LIBS@ -lX11 @X_EXTRA_LIBS@ -L/sw/lib -lg2c
