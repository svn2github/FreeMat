EXTRA_DIST = libXP.dsp libXP.vcproj

if NEED_PNG
  PNG_INCLUDES = -I@top_srcdir@/libs/lpng125 -I@top_srcdir@/libs/libz
endif

if NEED_TIFF
  TIFF_INCLUDES = -I@top_srcdir@/libs/libtiff361/libtiff
endif

if NEED_JPEG
  JPEG_INCLUDES = -I@top_srcdir@/libs/libjpeg6b
endif

noinst_LIBRARIES = libXP.a

libXP_a_SOURCES = DynLib.hpp \
DynLib.cpp \
System.hpp \
System.cpp \
PathSearch.cpp \
PathSearch.hpp \
GrayscaleImage.cpp \
GrayscaleImage.hpp \
RGBImage.cpp \
RGBImage.hpp \
Rect2D.cpp \
Rect2D.hpp \
PostScriptGC.cpp \
PostScriptGC.hpp \
Point2D.cpp \
Point2D.hpp \
GraphicsContext.hpp \
Color.cpp \
Color.hpp \
Reducer.hpp \
Reducer.cpp \
KeyManager.cpp \
KeyManager.hpp \
BitmapPrinterGC.cpp \
BitmapPrinterGC.hpp \
helv_table.h \
XPWidget.hpp \
XPWidget.cpp \
XPContainer.hpp \
XPContainer.cpp \
XPButton.hpp \
XPButton.cpp \
XPEditLine.hpp \
XPEditLine.cpp \
XPLabel.hpp \
XPLabel.cpp \
XPVSlider.hpp \
XPVSlider.cpp \
FLTKGC.hpp \
FLTKGC.cpp

AUTOMAKE_OPTIONS 	= foreign

INCLUDES= -I@top_srcdir@/libs/libFreeMat $(PNG_INCLUDES) $(TIFF_INCLUDES) $(JPEG_INCLUDES)  -I@top_srcdir@/libs/libFLTK
