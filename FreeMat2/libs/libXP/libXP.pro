TEMPLATE = lib

TARGET = XP

CONFIG += staticlib

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
XPWidget.hpp \
XPWindow.hpp \
QTGC.hpp \
TermWidget.hpp \
BaseTerminal.hpp \
GUITerminal.hpp \
QTTerm.hpp 

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
XPWidget.cpp \
XPWindow.cpp \
QTGC.cpp \
BaseTerminal.cpp \
GUITerminal.cpp \
QTTerm.cpp 

win32 {
QT += qt3support
HEADERS += qprintengine_ps_p.h qpsprinter.h 
SOURCES += qprintengine_ps.cpp qpsprinter.cpp
}
