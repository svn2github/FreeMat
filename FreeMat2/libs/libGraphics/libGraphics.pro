TEMPLATE = lib

TARGET = Graphics

CONFIG += staticlib debug opengl warn_off

INCLUDEPATH = ../libFreeMat ../libXP ../.. ../libCore

HEADERS += DataSet2D.hpp \
GraphicsCore.hpp \
Plot2D.hpp \
ScalarImage.hpp \
trackball.h \
Figure.hpp \
SurfPlot.hpp \
NewAxis.hpp \
Util.hpp \
QPWidget.hpp \
ColorBar.hpp \
Label.hpp \
HandleAxis.hpp \
HandleObject.hpp \
HandleProperty.hpp \
HandleFigure.hpp \
GLLabel.hpp \
GLRenderEngine.hpp \
HandleText.hpp \
RenderEngine.hpp \
HandleLineSeries.hpp \
RenderEngine.hpp \
HandleCommands.hpp \
HandleImage.hpp \
HandleSurface.hpp \
QTRenderEngine.hpp \
HandleWindow.hpp

SOURCES += DataSet2D.cpp \
LoadGraphicsCore.cpp \
Plot2D.cpp \
PlotCommands.cpp \
ImageCommands.cpp \
ScalarImage.cpp \
trackball.c \
Figure.cpp \
SurfPlot.cpp \
NewAxis.cpp \
Util.cpp \
QPWidget.cpp \
ColorBar.cpp \
Label.cpp \
HandleAxis.cpp \
HandleObject.cpp \
HandleProperty.cpp \
HandleFigure.cpp \
HandleText.cpp \
GLLabel.cpp \
GLRenderEngine.cpp \
HandleLineSeries.cpp \
RenderEngine.cpp \
HandleCommands.cpp \
HandleImage.cpp \
HandleSurface.cpp \
QTRenderEngine.cpp \
HandleWindow.cpp

DEPENDPATH += INCLUDEPATH

QT += opengl
