noinst_LIBRARIES = libGraphics.a

INCLUDES = -I@top_srcdir@/libs/libFreeMat -I@top_srcdir@/libs/libCore -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libMex

libGraphics_a_MOC = HandleUIControl.moc.cpp \
HandleMultiEdit.moc.cpp

BUILT_SOURCES = $(libGraphics_a_MOC)

libGraphics_a_HFILES = GLRenderEngine.hpp \
HandleAxis.hpp \
HandleCommands.hpp \
HandleFigure.hpp \
HandleImage.hpp \
HandleLineSeries.hpp \
HandleObject.hpp \
HandleProperty.hpp \
HandleSurface.hpp \
HandleText.hpp \
HandleWindow.hpp \
QTRenderEngine.hpp \
RenderEngine.hpp \
HandleUIControl.hpp \
HandleMultiEdit.hpp 

libGraphics_a_SOURCES = GLRenderEngine.cpp \
HandleAxis.cpp \
HandleCommands.cpp \
HandleFigure.cpp \
HandleImage.cpp \
HandleLineSeries.cpp \
HandleObject.cpp \
HandleProperty.cpp \
HandleSurface.cpp \
HandleText.cpp \
HandleWindow.cpp \
LoadGraphicsCore.cpp \
QTRenderEngine.cpp \
RenderEngine.cpp \
HandleUIControl.cpp \
HandleMultiEdit.cpp \
$(libGraphics_a_HFILES)

nodist_libGraphics_a_SOURCES = $(libGraphics_a_MOC) 

AUTOMAKE_OPTIONS 	= foreign

AM_CPPFLAGS = $(QT_CFLAGS) 

CLEANFILES = $(libGraphics_a_MOC)

%.moc.cpp: %.hpp
	$(QT_MOC) -o $@ $<

