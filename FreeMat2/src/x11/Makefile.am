if NEED_PNG
  PNGLIB = @top_builddir@/libs/lpng125/libpng.a @top_builddir@/libs/libz/libz.a
endif

if NEED_TIFF
  TIFFLIB = @top_builddir@/libs/libtiff361/libtiff/libtiff.a
endif

if NEED_JPEG
  JPEGLIB = @top_builddir@/libs/libjpeg6b/libjpeg.a
endif

INCLUDES=-I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libFN -I@top_srcdir@/libs/libGraphics -I@top_builddir@/libs/libffi/include -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libFLTK

bin_PROGRAMS = FreeMat

FreeMat_SOURCES = main.cpp \
DumbTerminal.cpp \
DumbTerminal.hpp \
Terminal.cpp \
Terminal.hpp \
XWindow.cpp \
XWindow.hpp \
XGC.hpp \
XGC.cpp

FreeMat_DEPENDENCIES = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libFFTPack/libFFTPack.a  @top_builddir@/libs/libffi/libffi.a @top_builddir@/libs/libXP/libXP.a $(PNGLIB) $(TIFFLIB) $(JPEGLIB) @top_builddir@/libs/libFLTK/libFLTK.a
FreeMat_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libFFTPack/libFFTPack.a  @top_builddir@/libs/libffi/libffi.a @top_builddir@/libs/libXP/libXP.a @top_builddir@/libs/libFLTK/libFLTK.a $(PNGLIB) $(TIFFLIB) $(JPEGLIB) @X_PRE_LIBS@ @X_LIBS@ -lX11 @X_EXTRA_LIBS@ -L/sw/lib -lg2c 
