noinst_LIBRARIES = libGraphics.a

CFLAGS=@CFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cflags`
CXXFLAGS=@CXXFLAGS@ `@top_builddir@/extlibs/wxWindows/wx-config --prefix=@top_srcdir@/extlibs/wxWindows --exec-prefix=@top_builddir@/extlibs/wxWindows --cxxflags` -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/extlibs/libtecla -I@top_srcdir@/src  -I@top_srcdir@/extlibs/libffi-2.00-beta/include -I@top_srcdir@/extlibs/volpack-1.0c2/src

libGraphics_a_SOURCES = Axis.cpp \
Axis.hpp \
DataSet2D.cpp \
DataSet2D.hpp \
LoadGraphicsCore.cpp \
GraphicsCore.hpp \
Plot2D.cpp \
Plot2D.hpp \
PrintableFrame.cpp \
PrintableFrame.hpp \
ScalarImage.cpp \
ScalarImage.hpp \
PlotCommands.cpp \
ImageCommands.cpp \
VolumeCommands.cpp \
Dialogs.cpp \
GraphicsServer.cpp \
GraphicsServer.hpp \
VolView.cpp \
VolView.hpp \
trackball.c \
trackball.h

EXTRA_DIST		= bootstrap
AUTOMAKE_OPTIONS 	= foreign
