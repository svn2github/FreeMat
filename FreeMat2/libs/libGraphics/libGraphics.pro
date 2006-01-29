TEMPLATE = lib

TARGET = Graphics

CONFIG += staticlib opengl warn_off

INCLUDEPATH = ../libFreeMat ../libXP ../libCore

HEADERS += Util.hpp \
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

SOURCES += Util.cpp \
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
