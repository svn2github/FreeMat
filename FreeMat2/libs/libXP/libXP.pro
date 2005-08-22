TEMPLATE = lib

TARGET = XP

CONFIG += staticlib debug

INCLUDEPATH = ../libFreeMat ../.. ../libCore ../libFN ../libGraphics

DEFINES -= UNICODE

HEADERS += DynLib.hpp \
System.hpp \
PathSearch.hpp \
Rect2D.hpp \
Point2D.hpp \
GraphicsContext.cpp \
Color.hpp \
KeyManager.hpp \
QTGC.hpp \
TermWidget.hpp \
GUITerminal.hpp \
QTTerm.hpp \
DrawEngine.hpp \
QPainterDrawEngine.hpp

DEPENDPATH += INCLUDEPATH

SOURCES += DynLib.cpp \
TermWidget.cpp \
System.cpp \
PathSearch.cpp \
Rect2D.cpp \
Point2D.cpp \
GraphicsContext.cpp \
Color.cpp \
KeyManager.cpp \
QTGC.cpp \
GUITerminal.cpp \
QTTerm.cpp \
QPainterDrawEngine.cpp \
PSDrawEngine.cpp

win32 {
QT += qt3support
SOURCES += qprintengine_ps.cpp qpsprinter.cpp
HEADERS += qprintengine_ps_p.h qpsprinter.h 
}

mac {
QT += qt3support
HEADERS += qprintengine_ps_p.h qpsprinter.h 
SOURCES += qprintengine_ps.cpp qpsprinter.cpp
}
