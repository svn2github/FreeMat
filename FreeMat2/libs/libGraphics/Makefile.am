EXTRA_DIST = libGraphics.dsp

noinst_LIBRARIES = libGraphics.a

CFLAGS=@CFLAGS@ @X_CFLAGS@
CXXFLAGS=@CXXFLAGS@  
INCLUDES=-I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libXP -I@top_builddir@/libs/libffi/include -I@top_srcdir@/src/x11

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
AUTOMAKE_OPTIONS 	= foreign
