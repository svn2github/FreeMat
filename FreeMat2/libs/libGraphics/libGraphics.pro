TEMPLATE = lib

TARGET = Graphics

CONFIG += staticlib debug

INCLUDEPATH = ../libFreeMat ../libXP ../.. ../libCore

HEADERS += Axis.hpp \
DataSet2D.hpp \
GraphicsCore.hpp \
Plot2D.hpp \
ScalarImage.hpp \
trackball.h \
Figure.hpp \
Plot3D.hpp \
SurfPlot.hpp \
NewAxis.hpp \
QTDraw.hpp

SOURCES += Axis.cpp \
DataSet2D.cpp \
LoadGraphicsCore.cpp \
Plot2D.cpp \
PlotCommands.cpp \
ImageCommands.cpp \
ScalarImage.cpp \
trackball.c \
Figure.cpp \
Plot3D.cpp \
SurfPlot.cpp \
NewAxis.cpp \
QTDraw.cpp

DEPENDPATH += INCLUDEPATH

win32 {
QT += qt3support
}

mac {
QT += qt3support
}
