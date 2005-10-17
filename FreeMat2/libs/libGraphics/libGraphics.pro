TEMPLATE = lib

TARGET = Graphics

CONFIG += staticlib debug opengl

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
HandleFigure.hpp

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
HandleFigure.cpp

DEPENDPATH += INCLUDEPATH

win32 {
QT += qt3support
}

mac {
QT += qt3support
}

QT += opengl
