AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = mac.r Info.plist appIcon.icns

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

FreeMat_SOURCES = main.cpp 

FreeMat_DEPENDENCIES = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a @top_builddir@/libs/libFreeMat/libFreeMat.a @top_builddir@/libs/libFFTPack/libFFTPack.a  @top_builddir@/libs/libffi/libffi.a @top_builddir@/libs/libXP/libXP.a $(PNGLIB) $(TIFFLIB) $(JPEGLIB) @top_builddir@/libs/libFLTK/libFLTK.a
FreeMat_LDADD = @top_builddir@/libs/libCore/libCore.a @top_builddir@/libs/libFN/libFN.a @top_builddir@/libs/libGraphics/libGraphics.a  @top_builddir@/libs/libFFTPack/libFFTPack.a @top_builddir@/libs/libXP/libXP.a @top_builddir@/libs/libFreeMat/libFreeMat.a  @top_builddir@/libs/libXP/libXP.a @top_builddir@/libs/libFLTK/libFLTK.a @top_builddir@/libs/libffi/libffi.a $(PNGLIB) $(TIFFLIB) $(JPEGLIB)  -lg2c 

bundle:
	/Developer/Tools/Rez @top_srcdir@/src/mac/mac.r -o @top_builddir@/src/mac/FreeMat
	mkdir -p @top_builddir@/src/mac/FreeMat.app
	mkdir -p @top_builddir@/src/mac/FreeMat.app/Contents
	cp @top_srcdir@/src/mac/Info.plist @top_builddir@/src/mac/FreeMat.app/Contents/.
	mkdir -p @top_builddir@/src/mac/FreeMat.app/Contents/MacOS
	mkdir -p @top_builddir@/src/mac/FreeMat.app/Contents/Resources
	rm -f @top_builddir@/src/mac/FreeMat.app/FreeMat
	cp @top_builddir@/src/mac/FreeMat @top_builddir@/src/mac/FreeMat.app/Contents/MacOS/.
	ln -s @top_builddir@/src/mac/FreeMat.app/Contents/MacOS/FreeMat @top_builddir@/src/mac/FreeMat.app/FreeMat
	cp @top_srcdir@/src/mac/appIcon.icns @top_builddir@/src/mac/FreeMat.app/Contents/Resources/.
	cp -R $(prefix)/share/FreeMat/html @top_builddir@/src/mac/FreeMat.app/Contents/Resources/.
	cp -R $(prefix)/share/FreeMat/MFiles @top_builddir@/src/mac/FreeMat.app/Contents/Resources/.
	hdiutil create -fs HFS+ -srcfolder @top_builddir@/src/mac/FreeMat.app @top_builddir@/src/mac/FreeMat.dmg
	echo "Rename .dmg with version number!"
