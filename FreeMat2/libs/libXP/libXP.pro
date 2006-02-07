TEMPLATE = lib

TARGET = XP

CONFIG += staticlib debug create_prl

INCLUDEPATH = ../libFreeMat ../.. ../libCore ../libFN ../libGraphics

DEFINES -= UNICODE

HEADERS += DynLib.hpp \
System.hpp \
PathSearch.hpp \
KeyManager.hpp \
QTGC.hpp \
TermWidget.hpp \
GUITerminal.hpp \
QTTerm.hpp 

DEPENDPATH += INCLUDEPATH

SOURCES += DynLib.cpp \
TermWidget.cpp \
System.cpp \
PathSearch.cpp \
KeyManager.cpp \
QTGC.cpp \
GUITerminal.cpp \
QTTerm.cpp 
