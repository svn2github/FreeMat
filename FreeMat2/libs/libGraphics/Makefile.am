noinst_LIBRARIES = libGraphics.a

CFLAGS=@CFLAGS@ @X_CFLAGS@
CXXFLAGS=@CXXFLAGS@  -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libffi/include -I@top_srcdir@/@maindir@

libGraphics_a_SOURCES = Axis.cpp \
Axis.hpp \
DataSet2D.cpp \
DataSet2D.hpp \
LoadGraphicsCore.cpp \
GraphicsCore.hpp \
Plot2D.cpp \
Plot2D.hpp \
PlotCommands.cpp \
ImageCommands.cpp \
ScalarImage.cpp \
ScalarImage.hpp

EXTRA_DIST		= bootstrap
AUTOMAKE_OPTIONS 	= foreign
